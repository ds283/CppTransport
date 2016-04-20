//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_ASCIITABLE_H
#define CPPTRANSPORT_ASCIITABLE_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <assert.h>
#include <vector>
#include <sstream>


#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"

#include "transport-runtime/ansi_colour_codes.h"

#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


constexpr unsigned int DEFAULT_ASCIITABLE_PRECISION     = (12);
constexpr bool         DEFAULT_ASCIITABLE_WRAP_WIDTH    = (true);


namespace transport
  {

    enum class column_justify { left, right };

    class column_descriptor
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        column_descriptor(std::string n, column_justify j=column_justify::left, bool b=true)
          : name(std::move(n)),
            just(j),
            bold(b),
            prev_width(0),
            prev_prec(0)
          {
          }

        //! destructor is default
        ~column_descriptor() = default;


        // INTERFACE

      public:

        //! get name
        const std::string& get_name() const { return(this->name); }

        //! get width
        size_t get_width() const { return(this->name.length()); }

        //! apply formatting for column
        template <typename Stream>
        void apply_format(Stream& out, size_t w, bool term, local_environment& env, argument_cache& cache)
          {
            // remember previous status of formatting flags, so we can restore later
            this->prev_flags = out.flags();
            this->prev_width = out.width();
            this->prev_prec = out.precision();

            switch(this->just)
              {
                case column_justify::left:
                  {
                    out << std::left;
                    break;
                  }

                case column_justify::right:
                  {
                    out << std::right;
                    break;
                  }
              }

            out << std::setw(w);
          }

        //! apply formatting for column titles
        template <typename Stream>
        void apply_format_title(Stream& out, size_t w, bool term, local_environment& env, argument_cache& cache)
          {
            if(term && this->bold && env.has_colour_terminal_support() && cache.get_colour_output())
              {
                out << ColourCode(ANSI_colour::bold);
              }

            this->apply_format(out, w, term, env, cache);
          }

        //! deapply formatting rules
        template <typename Stream>
        void deapply_format(Stream& out, bool term, local_environment& env, argument_cache& cache) const
          {
            // reset stream formatting flags
            out.flags(this->prev_flags);
            out.width(this->prev_width);
            out.precision(this->prev_prec);

            if(term && this->bold && env.has_colour_terminal_support() && cache.get_colour_output())
              {
                out << ColourCode(ANSI_colour::normal);
              }
          }


        // INTERNAL DATA

      private:

        //! column name
        std::string name;

        //! justification for this column
        column_justify just;

        //! apply bold to this column title?
        bool bold;

        //! cache previous formatting flags for stream
        std::ios_base::fmtflags prev_flags;

        //! cache previous width setting
        std::streamsize prev_width;

        //! cache previous precision setting
        std::streamsize prev_prec;

      };

    class asciitable
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        asciitable(std::ostream& s, local_environment& e, argument_cache& c)
	        : stream(s),
            env(e),
            arg_cache(c),
            precision(DEFAULT_ASCIITABLE_PRECISION),
	          display_width(e.detect_terminal_width()),
	          wrap_width(DEFAULT_ASCIITABLE_WRAP_WIDTH),
            terminal_output(false)
	        {
          }

        //! destructor is default
        ~asciitable() = default;


        // INTERFACE -- WRITE CONTENT

      public:

		    //! Write columns of numbers; the data in ys should be stored column-wise
        template <typename number>
        void write(std::string x_name, const std::vector<std::string>& columns,
                   const std::vector<double>& xs, const std::vector<std::vector<number> >& ys,
                   const std::string tag = "");

        //! Write columns of text; the data in table should be stored column-wise;
        //! vector of column descriptors is not marked const since the descriptors need to cache internal state
        //! while writing out the table
        void write(std::vector<column_descriptor>& columns, const std::vector< std::vector<std::string> >& table,
                   const std::string tag = "");


        // INTERFACE -- SET PROPERTIES

      public:

		    //! Set precision for output content
        void set_precision(unsigned int p) { this->precision = p; }

        //! Get precision
        unsigned int get_precision() const { return(this->precision); }

		    //! Set wrap width
        void set_display_width(unsigned int width) { this->display_width = (width > 1 ? width : 1); }

		    //! Get wrap width
        unsigned int get_display_width() const { return(this->display_width); }

		    //! Set wrap setting
        void set_wrap_status(bool wrap) { this->wrap_width = wrap; }

		    //! Get wrap setting
        bool get_wrap_status() const { return(this->wrap_width); }

        //! Set terminal setting
        void set_terminal_output(bool t) { this->terminal_output = t; }

        //! Get terminal setting
        bool get_terminal_output() const { return(this->terminal_output); }


        // INTERNAL DATA

      protected:

				//! output stream
        std::ostream& stream;

        //! reference to local environment policy class
        local_environment& env;

        //! reference to arguments policy class
        argument_cache& arg_cache;

        //! Precision with which to output numbers
        unsigned int precision;

        //! Display size in columns
        unsigned int display_width;

        //! Whether to wrap - good for display, bad for files
        bool wrap_width;

        //! Whether to treat the output stream as a terminal
        bool terminal_output;

	    };


      // IMPLEMENTATION -- CLASS asciitable


    void asciitable::write(std::vector<column_descriptor>& columns, const std::vector< std::vector<std::string> >& table,
                           const std::string tag)
      {
        assert(columns.size() == table.size());
        if(columns.size() != table.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ASCIITABLE_INCOMPATIBLE_COLUMNS);

        // determine width of each column
        std::vector<size_t> widths(columns.size());

        for(unsigned int i = 0; i < columns.size(); ++i)
          {
            widths[i] = columns[i].get_width();

            for(const std::string& entry : table[i])
              {
                if(entry.length() > widths[i]) widths[i] = entry.length();
              }

            // allow two spaces between columns
            widths[i] += 2;
          }

        // write out tag if one has been given
        if(!tag.empty())
          {
            this->stream << "// " << tag << '\n';
          }

        // output batches of columns, enough to fit across the display
        size_t columns_output = 0;
        while(columns_output < columns.size())
          {
            // work out how many columns we can fit in this batch
            size_t batch_size = 0;
            size_t batch_width = 0;
            size_t column_height = 0;

            bool batch_complete = false;
            while(!batch_complete)
              {
                batch_width += widths[columns_output + batch_size];
                if(table[columns_output + batch_size].size() > column_height) column_height = table[columns_output + batch_size].size();
                ++batch_size;

                if(columns_output + batch_size >= columns.size()) batch_complete = true;
                if(wrap_width && batch_width >= this->display_width) batch_complete = true;
              }

            // remove one column if the batch width is greater than the display width, and we have more
            // than one column in the batch
            if(batch_width > this->display_width && batch_size > 1) --batch_size;

            // we are outputting batch_size columns in this batch, and the largest column height is column_height
            // (not all columns need be the same height, however)

            // insert blank line if this is a continuation
            if(columns_output > 0) this->stream << '\n';

            // write out column headings
            for(size_t i = 0; i < batch_size; ++i)
              {
                columns[columns_output+i].apply_format_title(this->stream, widths[columns_output+i], this->terminal_output,
                                                             this->env, this->arg_cache);
                this->stream << columns[columns_output+i].get_name();
                columns[columns_output+i].deapply_format(this->stream, this->terminal_output, this->env, this->arg_cache);
              }
            this->stream << '\n';

            // write out columns from table
            for(size_t i = 0; i < column_height; ++i)
              {
                for(size_t j = 0; j < batch_size; ++j)
                  {
                    std::string entry = i < table[columns_output+j].size() ? (table[columns_output+j])[i] : "";
                    columns[columns_output+j].apply_format(this->stream, widths[columns_output+j], this->terminal_output,
                                                           this->env, this->arg_cache);
                    this->stream << entry;
                    columns[columns_output+j].deapply_format(this->stream, this->terminal_output, this->env, this->arg_cache);
                  }
                this->stream << '\n';
              }

            columns_output += batch_size;
          }
      }


    template <typename number>
    void asciitable::write(std::string x_name,
                           const std::vector<std::string>& columns, const std::vector<double>& xs,
                           const std::vector< std::vector<number> >& ys, const std::string tag)
	    {
        assert(xs.size() == ys.size());

        // format data into a set of column titles
        std::vector<column_descriptor> table_columns;
        std::vector< std::vector<std::string> > table(columns.size() + 1);

        // build vector of total column names
        table_columns.emplace_back(x_name, column_justify::right);
        for(const std::string& col : columns)
          {
            table_columns.emplace_back(col, column_justify::right);
          }

        size_t current_column = 0;

        // now build table; first, the x column
        for(const double x : xs)
          {
            std::ostringstream entry;
            entry << std::scientific << std::setprecision(this->precision-1) << x;
            table[current_column].push_back(entry.str());
          }
        ++current_column;

        // now aggregate the remaining columns
        for(const std::vector<number>& col : ys)
          {
            for(const double y : col)
              {
                std::ostringstream entry;

                if(std::isnan(y))
                  {
                    entry << "--";
                  }
                else
                  {
                    entry << std::scientific << std::setprecision(this->precision-1) << y;
                  }

                table[current_column].push_back(entry.str());
              }
            ++current_column;
          }

        this->write(table_columns, table, tag);
	    }

  }   // namespace transport

#endif //CPPTRANSPORT_ASCIITABLE_H
