//
// Created by David Seery on 25/04/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __unqlite_serializable_H_
#define __unqlite_serializable_H_


#include <string>
#include <list>
#include <functional>
#include <utility>

#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/serialization/serializable.h"
#include "transport-runtime-api/serialization/json_serialization_stack.h"


namespace transport
	{

    //! implements an UnQLite version of the serialization_writer interface
		class unqlite_serialization_writer: public serialization_writer
			{
		  public:

				//! Destructor
				virtual void ~unqlite_serialization_writer()
					{
					}


				// WRITING METHODS -- implements a 'serialization_writer' interface


				//! Begin a new node at the current level in the tree
				virtual void start_node(const std::string& name, bool empty=false) override;

		    //! Begin a new array at the current level in the tree
		    virtual void start_array(const std::string& name, bool empty=false) override;

		    //! End the current node or array
		    virtual void end_element(const std::string& name) override;

		    //! Write attributes to the current node
		    virtual void write_attribute(const std::string& name, const std::string& value) override;

		    //! Write a value
		    virtual void write_value(const std::string& name, const std::string& value) override;
		    virtual void write_value(const std::string& name, unsigned value) override;
		    virtual void write_value(const std::string& name, double value) override;
		    virtual void write_value(const std::string& name, bool value) override;


				// OUTPUT METHODS -- implementation dependent, not part of the interface


				//! Extract stringized contents in JSON format
				std::string get_contents() const;


		  protected:
				json_serialization_stack stack;

			};


    void unqlite_serialization_writer::start_node(const std::string& name, bool empty)
	    {
				this->stack.push_start_node(name, empty);
	    }


    void unqlite_serialization_writer::start_array(const std::string& name, bool empty)
	    {
				this->stack.push_start_array(name, empty);
	    }


    void unqlite_serialization_writer::end_element(const std::string& name)
	    {
        this->stack.push_end_element(name);
	    }


    void unqlite_serialization_writer::write_attribute(const std::string& name, const std::string& value)
	    {
				this->stack.write_attribute(name, value);
	    }


    void unqlite_serialization_writer::write_value(const std::string& name, const std::string& value)
	    {
				this->stack.write_value(name, value);
	    }


    void unqlite_serialization_writer::write_value(const std::string& name, unsigned value)
	    {
				this->stack.write_value(name, value);
	    }


    void unqlite_serialization_writer::write_value(const std::string& name, double value)
	    {
				this->stack.write_value(name, value);
	    }


    void unqlite_serialization_writer::write_value(const std::string& name, bool value)
	    {
				this->stack.write_value(name, value);
	    }


    std::string unqlite_serialization_writer::get_contents() const
	    {
        return(this->stack.get_contents());
	    }


		//! implements an UnQLite version of the serialization_reader interface
		class unqlite_serialization_reader: public serialization_reader
			{
		  public:

				//! destructor
				virtual ~unqlite_serialization_reader()
					{
					}


				// READING METHODS -- implements a 'serialization_reader' interface


		    //! Start reading a new node at the current level in the tree.
				//! Returns number of elements in the node, or throws an
				//! exception if the node cannot be read.
		    virtual unsigned int start_node(const std::string& name) override;

		    //! Start reading a new array at the current level in the tree.
				//! Returns number of elements in the array, or throws an
				//! exception if the node cannot be read.
		    virtual unsigned int start_array(const std::string& name) override;

		    //! End reading the current node or array
		    virtual void end_element(const std::string& name) override;

		    //! Read attributes from the current node
		    virtual bool read_attribute(const std::string& name, std::string& value) override;

		    //! Read a value
		    virtual bool read_value(const std::string& name, std::string& value) override;
		    virtual bool read_value(const std::string& name, unsigned int& value) override;
		    virtual bool read_value(const std::string& name, double& value) override;
		    virtual bool read_value(const std::string& name, bool& value) override;


		  protected:
				json_serialization_stack stack;

			};


    unsigned int unqlite_serialization_reader::start_node(const std::string& name)
	    {
		    unsigned int num_elements, num_attributes;

        if(!this->stack.pull_start_node(name, num_elements, num_attributes))
	        {
            std::ostringstream msg;
		        msg << __CPP_TRANSPORT_UNQLITE_READ_NODE_FAIL << " '" << name << "'";
		        throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

		    return(num_elements);
	    }


    unsigned int unqlite_serialization_reader::start_array(const std::string& name)
	    {
        unsigned int num_elements;

        if(!this->stack.pull_start_array(name, num_elements))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_READ_NODE_FAIL << " '" << name << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

        return(num_elements);
	    }


    void unqlite_serialization_reader::end_element(const std::string& name)
	    {
				this->stack.pull_end_element(name);
	    }


    bool unqlite_serialization_reader::read_attribute(const std::string& name, std::string& value)
	    {
        if(!this->stack.read_attribute(name, value))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_READ_ATTR_FAIL << " '" << name << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

        return(true);
	    }


    bool unqlite_serialization_reader::read_value(const std::string& name, std::string& value)
	    {
        if(!this->stack.read_value(name, value))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_READ_VALUE_FAIL << " '" << name << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

        return(true);
	    }


    bool unqlite_serialization_reader::read_value(const std::string& name, unsigned int& value)
	    {
        if(!this->stack.read_value(name, value))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_READ_VALUE_FAIL << " '" << name << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

        return(true);
	    }


    bool unqlite_serialization_reader::read_value(const std::string& name, double& value)
	    {
        if(!this->stack.read_value(name, value))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_READ_VALUE_FAIL << " '" << name << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

        return(true);
	    }


    bool unqlite_serialization_reader::read_value(const std::string& name, bool& value)
	    {
        if(!this->stack.read_value(name, value))
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_UNQLITE_READ_VALUE_FAIL << " '" << name << "'";
            throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, msg.str());
	        }

        return(true);
	    }


	}   // namespace transport


#endif //__unqlite_serializable_H_
