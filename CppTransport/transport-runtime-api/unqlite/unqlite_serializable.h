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

#include "unqlite/unqlite.h"


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


    void virtual unqlite_serialization_writer::start_node(const std::string& name, bool empty)
	    {
				this->stack.push_start_node(name, empty);
	    }


    void virtual unqlite_serialization_writer::start_array(const std::string& name, bool empty)
	    {
				this->stack.push_start_array(name, empty);
	    }


    void virtual unqlite_serialization_writer::end_element(const std::string& name)
	    {
        this->stack.push_end_element(name);
	    }


    void virtual unqlite_serialization_writer::write_attribute(const std::string& name, const std::string& value)
	    {
				this->stack.write_attribute(name, value);
	    }


    void virtual unqlite_serialization_writer::write_value(const std::string& name, const std::string& value)
	    {
				this->stack.write_value(name, value);
	    }


    void virtual unqlite_serialization_writer::write_value(const std::string& name, unsigned value)
	    {
				this->stack.write_value(name, value);
	    }


    void virtual unqlite_serialization_writer::write_value(const std::string& name, double value)
	    {
				this->stack.write_value(name, value);
	    }


    void virtual unqlite_serialization_writer::write_value(const std::string& name, bool value)
	    {
				this->stack.write_value(name, value);
	    }


    std::string virtual unqlite_serialization_writer::get_contents() const
	    {
        return(this->stack.get_contents());
	    }


		//! implements an UnQLite version of the serialization_reader interface
		class unqlite_serialization_reader: public serialization_reader
			{
		  public:

				//! constructor: accepts and unqlite_value which should correspond
				//! to a JSON object -- not a JSON array
				void unqlite_serialization_reader(unqlite_value* root);


				//! destructor
				virtual ~unqlite_serialization_reader()
					{
					}

		  protected:

				//! walk over an UnQLite JSON object/array and convert it to a JSON serialization stack;
				//! used during construction
				int array_walker(unqlite_value* key, unqlite_value* value, void* handle);

		  public:

				// READING METHODS -- implements a 'serialization_reader' interface


				//! Reset tree, ready for reading again
				virtual void reset() override;

				//! Push bookmark: further calls to reset() will return the tree to this position.
				virtual void push_bookmark() override;

				//! Pop bookmark
				virtual void pop_bookmark() override;

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


				// UPDATE METHODS


				//! Insert a new node at the current level in the tree.
				virtual void insert_node(const std::string& name, bool empty=false) override;

				//! Insert a new array at the current level in the tree
				virtual void insert_array(const std::string& name, bool empty=false) override;

				//! End the current node/array insertion
				virtual void insert_end_element(const std::string& name) override;

				//! Insert attributes in the current node
				virtual void insert_attribute(const std::string& name, const std::string& value) override;

				//! Insert a value in the current node
				virtual void insert_value(const std::string& name, const std::string& value) override;
				virtual void insert_value(const std::string& name, unsigned int value) override;
				virtual void insert_value(const std::string& name, double value) override;
				virtual void insert_value(const std::string& name, bool value) override;


		    // OUTPUT METHODS -- implementation dependent, not part of the interface


		    //! Extract stringized contents in JSON format
		    std::string get_contents() const;


		  protected:

				//! json_serialization_stack object which is used as the underlying implementation
				json_serialization_stack stack;

			};


		namespace
			{

			}   // anonymous namespace


    unqlite_serialization_reader::unqlite_serialization_reader(unqlite_value* root)
	    {
		    assert(root != nullptr);

				if(root == nullptr || !unqlite_value_is_json_object(root))
					throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_UNQLITE_READER_NOT_JSON_OBJECT);

		    // walk through the JSON object, pushing elements on to the stack as we go
		    unqlite_array_walk(root, std::bind(&unqlite_serialization_reader::array_walker, this,
		                                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), nullptr);

		    // reset stack position to HEAD
		    this->stack.pull_reset_head();
	    }


		int unqlite_serialization_reader::array_walker(unqlite_value* key, unqlite_value* value, void* handle)
			{
				if(key == nullptr || unqlite_value_is_json_object(key) || unqlite_value_is_json_array(key))
					throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_UNQLITE_READER_KEY_FAIL);

				if(value == nullptr) throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_UNQLITE_READER_VALUE_FAIL);

				const char* kn = unqlite_value_to_string(key, nullptr);
		    const std::string key_name(kn);

				if(unqlite_value_is_json_object(value))
					{
						// push this on the stack as a new node
						unsigned int number = static_cast<unsigned int>(unqlite_array_count(value));
						this->stack.push_start_node(key_name, number==0);
						unqlite_array_walk(value, std::bind(&unqlite_serialization_reader::array_walker, this,
																								std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), nullptr);
						this->stack.push_end_element(key_name);
					}
				else if(unqlite_value_is_json_array(value))
					{
						// push this on the stack as a new array
						unsigned int number = static_cast<unsigned int>(unqlite_array_count(value));
						this->stack.push_start_array(key_name, number==0);
						unqlite_array_walk(value, std::bind(&unqlite_serialization_reader::array_walker, this,
						                                    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), nullptr);
						this->stack.push_end_element(key_name);
					}
				else
					{
						// must be a value; but it could be either an attribute or one of the contents
						// check if it is tagged as an attribute
				    const std::string tag = __CPP_TRANSPORT_JSON_ATTRIBUTE_TAG;

						if(key_name.find(tag) == 0)
							{
								const char* vn = unqlite_value_to_string(value, nullptr);
						    const std::string value_name(vn);
						    std::string untagged_name = key_name;
								this->stack.write_attribute(untagged_name.erase(0,tag.length()), value_name);
							}
						else
							{
								if(unqlite_value_is_bool(value))
									{
								    this->stack.write_value(key_name, static_cast<bool>(unqlite_value_to_bool(value)));
									}
								else if(unqlite_value_is_int(value))
									{
										int number = unqlite_value_to_int(value);
										if(number < 0) throw runtime_exception(runtime_exception::REPOSITORY_BACKEND_ERROR, __CPP_TRANSPORT_REPO_JSON_FAIL);
								    this->stack.write_value(key_name, static_cast<unsigned int>(number));
									}
								else if(unqlite_value_is_numeric(value))
									{
										this->stack.write_value(key_name, unqlite_value_to_double(value));
									}
								else
									{
										const char* vn = unqlite_value_to_string(value, nullptr);
										const std::string value_name(vn);
										this->stack.write_value(key_name, value_name);
									}
							}
					}

				return(UNQLITE_OK);
			}


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
            msg << __CPP_TRANSPORT_UNQLITE_READ_ARRAY_FAIL << " '" << name << "'";
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


    std::string unqlite_serialization_reader::get_contents() const
	    {
        return(this->stack.get_contents());
	    }



    void virtual unqlite_serialization_reader::reset()
	    {
				this->stack.pull_reset_head();
	    }


		void virtual unqlite_serialization_reader::push_bookmark()
			{
				this->stack.push_bookmark();
			}


		void virtual unqlite_serialization_reader::pop_bookmark()
			{
				this->stack.pop_bookmark();
			}


		void virtual unqlite_serialization_reader::insert_node(const std::string& name, bool empty)
			{
				this->stack.push_start_node(name, empty);
			}


		void virtual unqlite_serialization_reader::insert_array(const std::string& name, bool empty)
			{
				this->stack.push_start_array(name, empty);
			}


		void virtual unqlite_serialization_reader::insert_end_element(const std::string& name)
			{
				this->stack.push_end_element(name);
			}


		void virtual unqlite_serialization_reader::insert_attribute(const std::string& name, const std::string& value)
			{
				this->stack.write_attribute(name, value);
			}


		void virtual unqlite_serialization_reader::insert_value(const std::string& name, const std::string& value)
			{
				this->stack.write_value(name, value);
			}


		void virtual unqlite_serialization_reader::insert_value(const std::string& name, unsigned int value)
			{
				this->stack.write_value(name, value);
			}


		void virtual unqlite_serialization_reader::insert_value(const std::string& name, double value)
			{
				this->stack.write_value(name, value);
			}


		void virtual unqlite_serialization_reader::insert_value(const std::string& name, bool value)
			{
				this->stack.write_value(name, value);
			}

	}   // namespace transport


#endif //__unqlite_serializable_H_
