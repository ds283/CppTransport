//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_wavenumber_series_H_
#define __zeta_wavenumber_series_H_

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_threepf_line.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_reduced_bispectrum_line.h"


namespace transport
	{

		namespace derived_data
			{

				//! zeta twopf wavenumber-data line
				template <typename number>
		    class zeta_twopf_wavenumber_series: public wavenumber_series<number>, public zeta_twopf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a zeta twopf wavenumber data object
				    zeta_twopf_wavenumber_series(const twopf_list_task<number>& tk, model<number>* m,
				                                 filter::time_filter tfilter,
				                                 filter::twopf_kconfig_filter kfilter,
				                                 unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    // deserialization constructor
				    zeta_twopf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

				    virtual ~zeta_twopf_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'time_series' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;


		        // CLONE

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new zeta_twopf_wavenumber_series<number>(static_cast<const zeta_twopf_wavenumber_series<number>&>(*this)); }


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
		    zeta_twopf_wavenumber_series<number>::zeta_twopf_wavenumber_series(const twopf_list_task<number>& tk, model<number>* m,
		                                                                       filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, m, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
			      zeta_twopf_line<number>(tk, m, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
		        assert(m != nullptr);
		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_twopf_wavenumber_series<number>::zeta_twopf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      zeta_twopf_line<number>(reader),
			      wavenumber_series<number>(reader)
			    {
		        assert(reader != nullptr);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
			    }


		    template <typename number>
		    void zeta_twopf_wavenumber_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
		                                                            const std::list<std::string>& tags) const
			    {
		        unsigned int N_fields = this->mdl->get_N_fields();

		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename data_manager<number>::datapipe::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

		        // pull time-configuration information from the database
		        typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();
		        const std::vector<double>& t_values = tc_handle.lookup_tag(t_tag);

		        // pull the background field configuration for each time sample point
		        std::vector< std::vector<number> > background(this->time_sample_sns.size());
				    for(unsigned int i = 0; i < 2*N_fields; i++)
					    {
						    typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i);
						    const std::vector<number>& bg_line = t_handle.lookup_tag(tag);
						    assert(bg_line.size() == this->time_sample_sns.size());

						    for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
							    {
								    background[j].push_back(bg_line[j]);
							    }
					    }

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
				        // cache gauge transformation matrices for this time sample
		            std::vector<number> dN;
				        this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[i], dN);

				        // information for zeta will be stored in 'line_data'
		            std::vector<number> line_data;
				        line_data.assign(this->kconfig_sample_sns.size(), 0.0);

		            for(unsigned int m = 0; m < 2*N_fields; m++)
			            {
		                for(unsigned int n = 0; n < 2*N_fields; n++)
			                {
	                      typename data_manager<number>::datapipe::cf_kconfig_data_tag tag =
	                        pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), this->time_sample_sns[i]);

	                      const std::vector<number>& sigma_line = k_handle.lookup_tag(tag);

				                for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
					                {
						                line_data[j] += dN[m]*dN[n]*sigma_line[j];
					                }
			                }
			            }

		            std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
		            std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(t_values[i]);

		            data_line<number> line = data_line<number>(data_line<number>::wavenumber_series, data_line<number>::correlation_function,
		                                                       wavenumber_axis, line_data, latex_label, nonlatex_label);

		            lines.push_back(line);
			        }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_twopf_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->derived_line<number>::write(out);
		        this->zeta_twopf_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_twopf_wavenumber_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES));

		        this->derived_line<number>::serialize(writer);
		        this->zeta_twopf_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }


				//! zeta threepf wavenumber data line
				template <typename number>
				class zeta_threepf_wavenumber_series: public wavenumber_series<number>, public zeta_threepf_line<number>
					{

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! construct a zeta threepf wavenumber-series object
						zeta_threepf_wavenumber_series(const threepf_task<number>& tk, model<number>* m,
						filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
						unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! deserialization constructor
						zeta_threepf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

						virtual ~zeta_threepf_wavenumber_series() = default;


				    // DERIVE LINES -- implements a 'derived_line' interface

				    //! generate data lines for plotting
				    virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
				                              const std::list<std::string>& tags) const override;


				    // CLONE

				    //! self-replicate
				    virtual derived_line<number>* clone() const override { return new zeta_threepf_wavenumber_series<number>(static_cast<const zeta_threepf_wavenumber_series<number>&>(*this)); }


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
		    zeta_threepf_wavenumber_series<number>::zeta_threepf_wavenumber_series(const threepf_task<number>& tk, model<number>* m,
		                                                                           filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, m, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
			      zeta_threepf_line<number>(tk, m, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
		        assert(m != nullptr);
		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_threepf_wavenumber_series<number>::zeta_threepf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      zeta_threepf_line<number>(reader),
			      wavenumber_series<number>(reader)
			    {
		        assert(reader != nullptr);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
			    }


		    template <typename number>
		    void zeta_threepf_wavenumber_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
		                                                              const std::list<std::string>& tags) const
			    {
		        unsigned int N_fields = this->mdl->get_N_fields();

		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename data_manager<number>::datapipe::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);
				    typename data_manager<number>::datapipe::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

		        // pull time-configuration information from the database
		        typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();
		        const std::vector<double>& t_values = tc_handle.lookup_tag(t_tag);

		        // pull the background field configuration for each time sample point
		        std::vector< std::vector<number> > background(this->time_sample_sns.size());
		        for(unsigned int i = 0; i < 2*N_fields; i++)
			        {
		            typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i);
		            const std::vector<number>& bg_line = t_handle.lookup_tag(tag);
		            assert(bg_line.size() == this->time_sample_sns.size());

		            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
			            {
		                background[j].push_back(bg_line[j]);
			            }
			        }

				    // extract k-configuration data
				    typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();
		        std::vector< typename data_manager<number>::threepf_configuration > configs = kc_handle.lookup_tag(k_tag);

				    // zip lists of serial numbers for each of the k1, k2, k3 configurations
		        std::vector<unsigned int> k1_serials;
		        std::vector<unsigned int> k2_serials;
		        std::vector<unsigned int> k3_serials;

				    for(typename std::vector< typename data_manager<number>::threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
					    {
						    k1_serials.push_back((*t).k1_serial);
						    k2_serials.push_back((*t).k2_serial);
						    k3_serials.push_back((*t).k3_serial);
					    }

				    // set up cache handles for each of these serial-number lists
				    typename data_manager<number>::datapipe::kconfig_data_handle& k1_handle = pipe.new_kconfig_data_handle(k1_serials);
				    typename data_manager<number>::datapipe::kconfig_data_handle& k2_handle = pipe.new_kconfig_data_handle(k2_serials);
				    typename data_manager<number>::datapipe::kconfig_data_handle& k3_handle = pipe.new_kconfig_data_handle(k3_serials);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
				        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing time-configuration " << i << std::endl;

		            // cache gauge transformation matrices for this time sample
		            std::vector<number> dN;
		            this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[i], dN);

				        // cache gauge transformation coefficients
				        // these have to be recomputed for each k-configuration, because they are time and shape-dependent
		            std::vector< std::vector< std::vector<number> > > ddN123(this->kconfig_sample_sns.size());
		            std::vector< std::vector< std::vector<number> > > ddN213(this->kconfig_sample_sns.size());
		            std::vector< std::vector< std::vector<number> > > ddN312(this->kconfig_sample_sns.size());
				        for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
					        {
						        this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[i], configs[j].k1_comoving, configs[j].k2_comoving, configs[j].k3_comoving, t_values[i], ddN123[j]);
				            this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[i], configs[j].k2_comoving, configs[j].k1_comoving, configs[j].k3_comoving, t_values[i], ddN213[j]);
				            this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[i], configs[j].k3_comoving, configs[j].k1_comoving, configs[j].k2_comoving, t_values[i], ddN312[j]);
					        }

		            // information for zeta will be stored in 'line_data'
		            std::vector<number> line_data;
		            line_data.assign(this->kconfig_sample_sns.size(), 0.0);

				        // linear part of the gauge transformation
				        for(unsigned int l = 0; l < 2*N_fields; l++)
					        {
				            for(unsigned int m = 0; m < 2*N_fields; m++)
					            {
				                for(unsigned int n = 0; n < 2*N_fields; n++)
					                {
				                    typename data_manager<number>::datapipe::cf_kconfig_data_tag tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_threepf, this->mdl->flatten(l,m,n), this->time_sample_sns[i]);

				                    std::vector<number> threepf_line = k_handle.lookup_tag(tag);

				                    // shift field so it represents a derivative correlation function, not a momentum one
				                    this->shifter.shift(this->parent_task, this->mdl, pipe, background[i], configs, threepf_line, l, m, n, this->time_sample_sns[i], t_values[i]);

				                    for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
					                    {
				                        line_data[j] += dN[l]*dN[m]*dN[n]*threepf_line[j];
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
										            // the indices are N_lm, N_p, N_q so the 2pfs we sum over are
										            // sigma_lp(k2)*sigma_mq(k3) etc.

										            typename data_manager<number>::datapipe::cf_kconfig_data_tag k1_re_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), this->time_sample_sns[i]);
								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k1_im_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), this->time_sample_sns[i]);

								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_re_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), this->time_sample_sns[i]);
								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_im_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), this->time_sample_sns[i]);

								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_re_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), this->time_sample_sns[i]);
								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_im_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), this->time_sample_sns[i]);

								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k3_re_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), this->time_sample_sns[i]);
								                typename data_manager<number>::datapipe::cf_kconfig_data_tag k3_im_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), this->time_sample_sns[i]);

								                const std::vector<number>& k1_re_lp = k1_handle.lookup_tag(k1_re_lp_tag);
								                const std::vector<number>& k1_im_lp = k1_handle.lookup_tag(k1_im_lp_tag);
								                const std::vector<number>& k2_re_lp = k2_handle.lookup_tag(k2_re_lp_tag);
								                const std::vector<number>& k2_im_lp = k2_handle.lookup_tag(k2_im_lp_tag);
								                const std::vector<number>& k2_re_mq = k2_handle.lookup_tag(k2_re_mq_tag);
								                const std::vector<number>& k2_im_mq = k2_handle.lookup_tag(k2_im_mq_tag);
								                const std::vector<number>& k3_re_mq = k3_handle.lookup_tag(k3_re_mq_tag);
								                const std::vector<number>& k3_im_mq = k3_handle.lookup_tag(k3_im_mq_tag);

								                for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
									                {
								                    line_data[j] += ddN123[j][l][m] * dN[p] * dN[q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
								                    line_data[j] += ddN213[j][l][m] * dN[p] * dN[q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
								                    line_data[j] += ddN312[j][l][m] * dN[p] * dN[q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);
									                }
									            }
							            }
					            }
			            }

		            std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
		            std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(t_values[i]);

		            data_line<number> line = data_line<number>(data_line<number>::wavenumber_series, data_line<number>::correlation_function,
		                                                       wavenumber_axis, line_data, latex_label, nonlatex_label);

		            lines.push_back(line);
			        }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_threepf_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->derived_line<number>::write(out);
		        this->zeta_threepf_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_threepf_wavenumber_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES));

		        this->derived_line<number>::serialize(writer);
		        this->zeta_threepf_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }


		    //! zeta reduced bispectrum wavenumber data line
		    template <typename number>
		    class zeta_reduced_bispectrum_wavenumber_series: public wavenumber_series<number>, public zeta_reduced_bispectrum_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct a zeta reduced bispectrum wavenumber-series object
		        zeta_reduced_bispectrum_wavenumber_series(const threepf_task<number>& tk, model<number>* m,
		                                                  filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
		                                                  unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        zeta_reduced_bispectrum_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

		        virtual ~zeta_reduced_bispectrum_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'derived_line' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;


		        // CLONE

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new zeta_reduced_bispectrum_wavenumber_series<number>(static_cast<const zeta_reduced_bispectrum_wavenumber_series<number>&>(*this)); }


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
		    zeta_reduced_bispectrum_wavenumber_series<number>::zeta_reduced_bispectrum_wavenumber_series(const threepf_task<number>& tk, model<number>* m,
		                                                                                                 filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, m, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
			      zeta_reduced_bispectrum_line<number>(tk, m, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
		        assert(m != nullptr);
		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_reduced_bispectrum_wavenumber_series<number>::zeta_reduced_bispectrum_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      zeta_reduced_bispectrum_line<number>(reader),
			      wavenumber_series<number>(reader)
			    {
		        assert(reader != nullptr);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
			    }


		    template <typename number>
		    void zeta_reduced_bispectrum_wavenumber_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
		                                                                         const std::list<std::string>& tags) const
			    {
		        unsigned int N_fields = this->mdl->get_N_fields();

		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename data_manager<number>::datapipe::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

		        // pull time-configuration information from the database
		        typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();
		        const std::vector<double>& t_values = tc_handle.lookup_tag(t_tag);

		        // pull the background field configuration for each time sample point
		        std::vector< std::vector<number> > background(this->time_sample_sns.size());
		        for(unsigned int i = 0; i < 2*N_fields; i++)
			        {
		            typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i);
		            const std::vector<number>& bg_line = t_handle.lookup_tag(tag);
		            assert(bg_line.size() == this->time_sample_sns.size());

		            for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
			            {
		                background[j].push_back(bg_line[j]);
			            }
			        }

		        // extract k-configuration data
		        typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = pipe.new_threepf_kconfig_tag();
		        std::vector< typename data_manager<number>::threepf_configuration > configs = kc_handle.lookup_tag(k_tag);

		        // zip lists of serial numbers for each of the k1, k2, k3 configurations
		        std::vector<unsigned int> k1_serials;
		        std::vector<unsigned int> k2_serials;
		        std::vector<unsigned int> k3_serials;

		        for(typename std::vector< typename data_manager<number>::threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			        {
		            k1_serials.push_back((*t).k1_serial);
		            k2_serials.push_back((*t).k2_serial);
		            k3_serials.push_back((*t).k3_serial);
			        }

		        // set up cache handles for each of these serial-number lists
		        typename data_manager<number>::datapipe::kconfig_data_handle& k1_handle = pipe.new_kconfig_data_handle(k1_serials);
		        typename data_manager<number>::datapipe::kconfig_data_handle& k2_handle = pipe.new_kconfig_data_handle(k2_serials);
		        typename data_manager<number>::datapipe::kconfig_data_handle& k3_handle = pipe.new_kconfig_data_handle(k3_serials);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
		            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing time-configuration " << i << std::endl;

		            // cache gauge transformation matrices for this time sample
		            std::vector<number> dN;
		            this->mdl->compute_gauge_xfm_1(this->parent_task->get_params(), background[i], dN);

		            // cache gauge transformation coefficients
		            // these have to be recomputed for each k-configuration, because they are time and shape-dependent
		            std::vector< std::vector< std::vector<number> > > ddN123(this->kconfig_sample_sns.size());
		            std::vector< std::vector< std::vector<number> > > ddN213(this->kconfig_sample_sns.size());
		            std::vector< std::vector< std::vector<number> > > ddN312(this->kconfig_sample_sns.size());
		            for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
			            {
		                this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[i], configs[j].k1_comoving, configs[j].k2_comoving, configs[j].k3_comoving, t_values[i], ddN123[j]);
		                this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[i], configs[j].k2_comoving, configs[j].k1_comoving, configs[j].k3_comoving, t_values[i], ddN213[j]);
		                this->mdl->compute_gauge_xfm_2(this->parent_task->get_params(), background[i], configs[j].k3_comoving, configs[j].k1_comoving, configs[j].k2_comoving, t_values[i], ddN312[j]);
			            }

		            // information for zeta will be stored in 'line_data'
		            std::vector<number> line_data;
		            line_data.assign(this->kconfig_sample_sns.size(), 0.0);

				        // FIRST, BUILD THE BISPECTRUM ITSELF

		            // linear part of the gauge transformation
		            for(unsigned int l = 0; l < 2*N_fields; l++)
			            {
		                for(unsigned int m = 0; m < 2*N_fields; m++)
			                {
		                    for(unsigned int n = 0; n < 2*N_fields; n++)
			                    {
		                        typename data_manager<number>::datapipe::cf_kconfig_data_tag tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_threepf, this->mdl->flatten(l,m,n), this->time_sample_sns[i]);

		                        std::vector<number> threepf_line = k_handle.lookup_tag(tag);

		                        // shift field so it represents a derivative correlation function, not a momentum one
		                        this->shifter.shift(this->parent_task, this->mdl, pipe, background[i], configs, threepf_line, l, m, n, this->time_sample_sns[i], t_values[i]);

		                        for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
			                        {
		                            line_data[j] += dN[l]*dN[m]*dN[n]*threepf_line[j];
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
		                            // the indices are N_lm, N_p, N_q so the 2pfs we sum over are
		                            // sigma_lp(k2)*sigma_mq(k3) etc.

		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k1_re_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), this->time_sample_sns[i]);
		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k1_im_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), this->time_sample_sns[i]);

		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_re_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(l,p), this->time_sample_sns[i]);
		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_im_lp_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(l,p), this->time_sample_sns[i]);

		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_re_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), this->time_sample_sns[i]);
		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k2_im_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), this->time_sample_sns[i]);

		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k3_re_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,q), this->time_sample_sns[i]);
		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag k3_im_mq_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, this->mdl->flatten(m,q), this->time_sample_sns[i]);

		                            const std::vector<number>& k1_re_lp = k1_handle.lookup_tag(k1_re_lp_tag);
		                            const std::vector<number>& k1_im_lp = k1_handle.lookup_tag(k1_im_lp_tag);
		                            const std::vector<number>& k2_re_lp = k2_handle.lookup_tag(k2_re_lp_tag);
		                            const std::vector<number>& k2_im_lp = k2_handle.lookup_tag(k2_im_lp_tag);
		                            const std::vector<number>& k2_re_mq = k2_handle.lookup_tag(k2_re_mq_tag);
		                            const std::vector<number>& k2_im_mq = k2_handle.lookup_tag(k2_im_mq_tag);
		                            const std::vector<number>& k3_re_mq = k3_handle.lookup_tag(k3_re_mq_tag);
		                            const std::vector<number>& k3_im_mq = k3_handle.lookup_tag(k3_im_mq_tag);

		                            for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
			                            {
		                                line_data[j] += ddN123[j][l][m] * dN[p] * dN[q] * (k2_re_lp[j]*k3_re_mq[j] - k2_im_lp[j]*k3_im_mq[j]);
		                                line_data[j] += ddN213[j][l][m] * dN[p] * dN[q] * (k1_re_lp[j]*k3_re_mq[j] - k1_im_lp[j]*k3_im_mq[j]);
		                                line_data[j] += ddN312[j][l][m] * dN[p] * dN[q] * (k1_re_lp[j]*k2_re_mq[j] - k1_im_lp[j]*k2_im_mq[j]);
			                            }
			                        }
			                    }
			                }
			            }

				        // SECOND, CONSTRUCT THE SPECTRA

		            std::vector<number> twopf_k1_data;
		            std::vector<number> twopf_k2_data;
		            std::vector<number> twopf_k3_data;
				        twopf_k1_data.assign(this->kconfig_sample_sns.size(), 0.0);
				        twopf_k2_data.assign(this->kconfig_sample_sns.size(), 0.0);
				        twopf_k3_data.assign(this->kconfig_sample_sns.size(), 0.0);

				        for(unsigned int m = 0; m < 2*N_fields; m++)
					        {
						        for(unsigned int n = 0; n < 2*N_fields; n++)
							        {
								        typename data_manager<number>::datapipe::cf_kconfig_data_tag tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, this->mdl->flatten(m,n), this->time_sample_sns[i]);

								        // pull twopf data
								        const std::vector<number>& k1_line = k1_handle.lookup_tag(tag);
								        const std::vector<number>& k2_line = k2_handle.lookup_tag(tag);
								        const std::vector<number>& k3_line = k3_handle.lookup_tag(tag);

								        for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
									        {
										        twopf_k1_data[j] += dN[m]*dN[n]*k1_line[j];
										        twopf_k2_data[j] += dN[m]*dN[n]*k2_line[j];
										        twopf_k3_data[j] += dN[m]*dN[n]*k3_line[j];
									        }
							        }
					        }

				        // THIRD, CONSTRUCT THE REDUCED BISPECTRUM

				        for(unsigned int j = 0; j < this->kconfig_sample_sns.size(); j++)
					        {
						        number form_factor = (6.0/5.0) * ( twopf_k1_data[j]*twopf_k2_data[j] + twopf_k1_data[j]*twopf_k3_data[j] + twopf_k2_data[j]*twopf_k3_data[j] );

						        line_data[j] = line_data[j] / form_factor;
					        }

		            std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
		            std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(t_values[i]);

		            data_line<number> line = data_line<number>(data_line<number>::wavenumber_series, data_line<number>::correlation_function,
		                                                       wavenumber_axis, line_data, latex_label, nonlatex_label);

		            lines.push_back(line);
			        }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_reduced_bispectrum_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->derived_line<number>::write(out);
		        this->zeta_reduced_bispectrum_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_reduced_bispectrum_wavenumber_series<number>::serialize(serialization_writer& writer) const
			    {
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES));

		        this->derived_line<number>::serialize(writer);
		        this->zeta_reduced_bispectrum_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }

			}

	}


#endif //__zeta_wavenumber_series_H_
