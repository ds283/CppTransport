//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __wavenumber_series_H_
#define __wavenumber_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"

#include "transport-runtime-api/utilities/latex_output.h"

#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/threepf_kconfig_shift.h"
#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"


#define CPPTRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_ROOT           "momentum-configuration-series"
#define CPPTRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_SPECTRAL_INDEX "spectral-index"


namespace transport
	{

    namespace derived_data
	    {

		    //! general wavenumber-series content producer, suitable
		    //! for producing content usable in eg. a 2d plot or table.
		    //! Note we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and twopf_line<>
        template <typename number>
        class wavenumber_series: public virtual derived_line<number>
	        {

	          // CONSTRUCTOR, DESTRUCTOR

          public:

		        //! Basic user-facing constructor
		        wavenumber_series(const derivable_task<number>& tk);

		        //! Deserialization constructor
		        wavenumber_series(Json::Value& reader);

		        virtual ~wavenumber_series() = default;


		        // SPECTRAL INDEX CALCULATION

          public:

		        //! are we a spectral index series?
		        bool is_spectral_index() const { return(this->compute_index); }

		        //! set spectral index calculation
		        void set_spectral_index(bool g) { this->compute_index = g; }


            // DATAPIPE SERVICES

          public:

            //! extract axis data, corresponding to our sample wavenumbers, from datapipe
            std::vector<double> pull_kconfig_axis(datapipe<number>& pipe, const SQL_twopf_kconfig_query& kquery) const;

            //! extract axis data, corresponding to our sample wavenumbers, from datapipe
            std::vector<double> pull_kconfig_axis(datapipe<number>& pipe, const SQL_threepf_kconfig_query& kquery) const;


		        // LABELLING SERVICES

          public:

		        //! Make a LaTeX label tag for a time configuration
		        std::string make_LaTeX_tag(double config) const;

		        //! Make a non-LaTeX label tag for a time configuration
		        std::string make_non_LaTeX_tag(double config) const;


		        // WRITE TO A STREAM

          public:

		        //! Write self-details to a stream
		        virtual void write(std::ostream& out);


		        // SERIALIZATION -- implements a 'serializable' interface

          public:

		        //! Serialize this object
		        virtual void serialize(Json::Value& writer) const override;


            // INTERNAL DATA

          protected:

		        // RUNTIME AGENTS

		        // momentum->derivative shifter
            threepf_kconfig_shift<number> shifter;


		        // SETTINGS

						//! compute spectral index?
		        bool compute_index;

	        };


        // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
		    template <typename number>
		    wavenumber_series<number>::wavenumber_series(const derivable_task<number>& tk)
          : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
            compute_index(false)
			    {
          }


		    template <typename number>
		    wavenumber_series<number>::wavenumber_series(Json::Value& reader)
          : derived_line<number>(reader)  // not called because of virtual inheritance; here to silence Intel compiler warning
			    {
		        compute_index = reader[CPPTRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_ROOT][CPPTRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_SPECTRAL_INDEX].asBool();
			    }




        template <typename number>
        std::vector<double> wavenumber_series<number>::pull_kconfig_axis(datapipe<number>& pipe, const SQL_twopf_kconfig_query& kquery) const
	        {
		        assert(this->x_type == axis_value::k_axis);

            typename datapipe<number>::twopf_kconfig_handle& handle = pipe.new_twopf_kconfig_handle(kquery);
            twopf_kconfig_tag<number> tag = pipe.new_twopf_kconfig_tag();

            // safe to take a reference here and avoid a copy
            const std::vector< twopf_kconfig >& configs = handle.lookup_tag(tag);

            std::vector<double> axis;

		        switch(this->x_type)
			        {
		            case axis_value::k_axis:
			            {
		                for(typename std::vector< twopf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
                        switch(this->klabel_meaning)
                          {
                            case klabel_type::comoving:
                              axis.push_back((*t).k_comoving);
                              break;

                            case klabel_type::conventional:
                              axis.push_back((*t).k_conventional);
                              break;
                          }
			                }
				            break;
			            }

		            case axis_value::efolds_exit_axis:
			            {
		                for(typename std::vector< twopf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
		                    axis.push_back(t->t_exit);
			                }
		                break;
			            }

		            default:
			            assert(false);

			        }

		        return(axis);
	        }


        template <typename number>
        std::vector<double> wavenumber_series<number>::pull_kconfig_axis(datapipe<number>& pipe, const SQL_threepf_kconfig_query& kquery) const
	        {
            typename datapipe<number>::threepf_kconfig_handle& handle = pipe.new_threepf_kconfig_handle(kquery);
            threepf_kconfig_tag<number> tag = pipe.new_threepf_kconfig_tag();

            // safe to take a reference here
            const std::vector< threepf_kconfig >& configs = handle.lookup_tag(tag);

            std::vector<double> axis;

		        switch(this->x_type)
			        {
		            case axis_value::k_axis:
			            {
				            // axis consists of k_t values
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
                        switch(this->klabel_meaning)
                          {
                            case klabel_type::comoving:
                              axis.push_back(t->kt_comoving);
                              break;

                            case klabel_type::conventional:
                              axis.push_back(t->kt_conventional);
                              break;
                          }
			                }
				            break;
			            }

		            case axis_value::efolds_exit_axis:
			            {
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
				                axis.push_back(t->t_exit);
			                }
		                break;
			            }

		            case axis_value::alpha_axis:
			            {
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
		                    axis.push_back(t->alpha);
			                }
		                break;
			            }

		            case axis_value::beta_axis:
			            {
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
		                    axis.push_back(t->beta);
			                }
		                break;
			            }

		            case axis_value::squeezing_fraction_k1_axis:
			            {
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
		                    axis.push_back(t->k1_conventional/t->kt_conventional);
			                }
		                break;
			            }

		            case axis_value::squeezing_fraction_k2_axis:
			            {
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
		                    axis.push_back(t->k2_conventional/t->kt_conventional);
			                }
		                break;
			            }

		            case axis_value::squeezing_fraction_k3_axis:
			            {
		                for(typename std::vector< threepf_kconfig >::const_iterator t = configs.begin(); t != configs.end(); ++t)
			                {
		                    axis.push_back(t->k3_conventional/t->kt_conventional);
			                }
		                break;
			            }

		            default:
			            assert(false);
			        };


		        return(axis);
	        }



        template <typename number>
		    std::string wavenumber_series<number>::make_LaTeX_tag(double config) const
			    {
		        std::ostringstream label;

				    label << CPPTRANSPORT_LATEX_T_SYMBOL << "=" << output_latex_number(config, this->precision);

				    return(label.str());
			    }


		    template <typename number>
		    std::string wavenumber_series<number>::make_non_LaTeX_tag(double config) const
			    {
		        std::ostringstream label;

				    label << std::setprecision(this->precision);

				    label << CPPTRANSPORT_NONLATEX_T_SYMBOL << "=" << config;

				    return(label.str());
			    }


		    template <typename number>
			  void wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
				    // DON'T CALL derived_line<> serialization because of virtual inheritance;
				    // concrete classes must call it themselves

		        writer[CPPTRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_ROOT][CPPTRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_SPECTRAL_INDEX] = this->compute_index;
			    }


		    template <typename number>
		    void wavenumber_series<number>::write(std::ostream& out)
			    {
			    }


	    }   // namespace derived_data

}   // namespace transport


#endif //__wavenumber_series_H_
