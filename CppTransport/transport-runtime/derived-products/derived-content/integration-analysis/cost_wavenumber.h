//
// Created by David Seery on 25/04/15.
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


#ifndef CPPTRANSPORT_COST_WAVENUMBER_SERIES_H
#define CPPTRANSPORT_COST_WAVENUMBER_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/derived-content/integration-analysis/cost_metrics.h"

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/utilities/integration_task_gadget.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"
#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query_helper.h"


namespace transport
	{

		namespace derived_data
			{

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE    = "type";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TWOPF   = "twopf";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_THREEPF = "threepf";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC  = "metric";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TIME    = "time";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_STEPS   = "steps";


				template <typename number=default_number_type>
				class cost_wavenumber: public wavenumber_series<number>
					{

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! basic user-facing constructor -- 2pf task version
						cost_wavenumber(const twopf_task<number>& tk, SQL_twopf_query& kq, cost_metric m=cost_metric::time,
						                unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

						//! basic user-facing constructor -- 3pf task version
						cost_wavenumber(const threepf_task<number>& tk, SQL_threepf_query& kq, cost_metric m=cost_metric::time,
						                unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

						//! deserialization constructor
						cost_wavenumber(Json::Value& reader, task_finder<number>& finder);

            //! copy constructor
            cost_wavenumber(const cost_wavenumber& obj);

						//! destructor
						virtual ~cost_wavenumber() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::integration_cost; }


				    // DERIVE LINES -- implements a 'derived_line' interface

				  public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags, slave_message_buffer& messages) const override;

				    //! generate a LaTeX label
				    std::string get_LaTeX_label() const;

				    //! generate a non-LaTeX label
				    std::string get_non_LaTeX_label() const;


				    // CLONE

				  public:

				    //! self-replicate
				    virtual cost_wavenumber<number>* clone() const override { return new cost_wavenumber<number>(static_cast<const cost_wavenumber<number>&>(*this)); }


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

						//! for some purposes we need access to the owning task
						integration_task_gadget<number> gadget;

						//! cost metric to use
						cost_metric metric;

						//! twopf or threepf analysis?
						analysis_type type;

						//! query object
						std::unique_ptr< SQL_query > kquery;

					};


				template <typename number>
				cost_wavenumber<number>::cost_wavenumber(const twopf_task<number>& tk, SQL_twopf_query& kq,
				                                         cost_metric m, unsigned int prec)
					: derived_line<number>(make_derivable_task_set_element(tk, false, true, false), axis_class::wavenumber,
                                 { axis_value::k, axis_value::efolds_exit }, prec),
					  wavenumber_series<number>(tk),
					  type(analysis_type::twopf),
		        gadget(tk),
		        metric(m),
						kquery(kq.clone())
					{
					}


		    template <typename number>
		    cost_wavenumber<number>::cost_wavenumber(const threepf_task<number>& tk, SQL_threepf_query& kq,
		                                             cost_metric m, unsigned int prec)
			    : derived_line<number>(make_derivable_task_set_element(tk, false, true, false), axis_class::wavenumber,
                                 { axis_value::k, axis_value::efolds_exit, axis_value::alpha, axis_value::beta,
                                   axis_value::squeeze_k1, axis_value::squeeze_k2, axis_value::squeeze_k3 }, prec),
			      wavenumber_series<number>(tk),
			      type(analysis_type::threepf),
			      gadget(tk),
			      metric(m),
		        kquery(kq.clone())
			    {
			    }


				template <typename number>
				cost_wavenumber<number>::cost_wavenumber(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
					  wavenumber_series<number>(reader),
            gadget(derived_line<number>::parent_tasks) // safe, will always be constructed after derived_line<number>()
					{
						kquery = SQL_query_helper::deserialize(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    std::string type_string = reader[CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE].asString();
						type = analysis_type::twopf;
						if(type_string == CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TWOPF)        type = analysis_type::twopf;
						else if(type_string == CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_THREEPF) type = analysis_type::threepf;
						else assert(false); // TODO: raise exception

				    std::string metric_string = reader[CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC].asString();
						metric = cost_metric::time;
						if(metric_string == CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TIME)       metric = cost_metric::time;
						else if(metric_string == CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_STEPS) metric = cost_metric::steps;
						else assert(false); // TODO: raise exception
					}


        template <typename number>
        cost_wavenumber<number>::cost_wavenumber(const cost_wavenumber<number>& obj)
          : derived_line<number>(obj),
            wavenumber_series<number>(obj),
            gadget(obj.gadget),
            metric(obj.metric),
            type(obj.type),
            kquery(obj.kquery->clone())
          {
          }


				template <typename number>
				void cost_wavenumber<number>::serialize(Json::Value& writer) const
					{
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_INTEGRATION_COST);

				    this->kquery->serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    switch(this->type)
					    {
				        case analysis_type::twopf:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TWOPF);
                    break;
                  }

				        case analysis_type::threepf:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_THREEPF);
                    break;
                  }
					    }

				    switch(this->metric)
					    {
				        case cost_metric::time:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC] = std::string(CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TIME);
                    break;
                  }

				        case cost_metric::steps:
                  {
                    writer[CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC] = std::string(CPPTRANSPORT_NODE_PRODUCT_INTEGRATION_COST_STEPS);
                    break;
                  }
					    }

				    this->wavenumber_series<number>::serialize(writer);
				    this->derived_line<number>::serialize(writer);
					}


		    template <typename number>
		    void cost_wavenumber<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                                   const std::list<std::string>& tags, slave_message_buffer& messages) const
			    {
		        // attach our datapipe to a content group
		        std::string group = this->attach(pipe, tags);

		        // produce output if we are attached to a record with statistics
		        // (we don't try to filter to find a content group which does have statistics,
		        // because we want to match the same content groups as any other content producer)
		        if(pipe.is_integration_attached())
			        {
		            content_group_record<integration_payload>* record = pipe.get_attached_integration_record();

		            if(record && record->get_payload().has_statistics())
			            {
		                // pull wavenumber axis
		                std::vector<double> w_axis;
				            switch(this->type)
					            {
				                case analysis_type::twopf:
					                {
						                auto* kquery_as_twopf = dynamic_cast<SQL_twopf_query*>(this->kquery.get());
						                assert(kquery_as_twopf != nullptr);   // TODO: raise exception
						                w_axis = this->pull_kconfig_axis(pipe, *kquery_as_twopf);
						                break;
					                };

				                case analysis_type::threepf:
					                {
						                auto* kquery_as_threepf = dynamic_cast<SQL_threepf_query*>(this->kquery.get());
						                assert(kquery_as_threepf != nullptr);   // TODO: raise exception
						                w_axis = this->pull_kconfig_axis(pipe, *kquery_as_threepf);
						                break;
					                };
					            }

		                // set up cache handles
		                typename datapipe<number>::k_statistics_handle& ks_handle = pipe.new_k_statistics_handle(*(this->kquery));

		                // pull the statistics from the database
		                k_statistics_tag<number>                     ks_tag     = pipe.new_k_statistics_tag();
		                const std::vector<kconfiguration_statistics> statistics = ks_handle.lookup_tag(ks_tag);

		                std::vector<number> line_data;
		                line_data.reserve(statistics.size());
		                value_type this_value = value_type::time;

		                for(const auto & statistic : statistics)
			                {
		                    switch(this->metric)
			                    {
		                        case cost_metric::time:
                              {
                                line_data.push_back(static_cast<number>(statistic.integration) / 1E9); // convert to seconds
                                this_value = value_type::time;
                                break;
                              }

		                        case cost_metric::steps:
                              {
                                line_data.push_back(static_cast<number>(statistic.steps));
                                this_value = value_type::steps;
                                break;
                              }
			                    }
			                }

		                data_line<number> line(group, this->x_type, this_value, w_axis, line_data,
		                                       this->get_LaTeX_label(), this->get_non_LaTeX_label(), messages, this->is_spectral_index());
				            line.set_data_line_type(data_line_type::scattered_data);
		                lines.push_back(line);
			            }
			        }

		        // detach pipe from content group
		        this->detach(pipe);
			    }


				template <typename number>
				std::string cost_wavenumber<number>::get_LaTeX_label() const
					{
				    std::string label;

				    if(this->label_set)
					    {
				        label = this->LaTeX_label;
					    }
				    else
					    {
				        label = "$" + (this->metric == cost_metric::time ? std::string(CPPTRANSPORT_LATEX_TIME_SYMBOL) : std::string(CPPTRANSPORT_LATEX_STEPS_SYMBOL)) + "$";
					    }

				    return(label);
					}


		    template <typename number>
		    std::string cost_wavenumber<number>::get_non_LaTeX_label() const
			    {
		        std::string label;

		        if(this->label_set)
			        {
		            label = this->non_LaTeX_label;
			        }
		        else
			        {
		            label = (this->metric == cost_metric::time ? std::string(CPPTRANSPORT_NONLATEX_TIME_SYMBOL) : std::string(CPPTRANSPORT_NONLATEX_STEPS_SYMBOL));
			        }

		        return(label);
			    }


		    template <typename number>
		    void cost_wavenumber<number>::write(std::ostream& out)
			    {
		        // TODO: implement me
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_COST_WAVENUMBER_SERIES_H
