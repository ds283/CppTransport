//
// Created by David Seery on 21/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_time_data_H_
#define __zeta_time_data_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/derived-products/time-data/time_data_line.h"
#include "transport-runtime-api/derived-products/time-data/general_time_data.h"

#include "transport-runtime-api/utilities/latex_output.h"


namespace transport
  {

    namespace derived_data
      {

        //! zeta twopf time data line
        template <typename number>
        class zeta_twopf_time_data: public general_time_data<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta two-pf time data object
            zeta_twopf_time_data(const twopf_list_task<number>& tk, model<number>* m,
                                 filter::time_filter tfilter,
                                 filter::twopf_kconfig_filter kfilter,
                                 unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_twopf_time_data(serialization_reader* reader, typename repository<number>::task_finder finder);

            virtual ~zeta_twopf_time_data() = default;


            // DERIVE LINES -- implements a 'general_time_data' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe,
                                      plot2d_product<number>& plot, std::list<time_data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


            // LABEL GENERATION

          protected:

            std::string make_label(plot2d_product<number>& plot, typename data_manager<number>::twopf_configuration& config, model<number>* mdl) const;


            // CLONE

            //! self-replicate
            virtual general_time_data<number>* clone() const override { return new twopf_time_data<number>(static_cast<const twopf_time_data<number>&>(*this)); }


            // WRITE TO A STREAM

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! serialize this object
            virtual void serialize(serialization_writer& writer) const override;


            // INTERNAL DATA

          private:

            //! record serial numbers of k-configurations we are using
            std::vector<unsigned int> kconfig_sample_sns;

            //! record precision of labels
            unsigned int precision;
          };


        template <typename number>
        zeta_twopf_time_data<number>::zeta_twopf_time_data(const twopf_list_task<number>& tk, model<number>* m,
                                                           filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
          : precision(prec), general_time_data<number>(tk, m, tfilter)
          {
            assert(m != nullptr);

            if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);

            this->f.filter_twopf_kconfig_sample(kfilter, tk.get_k_list(), kconfig_sample_sns);
          }


        template <typename number>
        zeta_twopf_time_data<number>::zeta_twopf_time_data(serialization_reader* reader, typename repository<number>::task_finder finder)
          : general_time_data<number>(reader, finder)
          {
            assert(reader != nullptr);

            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_READER);

            reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION, precision);

            unsigned int sns = reader->start_array(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

            for(unsigned int i = 0; i < sns; i++)
              {
                reader->start_array_element();

                unsigned int sn;
                reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, sn);

                kconfig_sample_sns.push_back(sn);

                reader->end_array_element();
              }

            reader->end_element(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);
          }


        template <typename number>
        void zeta_twopf_time_data<number>::derive_lines(typename data_manager<number>::datapipe& pipe,
                                                        plot2d_product<number>& plot, std::list<time_data_line<number> >& lines,
                                                        const std::list<std::string>& tags) const
          {
            unsigned int N_fields = this->mdl->get_N_fields();

            // attach datapipe to an output group
            this->attach(pipe, tags);

            std::vector<double> time_axis;
            this->pull_time_axis(pipe, time_axis);

            const std::vector<unsigned int>& time_sample_sns = this->get_time_sample_sns();

            // pull k-configuration information from the database
            typename std::vector< typename data_manager<number>::twopf_configuration > k_values;
            pipe.pull_twopf_kconfig_sample(this->kconfig_sample_sns, k_values);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            std::vector< std::vector<number> > background(time_sample_sns.size());

            for(unsigned int i = 0; i < N_fields; i++)
              {
                std::vector<number> bg_line;
                pipe.pull_background_time_sample(i, time_sample_sns, bg_line);

                assert(bg_line.size() == background.size();
                for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }

            // cache gauge transformation coefficients
            // maybe expensive on memory, but the alternative would be to cache all the components of
            // the twopf, which is worse ... !
            std::vector< std::vector<number> > dN(time_sample_sns.size());
            for(unsigned int j = 0; j < time_sample_sns.size(); j++)
              {
                this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
              }

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
                // time-line for zeta will be stored in 'line_data'
                std::vector<number> line_data;
                line_data.assign(time_sample_sns.size(), 0.0);

                for(unsigned int m = 0; m < 2*N_fields; m++)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; n++)
                      {
                        // pull twopf data for this component
                        std::vector<number> sigma_line;

                        pipe.pull_twopf_time_sample(this->mdl->flatten(m, n), time_sample_sns, this->kconfig_sample_sns[i], sigma_line,
                                                    data_manager<number>::twopf_real);

                        for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                          {
                            line_data[j] += dN[j][m]*dN[j][n]*sigma_line[j];
                          }
                      }
                  }

                time_data_line<number> line = time_data_line<number>(time_axis, line_data, this->make_label(plot, k_values[i], this->mdl));
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        template <typename number>
        std::string zeta_twopf_time_data<number>::make_label(plot2d_product<number>& plot, typename data_manager<number>::twopf_configuration& config, model<number>* mdl) const
          {
            std::ostringstream label;

            assert(mdl != nullptr);
            if(mdl == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_NULL_MODEL);

            if(plot.get_use_LaTeX())
              {
                label << "$";

                label << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL;

                label << "\\;"
                  << __CPP_TRANSPORT_LATEX_K_SYMBOL << "=";
                if(this->get_klabel_meaning() == general_time_data<number>::conventional) label << output_latex_number(config.k_conventional, this->precision);
                else                                                                      label << output_latex_number(config.k_comoving, this->precision);

                label << "$";
              }
            else
              {
                label << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL;

                label << " "
                  << __CPP_TRANSPORT_NONLATEX_K_SYMBOL << "=";
                if(this->get_klabel_meaning() == general_time_data<number>::conventional) label << config.k_conventional;
                else                                                                      label << config.k_comoving;
              }
          }


        template <typename number>
        void zeta_twopf_time_data<number>::write(std::ostream& out)
          {
            out << "  " << __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_LABEL_ZETA_TWOPF << std::endl;

            this->wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_GENERAL_TPLOT_KCONFIG_SN_LABEL " ");

            unsigned int count = 0;
            for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end() && count < __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN; t++)
              {
                std::ostringstream msg;
                msg << (*t);

                this->wrapper.wrap_list_item(out, true, msg.str(), count);
              }
            if(count == __CPP_TRANSPORT_PRODUCT_TDATA_MAX_SN) this->wrapper.wrap_list_item(out, true, "...", count);
            this->wrapper.wrap_newline(out);
          }


        template <typename number>
        void twopf_time_data<number>::serialize(serialization_writer& writer) const
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_TDATA_ZETA_TWOPF));

            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_LABEL_PRECISION, this->precision);

            this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
            for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
              {
                this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
                this->end_element(writer, "arrayelt");
              }
            this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

            this->general_time_data<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif //__zeta_time_data_H_
