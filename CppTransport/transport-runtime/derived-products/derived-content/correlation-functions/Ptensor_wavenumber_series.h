//
// Created by David Seery on 19/07/2021.
// --@@
// Copyright (c) 2021 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_PTENSOR_WAVENUMBER_SERIES_H
#define CPPTRANSPORT_PTENSOR_WAVENUMBER_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime/derived-products/line-collections/data_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/Ptensor_line.h"
#include "transport-runtime/derived-products/derived-content/concepts/lines/Ptensor_nt_line.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
  {

    namespace derived_data
      {

        //! tensor power spectrum wavenumber data line
        template <typename number=default_number_type>
        class Ptensor_wavenumber_series: public wavenumber_series<number>, public Ptensor_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a tensor two-point function time series data object
            Ptensor_wavenumber_series(const twopf_db_task<number>& tk, SQL_time_query tq, SQL_twopf_query kq,
                                unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            Ptensor_wavenumber_series(Json::Value& reader, task_finder<number>& finder);

            //! destructor is default
            virtual ~Ptensor_wavenumber_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            derived_line_type get_line_type() const override { return derived_line_type::Ptensor_wavenumber; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


            // DERIVE LINES -- implements a 'wavenumber_series' interface

          public:

            //! generate data lines for plotting
            data_line_set<number> derive_lines
              (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(double t) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(double t) const;


            // CLONE

          public:

            //! self-replicate
            Ptensor_wavenumber_series<number>* clone() const override { return new Ptensor_wavenumber_series<number>(static_cast<const Ptensor_wavenumber_series<number>&>(*this)); }


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


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of wavenumber_series<>.
        // We have to call it ourselves
        template <typename number>
        Ptensor_wavenumber_series<number>::Ptensor_wavenumber_series
          (const twopf_db_task<number>& tk, SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
          : derived_line<number>(make_derivable_task_set_element(tk, false, false, false),
                                 axis_class::wavenumber, { axis_value::k, axis_value::efolds_exit }, prec),
            Ptensor_line<number>(tk),
            wavenumber_series<number>(tk),
            tquery(tq),
            kquery(kq)
          {
          }


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of wavenumber_series<>.
        // We have to call it ourselves
        template <typename number>
        Ptensor_wavenumber_series<number>::Ptensor_wavenumber_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            Ptensor_line<number>(reader, finder),
            wavenumber_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
          {
          }


        template <typename number>
        data_line_set<number> Ptensor_wavenumber_series<number>::derive_lines
          (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const
          {
            // attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

            // pull wavenumber-axis data
            std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

            // set up cache handles
            auto& kc_handle = pipe.new_twopf_kconfig_handle(this->kquery);
            auto& tc_handle = pipe.new_time_config_handle(this->tquery);
            auto& k_handle  = pipe.new_kconfig_data_handle(this->kquery);

            // pull time-configuration data information from the database
            time_config_tag<number> t_tag = pipe.new_time_config_tag();
            const std::vector< time_config > t_values = tc_handle.lookup_tag(t_tag);

            // pull k-configuration information from the database
            twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
            const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            data_line_set<number> lines;

            // loop through all components of the tensor twopf, pulling data from the database for each t-component
            for(const auto& t_value : t_values)
              {
                cf_kconfig_data_tag<number> tag =
                  pipe.new_cf_kconfig_data_tag(cf_data_type::cf_tensor_twopf,
                                               this->gadget.get_model()->tensor_flatten(0, 0), t_value.serial);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                std::vector<number> line_data = k_handle.lookup_tag(tag);
                assert(line_data.size() == k_values.size());

                // The factor of 4.0 is required because CppTransport stores the power spectrum for a single
                // tensor polarization.
                // The conventionally-defined tensor power spectrum is the sum over polarizations, which yields
                // a factor of 2. But also, in our normalization there is another factor of 2 that comes from
                // tracing over the polarization tensors.

                value_type value;
                if(this->dimensionless)
                  {
                    auto l_pos = line_data.begin();
                    auto k_pos = k_values.begin();
                    for(; l_pos != line_data.end() && k_pos != k_values.end(); ++l_pos, ++k_pos)
                      {
                        *l_pos *= 4.0 / (2.0*M_PI*M_PI);
                      }
                    value = value_type::dimensionless;
                  }
                else
                  {
                    auto l_pos = line_data.begin();
                    auto k_pos = k_values.begin();
                    for(; l_pos != line_data.end() && k_pos != k_values.end(); ++l_pos, ++k_pos)
                      {
                        double k_cube = k_pos->k_comoving * k_pos->k_comoving * k_pos->k_comoving;
                        *l_pos *=  4.0 / k_cube;
                      }
                    value = value_type::correlation_function;
                  }

                wavenumber_data_line_factory(*this, lines, group, this->x_type, value, w_axis, line_data,
                                             this->get_LaTeX_label(t_value.t),
                                             this->get_non_LaTeX_label(t_value.t), messages);
              }

            // detach pipe from content group
            this->detach(pipe);
            return lines;
          }


        template <typename number>
        std::string Ptensor_wavenumber_series<number>::get_LaTeX_label(double t) const
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
        std::string Ptensor_wavenumber_series<number>::get_non_LaTeX_label(double t) const
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


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is not called from wavenumber_series<>. We have to call it for ourselves.
        template <typename number>
        void Ptensor_wavenumber_series<number>::write(std::ostream& out)
          {
            this->Ptensor_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is not called from wavenumber_series<>. We have to call it for ourselves.
        template <typename number>
        void Ptensor_wavenumber_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PTENSOR_WAVENUMBER_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->Ptensor_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
          }

          
        //! tensor power spectrum spectral index (n_t) wavenumber data line
        template <typename number=default_number_type>
        class Ptensor_nt_wavenumber_series: public wavenumber_series<number>, public Ptensor_nt_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a tensor two-point function wavenumber series data object
            Ptensor_nt_wavenumber_series(const twopf_db_task<number>& tk, SQL_time_query tq, SQL_twopf_query kq,
                                   unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            Ptensor_nt_wavenumber_series(Json::Value& reader, task_finder<number>& finder);

            //! destructor is default
            virtual ~Ptensor_nt_wavenumber_series() = default;


            // TYPE INTROSPECTION

          public:

            //! get type
            derived_line_type get_line_type() const override { return derived_line_type::Ptensor_nt_wavenumber; }


            // EXTRACT QUERIES

          public:

            //! get time query
            const SQL_time_query& get_time_query() const { return(this->tquery); }

            //! get wavenumber query
            const SQL_twopf_query& get_k_query() const { return(this->kquery); }


            // DERIVE LINES -- implements a 'wavenumber_series' interface

          public:

            //! generate data lines for plotting
            data_line_set<number> derive_lines
              (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(double t) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(double t) const;


            // CLONE

          public:

            //! self-replicate
            Ptensor_nt_wavenumber_series<number>* clone() const override { return new Ptensor_nt_wavenumber_series<number>(static_cast<const Ptensor_nt_wavenumber_series<number>&>(*this)); }


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


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of wavenumber_series<>.
        // We have to call it ourselves
        template <typename number>
        Ptensor_nt_wavenumber_series<number>::Ptensor_nt_wavenumber_series
          (const twopf_db_task<number>& tk, SQL_time_query tq, SQL_twopf_query kq, unsigned int prec)
          : derived_line<number>(make_derivable_task_set_element(tk, false, false, true),
                                 axis_class::wavenumber, { axis_value::k, axis_value::efolds_exit }, prec),
            Ptensor_nt_line<number>(tk),
            wavenumber_series<number>(tk),
            tquery(tq),
            kquery(kq)
          {
          }


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is not called automatically during construction of wavenumber_series<>.
        // We have to call it ourselves
        template <typename number>
        Ptensor_nt_wavenumber_series<number>::Ptensor_nt_wavenumber_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            Ptensor_nt_line<number>(reader, finder),
            wavenumber_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
          {
          }


        template <typename number>
        data_line_set<number> Ptensor_nt_wavenumber_series<number>::derive_lines
          (datapipe<number>& pipe, const tag_list& tags, slave_message_buffer& messages) const
          {
            // attach our datapipe to a content group
            std::string group = this->attach(pipe, tags);

            // pull wavenumber-axis data
            std::vector<double> w_axis = this->pull_kconfig_axis(pipe, this->kquery);

            // set up cache handles
            auto& kc_handle = pipe.new_twopf_kconfig_handle(this->kquery);
            auto& tc_handle = pipe.new_time_config_handle(this->tquery);
            auto& k_handle  = pipe.new_kconfig_data_handle(this->kquery);

            // pull time-configuration data information from the database
            time_config_tag<number> t_tag = pipe.new_time_config_tag();
            const std::vector< time_config > t_values = tc_handle.lookup_tag(t_tag);

            // pull k-configuration information from the database
            twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
            const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            data_line_set<number> lines;

            // loop through all components of the tensor twopf, pulling data from the database for each t-component
            for(const auto& t_value : t_values)
              {
                cf_kconfig_data_tag<number> Ptensor_tag =
                  pipe.new_cf_kconfig_data_tag(cf_data_type::cf_tensor_twopf,
                                               this->gadget.get_model()->tensor_flatten(0, 0), t_value.serial);

                cf_kconfig_data_tag<number> dlogk_Ptensor_tag =
                  pipe.new_cf_kconfig_data_tag(cf_data_type::cf_tensor_twopf_dlogk,
                                               this->gadget.get_model()->tensor_flatten(0, 0), t_value.serial);

                std::vector<number> Ptensor_data = k_handle.lookup_tag(Ptensor_tag);
                std::vector<number> line_data = k_handle.lookup_tag(dlogk_Ptensor_tag);
                assert(Ptensor_data.size() == k_values.size());
                assert(line_data.size() == k_values.size());

                // Our definition of the spectral matrix gives the spectral index for the raw power spectrum P
                // rather than the dimensionless power spectrum \cal{P}, so we need to adjust it by a +3 offset.

                auto l_pos = line_data.begin();
                auto Pt_pos = Ptensor_data.cbegin();
                for(; l_pos != line_data.end() && Pt_pos != Ptensor_data.cend(); ++l_pos, ++Pt_pos)
                  {
                    *l_pos = 3.0 + (*l_pos)/(*Pt_pos);
                  }

                wavenumber_data_line_factory(*this, lines, group, this->x_type, value_type::spectral_index,
                                             w_axis, line_data, this->get_LaTeX_label(t_value.t),
                                             this->get_non_LaTeX_label(t_value.t), messages);
              }

            // detach pipe from content group
            this->detach(pipe);
            return lines;
          }


        template <typename number>
        std::string Ptensor_nt_wavenumber_series<number>::get_LaTeX_label(double t) const
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
        std::string Ptensor_nt_wavenumber_series<number>::get_non_LaTeX_label(double t) const
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


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is not called from wavenumber_series<>. We have to call it for ourselves.
        template <typename number>
        void Ptensor_nt_wavenumber_series<number>::write(std::ostream& out)
          {
            this->Ptensor_nt_line<number>::write(out);
            this->wavenumber_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because wavenumber_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is not called from wavenumber_series<>. We have to call it for ourselves.
        template <typename number>
        void Ptensor_nt_wavenumber_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_PTENSOR_NT_WAVENUMBER_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->Ptensor_nt_line<number>::serialize(writer);
            this->wavenumber_series<number>::serialize(writer);
          }

      }   // namespace derived_data

  }   // namespace transport

#endif //CPPTRANSPORT_PTENSOR_WAVENUMBER_SERIES_H
