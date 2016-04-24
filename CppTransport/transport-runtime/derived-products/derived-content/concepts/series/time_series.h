//
// Created by David Seery on 19/05/2014.
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


#ifndef __time_series_H_
#define __time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime/data/datapipe/datapipe_decl.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/utilities/latex_output.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"


namespace transport
	{

		namespace derived_data
			{

		    //! general time-series content producer, suitable for
		    //! producing content usable in eg. a 2d plot or table.
				//! Note we derive virtually from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<>attributes,
				//! eg. time_series<> and twopf_line<>
		    template <typename number>
		    class time_series: public virtual derived_line<number>
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

		        //! Basic user-facing constructor
		        time_series(const derivable_task<number>& tk);

		        //! Deserialization constructor
		        time_series(Json::Value& reader);

		        virtual ~time_series() = default;


		        // DATAPIPE SERVICES

		      public:

		        //! extract axis data, corresponding to our sample times, from datapipe
		        std::vector<double> pull_time_axis(datapipe<number>& pipe, const SQL_time_config_query& tquery) const;


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label tag for a twopf k-configuration
				    std::string make_LaTeX_tag(const twopf_kconfig& config) const;

				    //! make a non-LaTeX label tag for a twopf k-configuration
				    std::string make_non_LaTeX_tag(const twopf_kconfig& config) const;

		        //! make a LaTeX label tag for a threepf k-configuration
		        std::string make_LaTeX_tag(const threepf_kconfig& config,
		                                   bool use_kt, bool use_alpha, bool use_beta) const;

				    //! make a non-LaTeX label tag for a threepf k-configuration
				    std::string make_non_LaTeX_tag(const threepf_kconfig& config,
				                                   bool use_kt, bool use_alpha, bool use_beta) const;


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! Serialize this object
		        virtual void serialize(Json::Value& writer) const override;

			    };


		    template <typename number>
		    time_series<number>::time_series(const derivable_task<number>& tk)
		      : derived_line<number>(tk)  // not called because of virtual inheritance; here to silence Intel compiler warning
			    {
          }


		    template <typename number>
		    time_series<number>::time_series(Json::Value& reader)
					: derived_line<number>(reader)  // not called because of virtual inheritance; here to silence Intel compiler warning
			    {
			    }


		    template <typename number>
		    std::vector<double> time_series<number>::pull_time_axis(datapipe<number>& pipe, const SQL_time_config_query& tquery) const
			    {
				    assert(this->x_type == axis_value::efolds_axis);
		        if(!pipe.validate_attached()) throw runtime_exception(exception_type::DATAPIPE_ERROR, CPPTRANSPORT_PRODUCT_TIME_SERIES_NULL_DATAPIPE);

		        // set-up time sample data
				    typename datapipe<number>::time_config_handle& handle = pipe.new_time_config_handle(tquery);
				    time_config_tag<number> tag = pipe.new_time_config_tag();

            const std::vector< time_config >& t_values = handle.lookup_tag(tag);

            std::vector<double> t_axis(t_values.size());
            unsigned int i = 0;
            for(std::vector<time_config>::const_iterator t = t_values.begin(); t != t_values.end(); ++t, ++i)
              {
                t_axis[i] = t->t;
              }

				    return(t_axis);
			    }


				template <typename number>
				std::string time_series<number>::make_non_LaTeX_tag(const twopf_kconfig& config) const
					{
				    std::ostringstream label;

				    label << std::setprecision(this->precision);

				    label << CPPTRANSPORT_NONLATEX_K_SYMBOL << "=";
            switch(this->get_klabel_meaning())
              {
                case klabel_type::conventional:
                  label << config.k_conventional;
                  break;

                case klabel_type::comoving:
                  label << config.k_comoving;
                  break;
              }

				    return(label.str());
					}


				template <typename number>
				std::string time_series<number>::make_LaTeX_tag(const twopf_kconfig& config) const
					{
				    std::ostringstream label;

				    label << CPPTRANSPORT_LATEX_K_SYMBOL << "=";
            switch(this->get_klabel_meaning())
              {
                case klabel_type::conventional:
                  label << output_latex_number(config.k_conventional, this->precision);
                  break;

                case klabel_type::comoving:
                  label << output_latex_number(config.k_comoving, this->precision);
                  break;
              }

            return(label.str());
					}


		    template <typename number>
		    std::string time_series<number>::make_LaTeX_tag(const threepf_kconfig& config,
		                                                    bool use_kt, bool use_alpha, bool use_beta) const
			    {
		        std::ostringstream label;

		        unsigned int count = 0;
		        if(use_kt)
			        {
		            label << (count > 0 ? ",\\, " : "") << CPPTRANSPORT_LATEX_KT_SYMBOL << "=";
                switch(this->get_klabel_meaning())
                  {
                    case klabel_type::conventional:
                      label << output_latex_number(config.kt_conventional, this->precision);
                      break;

                    case klabel_type::comoving:
                      label << output_latex_number(config.kt_comoving, this->precision);
                      break;
                  }
		            count++;
			        }
		        if(use_alpha)
			        {
		            label << (count > 0 ? ",\\, " : "") << CPPTRANSPORT_LATEX_ALPHA_SYMBOL << "=" << output_latex_number(config.alpha, this->precision);
		            count++;
			        }
		        if(use_beta)
			        {
		            label << (count > 0 ? ",\\, " : "") << CPPTRANSPORT_LATEX_BETA_SYMBOL << "=" << output_latex_number(config.beta, this->precision);
		            count++;
			        }

		        return (label.str());
			    }


				template <typename number>
				std::string time_series<number>::make_non_LaTeX_tag(const threepf_kconfig& config,
				                                                    bool use_kt, bool use_alpha, bool use_beta) const
					{
				    std::ostringstream label;

				    unsigned int count = 0;
				    if(use_kt)
					    {
				        label << (count > 0 ? ", " : "") << CPPTRANSPORT_NONLATEX_KT_SYMBOL << "=";
                switch(this->get_klabel_meaning())
                  {
                    case klabel_type::conventional:
                      label << config.kt_conventional;
                      break;

                    case klabel_type::comoving:
                      label << config.kt_comoving;
                      break;
                  }
				        count++;
					    }
				    if(use_alpha)
					    {
				        label << (count > 0 ? ", " : "") << CPPTRANSPORT_NONLATEX_ALPHA_SYMBOL << "=" << config.alpha;
				        count++;
					    }
				    if(use_beta)
					    {
				        label << (count > 0 ? ", " : "") << CPPTRANSPORT_NONLATEX_BETA_SYMBOL << "=" << config.beta;
				        count++;
					    }

				    return (label.str());
					}


		    template <typename number>
		    void time_series<number>::serialize(Json::Value& writer) const
			    {
			    }


		    template <typename number>
		    void time_series<number>::write(std::ostream& out)
			    {
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif //__time_series_H_
