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

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/tensor_twopf_line.h"


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
				    tensor_twopf_time_series(serialization_reader* reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~tensor_twopf_time_series() = default;


				    // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
		        virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                  const std::list<std::string>& tags) const override;


				    // CLONE

		      public:

				    //! self-replicate
				    virtual derived_line<number>* clone() const override { return new tensor_twopf_time_series<number>(static_cast<const tensor_twopf_time_series<number>&>(*this)); }


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a serializable interface

		      public:

				    //! serialize this object
				    virtual void serialize(serialization_writer& writer) const override;

			    };


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves
		    template <typename number>
		    tensor_twopf_time_series<number>::tensor_twopf_time_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
		                                                               filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
			    : derived_line<number>(tk, derived_line<number>::time_series, derived_line<number>::correlation_function, prec),
            tensor_twopf_line<number>(tk, sel, kfilter),
            time_series<number>(tk, tfilter)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves
		    template <typename number>
		    tensor_twopf_time_series<number>::tensor_twopf_time_series(serialization_reader* reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader, finder),
			      tensor_twopf_line<number>(reader),
			      time_series<number>(reader)
			    {
		        assert(reader != nullptr);
				    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
			    }


        template <typename number>
        void tensor_twopf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                            const std::list<std::string>& tags) const
	        {
            // attach our datapipe to an output group
		        this->attach(pipe, tags);

		        // pull time-axis data
		        const std::vector<double> time_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(this->time_sample_sns);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();

		        const typename std::vector< twopf_configuration > k_values = k_handle.lookup_tag(k_tag);

		        // for each k-configuration, loop through all components of the tensor twopf and pull data from the database
		        for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
			        {
				        for(unsigned int m = 0; m < 2; m++)
					        {
						        for(unsigned int n = 0; n < 2; n++)
							        {
						            std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
								            cf_time_data_tag<number> tag =
									                                                                      pipe.new_cf_time_data_tag(data_tag<number>::cf_tensor_twopf, this->mdl->tensor_flatten(m, n), this->kconfig_sample_sns[i]);

								            // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
								            const std::vector<number>& line_data = t_handle.lookup_tag(tag);

								            std::string latex_label = "$" + this->make_LaTeX_label(m,n) + "\\;" + this->make_LaTeX_tag(k_values[i]) + "$";
								            std::string nonlatex_label = this->make_non_LaTeX_label(m,n) + " " + this->make_non_LaTeX_tag(k_values[i]);

								            data_line<number> line = data_line<number>(data_line<number>::time_series, data_line<number>::correlation_function,
								                                                       time_axis, line_data, latex_label, nonlatex_label);

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
		    void tensor_twopf_time_series<number>::write(std::ostream& out)
			    {
				    this->derived_line<number>::write(out);
				    this->tensor_twopf_line<number>::write(out);
				    this->time_series<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is not called from time_series<>. We have to call it for ourselves.
		    template <typename number>
		    void tensor_twopf_time_series<number>::serialize(serialization_writer& writer) const
			    {
				    writer.write_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE,
				                       std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES));

				    this->derived_line<number>::serialize(writer);
				    this->tensor_twopf_line<number>::serialize(writer);
				    this->time_series<number>::serialize(writer);
			    }

	    }

	}


#endif //__tensor_time_series_H_
