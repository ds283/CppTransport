//
// Created by David Seery on 03/06/2014.
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


#ifndef CPPTRANSPORT_FIELD_WAVENUMBER_SERIES_H
#define CPPTRANSPORT_FIELD_WAVENUMBER_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/twopf_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/threepf_line.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
	{

    namespace derived_data
	    {

        //! twopf wavenumber data line
        template <typename number=default_number_type>
        class twopf_wavenumber_series: public wavenumber_series<number>, public twopf_line<number>
	        {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a twopf wavenumber-series object
            twopf_wavenumber_series(const twopf_db_task<number>& tk, index_selector<2> sel,
                                    SQL_time_query tq, SQL_twopf_query kq,
                                    unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

		        //! deserialization constructor
		        twopf_wavenumber_series(Json::Value& reader, task_finder<number>& finder);

		        virtual ~twopf_wavenumber_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::twopf_wavenumber; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


            // DERIVE LINES -- implements a 'derived_line' interface

          public:

            //! generate data lines for plotting
            data_line_set<number> derive_lines
              (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(unsigned int m, unsigned int n, double t) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(unsigned int m, unsigned int n, double t) const;


            // CLONE

          public:

            //! self-replicate
            virtual twopf_wavenumber_series<number>* clone() const override { return new twopf_wavenumber_series<number>(static_cast<const twopf_wavenumber_series<number>&>(*this)); }


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
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        twopf_wavenumber_series<number>::twopf_wavenumber_series(const twopf_db_task<number>& tk, index_selector<2> sel,
                                                                 SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
	        : derived_line<number>(make_derivable_task_set_element(tk, false, false, false),
                                 axis_class::wavenumber, { axis_value::k, axis_value::efolds_exit }, prec),
	          twopf_line<number>(tk, sel),
	          wavenumber_series<number>(tk),
	          tquery(tq),
	          kquery(kq)
	        {
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        twopf_wavenumber_series<number>::twopf_wavenumber_series(Json::Value& reader, task_finder<number>& finder)
	        : derived_line<number>(reader, finder),
	          twopf_line<number>(reader, finder),
	          wavenumber_series<number>(reader),
	          tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
	          kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
	        {
	        }


		    template <typename number>
		    data_line_set<number> twopf_wavenumber_series<number>::derive_lines
		      (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const
			    {
		        unsigned int N_fields = this->gadget.get_N_fields();

		        // attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

		        // pull wavenumber-axis data
		        std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

				    // set up cache handles
		        typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kquery);
				    typename datapipe<number>::time_config_handle&   tc_handle = pipe.new_time_config_handle(this->tquery);
				    typename datapipe<number>::kconfig_data_handle&  k_handle  = pipe.new_kconfig_data_handle(this->kquery);

				    // pull time-configuration information from the database
				    time_config_tag<number> t_tag = pipe.new_time_config_tag();
				    const std::vector< time_config > t_values = tc_handle.lookup_tag(t_tag);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
		        const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

		        data_line_set<number> lines;

				    // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
				    for(const auto& t_value : t_values)
					    {
						    for(unsigned int m = 0; m < 2*N_fields; ++m)
							    {
								    for(unsigned int n = 0; n < 2*N_fields; ++n)
									    {
								        std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
								            cf_kconfig_data_tag<number> tag = pipe.new_cf_kconfig_data_tag(this->is_real_twopf() ? cf_data_type::cf_twopf_re : cf_data_type::cf_twopf_im,
								                                                                           this->gadget.get_model()->flatten(m, n), t_value.serial);

								            std::vector<number> line_data = k_handle.lookup_tag(tag);
                            assert(line_data.size() == k_values.size());

                            value_type value;
                            if(this->dimensionless)
                              {
                                auto l_pos = line_data.begin();
                                auto k_pos = k_values.cbegin();
                                for(; l_pos != line_data.cend() && k_pos != k_values.cend(); ++l_pos, ++k_pos)
                                  {
                                    *l_pos *= 1.0 / (2.0 * M_PI * M_PI);
                                  }
                                value = value_type::dimensionless;
                              }
                            else
                              {
                                auto l_pos = line_data.begin();
                                auto k_pos = k_values.cbegin();
                                for(; l_pos != line_data.cend() && k_pos != k_values.cend(); ++l_pos, ++k_pos)
                                  {
                                    double k_cube = k_pos->k_comoving * k_pos->k_comoving * k_pos->k_comoving;
                                    *l_pos *= 1.0 / k_cube;
                                  }
                                value = value_type::correlation_function;
                              }

                            lines.emplace_back(group, this->x_type, value, w_axis, line_data,
                                               this->get_LaTeX_label(m,n,t_value.t), this->get_non_LaTeX_label(m,n,t_value.t), messages, this->is_spectral_index());
                          }
									    }
							    }
					    }

		        // detach pipe from content group
		        this->detach(pipe);
				    return lines;
			    }


        template <typename number>
        std::string twopf_wavenumber_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, double t) const
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
        std::string twopf_wavenumber_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, double t) const
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
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void twopf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->twopf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void twopf_wavenumber_series<number>::serialize(Json::Value& writer) const
	        {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TWOPF_WAVENUMBER_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->twopf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }


		    //! threepf wavenumber data line
		    template <typename number=default_number_type>
		    class threepf_wavenumber_series: public wavenumber_series<number>, public threepf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a threepf wavenumber-data object
				    threepf_wavenumber_series(const threepf_task<number>& tk, index_selector<3> sel,
				                              SQL_time_query tq, SQL_threepf_query kq,
				                              unsigned int prec=CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    threepf_wavenumber_series(Json::Value& reader, task_finder<number>& finder);

				    virtual ~threepf_wavenumber_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            virtual derived_line_type get_line_type() const override { return derived_line_type::threepf_wavenumber; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_threepf_query& get_k_query() const { return(this->kquery); }


		        // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
		        data_line_set<number> derive_lines
		          (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const;


		        // CLONE

		      public:

		        //! self-replicate
		        virtual threepf_wavenumber_series<number>* clone() const override { return new threepf_wavenumber_series<number>(static_cast<const threepf_wavenumber_series<number>&>(*this)); }


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! serialize this object
		        virtual void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

          protected:

		        //! SQL query representing x-axis
		        SQL_threepf_query kquery;

		        //! SQL query representing different lines
		        SQL_time_query tquery;

			    };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        threepf_wavenumber_series<number>::threepf_wavenumber_series(const threepf_task<number>& tk, index_selector<3> sel,
                                                                     SQL_time_query tq, SQL_threepf_query kq,
                                                                     unsigned int prec)
	        : derived_line<number>(make_derivable_task_set_element(tk, false, false, false), axis_class::wavenumber,
	                               { axis_value::k, axis_value::efolds_exit, axis_value::alpha, axis_value::beta,
                                   axis_value::squeeze_k1, axis_value::squeeze_k2, axis_value::squeeze_k3 }, prec),
	          threepf_line<number>(tk, sel),
	          wavenumber_series<number>(tk),
	          tquery(tq),
	          kquery(kq)
	        {
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        threepf_wavenumber_series<number>::threepf_wavenumber_series(Json::Value& reader, task_finder<number>& finder)
	        : derived_line<number>(reader, finder),
	          threepf_line<number>(reader, finder),
	          wavenumber_series<number>(reader),
	          tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
	          kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
	        {
	        }


        template <typename number>
        data_line_set<number> threepf_wavenumber_series<number>::derive_lines
          (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const
	        {
		        unsigned int N_fields = this->gadget.get_N_fields();

            // attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

            // pull wavenumber-axis data
            std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

            // set up cache handles
            typename datapipe<number>::time_config_handle    & tc_handle = pipe.new_time_config_handle(this->tquery);
            typename datapipe<number>::time_data_handle      & t_handle  = pipe.new_time_data_handle(this->tquery);
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kquery);
            typename datapipe<number>::kconfig_data_handle   & k_handle  = pipe.new_kconfig_data_handle(this->kquery);

            // pull time-configuration information from the database
            time_config_tag<number>        t_tag    = pipe.new_time_config_tag();
            const std::vector<time_config> t_values = tc_handle.lookup_tag(t_tag);

            // extract k-configuration data
            threepf_kconfig_tag<number>  k_tag    = pipe.new_threepf_kconfig_tag();
            std::vector<threepf_kconfig> k_values = kc_handle.lookup_tag(k_tag);

            data_line_set<number> lines;

            // loop through all components of the twopf, for each t-configuration we use, pulling data from the database
            for(const auto& t_value : t_values)
	            {
                for(unsigned int l = 0; l < 2*N_fields; ++l)
	                {
                    for(unsigned int m = 0; m < 2*N_fields; ++m)
	                    {
		                    for(unsigned int n = 0; n < 2*N_fields; ++n)
			                    {
		                        std::array<unsigned int, 3> index_set = { l, m, n };
		                        if(this->active_indices.is_on(index_set))
			                        {
		                            cf_kconfig_data_tag<number> tag = pipe.new_cf_kconfig_data_tag(this->get_dot_meaning() == dot_type::derivatives ? cf_data_type::cf_threepf_Nderiv : cf_data_type::cf_threepf_momentum,
                                                                                               this->gadget.get_model()->flatten(l,m,n), t_value.serial);

		                            std::vector<number> line_data = k_handle.lookup_tag(tag);
                                assert(line_data.size() == w_axis.size());

                                value_type value;
                                if(this->dimensionless)
                                  {
                                    value = value_type::dimensionless;
                                  }
                                else
                                  {
                                    auto l_pos = line_data.begin();
                                    auto k_pos = k_values.cbegin();
                                    for(; l_pos != line_data.cend() && k_pos != k_values.cend(); ++l_pos, ++k_pos)
                                      {
                                        double shape = k_pos->k1_comoving*k_pos->k1_comoving * k_pos->k2_comoving*k_pos->k2_comoving * k_pos->k3_comoving*k_pos->k3_comoving;
                                        *l_pos *= 1.0/shape;
                                      }
                                    value = value_type::correlation_function;
                                  }

                                lines.emplace_back(group, this->x_type, value, w_axis, line_data,
                                                   this->get_LaTeX_label(l,m,n,t_value.t), this->get_non_LaTeX_label(l,m,n,t_value.t), messages, this->is_spectral_index());
			                        }
			                    }
	                    }
	                }
	            }

            // detach pipe from content group
            this->detach(pipe);
            return lines;
	        }


        template <typename number>
        std::string threepf_wavenumber_series<number>::get_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const
	        {
            std::string tag = this->make_LaTeX_tag(t);
            std::string label;

            if(this->label_set)
	            {
                label = this->LaTeX_label;
	            }
            else
	            {
                label = "$" + this->make_LaTeX_label(l,m,n) + "$";
	            }

            if(this->use_tags) label += " $" + tag + "$";
            return(label);
	        }


        template <typename number>
        std::string threepf_wavenumber_series<number>::get_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n, double t) const
	        {
            std::string tag = this->make_non_LaTeX_tag(t);
            std::string label;

            if(this->label_set)
	            {
                label = this->non_LaTeX_label;
	            }
            else
	            {
                label = this->make_non_LaTeX_label(l,m,n);
	            }

            if(this->use_tags) label += " " + tag;
            return(label);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void threepf_wavenumber_series<number>::write(std::ostream& out)
	        {
            this->threepf_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);
	        }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void threepf_wavenumber_series<number>::serialize(Json::Value& writer) const
	        {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_WAVENUMBER_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->threepf_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
	        }


	    }   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_FIELD_WAVENUMBER_SERIES_H
