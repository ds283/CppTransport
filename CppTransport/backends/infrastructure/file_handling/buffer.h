//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//



#ifndef CPPTRANSPORT_BUFFER_H
#define CPPTRANSPORT_BUFFER_H

#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <functional>


// set default buffer capacity to 500 Mb
#define DEFAULT_BUFFER_CAPACITY (500*1024*1024)


class translator;

class buffer
  {

  public:

    struct delimiter
      {
        std::string left;
        std::string right;
      };


		// CONSTRUCTOR, DESTRUCTOR

  public:

		// construct a buffer with a named output file and capacity
    buffer(const std::string& file, unsigned int capacity=DEFAULT_BUFFER_CAPACITY);

		// construct a purely in-memory buffer
		buffer();

		// close the buffer, committing it to file if required
		~buffer();


    // INTERFACE

  public:

    // write a line to the end of the buffer
    void write_to_end(std::string line);

    // write a line at the current teg
    void write_to_tag(std::string line);

    // reset current tag position to the end of the buffer
    void set_tag_to_end();

		// merge from a second buffer
		void merge(buffer& source);


    // INTERFACE - DECORATION AND BEHAVIOUR

  public:

    // push a new delimiter value to the top of the stack
    void push_delimiter(std::string left, std::string right);

    // pop a delimiter value from the stack
    void pop_delimiter();

    // push a new blank line skipping flag to the top of the stack
    void push_skip_blank(bool skip);

    // pop a blank line skipping flag from the stack
    void pop_skip_blank();

		// inherit decoration from another buffer
		void inherit_decoration(buffer& source);


		// INTERFACE - GET DATA

  public:

		// is a memory buffer?
		bool is_memory() const { return(this->in_memory); }

		// get filename
		const std::string& get_filename() const { return(this->filename); }


		// PRINT

  public:

		void print_lines(unsigned int lines=10);


		// INTERNAL API

  protected:

		// add delimiters to a line
    void delimit_line              (std::string& line);

		// write a line at a specified insertion point
    void write                     (std::string& line, std::list<std::string>::iterator insertion_point);


		// INTERNAL DATA

  private:

		// is this a purely in-memory buffer?
		// if so, its capacity is not limited
		bool in_memory;

		// filename of output
		std::string filename;

		// current size
		unsigned size;

		// capacity, if relevant
		unsigned capacity;

		// std::stream representing the file to which we are connected
		std::ofstream out_stream;

    std::list<std::string>           buf;
    std::list<std::string>::iterator tag;  // position of tagged location within the buffer

    std::list<struct delimiter> delimiters;
    std::list<bool>             skips;
  };


#endif //CPPTRANSPORT_BUFFER_H
