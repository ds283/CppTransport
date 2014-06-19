//
// Created by David Seery on 22/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __output_tasks_H_
#define __output_tasks_H_


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
#include "transport-runtime-api/derived-products/derived_product.h"
#include "transport-runtime-api/messages.h"



#define __CPP_TRANSPORT_NODE_OUTPUT_ARRAY           "derived-data-tasks"
#define __CPP_TRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT "label"
#define __CPP_TRANSPORT_NODE_OUTPUT_SERIAL          "serial"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS       "tags"
#define __CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG        "tag"


namespace transport
  {

    // TASK STRUCTURES -- OUTPUT TASK

    // An output task is simply a list of derived products to be generated.
		// We can optionally enforce a set of metadata tags for the whole set of
		// content-providers which compose the product.

		template <typename number> class output_task_element;

		template <typename number>
		std::ostream& operator<<(std::ostream& out, const output_task_element<number>& obj);

    template <typename number>
    class output_task_element
	    {

      public:

        // CONSTRUCTOR, DESTRUCTOR

        //! Construct an output task element: requires a derived product and a list of tags to match.
        output_task_element(const derived_data::derived_product<number>& dp, const std::list<std::string>& tgs, unsigned int sn)
	        : product(dp.clone()), tags(tgs), serial(sn)
	        {
		        assert(product != nullptr);
	        }

		    //! Override the default copy constructor to perform a deep copy of the stored derived_product<>
		    output_task_element(const output_task_element<number>& obj)
		      : product(obj.product->clone()), tags(obj.tags), serial(obj.serial)
			    {
			    }

        //! Destroy an output task element
        ~output_task_element()
	        {
		        assert(this->product != nullptr);

            delete this->product;
	        }


        // INTERFACE - EXTRACT DETAILS

        //! Get name of derived product associated with this task element
        const std::string& get_product_name() const { return(this->product->get_name()); }

		    //! Get derived product associated with this task element
		    derived_data::derived_product<number>* get_product() const { return(this->product); }

        //! Get tags associated with this task element
        const std::list<std::string>& get_tags() const { return(this->tags); }

		    //! Get serial number of this element
		    unsigned int get_serial() const { return(this->serial); }


		    //! Write to a standard output stream
		    friend std::ostream& operator<< <>(std::ostream& out, const output_task_element<number>& obj);

        // INTERNAL DATA

      protected:

        //! Pointer to derived data product (part of the task description, specifying which eg. plot to produce) which which this output is associated
        derived_data::derived_product<number>* product;

        //! Optional list of tags to enforce for each content provider in the product
        std::list<std::string> tags;

		    //! Internal serial number
		    unsigned int serial;
	    };


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task_element<number>& obj)
	    {
        out << "  " << __CPP_TRANSPORT_OUTPUT_ELEMENT_OUTPUT << " " << obj.get_product_name() << ",";
        out << " " << __CPP_TRANSPORT_OUTPUT_ELEMENT_TAGS  << ": ";

        unsigned int count = 0;
        const std::list<std::string>& tags = obj.get_tags();
        for (std::list<std::string>::const_iterator u = tags.begin(); u != tags.end(); u++)
	        {
            if(count > 0) out << ", ";
            out << *u;
	        }
		    out << std::endl;

        return(out);
	    }


    template <typename number> class output_task;

    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task<number>& obj);

    //! An 'output_task' is a specialization of 'task' which generates a set of derived products.
    template <typename number>
    class output_task: public task<number>
	    {

      public:

        typedef typename std::function< transport::derived_data::derived_product<number>*(const std::string&) > derived_product_finder;


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
        output_task(const std::string& nm, serialization_reader* reader, derived_product_finder& pfinder);


        //! Destroy an output task
        virtual ~output_task() = default;

        //! Write to a standard output stream
        friend std::ostream& operator<< <>(std::ostream& out, const output_task<number>& obj);


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
        virtual void serialize(serialization_writer& writer) const override;


        // CLONE

      public:

        //! Virtual copy
        virtual task<number>* clone() const override { return new output_task<number>(static_cast<const output_task<number>&>(*this)); }


        // INTERNAL DATA

      protected:

        //! List of output elements which make up this task
        typename std::vector< output_task_element<number> > elements;

		    //! Serial number of added tasks
		    unsigned int serial;

	    };


    template <typename number>
    std::ostream& operator<<(std::ostream& out, const output_task<number>& obj)
	    {
        out << __CPP_TRANSPORT_OUTPUT_ELEMENTS << std::endl;
        for(typename std::vector< output_task_element<number> >::const_iterator t = obj.elements.begin(); t != obj.elements.end(); t++)
	        {
            out << *t;
	        }

        return(out);
	    }


    template <typename number>
    output_task<number>::output_task(const std::string& nm, serialization_reader* reader,
                                     typename output_task<number>::derived_product_finder& pfinder)
      : task<number>(nm, reader),
        serial(0)
      {
        assert(reader != nullptr);
        if(reader == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_TASK_NULL_SERIALIZATION_READER);

        // deserialize array of task elements
        unsigned int num_elements = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUT_ARRAY);

        for(unsigned int i = 0; i < num_elements; i++)
          {
            reader->start_array_element();

            std::string product_name;
            reader->read_value(__CPP_TRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT, product_name);

            unsigned int sn;
            reader->read_value(__CPP_TRANSPORT_NODE_OUTPUT_SERIAL, sn);

            std::list<std::string> tags;
            unsigned int num_tags = reader->start_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);
            for(unsigned int j = 0; j < num_tags; j++)
              {
                reader->start_array_element();
                std::string tag;
                reader->read_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG, tag);
                tags.push_back(tag);
                reader->end_array_element();
              }
            reader->end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);

            reader->end_array_element();

            derived_data::derived_product<number>* dp = pfinder(product_name);

            // construct a output_task_element<> object wrapping these elements, and push it to the list
            elements.push_back(output_task_element<number>(*dp, tags, sn));
            if(sn > serial) serial = sn;

            // the repository-supplied objects (via pfinder) can now be deleted; output_task_element performs a deep copy,
            // so there is no risk of dangling pointers or references
            delete dp;
          }

        reader->end_element(__CPP_TRANSPORT_NODE_OUTPUT_ARRAY);
      }


    // serialize an output task to the repository
    template <typename number>
    void output_task<number>::serialize(serialization_writer& writer) const
	    {
        writer.write_value(__CPP_TRANSPORT_NODE_TASK_TYPE, std::string(__CPP_TRANSPORT_NODE_TASK_TYPE_OUTPUT));

        // serialize array of task elements
        writer.start_array(__CPP_TRANSPORT_NODE_OUTPUT_ARRAY, this->elements.size() == 0);    // node name is actually ignored for an array
        for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); t++)
	        {
            writer.start_node("arrayelt", false);    // node name is ignored for arrays

            writer.write_value(__CPP_TRANSPORT_NODE_OUTPUT_DERIVED_PRODUCT, (*t).get_product_name());
		        writer.write_value(__CPP_TRANSPORT_NODE_OUTPUT_SERIAL, (*t).get_serial());

            const std::list<std::string>& tags = (*t).get_tags();

            writer.start_array(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS, tags.size() == 0);      // node name is ignored for an array ...
            for(std::list<std::string>::const_iterator u = tags.begin(); u != tags.end(); u++)
	            {
                writer.write_value(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAG, *u);
	            }
            writer.end_element(__CPP_TRANSPORT_NODE_OUTPUTGROUP_TAGS);

            writer.end_element("arrayelt");
	        }
        writer.end_element(__CPP_TRANSPORT_NODE_OUTPUT_ARRAY);

        this->task<number>::serialize(writer);
	    }


		template <typename number>
    void output_task<number>::add_element(const derived_data::derived_product<number>& prod, const std::list<std::string>& tags)
	    {
        // check that this derived product has a distinct filename

        for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); t++)
	        {
            if((*t).get_product()->get_filename() == prod.get_filename())
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_A << " " << prod.get_filename() << " "
	                << __CPP_TRANSPORT_OUTPUT_TASK_FILENAME_COLLISION_B << " '" << this->name << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }

            if((*t).get_product_name() == prod.get_name())
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_OUTPUT_TASK_NAME_COLLISION_A << " " << prod.get_name() << " "
	                << __CPP_TRANSPORT_OUTPUT_TASK_NAME_COLLISION_B << " '" << this->name << "'";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }
	        }

        elements.push_back(output_task_element<number>(prod, tags, serial++));
	    }


		template <typename number>
    const output_task_element<number>& output_task<number>::get(unsigned int i) const
	    {
        if(i >= this->elements.size())
	        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_OUTPUT_TASK_RANGE);

        return(this->elements[i]);
	    }


		template <typename number>
		derived_data::derived_product<number>* output_task<number>::lookup_derived_product(const std::string& name)
			{
		    derived_data::derived_product<number>* rval = nullptr;

				for(typename std::vector< output_task_element<number> >::const_iterator t = this->elements.begin(); t != this->elements.end(); t++)
					{
						if((*t).get_product_name() == name)
							{
								rval = (*t).get_product();
								break;
							}
					}

				return(rval);
			}


	}   // namespace transport


#endif //__output_tasks_H_
