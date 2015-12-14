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


#define BIND(X) std::bind(&temporary_pool::X, this, std::placeholders::_1)


namespace macro_packages
  {

    temporary_pool::temporary_pool(u_tensor_factory& uf, flattener& f, cse& cw, translator_data& p, language_printer& prn,
                                   std::string t)
	    : pool_template(t),
        unique(0),
        tag_set(false),
        replacement_rule_package(uf, f, cw, p, prn)
	    {
	    }


    const std::vector<simple_rule> temporary_pool::get_pre_rules()
      {
        std::vector<simple_rule> package;

        const std::vector<replacement_rule_simple> rules =
          { BIND(replace_temp_pool)
          };

        const std::vector<std::string> names =
          { "TEMP_POOL"
          };

        const std::vector<unsigned int> args =
          { 1
          };

        assert(rules.size() == names.size());
        assert(rules.size() == args.size());

        for(int i = 0; i < rules.size(); ++i)
          {
            package.emplace_back(names[i], rules[i], args[i]);
          }

        return(package);
      }


    const std::vector<simple_rule> temporary_pool::get_post_rules()
      {
        std::vector<simple_rule> package;

        return(package);
      }


    const std::vector<index_rule> temporary_pool::get_index_rules()
      {
        std::vector<index_rule> package;

        return(package);
      }


		void temporary_pool::report_end_of_input()
			{
		    if(!this->tag_set)
          {
            error_context err_context(this->data_payload.get_stack(), this->data_payload.get_error_handler(), this->data_payload.get_warning_handler());
            err_context.warn(WARNING_TEMPORARY_NO_TAG_SET);
          }
		    this->deposit_temporaries();
        this->replacement_rule_package::report_end_of_input();
			}


    // *******************************************************************


    void temporary_pool::deposit_temporaries()
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

            // get temporaries which need to be deposited
            std::string temps = this->cse_worker.temporaries(this->pool_template);

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

    std::string temporary_pool::replace_temp_pool(const macro_argument_list& args)
      {
        assert(args.size() == 1);
        std::string t = (args.size() >= 1 ? args[0] : OUTPUT_DEFAULT_POOL_TEMPLATE);

        std::string rval = "";

        // deposit any temporaries generated up to this point the current temporary pool
        //
        // the insertion happens before the element pointed
        // to by data.pool, so there should be no need
        // to update its location

        // get buffer and macro package from the top of the stack
        buffer& buf = this->data_payload.get_stack().top_buffer();

        // flush any existing temporaries to the preceding pool, if one exists
        if(this->tag_set) this->deposit_temporaries();

        // remember new template
        this->pool_template = t;

        // mark current endpoint in the buffer as the new insertion point
        buf.set_tag_to_end();
        this->tag_set = true;

        // temporary pool will be inserted *before* the line corresponding to this macro
        std::ostringstream label;
        label << OUTPUT_TEMPORARY_POOL_END << " (" << OUTPUT_TEMPORARY_POOL_SEQUENCE << "=" << this->unique << ")";
        rval = this->printer.comment(label.str());

        return(rval);
      }

  } // namespace macro_packages
