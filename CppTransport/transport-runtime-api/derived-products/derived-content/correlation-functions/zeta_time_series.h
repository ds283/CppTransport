//
// Created by David Seery on 21/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_ZETA_TIME_SERIES_H
#define CPPTRANSPORT_ZETA_TIME_SERIES_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_threepf_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_reduced_bispectrum_line.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"

namespace transport
  {

    namespace derived_data
      {

        //! zeta twopf time data line
        template <typename number>
        class zeta_twopf_time_series : public time_series<number>, public zeta_twopf_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta two-pf time data object
            zeta_twopf_time_series(const zeta_twopf_list_task<number>& tk,
                                   SQL_time_config_query tq, SQL_twopf_kconfig_query kq,
                                   unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_twopf_time_series(Json::Value& reader, task_finder<number>& finder);

            virtual ~zeta_twopf_time_series() = default;


            // DERIVE LINES -- implements a 'time_series' interface

          public:

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(const twopf_kconfig& k) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(const twopf_kconfig& k) const;


            // CLONE

          public:

            //! self-replicate
            virtual zeta_twopf_time_series<number>* clone() const override { return new zeta_twopf_time_series<number>(static_cast<const zeta_twopf_time_series<number>&>(*this)); }


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

            //! SQL query representing the x-axis
            SQL_time_config_query tquery;

            //! SQL query representing different lines
            SQL_twopf_kconfig_query kquery;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(const zeta_twopf_list_task<number>& tk,
                                                               SQL_time_config_query tq, SQL_twopf_kconfig_query kq, unsigned int prec)
	        : derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
	          zeta_twopf_line<number>(tk),
	          time_series<number>(tk),
            tquery(tq),
            kquery(kq)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            zeta_twopf_line<number>(reader),
            time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
          {
          }


        template <typename number>
        void zeta_twopf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                          const std::list<std::string>& tags) const
          {
            // attach datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
            typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kquery);
            typename datapipe<number>::time_zeta_handle&     z_handle  = pipe.new_time_zeta_handle(this->tquery);

            // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
            const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            for(std::vector<twopf_kconfig>::const_iterator t = k_values.begin(); t != k_values.end(); ++t)
              {
		            zeta_twopf_time_data_tag<number> tag = pipe.new_zeta_twopf_time_data_tag(*t);

                std::vector<number> line_data = z_handle.lookup_tag(tag);

                value_type value;
                if(this->dimensionless)
                  {
                    for(unsigned int j = 0; j < line_data.size(); ++j)
                      {
                        line_data[j] *= 1.0 / (2.0*M_PI*M_PI);
                      }
                    value = value_type::dimensionless_value;
                  }
                else
                  {
                    double k_cube = t->k_comoving * t->k_comoving * t->k_comoving;
                    for(unsigned int j = 0; j < line_data.size(); ++j)
                      {
                        line_data[j] *= 1.0 / k_cube;
                      }
                    value = value_type::correlation_function_value;
                  }

                data_line<number> line = data_line<number>(group, this->x_type, value, t_axis, line_data,
                                                           this->get_LaTeX_label(*t), this->get_non_LaTeX_label(*t));
                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        template <typename number>
        std::string zeta_twopf_time_series<number>::get_LaTeX_label(const twopf_kconfig& k) const
	        {
            std::string tag = this->make_LaTeX_tag(k);
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
        std::string zeta_twopf_time_series<number>::get_non_LaTeX_label(const twopf_kconfig& k) const
	        {
            std::string tag = this->make_non_LaTeX_tag(k);
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


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_twopf_time_series<number>::write(std::ostream& out)
          {
            this->zeta_twopf_line<number>::write(out);
            this->time_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_twopf_time_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->zeta_twopf_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


        //! zeta threepf time data line
        template <typename number>
        class zeta_threepf_time_series: public time_series<number>, public zeta_threepf_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta_threepf_time_series object
            zeta_threepf_time_series(const zeta_threepf_task<number>& tk,
                                     SQL_time_config_query tq, SQL_threepf_kconfig_query kq,
                                     unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_threepf_time_series(Json::Value& reader, task_finder<number>& finder);

            virtual ~zeta_threepf_time_series() = default;


            // DERIVE LINES -- implements a 'derived_line' interface

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list< data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(const threepf_kconfig& k) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(const threepf_kconfig& k) const;


            // CLONE

            //! self-replicate
            virtual zeta_threepf_time_series<number>* clone() const override { return new zeta_threepf_time_series<number>(static_cast<const zeta_threepf_time_series<number>&>(*this)); }


            // WRITE TO A STREAM

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! serialize this object
            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing the x-axis
            SQL_time_config_query tquery;

            //! SQL query representing different lines
            SQL_threepf_kconfig_query kquery;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(const zeta_threepf_task<number>& tk,
                                                                   SQL_time_config_query tq, SQL_threepf_kconfig_query kq,
                                                                   unsigned int prec)
          : derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
            zeta_threepf_line<number>(tk),
            time_series<number>(tk),
            tquery(tq),
            kquery(kq)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            zeta_threepf_line<number>(reader),
            time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
          {
          }


        template <typename number>
        void zeta_threepf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                            const std::list<std::string>& tags) const
          {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

		        // set up cache handles
		        typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kquery);
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->tquery);

            // pull k-configuration information from the database
		        threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
            const typename std::vector< threepf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            for(std::vector<threepf_kconfig>::const_iterator t = k_values.begin(); t != k_values.end(); ++t)
              {
		            zeta_threepf_time_data_tag<number> tag = pipe.new_zeta_threepf_time_data_tag(*t);

                std::vector<number> line_data = z_handle.lookup_tag(tag);

                std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(*t, this->use_kt_label, this->use_alpha_label, this->use_beta_label) + "$";
                std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(*t, this->use_kt_label, this->use_alpha_label, this->use_beta_label);

                value_type value;
                if(this->dimensionless)
                  {
                    value = value_type::dimensionless_value;
                  }
                else
                  {
                    double shape = t->k1_comoving*t->k1_comoving * t->k2_comoving*t->k2_comoving * t->k3_comoving*t->k3_comoving;
                    for(unsigned int j = 0; j < line_data.size(); ++j)
                      {
                        line_data[j] *= 1.0/shape;
                      }
                    value = value_type::correlation_function_value;
                  }

                data_line<number> line = data_line<number>(group, this->x_type, value, t_axis, line_data,
                                                           this->get_LaTeX_label(*t), this->get_non_LaTeX_label(*t));

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        template <typename number>
        std::string zeta_threepf_time_series<number>::get_LaTeX_label(const threepf_kconfig& k) const
	        {
            std::string tag = this->make_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
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
        std::string zeta_threepf_time_series<number>::get_non_LaTeX_label(const threepf_kconfig& k) const
	        {
            std::string tag = this->make_non_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
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


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_threepf_time_series<number>::write(std::ostream& out)
          {
            this->zeta_threepf_line<number>::write(out);
            this->time_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_threepf_time_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->zeta_threepf_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


        //! zeta reduced bispectrum time data line
        template <typename number>
        class zeta_reduced_bispectrum_time_series: public time_series<number>, public zeta_reduced_bispectrum_line<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! construct a zeta_reduced_bispectrum_time_series object
            zeta_reduced_bispectrum_time_series(const zeta_threepf_task<number>& tk,
                                                SQL_time_config_query tq, SQL_threepf_kconfig_query kq,
                                                unsigned int prec = CPPTRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_reduced_bispectrum_time_series(Json::Value& reader, task_finder<number>& finder);

            virtual ~zeta_reduced_bispectrum_time_series() = default;


            // DERIVE LINES -- implements a 'derived line' interface

            //! generate data lines for plotting
            virtual void derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                      const std::list<std::string>& tags) const override;

            //! generate a LaTeX label
            std::string get_LaTeX_label(const threepf_kconfig& k) const;

            //! generate a non-LaTeX label
            std::string get_non_LaTeX_label(const threepf_kconfig& k) const;


            // CLONE

            //! self-replicate
            virtual zeta_reduced_bispectrum_time_series<number>* clone() const override { return new zeta_reduced_bispectrum_time_series<number>(static_cast<const zeta_reduced_bispectrum_time_series<number>&>(*this)); }


            // WRITE TO A STREAM

            //! write self-details to a stream
            virtual void write(std::ostream& out) override;


            // SERIALIZATION -- implements a 'serializable' interface

          public:

            //! serialize this object
            virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

            //! SQL query representing the x-axis
            SQL_time_config_query tquery;

            //! SQL query representing different lines
            SQL_threepf_kconfig_query kquery;

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_reduced_bispectrum_time_series<number>::zeta_reduced_bispectrum_time_series(const zeta_threepf_task<number>& tk,
                                                                                         SQL_time_config_query tq, SQL_threepf_kconfig_query kq,
                                                                                         unsigned int prec)
          : derived_line<number>(tk, axis_class::time_axis, std::list<axis_value>{ axis_value::efolds_axis }, prec),
            zeta_reduced_bispectrum_line<number>(tk),
            time_series<number>(tk),
            tquery(tq),
            kquery(kq)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_reduced_bispectrum_time_series<number>::zeta_reduced_bispectrum_time_series(Json::Value& reader, task_finder<number>& finder)
          : derived_line<number>(reader, finder),
            zeta_reduced_bispectrum_line<number>(reader),
            time_series<number>(reader),
            tquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]),
            kquery(reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY])
          {
          }


        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                                       const std::list<std::string>& tags) const
          {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe, this->tquery);

            // set up cache handles
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kquery);
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->tquery);

            // pull k-configuration information from the database
            threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
            const typename std::vector< threepf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            for(std::vector<threepf_kconfig>::const_iterator t = k_values.begin(); t != k_values.end(); ++t)
              {
		            zeta_reduced_bispectrum_time_data_tag<number> tag = pipe.new_zeta_reduced_bispectrum_time_data_tag(*t);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

                data_line<number> line = data_line<number>(group, this->x_type, value_type::fNL_value, t_axis, line_data,
                                                           this->get_LaTeX_label(*t), this->get_non_LaTeX_label(*t));

                lines.push_back(line);
              }

            // detach pipe from output group
            this->detach(pipe);
          }


        template <typename number>
        std::string zeta_reduced_bispectrum_time_series<number>::get_LaTeX_label(const threepf_kconfig& k) const
	        {
            std::string tag = this->make_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
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
        std::string zeta_reduced_bispectrum_time_series<number>::get_non_LaTeX_label(const threepf_kconfig& k) const
	        {
            std::string tag = this->make_non_LaTeX_tag(k, this->use_kt_label, this->use_alpha_label, this->use_beta_label);
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


        // note that because time_series<> inherits virtually from derived_line<>, the write method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::write(std::ostream& out)
          {
            this->zeta_reduced_bispectrum_line<number>::write(out);
            this->time_series<number>::write(out);
            this->derived_line<number>::write(out);
          }


        // note that because time_series<> inherits virtually from derived_line<>, the serialize method for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::serialize(Json::Value& writer) const
          {
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES);

            this->tquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_T_QUERY]);
            this->kquery.serialize(writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_K_QUERY]);

            this->derived_line<number>::serialize(writer);
            this->zeta_reduced_bispectrum_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif // CPPTRANSPORT_ZETA_TIME_SERIES_H
