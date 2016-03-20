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

#include "transport-runtime-api/messages.h"
#include "transport-runtime-api/exceptions.h"


constexpr unsigned int DEFAULT_ASCIITABLE_PRECISION     = (12);
constexpr unsigned int DEFAULT_ASCIITABLE_DISPLAY_WIDTH = (80);
constexpr bool         DEFAULT_ASCIITABLE_WRAP_WIDTH    = (true);


namespace transport
  {

    enum class column_justify { left, right };

    class column_descriptor
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        column_descriptor(std::string n, column_justify j=column_justify::left)
          : name(std::move(n)),
            just(j)
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

        //! get justification
        template <typename Stream>
        void justify(Stream& out) const
          {
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
          }


        // INTERNAL DATA

      private:

        //! column name
        std::string name;

        //! justification
        column_justify just;

      };

    class asciitable
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        asciitable(std::ostream& s)
	        : stream(s), precision(DEFAULT_ASCIITABLE_PRECISION),
	          display_width(DEFAULT_ASCIITABLE_DISPLAY_WIDTH),
	          wrap_width(DEFAULT_ASCIITABLE_WRAP_WIDTH)
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

        //! Write columns of text; the data in table should be stored column-wise
        void write(const std::vector<column_descriptor>& columns, const std::vector< std::vector<std::string> >& table,
                   const std::string tag = "");


        // INTERFACE -- SET PROPERTIES

      public:

		    //! Set precision for output content
        void set_precision(unsigned int p);

		    //! Set wrap width
        void set_display_width(unsigned int width);

		    //! Get wrap width
        unsigned int get_display_width();

		    //! Set wrap setting
        void set_wrap_status(bool wrap);

		    //! Get wrap setting
        bool get_wrap_status();


        // INTERNAL DATA

      protected:

				//! Output stream
        std::ostream& stream;

        //! Precision with which to output numbers
        unsigned int precision;

        //! Display size in columns
        unsigned int display_width;

        //! Whether to wrap - good for display, bad for files
        bool wrap_width;

	    };


      // IMPLEMENTATION -- CLASS asciitable


    void asciitable::write(const std::vector<column_descriptor>& columns, const std::vector< std::vector<std::string> >& table,
                           const std::string tag)
      {
        assert(columns.size() == table.size());
        if(columns.size() != table.size()) throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_ASCIITABLE_INCOMPATIBLE_COLUMNS);

        // remember previous status of formatting flags, so we can restore later
        std::ios_base::fmtflags prev_flags = this->stream.flags();

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

            bool batch_complete;
            while(!batch_complete)
              {
                batch_width += widths[columns_output + batch_size];
                if(table[columns_output + batch_size].size() > column_height) column_height = table[columns_output + batch_size].size();
                ++batch_size;

                if(columns_output + batch_size >= columns.size()) batch_complete = true;
                if(wrap_width && batch_width >= this->display_width) batch_complete = true;
              }

            // we are outputting batch_size columns in this batch, and the largest column height is column_height
            // (not all columns need be the same height, however)

            // insert blank line if this is a continuation
            if(columns_output > 0) this->stream << '\n';

            // write out column headings
            for(size_t i = 0; i < batch_size; ++i)
              {
                columns[columns_output + i].justify(this->stream);
                this->stream << std::setw(widths[columns_output+i]) << columns[columns_output+i].get_name();
              }
            this->stream << '\n';

            // write out columns from table
            for(size_t i = 0; i < column_height; ++i)
              {
                for(size_t j = 0; j < batch_size; ++j)
                  {
                    std::string entry = i < table[columns_output+j].size() ? (table[columns_output+j])[i] : "";
                    columns[columns_output+j].justify(this->stream);
                    this->stream << std::setw(widths[columns_output+j]) << entry;
                  }
                this->stream << '\n';
              }

            columns_output += batch_size;
          }

        // reset stream formatting flags
        this->stream.flags(prev_flags);
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


    void asciitable::set_precision(unsigned int p)
	    {
        this->precision = p;
	    }


    void asciitable::set_display_width(unsigned int width)
	    {
        this->display_width = (width > 1 ? width : 1);
	    }


    unsigned int asciitable::get_display_width()
	    {
        return(this->display_width);
	    }


    void asciitable::set_wrap_status(bool wrap)
	    {
        this->wrap_width = wrap;
	    }


    bool asciitable::get_wrap_status()
	    {
        return(this->wrap_width);
	    }


  }   // namespace transport

#endif //CPPTRANSPORT_ASCIITABLE_H
