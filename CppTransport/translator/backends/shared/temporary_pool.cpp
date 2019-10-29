//
// Created by David Seery on 04/12/2013.
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


#include <assert.h>
#include <string>
#include <functional>

#include "temporary_pool.h"
#include "translation_unit.h"
#include "macro.h"
#include "msg_en.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, p, cw, lm, prn))
#define EMPLACE(pkg, obj) try { emplace_rule(pkg, obj); } catch(std::exception& xe) { }


namespace macro_packages
  {

    temporary_pool::temporary_pool(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn)
	    : replacement_rule_package(f, cw, lm, p, prn)
	    {
        EMPLACE(pre_package, BIND(replace_temp_pool, "TEMP_POOL"));
	    }


		void temporary_pool::report_end_of_input()
			{
        for(std::unique_ptr<replacement_rule_simple>& rule : this->pre_package)
          {
            rule->report_end_of_input();
          }

        for(std::unique_ptr<replacement_rule_simple>& rule : this->post_package)
          {
            rule->report_end_of_input();
          }

        // pass notification up to parent handler
        this->replacement_rule_package::report_end_of_input();
			}


    void replace_temp_pool::report_end_of_input()
      {
        // nothing to do if there are no temporaries to write
        auto number = this->cse_worker.number_temporaries() + this->lambda_mgr.number_temporaries();
        if(number == 0) return;

        if(!this->tag_set)
          {
            // TODO: Fix this error being triggered here for the sampling template file
            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.warn(WARNING_TEMPORARY_NO_TAG_SET);
          }
        else
          {
            this->deposit_temporaries();
          }
      }


    void replace_temp_pool::deposit_temporaries()
      {
        // deposit any temporaries generated in the current temporary pool,
        // and then reset the CSE agent

        // check if there is any work to do; if not, return
        auto number = this->cse_worker.number_temporaries() + this->lambda_mgr.number_temporaries();
        if(number == 0) return;

        // however, we should only do something if the location of a temporary pool has been defined
        // (we could get called before that has happened, eg., due to insertion of a kernel)
        // if there is no location defined, we should hold on until one is set later
        if(!tag_set) return;

        // get buffer and macro agent associated with current top-of-stack output file;
        // this will be our own parent macro agent
        buffer& buf = this->data_payload.get_stack().top_buffer();
        macro_agent& ma = this->data_payload.get_stack().top_macro_package();

        bool ok = true;

        size_t lhs_pos;
        size_t rhs_pos;

        if((lhs_pos = this->templ.find("$1")) == std::string::npos)
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_LHS << " '" << this->templ << "'";

            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.error(msg.str());
            ok = false;
          }
        if((rhs_pos = this->templ.find("$2")) == std::string::npos)
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_RHS << " '" << this->templ << "'";

            error_context err_ctx = this->data_payload.make_error_context();
            err_ctx.error(msg.str());
            ok = false;
          }

        if(!ok) return;

        std::string left = this->templ.substr(0, lhs_pos);
        std::string mid = this->templ.substr(lhs_pos + 2, rhs_pos - lhs_pos - 2);
        std::string right = this->templ.substr(rhs_pos + 2, std::string::npos);

        // get temporaries which need to be deposited from CSE manager
        std::unique_ptr<std::list<std::string> > cse_temps = this->cse_worker.temporaries(left, mid, right);

        // get temporaries which need to be deposited from lambda manager
        std::unique_ptr<std::list<std::string> > lambda_temps = this->lambda_mgr.temporaries(left, mid, right);

        // apply macro replacement to output from temporaries, in case this is required
        if(cse_temps->size() > 0 || lambda_temps->size() > 0)
          {
            std::ostringstream label;
            label << OUTPUT_TEMPORARY_POOL_START << " (" << OUTPUT_TEMPORARY_POOL_SEQUENCE << "=" <<
                  this->unique++ << ")";
            buf.write_to_tag(this->printer.comment(label.str()));
          }

        for(const std::string& temp : *cse_temps)
          {
            unsigned int replacements = 0;
            std::unique_ptr<std::list<std::string> > r_list = ma.apply(temp, replacements);

            if(r_list)
              {
                for(const std::string& l : *r_list)
                  {
                    if(l != "") buf.write_to_tag(l);
                  }
              }
          }

        for(const std::string& temp : *lambda_temps)
          {
            unsigned int replacements = 0;
            std::unique_ptr<std::list<std::string> > r_list = ma.apply(temp, replacements);

            if(r_list)
              {
                for(const std::string& l : *r_list)
                  {
                    if(l != "") buf.write_to_tag(l);
                  }
              }
          }

        // clear worker objects; if we don't we might duplicate temporaries we've already written out
        this->cse_worker.clear();
        this->lambda_mgr.clear();
      }

    std::string replace_temp_pool::evaluate(const macro_argument_list& args)
      {
        std::string t = args[TEMP_POOL_TEMPLATE_ARGUMENT];

        // deposit any temporaries generated up to this point into the current temporary pool
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location

        // get buffer and macro package from the top of the stack
        buffer& buf = this->data_payload.get_stack().top_buffer();

        // flush any existing temporaries to the preceding pool, if one exists
        if(this->tag_set) this->deposit_temporaries();

        // remember new template
        this->templ = t;

        // mark current endpoint in the buffer as the new insertion point
        buf.set_tag_to_end();
        this->tag_set = true;

        // temporary pool will be inserted *before* the line corresponding to this macro
        std::ostringstream label;
        label << OUTPUT_TEMPORARY_POOL_END << " (" << OUTPUT_TEMPORARY_POOL_SEQUENCE << "=" << this->unique << ")";
        std::string rval = this->printer.comment(label.str());

        return(rval);
      }

  } // namespace macro_packages
