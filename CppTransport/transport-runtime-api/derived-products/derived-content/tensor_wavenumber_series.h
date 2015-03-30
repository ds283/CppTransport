//
// Created by David Seery on 18/12/14.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __tensor_wavenumber_series_H_
#define __tensor_wavenumber_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/tensor_twopf_line.h"


namespace transport
	{

    namespace derived_data
	    {

		    //! tensor two-point function wavenumber data line
		    template <typename number>
		    class tensor_twopf_wavenumber_series: public wavenumber_series<number>, public tensor_twopf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a tensor twopf wavenumber-series object
				    tensor_twopf_wavenumber_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
				                                   filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
				                                   unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    tensor_twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~tensor_twopf_wavenumber_series() = default;


				    // DERIVE LINES -- implements a 'derived_line' interface

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags) const override;


				    // CLONE

				    //! self-replicate
				    virtual derived_line<number>* clone() const override { return new tensor_twopf_wavenumber_series<number>(static_cast<const tensor_twopf_wavenumber_series<number>&>(*this)); }


				    // WRITE TO A STREAM

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    //! serialize this object
				    virtual void serialize(Json::Value& writer) const override;

			    };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
        tensor_twopf_wavenumber_series<number>::tensor_twopf_wavenumber_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
                                                                               filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
	        : derived_line<number>(tk, derived_line<number>::wavenumber_series, derived_line<number>::correlation_function, prec),
	          tensor_twopf_line<number>(tk, sel, kfilter),
	          wavenumber_series<number>(tk, tfilter)
	        {
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
				tensor_twopf_wavenumber_series<number>::tensor_twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
            tensor_twopf_line<number>(reader),
            wavenumber_series<number>(reader)
	        {
	        }


        template <typename number>
        void tensor_twopf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                                  const std::list<std::string>& tags) const
	        {
						// attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull wavenumber-axis data
            std::vector<double> wavenumber_axis;
		        this->pull_wavenumber_axis(pipe, wavenumber_axis);

		        // set up cache handles
		        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
		        typename datapipe<number>::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);

		        // pull time-configuration data information from the database
		        time_config_tag<number> t_tag = pipe.new_time_config_tag();
		        const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the tensor twopf, pulling data from the database for each t-component
		        for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
			        {
				        for(unsigned int m = 0; m < 2; m++)
					        {
						        for(unsigned int n = 0; n < 2; n++)
							        {
						            std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
										        cf_kconfig_data_tag<number> tag =
											                                                                     pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_tensor_twopf, this->mdl->tensor_flatten(m,n), this->time_sample_sns[i]);

										        // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
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
        // derived_line<> is not called from time_series<>. We have to call it for ourselves.
        template <typename number>
        void tensor_twopf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->derived_line<number>::write(out);
            this->tensor_twopf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is not called from time_series<>. We have to call it for ourselves.
        template <typename number>
        void tensor_twopf_wavenumber_series<number>::serialize(Json::Value& writer) const
	        {
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES);

            this->derived_line<number>::serialize(writer);
            this->tensor_twopf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }

	    }

	}

#endif //__tensor_wavenumber_series_H_
