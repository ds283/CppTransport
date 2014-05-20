//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __wrapper_H_
#define __wrapper_H_


#include <iostream>
#include <string>

#include "transport-runtime-api/defaults.h"


namespace transport
	{

		namespace derived_data
			{

		    //! A wrapped-output utility function
		    class wrapped_output
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        wrapped_output(unsigned int width=__CPP_TRANSPORT_DEFAULT_WRAP_WIDTH)
			        : wrap_width(width), cpos(0)
			        {
			        }


		        // WRAPPED OUTPUT

		      public:

		        //! Get wrap width
		        unsigned int get_wrap_width() const { return(this->wrap_width); }

		        //! Set wrap width
		        void set_wrap_width(unsigned int w) { if(w > 0) this->wrap_width = w; }

		        //! Output a string to a stream
		        void wrap_out(std::ostream& out, const std::string& text);

		        //! Output an option to a stream
		        void wrap_list_item(std::ostream& out, bool value, const std::string& label, unsigned int& count);

		        //! Output a string value to a stream
		        void wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count);

		        //! Output a new line
		        void wrap_newline(std::ostream& out) { this->cpos = 0; out << std::endl; }

		        // INTERNAL DATA

		      protected:

		        //! Width at which to wrap output
		        unsigned int wrap_width;

		        //! Current line position
		        unsigned int cpos;
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
		            if(this->cpos + label.length() + 2 >= this->wrap_width) this->wrap_newline(out);
		            if(count > 0) out << ", ";
		            out << label;
		            count++;
		            this->cpos += label.length() + 2;
			        }
			    }


		    void wrapped_output::wrap_value(std::ostream& out, const std::string& value, const std::string& label, unsigned int& count)
			    {
		        if(this->cpos + value.length() + label.length() + 5 >= this->wrap_width) this->wrap_newline(out);
		        if(count > 0) out << ", ";
		        out << label << " = \"" << value << "\"";
		        count++;
		        this->cpos += value.length() + label.length() + 5;
			    }


			}   // namespace derived data

	}   // namespace transport


#endif //__wrapper_H_
