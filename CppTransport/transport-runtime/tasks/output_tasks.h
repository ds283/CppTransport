//
// Created by David Seery on 22/12/2013.
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


#ifndef CPPTRANSPORT_OUTPUT_TASKS_H
#define CPPTRANSPORT_OUTPUT_TASKS_H


#include <assert.h>
#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>
#include <functional>


#include "transport-runtime/tasks/task.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/defaults.h"

#include "transport-runtime/utilities/random_string.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

// forward declare derived products if needed
#include "transport-runtime/derived-products/derived_product_forward_declare.h"

#include "boost/optional.hpp"
#include "boost/log/utility/formatting_ostream.hpp"



#define CPPTRANSPORT_NODE_OUTPUT_ARRAY           "derived-data-tasks"
#define CPPTRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT "label"
#define CPPTRANSPORT_NODE_OUTPUT_SERIAL          "serial"
#define CPPTRANSPORT_NODE_OUTPUT_TAGS            "tags"


namespace transport
  {

    // TASK STRUCTURES -- OUTPUT TASK

    // An output task is simply a list of derived products to be generated.
		// We can optionally enforce a set of metadata tags for the whole set of
		// content-providers which compose the product.


    template <typename number=default_number_type> class output_task;

		template <typename number>
		output_task<number> operator+(const output_task<number>& lhs, const derived_data::derived_product<number>& rhs);

    template <typename number>
    output_task<number> operator+(const derived_data::derived_product<number>& lhs, const derived_data::derived_product<number>& rhs);


    //! An 'output_task' is a specialization of 'task' which generates a set of derived products.
    template <typename number>
    class output_task: public task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct empty named output task
        output_task(const std::string& nm)
          : task<number>(nm),
            serial(0)
          {
          }

        //! Construct a named output task using a supplied single derived_product<> object and a set of tags
        output_task(const std::string& nm, const derived_data::derived_product<number>& prod, const tag_list& tags)
	        : task<number>(nm),
            serial(0)
	        {
            elements.clear();
            elements.emplace_back(prod, tags, serial++);
	        }

        //! Construct a named output task using a supplied single derived_product<> object.
        //! No tags provided.
        output_task(const std::string& nm, const derived_data::derived_product<number>& prod)
	        : output_task(nm, prod, std::list<std::string>())
	        {
	        }

        //! Construct an unnamed output task using two supplied derived_product<> objects, no tags provided
        output_task(const derived_data::derived_product<number>& p1, const derived_data::derived_product<number>& p2)
          : output_task<number>(random_string(), p1)
          {
            this->add_element(p2);

            // mark as unserializable
            this->task<number>::serializable = false;
          }

        //! Deserialization constructor
        output_task(const std::string& nm, Json::Value& reader, derived_product_finder<number>& pfinder);

        //! Destructor is default
        virtual ~output_task() = default;


        // INTERFACE

      public:

        //! override set_name from task<> class to convert to serializable status
        void set_name(std::string s) { this->task<number>::set_name(std::move(s)); this->serializable = true; }


		    // OVERLOAD ARITHMETIC OPERATORS FOR CONVENIENCE

      public:

		    //! += operator for a derived product is the same as add_element()
		    output_task<number>& operator+=(const derived_data::derived_product<number>& prod) { this->add_element(prod); return(*this); }

        //! += operator for another output task adds all its elemnets
        output_task<number>& operator+=(const output_task<number>& tk);

		    //! + operator
		    friend output_task<number> operator+ <>(const output_task<number>& lhs, const derived_data::derived_product<number>& rhs);


        // INTERFACE

      public:

        //! Obtain number of task elements
        unsigned int size() const { return(this->elements.size()); }

		    //! Obtain task elements
		    const typename std::vector< output_task_element<number> >& get_elements() const { return(this->elements); }

        //! Obtain a specific element
        const output_task_element<number>& get(unsigned int i) const;

		    //! Add an element, no tags provided
		    output_task<number>& add_element(const derived_data::derived_product<number>& prod) { this->add_element(prod, std::list<std::string>()); return *this; }

		    //! Add an element, tags provided
        output_task<number>& add_element(const derived_data::derived_product<number>& prod, const tag_list& tags);

		    //! Lookup a derived product
		    boost::optional< derived_data::derived_product<number>& > lookup_derived_product(const std::string& name);


        // SERIALIZATION (implements a 'serializable' interface)

      public:

        //! Serialize this task to the repository
        virtual void serialize(Json::Value& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual output_task<number>* clone() const override { return new output_task<number>(static_cast<const output_task<number>&>(*this)); }


        // WRITE SELF TO STREAM

      public:

        //! write self-details
        template <typename Stream> void write(Stream& out) const;


        // INTERNAL DATA

      protected:

        //! List of output elements which make up this task
        typename std::vector< output_task_element<number> > elements;

		    //! Serial number of added tasks
		    unsigned int serial;

	    };


    template <typename number>
    template <typename Stream>
    void output_task<number>::write(Stream& out) const
      {
        out << CPPTRANSPORT_OUTPUT_ELEMENTS << '\n';

        for(const output_task_element<number>& ele : this->get_elements())
          {
            out << ele;
          }
      }


    template <typename number, typename Char, typename Traits>
    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const output_task<number>& obj)
	    {
        obj.write(out);
        return(out);
	    }


    template <typename number, typename Char, typename Traits, typename Allocator>
    boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, const output_task<number>& obj)
      {
        obj.write(out);
        return(out);
      }


    template <typename number>
    output_task<number>::output_task(const std::string& nm, Json::Value& reader, derived_product_finder<number>& pfinder)
      : task<number>(nm, reader),
        serial(0)
      {
        // deserialize array of task elements
        Json::Value& element_list = reader[CPPTRANSPORT_NODE_OUTPUT_ARRAY];
		    assert(element_list.isArray());

        for(Json::Value& value : element_list)
          {
            std::string  product_name = value[CPPTRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT].asString();
            unsigned int sn           = value[CPPTRANSPORT_NODE_OUTPUT_SERIAL].asUInt();

            std::list<std::string> tags;
            Json::Value& tag_list = value[CPPTRANSPORT_NODE_OUTPUT_TAGS];

            for(Json::Value& tag : tag_list)
              {
		            tags.push_back(tag.asString());
              }

            std::unique_ptr< derived_product_record<number> > product_record = pfinder(product_name);
            assert(product_record.get() != nullptr);
            derived_data::derived_product<number>* dp = product_record->get_product();

            // construct a output_task_element<> object wrapping these elements, and push it to the list
            elements.emplace_back(*dp, tags, sn);
            if(sn > serial) serial = sn;
          }
      }


    // serialize an output task to the repository
    template <typename number>
    void output_task<number>::serialize(Json::Value& writer) const
	    {
        writer[CPPTRANSPORT_NODE_TASK_TYPE] = std::string(CPPTRANSPORT_NODE_TASK_TYPE_OUTPUT);

        // serialize array of task elements
        Json::Value element_list(Json::arrayValue);

        for(const output_task_element<number>& ele : this->elements)
	        {
            Json::Value record(Json::objectValue);

            record[CPPTRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT] = ele.get_product_name();
            record[CPPTRANSPORT_NODE_OUTPUT_SERIAL]          = ele.get_serial();

            Json::Value tag_list(Json::objectValue);

            for(const std::string& tag : ele.get_tags())
	            {
                Json::Value tag_element = tag;
                tag_list.append(tag_element);
	            }
		        record[CPPTRANSPORT_NODE_OUTPUT_TAGS] = tag_list;
		        element_list.append(record);
	        }
        writer[CPPTRANSPORT_NODE_OUTPUT_ARRAY] = element_list;

        this->task<number>::serialize(writer);
	    }


		template <typename number>
    output_task<number>& output_task<number>::add_element(const derived_data::derived_product<number>& prod, const tag_list& tags)
	    {
        // check that this derived product has a distinct filename

        for(const output_task_element<number>& ele : this->elements)
	        {
            if(ele.get_product().get_filename() == prod.get_filename())
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A << " " << prod.get_filename() << " "
	                << CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B << " '" << this->name << "'";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	            }

            if(ele.get_product_name() == prod.get_name())
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_A << " " << prod.get_name() << " "
	                << CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_B << " '" << this->name << "'";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	            }
	        }

        this->elements.emplace_back(prod, tags, serial++);

        return *this;
	    }


		template <typename number>
    const output_task_element<number>& output_task<number>::get(unsigned int i) const
	    {
        if(i >= this->elements.size())
	        throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_OUTPUT_TASK_RANGE);

        return(this->elements[i]);
	    }


		template <typename number>
		boost::optional< typename derived_data::derived_product<number>& > output_task<number>::lookup_derived_product(const std::string& name)
			{
        boost::optional< derived_data::derived_product<number>& > rval;

        for(const output_task_element<number>& ele : this->elements)
					{
						if(ele.get_product_name() == name)
							{
								rval = ele.get_product();
								break;
							}
					}

				return(rval);
			}


    template <typename number>
    output_task<number>& output_task<number>::operator+=(const output_task<number>& tk)
      {
        for(const output_task_element<number>& element : tk.elements)
          {
            this->add_element(element.get_product(), element.get_tags());
          }

        return *this;
      }


    template <typename number>
    output_task<number> operator+(const output_task<number>& lhs, const derived_data::derived_product<number>& rhs)
	    {
		    return(output_task<number>(lhs) += rhs);
	    }


    template <typename number>
    output_task<number> operator+(const derived_data::derived_product<number>& lhs, const derived_data::derived_product<number>& rhs)
      {
        return output_task<number>(lhs, rhs);
      }


	}   // namespace transport


#endif //CPPTRANSPORT_OUTPUT_TASKS_H
