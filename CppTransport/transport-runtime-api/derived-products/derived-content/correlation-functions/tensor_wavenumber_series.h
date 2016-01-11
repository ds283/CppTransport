//
// Created by David Seery on 18/12/14.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
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

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/tensor_twopf_line.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"


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
				                                   SQL_time_config_query tq, SQL_twopf_kconfig_query kq,
				                                   unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    tensor_twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~tensor_twopf_wavenumber_series() = default;


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
				    virtual tensor_twopf_wavenumber_series<number>* clone() const override { return new tensor_twopf_wavenumber_series<number>(static_cast<const tensor_twopf_wavenumber_series<number>&>(*this)); }


				    // WRITE TO A STREAM

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    //! serialize this object
				    virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing x-axis
            SQL_twopf_kconfig_query kquery;

            //! SQL query representing different lines
            SQL_time_config_query tquery;

			    };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
        tensor_twopf_wavenumber_series<number>::tensor_twopf_wavenumber_series(const twopf_list_task<number>& tk, index_selector<2>& sel,
                                                                               SQL_time_config_query tq, SQL_twopf_kconfig_query kq,
                                                                               unsigned int prec)
	        : derived_line<number>(tk, axis_class::wavenumber_axis, std::list<axis_value>{ axis_value::k_axis, axis_value::efolds_exit_axis }, prec),
	          tensor_twopf_line<number>(tk, sel),
	          wavenumber_series<number>(tk),
            tquery(tq),
            kquery(kq)
	        {
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
				tensor_twopf_wavenumber_series<number>::tensor_twopf_wavenumber_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
            tensor_twopf_line<number>(reader, finder),
            wavenumber_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
	        {
	        }


        template <typename number>
        void tensor_twopf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                                  const std::list<std::string>& tags) const
	        {
						// attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
            std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

		        // set up cache handles
		        typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->tquery);
		        typename datapipe<number>::kconfig_data_handle& k_handle = pipe.new_kconfig_data_handle(this->kquery);

		        // pull time-configuration data information from the database
		        time_config_tag<number> t_tag = pipe.new_time_config_tag();
		        const std::vector< time_config > t_values = tc_handle.lookup_tag(t_tag);

		        // loop through all components of the tensor twopf, pulling data from the database for each t-component
		        for(std::vector<time_config>::const_iterator t = t_values.begin(); t != t_values.end(); ++t)
			        {
				        for(unsigned int m = 0; m < 2; ++m)
					        {
						        for(unsigned int n = 0; n < 2; ++n)
							        {
						            std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
										        cf_kconfig_data_tag<number> tag = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_tensor_twopf, this->gadget.get_model()->tensor_flatten(m,n), t->serial);

										        // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
										        const std::vector<number>& line_data = k_handle.lookup_tag(tag);

								            std::string latex_label = "$" + this->make_LaTeX_label(m,n) + "\\;" + this->make_LaTeX_tag(t->t) + "$";
								            std::string nonlatex_label = this->make_non_LaTeX_label(m,n) + " " + this->make_non_LaTeX_tag(t->t);

								            data_line<number> line = data_line<number>(group, this->x_type, value_type::correlation_function_value, w_axis, line_data,
								                                                       this->get_LaTeX_label(m,n,t->t), this->get_non_LaTeX_label(m,n,t->t), this->is_spectral_index());

										        lines.push_back(line);
									        }
							        }
					        }
			        }

		        // detach pipe from output group
		        this->detach(pipe);
	        }


        template <typename number>
        std::string tensor_twopf_wavenumber_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, double t) const
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
        std::string tensor_twopf_wavenumber_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, double t) const
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
        // derived_line<> is not called from time_series<>. We have to call it for ourselves.
        template <typename number>
        void tensor_twopf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->tensor_twopf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is not called from time_series<>. We have to call it for ourselves.
        template <typename number>
        void tensor_twopf_wavenumber_series<number>::serialize(Json::Value& writer) const
	        {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_WAVENUMBER_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->tensor_twopf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }

	    }

	}

#endif //__tensor_wavenumber_series_H_
