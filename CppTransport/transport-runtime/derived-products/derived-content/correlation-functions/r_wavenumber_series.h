//
// Created by David Seery on 09/03/15.
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


#ifndef CPPTRANSPORT_R_WAVENUMBER_SERIES_H
#define CPPTRANSPORT_R_WAVENUMBER_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/r_line.h"


namespace transport
	{

		namespace derived_data
			{

				//! tensor-to-scalar ratio wavenumber data line
				template <typename number=default_number_type>
		    class r_wavenumber_series: public wavenumber_series<number>, public r_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! construct an r_line wavenumber-series object
		        r_wavenumber_series(const zeta_twopf_db_task<number>& tk,
                                SQL_time_query tq, SQL_twopf_query kq,
		                            unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        r_wavenumber_series(Json::Value& reader, task_finder<number>& finder);

		        //! destructor is default
		        virtual ~r_wavenumber_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::r_wavenumber; }


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
		        std::string get_LaTeX_label(double t) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(double t) const;


		        // CLONE

		        //! self-replicate
		        virtual r_wavenumber_series<number>* clone() const override { return new r_wavenumber_series<number>(static_cast<const r_wavenumber_series<number>&>(*this)); }


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
				// We have to call it ourselves.
				template <typename number>
				r_wavenumber_series<number>::r_wavenumber_series(const zeta_twopf_db_task<number>& tk,
				                                                 SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
          : derived_line<number>(make_derivable_task_set_element(tk,
                                                                 precomputed_products(true, false, false,
                                                                                      false,
                                                                                      false, false, false,
                                                                                      false),
                                                                 r_line_impl::ZETA_TASK),
                                 make_derivable_task_set_element(*(tk.get_parent_task()), false, false,
                                                                 false,
                                                                 r_line_impl::INTEGRATION_TASK),
                                 axis_class::wavenumber, {axis_value::k, axis_value::efolds_exit}, prec),
					  r_line<number>(tk),
					  wavenumber_series<number>(tk),
            tquery(tq),
            kquery(kq)
					{
					}


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves.
				template <typename number>
				r_wavenumber_series<number>::r_wavenumber_series(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
		        r_line<number>(reader, finder),
		        wavenumber_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
			    {
			    }


		    template <typename number>
		    void r_wavenumber_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
		                                                   const tag_list& tags, slave_message_buffer& messages) const
			    {
            content_group_name_set groups;

						// attach datapipe to a content group for the zeta part of r
            std::string group = this->attach(pipe, tags, r_line_impl::ZETA_TASK);
            groups.insert(group);

				    // pull wavenumber-axis data
		        std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

				    // set up cache handles
		        typename datapipe<number>::time_config_handle & tc_handle = pipe.new_time_config_handle(this->tquery);
		        typename datapipe<number>::kconfig_zeta_handle& z_handle  = pipe.new_kconfig_zeta_handle(this->kquery);

				    // pull time-configuration data from the database
		        time_config_tag<number>        t_tag    = pipe.new_time_config_tag();
		        const std::vector<time_config> t_values = tc_handle.lookup_tag(t_tag);

		        std::vector< std::vector<number> > zeta_data;
				    zeta_data.resize(t_values.size());

				    // for each t-configuration, pull zeta data from the database and cache it
            unsigned int i = 0;
				    for(auto t = t_values.begin(); t != t_values.end(); ++t, ++i)
					    {
						    zeta_twopf_kconfig_data_tag<number> zeta_tag = pipe.new_zeta_twopf_kconfig_data_tag(t->serial);

						    // this time we can take a reference
						    zeta_data[i] = z_handle.lookup_tag(zeta_tag);
					    }

				    pipe.detach();

				    // attach datapipe to a content group for the tensor part of r
				    group = this->attach(pipe, tags, r_line_impl::INTEGRATION_TASK);
            groups.insert(group);

				    // rebind handles
		        typename datapipe<number>::kconfig_data_handle& k_handle = pipe.new_kconfig_zeta_handle(this->kquery);

				    // for each t-configuration, pull tensor data from the database and create a data_line<> for r
				    i = 0; // reset counter
				    for(auto t = t_values.begin(); t != t_values.end(); ++t, ++i)
					    {
				        cf_kconfig_data_tag<number> tensor_tag =
					                                    pipe.new_cf_kconfig_data_tag(cf_data_type::cf_tensor_twopf, this->gadget.get_model()->tensor_flatten(0,0), t->serial);

				        // can take a reference here to avoid a copy
				        const std::vector<number>& tensor_data = k_handle.lookup_tag(tensor_tag);
						    assert(tensor_data.size() == zeta_data[i].size());

				        std::vector<number> line_data(tensor_data.size());

				        for(unsigned int j = 0; j < tensor_data.size(); ++j)
					        {
                    // The factor of 4.0 is required because CppTransport stores the power spectrum for a single
                    // tensor polarization.
                    // The conventionally-defined tensor power spectrum is the sum over polarizations, which yields
                    // a factor of 2. But also, in our normalization there is another factor of 2 that comes from
                    // tracing over the polarization tensors.
				            line_data[j] = 4.0 * tensor_data[j] / zeta_data[i][j];
					        }

                lines.emplace_back(groups, this->x_type, value_type::r, w_axis, line_data,
                                   this->get_LaTeX_label(t->t), this->get_non_LaTeX_label(t->t), messages, this->is_spectral_index());
					    }

				    // detach pipe from content group
				    this->detach(pipe);
			    }


		    template <typename number>
		    std::string r_wavenumber_series<number>::get_LaTeX_label(double t) const
			    {
		        std::string tag = this->make_LaTeX_tag(t);
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
		    std::string r_wavenumber_series<number>::get_non_LaTeX_label(double t) const
			    {
		        std::string tag = this->make_non_LaTeX_tag(t);
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
		    void r_wavenumber_series<number>::write(std::ostream& out)
			    {
		        this->r_line<number>::write(out);
		        this->wavenumber_series<number>::write(out);
		        this->derived_line<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually form derived_line<>, the serialize method for
		    // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
		    template <typename number>
		    void r_wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
		        writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_R_WAVENUMBER_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

		        this->derived_line<number>::serialize(writer);
		        this->r_line<number>::serialize(writer);
		        this->wavenumber_series<number>::serialize(writer);
			    }



			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_R_WAVENUMBER_SERIES_H
