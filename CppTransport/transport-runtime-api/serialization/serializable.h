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

				//! Write a value to the current node
				virtual void write_value(const std::string& name, const std::string& value) = 0;
				virtual void write_value(const std::string& name, unsigned int value) = 0;
				virtual void write_value(const std::string& name, long long int value) = 0;
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

		    //! Start reading a new array element at the current level in the tree
		    virtual unsigned int start_array_element() = 0;

        //! End reading the current node or array
        virtual void end_element(const std::string& name) = 0;

		    //! End reading the current array element
		    virtual void end_array_element() = 0;

        //! Read attributes from the current node
        virtual bool read_attribute(const std::string& name, std::string& value) = 0;

        //! Read a value from the current node
        virtual bool read_value(const std::string& name, std::string& value) = 0;
        virtual bool read_value(const std::string& name, unsigned int& value) = 0;
		    virtual bool read_value(const std::string& name, long long int& value) = 0;
        virtual bool read_value(const std::string& name, double& value) = 0;
        virtual bool read_value(const std::string& name, bool& value) = 0;


		    // UPDATE METHODS


		    //! Insert a new node at the current level in the tree
		    virtual void insert_node(const std::string& name, bool empty=false) = 0;

		    //! Insert a new array at the current level in the tree
		    virtual void insert_array(const std::string& name, bool empty=false) = 0;

		    //! End the current node/array insertion
		    virtual void insert_end_element(const std::string& name) = 0;

		    //! Insert attributes in the current node
		    virtual void insert_attribute(const std::string& name, const std::string& value) = 0;

		    //! Insert a value in the current node
		    virtual void insert_value(const std::string& name, const std::string& value) = 0;
		    virtual void insert_value(const std::string& name, unsigned int value) = 0;
		    virtual void insert_value(const std::string& name, long long int value) = 0;
		    virtual void insert_value(const std::string& name, double value) = 0;
		    virtual void insert_value(const std::string& name, bool value) = 0;

		    //! Insert the contents of a serialization_writer
		    virtual void insert_writer_contents(serialization_writer& writer) = 0;

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
	      virtual ~serializable() = default;


	      // SERIALIZATION INTERFACE

	      //! Serialize this object to the repository. Should be implemented by the derived class
	      virtual void serialize(serialization_writer& writer) const = 0;

      };  // class serializable


	}   // namespace transport


#endif //__serializable_H_
