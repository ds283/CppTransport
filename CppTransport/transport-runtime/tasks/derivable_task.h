//
// Created by David Seery on 02/04/15.
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


#ifndef CPPTRANSPORT_DERIVABLE_TASK_H
#define CPPTRANSPORT_DERIVABLE_TASK_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>
#include <cfloat>
#include <utility>

#include "transport-runtime/serialization/serializable.h"
#include "transport-runtime/messages.h"

#include "transport-runtime/tasks/task.h"
#include "transport-runtime/tasks/task_configurations.h"
#include "transport-runtime/tasks/configuration-database/time_config_database.h"


namespace transport
	{

    //! A derivable_task is a task which produces output which can be consumed
    //! by another task or an item of derived content
    template <typename number>
    class derivable_task: public task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! construct a derivable task
        derivable_task(const std::string& nm);

        //! deserialization constructor
        derivable_task(const std::string& nm, Json::Value& reader);

        //! destructor
        virtual ~derivable_task() = default;


        // INTERFACE

      public:

        //! Get database of stored time configurations
        virtual const time_config_database& get_stored_time_config_database() const = 0;

        //! Identify task type
        virtual task_type get_type() const = 0;


        // SERIALIZE -- implements a 'serializable' interface

      public:

        //! serialize this object
        virtual void serialize(Json::Value& writer) const override;

	    };


    template <typename number>
    derivable_task<number>::derivable_task(const std::string& nm)
	    : task<number>(nm)
	    {
	    }


    template <typename number>
    derivable_task<number>::derivable_task(const std::string& nm, Json::Value& reader)
	    : task<number>(nm, reader)
	    {
	    }


    template <typename number>
    void derivable_task<number>::serialize(Json::Value& writer) const
	    {
        this->task<number>::serialize(writer);
	    }


    enum class content_group_type
      {
        integration, postintegration
      };


    //! used to specify which properties are needed in a content group for each task
    //! that supplies data to a derived_product<> via a derivable_task<>
    class content_group_specifier
      {

        // We need to capture information about different types of content groups, and while we could do that
        // polymorphically, that woud lead to lots of dynamic_cast<> usage which is messy and slow.
        // Instead we capture everything in a single object, with flags to indicate which kind of object is
        // under discussion. This is also messier than we would like, but we're stuck with it for now

      public:

        // each constructor captures all necessary data

        //! constructor: integration group
        content_group_specifier(bool i, bool s, bool sd)
          : type{content_group_type::integration},
            ics{i},
            statistics{s},
            spectral_data{sd}
          {
          }

        //! constructor: postintegration group
        content_group_specifier(precomputed_products p)
          : type{content_group_type::postintegration},
            products(p),
            ics{false},
            statistics{false},
            spectral_data{false}
          {
          }

        //! destructor
        ~content_group_specifier() = default;


        // INTEGRATION GROUPS

      public:

        //! integration content group flag: requires initial conditions
        bool requires_ics() const;

        //! integration content group flag: requires per-configuration statistics
        bool requires_statistics() const;

        //! integration content group flag: requires spectral data
        bool requires_spectral_data() const;


        // POSTINTEGRATION GROUPS

      public:

        //! postintegration content group: which precomputed products are required?
        const precomputed_products& requires_products() const;


        // INTERNAL DATA

      protected:

        //! type of required content group
        content_group_type type;


        // FLAGS FOR INTEGRATION GROUPS

        //! true if we require a content group with initial conditions data (not currently used)
        bool ics;

        //! true if we require a content group with per-configuraiton statistics
        bool statistics;

        //! true if we require a content group with spectral data
        bool spectral_data;


        // PRECOMPUTED SPECIFIERS FOR POSTINTEGRATION GROUPS

        //! specify which precomputed products are needed
        precomputed_products products;

      };


    bool content_group_specifier::requires_ics() const
      {
        if(this->type != content_group_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return ics;
      }


    bool content_group_specifier::requires_statistics() const
      {
        if(this->type != content_group_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return statistics;
      }


    bool content_group_specifier::requires_spectral_data() const
      {
        if(this->type != content_group_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return spectral_data;
      }


    const precomputed_products& content_group_specifier::requires_products() const
      {
        if(this->type != content_group_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return products;
      }


    template <typename number>
    class derivable_task_list
      {
      public:
        using type = std::list< std::pair< transport::derivable_task<number>*, std::unique_ptr<content_group_specifier> > >;
        using element_type = typename type::value_type;
      };


    namespace derivable_task_list_impl
      {

        template <typename number>
        class NameComparator
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            NameComparator() = default;

            //! destructor
            ~NameComparator() = default;


            // INTERFACE

          public:

            //! compare two derivable_task_list<number>::type elements by name
            bool operator()(const typename derivable_task_list<number>::element_type& A,
                            const typename derivable_task_list<number>::element_type& B)
              {
                const auto& A_tk = A.first;
                const auto& B_tk = B.first;

                if(A_tk == nullptr || B_tk == nullptr) return false;
                return A_tk->get_name() < B_tk->get_name();
              }

          };


        template <typename number>
        class NameEquality
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            NameEquality() = default;

            //! destructor
            ~NameEquality() = default;


            // INTERFACE

          public:

            //! compare two derivable_task_list<number>::type elements by name
            bool operator()(const typename derivable_task_list<number>::element_type& A,
                            const typename derivable_task_list<number>::element_type& B)
              {
                const auto& A_tk = A.first;
                const auto& B_tk = B.first;

                if(A_tk == nullptr || B_tk == nullptr) return false;
                return A_tk->get_name() == B_tk->get_name();
              }

          };

      }    //  namespace derivable_task_list_impl

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVABLE_TASK_H
