//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __field_wavenumber_series_H_
#define __field_wavenumber_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/threepf_line.h"


namespace transport
	{

    namespace derived_data
	    {

        //! twopf wavenumber data line
        template <typename number>
        class twopf_wavenumber_series: public wavenumber_series<number>, public twopf_line<number>
	        {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a twopf wavenumber-series object
            twopf_wavenumber_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
                                    filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
                                    unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

		        virtual ~twopf_wavenumber_series() = default;


            // DERIVE LINES -- implements a 'derived_line' interface

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(unsigned int m, unsigned int n, double t) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(unsigned int m, unsigned int n, double t) const;


            // CLONE

            //! self-replicate
            virtual derived_line<number>* clone() const override { return new twopf_wavenumber_series<number>(static_cast<const twopf_wavenumber_series<number>&>(*this)); }


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
        twopf_wavenumber_series<number>::twopf_wavenumber_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
                                                                 filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
	        : derived_line<number>(tk, wavenumber_axis, std::list<axis_value>{ k_axis, efolds_exit_axis }, prec),
	          twopf_line<number>(tk, sel, kfilter),
	          wavenumber_series<number>(tk, tfilter)
	        {
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        twopf_wavenumber_series<number>::twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
	        : derived_line<number>(reader, finder),
	          twopf_line<number>(reader, finder),
	          wavenumber_series<number>(reader)
	        {
	        }


		    template <typename number>
		    void twopf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
		                                                       const std::list<std::string>& tags) const
			    {
		        unsigned int N_fields = this->gadget.get_N_fields();

		        // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> w_axis = this->pull_twopf_kconfig_axis(pipe);

				    // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
				    typename datapipe<number>::time_config_handle&   tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename datapipe<number>::kconfig_data_handle&  k_handle  = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

				    // pull time-configuration information from the database
				    time_config_tag<number> t_tag = pipe.new_time_config_tag();
				    const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
		        const typename std::vector< twopf_configuration > k_values = kc_handle.lookup_tag(k_tag);

				    // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
				    for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
					    {
						    for(unsigned int m = 0; m < 2*N_fields; m++)
							    {
								    for(unsigned int n = 0; n < 2*N_fields; n++)
									    {
								        std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
								            cf_kconfig_data_tag<number> tag = pipe.new_cf_kconfig_data_tag(this->is_real_twopf() ? data_tag<number>::cf_twopf_re : data_tag<number>::cf_twopf_im,
								                                                                           this->gadget.get_model()->flatten(m, n), this->time_sample_sns[i]);

								            std::vector<number> line_data = k_handle.lookup_tag(tag);

								            if(this->dimensionless)
									            {
								                assert(line_data.size() == k_values.size());
								                for(unsigned int j = 0; j < line_data.size() && j <= k_values.size(); j++)
									                {
								                    line_data[j] *= k_values[j].k_comoving*k_values[j].k_comoving*k_values[j].k_comoving / (2.0*M_PI*M_PI);
									                }

								                data_line<number> line = data_line<number>(group, this->x_type, dimensionless_value, w_axis, line_data,
								                                                           this->get_LaTeX_label(m,n,t_values[i]), this->get_non_LaTeX_label(m,n,t_values[i]), this->is_spectral_index());
								                lines.push_back(line);
									            }
								            else
									            {
								                data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, w_axis, line_data,
								                                                           this->get_LaTeX_label(m,n,t_values[i]), this->get_non_LaTeX_label(m,n,t_values[i]), this->is_spectral_index());
								                lines.push_back(line);
									            }
									        }
									    }
							    }
					    }

		        // detach pipe from output group
		        this->detach(pipe);
			    }


        template <typename number>
        std::string twopf_wavenumber_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, double t) const
	        {
            std::string tag = this->make_LaTeX_tag(t);
            std::string label;

            if(this->label_set)
	            {
                label = this->LaTeX_label;
	            }
            else
	            {
                label = "$" + this->make_LaTeX_label(m,n) + "$";
	            }

            if(this->use_tags) label += " $" + tag + "$";
            return(label);
	        }


        template <typename number>
        std::string twopf_wavenumber_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, double t) const
	        {
            std::string tag = this->make_non_LaTeX_tag(t);
            std::string label;

            if(this->label_set)
	            {
                label = this->non_LaTeX_label;
	            }
            else
	            {
                label = this->make_non_LaTeX_label(m,n);
	            }

            if(this->use_tags) label += " " + tag;
            return(label);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void twopf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->twopf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);

            twopf_list_task<number>* ptk = dynamic_cast< twopf_list_task<number>* >(this->get_parent_task());
            if(ptk != nullptr)
	            {
                this->write_kconfig_list(out, ptk->get_twopf_kconfig_list());
	            }
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void twopf_wavenumber_series<number>::serialize(Json::Value& writer) const
	        {
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES);

            this->derived_line<number>::serialize(writer);
            this->twopf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }


		    //! threepf wavenumber data line
		    template <typename number>
		    class threepf_wavenumber_series: public wavenumber_series<number>, public threepf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a threepf wavenumber-data object
				    threepf_wavenumber_series(const threepf_task<number>& tk, index_selector<3>& sel,
				    filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
				    unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    threepf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~threepf_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
		        virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const;


		        // CLONE

		      public:

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new threepf_wavenumber_series<number>(static_cast<const threepf_wavenumber_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		      public:

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
        threepf_wavenumber_series<number>::threepf_wavenumber_series(const threepf_task<number>& tk, index_selector<3>& sel,
                                                                     filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                                                     unsigned int prec)
	        : derived_line<number>(tk, wavenumber_axis,
	                               std::list<axis_value>{ k_axis, efolds_exit_axis, alpha_axis, beta_axis, squeezing_fraction_k1_axis, squeezing_fraction_k2_axis, squeezing_fraction_k3_axis },
	                               prec),
	          threepf_line<number>(tk, sel, kfilter),
	          wavenumber_series<number>(tk, tfilter)
	        {
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        threepf_wavenumber_series<number>::threepf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
	        : derived_line<number>(reader, finder),
	          threepf_line<number>(reader, finder),
	          wavenumber_series<number>(reader)
	        {
	        }


        template <typename number>
        void threepf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                             const std::list<std::string>& tags) const
	        {
		        unsigned int N_fields = this->gadget.get_N_fields();

            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

            // pull wavenumber-axis data
            std::vector<double> w_axis = this->pull_threepf_kconfig_axis(pipe);

            // set up cache handles
            typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
            typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
            typename datapipe<number>::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

            // pull time-configuration information from the database
            time_config_tag<number> t_tag = pipe.new_time_config_tag();
            const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

            // pull the background field configuration for each time sample point
            std::vector< std::vector<number> > background(this->time_sample_sns.size());
            for(unsigned int i = 0; i < 2*N_fields; i++)
	            {
                background_time_data_tag<number> tag = pipe.new_background_time_data_tag(i);
                const std::vector<number> bg_line = t_handle.lookup_tag(tag);
                assert(bg_line.size() == this->time_sample_sns.size());

                for(unsigned int j = 0; j < this->time_sample_sns.size(); j++)
	                {
                    background[j].push_back(bg_line[j]);
	                }
	            }

            // extract k-configuration data
            threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
            std::vector< threepf_configuration > configs = kc_handle.lookup_tag(k_tag);

            // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
            for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
	            {
                for(unsigned int l = 0; l < 2*N_fields; l++)
	                {
                    for(unsigned int m = 0; m < 2*N_fields; m++)
	                    {
		                    for(unsigned int n = 0; n < 2*N_fields; n++)
			                    {
		                        std::array<unsigned int, 3> index_set = { l, m, n };
		                        if(this->active_indices.is_on(index_set))
			                        {
		                            cf_kconfig_data_tag<number> tag = pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_threepf, this->gadget.get_model()->flatten(l,m,n), this->time_sample_sns[i]);

		                            std::vector<number> line_data = k_handle.lookup_tag(tag);

				                        // the integrator produces correlation functions involving the canonical momenta,
				                        // not the derivatives. If the user wants derivatives then we have to shift.
				                        if(this->get_dot_meaning() == derived_line<number>::derivatives)
					                        this->shifter.shift(this->gadget.get_integration_task(), this->gadget.get_model(), pipe, background[i], configs, line_data, l, m, n, this->time_sample_sns[i], t_values[i]);

		                            data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, w_axis, line_data,
		                                                                       this->get_LaTeX_label(l,m,n,t_values[i]), this->get_non_LaTeX_label(l,m,n,t_values[i]), this->is_spectral_index());

		                            lines.push_back(line);
			                        }
			                    }
	                    }
	                }
	            }

            // detach pipe from output group
            this->detach(pipe);
	        }


        template <typename number>
        std::string threepf_wavenumber_series<number>::get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const
	        {
            std::string tag = this->make_LaTeX_tag(t);
            std::string label;

            if(this->label_set)
	            {
                label = this->LaTeX_label;
	            }
            else
	            {
                label = "$" + this->make_LaTeX_label(l,m,n) + "$";
	            }

            if(this->use_tags) label += " $" + tag + "$";
            return(label);
	        }


        template <typename number>
        std::string threepf_wavenumber_series<number>::get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const
	        {
            std::string tag = this->make_non_LaTeX_tag(t);
            std::string label;

            if(this->label_set)
	            {
                label = this->non_LaTeX_label;
	            }
            else
	            {
                label = this->make_non_LaTeX_label(l,m,n);
	            }

            if(this->use_tags) label += " " + tag;
            return(label);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void threepf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->threepf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);

            threepf_task<number>* ptk = dynamic_cast< threepf_task<number>* >(this->get_parent_task());
            if(ptk != nullptr)
	            {
                this->write_kconfig_list(out, ptk->get_threepf_kconfig_list());
	            }
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void threepf_wavenumber_series<number>::serialize(Json::Value& writer) const
	        {
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES);

            this->derived_line<number>::serialize(writer);
            this->threepf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }


	    }   // namespace derived_data

	}   // namespace transport


#endif //__field_wavenumber_series_H_
