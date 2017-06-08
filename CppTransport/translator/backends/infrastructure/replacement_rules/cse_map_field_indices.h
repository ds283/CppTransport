//
// Created by David Seery on 22/12/2015.
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

#ifndef CPPTRANSPORT_CSE_MAP_FIELD_INDICES_H
#define CPPTRANSPORT_CSE_MAP_FIELD_INDICES_H


#include "replacement_rule_definitions.h"
#include "cse.h"
#include "indices.h"
#include "index_flatten.h"


namespace macro_packages
  {

    class cse_map_field1 : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cse_map_field1(std::string nm, unsigned int a, param_index num_params, field_index num_fields)
          : replacement_rule_index(std::move(nm), a, 1, index_class::field_only),
            fl(static_cast<unsigned int>(num_params), static_cast<unsigned int>(num_fields))
          {
          }

        //! destructor
        virtual ~cse_map_field1() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! default post-hook will release CSE map
        void post_hook(const macro_argument_list&) override { this->map.release(); }


        // INTERNAL DATA

      protected:

        //! CSE map object; should be filled in by implementation-defined 'pre' method
        std::unique_ptr<cse_map> map;

        //! index flattener
        index_flatten fl;

      };


    class cse_map_field2 : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cse_map_field2(std::string nm, unsigned int a, param_index num_params, field_index num_fields)
          : replacement_rule_index(std::move(nm), a, 2, index_class::field_only),
            fl(static_cast<unsigned int>(num_params), static_cast<unsigned int>(num_fields))
          {
          }

        //! destructor
        virtual ~cse_map_field2() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! default post-hook will release CSE map
        void post_hook(const macro_argument_list&) override { this->map.release(); }


        // INTERNAL DATA

      protected:

        //! CSE map object; should be filled in by implementation-defined 'pre' method
        std::unique_ptr<cse_map> map;

        //! index flattener
        index_flatten fl;

      };


    class cse_map_field3 : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cse_map_field3(std::string nm, unsigned int a, param_index num_params, field_index num_fields)
          : replacement_rule_index(std::move(nm), a, 3, index_class::field_only),
            fl(static_cast<unsigned int>(num_params), static_cast<unsigned int>(num_fields))
          {
          }

        //! destructor
        virtual ~cse_map_field3() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! default post-hook will release CSE map
        void post_hook(const macro_argument_list&) override { this->map.release(); }


        // INTERNAL DATA

      protected:

        //! CSE map object; should be filled in by implementation-defined 'pre' method
        std::unique_ptr<cse_map> map;

        //! index flattener
        index_flatten fl;

      };

  }   // namespace macro_packages



#endif //CPPTRANSPORT_CSE_MAP_FIELD_INDICES_H
