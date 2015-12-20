//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ASCIITABLE_H_
#define CPPTRANSPORT_ASCIITABLE_H_

#include <iostream>
#include <iomanip>
#include <cmath>
#include <assert.h>

#include <vector>


#define DEFAULT_ASCIITABLE_PRECISION     (12)
#define DEFAULT_ASCIITABLE_DISPLAY_WIDTH (80)
#define DEFAULT_ASCIITABLE_WRAP_WIDTH    (true)


namespace transport
  {

    template <typename number>
    class asciitable
	    {

      public:

        asciitable(std::ostream& s)
	        : stream(s), precision(DEFAULT_ASCIITABLE_PRECISION),
	          display_width(DEFAULT_ASCIITABLE_DISPLAY_WIDTH),
	          wrap_width(DEFAULT_ASCIITABLE_WRAP_WIDTH)
	        {
	        }


		    //! Write content to the table
        void write(std::string x_name, const std::vector<std::string>& columns,
                   const std::vector<double>& xs, const std::vector<std::vector<number> >& ys, const std::string tag = "");

		    //! Set precision for output content
        void set_precision(unsigned int p);

		    //! Set wrap width
        void         set_display_width(unsigned int width);
		    //! Get wrap width
        unsigned int get_display_width();

		    //! Set wrap setting
        void set_wrap_status(bool wrap);
		    //! Get wrap setting
        bool get_wrap_status();

      protected:

				//! Output stream
        std::ostream& stream;

        //! Precision with which to output numbers
        unsigned int precision;

        //! Display size in columns
        unsigned int display_width;

        //! Whether to wrap - good for display, bad for files
        bool         wrap_width;
	    };


      // IMPLEMENTATION -- CLASS asciitable


    template <typename number>
    void asciitable<number>::write(std::string x_name,
                                   const std::vector<std::string>& columns, const std::vector<double>& xs, const std::vector< std::vector<number> >& ys,
                                   const std::string tag)
	    {
        // save current precision
        std::streamsize         prev_precision = this->stream.precision();
        std::ios_base::fmtflags prev_flags     = this->stream.flags();

        // a number with specified precision may have that many digits, plus a decimal point
        // plus possible a mantissa 'e+XXX', plus a leading sign
        size_t max = this->precision + 1 + 5 + 1;

        assert(xs.size() == ys.size());

        for(size_t i = 0; i < columns.size(); ++i)
	        {
            size_t len;
            if((len = columns[i].size()) > max)
	            {
                max = len;
	            }
	        }
        max++;

        // write out tag if one has been given
        if(!tag.empty())
	        {
            this->stream << "// " << tag << '\n';
	        }

        // max is the column width
        // if we are going to wrap, want to work out how many columns we can fit
        size_t columns_per_display = this->display_width / max;
        if(columns_per_display < 1) columns_per_display = 1;
        if(!wrap_width) columns_per_display             = columns.size();     // if not wrapping, then output all columns at once

        unsigned int columns_output = 0;
        while(columns_output < columns.size())
	        {
            // insert blank line if this is a continuation
            if(columns_output > 0) this->stream << '\n';

            // how many columns to output on this go?
            size_t columns_left     = columns.size() - columns_output;
            size_t columns_to_print = (columns_left < columns_per_display ? columns_left : columns_per_display);

            // write out column headings
            this->stream << std::right << std::setw((unsigned int) max) << x_name;
            for(size_t i = 0; i < columns_to_print; ++i)
	            {
                this->stream << std::right << std::setw((unsigned int) max) << columns[columns_output + i];
	            }
            this->stream << '\n';

            // write out data
            for(size_t i = 0; i < ys.size(); ++i)
	            {
                assert(columns.size() == ys[i].size());

                this->stream << " "
	                << std::right << std::setw((unsigned int) (max - 1))
	                << std::scientific
	                << std::setprecision(this->precision - 1) << xs[i];

                for(int j = 0; j < columns_to_print; ++j)
	                {
		                if(std::isnan((ys[i])[columns_output + j]))
			                {
		                    this->stream << " "
			                    << std::right << std::setw((unsigned int) (max - 1))
			                    << std::scientific
			                    << std::setprecision(this->precision - 1) << "--";
			                }
		                else
			                {
		                    this->stream << " "
			                    << std::right << std::setw((unsigned int) (max - 1))
			                    << std::scientific
			                    << std::setprecision(this->precision - 1) << (ys[i])[columns_output + j];
			                }
	                }
                this->stream << '\n';
	            }
            columns_output += columns_to_print;
	        }

        this->stream.precision(prev_precision);
        this->stream.flags(prev_flags);
	    }


    template <typename number>
    void asciitable<number>::set_precision(unsigned int p)
	    {
        this->precision = p;
	    }


    template <typename number>
    void asciitable<number>::set_display_width(unsigned int width)
	    {
        this->display_width = (width > 1 ? width : 1);
	    }


    template <typename number>
    unsigned int asciitable<number>::get_display_width()
	    {
        return (this->display_width);
	    }


    template <typename number>
    void asciitable<number>::set_wrap_status(bool wrap)
	    {
        this->wrap_width = wrap;
	    }


    template <typename number>
    bool asciitable<number>::get_wrap_status()
	    {
        return (this->wrap_width);
	    }


  }   // namespace transport

#endif //CPPTRANSPORT_ASCIITABLE_H_
