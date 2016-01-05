//
// Created by David Seery on 05/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#include <sstream>

#include "directives.h"
#include "macro.h"
#include "msg_en.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, p, prn))


namespace macro_packages
  {

    directives::directives(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        index_package.emplace_back(BIND_SYMBOL(set_directive, "SET"));
      }


    std::string set_directive::unroll(const macro_argument_list& args, const assignment_list& indices)
      {
        throw rule_apply_fail(ERROR_DIRECTIVE_CALLED_AS_UNROLL);
      }


    std::string set_directive::roll(const macro_argument_list& args, const abstract_index_list& indices)
      {
        std::string name = args[SET_DIRECTIVE_NAME_ARGUMENT];
        std::string defn = args[SET_DIRECTIVE_DEFINITION_ARGUMENT];

        // get macro agent associated with current top-of-stack output file;
        // this will be our own parent macro agent
        macro_agent& ma = this->payload.get_stack().top_macro_package();

        std::unique_ptr<token_list> tokens = ma.tokenize(defn);

        try
          {
            // check that indices discovered during tokenization match those declared to SET
            this->validate_discovered_indices(indices, tokens->get_indices());
          }
        catch(index_mismatch& xe)
          {
            const error_context& ctx = args[SET_DIRECTIVE_DEFINITION_ARGUMENT].get_declaration_point();
            ctx.error(xe.what());

            std::ostringstream msg;
            msg << ERROR_DIRECTIVE_SET << " '" << name << "': " << xe.what();
            return this->printer.comment(msg.str());
          }

        macro_table::const_iterator t = this->macros.find(name);
        if(t != this->macros.end())
          {
            const error_context& ctx = args[SET_DIRECTIVE_DEFINITION_ARGUMENT].get_declaration_point();

            std::ostringstream msg;
            msg << ERROR_SET_REDEFINITION << " '" << name << "'";
            ctx.error(msg.str());

            const error_context& prior_ctx = t->second->get_declaration_point();

            std::ostringstream prior_msg;
            prior_msg << WARN_PRIOR_REDEFINITION;
            prior_ctx.warn(prior_msg.str());
          }

        // construct user-defined macro corresponding to this token list
        std::pair<macro_table::iterator, bool> result =
          this->macros.emplace(
            std::make_pair(name, std::make_unique<directives_impl::user_macro>(name, std::move(tokens), indices,
                                                                               args[SET_DIRECTIVE_NAME_ARGUMENT].get_declaration_point())));

        // inject this macro into the top-of-stack macro package
        if(result.second)
          {
            ma.inject_macro(result.first->second.get());
          }

        std::ostringstream msg;
        msg << DIRECTIVE_SET_MACRO_A << " '" << name << "' " << DIRECTIVE_SET_MACRO_B << " \"" << defn << "\"";
        return this->printer.comment(msg.str());
      }


    void set_directive::validate_discovered_indices(const abstract_index_list& supplied, const abstract_index_list& discovered)
      {
        if(supplied.size() != discovered.size())
          {
            std::ostringstream msg;
            msg << ERROR_SET_WRONG_NUMBER_INDICES_A << " " << supplied.size() << ", " << ERROR_SET_WRONG_NUMBER_INDICES_B << " " << discovered.size();
            throw index_mismatch(msg.str());
          }

        for(const abstract_index& idx : discovered)
          {
            abstract_index_list::const_iterator t = supplied.find(idx.get_label());

            if(t == supplied.end())
              {
                std::ostringstream msg;
                msg << ERROR_SET_UNDECLARED_INDEX << " '" << idx.get_label() << "'";
                throw index_mismatch(msg.str());
              }
          }
      }


    namespace directives_impl
      {

        void directives_impl::user_macro::pre_hook(const macro_argument_list& args)
          {
            this->tokens->evaluate_macros(simple_macro_type::pre);
          }


        // force call to post-hooks for tokens in our list
        void directives_impl::user_macro::post_hook(const macro_argument_list& args)
          {
            this->tokens->reset();
          }


        std::string user_macro::unroll(const macro_argument_list& args, const assignment_list& idxs)
          {
            // map indices between declaration and assignment
            assignment_list remap_idx;

            abstract_index_list::const_iterator indices_t = this->indices.begin();
            assignment_list::const_iterator idxs_t = idxs.begin();

            while(indices_t != this->indices.end() && idxs_t != idxs.end())
              {
                remap_idx.emplace_back(std::make_pair(indices_t->get_label(), std::make_shared<assignment_record>(*indices_t, idxs_t->get_numeric_value())));

                ++indices_t;
                ++idxs_t;
              }

            this->tokens->evaluate_macros(remap_idx);
            this->tokens->evaluate_macros(simple_macro_type::post);

            return this->tokens->to_string();
          }


        std::string user_macro::roll(const macro_argument_list& args, const abstract_index_list& idxs)
          {
            // map indices between declaration and assignment
            index_remap_rule rule;

            abstract_index_list::const_iterator indices_t = this->indices.begin();
            abstract_index_list::const_iterator idxs_t = idxs.begin();

            while(indices_t != this->indices.end() && idxs_t != idxs.end())
              {
                rule.emplace(std::make_pair(*indices_t, *idxs_t));

                ++indices_t;
                ++idxs_t;
              }

            this->tokens->evaluate_macros(rule);
            this->tokens->evaluate_macros(simple_macro_type::post);

            return this->tokens->to_string();
          }

      }   // namespace directives_impl

  }   // namespace macro_packages
