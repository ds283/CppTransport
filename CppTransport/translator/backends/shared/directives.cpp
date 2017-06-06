//
// Created by David Seery on 05/01/2016.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#include <sstream>

#include "directives.h"
#include "macro.h"
#include "msg_en.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, p, prn))
#define BIND_IF_SYMBOL(X, N) std::move(std::make_unique<X>(N, p, prn, istack))


namespace macro_packages
  {

    directives::directives(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
      : replacement_rule_package(f, cw, lm, p, prn)
      {
        pre_package.emplace_back(BIND_IF_SYMBOL(if_directive, "IF"));
        pre_package.emplace_back(BIND_IF_SYMBOL(else_directive, "ELSE"));
        pre_package.emplace_back(BIND_IF_SYMBOL(endif_directive, "ENDIF"));

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

        auto t = this->macros.find(name);
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
            ma.inject_macro(*result.first->second);
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


    std::string if_directive::evaluate(const macro_argument_list& args)
      {
        std::string condition = args[IF_DIRECTIVE_CONDITION_ARGUMENT];

        bool truth = false;

        macro_agent& ma = this->payload.get_stack().top_macro_package();

        if(condition == std::string("fast") && this->payload.fast()) truth = true;
        else if(condition == std::string("!fast") && !this->payload.fast()) truth = true;

        this->istack.emplace(condition, truth);

        if(this->istack.top().is_enabled())
          {
            ma.enable_output();
          }
        else
          {
            ma.disable_output();
          }

        std::ostringstream msg;
        msg << "IF " << condition;
        return this->printer.comment(msg.str());
      }


    void else_directive::post_tokenize_hook(const macro_argument_list& args)
      {
        if(this->istack.size() == 0) throw rule_apply_fail(ERROR_UNPAIRED_ELSE);
        if(!this->istack.top().in_if_branch()) throw rule_apply_fail(ERROR_DUPLICATE_ELSE);

        this->istack.top().mark_else_branch();

        macro_agent& ma = this->payload.get_stack().top_macro_package();

        if(this->istack.top().is_enabled())
          {
            ma.enable_output();
          }
        else
          {
            ma.disable_output();
          }
      }


    std::string else_directive::evaluate(const macro_argument_list& args)
      {
        std::ostringstream msg;
        msg << "ELSE " << this->istack.top().get_condition();
        return this->printer.comment(msg.str());
      }


    void endif_directive::post_tokenize_hook(const macro_argument_list& args)
      {
        if(this->istack.size() == 0) throw rule_apply_fail(ERROR_UNPAIRED_ENDIF);

        macro_agent& ma = this->payload.get_stack().top_macro_package();

        this->condition_cache = this->istack.top().get_condition();
        this->istack.pop();

        if(this->istack.size() == 0 || this->istack.top().is_enabled())
          {
            ma.enable_output();
          }
        else
          {
            ma.disable_output();
          }
      }


    std::string endif_directive::evaluate(const macro_argument_list& args)
      {
        std::ostringstream msg;
        msg << "ENDIF " << this->condition_cache;

        return this->printer.comment(msg.str());
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
                remap_idx.emplace_back(std::make_pair(indices_t->get_label(),
                                                      std::make_shared<assignment_record>(*indices_t,
                                                                                          idxs_t->get_numeric_value())));

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
