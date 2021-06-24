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
        explicit derivable_task(std::string nm);

        //! deserialization constructor
        derivable_task(std::string nm, Json::Value& reader);

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
    derivable_task<number>::derivable_task(std::string nm)
	    : task<number>(std::move(nm))
	    {
	    }


    template <typename number>
    derivable_task<number>::derivable_task(std::string nm, Json::Value& reader)
	    : task<number>(std::move(nm), reader)
	    {
	    }


    template <typename number>
    void derivable_task<number>::serialize(Json::Value& writer) const
	    {
        this->task<number>::serialize(writer);
	    }


    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_TYPE        = "type";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_ICS         = "require-ics";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_STATISTICS  = "require-statistics";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_SPECTRAL    = "require-spectral-data";
    constexpr auto CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_PRECOMPUTED = "required-precomputed";


    //! used to specify which properties are needed in a content group for each task
    //! that supplies data to a derived_product<> via a derivable_task<>
    class content_group_specifier: public serializable
      {

        // We need to capture information about different types of content groups, and while we could do that
        // polymorphically, that woud lead to lots of dynamic_cast<> usage which is messy and slow.
        // Instead we capture everything in a single object, with flags to indicate which kind of object is
        // under discussion. This is also messier than we would like, but we're stuck with it for now

      public:

        // each constructor captures all necessary data

        //! constructor: integration group
        content_group_specifier(bool i, bool s, bool sd)
          : type{task_type::integration},
            ics{i},
            statistics{s},
            spectral_data{sd}
          {
          }

        //! constructor: postintegration group
        explicit content_group_specifier(precomputed_products p)
          : type{task_type::postintegration},
            products(p),
            ics{false},
            statistics{false},
            spectral_data{false}
          {
          }

        //! deserialization constructor
        explicit content_group_specifier(const Json::Value& reader);

        //! destructor
        ~content_group_specifier() = default;


        // OPERATIONS

      public:

        //! merge with a second object
        //! the resulting flags should be suitable to satisfy the requirements of both original specifiers
        content_group_specifier& operator|=(const content_group_specifier& obj);


        // QUERY: INTEGRATION GROUPS

      public:

        //! integration content group flag: requires initial conditions
        bool requires_ics() const;

        //! integration content group flag: requires per-configuration statistics
        bool requires_statistics() const;

        //! integration content group flag: requires spectral data
        bool requires_spectral_data() const;


        // QUERY: POSTINTEGRATION GROUPS

      public:

        //! postintegration content group: which precomputed products are required?
        const precomputed_products& requires_products() const;


        // SERIALIZATION -- implements a serializable interface

      public:

        void serialize(Json::Value& writer) const override;


        // INTERNAL DATA

      protected:

        //! type of required content group
        task_type type;


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


    // deserialization constructor
    content_group_specifier::content_group_specifier(const Json::Value& reader)
      : type{task_type_from_string(reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_TYPE].asString())},
        ics{false},
        statistics{false},
        spectral_data{false}
      {
        switch(type)
          {
            case task_type::integration:
              {
                // deserialize data for integration content groups
                ics = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_ICS].asBool();
                statistics = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_STATISTICS].asBool();
                spectral_data = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_SPECTRAL].asBool();
              }

            case task_type::postintegration:
              {
                // deserialize precomputed products information and assign it to our local copy
                const Json::Value& pdata = reader[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_PRECOMPUTED];
                precomputed_products p(pdata);
                products = p;
              }

            case task_type::output:
              {
                throw runtime_exception(exception_type::SERIALIZATION_ERROR,
                                        CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
              }
          }
      }


    // serialize to a JSON document
    void content_group_specifier::serialize(Json::Value& writer) const
      {
        writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_TYPE] = task_type_to_string(this->type);

        switch(this->type)
          {
            case task_type::integration:
              {
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_ICS] = this->ics;
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_STATISTICS] = this->statistics;
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_SPECTRAL] = this->spectral_data;
                break;
              }

            case task_type::postintegration:
              {
                Json::Value p(Json::objectValue);
                this->products.serialize(p);
                writer[CPPTRANSPORT_NODE_CONTENT_GROUP_SPECIFIER_PRECOMPUTED] = p;
                break;
              }

            case task_type::output:
              {
                throw runtime_exception(exception_type::SERIALIZATION_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
              }
          }
      }


    bool content_group_specifier::requires_ics() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return ics;
      }


    bool content_group_specifier::requires_statistics() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return statistics;
      }


    bool content_group_specifier::requires_spectral_data() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return spectral_data;
      }


    const precomputed_products& content_group_specifier::requires_products() const
      {
        if(this->type != task_type::integration)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_OUT_OF_RANGE);

        return products;
      }


    content_group_specifier& content_group_specifier::operator|=(const content_group_specifier& obj)
      {
        if(this->type != obj.type)
          throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_MERGE);

        switch(this->type)
          {
            case task_type::integration:
              {
                this->ics |= obj.ics;
                this->statistics |= obj.statistics;
                this->spectral_data |= obj.spectral_data;
                break;
              }

            case task_type::postintegration:
              {
                this->products |= obj.products;
                break;
              }

            case task_type::output:
              {
                throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_TASK_CONTENT_GROUP_BAD_TASK_TYPE);
              }
          }

        return *this;
      }


    template <typename number>
    class derivable_task_set_element
      {
      public:

        //! constructor
        derivable_task_set_element(std::unique_ptr< derivable_task<number> > tk,
                                    std::unique_ptr< content_group_specifier > sp,
                                    unsigned int tg=0)
          : task(std::move(tk)),
            specifier(std::move(sp)),
            tag(tg)
          {
          }


        // INTERFACE

      public:

        //! obtain task (const only)
        const derivable_task<number>& get_task() const { return *this->task; }

        //! obtain specifier
        content_group_specifier& get_specifier() { return *this->specifier; }

        //! obtain specifier (const version)
        const content_group_specifier& get_specifier() const { return *this->specifier; }

        //! obtain tag
        unsigned int get_tag() const { return this->tag; }


        // INTERNAL DATA:

      private:

        // it's unclear whether we should use std::unique_ptr<> + clone() to manage the derivable_task<> here,
        // or std::shared_ptr<>. We can expect derivable_task_sets (and their elements) to be shared.
        // TODO: We may want to revisit this solution in future.

        //! pointer to derivable_task<> instance
        std::unique_ptr< derivable_task<number> > task;

        //! pointer to content_group_specifier instance
        std::unique_ptr< content_group_specifier > specifier;

        //! tag, used to identify particular tasks (e.g. to specify which task should be attached to a datapipe)
        unsigned int tag;

      };


    template <typename number>
    class derivable_task_set
      {
      public:
        using type = std::map< unsigned int, derivable_task_set_element<number> >;
        using element_type = derivable_task_set_element<number>;
      };


    //! factory function to construct a derivable_task_set<>::element_type from a derivable_task<> and
    //! the flags for a content_group_specifier for an integration task
    template <typename number>
    typename derivable_task_set<number>::type::value_type
    make_derivable_task_set_element(const derivable_task<number>& tk, bool i, bool s, bool sd, unsigned int id=0)
      {
        return {id, std::move(derivable_task_set_element<number>{
          std::unique_ptr<derivable_task<number> >(dynamic_cast< derivable_task<number>* >(tk.clone())),
          std::make_unique<content_group_specifier>(i, s, sd), id})};
      }


    //! factory function to construct a derivable_task_set<>::element_type from a derivable_task<> and
    //! a precomputed_products object forming the flags for a content_group_specifier for an postintegration task
    template <typename number>
    typename derivable_task_set<number>::type::value_type
    make_derivable_task_set_element(const derivable_task<number>& tk, precomputed_products p, unsigned int id=0)
      {
        return {id, std::move(derivable_task_set_element<number>{
          std::unique_ptr<derivable_task<number> >(dynamic_cast< derivable_task<number>* >(tk.clone())),
          std::make_unique<content_group_specifier>(p), id})};
      }


    //! factory function to construct a derivable_task_set<>::element_type from a derivable_task<> and
    //! an existing content_group_specifier
    template <typename number>
    typename derivable_task_set<number>::type::value_type
    make_derivable_task_set_element(const derivable_task<number>& tk, const content_group_specifier& sp, unsigned int id=0)
      {
        return {id, std::move(derivable_task_set_element<number>{
          std::unique_ptr<derivable_task<number> >(dynamic_cast< derivable_task<number>* >(tk.clone())),
          std::make_unique<content_group_specifier>(sp), id})};
      }


    namespace derivable_task_set_impl
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

            //! compare two derivable_task_set<number>::type elements by name
            bool operator()(const typename derivable_task_set<number>::type::value_type & A,
                            const typename derivable_task_set<number>::type::value_type & B)
              {
                const auto& A_tk = A.second.get_task();
                const auto& B_tk = B.second.get_task();

                // compares true if A.name < B.name
                return A_tk.get_name() < B_tk.get_name();
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

            //! compare two derivable_task_set<number>::type elements by name
            bool operator()(const typename derivable_task_set<number>::type::value_type& A,
                            const typename derivable_task_set<number>::type::value_type & B)
              {
                const auto& A_tk = A.second.get_task();
                const auto& B_tk = B.second.get_task();

                // compares true if A.name == B.name
                return A_tk.get_name() == B_tk.get_name();
              }

          };


        template <typename number>
        class FindByName
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor captures the name we are searching for
            FindByName(std::string n)
              : name{std::move(n)}
              {
              }

            //! destructor is default
            ~FindByName() = default;


            // INTERFACE

          public:

            //! test for match
            bool operator()(const typename derivable_task_set<number>::type::value_type & elt)
              {
                const auto& tk = elt.second.get_task();

                return tk.get_name() == this->name;
              }


            // INTERNAL DATA

          private:

            //! name to test
            std::string name;

          };

      }    //  namespace derivable_task_set_impl

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVABLE_TASK_H
