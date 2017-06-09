//
// Created by David Seery on 05/12/2013.
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


buffer::buffer(boost::filesystem::path fn, unsigned int cp)
	: in_memory(false),
	  filename(std::move(fn)),
		capacity(cp),
		size(0)
  {
    tag = buf.insert(buf.end(), BUFFER_MAGIC_TAG);

		// connect output stream to the destination file
		out_stream.open(filename.string());

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
						for(const auto& str : this->buf)
							{
								this->out_stream << str << '\n';
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

    for(auto& str : lines)
      {
        bool write = true;
        if(this->skips.size() > 0)
          {
            if(this->skips.front() && str.length() == 0) write = false;
          }

        if(write)
          {
            this->delimit_line(str);

		        // insert line, and increase size accordingly
            this->buf.insert(insertion_point, str);
		        this->size += str.length();
          }
      }

		// if we are now over capacity, emit lines from the front of the buffer
		// if this is possible without overrunning the current tag position
		if(!this->in_memory)
			{
		    // iterators to std::list are not random access, so we cannot work out our position
		    // in the buffer simply by comparison.
		    // First, work out the position of the tag:
		    size_t pos = std::distance(this->buf.begin(), this->tag);

		    size_t i = 0;
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
    for(struct delimiter& dlmt : this->delimiters)
      {
        item.insert(0, dlmt.left);
        item.append(dlmt.right);
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
    for(const auto& str : this->buf)
			{
		    std::cout << str << '\n';

        ++c;
        if(c >= lines) break;
			}

    // print continuation ellipsis if output was truncated
		if(c == lines) std::cout << "..." << '\n';
	}
