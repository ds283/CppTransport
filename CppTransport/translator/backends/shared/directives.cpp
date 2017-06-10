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


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, p))
#define BIND_IF_SYMBOL(X, N) std::move(std::make_unique<X>(N, p, istack))


namespace macro_packages
  {

    directives::directives(translator_data& p)
      : directive_package(p)
      {
        simple_package.emplace_back(BIND_IF_SYMBOL(if_directive, "IF"));
        simple_package.emplace_back(BIND_IF_SYMBOL(else_directive, "ELSE"));
        simple_package.emplace_back(BIND_IF_SYMBOL(endif_directive, "ENDIF"));

        index_package.emplace_back(BIND_SYMBOL(set_directive, "SET"));
      }


    std::string set_directive::evaluate(const macro_argument_list& args, const index_literal_list& indices)
      {
        std::string name = args[SET_DIRECTIVE_NAME_ARGUMENT];
        std::string defn = args[SET_DIRECTIVE_DEFINITION_ARGUMENT];

        auto fail_handler = [&](const std::string& msg) -> std::string
          {
            const error_context& ctx = args[SET_DIRECTIVE_DEFINITION_ARGUMENT].get_declaration_point();
            ctx.error(msg);

            std::ostringstream rval;
            rval << ERROR_DIRECTIVE_SET << " '" << name << "': " << msg;
            return rval.str();
          };

        // convert the supplied index_literal_list to an index_literal_database
        // fails if the index_literal_list contains more than a single instance of any index
        // if successful, returns a database view onto the original index_literal_list
        std::unique_ptr<index_literal_database> db;
        try
          {
            db = to_database(indices);
          }
        catch(duplicate_index& xe)
          {
            const error_context& ctx = xe.get_error_point();
            
            std::ostringstream msg;
            msg << ERROR_SET_INDEX_DUPLICATE << " '" << xe.what() << "'";
            ctx.error(msg.str());
            
            return msg.str();
          }

        // get macro agent associated with current top-of-stack output file;
        // this will be our own parent macro agent
        macro_agent& ma = this->payload.get_stack().top_macro_package();

        // tokenize the macro definition provided by the user,
        // asking the tokenizer to validate all supplied indices against
        // the provided index_literal_database.
        // requires strict validation -- indices not in the validation database are rejected
        std::unique_ptr<token_list> tokens = ma.tokenize(defn, *db, true);

        // check whether a replacement rule with the required name already exists
        auto t = this->rules.find(name);
        if(t != this->rules.end())
          {
            const error_context& ctx = args[SET_DIRECTIVE_NAME_ARGUMENT].get_declaration_point();

            std::ostringstream msg;
            msg << ERROR_SET_REDEFINITION << " '" << name << "'";
            ctx.error(msg.str());

            const error_context& prior_ctx = t->second->get_declaration_point();
            prior_ctx.warn(WARN_PRIOR_REDEFINITION);
            
            return msg.str();
          }

        // move references to parent abstract_index objects to those contained in the database held by tokens
        // these will persist (because ownership of tokens will move to the newly-created user_macro
        // record), whereas those currently held by indices will become invalidated when the token_list
        // for the line currently being recognized (ie. the one containing the $SET) is destroyed
        auto new_indices = indices;
        replace_database(new_indices, tokens->get_index_database());

        // construct user-defined macro corresponding to this token list
        std::pair<macro_table::iterator, bool> result = this->rules.emplace(
          std::make_pair(name,
                         std::make_unique<directives_impl::user_macro>(name, std::move(tokens), std::move(new_indices),
                                                                       args[SET_DIRECTIVE_NAME_ARGUMENT].get_declaration_point())));

        // inject this macro into the local definitions for the top-of-stack macro package
        if(result.second)
          {
            ma.inject_macro(*result.first->second);
          }

        std::ostringstream msg;
        msg << DIRECTIVE_SET_MACRO_A << " '" << name << "' " << DIRECTIVE_SET_MACRO_B << " \"" << defn << "\"";
        return msg.str();
      }


    std::string if_directive::evaluate(const macro_argument_list& args)
      {
        std::string condition = args[IF_DIRECTIVE_CONDITION_ARGUMENT];

        bool truth = false;

        macro_agent& ma = this->payload.get_stack().top_macro_package();

        // currently we support only the "fast" condition, so we can bodge the job
        // of evaluating the conditional clause; in general, this would require
        // tokenization, parsing, and the result would be a lot more complex
        if(condition == std::string("fast") && this->payload.fast()) truth = true;
        else if(condition == std::string("!fast") && !this->payload.fast()) truth = true;

        // push a new clause onto the "if" stack, with the determined truth value
        this->istack.emplace(condition, truth);

        // enable or disable output, as appropriate
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
        return msg.str();
      }
    
    
    std::string else_directive::evaluate(const macro_argument_list& args)
      {
        // check for unpaired or duplicate "else" clause
        if(this->istack.size() == 0) throw rule_apply_fail(ERROR_UNPAIRED_ELSE);
        if(!this->istack.top().in_if_branch()) throw rule_apply_fail(ERROR_DUPLICATE_ELSE);
    
        this->istack.top().mark_else_branch();
    
        macro_agent& ma = this->payload.get_stack().top_macro_package();
    
        // enable or disable output, as appropriate
        if(this->istack.top().is_enabled())
          {
            ma.enable_output();
          }
        else
          {
            ma.disable_output();
          }

        std::ostringstream msg;
        msg << "ELSE " << this->istack.top().get_condition();
        return msg.str();
      }
    
    
    std::string endif_directive::evaluate(const macro_argument_list& args)
      {
        // check for an unpaired "endif" clause
        if(this->istack.size() == 0) throw rule_apply_fail(ERROR_UNPAIRED_ENDIF);
    
        macro_agent& ma = this->payload.get_stack().top_macro_package();
    
        // cache condition string from #if clause
        std::string if_condition = this->istack.top().get_condition();
    
        // remove top-of-stack #if clause
        this->istack.pop();
    
        // enable or disable output, as appropriate
        if(this->istack.size() == 0 || this->istack.top().is_enabled())
          {
            ma.enable_output();
          }
        else
          {
            ma.disable_output();
          }

        std::ostringstream msg;
        msg << "ENDIF " << if_condition;

        return msg.str();
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


        std::string user_macro::unroll(const macro_argument_list& args, const index_literal_assignment& idxs)
          {
            // map indices between declaration and assignment
            indices_assignment rule;

            auto declare_t = this->indices.begin();
            auto invoke_t = idxs.begin();

            while(declare_t != this->indices.end() && invoke_t != idxs.end())
              {
                const index_literal& lit = **declare_t;
                const abstract_index& idx = lit;
                
                const index_value& value = invoke_t->second.get();
                
                rule.emplace_back(
                  std::make_pair(idx.get_label(), std::make_shared<index_value>(idx, value.get_numeric_value())));

                ++declare_t;
                ++invoke_t;
              }

            this->tokens->evaluate_macros(rule);
            this->tokens->evaluate_macros(simple_macro_type::post);

            return this->tokens->to_string();
          }


        std::string user_macro::roll(const macro_argument_list& args, const index_literal_list& idxs)
          {
            // map indices between declaration and assignment
            index_remap_rule rule;

            auto declare_t = this->indices.begin();
            auto invoke_t = idxs.begin();

            while(declare_t != this->indices.end() && invoke_t != idxs.end())
              {
                rule.emplace(std::make_pair(std::ref(**declare_t), std::ref(**invoke_t)));

                ++declare_t;
                ++invoke_t;
              }

            this->tokens->evaluate_macros(rule);
            this->tokens->evaluate_macros(simple_macro_type::post);

            return this->tokens->to_string();
          }

      }   // namespace directives_impl

  }   // namespace macro_packages
