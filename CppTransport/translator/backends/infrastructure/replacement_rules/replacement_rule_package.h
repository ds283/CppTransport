//
// Created by David Seery on 25/06/2013.
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

#ifndef CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H
#define CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H


#include "ginac/ginac.h"

#include "language_printer.h"
#include "cse.h"
#include "cse_map.h"
#include "lambda_manager.h"
#include "error.h"
#include "replacement_rule_definitions.h"
#include "index_assignment.h"
#include "index_flatten.h"
#include "translator_data.h"

#include "concepts/tensor_factory.h"


// need forward reference to avoid circularity
class u_tensor_factory;


namespace macro_packages
  {

    // abstract replacement_rule_package class
    class replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replacement_rule_package(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        // provide virtual destructor so that derived classes delete correctly
        virtual ~replacement_rule_package() = default;


        // INTERFACE

      public:

        // these methods must be overridden by derived classes which implement the replacement_rule_package concept


        //! return pre- macros package
        const std::vector< std::unique_ptr<replacement_rule_simple> >& get_pre_rules()  { return(this->pre_package); }

        //! return post- macros package
        const std::vector< std::unique_ptr<replacement_rule_simple> >& get_post_rules() { return(this->post_package); }

        //! get index-macro package
        const std::vector< std::unique_ptr<replacement_rule_index> >& get_index_rules() { return(this->index_package); }


        // INTERFACE -- END OF INPUT

      public:

        //! report end of input; default implementation is empty, but can be overridden by
        //! implementation classes if needed
        virtual void report_end_of_input() { return; }


        // INTERNAL DATA

      protected:

        // MACRO PACKAGES

        // held as a container of std::unique_ptr<>, because ownership of these
        // replacement rules is vested in this package.
        // They are shared with clients as raw pointers or references.

        //! package of pre- macros
        std::vector< std::unique_ptr<replacement_rule_simple> > pre_package;

        //! package of post- macros
        std::vector< std::unique_ptr<replacement_rule_simple> > post_package;

        //! package of index-macros
        std::vector< std::unique_ptr<replacement_rule_index> > index_package;


        // REFERENCES TO EXTERNALLY-SUPPLIED AGENTS

        //! data payload from parent translator
        translator_data& data_payload;

        //! language-printer
        language_printer& printer;

        //! tensor factory
        tensor_factory& fctry;

        // CSE worker object
        cse& cse_worker;

        // lambda manager object
        lambda_manager& lambda_mgr;


        // CACHE USEFUL OBJECTS

        //! symbol factory
        symbol_factory& sym_factory;


        // INTERNAL AGENTS

        //! index flattener
        index_flatten fl;

      };

  }


#endif // CPPTRANSPORT_REPLACEMENT_RULE_PACKAGE_H
