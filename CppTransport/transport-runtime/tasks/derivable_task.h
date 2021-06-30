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


    template <typename number>
    bool operator==(const derivable_task_set_element<number>& a, const derivable_task_set_element<number>& b)
      {
        const auto& a_task = a.get_task();
        const auto& a_specifier = a.get_specifier();

        const auto& b_task = b.get_task();
        const auto& b_specifier = b.get_specifier();

        if(a_task.get_name() != b_task.get_name())
          return false;

        return a_specifier == b_specifier;
      }

	}   // namespace transport


#endif //CPPTRANSPORT_DERIVABLE_TASK_H
