//
// Created by David Seery on 21/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_time_series_H_
#define __zeta_time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/time_series.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_twopf_line.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_threepf_line.h"
#include "transport-runtime-api/derived-products/derived-content/zeta_reduced_bispectrum_line.h"

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
                                   filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter,
                                   unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_twopf_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

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

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(const zeta_twopf_list_task<number>& tk,
                                                               filter::time_filter tfilter, filter::twopf_kconfig_filter kfilter, unsigned int prec)
	        : derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }, prec),
	          zeta_twopf_line<number>(tk, kfilter),
	          time_series<number>(tk, tfilter)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_twopf_time_series<number>::zeta_twopf_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            zeta_twopf_line<number>(reader),
            time_series<number>(reader)
          {
          }


        template <typename number>
        void zeta_twopf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                          const std::list<std::string>& tags) const
          {
            // attach datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe);

		        // set up cache handles
            typename datapipe<number>::twopf_kconfig_handle& kc_handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
            typename datapipe<number>::time_zeta_handle&     z_handle  = pipe.new_time_zeta_handle(this->time_sample_sns);

            // pull k-configuration information from the database
		        twopf_kconfig_tag<number> k_tag = pipe.new_twopf_kconfig_tag();
            const typename std::vector< twopf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
		            zeta_twopf_time_data_tag<number> tag = pipe.new_zeta_twopf_time_data_tag(k_values[i]);

                std::vector<number> line_data = z_handle.lookup_tag(tag);

		            if(this->dimensionless)
			            {
				            for(unsigned int j = 0; j < line_data.size(); j++)
					            {
						            line_data[j] *= k_values[i].k_comoving*k_values[i].k_comoving*k_values[i].k_comoving / (2.0*M_PI*M_PI);
					            }

		                data_line<number> line = data_line<number>(group, this->x_type, dimensionless_value, t_axis, line_data,
		                                                           this->get_LaTeX_label(k_values[i]), this->get_non_LaTeX_label(k_values[i]));
		                lines.push_back(line);
			            }
		            else
			            {
		                data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, t_axis, line_data,
		                                                           this->get_LaTeX_label(k_values[i]), this->get_non_LaTeX_label(k_values[i]));
		                lines.push_back(line);
			            }
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
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_TWOPF_TIME_SERIES);

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
            zeta_threepf_time_series(const zeta_threepf_task<number>& tk, filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                     unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_threepf_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

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

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(const zeta_threepf_task<number>& tk,
                                                                   filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter, unsigned int prec)
          : derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }, prec),
            zeta_threepf_line<number>(tk, kfilter),
            time_series<number>(tk, tfilter)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_threepf_time_series<number>::zeta_threepf_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            zeta_threepf_line<number>(reader),
            time_series<number>(reader)
          {
          }


        template <typename number>
        void zeta_threepf_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                            const std::list<std::string>& tags) const
          {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe);

		        // set up cache handles
		        typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->time_sample_sns);

            // pull k-configuration information from the database
		        threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
            const typename std::vector< threepf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
		            zeta_threepf_time_data_tag<number> tag = pipe.new_zeta_threepf_time_data_tag(k_values[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

                std::string latex_label = "$" + this->make_LaTeX_label() + "\\;" + this->make_LaTeX_tag(k_values[i], this->use_kt_label, this->use_alpha_label, this->use_beta_label) + "$";
                std::string nonlatex_label = this->make_non_LaTeX_label() + " " + this->make_non_LaTeX_tag(k_values[i], this->use_kt_label, this->use_alpha_label, this->use_beta_label);

                data_line<number> line = data_line<number>(group, this->x_type, correlation_function_value, t_axis, line_data,
                                                           this->get_LaTeX_label(k_values[i]), this->get_non_LaTeX_label(k_values[i]));

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
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_THREEPF_TIME_SERIES);

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
                                                filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                                unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

            //! deserialization constructor
            zeta_reduced_bispectrum_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

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

          };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_reduced_bispectrum_time_series<number>::zeta_reduced_bispectrum_time_series(const zeta_threepf_task<number>& tk,
                                                                                         filter::time_filter tfilter, filter::threepf_kconfig_filter kfilter,
                                                                                         unsigned int prec)
          : derived_line<number>(tk, time_axis, std::list<axis_value>{ efolds_axis }, prec),
            zeta_reduced_bispectrum_line<number>(tk, kfilter),
            time_series<number>(tk, tfilter)
          {
          }


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>. We have to call it ourselves.
        template <typename number>
        zeta_reduced_bispectrum_time_series<number>::zeta_reduced_bispectrum_time_series(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
          : derived_line<number>(reader, finder),
            zeta_reduced_bispectrum_line<number>(reader),
            time_series<number>(reader)
          {
          }


        template <typename number>
        void zeta_reduced_bispectrum_time_series<number>::derive_lines(datapipe<number>& pipe, std::list<data_line<number> >& lines,
                                                                       const std::list<std::string>& tags) const
          {
            // attach our datapipe to an output group
            std::string group = this->attach(pipe, tags);

            const std::vector<double> t_axis = this->pull_time_axis(pipe);

            // set up cache handles
            typename datapipe<number>::threepf_kconfig_handle& kc_handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename datapipe<number>::time_zeta_handle& z_handle = pipe.new_time_zeta_handle(this->time_sample_sns);

            // pull k-configuration information from the database
            threepf_kconfig_tag<number> k_tag = pipe.new_threepf_kconfig_tag();
            const typename std::vector< threepf_kconfig > k_values = kc_handle.lookup_tag(k_tag);

            for(unsigned int i = 0; i < this->kconfig_sample_sns.size(); i++)
              {
                BOOST_LOG_SEV(pipe.get_log(), datapipe<number>::normal) << "§§ Processing 3pf k-configuration " << i << std::endl;

		            zeta_reduced_bispectrum_time_data_tag<number> tag = pipe.new_zeta_reduced_bispectrum_time_data_tag(k_values[i]);

                // it's safe to take a reference here to avoid a copy; we don't need the cache data to survive over multiple calls to lookup_tag()
                const std::vector<number>& line_data = z_handle.lookup_tag(tag);

                data_line<number> line = data_line<number>(group, this->x_type, fNL_value, t_axis, line_data,
                                                           this->get_LaTeX_label(k_values[i]), this->get_non_LaTeX_label(k_values[i]));

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
            writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_TYPE] = std::string(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_ZETA_REDUCED_BISPECTRUM_TIME_SERIES);

            this->derived_line<number>::serialize(writer);
            this->zeta_reduced_bispectrum_line<number>::serialize(writer);
            this->time_series<number>::serialize(writer);
          }


      }   // namespace derived_data

  }   // namespace transport


#endif // __zeta_time_series_H_
