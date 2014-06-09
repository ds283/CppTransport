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

#include "transport-runtime-api/unqlite/unqlite_data.h"


//#define __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG


extern "C"
{
#include "unqlite/unqlite.h"
}


namespace transport
	{

		namespace
			{

				//! UnQLite array walk callback; has to accept a plain C function pointer,
				//! whereas we want to use std::bind. So we need this intermediate step
				int unqlite_serializable_array_walker(unqlite_value* key, unqlite_value* value, void* handle)
					{
				    std::function<int(unqlite_value*, unqlite_value*)>* f = static_cast< std::function<int(unqlite_value*, unqlite_value*)>* >(handle);

						return((*f)(key,value));
					}

			}   // unnamed namespace


		// Forward declare unqlite_serialization_reader for friending
		class unqlite_serialization_reader;


    //! implements an UnQLite version of the serialization_writer interface
		class unqlite_serialization_writer: public serialization_writer
			{
		  public:

				//! Destructor
				virtual ~unqlite_serialization_writer()
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
		    virtual void write_value(const std::string& name, unsigned int value) override;
				virtual void write_value(const std::string& name, long long int value) override;
		    virtual void write_value(const std::string& name, double value) override;
		    virtual void write_value(const std::string& name, bool value) override;


				// OUTPUT METHODS -- implementation dependent, not part of the interface


				//! Extract stringized contents in JSON format
				std::string get_contents() const;

				friend class unqlite_serialization_reader;


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


    void unqlite_serialization_writer::write_value(const std::string& name, unsigned int value)
	    {
        this->stack.write_value(name, value);
	    }


		void unqlite_serialization_writer::write_value(const std::string& name, long long int value)
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

				//! constructor: accepts and unqlite_value which should correspond
				//! to a JSON object -- not a JSON array
				unqlite_serialization_reader(unqlite_value* root);

				//! destructor
				virtual ~unqlite_serialization_reader()
					{
					}

		  protected:

				//! walk over an UnQLite JSON object/array and convert it to a JSON serialization stack;
				//! used during construction
				int array_walker(unqlite_value* key, unqlite_value* value);

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

				//! Start reading a new array element at the current level in the tree.
				//! Returns the number of elments it contains, or throws an
				//! exception if the top-of-stack node isn't an array, or there are no
				//! more elements to be read.
				virtual unsigned int start_array_element() override;

		    //! End reading the current node or array
		    virtual void end_element(const std::string& name) override;

				//! End reading the current array element.
				//! Throws an exception if the top-of-stack node isn't an array.
				virtual void end_array_element() override;

		    //! Read attributes from the current node
		    virtual bool read_attribute(const std::string& name, std::string& value) override;

		    //! Read a value
		    virtual bool read_value(const std::string& name, std::string& value) override;
		    virtual bool read_value(const std::string& name, unsigned int& value) override;
				virtual bool read_value(const std::string& name, long long int& value) override;
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
				virtual void insert_value(const std::string& name, long long int value) override;
				virtual void insert_value(const std::string& name, double value) override;
				virtual void insert_value(const std::string& name, bool value) override;

		    //! Insert the contents of a serialization_writer.
				//! Destroys the contents of the writer object, which becomes empty.
		    virtual void insert_writer_contents(serialization_writer& writer) override;


		    // OUTPUT METHODS -- implementation dependent, not part of the interface


		    //! Extract stringized contents in JSON format
		    std::string get_contents() const;


		  protected:

				//! json_serialization_stack object which is used as the underlying implementation
				json_serialization_stack stack;

			};


    unqlite_serialization_reader::unqlite_serialization_reader(unqlite_value* root)
	    {
		    assert(root != nullptr);

				if(root == nullptr || !unqlite_value_is_json_object(root))
					throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_UNQLITE_READER_NOT_JSON_OBJECT);

		    #ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
        std::cerr << std::endl << "READER: Beginning to parse JSON object" << std::endl;
		    #endif

		    // walk through the JSON object, pushing elements on to the stack as we go
        std::function<int(unqlite_value*,unqlite_value*)> f = std::bind(&unqlite_serialization_reader::array_walker, this,
                                                                        std::placeholders::_1, std::placeholders::_2);
		    unqlite_array_walk(root, &unqlite_serializable_array_walker, &f);

		    // reset stack position to HEAD
		    this->stack.pull_reset_head();

		    #ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
        std::cerr << "READER: Finished parsing JSON object" << std::endl << std::endl;
		    #endif
	    }


		int unqlite_serialization_reader::array_walker(unqlite_value* key, unqlite_value* value)
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
				    this->stack.push_start_node(key_name, number == 0);

						#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
				    std::cerr << "READER: Reading node '" << key_name << "'" << std::endl;
						#endif

				    std::function<int(unqlite_value*,unqlite_value*)> f = std::bind(&unqlite_serialization_reader::array_walker, this,
				                                                                    std::placeholders::_1, std::placeholders::_2);
				    unqlite_array_walk(value, &unqlite_serializable_array_walker, &f);

						#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
				    std::cerr << "READER: Finished node '" << key_name << "'" << std::endl;
						#endif

				    this->stack.push_end_element(key_name);
					}
				else if(unqlite_value_is_json_array(value))
					{
						// push this on the stack as a new array
						unsigned int number = static_cast<unsigned int>(unqlite_array_count(value));
				    this->stack.push_start_array(key_name, number == 0);

						#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
				    std::cerr << "READER: Reading array '" << key_name << "'" << std::endl;
						#endif

				    std::function<int(unqlite_value*,unqlite_value*)> f = std::bind(&unqlite_serialization_reader::array_walker, this,
				                                                                    std::placeholders::_1, std::placeholders::_2);
				    unqlite_array_walk(value, &unqlite_serializable_array_walker, &f);

						#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
				    std::cerr << "READER: Finished array '" << key_name << "'" << std::endl;
						#endif

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

								// erase tag from key value
						    std::string untagged_name = key_name;
								untagged_name.erase(0, tag.length());

								#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
						    std::cerr << "READER: Attribute '" << untagged_name << "' = '" << value_name << "'" << std::endl;
								#endif

						    this->stack.write_attribute(untagged_name, value_name);
							}
						else if(key_name != __CPP_TRANSPORT_UNQLITE_RECORD_ID)  // ignore unqlite's internal ID field
							{
								if(unqlite_value_is_bool(value))
									{
										#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
								    std::cerr << "READER: boolean '" << key_name << "' = " << static_cast<bool>(unqlite_value_to_bool(value)) << std::endl;
										#endif

								    this->stack.write_value(key_name, static_cast<bool>(unqlite_value_to_bool(value)));
									}
								else if(unqlite_value_is_int(value))
									{
										unqlite_int64 number = unqlite_value_to_int64(value);

										#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
								    std::cerr << "READER: unqlite_int64 '" << key_name << "' = " << static_cast<unsigned int>(number) << std::endl;
										#endif

										if(number < 0 || number > UINT_MAX) this->stack.write_value(key_name, static_cast<long long int>(number));
										else this->stack.write_value(key_name, static_cast<unsigned int>(number));
									}
								else if(unqlite_value_is_float(value))
									{
										#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
								    std::cerr << "READER: double '" << key_name << "' = " << unqlite_value_to_double(value) << std::endl;
										#endif

								    this->stack.write_value(key_name, unqlite_value_to_double(value));
									}
								else
									{
										const char* vs = unqlite_value_to_string(value, nullptr);
										const std::string value_string(vs);

										#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
								    std::cerr << "READER: string '" << key_name << "' = '" << value_string << "'" << std::endl;
										#endif

								    this->stack.write_value(key_name, value_string);
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


		unsigned int unqlite_serialization_reader::start_array_element()
			{
				unsigned int num_elements;

				if(!this->stack.pull_array_element(num_elements))
					throw runtime_exception(runtime_exception::SERIALIZATION_ERROR, __CPP_TRANSPORT_UNQLITE_READ_ARRAY_ELT_FAIL);

				return(num_elements);
			}


    void unqlite_serialization_reader::end_element(const std::string& name)
	    {
				this->stack.pull_end_element(name);
	    }


		void unqlite_serialization_reader::end_array_element()
			{
				this->stack.pull_end_array_element();
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


    bool unqlite_serialization_reader::read_value(const std::string& name, long long int& value)
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



    void unqlite_serialization_reader::reset()
	    {
//		    assert(false);  // FIXME: disable this method until I work out how to handle restoration of stack elements
				this->stack.pull_reset_head();
	    }


		void unqlite_serialization_reader::push_bookmark()
			{
				this->stack.push_bookmark();
			}


		void unqlite_serialization_reader::pop_bookmark()
			{
				this->stack.pop_bookmark();
			}


		void unqlite_serialization_reader::insert_node(const std::string& name, bool empty)
			{
				#ifdef __CPP_TRANSPORT_UNQLITE_SERIALIZABLE_DEBUG
		    std::cerr << "READER: Inserting node '" << name << "'" << std::endl;
				#endif
		    this->stack.push_start_node(name, empty, true);
			}


		void unqlite_serialization_reader::insert_array(const std::string& name, bool empty)
			{
		    this->stack.push_start_array(name, empty, true);
			}


		void unqlite_serialization_reader::insert_end_element(const std::string& name)
			{
		    this->stack.push_end_element(name, true);
			}


		void unqlite_serialization_reader::insert_attribute(const std::string& name, const std::string& value)
			{
		    this->stack.write_attribute(name, value, true);
			}


		void unqlite_serialization_reader::insert_value(const std::string& name, const std::string& value)
			{
		    this->stack.write_value(name, value, true);
			}


		void unqlite_serialization_reader::insert_value(const std::string& name, unsigned int value)
			{
		    this->stack.write_value(name, value, true);
			}


    void unqlite_serialization_reader::insert_value(const std::string& name, long long int value)
	    {
        this->stack.write_value(name, value, true);
	    }


		void unqlite_serialization_reader::insert_value(const std::string& name, double value)
			{
		    this->stack.write_value(name, value, true);
			}


		void unqlite_serialization_reader::insert_value(const std::string& name, bool value)
			{
		    this->stack.write_value(name, value, true);
			}


		void unqlite_serialization_reader::insert_writer_contents(serialization_writer& writer)
			{
				unqlite_serialization_writer& unq_writer = dynamic_cast<unqlite_serialization_writer&>(writer);
				this->stack.merge(unq_writer.stack);
			}

	}   // namespace transport


#endif //__unqlite_serializable_H_
