//
// Created by David Seery on 01/06/15.
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


#ifndef __largest_u2_line_H_
#define __largest_u2_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime/derived-products/derived-content/utilities/integration_task_gadget.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"
#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query_helper.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class largest_u2_line: public time_series<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! basic user-facing constructor
				    largest_u2_line(const twopf_db_task<number>& tk, SQL_time_query tq, SQL_twopf_query,
                            unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    largest_u2_line(Json::Value& reader, task_finder<number>& finder);

				    //! destructor
				    virtual ~largest_u2_line() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::largest_u2; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


				    // DERIVE LIVES -- implements a 'derived_line' interface

		      public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags, slave_message_buffer& messages) const override;

		      protected:

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(const twopf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(const twopf_kconfig& k) const;


		        // CLONE

		      public:

		        //! self-replicate
		        virtual largest_u2_line<number>* clone() const override { return new largest_u2_line<number>(static_cast<const largest_u2_line<number>&>(*this)); }


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a serializable interface

		      public:

		        //! Serialize this object
		        virtual void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

		      protected:

		        //! integration task gadget
		        integration_task_gadget<number> gadget;

		        //! time query object
				    SQL_time_query tquery;

            //! kconfig query object
            SQL_twopf_query kquery;

			    };


				template <typename number>
				largest_u2_line<number>::largest_u2_line(const twopf_db_task<number>& tk, SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
					: derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
		        time_series<number>(tk),
		        gadget(tk),
		        tquery(tq),
            kquery(kq)
					{
					}


				template <typename number>
				largest_u2_line<number>::largest_u2_line(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
		        time_series<number>(reader),
						gadget(),
						tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
					{
						assert(this->parent_task != nullptr);
						gadget.set_task(this->parent_task, finder);
					}


				template <typename number>
				void largest_u2_line<number>::serialize(Json::Value& writer) const
					{
						writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U2_LINE);

						this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

						this->time_series<number>::serialize(writer);
						this->derived_line<number>::serialize(writer);
					}


				template <typename number>
				void largest_u2_line<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                   const std::list<std::string>& tags, slave_message_buffer& messages) const
					{
				    // attach our datapipe to a content group
				    std::string group = this->attach(pipe, tags);

				    const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

            // get time config data
            typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->tquery);
            time_config_tag<number>  tc_tag                          = pipe.new_time_config_tag();
            std::vector<time_config> t_configs                       = tc_handle.lookup_tag(tc_tag);

            // get kconfig data
            typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kquery);
            twopf_kconfig_tag<number>  kc_tag                          = pipe.new_twopf_kconfig_tag();
            std::vector<twopf_kconfig> k_configs                       = kc_handle.lookup_tag(kc_tag);

				    // pull background data
				    typename datapipe<number>::time_data_handle& handle = pipe.new_time_data_handle(this->tquery);
				    std::vector<std::vector<number> > bg_data(t_axis.size());

            unsigned int Nfields = this->gadget.get_N_fields();

				    for(unsigned int m = 0; m < 2*Nfields; ++m)
					    {
				        std::array<unsigned int, 1>      index_set = { m };
				        background_time_data_tag<number> tag       = pipe.new_background_time_data_tag(this->gadget.get_model()->flatten(m));

				        // safe to take a reference here and avoid a copy
				        const std::vector<number>& bg = handle.lookup_tag(tag);

				        for(unsigned int j = 0; j < t_axis.size(); ++j)
					        {
				            bg_data[j].push_back(bg[j]);
					        }
					    }

		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

            std::vector<number> u2_tensor(2*Nfields * 2*Nfields);

            for(std::vector<twopf_kconfig>::iterator t = k_configs.begin(); t != k_configs.end(); ++t)
              {
                std::vector<number> line_data(t_axis.size());

                for(unsigned int j = 0; j < line_data.size(); ++j)
                  {
                    mdl->u2(this->gadget.get_integration_task(), bg_data[j], t->k_comoving, t_configs[j].t, u2_tensor);
                    number val = -std::numeric_limits<number>::max();

                    for(unsigned int m = 0; m < 2*Nfields; ++m)
                      {
                        for(unsigned int n = 0; n < 2*Nfields; ++n)
                          {
                            if(mdl->is_momentum(m) && mdl->is_field(n)) // only look at the momentum-field block, which is M/H^2; the other blocks aren't relate to SR
                              {
                                number value = std::abs(u2_tensor[mdl->flatten(m,n)]);
                                if(value > val) val = value;
                              }
                          }
                      }

                    line_data[j] = val;
                  }

                lines.emplace_back(group, this->x_type, value_type::dimensionless_value, t_axis, line_data,
                                   this->get_LaTeX_label(*t), this->get_non_LaTeX_label(*t), messages);
              }

            this->detach(pipe);
			    }


				template <typename number>
				std::string largest_u2_line<number>::get_LaTeX_label(const twopf_kconfig& k) const
					{
				    std::string label;

						if(this->label_set)
							{
								label = this->LaTeX_label;
							}
						else
							{
                label = "$" + std::string(CPPTRANSPORT_LATEX_LARGEST_U2_SYMBOL) + "$";

                if(this->use_tags) label += " $" + this->make_LaTeX_tag(k) + "$";
							}

						return(label);
					}


		    template <typename number>
		    std::string largest_u2_line<number>::get_non_LaTeX_label(const twopf_kconfig& k) const
			    {
            std::string label;

            unsigned int N_fields = this->gadget.get_N_fields();
            const std::vector<std::string>& field_names = this->gadget.get_model()->get_field_names();

            if(this->label_set)
              {
                label = this->non_LaTeX_label;
              }
            else
              {
                label = CPPTRANSPORT_NONLATEX_LARGEST_U2_SYMBOL;

                if(this->use_tags) label += " " + this->make_non_LaTeX_tag(k);
              }

            return(label);
			    }


				template <typename number>
				void largest_u2_line<number>::write(std::ostream& out)
					{
						// TODO: implement me
					}

			}   // namespace derived_data


	}   // namespace transport


#endif //__largest_u2_line_H_
