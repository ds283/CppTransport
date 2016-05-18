//
// Created by David Seery on 19/05/2014.
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


#ifndef CPPTRANSPORT_WRAPPER_H
#define CPPTRANSPORT_WRAPPER_H


#include <iostream>
#include <string>

#include "transport-runtime/defaults.h"


namespace transport
	{

		namespace derived_data
			{

		    //! A wrapped-output utility function
		    class wrapped_output
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        wrapped_output(unsigned int width=CPPTRANSPORT_DEFAULT_WRAP_WIDTH)
			        : wrap_width(width),
			          cpos(0),
		            left_margin(0)
			        {
			        }


		        // WRAPPED OUTPUT

		      public:

		        //! Get wrap width
		        unsigned int get_wrap_width() const { return(this->wrap_width); }

		        //! Set wrap width
		        void set_wrap_width(unsigned int w) { if(w > 0) this->wrap_width = w; }

				    //! Get left margin
				    unsigned int get_left_margin() const { return(this->left_margin); }

				    //! Set left margin
				    void set_left_margin(unsigned int w) { this->left_margin = w; }

		        //! Output a string to a stream
		        void wrap_out(std::ostream& out, const std::string& text);

		        //! Output an option to a stream
		        void wrap_list_item(std::ostream& out, bool value, const std::string& label, unsigned int& count);

		        //! Output a string value to a stream
		        void wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count);

		        //! Output a new line
		        void wrap_newline(std::ostream& out);

		        // INTERNAL DATA

		      protected:

		        //! Width at which to wrap output
		        unsigned int wrap_width;

		        //! Current line position
		        unsigned int cpos;

				    //! Current left-margin value
				    unsigned int left_margin;

			    };


		    void wrapped_output::wrap_out(std::ostream& out, const std::string& text)
			    {
		        if(this->cpos + text.length() >= this->wrap_width) this->wrap_newline(out);
		        out << text;
		        this->cpos += text.length();
			    }


		    void wrapped_output::wrap_list_item(std::ostream& out, bool value, const std::string& label, unsigned int& count)
			    {
		        if(value)
			        {
		            if(count > 0) out << ", ";
		            if(this->cpos + label.length() + 2 >= this->wrap_width) this->wrap_newline(out);
		            out << label;
		            count++;
		            this->cpos += label.length() + 2;
			        }
			    }


		    void wrapped_output::wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count)
			    {
		        if(count > 0) out << ", ";
		        if(this->cpos + value.length() + label.length() + 5 >= this->wrap_width) this->wrap_newline(out);
		        out << label << " = \"" << value << "\"";
		        count++;
		        this->cpos += value.length() + label.length() + 5;
			    }


				void wrapped_output::wrap_newline(std::ostream& out)
					{
				    out << '\n';

						for(unsigned int i = 0; i < this->left_margin; ++i)
							{
								out << " ";
							}
				    this->cpos = this->left_margin;
					}


			}   // namespace derived data

	}   // namespace transport


#endif //CPPTRANSPORT_WRAPPER_H
