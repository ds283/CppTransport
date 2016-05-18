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
                force_simple
              };


            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            key_value(local_environment& e, argument_cache& a)
              : env(e),
                arg_cache(a),
                tile(false)
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

            //! write to a stream
            void write(std::ostream& out, print_options opts=print_options::none);

            //! reset
            void reset() { this->db.clear(); this->tile = false; this->title.clear(); }


            // INTERNAL API

          protected:

            //! compute number of columns and column width for current terminal
            void compute_columns(unsigned int& columns, unsigned int& column_width, print_options opts);


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

            //! title string, if used
            std::string title;

          };


        void key_value::write(std::ostream& out, print_options opts)
          {
            unsigned int columns = 1;
            unsigned int column_width = 0;

            // if tiling, compute number of columns and column width
            if(tile) this->compute_columns(columns, column_width, opts);

            bool colour = this->env.has_colour_terminal_support() && this->arg_cache.get_colour_output();
            if(opts == print_options::force_simple) colour = false;

            if(!this->title.empty())
              {
                if(colour) out << ColourCode(ANSI_colour::bold_green);
                out << title;
                if(colour) out << ColourCode(ANSI_colour::normal);
                std::cout << '\n';
              }

            unsigned int current_column = 0;
            for(const std::pair< std::string, std::string >& elt : this->db)
              {
                if(colour) out << ColourCode(ANSI_colour::bold);
                out << elt.first;
                if(colour) out << ColourCode(ANSI_colour::normal);

                // set column width if required by tiling
                if(this->tile)
                  {
                    out << std::left << std::setw(column_width - elt.first.length());
                  }

                out << ": " + elt.second + "  ";

                ++current_column;
                if(current_column >= columns)
                  {
                    out << '\n';
                    current_column = 0;
                  }
              }
            if(current_column != 0) out << '\n';
          }


        void key_value::compute_columns(unsigned int& columns, unsigned int& column_width, print_options opts)
          {
            unsigned int width = CPPTRANSPORT_DEFAULT_TERMINAL_WIDTH;
            if(opts != print_options::force_simple) this->env.detect_terminal_width();

            columns = 1;
            column_width = width;

            // no need to do any work if no key-value pairs
            if(this->db.empty()) return;

            unsigned int max_width = 1;
            for(const std::pair< std::string, std::string >& elt : this->db)
              {
                unsigned int w = elt.first.length() + elt.second.length() + 2 + 2; // +2 for ': ', +2 for space between columns

                if(w > max_width) max_width = w;
              }

            // can we fit more than one column on the terminal? if not, just return
            if(max_width >= width/2) return;

            columns = width / max_width;
            column_width = max_width;
          }


      }

  }



#endif //CPPTRANSPORT_REPORTING_KEY_VALUE_H
