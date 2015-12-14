//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __macropackage_temporary_pool_H_
#define __macropackage_temporary_pool_H_


#include <list>

#include "replacement_rule_package.h"
#include "buffer.h"
#include "core.h"


namespace macro_packages
  {

    class temporary_pool: public replacement_rule_package
      {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    // construct a temporary pool package
		    // by default, it assumes that a literal pool location has not been set (s=false)
		    // this needs to be overridden for kernel buffers, where the literal pool coincides with
		    // the beginning of the buffer
        temporary_pool(u_tensor_factory& uf, flattener& f, cse& cw, translator_data& u, language_printer& prn,
                       std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE);

        //! destructor is default
        ~temporary_pool() = default;


		    // INTERFACE - implements a 'replacement_rule_package' interface

      public:

        const std::vector<simple_rule> get_pre_rules() override;
        const std::vector<simple_rule> get_post_rules() override;
        const std::vector<index_rule>  get_index_rules() override;


		    // INTERFACE -- report end of an input; used to deposit temporaries if necessary

      public:

        //! override default report_end_of_input() implementation to dump temporaries
		    virtual void report_end_of_input() override;


		    // INTERNAL API

      protected:

        void deposit_temporaries();

        std::string replace_temp_pool(const macro_argument_list& args);


		    // INTERNAL DATA

      protected:

        std::string  pool_template;       // template to use when constructing temporaries
        unsigned int unique;              // unique id labelling temporary pools (helpful for matching up while debugging)
        bool         tag_set;             // keep track of whether we have set a tag yet (helpful for debugging)

      };

  } // namespace macro_packages


#endif //__macropackage_temporary_pool_H_
