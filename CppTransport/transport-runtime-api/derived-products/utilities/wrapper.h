//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
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


#endif //__wrapper_H_
