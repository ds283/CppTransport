//
// Created by David Seery on 06/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __r_time_series_H_
#define __r_time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/r_line.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class r_time_series: public time_series<number>, public r_line<number>
			    {

				    // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct an r_line time series data object
				    r_time_series(const zeta_twopf_list_task<number>& tk, filter::time_filter, filter::twopf_kconfig_filter kfilter,
				                  unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    r_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~r_time_series() = default;


				    // DERIVE LINES -- implements a 'time series' interface

		      public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags) const override;


				    // CLONE

		      public:

				    //! self-replicate
				    virtual derived_line<number>* clone() const override { return new r_time_series<number>(static_cast<const r_time_series<number>&>(*this)); }


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a serializable interface

		      public:

				    //! serialize this object
				    virtual void serialize(Json::Value& writer) const override;

			    };


				// note that because time_series<> inherits virtually from derived_line<>, the constructor for
				// derived_line<> is not called automatically during construction of time_series<>.
				// We have to call it ourselves
				template <typename number>
				r_time_series<number>::r_time_series(const zeta_twopf_list_task<number>& tk, filter::time_filter tfilter,
				                                     filter::twopf_kconfig_filter kfilter, unsigned int prec)
					: derived_line<number>(tk, time_axis, prec),
					  r_line<number>(tk, kfilter),
					  time_series<number>(tk, tfilter)
					{
					}


				// note that because time_series<> inherits virtually from derived_line<>, the constructor for
				// derived_line<> is not called automatically during construction of time_series<>.
				// We have to call it ourselves
				template <typename number>
				r_time_series<number>::r_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
		        r_line<number>(reader, finder),
		        time_series<number>(reader)
					{
					}


		    template <typename number>
		    void r_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                             const std::list<std::string>& tags) const
			    {
						// attach our datapipe to an output group
		        this->attach(pipe, tags, this->parent_task);

				    // pull time-axis data
				    const std::vector<double> t_axis = this->pull_time_axis(pipe);

				    // set up cache handles
				    typename datapipe<number>::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
				    typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->time_sample_sns);

				    // pull k-configuration information from the database
				    twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
				    const typename std::vector< twopf_configuration > k_values = k_handle.lookup_tag(k_tag);

		        std::vector< std::vector<number> > zeta_data;
				    zeta_data.resize(this->kconfig_sample_sns.size());

				    // for each k-configuration, pull data from the database
				    for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
					    {
				        zeta_twopf_time_data_tag<number> zeta_tag = pipe.new_zeta_twopf_time_data_tag(k_values[i]);

				        // this time we can take a reference
				        zeta_data[i] = z_handle.lookup_tag(zeta_tag);
					    }

				    pipe.detach();

				    // attach datapipe to an output group for the tensor part of r
				    postintegration_task<number>* ptk = dynamic_cast< postintegration_task<number>* >(this->parent_task);
				    assert(ptk != nullptr);

				    this->attach(pipe, tags, ptk->get_parent_task());

						// rebind handles
		        typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

				    for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
					    {
				        cf_time_data_tag<number> tensor_tag = pipe.new_cf_time_data_tag(data_tag<number>::cf_tensor_twopf, this->gadget.get_model()->tensor_flatten(0,0), this->kconfig_sample_sns[i]);

				        // must copy this, because we will call lookup_tag() again
				        const std::vector<number> tensor_data = t_handle.lookup_tag(tensor_tag);

				        std::vector<number> line_data(tensor_data.size());

				        for(unsigned int j = 0; j < tensor_data.size(); j++)
					        {
				            line_data[j] = tensor_data[j] / zeta_data[i][j];
					        }

				        std::string latex_label    = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(k_values[i]) + "$";
				        std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(k_values[i]);

				        data_line<number> line = data_line<number>(time_axis, r_value, t_axis, line_data, latex_label, nonlatex_label);

				        lines.push_back(line);
					    }

				    // detach pipe from output group
				    this->detach(pipe);
			    }


				// note that because time_series<> inherits virtually from derived_line<>, the write method for
				// derived_line<> is *not* called from time_series<>. We have to call it ourselves.
				template <typename number>
				void r_time_series<number>::write(std::ostream& out)
					{
						this->derived_line<number>::write(out);
						this->r_line<number>::write(out);
						this->time_series<number>::write(out);
					}


				// note that because time_series<> inherits virtually form derived_line<>, the serialize method for
				// derived_line<> is *not* called from time_series<>. We have to call it ourselves.
				template <typename number>
				void r_time_series<number>::serialize(Json::Value& writer) const
					{
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES);

				    this->derived_line<number>::serialize(writer);
				    this->r_line<number>::serialize(writer);
				    this->time_series<number>::serialize(writer);
					}


			}   // namespace derived_data

	}   // namespace transport


#endif //__r_time_series_H_
