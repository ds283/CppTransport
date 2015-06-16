//
// Created by David Seery on 01/06/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __largest_u3_line_H_
#define __largest_u3_line_H_


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


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class largest_u3_line: public time_series<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! basic user-facing constructor
				    largest_u3_line(const threepf_task<number>& tk, SQL_time_config_query tq, SQL_threepf_kconfig_query,
                            unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    largest_u3_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    //! destructor
				    virtual ~largest_u3_line() = default;


				    // DERIVE LIVES -- implements a 'derived_line' interface

		      public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines, const std::list<std::string>& tags) const override;

		      protected:

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(const threepf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(const threepf_kconfig& k) const;


            // MANAGE LABEL OPTIONS

            //! get k_t label setting
            bool get_use_kt_label() const { return(this->use_kt_label); }
            //! set k_t label setting
            void set_use_kt_label(bool g) { this->use_kt_label = g; }
            //! get alpha label setting
            bool get_use_alpha_label() const { return(this->use_alpha_label); }
            //! set alpha label setting
            void set_use_alpha_label(bool g) { this->use_alpha_label = g; }
            //! get beta label setting
            bool get_use_beta_label() const { return(this->use_beta_label); }
            //! set beta label setting
            void set_use_beta_label(bool g) { this->use_beta_label = g; }


		        // CLONE

		      public:

		        //! self-replicate
		        virtual largest_u3_line<number>* clone() const override { return new largest_u3_line<number>(static_cast<const largest_u3_line<number>&>(*this)); }


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
				    SQL_time_config_query tquery;

            //! kconfig query object
            SQL_threepf_kconfig_query kquery;

            //! use k_t on line labels?
            bool use_kt_label;

            //! use alpha on line labels?
            bool use_alpha_label;

            //! use beta on line labels?
            bool use_beta_label;

			    };


				template <typename number>
				largest_u3_line<number>::largest_u3_line(const threepf_task<number>& tk, SQL_time_config_query tq, SQL_threepf_kconfig_query kq, unsigned int prec)
					: derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }, prec),
		        time_series<number>(tk),
		        gadget(tk),
		        tquery(tq),
            kquery(kq),
            use_kt_label(true),
            use_alpha_label(false),
            use_beta_label(false)
					{
					}


				template <typename number>
				largest_u3_line<number>::largest_u3_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader, finder),
		        time_series<number>(reader),
						gadget(),
						tquery(reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
					{
						assert(this->parent_task != nullptr);
						gadget.set_task(this->parent_task, finder);

            use_kt_label    = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT].asBool();
            use_alpha_label = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA].asBool();
            use_beta_label  = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA].asBool();
					}


				template <typename number>
				void largest_u3_line<number>::serialize(Json::Value& writer) const
					{
						writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_LARGEST_U3_LINE);

						this->tquery.serialize(writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT]    = this->use_kt_label;
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA] = this->use_alpha_label;
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA]  = this->use_beta_label;

						this->time_series<number>::serialize(writer);
						this->derived_line<number>::serialize(writer);
					}


				template <typename number>
				void largest_u3_line<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines, const std::list<std::string>& tags) const
					{
				    // attach our datapipe to an output group
				    std::string group = this->attach(pipe, tags);

				    const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

            // get time config data
            typename datapipe<number>::time_config_handle& tc_handle = pipe.new_time_config_handle(this->tquery);
            time_config_tag<number>  tc_tag                          = pipe.new_time_config_tag();
            std::vector<time_config> t_configs                       = tc_handle.lookup_tag(tc_tag);

            // get kconfig data
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kquery);
            threepf_kconfig_tag<number>  kc_tag                          = pipe.new_threepf_kconfig_tag();
            std::vector<threepf_kconfig> k_configs                       = kc_handle.lookup_tag(kc_tag);

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

		        model<number>* mdl = this->gadget.get_model();
		        assert(mdl != nullptr);

            std::vector< std::vector< std::vector<number> > > u3_tensor;

            for(std::vector<threepf_kconfig>::iterator t = k_configs.begin(); t != k_configs.end(); ++t)
              {
                std::vector<number> line_data(t_axis.size());

                for(unsigned int j = 0; j < line_data.size(); ++j)
                  {
                    mdl->u3(this->gadget.get_integration_task(), bg_data[j], t->k1_comoving, t->k2_comoving, t->k3_comoving, t_configs[j].t, u3_tensor);
                    number val = -std::numeric_limits<number>::max();

                    for(unsigned int l = 0; l < 2 * this->gadget.get_N_fields(); ++l)
                      {
                        for(unsigned int m = 0; m < 2 * this->gadget.get_N_fields(); ++m)
                          {
                            for(unsigned int n = 0; n < 2 * this->gadget.get_N_fields(); ++n)
                              {
                                number value = std::abs(u3_tensor[l][m][n]);
                                if(value > val) val = value;
                              }
                          }
                      }

                    line_data[j] = val;
                  }

                data_line<number> line(group, this->x_type, dimensionless_value, t_axis, line_data,
                                       this->get_LaTeX_label(*t), this->get_non_LaTeX_label(*t));
                lines.push_back(line);
              }

            this->detach(pipe);
			    }


				template <typename number>
				std::string largest_u3_line<number>::get_LaTeX_label(const threepf_kconfig& k) const
					{
				    std::string label;

						if(this->label_set)
							{
								label = this->LaTeX_label;
							}
						else
							{
                label = "$" + std::string(__CPP_TRANSPORT_LATEX_LARGEST_U3_SYMBOL) + "$";

                if(this->use_tags) label += " $" + this->make_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label) + "$";
							}

						return(label);
					}


		    template <typename number>
		    std::string largest_u3_line<number>::get_non_LaTeX_label(const threepf_kconfig& k) const
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
                label = __CPP_TRANSPORT_NONLATEX_LARGEST_U3_SYMBOL;

                if(this->use_tags) label += " " + this->make_non_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
              }

            return(label);
			    }


				template <typename number>
				void largest_u3_line<number>::write(std::ostream& out)
					{
						// TODO: implement me
					}

			}   // namespace derived_data


	}   // namespace transport


#endif //__largest_u3_line_H_
