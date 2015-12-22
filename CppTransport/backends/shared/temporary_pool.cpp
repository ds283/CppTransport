//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#include <assert.h>
#include <string>
#include <functional>

#include "temporary_pool.h"
#include "translation_unit.h"
#include "macro.h"
#include "msg_en.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, this->data_payload, this->cse_worker, this->printer))


namespace macro_packages
  {

    temporary_pool::temporary_pool(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn)
	    : replacement_rule_package(f, cw, p, prn)
	    {
        pre_package.emplace_back(BIND(replace_temp_pool, "TEMP_POOL"));
	    }


    const std::vector<index_rule> temporary_pool::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
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
        if(!this->tag_set)
          {
            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.warn(WARNING_TEMPORARY_NO_TAG_SET);
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

        // however, we should only do something if the location of a temporary pool has been defined
        // (we could get called before that has happened, eg., due to insertion of a kernel)
        // if there is no location defined, we should hold on until one is set later
        if(tag_set)
          {
            // get buffer and macro package from the top of the stack
            buffer&      buf = this->data_payload.get_stack().top_buffer();
            macro_agent& ms  = this->data_payload.get_stack().top_macro_package();

            // get any temporaries which need to be deposited
            std::string temps = this->cse_worker.temporaries(this->templ);

            // apply macro replacement to them, in case this is required
            unsigned int replacements;
            std::unique_ptr< std::list<std::string> > r_list = ms.apply(temps, replacements);

            if(r_list)
              {
                // write to current tagged position, but don't move it - we might need to write again later
                std::ostringstream label;
                label << OUTPUT_TEMPORARY_POOL_START << " (" << OUTPUT_TEMPORARY_POOL_SEQUENCE << "=" << this->unique++ << ")";
                buf.write_to_tag(this->printer.comment(label.str()));

                for(const std::string& l : *r_list)
                  {
                    if(temps != "") buf.write_to_tag(l);
                  }
              }

            // clear worker object; if we don't we might duplicate temporaries we've already written out
            this->cse_worker.clear();
          }
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
