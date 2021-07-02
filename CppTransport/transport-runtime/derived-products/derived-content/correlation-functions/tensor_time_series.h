//
// Created by David Seery on 16/12/14.
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


#ifndef CPPTRANSPORT_TENSOR_TIME_SERIES_H
#define CPPTRANSPORT_TENSOR_TIME_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/tensor_twopf_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/tensor_nt_line.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
	{

    namespace derived_data
	    {

		    //! tensor two-point function time data line
		    template <typename number=default_number_type>
		    class tensor_twopf_time_series: public time_series<number>, public tensor_twopf_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! construct a tensor two-point function time series data object
				    tensor_twopf_time_series(const twopf_db_task<number>& tk, index_selector<2> sel,
				                             SQL_time_query tq, SQL_twopf_query kq,
				                             unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

				    //! deserialization constructor
				    tensor_twopf_time_series(Json::Value& reader, task_finder<number>& finder);

				    virtual ~tensor_twopf_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            derived_line_type get_line_type() const override { return derived_line_type::tensor_twopf_time; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


				    // DERIVE LINES -- implements a 'time_series' interface

		      public:

		        //! generate data lines for plotting
		        void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                              const tag_list& tags, slave_message_buffer& messages) const override;

		        //! generate a LaTeX label
		        std::string get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;

		        //! generate a non-LaTeX label
		        std::string get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;


				    // CLONE

		      public:

				    //! self-replicate
				    tensor_twopf_time_series<number>* clone() const override { return new tensor_twopf_time_series<number>(static_cast<const tensor_twopf_time_series<number>&>(*this)); }


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a serializable interface

		      public:

				    //! serialize this object
				    void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing x axis
            SQL_time_query tquery;

            //! SQL query representing different lines
            SQL_twopf_query kquery;

			    };


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves
		    template <typename number>
		    tensor_twopf_time_series<number>::tensor_twopf_time_series(const twopf_db_task<number>& tk, index_selector<2> sel,
		                                                               SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
			    : derived_line<number>(make_derivable_task_set_element(tk, false, false, false),
                                 axis_class::time, { axis_value::efolds }, prec),
            tensor_twopf_line<number>(tk, sel),
            time_series<number>(tk),
            tquery(tq),
            kquery(kq)
			    {
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the constructor for
		    // derived_line<> is not called automatically during construction of time_series<>.
		    // We have to call it ourselves
		    template <typename number>
		    tensor_twopf_time_series<number>::tensor_twopf_time_series(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader, finder),
			      tensor_twopf_line<number>(reader, finder),
			      time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
			    {
			    }


        template <typename number>
        void tensor_twopf_time_series<number>::derive_lines
          (datapipe<number>& pipe, std::list<data_line<number> >& lines, const tag_list& tags, slave_message_buffer& messages) const
	        {
            // attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

		        // pull time-axis data
		        const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
            auto& k_handle = pipe.new_twopf_kconfig_handle(this->kquery);
            auto& t_handle = pipe.new_time_data_handle(this->tquery);

		        // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();

		        const typename std::vector< twopf_kconfig > k_values = k_handle.lookup_tag(k_tag);

		        // for each k-configuration, loop through all components of the tensor twopf and pull data from the database
		        for(const auto& k_value : k_values)
			        {
				        for(unsigned int m = 0; m < 2; ++m)
					        {
						        for(unsigned int n = 0; n < 2; ++n)
							        {
						            std::array<unsigned int, 2> index_set = { m, n };
								        if(this->active_indices.is_on(index_set))
									        {
								            cf_time_data_tag<number> tag =
								              pipe.new_cf_time_data_tag(cf_data_type::cf_tensor_twopf,
                                                        this->gadget.get_model()->tensor_flatten(m, n), k_value.serial);

								            std::vector<number> line_data = t_handle.lookup_tag(tag);

                            value_type value;
                            if(this->dimensionless)
                              {
                                for(unsigned int j = 0; j < line_data.size(); ++j)
                                  {
                                    line_data[j] *= 1.0 / (2.0*M_PI*M_PI);
                                  }
                                value = value_type::dimensionless;
                              }
                            else
                              {
                                double k_cube = k_value.k_comoving * k_value.k_comoving * k_value.k_comoving;
                                for(unsigned int j = 0; j < line_data.size(); ++j)
                                  {
                                    line_data[j] *= 1.0 / k_cube;
                                  }
                                value = value_type::correlation_function;
                              }

                            lines.emplace_back(group, this->x_type, value, t_axis, line_data,
                                               this->get_LaTeX_label(m,n,k_value), this->get_non_LaTeX_label(m,n,k_value), messages);
									        }
							        }
					        }
			        }

            // detach pipe from content group
            this->detach(pipe);
	        }


        template <typename number>
        std::string tensor_twopf_time_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
	        {
            std::string tag = this->make_LaTeX_tag(k);
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
        std::string tensor_twopf_time_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
	        {
            std::string tag = this->make_non_LaTeX_tag(k);
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
		    // derived_line<> is not called from time_series<>. We have to call it for ourselves.
		    template <typename number>
		    void tensor_twopf_time_series<number>::write(std::ostream& out)
			    {
		        this->tensor_twopf_line<number>::write(out);
		        this->time_series<number>::write(out);
				    this->derived_line<number>::write(out);
			    }


		    // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
		    // derived_line<> is not called from time_series<>. We have to call it for ourselves.
		    template <typename number>
		    void tensor_twopf_time_series<number>::serialize(Json::Value& writer) const
			    {
				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

				    this->derived_line<number>::serialize(writer);
				    this->tensor_twopf_line<number>::serialize(writer);
				    this->time_series<number>::serialize(writer);
			    }


        //! tensor two-point spectral index time data line
        template <typename number=default_number_type>
        class tensor_nt_time_series: public time_series<number>, public tensor_nt_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a tensor two-point spectral index time series data object
            tensor_nt_time_series(const twopf_db_task<number>& tk, index_selector<2> sel,
                                  SQL_time_query tq, SQL_twopf_query kq,
                                  unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            tensor_nt_time_series(Json::Value& reader, task_finder<number>& finder);

            virtual ~tensor_nt_time_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            derived_line_type get_line_type() const override { return derived_line_type::tensor_nt_time; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


            // DERIVE LINES -- implements a 'time_series' interface

          public:

            //! generate data lines for plotting
            void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const tag_list& tags, slave_message_buffer& messages) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const;


            // CLONE

          public:

            //! self-replicate
            tensor_nt_time_series<number>* clone() const override { return new tensor_nt_time_series<number>(static_cast<const tensor_nt_time_series<number>&>(*this)); }


            // WRITE TO A STREAM

          public:

            //! write self-details to a stream
            void write(std::ostream& out) override;


            // SERIALIZATION -- implements a serializable interface

          public:

            //! serialize this object
            void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing x axis
            SQL_time_query tquery;

            //! SQL query representing different lines
            SQL_twopf_query kquery;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
        tensor_nt_time_series<number>::tensor_nt_time_series(const twopf_db_task<number>& tk, index_selector<2> sel,
                                                             SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
          : derived_line<number>(make_derivable_task_set_element(tk, false, false, true),
                                 axis_class::time, { axis_value::efolds }, prec),
            tensor_nt_line<number>(tk, sel),
            time_series<number>(tk),
            tquery(tq),
            kquery(kq)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of time_series<>.
        // We have to call it ourselves
        template <typename number>
        tensor_nt_time_series<number>::tensor_nt_time_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            tensor_nt_line<number>(reader, finder),
            time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
          {
          }


        template <typename number>
        void tensor_nt_time_series<number>::derive_lines
          (datapipe<number>& pipe, std::list<data_line<number> >& lines, const tag_list& tags, slave_message_buffer& messages) const
          {
            // attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

            // pull time-axis data
            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

            // set up cache handles
            auto& k_handle = pipe.new_twopf_kconfig_handle(this->kquery);
            auto& t_handle = pipe.new_time_data_handle(this->tquery);

            // pull k-configuration information from the database
            twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();

            const typename std::vector< twopf_kconfig > k_values = k_handle.lookup_tag(k_tag);

            // for each k-configuration, loop through all components of the tensor twopf and pull data from the database
            for(const auto& k_value : k_values)
              {
                for(unsigned int m = 0; m < 2; ++m)
                  {
                    for(unsigned int n = 0; n < 2; ++n)
                      {
                        std::array<unsigned int, 2> index_set = { m, n };
                        if(this->active_indices.is_on(index_set))
                          {
                            cf_time_data_tag<number> tag =
                              pipe.new_cf_time_data_tag(cf_data_type::cf_tensor_nt,
                                                        this->gadget.get_model()->tensor_flatten(m, n), k_value.serial);

                            std::vector<number> line_data = t_handle.lookup_tag(tag);

                            lines.emplace_back(group, this->x_type, value_type::spectral_index, t_axis, line_data,
                                               this->get_LaTeX_label(m,n,k_value), this->get_non_LaTeX_label(m,n,k_value), messages, true);
                          }
                      }
                  }
              }

            // detach pipe from content group
            this->detach(pipe);
          }


        template <typename number>
        std::string tensor_nt_time_series<number>::get_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
          {
            std::string tag = this->make_LaTeX_tag(k);
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
        std::string tensor_nt_time_series<number>::get_non_LaTeX_label(unsigned int m, unsigned int n, const twopf_kconfig& k) const
          {
            std::string tag = this->make_non_LaTeX_tag(k);
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
        // derived_line<> is not called from time_series<>. We have to call it for ourselves.
        template <typename number>
        void tensor_nt_time_series<number>::write(std::ostream& out)
          {
            this->tensor_nt_line<number>::write(out);
            this->time_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is not called from time_series<>. We have to call it for ourselves.
        template <typename number>
        void tensor_nt_time_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TENSOR_TWOPF_NT_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->tensor_nt_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }

	    }

	}


#endif //CPPTRANSPORT_TENSOR_TIME_SERIES_H
