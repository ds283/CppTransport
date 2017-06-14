//
// Created by David Seery on 07/06/2017.
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_DIRECTIVE_PACKAGE_H
#define CPPTRANSPORT_DIRECTIVE_PACKAGE_H


#include "disable_warnings.h"
#include "ginac/ginac.h"

#include "directive_definitions.h"
#include "index_assignment.h"
#include "index_flatten.h"
#include "translator_data.h"


namespace macro_packages
  {
    
    //! abstract directive_package class
    class directive_package
      {
        
        // TYPES
        
      public:
        
        //! package of simple directives
        typedef std::vector< std::unique_ptr<directive_simple> > simple_directive_package;
        
        //! package of index directives
        typedef std::vector< std::unique_ptr<directive_index> > index_directive_package;
        
        
        // CONSTUCTOR, DESTRUCTOR
        
      public:
        
        //! constructor
        directive_package(translator_data& p);
        
        //! destructure is default
        virtual ~directive_package() = default;


        // EMPLACE RULE

      public:

        //! emplace a rule into a rule package
        template <typename RulePackage>
        void emplace_directive(RulePackage& pkg, typename RulePackage::value_type rule);


        // GET DIRECTIVE PACKAGES
        
      public:
        
        //! return simple directives package
        const simple_directive_package& get_simple() { return this->simple_package; }
        
        //! return index directives package
        const index_directive_package& get_index() const { return this->index_package; }
        
        
        // INTERNAL DATA
        
      protected:
        
        // DIRECTIVE PACKAGES
        
        //! package of simple directives
        simple_directive_package simple_package;
        
        //! package of index directives
        index_directive_package index_package;
        
        
        // REFERNCES TO EXTERNALLY-SUPPLIED AGENTS
        
        //! data payload from parent translator
        translator_data& data_payload;
        
      };


    template <typename RulePackage>
    void directive_package::emplace_directive(RulePackage& pkg, typename RulePackage::value_type rule)
      {
        // determine whether this directive is enabled for this model type
        if(rule->enable_if(this->data_payload.model.get_lagrangian_type()))
          {
            pkg.emplace_back(std::move(rule));
          }
      }

  }   // namespace macro_packages;


#endif //CPPTRANSPORT_DIRECTIVE_PACKAGE_H
