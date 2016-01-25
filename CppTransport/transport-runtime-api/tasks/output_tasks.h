//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
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


#include "transport-runtime-api/tasks/task.h"
#include "transport-runtime-api/messages.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

// forward declare derived products if needed
#include "transport-runtime-api/derived-products/derived_product_forward_declare.h"

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


    template <typename number> class output_task;

		template <typename number>
		output_task<number> operator+(const output_task<number>& lhs, const derived_data::derived_product<number>& rhs);


    //! An 'output_task' is a specialization of 'task' which generates a set of derived products.
    template <typename number>
    class output_task: public task<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Construct a named output task using a supplied list of elements

        output_task(const std::string& nm, typename std::vector< output_task_element<number> >& eles)
	        : serial(0), elements(eles), task<number>(nm)
	        {
		        serial = elements.size();
	        }

        //! Construct a named output task using a supplied single derived_product<> object.
        //! Tags provided.
        output_task(const std::string& nm, const derived_data::derived_product<number>& prod, const std::list<std::string>& tags)
	        : serial(0), task<number>(nm)
	        {
            elements.clear();

            elements.push_back(output_task_element<number>(prod, tags, serial++));
	        }

        //! Construct a named output task using a supplied single derived_product<> object.
        //! No tags provided.
        output_task(const std::string& nm, const derived_data::derived_product<number>& prod)
	        : output_task(nm, prod, std::list<std::string>())
	        {
	        }

        //! Deserialization constructor
        output_task(const std::string& nm, Json::Value& reader, derived_product_finder<number>& pfinder);


        //! Destroy an output task
        virtual ~output_task() = default;


		    // OVERLOAD ARITHMETIC OPERATORS FOR CONVENIENCE

      public:

		    //! += operator is the same as add_element()
		    output_task<number>& operator+=(const derived_data::derived_product<number>& prod) { this->add_element(prod); return(*this); }

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
		    void add_element(const derived_data::derived_product<number>& prod) { this->add_element(prod, std::list<std::string>()); }

		    //! Add an element, tags provided
        void add_element(const derived_data::derived_product<number>& prod, const std::list<std::string>& tags);

		    //! Lookup a derived product
		    derived_data::derived_product<number>* lookup_derived_product(const std::string& name);


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

        const std::vector< output_task_element<number> > elements = this->get_elements();

        for(typename std::vector< output_task_element<number> >::const_iterator t = elements.begin(); t != elements.end(); ++t)
          {
            out << *t;
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

        for(Json::Value::iterator t = element_list.begin(); t != element_list.end(); ++t)
          {
            std::string  product_name = (*t)[CPPTRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT].asString();
            unsigned int sn           = (*t)[CPPTRANSPORT_NODE_OUTPUT_SERIAL].asUInt();

            std::list<std::string> tags;
            Json::Value& tag_list = (*t)[CPPTRANSPORT_NODE_OUTPUT_TAGS];

            for(Json::Value::iterator u = tag_list.begin(); u != tag_list.end(); ++u)
              {
		            tags.push_back(u->asString());
              }

            std::unique_ptr< derived_product_record<number> > product_record(pfinder(product_name));
            assert(product_record.get() != nullptr);
            derived_data::derived_product<number>* dp = product_record->get_product();

            // construct a output_task_element<> object wrapping these elements, and push it to the list
            elements.push_back(output_task_element<number>(*dp, tags, sn));
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

        for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); ++t)
	        {
            Json::Value elem(Json::objectValue);

            elem[CPPTRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT] = t->get_product_name();
            elem[CPPTRANSPORT_NODE_OUTPUT_SERIAL]          = t->get_serial();

            const std::list<std::string>& tags = t->get_tags();

            Json::Value tag_list(Json::objectValue);

            for(std::list<std::string>::const_iterator u = tags.begin(); u != tags.end(); ++u)
	            {
                Json::Value tag_element = *u;
                tag_list.append(tag_element);
	            }
		        elem[CPPTRANSPORT_NODE_OUTPUT_TAGS] = tag_list;
		        element_list.append(elem);
	        }
        writer[CPPTRANSPORT_NODE_OUTPUT_ARRAY] = element_list;

        this->task<number>::serialize(writer);
	    }


		template <typename number>
    void output_task<number>::add_element(const derived_data::derived_product<number>& prod, const std::list<std::string>& tags)
	    {
        // check that this derived product has a distinct filename

        for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); ++t)
	        {
            if(t->get_product()->get_filename() == prod.get_filename())
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A << " " << prod.get_filename() << " "
	                << CPPTRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B << " '" << this->name << "'";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	            }

            if(t->get_product_name() == prod.get_name())
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_A << " " << prod.get_name() << " "
	                << CPPTRANSPORT_OUTPUT_TASK_NAME_COLLISION_B << " '" << this->name << "'";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	            }
	        }

        elements.push_back(output_task_element<number>(prod, tags, serial++));
	    }


		template <typename number>
    const output_task_element<number>& output_task<number>::get(unsigned int i) const
	    {
        if(i >= this->elements.size())
	        throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_OUTPUT_TASK_RANGE);

        return(this->elements[i]);
	    }


		template <typename number>
		derived_data::derived_product<number>* output_task<number>::lookup_derived_product(const std::string& name)
			{
		    derived_data::derived_product<number>* rval = nullptr;

				for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); ++t)
					{
						if(t->get_product_name() == name)
							{
								rval = t->get_product();
								break;
							}
					}

				return(rval);
			}


    template <typename number>
    output_task<number> operator+(const output_task<number>& lhs, const derived_data::derived_product<number>& rhs)
	    {
		    return(output_task<number>(lhs) += rhs);
	    }


	}   // namespace transport


#endif //CPPTRANSPORT_OUTPUT_TASKS_H
