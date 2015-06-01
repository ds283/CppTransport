//
// Created by David Seery on 01/06/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __epsilon_H_
#define __epsilon_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"
#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query_helper.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE       "type"
#define __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_EPSILON    "epsilon"
#define __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_HUBBLE     "hubble"
#define __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_A_HUBBLE   "a-hubble"


namespace transport
	{

		namespace derived_data
			{

		    typedef enum { epsilon, Hubble, aHubble } background_line_type;

				template <typename number>
		    class background_line: public time_series<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! basic user-facing constructor
				    background_line(const twopf_list_task<number>& tk, SQL_time_config_query tq, background_line_type t, unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    background_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    //! destructor
				    virtual ~background_line() = default;


				    // DERIVE LIVES -- implements a 'derived_line' interface

		      public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines, const std::list<std::string>& tags) const override;

		      protected:

				    //! epsilon line
				    void epsilon_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines, const std::list<std::string>& tags,
				                      const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const;

		        //! Hubble line
		        void Hubble_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines, const std::list<std::string>& tags,
		                         const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const;

		        //! aHubble line
		        void aHubble_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines, const std::list<std::string>& tags,
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
				    SQL_time_config_query tquery;

				    //! line type
				    background_line_type type;

			    };


				template <typename number>
				background_line<number>::background_line(const twopf_list_task<number>& tk, SQL_time_config_query tq, background_line_type t, unsigned int prec)
					: derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }, prec),
		        time_series<number>(tk),
		        gadget(tk),
		        tquery(tq),
						type(t)
					{
					}


				template <typename number>
				background_line<number>::background_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
		        time_series<number>(reader),
						gadget(),
						tquery(reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY])
					{
						assert(this->parent_task != nullptr);
						gadget.set_task(this->parent_task, finder);

				    std::string type_string = reader[__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE].asString();
						type = epsilon;
						if(type_string == __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_EPSILON)         type = epsilon;
						else if(type_string == __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_HUBBLE)     type = Hubble;
						else if(type_string == __CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_A_HUBBLE)   type = aHubble;
						else assert(false); // TODO: raise exception
					}


				template <typename number>
				void background_line<number>::serialize(Json::Value& writer) const
					{
						writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_BACKGROUND_LINE);

						this->tquery.serialize(writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);

				    switch(this->type)
					    {
				        case epsilon:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_EPSILON);
						      break;

				        case Hubble:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_HUBBLE);
						      break;

				        case aHubble:
					        writer[__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_BACKGROUND_LINE_A_HUBBLE);
						      break;

				        default:
					        assert(false);
						      // TODO: raise exception
					    }

						this->time_series<number>::serialize(writer);
						this->derived_line<number>::serialize(writer);
					}


				template <typename number>
				void background_line<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines, const std::list<std::string>& tags) const
					{
				    // attach our datapipe to an output group
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
				        case epsilon:
					        this->epsilon_line(group, pipe, lines, tags, t_axis, bg_data);
					        break;

				        case Hubble:
					        this->Hubble_line(group, pipe, lines, tags, t_axis, bg_data);
					        break;

				        case aHubble:
					        this->aHubble_line(group, pipe, lines, tags, t_axis, bg_data);
					        break;
					    }

				    this->detach(pipe);
					}


		    template <typename number>
		    void background_line<number>::epsilon_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines, const std::list<std::string>& tags,
		                                               const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const
			    {
		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

		        std::vector<number> line_data(t_axis.size());

				    for(unsigned int j = 0; j < line_data.size(); ++j)
					    {
				        line_data[j] = mdl->epsilon(this->gadget.get_integration_task()->get_params(), bg_data[j]);
					    }

				    data_line<number> line(group, this->x_type, dimensionless_value, t_axis, line_data,
				                           this->get_LaTeX_label(), this->get_non_LaTeX_label());
				    lines.push_back(line);
			    }


		    template <typename number>
		    void background_line<number>::Hubble_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines, const std::list<std::string>& tags,
		                                              const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const
			    {
		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

		        std::vector<number> line_data(t_axis.size());

		        for(unsigned int j = 0; j < line_data.size(); ++j)
			        {
		            line_data[j] = mdl->H(this->gadget.get_integration_task()->get_params(), bg_data[j]) / this->gadget.get_integration_task()->get_params().get_Mp();
			        }

		        data_line<number> line(group, this->x_type, dimensionless_value, t_axis, line_data,
		                               this->get_LaTeX_label(), this->get_non_LaTeX_label());
		        lines.push_back(line);
			    }


		    template <typename number>
		    void background_line<number>::aHubble_line(const std::string& group, datapipe<number>& pipe, std::list<data_line<number> >& lines, const std::list<std::string>& tags,
		                                               const std::vector<double>& t_axis, std::vector<std::vector<number> >& bg_data) const
			    {
		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

		        std::vector<number> line_data(t_axis.size());

		        for(unsigned int j = 0; j < line_data.size(); ++j)
			        {
		            number a = exp(t_axis[j] - this->gadget.get_integration_task()->get_N_horizon_crossing());
		            line_data[j] = a * mdl->H(this->gadget.get_integration_task()->get_params(), bg_data[j]) / this->gadget.get_integration_task()->get_params().get_Mp();
			        }

		        data_line<number> line(group, this->x_type, dimensionless_value, t_axis, line_data,
		                               this->get_LaTeX_label(), this->get_non_LaTeX_label());
		        lines.push_back(line);
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
								    case epsilon:
									    label = "$" + std::string(__CPP_TRANSPORT_LATEX_EPSILON_SYMBOL) + "$";
											break;

								    case Hubble:
									    label = "$" + std::string(__CPP_TRANSPORT_LATEX_HUBBLE_SYMBOL) + "$";
											break;

								    case aHubble:
									    label = "$" + std::string(__CPP_TRANSPORT_LATEX_A_HUBBLE_SYMBOL) + "$";
											break;

								    default:
									    assert(false);
											// TODO: raise exception
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
		                case epsilon:
			                label = "$" + std::string(__CPP_TRANSPORT_NONLATEX_EPSILON_SYMBOL) + "$";
		                break;

		                case Hubble:
			                label = "$" + std::string(__CPP_TRANSPORT_NONLATEX_HUBBLE_SYMBOL) + "$";
		                break;

		                case aHubble:
			                label = "$" + std::string(__CPP_TRANSPORT_NONLATEX_A_HUBBLE_SYMBOL) + "$";
		                break;

		                default:
			                assert(false);
		                // TODO: raise exception
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


#endif //__epsilon_H_
