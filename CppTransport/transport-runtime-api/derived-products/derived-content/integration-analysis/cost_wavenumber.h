//
// Created by David Seery on 25/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __cost_wavenumber_series_H_
#define __cost_wavenumber_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/derived-content/integration-analysis/cost_metrics.h"

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"
#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query_helper.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE    "type"
#define __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TWOPF   "twopf"
#define __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_THREEPF "threepf"

#define __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC  "metric"
#define __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TIME    "time"
#define __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_STEPS   "steps"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
				class cost_wavenumber: public wavenumber_series<number>
					{

						typedef enum { twopf_analysis, threepf_analysis } analysis_type;

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! basic user-facing constructor -- 2pf task version
						cost_wavenumber(const twopf_task<number>& tk, SQL_twopf_kconfig_query kq, cost_metric m=time_cost,
						                unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! basic user-facing constructor -- 3pf task version
						cost_wavenumber(const threepf_task<number>& tk, SQL_threepf_kconfig_query kq, cost_metric m=time_cost,
						                unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! override copy constructor to provide deep copy of SQL_query object
						cost_wavenumber(const cost_wavenumber<number>& obj);

						//! deserialization constructor
						cost_wavenumber(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

						//! destructor
						virtual ~cost_wavenumber() = default;


				    // DERIVE LINES -- implements a 'time_series' interface

				  public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
				                              const std::list<std::string>& tags) const override;

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

						//! integration task gadget
						integration_task_gadget<number> gadget;

						//! cost metric to use
						cost_metric metric;

						//! twopf or threepf analysis?
						analysis_type type;

						//! query object
						SQL_query* kquery;

					};


				template <typename number>
				cost_wavenumber<number>::cost_wavenumber(const twopf_task<number>& tk, SQL_twopf_kconfig_query kq,
				                                         cost_metric m, unsigned int prec)
					: derived_line<number>(tk, wavenumber_axis, std::list<axis_value>{ k_axis, efolds_exit_axis }, prec),
					  wavenumber_series<number>(tk),
					  type(twopf_analysis),
		        gadget(tk),
		        metric(m),
						kquery(kq.clone())
					{
					}


		    template <typename number>
		    cost_wavenumber<number>::cost_wavenumber(const threepf_task<number>& tk, SQL_threepf_kconfig_query kq,
		                                             cost_metric m, unsigned int prec)
			    : derived_line<number>(tk, wavenumber_axis, std::list<axis_value>{ k_axis, efolds_exit_axis, alpha_axis, beta_axis, squeezing_fraction_k1_axis, squeezing_fraction_k2_axis, squeezing_fraction_k3_axis }, prec),
			      wavenumber_series<number>(tk),
			      type(threepf_analysis),
			      gadget(tk),
			      metric(m),
		        kquery(kq.clone())
			    {
			    }


				template <typename number>
				cost_wavenumber<number>::cost_wavenumber(const cost_wavenumber<number>& obj)
					: derived_line<number>(obj),
					  wavenumber_series<number>(obj),
					  type(obj.type),
					  gadget(obj.gadget),
					  metric(obj.metric),
					  kquery(obj.kquery->clone())
					{
					}


				template <typename number>
				cost_wavenumber<number>::cost_wavenumber(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
					  wavenumber_series<number>(reader),
					  gadget()
					{
						assert(this->parent_task != nullptr);
						gadget.set_task(this->parent_task, finder);

						kquery = SQL_query_helper::deserialize(reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    std::string type_string = reader[__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE].asString();
						type = twopf_analysis;
						if(type_string == __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TWOPF)        type = twopf_analysis;
						else if(type_string == __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_THREEPF) type = threepf_analysis;
						else assert(false);

				    std::string metric_string = reader[__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC].asString();
						metric = time_cost;
						if(metric_string == __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TIME)       metric = time_cost;
						else if(metric_string == __CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_STEPS) metric = steps_cost;
						else assert(false);
					}


				template <typename number>
				void cost_wavenumber<number>::serialize(Json::Value& writer) const
					{
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_INTEGRATION_COST);

				    this->kquery->serialize(writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    switch(this->type)
					    {
				        case twopf_analysis:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TWOPF);
					        break;

				        case threepf_analysis:
				        writer[__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_THREEPF);
					        break;

				        default:
					        assert(false);
					    }

				    switch(this->metric)
					    {
				        case time_cost:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_TIME);
					        break;

				        case steps_cost:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_METRIC] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_INTEGRATION_COST_STEPS);
					        break;

				        default:
					        assert(false);
					    }

				    this->wavenumber_series<number>::serialize(writer);
				    this->derived_line<number>::serialize(writer);
					}


		    template <typename number>
		    void cost_wavenumber<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number>>& lines, const std::list<std::string>& tags) const
			    {
		        // attach our datapipe to an output group
		        std::string group = this->attach(pipe, tags);

		        // produce output if we are attached to a record with statistics
		        // (we don't try to filter to find an output group which does have statistics,
		        // because we want to match the same output groups as any other content producer)
		        if(pipe.is_integration_attached())
			        {
		            std::shared_ptr <output_group_record<integration_payload>> record = pipe.get_attached_integration_record();

		            if(record && record->get_payload().has_statistics())
			            {
		                // pull wavenumber axis
		                std::vector<double> w_axis;
				            switch(this->type)
					            {
				                case twopf_analysis:
					                {
						                SQL_twopf_kconfig_query* kquery_as_twopf = dynamic_cast<SQL_twopf_kconfig_query*>(this->kquery);
						                w_axis = this->pull_kconfig_axis(pipe, *kquery_as_twopf);
						                break;
					                };

				                case threepf_analysis:
					                {
						                SQL_threepf_kconfig_query* kquery_as_threepf = dynamic_cast<SQL_threepf_kconfig_query*>(this->kquery);
						                w_axis = this->pull_kconfig_axis(pipe, *kquery_as_threepf);
						                break;
					                };

				                default:
					                assert(false);
					            }

		                // set up cache handles
		                typename datapipe<number>::k_statistics_handle& ks_handle = pipe.new_k_statistics_handle(*(this->kquery));

		                // pull the statistics from the database
		                k_statistics_tag<number>                     ks_tag     = pipe.new_k_statistics_tag();
		                const std::vector<kconfiguration_statistics> statistics = ks_handle.lookup_tag(ks_tag);

		                std::vector<number> line_data;
		                line_data.reserve(statistics.size());
		                value_type this_value = time_value;

		                for(std::vector<kconfiguration_statistics>::const_iterator t = statistics.begin(); t != statistics.end(); ++t)
			                {
		                    switch(this->metric)
			                    {
		                        case time_cost:
			                        line_data.push_back(static_cast<number>(t->integration) / (1000.0*1000.0*1000.0)); // convert to seconds
			                        this_value = time_value;
			                        break;

		                        case steps_cost:
			                        line_data.push_back(static_cast<number>(t->steps));
			                        this_value = steps_value;
			                        break;

		                        default:
			                        assert(false);
			                    }
			                }

		                data_line<number> line = data_line<number>(group, this->x_type, this_value, w_axis, line_data,
		                                                           this->get_LaTeX_label(), this->get_non_LaTeX_label(), this->is_spectral_index());
				            line.set_data_line_type(scattered_data);

		                lines.push_back(line);
			            }
			        }

		        // detach pipe from output group
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
				        label = "$" + (this->metric == time_cost ? std::string(__CPP_TRANSPORT_LATEX_TIME_SYMBOL) : std::string(__CPP_TRANSPORT_LATEX_STEPS_SYMBOL)) + "$";
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
		            label = (this->metric == time_cost ? std::string(__CPP_TRANSPORT_NONLATEX_TIME_SYMBOL) : std::string(__CPP_TRANSPORT_NONLATEX_STEPS_SYMBOL));
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


#endif //__cost_wavenumber_series_H_
