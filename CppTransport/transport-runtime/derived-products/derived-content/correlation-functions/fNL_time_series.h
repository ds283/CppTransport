//
// Created by David Seery on 22/06/2014.
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


#ifndef CPPTRANSPORT_FNL_TIME_SERIES_H
#define CPPTRANSPORT_FNL_TIME_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/fNL_line.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
  {

    namespace derived_data
      {

        //! fNL time data line
        template <typename number=default_number_type>
        class fNL_time_series: public time_series<number>, public fNL_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct an fNL time series data object
            fNL_time_series(const fNL_task<number>& tk, SQL_time_query tq);

            //! deserialization constructor
            fNL_time_series(Json::Value& reader, task_finder<number>& finder);

            virtual ~fNL_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::fNL_time; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }


            // DERIVE LINES -- implements a 'time_series' interface

          public:

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const tag_list& tags, slave_message_buffer& messages) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label() const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label() const;


            // CLONE

          public:

            //! self-replicate
            virtual fNL_time_series<number>* clone() const override { return new fNL_time_series<number>(static_cast<const fNL_time_series<number>&>(*this)); }


            // WRITE TO A STREAM

          public:

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a serializable interface

          public:

            //! serialize this object
            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing x-axis
            SQL_time_query tquery;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor
        // for derived_line<> is not called automatically when constructing time_series<>.
        // We have to call it ourselves.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(const fNL_task<number>& tk, SQL_time_query tq)
          : derived_line<number>(make_derivable_task_set_element(tk,
                                    precomputed_products(false, false, false, false,
                                                         tk.get_template() == bispectrum_template::local,
                                                         tk.get_template() == bispectrum_template::equilateral,
                                                         tk.get_template() == bispectrum_template::orthogonal,
                                                         tk.get_template() == bispectrum_template::DBI)),
                                 axis_class::time, { axis_value::efolds }),
            fNL_line<number>(tk),
            time_series<number>(tk),
            tquery(tq)
          {
          }


        // the situation is the same with the deserialization constructor; the constructor for
        // derived_line<> is not called automatically when constructing time_series<>, so we
        // have to call it explicitly.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            fNL_line<number>(reader),
            time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY])
          {
          }


        template <typename number>
        void fNL_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                   const tag_list& tags, slave_message_buffer& messages) const
          {
            // attach datapipe to a content group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->tquery);

		        // set up tag for our fNL template
		        fNL_time_data_tag<number> tag = pipe.new_fNL_time_data_tag(this->type);

		        // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
            const std::vector<number>& line_data = z_handle.lookup_tag(tag);

            lines.emplace_back(group, this->x_type, value_type::fNL, t_axis, line_data, this->get_LaTeX_label(), this->get_non_LaTeX_label(), messages);

            // detach pipe from content group
            this->detach(pipe);
          }


        template <typename number>
        std::string fNL_time_series<number>::get_LaTeX_label() const
	        {
            std::string label;

            if(this->label_set)
	            {
                label = this->LaTeX_label;
	            }
            else
	            {
                label = "$" + this->make_LaTeX_label() + "$";
	            }

            return(label);
	        }


        template <typename number>
        std::string fNL_time_series<number>::get_non_LaTeX_label() const
	        {
            std::string label;

            if(this->label_set)
	            {
                label = this->non_LaTeX_label;
	            }
            else
	            {
                label = this->make_non_LaTeX_label();
	            }

            return(label);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void fNL_time_series<number>::write(std::ostream& out)
          {
            this->fNL_line<number>::write(out);
            this->time_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void fNL_time_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->fNL_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_FNL_TIME_SERIES_H
