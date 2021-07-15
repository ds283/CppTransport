//
// Created by David Seery on 06/03/15.
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


#ifndef CPPTRANSPORT_R_TIME_SERIES_H
#define CPPTRANSPORT_R_TIME_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/r_line.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number=default_number_type>
		    class r_time_series: public time_series<number>, public r_line<number>
			    {

				    // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct an r_line time series data object
				    r_time_series(const zeta_twopf_db_task<number>& tk,
                          SQL_time_query tq, SQL_twopf_query kq,
				                  unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    r_time_series(Json::Value& reader, task_finder<number>& finder);

				    //! destructor is default
				    virtual ~r_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::r_time; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


				    // DERIVE LINES -- implements a 'time series' interface

		      public:

				    //! generate data lines for plotting
				    data_line_set<number> derive_lines
				      (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(const twopf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(const twopf_kconfig& k) const;


				    // CLONE

		      public:

				    //! self-replicate
				    virtual r_time_series<number>* clone() const override { return new r_time_series<number>(static_cast<const r_time_series<number>&>(*this)); }


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a serializable interface

		      public:

				    //! serialize this object
				    virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing x-axis
            SQL_time_query tquery;

            //! SQL query representing different lines
            SQL_twopf_query kquery;

			    };


				// note that because time_series<> inherits virtually from derived_line<>, the constructor for
				// derived_line<> is not called automatically during construction of time_series<>.
				// We have to call it ourselves
				template <typename number>
				r_time_series<number>::r_time_series(const zeta_twopf_db_task<number>& tk,
				                                     SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
          : derived_line<number>(make_derivable_task_set_element(tk,
                                                                 precomputed_products(true, false, false, false,
                                                                                      false, false, false, false),
                                                                 r_line_impl::ZETA_TASK),
                                 make_derivable_task_set_element(*(tk.get_parent_task()), false, false, false,
                                                                 r_line_impl::INTEGRATION_TASK),
                                 axis_class::time, {axis_value::efolds}, prec),
					  r_line<number>(tk),
					  time_series<number>(tk),
            tquery(tq),
            kquery(kq)
					{
					}


				// note that because time_series<> inherits virtually from derived_line<>, the constructor for
				// derived_line<> is not called automatically during construction of time_series<>.
				// We have to call it ourselves
				template <typename number>
				r_time_series<number>::r_time_series(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
		        r_line<number>(reader, finder),
		        time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
					{
					}


		    template <typename number>
		    data_line_set<number> r_time_series<number>::derive_lines
		      (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const
			    {
            content_group_name_set groups;

						// attach our datapipe to a content group for the zeta part of r
            std::string group = this->attach(pipe, tags, r_line_impl::ZETA_TASK);
            groups.insert(group);

				    // pull time-axis data
				    const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

				    // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& k_handle = pipe.new_twopf_kconfig_handle(this->kquery);
		        typename datapipe<number>::time_zeta_handle    & z_handle = pipe.new_time_zeta_handle(this->tquery);

				    // pull k-configuration information from the database
		        twopf_kconfig_tag<number>                 k_tag    = pipe.new_twopf_kconfig_tag();
		        const typename std::vector<twopf_kconfig> k_values = k_handle.lookup_tag(k_tag);

		        // store time history of zeta data for each k-configuration, indexed by the configuration serial number
		        std::map< unsigned int, std::vector<number> > zeta_data;

				    // for each k-configuration, pull data from the database
				    for(const auto& k_value : k_values)
					    {
				        zeta_twopf_time_data_tag<number> zeta_tag = pipe.new_zeta_twopf_time_data_tag(k_value);

				        // this time we can take a reference
				        zeta_data[k_value.serial] = z_handle.lookup_tag(zeta_tag);
					    }

				    pipe.detach();

				    // attach datapipe to a content group for the tensor part of r
            group = this->attach(pipe, tags, r_line_impl::INTEGRATION_TASK);
            groups.insert(group);

						// rebind handles
		        typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(this->tquery);

		        data_line_set<number> lines;

				    for(const auto& k_value : k_values)
					    {
				        cf_time_data_tag<number> tensor_tag = pipe.new_cf_time_data_tag(cf_data_type::cf_tensor_twopf, this->gadget.get_model()->tensor_flatten(0,0), k_value.serial);

				        // must copy this, because we will call lookup_tag() again
				        const std::vector<number> tensor_data = t_handle.lookup_tag(tensor_tag);
						    assert(tensor_data.size() == zeta_data[k_value.serial].size());

				        std::vector<number> line_data(tensor_data.size());

				        for(unsigned int j = 0; j < tensor_data.size(); ++j)
					        {
					          // The factor of 4.0 is required because CppTransport stores the power spectrum for a single
					          // tensor polarization.
					          // The conventionally-defined tensor power spectrum is the sum over polarizations, which yields
					          // a factor of 2. But also, in our normalization there is another factor of 2 that comes from
					          // tracing over the polarization tensors.
				            line_data[j] = 4.0 * tensor_data[j] / zeta_data[k_value.serial][j];
					        }

				        time_data_line_factory(*this, lines, groups, this->x_type, value_type::r, t_axis, line_data,
                                       this->get_LaTeX_label(k_value), this->get_non_LaTeX_label(k_value), messages);
					    }

				    // detach pipe from content group
				    this->detach(pipe);
				    return lines;
			    }


		    template <typename number>
		    std::string r_time_series<number>::get_LaTeX_label(const twopf_kconfig& k) const
			    {
		        std::string tag = this->make_LaTeX_tag(k);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->LaTeX_label;
			        }
		        else
			        {
		            label = "$" + this->make_LaTeX_label() + "$";
			        }

		        if(this->use_tags) label += " $" + tag + "$";
		        return(label);
			    }


		    template <typename number>
		    std::string r_time_series<number>::get_non_LaTeX_label(const twopf_kconfig& k) const
			    {
		        std::string tag = this->make_non_LaTeX_tag(k);
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            label = this->make_non_LaTeX_label();
			        }

		        if(this->use_tags) label += " " + tag;
		        return(label);
			    }


				// note that because time_series<> inherits virtually from derived_line<>, the write method for
				// derived_line<> is *not* called from time_series<>. We have to call it ourselves.
				template <typename number>
				void r_time_series<number>::write(std::ostream& out)
					{
				    this->r_line<number>::write(out);
				    this->time_series<number>::write(out);
						this->derived_line<number>::write(out);
					}


				// note that because time_series<> inherits virtually form derived_line<>, the serialize method for
				// derived_line<> is *not* called from time_series<>. We have to call it ourselves.
				template <typename number>
				void r_time_series<number>::serialize(Json::Value& writer) const
					{
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    this->derived_line<number>::serialize(writer);
				    this->r_line<number>::serialize(writer);
				    this->time_series<number>::serialize(writer);
					}


			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_R_TIME_SERIES_H
