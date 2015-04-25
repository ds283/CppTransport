//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
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

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_threepf_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_reduced_bispectrum_line.h"


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
				    zeta_twopf_wavenumber_series(const zeta_twopf_list_task<number>& tk,
				                                 filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
				                                 unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    // deserialization constructor
				    zeta_twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~zeta_twopf_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'time_series' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(double t) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(double t) const;


		        // CLONE

		        //! self-replicate
		        virtual zeta_twopf_wavenumber_series<number>* clone() const override { return new zeta_twopf_wavenumber_series<number>(static_cast<const zeta_twopf_wavenumber_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(Json::Value& writer) const override;

			    };


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_twopf_wavenumber_series<number>::zeta_twopf_wavenumber_series(const zeta_twopf_list_task<number>& tk,
		                                                                       filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, wavenumber_axis, std::list<axis_value>{ k_axis, efolds_exit_axis }, prec),
			      zeta_twopf_line<number>(tk, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_twopf_wavenumber_series<number>::zeta_twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      zeta_twopf_line<number>(reader),
			      wavenumber_series<number>(reader)
			    {
			    }


		    template <typename number>
		    void zeta_twopf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
		                                                            const std::list<std::string>& tags) const
			    {
		        // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> w_axis = this->pull_twopf_kconfig_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        typename datapipe<number>::time_config_handle&   tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename datapipe<number>::kconfig_zeta_handle&  z_handle  = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

            // pull time-configuration information from the database
		        time_config_tag<number> t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
		        const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); ++i)
			        {
				        zeta_twopf_kconfig_data_tag<number> tag = pipe.new_zeta_twopf_kconfig_data_tag(this->time_sample_sns[i]);

                std::vector<number> line_data = z_handle.lookup_tag(tag);

				        if(this->dimensionless)
					        {
						        assert(line_data.size() == k_values.size());
						        for(unsigned int j = 0; j < line_data.size() && j <= k_values.size(); ++j)
							        {
								        line_data[j] *= k_values[j].k_comoving*k_values[j].k_comoving*k_values[j].k_comoving / (2.0*M_PI*M_PI);
							        }

				            data_line<number> line = data_line<number>(group, this->x_type, dimensionless_value, w_axis, line_data,
				                                                       this->get_LaTeX_label(t_values[i]), this->get_non_LaTeX_label(t_values[i]), this->is_spectral_index());
				            lines.push_back(line);
					        }
				        else
					        {
				            data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, w_axis, line_data,
				                                                       this->get_LaTeX_label(t_values[i]), this->get_non_LaTeX_label(t_values[i]), this->is_spectral_index());
				            lines.push_back(line);
					        }
			        }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


		    template <typename number>
		    std::string zeta_twopf_wavenumber_series<number>::get_LaTeX_label(double t) const
			    {
		        std::string tag = this->make_LaTeX_tag(t);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->LaTeX_label;
			        }
		        else
			        {
		            label = "$" + this->make_LaTeX_label() + "$";
			        }

		        if(this->use_tags) label += " $" + tag + "$";
		        return(label);
			    }


		    template <typename number>
		    std::string zeta_twopf_wavenumber_series<number>::get_non_LaTeX_label(double t) const
			    {
		        std::string tag = this->make_non_LaTeX_tag(t);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            label = this->make_non_LaTeX_label();
			        }

		        if(this->use_tags) label += " " + tag;
		        return(label);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_twopf_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->zeta_twopf_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
		        this->derived_line<number>::write(out);

		        zeta_twopf_list_task<number>* ptk = dynamic_cast< zeta_twopf_list_task<number>* >(this->get_parent_task());
		        if(ptk != nullptr)
			        {
		            this->write_kconfig_list(out, ptk->get_twopf_database());
			        }
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_twopf_wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_WAVENUMBER_SERIES);

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
            zeta_threepf_wavenumber_series(const zeta_threepf_task<number>& tk,
                                           filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                           unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! deserialization constructor
						zeta_threepf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

						virtual ~zeta_threepf_wavenumber_series() = default;


				    // DERIVE LINES -- implements a 'derived_line' interface

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
				                              const std::list<std::string>& tags) const override;

				    //! generate a LaTeX label
				    std::string get_LaTeX_label(double t) const;

				    //! generate a non-LaTeX label
				    std::string get_non_LaTeX_label(double t) const;


				    // CLONE

				    //! self-replicate
				    virtual zeta_threepf_wavenumber_series<number>* clone() const override { return new zeta_threepf_wavenumber_series<number>(static_cast<const zeta_threepf_wavenumber_series<number>&>(*this)); }


				    // WRITE TO A STREAM

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

				  public:

				    //! serialize this object
				    virtual void serialize(Json::Value& writer) const override;
						
					};


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_threepf_wavenumber_series<number>::zeta_threepf_wavenumber_series(const zeta_threepf_task<number>& tk,
		                                                                           filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, wavenumber_axis,
			                           std::list<axis_value>{ k_axis, efolds_exit_axis, alpha_axis, beta_axis, squeezing_fraction_k1_axis, squeezing_fraction_k2_axis, squeezing_fraction_k3_axis },
			                           prec),
			      zeta_threepf_line<number>(tk, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_threepf_wavenumber_series<number>::zeta_threepf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      zeta_threepf_line<number>(reader),
			      wavenumber_series<number>(reader)
			    {
			    }


		    template <typename number>
		    void zeta_threepf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                                              const std::list<std::string>& tags) const
			    {
		        // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> w_axis = this->pull_threepf_kconfig_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename datapipe<number>::kconfig_zeta_handle& z_handle = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

            // pull time-configuration information from the database
		        time_config_tag<number> t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); ++i)
			        {
				        zeta_threepf_kconfig_data_tag<number> tag = pipe.new_zeta_threepf_kconfig_data_tag(this->time_sample_sns[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

		            data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, w_axis, line_data,
		                                                       this->get_LaTeX_label(t_values[i]), this->get_non_LaTeX_label(t_values[i]), this->is_spectral_index());

		            lines.push_back(line);
			        }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


		    template <typename number>
		    std::string zeta_threepf_wavenumber_series<number>::get_LaTeX_label(double t) const
			    {
		        std::string tag = this->make_LaTeX_tag(t);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->LaTeX_label;
			        }
		        else
			        {
		            label = "$" + this->make_LaTeX_label() + "$";
			        }

		        if(this->use_tags) label += " $" + tag + "$";
		        return(label);
			    }


		    template <typename number>
		    std::string zeta_threepf_wavenumber_series<number>::get_non_LaTeX_label(double t) const
			    {
		        std::string tag = this->make_non_LaTeX_tag(t);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            label = this->make_non_LaTeX_label();
			        }

		        if(this->use_tags) label += " " + tag;
		        return(label);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_threepf_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->zeta_threepf_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
		        this->derived_line<number>::write(out);

		        zeta_threepf_task<number>* ptk = dynamic_cast< zeta_threepf_task<number>* >(this->get_parent_task());
		        if(ptk != nullptr)
			        {
		            this->write_kconfig_list(out, ptk->get_threepf_database());
			        }
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_threepf_wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_WAVENUMBER_SERIES);

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
		        zeta_reduced_bispectrum_wavenumber_series(const zeta_threepf_task<number>& tk,
		                                                  filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
		                                                  unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        zeta_reduced_bispectrum_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

		        virtual ~zeta_reduced_bispectrum_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'derived_line' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;

				    //! generate a LaTeX label
				    std::string get_LaTeX_label(double t) const;

				    //! generate a non-LaTeX label
				    std::string get_non_LaTeX_label(double t) const;


		        // CLONE

		        //! self-replicate
		        virtual zeta_reduced_bispectrum_wavenumber_series<number>* clone() const override { return new zeta_reduced_bispectrum_wavenumber_series<number>(static_cast<const zeta_reduced_bispectrum_wavenumber_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(Json::Value& writer) const override;

			    };


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_reduced_bispectrum_wavenumber_series<number>::zeta_reduced_bispectrum_wavenumber_series(const zeta_threepf_task<number>& tk,
		                                                                                                 filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, wavenumber_axis,
			                           std::list<axis_value>{ k_axis, efolds_exit_axis, alpha_axis, beta_axis, squeezing_fraction_k1_axis, squeezing_fraction_k2_axis, squeezing_fraction_k3_axis },
			                           prec),
			      zeta_reduced_bispectrum_line<number>(tk, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_reduced_bispectrum_wavenumber_series<number>::zeta_reduced_bispectrum_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      zeta_reduced_bispectrum_line<number>(reader),
			      wavenumber_series<number>(reader)
			    {
			    }


		    template <typename number>
		    void zeta_reduced_bispectrum_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                                                         const std::list<std::string>& tags) const
			    {
		        // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> w_axis = this->pull_threepf_kconfig_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename datapipe<number>::kconfig_zeta_handle& z_handle = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

		        // pull time-configuration information from the database
		        time_config_tag<number> t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); ++i)
			        {
				        zeta_reduced_bispectrum_kconfig_data_tag<number> tag = pipe.new_zeta_reduced_bispectrum_kconfig_data_tag(this->time_sample_sns[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

		            data_line<number> line = data_line<number>(group, this->x_type, fNL_value, w_axis, line_data,
		                                                       this->get_LaTeX_label(t_values[i]), this->get_non_LaTeX_label(t_values[i]), this->is_spectral_index());

		            lines.push_back(line);
			        }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


				template <typename number>
				std::string zeta_reduced_bispectrum_wavenumber_series<number>::get_LaTeX_label(double t) const
					{
				    std::string tag = this->make_LaTeX_tag(t);
				    std::string label;

				    if(this->label_set)
					    {
				        label = this->LaTeX_label;
					    }
				    else
					    {
				        label = "$" + this->make_LaTeX_label() + "$";
					    }

				    if(this->use_tags) label += " $" + tag + "$";
				    return(label);
					}


				template <typename number>
				std::string zeta_reduced_bispectrum_wavenumber_series<number>::get_non_LaTeX_label(double t) const
					{
				    std::string tag = this->make_non_LaTeX_tag(t);
				    std::string label;

				    if(this->label_set)
					    {
				        label = this->non_LaTeX_label;
					    }
				    else
					    {
				        label = this->make_non_LaTeX_label();
					    }

				    if(this->use_tags) label += " " + tag;
				    return(label);
					}


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_reduced_bispectrum_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->zeta_reduced_bispectrum_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
		        this->derived_line<number>::write(out);

				    zeta_threepf_task<number>* ptk = dynamic_cast< zeta_threepf_task<number>* >(this->get_parent_task());
				    if(ptk != nullptr)
					    {
				        this->write_kconfig_list(out, ptk->get_threepf_database());
					    }
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void zeta_reduced_bispectrum_wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES);

		        this->derived_line<number>::serialize(writer);
		        this->zeta_reduced_bispectrum_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }

			}

	}


#endif //__zeta_wavenumber_series_H_
