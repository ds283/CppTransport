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


#ifndef CPPTRANSPORT_BACKGROUND_LINE_H
#define CPPTRANSPORT_BACKGROUND_LINE_H


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


#define CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE       "type"
#define CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_EPSILON    "epsilon"
#define CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_HUBBLE     "hubble"
#define CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_A_HUBBLE   "a-hubble"


namespace transport
	{

		namespace derived_data
			{

				template <typename number=default_number_type>
		    class background_line: public time_series<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! basic user-facing constructor
				    background_line(const twopf_db_task<number>& tk, SQL_time_query tq, background_quantity t, unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    background_line(Json::Value& reader, task_finder<number>& finder);

				    //! destructor
				    virtual ~background_line() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::background_line; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }


            // INTERFACE

          public:

            //! get line type
            background_quantity get_quantity() const { return(this->type); }


				    // DERIVE LIVES -- implements a 'derived_line' interface

		      public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags, slave_message_buffer& messages) const override;

		      protected:

				    //! epsilon line
				    void epsilon_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines,
                              const std::list<std::string>& tags, slave_message_buffer& messages,
				                      const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const;

		        //! Hubble line
		        void Hubble_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines,
                             const std::list<std::string>& tags, slave_message_buffer& messages,
		                         const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const;

		        //! aHubble line
		        void aHubble_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines,
                              const std::list<std::string>& tags, slave_message_buffer& messages,
		                          const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label() const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label() const;


		        // CLONE

		      public:

		        //! self-replicate
		        virtual background_line<number>* clone() const override { return new background_line<number>(static_cast<const background_line<number>&>(*this)); }


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

		        //! query object
				    SQL_time_query tquery;

				    //! line type
            background_quantity type;

			    };


				template <typename number>
				background_line<number>::background_line(const twopf_db_task<number>& tk, SQL_time_query tq, background_quantity t, unsigned int prec)
					: derived_line<number>(tk, axis_class::time, std::list<axis_value>{ axis_value::efolds }, prec),
		        time_series<number>(tk),
		        gadget(tk),
		        tquery(tq),
						type(t)
					{
					}


				template <typename number>
				background_line<number>::background_line(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
		        time_series<number>(reader),
						gadget(),
						tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY])
					{
						assert(this->parent_task != nullptr);
						gadget.set_task(this->parent_task, finder);

				    std::string type_string = reader[CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE].asString();
						type = background_quantity::epsilon;
						if(type_string == CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_EPSILON)         type = background_quantity::epsilon;
						else if(type_string == CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_HUBBLE)     type = background_quantity::Hubble;
						else if(type_string == CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_A_HUBBLE)   type = background_quantity::aHubble;
						else assert(false); // TODO: raise exception
					}


				template <typename number>
				void background_line<number>::serialize(Json::Value& writer) const
					{
						writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND_LINE);

						this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);

				    switch(this->type)
					    {
				        case background_quantity::epsilon:
					        writer[CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_EPSILON);
						      break;

				        case background_quantity::Hubble:
					        writer[CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_HUBBLE);
						      break;

                case background_quantity::aHubble:
					        writer[CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_A_HUBBLE);
						      break;
					    }

						this->time_series<number>::serialize(writer);
						this->derived_line<number>::serialize(writer);
					}


				template <typename number>
				void background_line<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                   const std::list<std::string>& tags, slave_message_buffer& messages) const
					{
				    // attach our datapipe to a content group
				    std::string group = this->attach(pipe, tags);

				    const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

				    // pull background data
				    typename datapipe<number>::time_data_handle& handle = pipe.new_time_data_handle(this->tquery);
				    std::vector<std::vector<number> > bg_data(t_axis.size());

				    for(unsigned int m = 0; m < 2 * this->gadget.get_N_fields(); ++m)
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

				    switch(this->type)
					    {
				        case background_quantity::epsilon:
					        this->epsilon_line(group, pipe, lines, tags, messages, t_axis, bg_data);
					        break;

				        case background_quantity::Hubble:
					        this->Hubble_line(group, pipe, lines, tags, messages, t_axis, bg_data);
					        break;

				        case background_quantity::aHubble:
					        this->aHubble_line(group, pipe, lines, tags, messages, t_axis, bg_data);
					        break;
					    }

				    this->detach(pipe);
					}


		    template <typename number>
		    void background_line<number>::epsilon_line(const std::string& group, datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                   const std::list<std::string>& tags, slave_message_buffer& messages,
		                                               const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const
			    {
		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

		        std::vector<number> line_data(t_axis.size());

				    for(unsigned int j = 0; j < line_data.size(); ++j)
					    {
				        line_data[j] = mdl->epsilon(this->gadget.get_integration_task()->get_params(), bg_data[j]);
					    }

            lines.emplace_back(group, this->x_type, value_type::dimensionless, t_axis, line_data,
                               this->get_LaTeX_label(), this->get_non_LaTeX_label(), messages);
			    }


		    template <typename number>
		    void background_line<number>::Hubble_line(const std::string& group, datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                  const std::list<std::string>& tags, slave_message_buffer& messages,
		                                              const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const
			    {
		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

		        std::vector<number> line_data(t_axis.size());

		        for(unsigned int j = 0; j < line_data.size(); ++j)
			        {
		            line_data[j] = mdl->H(this->gadget.get_integration_task()->get_params(), bg_data[j]) / this->gadget.get_integration_task()->get_params().get_Mp();
			        }

            lines.emplace_back(group, this->x_type, value_type::dimensionless, t_axis, line_data,
                               this->get_LaTeX_label(), this->get_non_LaTeX_label(), messages);
			    }


		    template <typename number>
		    void background_line<number>::aHubble_line(const std::string& group, datapipe<number>& pipe, std::list< data_line<number> >& lines,
		                                               const std::list<std::string>& tags, slave_message_buffer& messages,
		                                               const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const
			    {
		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

            typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->tquery);
            time_config_tag<number>  tc_tag                          = pipe.new_time_config_tag();
            std::vector<time_config> t_configs                       = tc_handle.lookup_tag(tc_tag);

		        std::vector<number> line_data(t_axis.size());

		        for(unsigned int j = 0; j < line_data.size(); ++j)
			        {
		            number a = exp(t_configs[j].t - this->gadget.get_integration_task()->get_N_horizon_crossing());
		            line_data[j] = a * mdl->H(this->gadget.get_integration_task()->get_params(), bg_data[j]) / this->gadget.get_integration_task()->get_params().get_Mp();
			        }

            lines.emplace_back(group, this->x_type, value_type::dimensionless, t_axis, line_data,
                               this->get_LaTeX_label(), this->get_non_LaTeX_label(), messages);
			    }


				template <typename number>
				std::string background_line<number>::get_LaTeX_label() const
					{
				    std::string label;

						if(this->label_set)
							{
								label = this->LaTeX_label;
							}
						else
							{
								switch(this->type)
									{
								    case background_quantity::epsilon:
									    label = "$" + std::string(CPPTRANSPORT_LATEX_EPSILON_SYMBOL) + "$";
											break;

								    case background_quantity::Hubble:
									    label = "$" + std::string(CPPTRANSPORT_LATEX_HUBBLE_SYMBOL) + "$";
											break;

                    case background_quantity::aHubble:
									    label = "$" + std::string(CPPTRANSPORT_LATEX_A_HUBBLE_SYMBOL) + "$";
											break;
									}
							}

						return(label);
					}


		    template <typename number>
		    std::string background_line<number>::get_non_LaTeX_label() const
			    {
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            switch(this->type)
			            {
		                case background_quantity::epsilon:
			                label = "$" + std::string(CPPTRANSPORT_NONLATEX_EPSILON_SYMBOL) + "$";
		                  break;

		                case background_quantity::Hubble:
			                label = "$" + std::string(CPPTRANSPORT_NONLATEX_HUBBLE_SYMBOL) + "$";
	  	                break;

                    case background_quantity::aHubble:
			                label = "$" + std::string(CPPTRANSPORT_NONLATEX_A_HUBBLE_SYMBOL) + "$";
  		                break;
			            }
			        }

		        return(label);
			    }


				template <typename number>
				void background_line<number>::write(std::ostream& out)
					{
						// TODO: implement me
					}

			}   // namespace derived_data


	}   // namespace transport


#endif //CPPTRANSPORT_BACKGROUND_LINE_H
