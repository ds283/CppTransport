//
// Created by David Seery on 21/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_time_series_H_
#define __zeta_time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"
#include "field_time_series.h"

#include "transport-runtime-api/utilities/latex_output.h"


namespace transport
  {

    namespace derived_data
      {

        //! zeta twopf time data line
        template <typename number>
        class zeta_twopf_time_series : public time_series<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta two-pf time data object
            zeta_twopf_time_series(const twopf_list_task<number>& tk, model<number>* m,
                                 filter::time_filter tfilter,
                                 filter::twopf_kconfig_filter kfilter,
                                 unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_twopf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

            virtual ~zeta_twopf_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


            // LABEL GENERATION

          protected:

		        //! Make a LaTeX label for this line
            std::string make_LaTeX_label(const typename data_manager<number>::twopf_configuration& config) const;

		        //! Make a non-LaTeX label for this line
		        std::string make_non_LaTeX_label(const typename data_manager<number>::twopf_configuration& config) const;


            // CLONE

            //! self-replicate
            virtual derived_line<number>* clone() const override { return new zeta_twopf_time_series<number>(static_cast<const zeta_twopf_time_series<number>&>(*this)); }


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

          };


        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(const twopf_list_task<number>& tk, model<number>* m,
                                                           filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
          : time_series<number>(tk, m, tfilter, derived_line<number>::correlation_function, prec)
          {
            assert(m != nullptr);

            if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);

            this->f.filter_twopf_kconfig_sample(kfilter, tk.get_k_list(), kconfig_sample_sns);
          }


        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
          : time_series<number>(reader, finder)
          {
            assert(reader != nullptr);

            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

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
        void zeta_twopf_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                          const std::list<std::string>& tags) const
          {
            unsigned int N_fields = this->mdl->get_N_fields();

            // attach datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

            const std::vector<unsigned int>& time_sample_sns = this->get_time_sample_sns();

		        // set up cache handles
		        typename data_manager<number>::datapipe::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(time_sample_sns);

            // pull k-configuration information from the database
		        typename data_manager<number>::datapipe::twopf_kconfig_tag k_tag = pipe.new_twopf_kconfig_tag();

            const typename std::vector< typename data_manager<number>::twopf_configuration >& k_values = k_handle.lookup_tag(k_tag);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            std::vector< std::vector<number> > background(time_sample_sns.size());

            for(unsigned int i = 0; i < 2*N_fields; i++)
              {
		            typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i);

                const std::vector<number>& bg_line = t_handle.lookup_tag(tag);

                assert(bg_line.size() == background.size());
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
		                    typename data_manager<number>::datapipe::cf_time_data_tag tag =
			                    pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), this->kconfig_sample_sns[i]);

                        // pull twopf data for this component
                        const std::vector<number>& sigma_line = t_handle.lookup_tag(tag);

                        for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                          {
                            line_data[j] += dN[j][m]*dN[j][n]*sigma_line[j];
                          }
                      }
                  }

                data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
                                                           time_axis, line_data,
                                                           this->make_LaTeX_label(k_values[i]), this->make_non_LaTeX_label(k_values[i]));

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        template <typename number>
        std::string zeta_twopf_time_series<number>::make_LaTeX_label(const typename data_manager<number>::twopf_configuration& config) const
	        {
            std::ostringstream label;

            label << std::setprecision(this->precision);

            label << "$";

            label << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL;

            label << "\\;" << __CPP_TRANSPORT_LATEX_K_SYMBOL << "=";
            if(this->get_klabel_meaning() == derived_line<number>::conventional) label << output_latex_number(config.k_conventional, this->precision);
            else label << output_latex_number(config.k_comoving, this->precision);

            label << "$";

            return (label.str());
	        }


        template <typename number>
        std::string zeta_twopf_time_series<number>::make_non_LaTeX_label(const typename data_manager<number>::twopf_configuration& config) const
	        {
            std::ostringstream label;

            label << std::setprecision(this->precision);

            label << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL;

            label << " "
	            << __CPP_TRANSPORT_NONLATEX_K_SYMBOL << "=";
            if(this->get_klabel_meaning() == derived_line<number>::conventional) label << config.k_conventional;
            else label << config.k_comoving;

            return (label.str());
	        }


        template <typename number>
        void zeta_twopf_time_series<number>::write(std::ostream& out)
          {
		        this->time_series<number>::write(out);

            out << "  " << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_LABEL_ZETA_TWOPF << std::endl;

            this->wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_TIME_SERIES_KCONFIG_SN_LABEL " ");

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
        void zeta_twopf_time_series<number>::serialize(serialization_writer& writer) const
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES));

            this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
            for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
              {
                this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
                this->end_element(writer, "arrayelt");
              }
            this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

            this->time_series<number>::serialize(writer);
          }


        //! zeta threepf time data line
        template <typename number>
        class zeta_threepf_time_series : public basic_threepf_time_series<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta_threepf_time_series object
            zeta_threepf_time_series(const threepf_task<number>& tk, model<number>* m,
                                   filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                   unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder);

            virtual ~zeta_threepf_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


            // LABEL GENERATION

          protected:

		        //! Make a LaTeX label for this line
            std::string make_LaTeX_label(const typename data_manager<number>::threepf_configuration& config) const;

		        //! Make a non-LaTeX label for this line
		        std::string make_non_LaTeX_label(const typename data_manager<number>::threepf_configuration& config) const;


            // CLONE

            //! self-replicate
            virtual derived_line<number>* clone() const override { return new zeta_threepf_time_series<number>(static_cast<const zeta_threepf_time_series<number>&>(*this)); }


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

          };


        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(const threepf_task<number>& tk, model<number>* m,
                                                               filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
          : basic_threepf_time_series<number>(tk, m, tfilter, prec)
          {
            assert(m != nullptr);

            if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);

            this->f.filter_threepf_kconfig_sample(kfilter, tk.get_sample(), kconfig_sample_sns);
          }


        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder finder)
          : basic_threepf_time_series<number>(reader, finder)
          {
            assert(reader != nullptr);

            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

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
        void zeta_threepf_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                            const std::list<std::string>& tags) const
          {
            unsigned int N_fields = this->mdl->get_N_fields();

            // attach our datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

            const std::vector<unsigned int>& time_sample_sns = this->get_time_sample_sns();

		        // set up cache handles
		        typename data_manager<number>::datapipe::threepf_kconfig_handle& k_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(time_sample_sns);

            // pull k-configuration information from the database
		        typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();

            const typename std::vector< typename data_manager<number>::threepf_configuration >& k_values = k_handle.lookup_tag(k_tag);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            std::vector< std::vector<number> > background(time_sample_sns.size());

            for(unsigned int i = 0; i < 2*N_fields; i++)
              {
                typename data_manager<number>::datapipe::background_time_data_tag bg_tag = pipe.new_background_time_data_tag(i);

                const std::vector<number>& bg_line = t_handle.lookup_tag(bg_tag);

                assert(bg_line.size() == background.size());
                for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }

            // cache linear gauge transformation coefficients
		        // we can cache these just once because they don't depend on the k-configuration
            std::vector< std::vector<number> > dN(time_sample_sns.size());
            for(unsigned int j = 0; j < time_sample_sns.size(); j++)
              {
                this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
//                this->mdl->compute_deltaN_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
              }

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing 3pf k-configuration " << i << std::endl;

                typename basic_threepf_time_series<number>::extractor k1(1, k_values[i]);
                typename basic_threepf_time_series<number>::extractor k2(2, k_values[i]);
                typename basic_threepf_time_series<number>::extractor k3(3, k_values[i]);

                // cache gauge transformation coefficients
                // these have to be recomputed for each k-configuration, because they are scale- and shape-dependent
                std::vector< std::vector< std::vector<number> > > ddN123(time_sample_sns.size());
                std::vector< std::vector< std::vector<number> > > ddN213(time_sample_sns.size());
                std::vector< std::vector< std::vector<number> > > ddN312(time_sample_sns.size());
                for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                  {
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k1.comoving(), k2.comoving(), k3.comoving(), time_axis[j], ddN123[j]);
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k2.comoving(), k1.comoving(), k3.comoving(), time_axis[j], ddN213[j]);
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k3.comoving(), k1.comoving(), k2.comoving(), time_axis[j], ddN312[j]);
//		                this->mdl->compute_deltaN_xfm_2(this->parent_task->get_params(), background[j], ddN123[j]);
//                    this->mdl->compute_deltaN_xfm_2(this->parent_task->get_params(), background[j], ddN213[j]);
//                    this->mdl->compute_deltaN_xfm_2(this->parent_task->get_params(), background[j], ddN312[j]);
                  }

                // time-line for zeta will be stored in 'line_data'
                std::vector<number> line_data;
                line_data.assign(time_sample_sns.size(), 0.0);

                // linear component of the gauge transformation
                for(unsigned int l = 0; l < 2*N_fields; l++)
                  {
                    for(unsigned int m = 0; m < 2*N_fields; m++)
                      {
                        for(unsigned int n = 0; n < 2*N_fields; n++)
                          {
                            // pull threepf data for this component
		                        typename data_manager<number>::datapipe::cf_time_data_tag tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_threepf, this->mdl->flatten(l,m,n), this->kconfig_sample_sns[i]);

		                        // have to take a copy of the data line, rather than use a reference, because shifting the derivative will modify it in place
                            std::vector<number> threepf_line = t_handle.lookup_tag(tag);

                            // shift field so it represents a derivative correlation function, not a momentum one
                            this->shift_derivatives(pipe, time_sample_sns, threepf_line, time_axis, l, m, n, k_values[i]);

                            for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                              {
                                line_data[j] += dN[j][l]*dN[j][m]*dN[j][n]*threepf_line[j];
                              }
                          }
                      }
                  }

                // quadratic component of the gauge transformation
                for(unsigned int l = 0; l < 2*N_fields; l++)
                  {
                    for(unsigned int m = 0; m < 2*N_fields; m++)
                      {
                        for(unsigned int p = 0; p < 2*N_fields; p++)
                          {
                            for(unsigned int q = 0; q < 2*N_fields; q++)
                              {
                                // the indices are N_lm, N_p, N_q, so the 2pfs we sum over are
                                // sigma_lp(k2)*sigma_mq(k3) etc.

		                            typename data_manager<number>::datapipe::cf_time_data_tag k1_re_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), k1.serial());
                                typename data_manager<number>::datapipe::cf_time_data_tag k1_im_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), k1.serial());

                                typename data_manager<number>::datapipe::cf_time_data_tag k2_re_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), k2.serial());
                                typename data_manager<number>::datapipe::cf_time_data_tag k2_im_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), k2.serial());

                                typename data_manager<number>::datapipe::cf_time_data_tag k2_re_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), k2.serial());
                                typename data_manager<number>::datapipe::cf_time_data_tag k2_im_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), k2.serial());

                                typename data_manager<number>::datapipe::cf_time_data_tag k3_re_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), k3.serial());
                                typename data_manager<number>::datapipe::cf_time_data_tag k3_im_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), k3.serial());

                                const std::vector<number>& k1_re_lp = t_handle.lookup_tag(k1_re_lp_tag);
                                const std::vector<number>& k1_im_lp = t_handle.lookup_tag(k1_im_lp_tag);
                                const std::vector<number>& k2_re_lp = t_handle.lookup_tag(k2_re_lp_tag);
                                const std::vector<number>& k2_im_lp = t_handle.lookup_tag(k2_im_lp_tag);
                                const std::vector<number>& k2_re_mq = t_handle.lookup_tag(k2_re_mq_tag);
                                const std::vector<number>& k2_im_mq = t_handle.lookup_tag(k2_im_mq_tag);
                                const std::vector<number>& k3_re_mq = t_handle.lookup_tag(k3_re_mq_tag);
                                const std::vector<number>& k3_im_mq = t_handle.lookup_tag(k3_im_mq_tag);

                                for(unsigned int j = 0; j < time_sample_sns.size(); j++)
                                  {
                                    line_data[j] += ddN123[j][l][m] * dN[j][p] * dN[j][q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
                                    line_data[j] += ddN213[j][l][m] * dN[j][p] * dN[j][q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
                                    line_data[j] += ddN312[j][l][m] * dN[j][p] * dN[j][q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);
                                  }
                              }
                          }
                      }
                  }

                data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
                                                           time_axis, line_data,
                                                           this->make_LaTeX_label(k_values[i]), this->make_non_LaTeX_label(k_values[i]));

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        template <typename number>
        std::string zeta_threepf_time_series<number>::make_LaTeX_label(const typename data_manager<number>::threepf_configuration& config) const
	        {
            std::ostringstream label;

            label << std::setprecision(this->precision);

            label << "$";

            label << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_LATEX_ZETA_SYMBOL;

            label << "\\;";
            unsigned int count = 0;
            if(this->use_kt_label)
	            {
                label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_KT_SYMBOL << "=";
                if(this->get_klabel_meaning() == derived_line<number>::conventional) label << output_latex_number(config.kt_conventional, this->precision);
                else label << output_latex_number(config.kt_comoving, this->precision);
                count++;
	            }
            if(this->use_alpha_label)
	            {
                label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_ALPHA_SYMBOL << "=" << output_latex_number(config.alpha, this->precision);
                count++;
	            }
            if(this->use_beta_label)
	            {
                label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_BETA_SYMBOL << "=" << output_latex_number(config.beta, this->precision);
                count++;
	            }

            label << "$";

            return (label.str());
	        }


        template <typename number>
        std::string zeta_threepf_time_series<number>::make_non_LaTeX_label(const typename data_manager<number>::threepf_configuration& config) const
	        {
            std::ostringstream label;

            label << std::setprecision(this->precision);

            label << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL << " " << __CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL;

            label << " ";
            unsigned int count = 0;
            if(this->use_kt_label)
	            {
                label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_KT_SYMBOL << "=";
                if(this->get_klabel_meaning() == derived_line<number>::conventional) label << config.kt_conventional;
                else label << config.kt_comoving;
                count++;
	            }
            if(this->use_alpha_label)
	            {
                label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_ALPHA_SYMBOL << "=" << config.alpha;
                count++;
	            }
            if(this->use_beta_label)
	            {
                label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_BETA_SYMBOL << "=" << config.beta;
                count++;
	            }

            return (label.str());
	        }


        template <typename number>
        void zeta_threepf_time_series<number>::write(std::ostream& out)
          {
		        this->basic_threepf_time_series<number>::write(out);

            out << "  " << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_LABEL_ZETA_THREEPF << std::endl;

            this->wrapper.wrap_out(out, "  " __CPP_TRANSPORT_PRODUCT_TIME_SERIES_KCONFIG_SN_LABEL " ");

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
        void zeta_threepf_time_series<number>::serialize(serialization_writer& writer) const
          {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES));

            this->begin_array(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS, this->kconfig_sample_sns.size() == 0);
            for(std::vector<unsigned int>::const_iterator t = this->kconfig_sample_sns.begin(); t != this->kconfig_sample_sns.end(); t++)
              {
                this->begin_node(writer, "arrayelt", false);    // node name ignored for arrays
                this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBER, *t);
                this->end_element(writer, "arrayelt");
              }
            this->end_element(writer, __CPP_TRANSPORT_NODE_PRODUCT_TDATA_K_SERIAL_NUMBERS);

            this->basic_threepf_time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif // __zeta_time_series_H_
