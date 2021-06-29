//
// Created by David Seery on 18/12/14.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_TENSOR_WAVENUMBER_SERIES_H
#define CPPTRANSPORT_TENSOR_WAVENUMBER_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/tensor_twopf_line.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
	{

    namespace derived_data
	    {

		    //! tensor two-point function wavenumber data line
		    template <typename number=default_number_type>
		    class tensor_twopf_wavenumber_series: public wavenumber_series<number>, public tensor_twopf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a tensor twopf wavenumber-series object
				    tensor_twopf_wavenumber_series(const twopf_db_task<number>& tk, index_selector<2> sel,
				                                   SQL_time_query tq, SQL_twopf_query kq,
				                                   unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    tensor_twopf_wavenumber_series(Json::Value& reader, task_finder<number>& finder);

				    virtual ~tensor_twopf_wavenumber_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::tensor_twopf_wavenumber; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


				    // DERIVE LINES -- implements a 'derived_line' interface

          public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const tag_list& tags, slave_message_buffer& messages) const override;

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
            SQL_twopf_query kquery;

            //! SQL query representing different lines
            SQL_time_query tquery;

			    };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
        tensor_twopf_wavenumber_series<number>::tensor_twopf_wavenumber_series(const twopf_db_task<number>& tk, index_selector<2> sel,
                                                                               SQL_time_query tq, SQL_twopf_query kq,
                                                                               unsigned int prec)
	        : derived_line<number>(make_derivable_task_set_element(tk, false, false, false),
                                 axis_class::wavenumber, { axis_value::k, axis_value::efolds_exit }, prec),
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
				tensor_twopf_wavenumber_series<number>::tensor_twopf_wavenumber_series(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
            tensor_twopf_line<number>(reader, finder),
            wavenumber_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
	        {
	        }


        template <typename number>
        void tensor_twopf_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                                  const tag_list& tags, slave_message_buffer& messages) const
	        {
						// attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
            std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

		        // set up cache handles
            typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kquery);
		        typename datapipe<number>::time_config_handle& tc_handle   = pipe.new_time_config_handle(this->tquery);
		        typename datapipe<number>::kconfig_data_handle& k_handle   = pipe.new_kconfig_data_handle(this->kquery);

		        // pull time-configuration data information from the database
		        time_config_tag<number> t_tag = pipe.new_time_config_tag();
		        const std::vector< time_config > t_values = tc_handle.lookup_tag(t_tag);

            // pull k-configuration information from the database
            twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
            const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

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
										        std::vector<number> line_data = k_handle.lookup_tag(tag);
                            assert(line_data.size() == k_values.size());

                            value_type value;
                            if(this->dimensionless)
                              {
                                typename std::vector<number>::iterator     l_pos = line_data.begin();
                                std::vector<twopf_kconfig>::const_iterator k_pos = k_values.begin();
                                for(; l_pos != line_data.end() && k_pos != k_values.end(); ++l_pos, ++k_pos)
                                  {
                                    *l_pos *= 1.0 / (2.0*M_PI*M_PI);
                                  }
                                value = value_type::dimensionless;
                              }
                            else
                              {
                                typename std::vector<number>::iterator     l_pos = line_data.begin();
                                std::vector<twopf_kconfig>::const_iterator k_pos = k_values.begin();
                                for(; l_pos != line_data.end() && k_pos != k_values.end(); ++l_pos, ++k_pos)
                                  {
                                    double k_cube = k_pos->k_comoving * k_pos->k_comoving * k_pos->k_comoving;
                                    *l_pos *=  1.0 / k_cube;
                                  }
                                value = value_type::correlation_function;
                              }

                            lines.emplace_back(group, this->x_type, value, w_axis, line_data,
                                               this->get_LaTeX_label(m,n,t->t), this->get_non_LaTeX_label(m,n,t->t), messages, this->is_spectral_index());
									        }
							        }
					        }
			        }

		        // detach pipe from content group
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

#endif //CPPTRANSPORT_TENSOR_WAVENUMBER_SERIES_H
