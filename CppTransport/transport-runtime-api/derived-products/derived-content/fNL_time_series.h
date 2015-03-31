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

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/fNL_line.h"


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
            fNL_time_series(const threepf_task<number>& tk, filter::time_filter tfilter);

            //! deserialization constructor
            fNL_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

            virtual ~fNL_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

          public:

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


            // CLONE

          public:

            //! self-replicate
            virtual derived_line<number>* clone() const override { return new fNL_time_series<number>(static_cast<const fNL_time_series<number>&>(*this)); }


            // WRITE TO A STREAM

          public:

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a serializable interface

          public:

            //! serialize this object
            virtual void serialize(Json::Value& writer) const override;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor
        // for derived_line<> is not called automatically when constructing time_series<>.
        // We have to call it ourselves.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(const threepf_task<number>& tk, filter::time_filter tfilter)
          : derived_line<number>(tk, time_axis, fNL),
            fNL_line<number>(tk),
            time_series<number>(tk, tfilter)
          {
            if(!tk.is_integrable())
              {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_FNL_TASK_NOT_INTEGRABLE << " '" << tk.get_name() << "'";
                throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
              }
          }


        // the situation is the same with the deserialization constructor; the constructor for
        // derived_line<> is not called automatically when constructing time_series<>, so we
        // have to call it explicitly.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            fNL_line<number>(reader),
            time_series<number>(reader)
          {
          }


        template <typename number>
        void fNL_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                   const std::list<std::string>& tags) const
          {
            // attach datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->time_sample_sns);

		        // set up tag for our fNL template
		        fNL_time_data_tag<number> tag = pipe.new_fNL_time_data_tag(this->type);

		        // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
            const std::vector<number>& line_data = z_handle.lookup_tag(tag);

            std::string latex_label = "$" + this->make_LaTeX_label() + "$";
            std::string nonlatex_label = this->make_non_LaTeX_label();

            data_line<number> line = data_line<number>(time_axis, fNL,
                                                       t_axis, line_data, latex_label, nonlatex_label);

            lines.push_back(line);

            // detach pipe from output group
            this->detach(pipe);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void fNL_time_series<number>::write(std::ostream& out)
          {
            this->derived_line<number>::write(out);
            this->fNL_line<number>::write(out);
            this->time_series<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void fNL_time_series<number>::serialize(Json::Value& writer) const
          {
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES);

            this->derived_line<number>::serialize(writer);
            this->fNL_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //__fNL_time_series_H_
