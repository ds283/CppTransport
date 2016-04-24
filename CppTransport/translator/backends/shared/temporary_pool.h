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



#ifndef CPPTRANSPORT_MACROS_TEMPORARY_POOL_H
#define CPPTRANSPORT_MACROS_TEMPORARY_POOL_H


#include <list>

#include "replacement_rule_package.h"
#include "buffer.h"
#include "core.h"


namespace macro_packages
  {

    constexpr unsigned int TEMP_POOL_TEMPLATE_ARGUMENT = 0;
    constexpr unsigned int TEMP_POOL_TOTAL_ARGUMENTS = 1;


    class replace_temp_pool : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_temp_pool(std::string n, translator_data& p, cse& cw, lambda_manager& lm, language_printer& prn,
                          std::string t = OUTPUT_DEFAULT_POOL_TEMPLATE)
          : replacement_rule_simple(std::move(n), TEMP_POOL_TOTAL_ARGUMENTS),
            data_payload(p),
            cse_worker(cw),
            lambda_mgr(lm),
            printer(prn),
            templ(t),
            unique(0),
            tag_set(false)
          {
          }

        //! destructor
        virtual ~replace_temp_pool() = default;


        // INTERFACE

      public:

        //! over-ride default end-of-input handler
        virtual void report_end_of_input() override;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;

        //! deposit temporary pool
        void deposit_temporaries();


        // INTERNAL DATA

      private:

        //! data payload
        translator_data& data_payload;

        //! CSE worker
        cse& cse_worker;

        //! lambda manager
        lambda_manager& lambda_mgr;

        //! language printer
        language_printer& printer;

        //! template
        std::string templ;

        //! has a tag been set in the buffer?
        bool tag_set;

        //! unique identifier for each temporary pool
        unsigned int unique;

      };


    class temporary_pool: public replacement_rule_package
      {

	      // CONSTRUCTOR, DESTRUCTOR

      public:

		    // construct a temporary pool package
		    // by default, it assumes that a literal pool location has not been set (s=false)
		    // this needs to be overridden for kernel buffers, where the literal pool coincides with
		    // the beginning of the buffer
        temporary_pool(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& u, language_printer& prn);

        //! destructor is default
        ~temporary_pool() = default;


		    // INTERFACE -- report end of an input; used to deposit temporaries if necessary

      public:

        //! override default report_end_of_input() implementation to dump temporaries
		    virtual void report_end_of_input() override;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROS_TEMPORARY_POOL_H
