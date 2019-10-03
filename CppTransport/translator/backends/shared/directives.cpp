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
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@
//


#include <sstream>

#include "directives.h"
#include "macro.h"
#include "msg_en.h"


#define BIND_SYMBOL(X, N) std::move(std::make_unique<X>(N, p))
#define BIND_IF_SYMBOL(X, N) std::move(std::make_unique<X>(N, p, istack))
#define BIND_FOR_SYMBOL(X, N) std::move(std::make_unique<X>(N, p, istack))
#define EMPLACE(pkg, obj) try { emplace_directive(pkg, obj); } catch(std::exception& xe) { }


namespace macro_packages
  {

    directives::directives(translator_data& p)
      : directive_package(p)
      {
        EMPLACE(simple_package, BIND_IF_SYMBOL(if_directive, "IF"));
        EMPLACE(simple_package, BIND_IF_SYMBOL(else_directive, "ELSE"));
        EMPLACE(simple_package, BIND_IF_SYMBOL(endif_directive, "ENDIF"));

        //! Custom codes to try to implement FOR loops in the CppTransport translator for use with CpptSample
        EMPLACE(for_package,    BIND_FOR_SYMBOL(for_directive, "FOR"));

        EMPLACE(index_package, BIND_SYMBOL(set_directive, "SET"));
      }


    std::string set_directive::apply(const macro_argument_list& args, const index_literal_list& indices)
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


    std::string if_directive::apply(const macro_argument_list& args)
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
    
    
    std::string else_directive::apply(const macro_argument_list& args)
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
    
    
    std::string endif_directive::apply(const macro_argument_list& args)
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

    // Custom command that replaces certain strings provided in the 'from' arg to the 'to' arg as many times as
    // there are appearances in the 'str' arg. We could replace this with a more intelligent replacer using the
    // internal CppT translator in the future, as this would fix the indentation issues automatically.
    bool Replace(std::string& str, const std::string& from, const std::string& to) {
      while(str.find(from) != std::string::npos){
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
          return false;
        str.replace(start_pos, from.length(), to);
      }
      return true;
    }

    // The function that gets executed when the translator detects a FOR loop
    std::string for_directive::apply(const forloop_argument_list& args)
    {
      std::string iterator    = args[FOR_DIRECTIVE_ITERATOR_ARGUMENT]; // This is what we're replacing in the provided string
      std::string replaceme   = args[FOR_DIRECTIVE_REPLACE_ARGUMENT]; // The string that we want to replace certain key-words with
      std::string arg_list    = args[FOR_DIRECTIVE_LIST_ARGUMENT]; // What we want to replace the iterator with
      std::string endl_arg    = args[FOR_DIRECTIVE_ENDL_ARGUMENT]; // Do we end the line after each term in the argument_list?
      std::string no_last_comma_arg = args[FOR_DIRECTIVE_NO_COMMA_ARGUMENT]; // Do we have a trailing comma at the end of the replacement?

      // Turn the string end_arg and no_last_comma_arg into booleans
      bool endl_bool = endl_arg == "True";
      bool no_last_comma_bool = no_last_comma_arg == "True";

      std::vector<std::string> Listy;
      std::ostringstream msg;  // Output string

      if (arg_list == "Fields"){
        Listy = this->payload.model.get_field_name_list();
      } else if (arg_list == "Params"){
        Listy = this->payload.model.get_param_name_list();
      } else if (arg_list == "NumFields"){
        int NumberofFields = this->payload.model.get_field_name_list().size();
        for (int i = 0; i < NumberofFields; ++i){
          std::string Output;
          Output = std::to_string(i);
          Listy.push_back(Output + "_1");
          Listy.push_back(Output + "_2");
          Listy.push_back(Output + "_3");
          Listy.push_back(Output + "_4");
        }
      } else if (arg_list == "FieldNum"){
        int NumberofFields = this->payload.model.get_field_name_list().size();
        for (int i = 0; i < NumberofFields; ++i){
          Listy.push_back(std::to_string(i));
        }
      } else if (arg_list == "Values"){
        symbol_list Field_vals      = this->payload.model.get_field_val();
        symbol_list Field_derivvals = this->payload.model.get_field_derivval();
        symbol_list Param_vals      = this->payload.model.get_param_values();

        if (!((Field_vals.empty()) && (Field_derivvals.empty()) && (Param_vals.empty()) )){
          Listy.emplace_back("[inflation_parameters]");
        }

        for(auto & Field_val : Field_vals)
        {
          Listy.push_back(Field_val.get_name());
        }
        for(auto & Field_derivval : Field_derivvals)
        {
          Listy.push_back(Field_derivval.get_name());
        }
        for(auto & Param_val : Param_vals)
        {
          Listy.push_back(Param_val.get_name());
        }
      } else if (arg_list == "Priors") {
        symbol_list Field_priors      = this->payload.model.get_field_prior();
        symbol_list Field_derivpriors = this->payload.model.get_field_derivprior();
        symbol_list Param_priors      = this->payload.model.get_param_priors();

        if (!((Field_priors.empty()) && (Field_derivpriors.empty()) && (Param_priors.empty()) )) {
          Listy.emplace_back("[inflation_parameters]");
        }

        for (auto &Field_prior : Field_priors) {
          Listy.push_back(Field_prior.get_name());
        }
        for (auto &Field_derivprior : Field_derivpriors) {
          Listy.push_back(Field_derivprior.get_name());
        }
        for (auto &Param_prior : Param_priors) {
          Listy.push_back(Param_prior.get_name());
        }
      } else if (arg_list == "FieldsInit"){
        symbol_list f_list = this->payload.model.get_field_symbols();
        //symbol_list d_list = this->data_payload.model.get_deriv_symbols();
        symbol_list d_list = this->payload.model.get_field_deriv();

        for(int i = 0; i < f_list.size(); ++i)
        {
          Listy.push_back(f_list[i].get_name().append("_Init"));
        }
        for(int i = 0; i < d_list.size(); ++i)
        {
          Listy.push_back(d_list[i].get_name().append("_Init"));
        }
      }
      const int Lengthy = Listy.size();
      int itter = 1;
      for (auto& i : Listy) {
        auto TempReplace = replaceme;
        Replace(TempReplace, iterator, i);
        Replace(TempReplace, "£QUOTE", "\"");
        if(no_last_comma_bool){
          if(itter != Lengthy) {
            Replace(TempReplace, "£COMMA", ",");
          }else{
            Replace(TempReplace, "£COMMA", "");
          }
        } else{
          Replace(TempReplace, "£COMMA", ",");
        }

        if (endl_bool){
          msg << TempReplace << std::endl;
        }
        else{
          msg << TempReplace;
        }
        ++itter;
      }
      return msg.str();
    }


    namespace directives_impl
      {

        void directives_impl::user_macro::pre_hook(const macro_argument_list& args, const index_literal_list& indices)
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
