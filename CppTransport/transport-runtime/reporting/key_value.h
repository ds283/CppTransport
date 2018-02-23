//
// Created by David Seery on 22/03/2016.
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

#ifndef CPPTRANSPORT_REPORTING_KEY_VALUE_H
#define CPPTRANSPORT_REPORTING_KEY_VALUE_H


#include <iostream>
#include <iomanip>

#include "transport-runtime/ansi_colour_codes.h"

#include "transport-runtime/manager/environment.h"
#include "transport-runtime/manager/argument_cache.h"


namespace transport
  {

    namespace reporting
      {

        class key_value
          {

            // ASSOCIATED TYPES

          public:

            enum class print_options
              {
                none,
                fixed_width
              };


            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            key_value(local_environment& e, argument_cache& a)
              : env(e),
                arg_cache(a),
                tile(false),
                fix_width(CPPTRANSPORT_DEFAULT_TERMINAL_WIDTH)
              {
              }

            //! destructor is default
            ~key_value() = default;


            // INTERFACE

          public:

            //! store a new key-value pair
            void insert_back(std::string key, std::string value) { db.emplace_back(std::make_pair(std::move(key), std::move(value))); }

            //! set tiling policy
            void set_tiling(bool t) { this->tile = t; }

            //! get tiling policy
            bool get_tiling() const { return(this->tile); }

            //! set title
            void set_title(std::string t) { this->title = std::move(t); }
            
            //! set fixed width (not used unless the appropriate option given to write())
            void set_fixed_width(size_t w) { this->fix_width = w; }

            //! write to a stream
            void write(std::ostream& out, print_options opts=print_options::none);

            //! reset
            void reset() { this->db.clear(); this->tile = false; this->title.clear(); }


            // INTERNAL API

          protected:

            //! compute number of columns and column width for current terminal
            //! returns std::pair representing (number of columns per batch, width of column)
            std::pair<unsigned int, unsigned int> compute_columns(print_options opts);


            // INTERNAL DATA

          private:

            //! local environment policy class
            local_environment& env;

            //! argument policy class
            argument_cache& arg_cache;


            // KEY, VALUE PAIRS

            typedef std::list< std::pair< std::string, std::string > > key_value_db;

            //! database of key-value pairs, stored in order
            key_value_db db;


            // BEHAVIOUR

            //! tile pairs in columns, or just produce a single list?
            bool tile;
            
            //! fixed width, if used
            size_t fix_width;

            //! title string, if used
            std::string title;

          };


        void key_value::write(std::ostream& out, print_options opts)
          {
            unsigned int columns_per_batch = 1;
            unsigned int column_width = 0;
            
            // if tiling, compute number of columns and column width
            if(this->tile)
              {
                std::tie(columns_per_batch, column_width) = this->compute_columns(opts);
              }

            bool colour = this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output();
            if(opts == print_options::fixed_width) colour = false;    // assume fixed width means we don't know anything about the terminal's properties

            if(!this->title.empty())
              {
                if(colour) out << ColourCode(ANSI_colour::bold_green);
                out << title;
                if(colour) out << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
              }

            unsigned int current_column = 0;
            unsigned int printed_columns = 0;
            const unsigned int total_columns = static_cast<unsigned int>(this->db.size());

            for(const auto& elt : this->db)
              {
                if(colour) out << ColourCode(ANSI_colour::bold);
                out << elt.first;
                if(colour) out << ColourCode(ANSI_colour::normal);

                // set column width if required by tiling, provided we are not the last
                // column
                // If we are the last column, omit the column width specifier to avoid
                // the end of line being padded with spaces
                ++current_column;
                ++printed_columns;
                if(this->tile && current_column < columns_per_batch && printed_columns < total_columns)
                  {
                    out << std::left << std::setw(column_width - static_cast<unsigned int>(elt.first.length()));
                  }

                // emit remainder of field, omitting spacing "  " between columns if we are the
                // last one in a batch, or the last one overall
                // notice we have to emit everything as a *single* string, because the std::setw()
                // field applies only to the next atomic output that appears on the stream
                if(current_column < columns_per_batch && printed_columns < total_columns)
                  {
                    out << ": " + elt.second + "  ";
                  }
                else
                  {
                    out << ": " + elt.second;
                  }

                // emit newline if we are the end of a batch
                if(current_column >= columns_per_batch)
                  {
                    out << '\n';
                    current_column = 0;
                  }
              }
            
            // emit newline if there was some output after the last one
            if(current_column != 0) out << '\n';
          }


        std::pair<unsigned int, unsigned int> key_value::compute_columns(print_options opts)
          {
            size_t width =
              (opts == print_options::fixed_width ? this->fix_width
                                                  : this->env.detect_terminal_width(this->arg_cache.get_default_terminal_width()));

            // set up default return values; these will be overwritten later if we use
            // a multicolumn configuration
            unsigned int columns_per_batch = 1;
            unsigned int column_width = static_cast<unsigned int>(width);

            // no need to do any work if no key-value pairs
            if(this->db.empty()) return std::make_pair(columns_per_batch, column_width);

            unsigned int max_width = 1;
            for(const auto& elt : this->db)
              {
                size_t w = elt.first.length() + elt.second.length() + 2 + 2; // +2 for ': ', +2 for space between columns
                
                if(w > max_width) max_width = static_cast<unsigned int>(w);
              }

            // can we fit more than one column on the terminal? if not, just return
            if(max_width >= width/2) return std::make_pair(columns_per_batch, column_width);

            columns_per_batch = static_cast<unsigned int>(width) / max_width;
            column_width = max_width;
            
            return std::make_pair(columns_per_batch, column_width);
          }


      }

  }

#endif //CPPTRANSPORT_REPORTING_KEY_VALUE_H
