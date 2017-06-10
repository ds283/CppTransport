//
// Created by David Seery on 06/06/2017.
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


#include "macro_tokens.h"

#include "msg_en.h"


namespace token_list_impl
  {


    constexpr unsigned int MAX_TOKEN_ERRORS = 4;


    generic_token::generic_token(const std::string& c, error_context ec)
      : conversion(c),
        err_ctx(std::move(ec)),
        num_errors(0),
        silent(false)
      {
      }


    void generic_token::error(const std::string& msg)
      {
        if(!this->silent)
          {
            this->err_ctx.error(msg);
          }

        ++this->num_errors;

        if(!this->silent && this->num_errors >= MAX_TOKEN_ERRORS)
          {
            this->err_ctx.warn(ERROR_TOKENIZE_TOO_MANY_ERRORS);
            this->silent = true;
          }
      }


    text_token::text_token(const std::string& l, error_context ec)
      : generic_token(l, std::move(ec))
      {
      }


    index_literal_token::index_literal_token(index_literal l, error_context ec)
      : generic_token(std::string(1, l.get().get_label()), std::move(ec)),
        index(std::move(l))
      {
      }


    void index_literal_token::evaluate(const indices_assignment& a)
      {
        const abstract_index& idx = this->index;
        
        auto t = a.find(idx.get_label());

        if(t == a.end())
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_INDEX_ASSIGNMENT << " '" << idx.get_label() << "'";

            throw macro_packages::rule_apply_fail(msg.str());
          }

        std::ostringstream cnv;
        cnv << t->get_numeric_value();
        this->conversion = cnv.str();
      }


    void index_literal_token::evaluate()
      {
        const abstract_index& idx = this->index;
        this->conversion = idx.get_loop_variable();
      }


    void index_literal_token::evaluate(const index_remap_rule& rule)
      {
        // find substitution for the index we are carrying
        auto t = rule.find(std::ref(this->index));

        if(t == rule.end())
          {
            std::ostringstream msg;
            msg << ERROR_INDEX_SUBSTITUTION << " '" << this->index.get().get_label() << "'";
            throw macro_packages::rule_apply_fail(msg.str());
          }

        this->conversion = t->second.get().get().get_loop_variable();
      }


    simple_macro_token::simple_macro_token(const std::string& m, const macro_argument_list& a,
                                           macro_packages::replacement_rule_simple& r, simple_macro_type t,
                                           error_context ec)
      : generic_token(m, std::move(ec)),
        name(m),
        args(a),
        rule(r),
        type(t),
        argument_error(false)
      {
      }


    void simple_macro_token::evaluate()
      {
        // evaluate the macro, and cache the result
        try
          {
            this->conversion = this->rule(this->args);
          }
        catch(macro_packages::argument_mismatch& xe)
          {
            if(!this->argument_error)
              {
                this->error(xe.what());
                this->argument_error = true;
              }
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }


    index_macro_token::index_macro_token(const std::string& m, const index_literal_list& i,
                                         const macro_argument_list& a, macro_packages::replacement_rule_index& r,
                                         error_context ec)
      : generic_token(m, std::move(ec)),
        name(m),
        args(a),
        indices(std::move(i)),
        rule(r),
        initialized(false),
        argument_error(false),
        index_error(false)
      {
      }


    index_macro_token::~index_macro_token()
      {
        try
          {
            if(this->initialized) this->rule.post(this->args);
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }


    void index_macro_token::evaluate_unroll(const indices_assignment& a)
      {
        // call 'pre'-handler if it has not already been invoked
        this->invoke_pre_handler();

        // strip out the index assignment -- just for the indices this macro requires;
        // we preserve the ordering and bundle the assignments together with the index_literals
        // that define the location of the original declarations
        index_literal_assignment index_values;

        for(const auto& T : this->indices)
          {
            const index_literal& l = *T;
            const abstract_index& idx = l;
            
            auto it = a.find(idx.get_label());
            if(it == a.end())
              {
                std::ostringstream msg;
                msg << ERROR_MISSING_INDEX_ASSIGNMENT << " '" << idx.get_label() << "'";

                throw macro_packages::rule_apply_fail(msg.str());
              }
            
            const index_value& rec = *it;
            
            index_values.emplace_back(std::make_pair(std::cref(l), std::cref(rec)));
          }

        try
          {
            this->conversion = this->rule.evaluate_unroll(this->args, index_values);
          }
        catch(macro_packages::argument_mismatch& xe)
          {
            if(!this->argument_error)
              {
                this->error(xe.what());
                this->argument_error = true;
              }
          }
         catch(macro_packages::index_mismatch& xe)
          {
            if(!this->index_error)
              {
                const error_context& ctx = xe.get_declaration_point();
                ctx.error(xe.what());
                this->index_error = true;
              }
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }


    void index_macro_token::evaluate_roll()
      {
        // as a performance optimization, 'pre' handler is not called for roll-up evaluation;
        // it just results in lots of CSE being performed which is unnecessary for roll-up
        // cases

        try
          {
            this->conversion = this->rule.evaluate_roll(this->args, this->indices);
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }


    void index_macro_token::evaluate_roll(const index_remap_rule& rule)
      {
        // build index set using substitution rules
        index_literal_list remap_list;

        // loop through index_literal_list provided at declaration and map its entries to the corresponding ones
        // provided by the index_remap_rule
        for(auto& T : this->indices)
          {
            index_literal& decl_idx = *T;
            auto t = rule.find(std::ref(decl_idx));

            if(t == rule.end())
              {
                std::ostringstream msg;
                msg << ERROR_INDEX_SUBSTITUTION << " '" << decl_idx.get().get_label() << "'";
                throw macro_packages::rule_apply_fail(msg.str());
              }
            
            index_literal& mapped_idx = t->second.get();
            remap_list.emplace_back(std::make_shared<index_literal>(mapped_idx));
          }

        try
          {
            this->conversion = this->rule.evaluate_roll(this->args, remap_list);
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }


    void index_macro_token::reset()
      {
        try
          {
            if(this->initialized) this->rule.post(this->args);
            this->initialized = false;
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }
    
    
    void index_macro_token::invoke_pre_handler()
      {
        if(!initialized)
          {
            try
              {
                this->rule.pre(args);
              }
            catch(macro_packages::argument_mismatch& xe)
              {
                if(!this->argument_error)
                  {
                    this->error(xe.what());
                    this->argument_error = true;
                  }
              }
            catch(macro_packages::index_mismatch& xe)
              {
                if(!this->index_error)
                  {
                    const error_context& ctx = xe.get_declaration_point();
                    ctx.error(xe.what());
                    this->index_error = true;
                  }
              }
            catch(macro_packages::rule_apply_fail& xe)
              {
                this->error(xe.what());
              }
        
            initialized = true;
          }
      }
    
    
    simple_directive_token::simple_directive_token(const std::string& m, const macro_argument_list& a,
                                                   macro_packages::directive_simple& r, error_context ec)
      : generic_token(m, std::move(ec)),
        name(m),
        args(a),
        rule(r),
        argument_error(false)
      {
        // evaluation occurs on construction in order to guarantee that
        // each directive is evaluated exactly once
        try
          {
            conversion = rule(args);
          }
        catch(macro_packages::argument_mismatch& xe)
          {
            this->error(xe.what());
            argument_error = true;
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }
    
    
    void simple_directive_token::evaluate()
      {
      }
    
    
    index_directive_token::index_directive_token(const std::string& m, const index_literal_list& i,
                                                 const macro_argument_list& a, macro_packages::directive_index& r,
                                                 error_context ec)
      : generic_token(m, std::move(ec)),
        name(m),
        args(a),
        indices(std::move(i)),
        rule(r),
        argument_error(false),
        index_error(false)
      {
        // evaluation occurs on construction in order to guarantee that
        // each directive is evaluated exactly once
        try
          {
            conversion = rule(args, indices);
          }
        catch(macro_packages::argument_mismatch& xe)
          {
            this->error(xe.what());
            argument_error = true;
          }
        catch(macro_packages::index_mismatch& xe)
          {
            const error_context& ctx = xe.get_declaration_point();
            ctx.error(xe.what());
            index_error = true;
          }
        catch(macro_packages::rule_apply_fail& xe)
          {
            this->error(xe.what());
          }
      }
    
    
    void index_directive_token::evaluate()
      {
      }
    
  }   // namespace token_list_impl
