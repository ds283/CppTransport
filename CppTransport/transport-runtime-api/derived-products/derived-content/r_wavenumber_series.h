//
// Created by David Seery on 09/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __r_wavenumber_series_H_
#define __r_wavenumber_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/r_line.h"


namespace transport
	{

		namespace derived_data
			{

				//! tensor-to-scalar ratio wavenumber data line
				template <typename number>
		    class r_wavenumber_series: public wavenumber_series<number>, public r_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct an r_line wavenumber-series object
		        r_wavenumber_series(const twopf_list_task<number>& tk, filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
		                            unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        r_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

		        virtual ~r_wavenumber_series() = default;


		        // DERIVE LINES -- implements a 'derived_line' interface

		        //! generate data lines for plotting
		        virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;


		        // CLONE

		        //! self-replicate
		        virtual derived_line<number>* clone() const override { return new r_wavenumber_series<number>(static_cast<const r_wavenumber_series<number>&>(*this)); }


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
				// We have to call it ourselves.
				template <typename number>
				r_wavenumber_series<number>::r_wavenumber_series(const twopf_list_task<number>& tk, filter::time_filter tfilter,
				                                                 filter::twopf_kconfig_filter kfilter, unsigned int prec)
					: derived_line<number>(tk, wavenumber_axis, prec),
					  r_line<number>(tk, kfilter),
					  wavenumber_series<number>(tk, tfilter)
					{
					}


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves.
				template <typename number>
				r_wavenumber_series<number>::r_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
		        r_line<number>(reader, finder),
		        wavenumber_series<number>(reader)
			    {
			    }


		    template <typename number>
		    void r_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                                   const std::list<std::string>& tags) const
			    {
						// attach our datapipe to an output group
				    this->attach(pipe, tags);

				    // pull wavenumber-axis data
		        std::vector<double> w_axis;
				    this->pull_wavenumber_axis(pipe, w_axis);

				    // set up cache handles
				    typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename datapipe<number>::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kconfig_sample_sns);
				    typename datapipe<number>::kconfig_zeta_handle& z_handle = pipe.new_kconfig_zeta_handle(this->kconfig_sample_sns);

				    // pull time-configuration data from the database
				    time_config_tag<number> t_tag = pipe.new_time_config_tag();
				    const std::vector<double> t_values = tc_handle.lookup_tag(t_tag);

				    // for each t-configuration, pull data from the database
				    for(unsigned int i = 0; i < this->time_sample_sns.size(); i++)
					    {
						    cf_kconfig_data_tag<number> tensor_tag =
							    pipe.new_cf_kconfig_data_tag(data_tag<number>::cf_tensor_twopf, this->gadget.get_model()->tensor_flatten(0,0), this->time_sample_sns[i]);

						    // must copy this, becaue we will call lookup_tag() again
						    const std::vector<number> tensor_data = k_handle.lookup_tag(tensor_tag);

						    zeta_twopf_kconfig_data_tag<number> zeta_tag = pipe.new_zeta_twopf_kconfig_data_tag(this->time_sample_sns[i]);

						    // this time we can take a reference
						    const std::vector<number>& zeta_data = z_handle.lookup_tag(zeta_tag);

				        std::vector<number> line_data(zeta_data.size());

				        for(unsigned int j = 0; j < zeta_data.size(); j++)
					        {
						        line_data[j] = tensor_data[j] / zeta_data[j];
					        }

				        std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(t_values[i]) + "$";
				        std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(t_values[i]);

				        data_line<number> line = data_line<number>(wavenumber_axis, r_value,
				                                                   w_axis, line_data, latex_label, nonlatex_label);

						    lines.push_back(line);
					    }

				    // detach pipe from output group
				    this->detach(pipe);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the write method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void r_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->derived_line<number>::write(out);
		        this->r_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually form derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void r_wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES);

		        this->derived_line<number>::serialize(writer);
		        this->r_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }



			}   // namespace derived_data

	}   // namespace transport


#endif //__r_wavenumber_series_H_
