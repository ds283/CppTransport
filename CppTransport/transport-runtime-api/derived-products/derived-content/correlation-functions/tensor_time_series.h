//
// Created by David Seery on 16/12/14.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __tensor_time_series_H_
#define __tensor_time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/tensor_twopf_line.h"


namespace transport
	{

    namespace derived_data
	    {

		    //! tensor two-point function time data line
		    template <typename number>
		    class tensor_twopf_time_series: public time_series<number>, public tensor_twopf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a tensor two-point function time series data object
				    tensor_twopf_time_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
				                             filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
				                             unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    tensor_twopf_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~tensor_twopf_time_series() = default;


				    // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
		        virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;


				    // CLONE

		      public:

				    //! self-replicate
				    virtual tensor_twopf_time_series<number>* clone() const override { return new tensor_twopf_time_series<number>(static_cast<const tensor_twopf_time_series<number>&>(*this)); }


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
		    tensor_twopf_time_series<number>::tensor_twopf_time_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
		                                                               filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }, prec),
            tensor_twopf_line<number>(tk, sel, kfilter),
            time_series<number>(tk, tfilter)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves
		    template <typename number>
		    tensor_twopf_time_series<number>::tensor_twopf_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      tensor_twopf_line<number>(reader, finder),
			      time_series<number>(reader)
			    {
			    }


        template <typename number>
        void tensor_twopf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                            const std::list<std::string>& tags) const
	        {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull time-axis data
		        const std::vector<double> t_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();

		        const typename std::vector< twopf_kconfig > k_values = k_handle.lookup_tag(k_tag);

		        // for each k-configuration, loop through all components of the tensor twopf and pull data from the database
		        for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); ++i)
			        {
				        for(unsigned int m = 0; m < 2; ++m)
					        {
						        for(unsigned int n = 0; n < 2; ++n)
							        {
						            std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
								            cf_time_data_tag<number> tag = pipe.new_cf_time_data_tag(data_tag<number>::cf_tensor_twopf, this->gadget.get_model()->tensor_flatten(m, n), this->kconfig_sample_sns[i]);

								            // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
								            const std::vector<number>& line_data = t_handle.lookup_tag(tag);

								            data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, t_axis, line_data,
								                                                       this->get_LaTeX_label(m,n,k_values[i]), this->get_non_LaTeX_label(m,n,k_values[i]));

								            lines.push_back(line);
									        }
							        }
					        }
			        }

            // detach pipe from output group
            this->detach(pipe);
	        }


        template <typename number>
        std::string tensor_twopf_time_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
	        {
            std::string tag = this->make_LaTeX_tag(k);
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
        std::string tensor_twopf_time_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
	        {
            std::string tag = this->make_non_LaTeX_tag(k);
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
		    // derived_line<> is not called from time_series<>. We have to call it for ourselves.
		    template <typename number>
		    void tensor_twopf_time_series<number>::write(std::ostream& out)
			    {
		        this->tensor_twopf_line<number>::write(out);
		        this->time_series<number>::write(out);
				    this->derived_line<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is not called from time_series<>. We have to call it for ourselves.
		    template <typename number>
		    void tensor_twopf_time_series<number>::serialize(Json::Value& writer) const
			    {
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES);

				    this->derived_line<number>::serialize(writer);
				    this->tensor_twopf_line<number>::serialize(writer);
				    this->time_series<number>::serialize(writer);
			    }

	    }

	}


#endif //__tensor_time_series_H_
