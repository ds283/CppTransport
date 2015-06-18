//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __fNL_time_series_H_
#define __fNL_time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/fNL_line.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
  {

    namespace derived_data
      {

        //! fNL time data line
        template <typename number>
        class fNL_time_series: public time_series<number>, public fNL_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct an fNL time series data object
            fNL_time_series(const fNL_task<number>& tk, SQL_time_config_query tq);

            //! deserialization constructor
            fNL_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

            virtual ~fNL_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

          public:

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

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
            SQL_time_config_query tquery;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor
        // for derived_line<> is not called automatically when constructing time_series<>.
        // We have to call it ourselves.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(const fNL_task<number>& tk, SQL_time_config_query tq)
          : derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }),
            fNL_line<number>(tk),
            time_series<number>(tk),
            tquery(tq)
          {
          }


        // the situation is the same with the deserialization constructor; the constructor for
        // derived_line<> is not called automatically when constructing time_series<>, so we
        // have to call it explicitly.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            fNL_line<number>(reader),
            time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY])
          {
          }


        template <typename number>
        void fNL_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                   const std::list<std::string>& tags) const
          {
            // attach datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->tquery);

		        // set up tag for our fNL template
		        fNL_time_data_tag<number> tag = pipe.new_fNL_time_data_tag(this->type);

		        // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
            const std::vector<number>& line_data = z_handle.lookup_tag(tag);

            data_line<number> line = data_line<number>(group, this->x_type, fNL_value, t_axis, line_data, this->get_LaTeX_label(), this->get_non_LaTeX_label());

            lines.push_back(line);

            // detach pipe from output group
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


#endif //__fNL_time_series_H_
