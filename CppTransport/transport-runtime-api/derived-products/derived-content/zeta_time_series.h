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
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_threepf_line.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_reduced_bispectrum_line.h"


namespace transport
  {

    namespace derived_data
      {

        //! zeta twopf time data line
        template <typename number>
        class zeta_twopf_time_series : public time_series<number>, public zeta_twopf_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta two-pf time data object
            zeta_twopf_time_series(const twopf_list_task<number>& tk,
                                   filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
                                   unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_twopf_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

            virtual ~zeta_twopf_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


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

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(const twopf_list_task<number>& tk,
                                                               filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
	        : derived_line<number>(tk, derived_line<number>::time_series, derived_line<number>::correlation_function, prec),
	          zeta_twopf_line<number>(tk, kfilter),
	          time_series<number>(tk, tfilter)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            zeta_twopf_line<number>(reader),
            time_series<number>(reader)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
          }


        template <typename number>
        void zeta_twopf_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                          const std::list<std::string>& tags) const
          {
            unsigned int N_fields = this->mdl->get_N_fields();

            // attach datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename data_manager<number>::datapipe::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

            // pull k-configuration information from the database
		        typename data_manager<number>::datapipe::twopf_kconfig_tag k_tag = pipe.new_twopf_kconfig_tag();

            const typename std::vector< typename data_manager<number>::twopf_configuration >& k_values = k_handle.lookup_tag(k_tag);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            std::vector< std::vector<number> > background(this->time_sample_sns.size());

            for(unsigned int i = 0; i < 2*N_fields; i++)
              {
		            typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i);

                const std::vector<number>& bg_line = t_handle.lookup_tag(tag);

                assert(bg_line.size() == background.size());
                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }

            // cache gauge transformation coefficients
            // maybe expensive on memory, but the alternative would be to cache all the components of
            // the twopf, which is worse ... !
            std::vector< std::vector<number> > dN(this->time_sample_sns.size());
            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
              {
                this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
//                this->mdl->compute_deltaN_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
              }

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
                // time-line for zeta will be stored in 'line_data'
                std::vector<number> line_data;
                line_data.assign(this->time_sample_sns.size(), 0.0);

                std::vector<number> small;
                std::vector<number> large;
		            small.assign(this->time_sample_sns.size(), +DBL_MAX);
		            large.assign(this->time_sample_sns.size(), -DBL_MAX);

                for(unsigned int m = 0; m < 2*N_fields; m++)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; n++)
                      {
		                    typename data_manager<number>::datapipe::cf_time_data_tag tag =
			                    pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), this->kconfig_sample_sns[i]);

                        // pull twopf data for this component
                        const std::vector<number>& sigma_line = t_handle.lookup_tag(tag);

                        for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                          {
		                        number component = dN[j][m]*dN[j][n]*sigma_line[j];

														if(fabs(component) > large[j]) large[j] = fabs(component);
		                        if(fabs(component) < small[j]) small[j] = fabs(component);
                            line_data[j] += component;
                          }
                      }
                  }

		            number global_small = +DBL_MAX;
		            number global_large = -DBL_MAX;
		            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
			            {
				            number large_fraction = fabs(large[j]/line_data[j]);
				            number small_fraction = fabs(small[j]/line_data[j]);

				            if(large_fraction > global_large) global_large = large_fraction;
				            if(small_fraction < global_small) global_small = small_fraction;
			            }

                std::ostringstream msg;
		            msg << std::setprecision(2) << "-- zeta twopf time series: sample " << i << ": smallest intermediate = " << global_small*100.0 << "%, largest intermediate = " << global_large*100.0 << "%";
                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << msg.str();
                std::cout << msg.str() << std::endl;

                std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(k_values[i]) + "$";
                std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(k_values[i]);

                data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
                                                           time_axis, line_data, latex_label, nonlatex_label);

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_twopf_time_series<number>::write(std::ostream& out)
          {
            this->derived_line<number>::write(out);
            this->zeta_twopf_line<number>::write(out);
		        this->time_series<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_twopf_time_series<number>::serialize(serialization_writer& writer) const
          {
            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES));

            this->derived_line<number>::serialize(writer);
            this->zeta_twopf_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


        //! zeta threepf time data line
        template <typename number>
        class zeta_threepf_time_series: public time_series<number>, public zeta_threepf_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta_threepf_time_series object
            zeta_threepf_time_series(const threepf_task<number>& tk, filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                     unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

            virtual ~zeta_threepf_time_series() = default;


            // DERIVE LINES -- implements a 'derived_line' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


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

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(const threepf_task<number>& tk,
                                                                   filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
          : derived_line<number>(tk, derived_line<number>::time_series, derived_line<number>::correlation_function, prec),
            zeta_threepf_line<number>(tk, kfilter),
            time_series<number>(tk, tfilter)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            zeta_threepf_line<number>(reader),
            time_series<number>(reader)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
          }


        template <typename number>
        void zeta_threepf_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                            const std::list<std::string>& tags) const
          {
            unsigned int N_fields = this->mdl->get_N_fields();

            // attach our datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename data_manager<number>::datapipe::threepf_kconfig_handle& k_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

            // pull k-configuration information from the database
		        typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();

            const typename std::vector< typename data_manager<number>::threepf_configuration >& k_values = k_handle.lookup_tag(k_tag);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            std::vector< std::vector<number> > background(this->time_sample_sns.size());

            for(unsigned int i = 0; i < 2*N_fields; i++)
              {
                typename data_manager<number>::datapipe::background_time_data_tag bg_tag = pipe.new_background_time_data_tag(i);

                const std::vector<number>& bg_line = t_handle.lookup_tag(bg_tag);

                assert(bg_line.size() == background.size());
                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }

            // cache linear gauge transformation coefficients
		        // we can cache these just once because they don't depend on the k-configuration
            std::vector< std::vector<number> > dN(this->time_sample_sns.size());
            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
              {
                this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
//                this->mdl->compute_deltaN_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
              }

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing 3pf k-configuration " << i << std::endl;

                // cache gauge transformation coefficients
                // these have to be recomputed for each k-configuration, because they are time and shape-dependent
                std::vector< std::vector< std::vector<number> > > ddN123(this->time_sample_sns.size());
                std::vector< std::vector< std::vector<number> > > ddN213(this->time_sample_sns.size());
                std::vector< std::vector< std::vector<number> > > ddN312(this->time_sample_sns.size());
                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k_values[i].k1_comoving, k_values[i].k2_comoving, k_values[i].k3_comoving, time_axis[j], ddN123[j]);
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k_values[i].k2_comoving, k_values[i].k1_comoving, k_values[i].k3_comoving, time_axis[j], ddN213[j]);
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k_values[i].k3_comoving, k_values[i].k1_comoving, k_values[i].k2_comoving, time_axis[j], ddN312[j]);
//		                this->mdl->compute_deltaN_xfm_2(this->parent_task->get_params(), background[j], ddN123[j]);
//                    this->mdl->compute_deltaN_xfm_2(this->parent_task->get_params(), background[j], ddN213[j]);
//                    this->mdl->compute_deltaN_xfm_2(this->parent_task->get_params(), background[j], ddN312[j]);
                  }

                // time-line for zeta will be stored in 'line_data'
                std::vector<number> line_data;
                line_data.assign(this->time_sample_sns.size(), 0.0);

                std::vector<number> small;
                std::vector<number> large;
                small.assign(this->time_sample_sns.size(), +DBL_MAX);
                large.assign(this->time_sample_sns.size(), -DBL_MAX);

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
                            this->shifter.shift(this->parent_task, this->mdl, pipe, this->time_sample_sns, threepf_line, time_axis, l, m, n, k_values[i]);

                            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                              {
		                            number component = dN[j][l]*dN[j][m]*dN[j][n]*threepf_line[j];

                                if(fabs(component) > large[j])  large[j]  = fabs(component);
                                if(fabs(component) < small[j]) small[j] = fabs(component);
                                line_data[j] += component;
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

		                            typename data_manager<number>::datapipe::cf_time_data_tag k1_re_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), k_values[i].k1_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k1_im_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), k_values[i].k1_serial);

                                typename data_manager<number>::datapipe::cf_time_data_tag k2_re_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), k_values[i].k2_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k2_im_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), k_values[i].k2_serial);

                                typename data_manager<number>::datapipe::cf_time_data_tag k2_re_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), k_values[i].k2_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k2_im_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), k_values[i].k2_serial);

                                typename data_manager<number>::datapipe::cf_time_data_tag k3_re_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), k_values[i].k3_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k3_im_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), k_values[i].k3_serial);

                                const std::vector<number>& k1_re_lp = t_handle.lookup_tag(k1_re_lp_tag);
                                const std::vector<number>& k1_im_lp = t_handle.lookup_tag(k1_im_lp_tag);
                                const std::vector<number>& k2_re_lp = t_handle.lookup_tag(k2_re_lp_tag);
                                const std::vector<number>& k2_im_lp = t_handle.lookup_tag(k2_im_lp_tag);
                                const std::vector<number>& k2_re_mq = t_handle.lookup_tag(k2_re_mq_tag);
                                const std::vector<number>& k2_im_mq = t_handle.lookup_tag(k2_im_mq_tag);
                                const std::vector<number>& k3_re_mq = t_handle.lookup_tag(k3_re_mq_tag);
                                const std::vector<number>& k3_im_mq = t_handle.lookup_tag(k3_im_mq_tag);

                                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                                  {
		                                number component1 = ddN123[j][l][m] * dN[j][p] * dN[j][q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
		                                number component2 = ddN213[j][l][m] * dN[j][p] * dN[j][q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
		                                number component3 = ddN312[j][l][m] * dN[j][p] * dN[j][q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);

                                    if(fabs(component1) > large[j]) large[j] = fabs(component1);
                                    if(fabs(component1) < small[j]) small[j] = fabs(component1);
                                    if(fabs(component2) > large[j]) large[j] = fabs(component2);
                                    if(fabs(component2) < small[j]) small[j] = fabs(component2);
                                    if(fabs(component3) > large[j]) large[j] = fabs(component3);
                                    if(fabs(component3) < small[j]) small[j] = fabs(component3);

                                    line_data[j] += component1;
                                    line_data[j] += component2;
                                    line_data[j] += component3;
                                  }
                              }
                          }
                      }
                  }

                number global_small = +DBL_MAX;
                number global_large = -DBL_MAX;
                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
	                {
                    number large_fraction = fabs(large[j]/line_data[j]);
                    number small_fraction = fabs(small[j]/line_data[j]);

                    if(large_fraction > global_large) global_large = large_fraction;
                    if(small_fraction < global_small) global_small = small_fraction;
	                }

                std::ostringstream msg;
                msg << std::setprecision(2) << "-- zeta threepf time series: sample " << i << ": smallest intermediate = " << global_small*100.0 << "%, largest intermediate = " << global_large*100.0 << "%";
                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << msg.str();
                std::cout << msg.str() << std::endl;

                std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(k_values[i], this->use_kt_label, this->use_alpha_label, this->use_beta_label) + "$";
                std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(k_values[i], this->use_kt_label, this->use_alpha_label, this->use_beta_label);

                data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
                                                           time_axis, line_data, latex_label, nonlatex_label);

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_threepf_time_series<number>::write(std::ostream& out)
          {
            this->derived_line<number>::write(out);
            this->zeta_threepf_line<number>::write(out);
            this->time_series<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_threepf_time_series<number>::serialize(serialization_writer& writer) const
          {
            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES));

            this->derived_line<number>::serialize(writer);
            this->zeta_threepf_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


        //! zeta reduced bispectrum time data line
        template <typename number>
        class zeta_reduced_bispectrum_time_series: public time_series<number>, public zeta_reduced_bispectrum_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta_reduced_bispectrum_time_series object
            zeta_reduced_bispectrum_time_series(const threepf_task<number>& tk,
                                                filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                                unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_reduced_bispectrum_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

            virtual ~zeta_reduced_bispectrum_time_series() = default;


            // DERIVE LINES -- implements a 'derived line' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


            // CLONE

            //! self-replicate
            virtual derived_line<number>* clone() const override { return new zeta_reduced_bispectrum_time_series<number>(static_cast<const zeta_reduced_bispectrum_time_series<number>&>(*this)); }


            // WRITE TO A STREAM

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! serialize this object
            virtual void serialize(serialization_writer& writer) const override;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_reduced_bispectrum_time_series<number>::zeta_reduced_bispectrum_time_series(const threepf_task<number>& tk,
                                                                                         filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                                                                         unsigned int prec)
          : derived_line<number>(tk, derived_line<number>::time_series, derived_line<number>::fNL, prec),
            zeta_reduced_bispectrum_line<number>(tk, kfilter),
            time_series<number>(tk, tfilter)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_reduced_bispectrum_time_series<number>::zeta_reduced_bispectrum_time_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            zeta_reduced_bispectrum_line<number>(reader),
            time_series<number>(reader)
          {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
          }


        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                                       const std::list<std::string>& tags) const
          {
            unsigned int N_fields = this->mdl->get_N_fields();

            // attach our datapipe to an output group
            this->attach(pipe, tags);

            const std::vector<double> time_axis = this->pull_time_axis(pipe);

            // set up cache handles
            typename data_manager<number>::datapipe::threepf_kconfig_handle& k_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
            typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

            // pull k-configuration information from the database
            typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();

            const typename std::vector< typename data_manager<number>::threepf_configuration >& k_values = k_handle.lookup_tag(k_tag);

            // pull background data for the time_sample we are using,
            // and slice it up by time in an array 'background'
            std::vector< std::vector<number> > background(this->time_sample_sns.size());

            for(unsigned int i = 0; i < 2*N_fields; i++)
              {
                typename data_manager<number>::datapipe::background_time_data_tag bg_tag = pipe.new_background_time_data_tag(i);

                const std::vector<number>& bg_line = t_handle.lookup_tag(bg_tag);

                assert(bg_line.size() == background.size());
                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    background[j].push_back(bg_line[j]);
                  }
              }

            // cache linear gauge transformation coefficients
            // we can cache these just once because they don't depend on the k-configuration
            std::vector< std::vector<number> > dN(this->time_sample_sns.size());
            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
              {
                this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[j], dN[j]);
              }

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
                BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing 3pf k-configuration " << i << std::endl;

                // cache gauge transformation coefficients
                // these have to be recomputed for each k-configuration, because they are scale- and shape-dependent
                std::vector< std::vector< std::vector<number> > > ddN123(this->time_sample_sns.size());
                std::vector< std::vector< std::vector<number> > > ddN213(this->time_sample_sns.size());
                std::vector< std::vector< std::vector<number> > > ddN312(this->time_sample_sns.size());
                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k_values[i].k1_comoving, k_values[i].k2_comoving, k_values[i].k3_comoving, time_axis[j], ddN123[j]);
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k_values[i].k2_comoving, k_values[i].k1_comoving, k_values[i].k3_comoving, time_axis[j], ddN213[j]);
                    this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[j], k_values[i].k3_comoving, k_values[i].k1_comoving, k_values[i].k2_comoving, time_axis[j], ddN312[j]);
                  }

                // time-line for the reduced bispectrum will be stored in 'line_data'
                std::vector<number> line_data;
                line_data.assign(this->time_sample_sns.size(), 0.0);

                // FIRST, BUILD THE BISPECTRUM ITSELF

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
                            this->shifter.shift(this->parent_task, this->mdl, pipe, this->time_sample_sns, threepf_line, time_axis, l, m, n, k_values[i]);

                            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
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

                                typename data_manager<number>::datapipe::cf_time_data_tag k1_re_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), k_values[i].k1_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k1_im_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), k_values[i].k1_serial);

                                typename data_manager<number>::datapipe::cf_time_data_tag k2_re_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), k_values[i].k2_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k2_im_lp_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), k_values[i].k2_serial);

                                typename data_manager<number>::datapipe::cf_time_data_tag k2_re_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), k_values[i].k2_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k2_im_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), k_values[i].k2_serial);

                                typename data_manager<number>::datapipe::cf_time_data_tag k3_re_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), k_values[i].k3_serial);
                                typename data_manager<number>::datapipe::cf_time_data_tag k3_im_mq_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), k_values[i].k3_serial);

                                const std::vector<number>& k1_re_lp = t_handle.lookup_tag(k1_re_lp_tag);
                                const std::vector<number>& k1_im_lp = t_handle.lookup_tag(k1_im_lp_tag);
                                const std::vector<number>& k2_re_lp = t_handle.lookup_tag(k2_re_lp_tag);
                                const std::vector<number>& k2_im_lp = t_handle.lookup_tag(k2_im_lp_tag);
                                const std::vector<number>& k2_re_mq = t_handle.lookup_tag(k2_re_mq_tag);
                                const std::vector<number>& k2_im_mq = t_handle.lookup_tag(k2_im_mq_tag);
                                const std::vector<number>& k3_re_mq = t_handle.lookup_tag(k3_re_mq_tag);
                                const std::vector<number>& k3_im_mq = t_handle.lookup_tag(k3_im_mq_tag);

                                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                                  {
                                    line_data[j] += ddN123[j][l][m] * dN[j][p] * dN[j][q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
                                    line_data[j] += ddN213[j][l][m] * dN[j][p] * dN[j][q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
                                    line_data[j] += ddN312[j][l][m] * dN[j][p] * dN[j][q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);
                                  }
                              }
                          }
                      }
                  }

                // SECOND, CONSTRUCT THE SPECTRA

                std::vector<number> twopf_k1_data;
                std::vector<number> twopf_k2_data;
                std::vector<number> twopf_k3_data;
                twopf_k1_data.assign(this->time_sample_sns.size(), 0.0);
                twopf_k2_data.assign(this->time_sample_sns.size(), 0.0);
                twopf_k3_data.assign(this->time_sample_sns.size(), 0.0);

                for(unsigned int m = 0; m < 2*N_fields; m++)
                  {
                    for(unsigned int n = 0; n < 2*N_fields; n++)
                      {
                        typename data_manager<number>::datapipe::cf_time_data_tag k1_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), k_values[i].k1_serial);
                        typename data_manager<number>::datapipe::cf_time_data_tag k2_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), k_values[i].k2_serial);
                        typename data_manager<number>::datapipe::cf_time_data_tag k3_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), k_values[i].k3_serial);

                        // pull twopf data for each k-number
                        const std::vector<number>& k1_line = t_handle.lookup_tag(k1_tag);
                        const std::vector<number>& k2_line = t_handle.lookup_tag(k2_tag);
                        const std::vector<number>& k3_line = t_handle.lookup_tag(k3_tag);

                        for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                          {
                            twopf_k1_data[j] += dN[j][m]*dN[j][n]*k1_line[j];
                            twopf_k2_data[j] += dN[j][m]*dN[j][n]*k2_line[j];
                            twopf_k3_data[j] += dN[j][m]*dN[j][n]*k3_line[j];
                          }
                      }
                  }

                // THIRD, CONSTRUCT THE REDUCED BISPECTRUM

                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
                  {
                    number form_factor = (6.0/5.0) * ( twopf_k1_data[j]*twopf_k2_data[j] + twopf_k1_data[j]*twopf_k3_data[j] + twopf_k2_data[j]*twopf_k3_data[j] );

                    line_data[j] = line_data[j] / form_factor;
                  }

                std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(k_values[i], this->use_kt_label, this->use_alpha_label, this->use_beta_label) + "$";
                std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(k_values[i], this->use_kt_label, this->use_alpha_label, this->use_beta_label);

                data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::fNL,
                                                           time_axis, line_data, latex_label, nonlatex_label);

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::write(std::ostream& out)
          {
            this->derived_line<number>::write(out);
            this->zeta_reduced_bispectrum_line<number>::write(out);
            this->time_series<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::serialize(serialization_writer& writer) const
          {
            writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES));

            this->derived_line<number>::serialize(writer);
            this->zeta_reduced_bispectrum_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif // __zeta_time_series_H_
