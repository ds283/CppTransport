//
// Created by David Seery on 01/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef __u3_line_H_
#define __u3_line_H_


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


#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT          "u3-line-settings"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class u3_line: public time_series<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! basic user-facing constructor
				    u3_line(const threepf_task<number>& tk, index_selector<3>& sel, SQL_time_config_query tq, SQL_threepf_kconfig_query,
                    unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    u3_line(Json::Value& reader, task_finder<number>& finder);

				    //! destructor
				    virtual ~u3_line() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::u3; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_config_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_threepf_kconfig_query& get_k_query() const { return(this->kquery); }


				    // DERIVE LIVES -- implements a 'derived_line' interface

		      public:

				    //! generate data lines for plotting
				    virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags, slave_message_buffer& messages) const override;

		      protected:

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const;


            // MANAGE LABEL OPTIONS

          public:

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
		        virtual u3_line<number>* clone() const override { return new u3_line<number>(static_cast<const u3_line<number>&>(*this)); }


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

            //! record which indices are active
            index_selector<3> active_indices;

            //! use k_t on line labels?
            bool use_kt_label;

            //! use alpha on line labels?
            bool use_alpha_label;

            //! use beta on line labels?
            bool use_beta_label;

			    };


				template <typename number>
				u3_line<number>::u3_line(const threepf_task<number>& tk, index_selector<3>& sel,
                                 SQL_time_config_query tq, SQL_threepf_kconfig_query kq, unsigned int prec)
					: derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
		        time_series<number>(tk),
		        gadget(tk),
            active_indices(sel),
		        tquery(tq),
            kquery(kq),
            use_kt_label(true),
            use_alpha_label(false),
            use_beta_label(false)
					{
					}


				template <typename number>
				u3_line<number>::u3_line(Json::Value& reader, task_finder<number>& finder)
					: derived_line<number>(reader, finder),
		        time_series<number>(reader),
						gadget(),
            active_indices(reader),
						tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
					{
						assert(this->parent_task != nullptr);
						gadget.set_task(this->parent_task, finder);

            use_kt_label    = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT].asBool();
            use_alpha_label = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA].asBool();
            use_beta_label  = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA].asBool();
					}


				template <typename number>
				void u3_line<number>::serialize(Json::Value& writer) const
					{
						writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_U3_LINE);

						this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->active_indices.serialize(writer);

            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT]    = this->use_kt_label;
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA] = this->use_alpha_label;
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_U3_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA]  = this->use_beta_label;

						this->time_series<number>::serialize(writer);
						this->derived_line<number>::serialize(writer);
					}


				template <typename number>
				void u3_line<number>::derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
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
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kquery);
            threepf_kconfig_tag<number>  kc_tag                          = pipe.new_threepf_kconfig_tag();
            std::vector<threepf_kconfig> k_configs                       = kc_handle.lookup_tag(kc_tag);

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

            std::vector<number> u3_tensor(2*Nfields * 2*Nfields * 2*Nfields);

            for(std::vector<threepf_kconfig>::iterator t = k_configs.begin(); t != k_configs.end(); ++t)
              {
                for(unsigned int l = 0; l < 2*Nfields; ++l)
                  {
                    for(unsigned int m = 0; m < 2*Nfields; ++m)
                      {
                        for(unsigned int n = 0; n < 2*Nfields; ++n)
                          {
                            std::array<unsigned int, 3> index_set = { l, m, n };
                            if(this->active_indices.is_on(index_set))
                              {
                                std::vector<number> line_data(t_axis.size());

                                for(unsigned int j = 0; j < line_data.size(); ++j)
                                  {
                                    mdl->u3(this->gadget.get_integration_task(), bg_data[j], t->k1_comoving, t->k2_comoving, t->k3_comoving, t_configs[j].t, u3_tensor);
                                    line_data[j] = u3_tensor[mdl->flatten(l,m,n)];
                                  }

                                lines.emplace_back(group, this->x_type, value_type::dimensionless_value, t_axis, line_data,
                                                   this->get_LaTeX_label(l,m,n,*t), this->get_non_LaTeX_label(l,m,n,*t), messages);
                              }
                          }
                      }
                  }
              }

            this->detach(pipe);
			    }


				template <typename number>
				std::string u3_line<number>::get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const
					{
				    std::string label;

            unsigned int N_fields = this->gadget.get_N_fields();
            const std::vector<std::string>& field_names = this->gadget.get_model()->get_f_latex_names();

						if(this->label_set)
							{
								label = this->LaTeX_label;
							}
						else
							{
                std::ostringstream lbl;

                lbl << CPPTRANSPORT_LATEX_U3_SYMBOL << "_{";

                lbl << field_names[l % N_fields] << (l >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
                    << field_names[m % N_fields] << (m >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
                    << field_names[n % N_fields] << (n >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "");

                lbl << "}";

                label = "$" + lbl.str() + "$";

                if(this->use_tags) label += " $" + this->make_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label) + "$";
							}

						return(label);
					}


		    template <typename number>
		    std::string u3_line<number>::get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& k) const
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
                std::ostringstream lbl;

                lbl << CPPTRANSPORT_NONLATEX_U3_SYMBOL << "[";

                lbl << field_names[l % N_fields] << (m >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "") << " "
                    << field_names[m % N_fields] << (m >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "") << " "
                    << field_names[n % N_fields] << (n >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "");

                lbl << "]";

                label = lbl.str();

                if(this->use_tags) label += " " + this->make_non_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
              }

            return(label);
			    }


				template <typename number>
				void u3_line<number>::write(std::ostream& out)
					{
						// TODO: implement me
					}

			}   // namespace derived_data


	}   // namespace transport


#endif //__u3_line_H_
