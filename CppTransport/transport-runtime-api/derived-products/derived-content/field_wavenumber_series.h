//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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
        class twopf_wavenumber_series : public wavenumber_series<number>, public twopf_line<number>
	        {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a twopf wavenumber-series object
            twopf_wavenumber_series(const twopf_list_task<number>& tk, model<number>* m, index_selector<2>& sel,
                                    filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
                                    unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        twopf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

		        virtual ~twopf_wavenumber_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

            //! generate data lines for plotting
            virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;


            // CLONE

            //! self-replicate
            virtual derived_line<number>* clone() const override { return new twopf_wavenumber_series<number>(static_cast<const twopf_wavenumber_series<number>&>(*this)); }


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
        twopf_wavenumber_series<number>::twopf_wavenumber_series(const twopf_list_task<number>& tk, model<number>* m, index_selector<2>& sel,
                                                                 filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
	        : derived_line<number>(tk, m, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
	          twopf_line<number>(tk, m, sel, kfilter),
	          wavenumber_series<number>(tk, tfilter)
	        {
            assert(m != nullptr);
            if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        twopf_wavenumber_series<number>::twopf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
	        : derived_line<number>(reader, finder),
	          twopf_line<number>(reader),
	          wavenumber_series<number>(reader)
	        {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
	        }


		    template <typename number>
		    void twopf_wavenumber_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
		                                                       const std::list<std::string>& tags) const
			    {
		        // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull time-axis data
		        std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

				    // set up cache handles
				    typename data_manager<number>::datapipe::time_config_handle& t_handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename data_manager<number>::datapipe::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

				    // pull time-configuration information from the database
				    typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();

				    const std::vector<double>& t_values = t_handle.lookup_tag(t_tag);

				    // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
				    for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
					    {
						    for(unsigned int m = 0; m < 2*this->mdl->get_N_fields(); m++)
							    {
								    for(unsigned int n = 0; n < 2*this->mdl->get_N_fields(); n++)
									    {
								        std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
								            typename data_manager<number>::datapipe::cf_kconfig_data_tag tag =
									                                                                      pipe.new_cf_kconfig_data_tag(this->is_real_twopf() ? data_manager<number>::datapipe::cf_twopf_re : data_manager<number>::datapipe::cf_twopf_im,
									                                                                                                this->mdl->flatten(m,n), this->time_sample_sns[i]);

								            const std::vector<number>& line_data = k_handle.lookup_tag(tag);

								            std::string latex_label = "$" + this->make_LaTeX_label(m,n) + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
								            std::string nonlatex_label = this->make_non_LaTeX_label(m,n) + " " + this->make_non_LaTeX_tag(t_values[i]);

								            data_line<number> line = data_line<number>(data_line<number>::wavenumber_series, data_line<number>::correlation_function,
								                                                       wavenumber_axis, line_data, latex_label, nonlatex_label);

								            lines.push_back(line);
									        }
									    }
							    }
					    }

		        // detach pipe from output group
		        this->detach(pipe);
			    }

        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void twopf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->derived_line<number>::write(out);
            this->twopf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
	        }

        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void twopf_wavenumber_series<number>::serialize(serialization_writer& writer) const
	        {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES));

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
				    threepf_wavenumber_series(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel,
				    filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
				    unsigned int prec=__CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    threepf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder);

				    virtual ~threepf_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
		        virtual void derive_lines(typename data_manager<number>::datapipe& pipe, std::list< data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;


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
		        virtual void serialize(serialization_writer& writer) const override;

			    };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        threepf_wavenumber_series<number>::threepf_wavenumber_series(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel,
                                                                     filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                                                     unsigned int prec)
	        : derived_line<number>(tk, m, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
	          threepf_line<number>(tk, m, sel, kfilter),
	          wavenumber_series<number>(tk, tfilter)
	        {
            assert(m != nullptr);
            if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_MODEL);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        threepf_wavenumber_series<number>::threepf_wavenumber_series(serialization_reader* reader, typename repository<number>::task_finder& finder)
	        : derived_line<number>(reader, finder),
	          threepf_line<number>(reader),
	          wavenumber_series<number>(reader)
	        {
            assert(reader != nullptr);
            if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
	        }


        template <typename number>
        void threepf_wavenumber_series<number>::derive_lines(typename data_manager<number>::datapipe& pipe, std::list<data_line<number> >& lines,
                                                             const std::list<std::string>& tags) const
	        {
            // attach our datapipe to an output group
            this->attach(pipe, tags);

            // pull time-axis data
            std::vector<double> wavenumber_axis;
            this->pull_wavenumber_axis(pipe, wavenumber_axis);

            // set up cache handles
            typename data_manager<number>::datapipe::time_config_handle& t_handle = pipe.new_time_config_handle(this->time_sample_sns);
            typename data_manager<number>::datapipe::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

            // pull time-configuration information from the database
            typename data_manager<number>::datapipe::time_config_tag t_tag = pipe.new_time_config_tag();

            const std::vector<double>& t_values = t_handle.lookup_tag(t_tag);

            // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
            for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
	            {
                for(unsigned int l = 0; l < 2*this->mdl->get_N_fields(); l++)
	                {
                    for(unsigned int m = 0; m < 2*this->mdl->get_N_fields(); m++)
	                    {
		                    for(unsigned int n = 0; n < 2*this->mdl->get_N_fields(); n++)
			                    {
		                        std::array<unsigned int, 3> index_set = { l, m, n };
		                        if(this->active_indices.is_on(index_set))
			                        {
		                            typename data_manager<number>::datapipe::cf_kconfig_data_tag tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_threepf, this->mdl->flatten(l,m,n), this->time_sample_sns[i]);

				                        // have to take a copy of the data, rather than use a reference, because we are going to modify it in-place
		                            std::vector<number> line_data = k_handle.lookup_tag(tag);

				                        // the integrator produces correlation functions involving the canonical momenta,
				                        // not the derivatives. If the user wants derivatives then we have to shift.
				                        if(this->get_dot_meaning() == derived_line<number>::derivatives)
					                        this->shifter.shift(this->parent_task, this->mdl, pipe, this->kconfig_sample_sns, line_data, l, m, n, this->time_sample_sns[i], t_values[i]);

		                            std::string latex_label = "$" + this->make_LaTeX_label(l,m,n) + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
		                            std::string nonlatex_label = this->make_non_LaTeX_label(l,m,n) + " " + this->make_non_LaTeX_tag(t_values[i]);

		                            data_line<number> line = data_line<number>(data_line<number>::wavenumber_series, data_line<number>::correlation_function,
		                                                                       wavenumber_axis, line_data, latex_label, nonlatex_label);

		                            lines.push_back(line);
			                        }
			                    }
	                    }
	                }
	            }

            // detach pipe from output group
            this->detach(pipe);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void threepf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->derived_line<number>::write(out);
            this->threepf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void threepf_wavenumber_series<number>::serialize(serialization_writer& writer) const
	        {
            this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
                                   std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES));

            this->derived_line<number>::serialize(writer);
            this->threepf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }


	    }   // namespace derived_data

	}   // namespace transport


#endif //__field_wavenumber_series_H_
