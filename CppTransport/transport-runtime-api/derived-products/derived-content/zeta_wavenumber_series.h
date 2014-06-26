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
				    zeta_twopf_wavenumber_series(const twopf_list_task<number>& tk,
				                                 filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
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
		    zeta_twopf_wavenumber_series<number>::zeta_twopf_wavenumber_series(const twopf_list_task<number>& tk,
		                                                                       filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
			      zeta_twopf_line<number>(tk, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
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
		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename data_manager<number>::datapipe::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::kconfig_zeta_handle& z_handle = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

            // pull time-configuration information from the database
		        typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
				        typename data_manager<number>::datapipe::zeta_twopf_kconfig_data_tag tag = pipe.new_zeta_twopf_kconfig_data_tag(this->time_sample_sns[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

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
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
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
            zeta_threepf_wavenumber_series(const threepf_task<number>& tk,
                                           filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                           unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

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


            // INTERNAL DATA

          protected:

            //! compute delegate
            zeta_kseries_compute<number> computer;

					};


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    zeta_threepf_wavenumber_series<number>::zeta_threepf_wavenumber_series(const threepf_task<number>& tk,
		                                                                           filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
			      zeta_threepf_line<number>(tk, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
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
		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename data_manager<number>::datapipe::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename data_manager<number>::datapipe::kconfig_zeta_handle& z_handle = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

            // pull time-configuration information from the database
		        typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
				        BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing time-configuration " << i << std::endl;

				        typename data_manager<number>::datapipe::zeta_threepf_kconfig_data_tag tag = pipe.new_zeta_threepf_kconfig_data_tag(this->time_sample_sns[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

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
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
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
		        zeta_reduced_bispectrum_wavenumber_series(const threepf_task<number>& tk,
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
		    zeta_reduced_bispectrum_wavenumber_series<number>::zeta_reduced_bispectrum_wavenumber_series(const threepf_task<number>& tk,
		                                                                                                 filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, derived_line<number>::wavenumber_series, derived_line<number>::fNL, prec),
			      zeta_reduced_bispectrum_line<number>(tk, kfilter),
			      wavenumber_series<number>(tk, tfilter)
			    {
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
		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename data_manager<number>::datapipe::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename data_manager<number>::datapipe::kconfig_zeta_handle& z_handle = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

		        // pull time-configuration information from the database
		        typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
		            BOOST_LOG_SEV(pipe.get_log(), data_manager<number>::normal) << std::endl << "§§ Processing time-configuration " << i << std::endl;

				        typename data_manager<number>::datapipe::zeta_reduced_bispectrum_kconfig_data_tag tag = pipe.new_zeta_reduced_bispectrum_kconfig_data_tag(this->time_sample_sns[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

		            std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
		            std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(t_values[i]);

		            data_line<number> line = data_line<number>(data_line<number>::wavenumber_series, data_line<number>::fNL,
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
		        writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
		                               std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_WAVENUMBER_SERIES));

		        this->derived_line<number>::serialize(writer);
		        this->zeta_reduced_bispectrum_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }

			}

	}


#endif //__zeta_wavenumber_series_H_
