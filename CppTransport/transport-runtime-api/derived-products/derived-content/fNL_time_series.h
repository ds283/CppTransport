//
// Created by David Seery on 22/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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
            fNL_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

            virtual ~fNL_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

          public:

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
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
            virtual void serialize(serialization_writer& writer) const override;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor
        // for derived_line<> is not called automatically when constructing time_series<>.
        // We have to call it ourselves.
        template <typename number>
        fNL_time_series<number>::fNL_time_series(const threepf_task<number>& tk, filter::time_filter tfilter)
          : derived_line<number>(tk, derived_line<number>::time_series, derived_line<number>::fNL),
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
        fNL_time_series<number>::fNL_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            fNL_line<number>(reader),
            time_series<number>(reader)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
          }


        template <typename number>
        void fNL_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                   const std::list<std::string>& tags) const
          {
            // attach datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

            threepf_task<number>* tk = dynamic_cast<threepf_task<number>*>(this->parent_task);
            const std::vector<threepf_kconfig>& raw_kconfigs = tk->get_threepf_kconfig_list();

            std::vector<unsigned int> kserials(raw_kconfigs.size());
            for(unsigned int i = 0; i < raw_kconfigs.size(); i++)
              {
                kserials[i] = raw_kconfigs[i].serial;
              }

            // set up cache handles
            typename data_manager<number>::datapipe::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(kserials);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

            // pull 3pf k-configuration information from the database
            typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();

            const typename std::vector< typename data_manager<number>::threepf_configuration >& k_values = kc_handle.lookup_tag(k_tag);

            // time-line will be stored in 'line_data'
            std::vector<number> BT_line;  // will hold <B, T>
            std::vector<number> TT_line;  // will hold <T, T>
            BT_line.assign(this->time_sample_sns.size(), 0.0);
		        TT_line.assign(this->time_sample_sns.size(), 0.0);

            for(unsigned int i = 0; i < k_values.size(); i++)
              {
								typename data_manager<number>::datapipe::zeta_threepf_time_data_tag bsp_tag = pipe.new_zeta_threepf_time_data_tag(k_values[i]);

                // pull bispectrum information for this triangle
                const std::vector<number>& bispectrum = t_handle.lookup_tag(bsp_tag);

                typename data_manager<number>::twopf_configuration k1;
                typename data_manager<number>::twopf_configuration k2;
                typename data_manager<number>::twopf_configuration k3;

                k1.serial         = k_values[i].k1_serial;
                k1.k_comoving     = k_values[i].k1_comoving;
                k1.k_conventional = k_values[i].k1_conventional;

                k2.serial         = k_values[i].k2_serial;
                k2.k_comoving     = k_values[i].k2_comoving;
                k2.k_conventional = k_values[i].k2_conventional;

                k3.serial         = k_values[i].k3_serial;
                k3.k_comoving     = k_values[i].k3_comoving;
                k3.k_conventional = k_values[i].k3_conventional;

		            typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k1_tag = pipe.new_zeta_twopf_time_data_tag(k1);
                typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k2_tag = pipe.new_zeta_twopf_time_data_tag(k2);
                typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k3_tag = pipe.new_zeta_twopf_time_data_tag(k3);

                const std::vector<number>& twopf_k1 = t_handle.lookup_tag(k1_tag);
                const std::vector<number>& twopf_k2 = t_handle.lookup_tag(k2_tag);
                const std::vector<number>& twopf_k3 = t_handle.lookup_tag(k3_tag);

                std::vector<number> S_bispectrum;
                std::vector<number> S_template;
                this->shape_function(bispectrum, twopf_k1, twopf_k2, twopf_k3, S_bispectrum);
		            this->shape_function(twopf_k1, twopf_k2, twopf_k3, S_template);

                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    BT_line[j] += S_bispectrum[j]*S_template[j];
		                TT_line[j] += S_template[j]  *S_template[j];
                  }
              }

            std::vector<number> line_data(this->time_sample_sns.size());
            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
              {
                line_data[j] = (5.0/3.0) * BT_line[j]/TT_line[j];
              }

            std::string latex_label = "$" + this->make_LaTeX_label() + "$";
            std::string nonlatex_label = this->make_non_LaTeX_label();

            data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::fNL,
                                                       time_axis, line_data, latex_label, nonlatex_label);

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
        void fNL_time_series<number>::serialize(serialization_writer& writer) const
          {
            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_FNL_TIME_SERIES));

            this->derived_line<number>::serialize(writer);
            this->fNL_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //__fNL_time_series_H_
