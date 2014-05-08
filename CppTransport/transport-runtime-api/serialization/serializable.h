//
// Created by David Seery on 25/04/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __serializable_H_
#define __serializable_H_


#include <list>
#include <string>
#include <functional>
#include <utility>

#include "boost/lexical_cast.hpp"


namespace transport
	{

    //! The API has some XML-like concepts, such as 'attributes', which may not exist
    //! in the format which is really used as the backend serialization - eg, JSON
    //! does not have this conception.
    //! The original repository was implemented using the Berkeley DB XML
    //! backend, which explicitly used XML and therefore supported these concepts.
		//! Berkeley DB XML has since been replaced, but
    //! they have been retained in order not to break the interface.
    //! Eventually we might want to look at removing them.


		// WRITER INTERFACE


		//! an abstract class which defines a serialization output stream

		class serialization_writer
			{
		  public:

				//! define virtual destructor so that all derived destructors get called correctly
				virtual ~serialization_writer()
					{
					}


				// WRITING METHODS


				//! Begin a new node at the current level in the tree
				virtual void start_node(const std::string& name, bool empty=false) = 0;

        //! Begin a new array at the current level in the tree
        virtual void start_array(const std::string& name, bool empty=false) = 0;

		    //! End the current node or array
				virtual void end_element(const std::string& name) = 0;

				//! Write attributes to the current node
				virtual void write_attribute(const std::string& name, const std::string& value) = 0;

				//! Write a value
				virtual void write_value(const std::string& name, const std::string& value) = 0;
				virtual void write_value(const std::string& name, unsigned value) = 0;
				virtual void write_value(const std::string& name, double value) = 0;
				virtual void write_value(const std::string& name, bool value) = 0;


        // OUTPUT METHODS


        // None defined as part of the interface -- this is implementation dependent

			};  // class serialization_writer


    // READER INTERFACE


    //! an abstract class which defines a serialization input stream

    class serialization_reader
	    {
      public:

        //! define virtual destructor so that all derived destructors get called correctly
        virtual ~serialization_reader()
	        {
	        }


        // READING METHODS

		    //! Reset tree, ready for reading again
		    virtual void reset() = 0;

		    //! Push bookmark: further calls to reset() will return the tree to this position.
		    virtual void push_bookmark() = 0;

		    //! Pop bookmark
		    virtual void pop_bookmark() = 0;

        //! Start reading a new node at the current level in the tree
        virtual unsigned int start_node(const std::string& name) = 0;

        //! Start reading a new array at the current level in the tree
        virtual unsigned int start_array(const std::string& name) = 0;

        //! End reading the current node or array
        virtual void end_element(const std::string& name) = 0;

        //! Read attributes from the current node
        virtual bool read_attribute(const std::string& name, std::string& value) = 0;

        //! Read a value
        virtual bool read_value(const std::string& name, std::string& value) = 0;
        virtual bool read_value(const std::string& name, unsigned int& value) = 0;
        virtual bool read_value(const std::string& name, double& value) = 0;
        virtual bool read_value(const std::string& name, bool& value) = 0;


        // OUTPUT METHODS


        // None defined as part of the interface -- this is implementation dependent

	    };  // class serialization_reader


		// SERIALIZABLE OBJECT INTERFACE


		//! an abstract class which defines the serialization interface

		//! Serialization is used when writing tasks and models to the repository

    class serializable
      {
      public:
				// define virtual destructor so that all derived destructors get called correctly
	      virtual ~serializable()
		      {
		      }


	      // SERIALIZATION INTERFACE

	      //! Serialize this object to the repository. Should be implemented by the derived class
	      virtual void serialize(serialization_writer& writer) const = 0;


	      // STANDARD SERIALIZATION TOOLS, USABLE BY DERIVED CLASSES WHICH IMPLEMENT A VERSION OF THIS INTERFACE


	      //! Begin a node, perhaps with an arbitrary number of attributes
	      template <typename... attrs>
	      void begin_node(serialization_writer& writer, const std::string& name, bool empty, attrs... attributes);

        //! Begin an array. Arrays can have no attributes
        void begin_array(serialization_writer& writer, const std::string& name, bool empty);

	      //! End the current element -- node or array
	      void end_element(serialization_writer& writer, const std::string& name);

		    //! Write a node with a single value, but no attributes
        template <typename T>
        void write_value_node(serialization_writer& writer, const std::string& name, const T& value);

	      //! Write a node with a single value and attributes
	      template <typename T, typename... attrs>
	      void write_value_node(serialization_writer& writer, const std::string& name, const T& value, attrs... attributes);

	      //! Write attributes: base case, no attributes to write
	      void write_attributes(serialization_writer& writer);

	      //! Write attributes: inductive case
		    template <typename T, typename... attrs>
	      void write_attributes(serialization_writer& writer, const std::string& attr_name, const T& attr_val, attrs... other_attributes);


		    // STANDARD DESERIALIZATION TOOLS, USABLE BY DERIVED CLASSES WHICH IMPLEMENT A VERSION OF THIS INTERFACE

      };  // class serializable


		// SERIALIZATION METHODS


		// begin a node
		template <typename... attrs>
		void serializable::begin_node(serialization_writer& writer, const std::string& name, bool empty, attrs... attributes)
			{
				static_assert(sizeof...(attributes) %2 == 0, "Attributes for serializable::begin_node must occur in (std::string, typename) pairs");

				writer.start_node(name, empty);
				this->write_attributes(writer, std::forward<attrs>(attributes)...);
			}


		// write attributes - base case, nothing to do
		void serializable::write_attributes(serialization_writer& writer)
			{
			}


		// write attributes - inductive case
		template <typename T, typename... attrs>
		void serializable::write_attributes(serialization_writer& writer, const std::string& attr_name const T& attr_val, attrs... other_attributes)
			{
				static_assert(sizeof...(other_attributes) % 2 == 0, "Attributes for serializable::write_attributes must occur in (std::string, typename) pairs");

				// stringize value of attribute
				std::string value = boost::lexical_cast<std::string>(attr_val);

				// write this attribute, then call recursively to write the rest
				writer.write_attribute(attr_name, value);
				this->write_attributes(writer, std::forward<attrs>(other_attributes)...);
			}


    // begin an array
    void begin_array(serialization_writer& writer, const std::string& name, bool empty)
      {
        writer.start_array(name, empty);
      }


		// end an element -- node or array
		void serializable::end_element(serialization_writer& writer, const std::string& name)
			{
				writer.end_element(name);
			}


		// write a node with a single value
    template <typename T>
		void serializable::write_value_node(serialization_writer& writer, const std::string name, const T& value)
			{
				writer.write_value(name, value);
			}

		template <typename T, typename... attrs>
		void serializable::write_value_node(serialization_writer& writer, const std::string& name, const T& value, attrs... attributes)
			{
				this->begin_node(writer, name, false, std::forward<attrs>(attributes)...);

				writer.write_value(name, value);

				this->end_element(writer, name);
			}


	}   // namespace transport


#endif //__serializable_H_
