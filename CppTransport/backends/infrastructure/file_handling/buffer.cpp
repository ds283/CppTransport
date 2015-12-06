//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <stdexcept>

#include "core.h"
#include "buffer.h"
#include "error.h"
#include "translator.h"

#include "boost/algorithm/string.hpp"


#define BUFFER_MAGIC_TAG "MAGIC_TAG"


buffer::buffer(const std::string& fn, unsigned int cp)
	: in_memory(false),
	  filename(fn),
		capacity(cp),
		size(0)
  {
    tag = buf.insert(buf.end(), BUFFER_MAGIC_TAG);

		// connect output stream to the destination file
		out_stream.open(filename);

		if(!out_stream.is_open() || out_stream.fail())
			{
		    std::ostringstream msg;
		    msg << ERROR_CPP_BUFFER_WRITE << " '" << fn << "'";

        throw std::runtime_error(msg.str());
			}
  }


buffer::buffer()
	: in_memory(true),
    capacity(0),
    size(0)
	{
		// set tag position to the end of the buffer
		tag = buf.insert(buf.end(), BUFFER_MAGIC_TAG);
	}


buffer::~buffer()
	{
		// remove magic tag, if it is still present (we expect it should be)
		if(*(this->tag) == BUFFER_MAGIC_TAG)
			{
				this->buf.erase(this->tag);
			}

		// write any remaining lines to file, if required
		if(!this->in_memory)
			{
		    // check if safe to write, but don't raise an error if not
		    // presumably this was flagged on construction
				if(this->out_stream.is_open())
					{
						for(std::list<std::string>::iterator t = this->buf.begin(); t != this->buf.end(); ++t)
							{
								this->out_stream << (*t) << '\n';
							}

						this->out_stream.close();
					}
			}
	}


void buffer::write(std::string& line, std::list<std::string>::iterator insertion_point)
  {
    // break up the string to be written into individual lines
    std::vector<std::string> lines;
    boost::split(lines, line, boost::is_any_of(NEWLINE_CHAR));

    for(std::vector<std::string>::const_iterator t = lines.begin(); t != lines.end(); ++t)
      {
        bool write = true;
        if(this->skips.size() > 0)
          {
            if(this->skips.front() && t->length() == 0) write = false;
          }

        if(write)
          {
            std::string item = *t;
            this->delimit_line(item);

		        // insert line, and increase size accordingly
            this->buf.insert(insertion_point, item);
		        this->size += item.length();
          }
      }

		// if we are now over capacity, emit lines from the front of the buffer
		// if this is possible without overrunning the current tag position
		if(!this->in_memory)
			{
		    // iterators to std::list are not random access, so we cannot work out our position
		    // in the buffer simply by comparison.
		    // First, work out the position of the tag:
		    unsigned int pos = std::distance(this->buf.begin(), this->tag);

		    unsigned int i = 0;
		    while(this->size > this->capacity && i < pos)
			    {
		        if(this->out_stream.is_open() && !this->out_stream.fail())
			        {
		            this->out_stream << *(this->buf.begin()) << '\n';
			        }

		        // remove line from front of buffer, and decrease size accordingly
		        this->size -= this->buf.begin()->length();
		        this->buf.pop_front();
		        i++;
			    }
			}
  }


void buffer::write_to_end(std::string line)
  {
    this->write(line, this->buf.end());
  }


void buffer::write_to_tag(std::string line)
  {
    this->write(line, this->tag);
  }


void buffer::delimit_line(std::string& item)
  {
    for(std::list<struct delimiter>::iterator t = this->delimiters.begin(); t != this->delimiters.end(); ++t)
      {
        item.insert(0, (*t).left);
        item.append((*t).right);
      }
  }


void buffer::set_tag_to_end()
  {
    // remove blank placeholder line if it is safe to do so (should always be true)
    if(*(this->tag) == BUFFER_MAGIC_TAG) this->buf.erase(this->tag);

    // magic to keep track of tagged line position
    // (it's hard to make sure tagged content appears at the right place, because
    // tags will usually be set *before* output of the line they correspond to--that is,
    // macro replacement functions which set the tag location have to return a string,
    // and the string is sent to the buffer *afterwards*)
    // this magic uses a special tagged line to keep track of where
    // the insertion should really happen
    this->tag = this->buf.insert(this->buf.end(), BUFFER_MAGIC_TAG);
  }


void buffer::merge(buffer& source)
	{
		if(*(source.tag) == BUFFER_MAGIC_TAG)
			{
				source.buf.erase(source.tag);
			}

		this->buf.splice(this->buf.end(), source.buf);
		this->size += source.size;
		source.size = 0;

		source.tag = source.buf.insert(source.buf.begin(), BUFFER_MAGIC_TAG);
	}


void buffer::push_delimiter(std::string left, std::string right)
  {
    struct delimiter d;

    d.left = left;
    d.right = right;

    this->delimiters.push_back(d);
  }


void buffer::pop_delimiter()
  {
    if(this->delimiters.size() > 0)
      {
        this->delimiters.pop_back();
      }
  }


void buffer::push_skip_blank(bool skip)
  {
    this->skips.push_front(skip);
  }


void buffer::pop_skip_blank()
  {
    if(this->skips.size() > 0)
      {
        this->skips.pop_front();
      }
  }


void buffer::inherit_decoration(buffer& source)
	{
		this->delimiters = source.delimiters;
		this->skips = source.skips;
	}


void buffer::print_lines(unsigned int lines)
	{
		unsigned int c = 0;
		for(std::list<std::string>::iterator t = this->buf.begin(); t != this->buf.end() && c < lines; ++t, ++c)
			{
		    std::cout << (*t) << '\n';
			}
		if(c == lines) std::cout << "..." << '\n';
	}