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
#include "transport-runtime-api/derived-products/utilities/filter.h"

#include "transport-runtime-api/utilities/latex_output.h"

#include "transport-runtime-api/derived-products/derived-content/threepf_kconfig_shift.h"


#define __CPP_TRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_ROOT           "momentum-configuration-series"
#define __CPP_TRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_SPECTRAL_INDEX "spectral-index"


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
		        wavenumber_series(const derivable_task<number>& tk, filter::time_filter tfilter);

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
            std::vector<double> pull_twopf_kconfig_axis(datapipe<number>& pipe) const;

            //! extract axis data, corresponding to our sample wavenumbers, from datapipe
            std::vector<double> pull_threepf_kconfig_axis(datapipe<number>& pipe) const;


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

		        //! Write details of the k-configurations included in this product - twopf version
		        void write_kconfig_list(std::ostream& out, const twopf_kconfig_database& twopf_db);

		        //! Write details of the k-configurations included in this product - threepf version
		        void write_kconfig_list(std::ostream& out, const threepf_kconfig_database& threepf_db);


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
		    wavenumber_series<number>::wavenumber_series(const derivable_task<number>& tk, filter::time_filter tfilter)
          : derived_line<number>(tk),
            compute_index(false)
			    {
				    // set up list of serial numbers corresponding to the sample times for this derived line
            try
              {
                this->f.filter_time_sample(tfilter, tk.get_time_config_list(), this->time_sample_sns);
              }
            catch(runtime_exception& xe)
              {
                if(xe.get_exception_code() == runtime_exception::FILTER_EMPTY)
                  {
                    std::ostringstream msg;
                    msg << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_EMPTY_FILTER << " '" << this->get_parent_task()->get_name() << "'";
                    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
                  }
                else throw xe;
              }
          }


        // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
		    template <typename number>
		    wavenumber_series<number>::wavenumber_series(Json::Value& reader)
          : derived_line<number>(reader)
			    {
		        compute_index = reader[__CPP_TRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_ROOT][__CPP_TRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_SPECTRAL_INDEX].asBool();
			    }




        template <typename number>
        std::vector<double> wavenumber_series<number>::pull_twopf_kconfig_axis(datapipe<number>& pipe) const
	        {
		        assert(this->x_type == k_axis);

            typename datapipe<number>::twopf_kconfig_handle& handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
            twopf_kconfig_tag<number> tag = pipe.new_twopf_kconfig_tag();

            // safe to take a reference here and avoid a copy
            const std::vector< twopf_configuration >& configs = handle.lookup_tag(tag);

            std::vector<double> axis;

            for(typename std::vector< twopf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
	            {
                if(this->klabel_meaning == derived_line<number>::comoving) axis.push_back((*t).k_comoving);
                else if(this->klabel_meaning == derived_line<number>::conventional) axis.push_back((*t).k_conventional);
                else
	                {
                    assert(false);
                    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
	                }
	            }

		        return(axis);
	        }


        template <typename number>
        std::vector<double> wavenumber_series<number>::pull_threepf_kconfig_axis(datapipe<number>& pipe) const
	        {
            typename datapipe<number>::threepf_kconfig_handle& handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
            threepf_kconfig_tag<number> tag = pipe.new_threepf_kconfig_tag();

            // safe to take a reference here
            const std::vector< threepf_configuration >& configs = handle.lookup_tag(tag);

            std::vector<double> axis;

		        switch(this->x_type)
			        {
		            case k_axis:
			            {
				            // axis consists of k_t values
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			                {
		                    if(this->klabel_meaning == derived_line<number>::comoving) axis.push_back(t->kt_comoving);
		                    else if(this->klabel_meaning == derived_line<number>::conventional) axis.push_back(t->kt_conventional);
		                    else
			                    {
		                        assert(false);
		                        throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
			                    }
			                }
				            break;
			            }

		            case efolds_exit_axis:
			            {
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			                {
				                axis.push_back(log(t->kt_conventional));
			                }
		                break;
			            }

		            case alpha_axis:
			            {
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			                {
		                    axis.push_back(t->alpha);
			                }
		                break;
			            }

		            case beta_axis:
			            {
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			                {
		                    axis.push_back(t->beta);
			                }
		                break;
			            }

		            case squeezing_fraction_k1_axis:
			            {
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			                {
		                    axis.push_back(t->k1_conventional/t->kt_conventional);
			                }
		                break;
			            }

		            case squeezing_fraction_k2_axis:
			            {
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			                {
		                    axis.push_back(t->k2_conventional/t->kt_conventional);
			                }
		                break;
			            }

		            case squeezing_fraction_k3_axis:
			            {
		                for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
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

				    label << __CPP_TRANSPORT_LATEX_T_SYMBOL << "=" << output_latex_number(config, this->precision);

				    return(label.str());
			    }


		    template <typename number>
		    std::string wavenumber_series<number>::make_non_LaTeX_tag(double config) const
			    {
		        std::ostringstream label;

				    label << std::setprecision(this->precision);

				    label << __CPP_TRANSPORT_NONLATEX_T_SYMBOL << "=" << config;

				    return(label.str());
			    }


		    template <typename number>
			  void wavenumber_series<number>::serialize(Json::Value& writer) const
			    {
				    // DON'T CALL derived_line<> serialization because of virtual inheritance;
				    // concrete classes must call it themselves

		        writer[__CPP_TRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_ROOT][__CPP_TRANSPORT_NODE_PRODUCT_WAVENUMBER_SERIES_SPECTRAL_INDEX] = this->compute_index;
			    }


		    template <typename number>
		    void wavenumber_series<number>::write(std::ostream& out)
			    {
			    }


		    template <typename number>
		    void wavenumber_series<number>::write_kconfig_list(std::ostream& out, const twopf_kconfig_database& twopf_db)
			    {
		        unsigned int count = 0;

		        unsigned int saved_left_margin = this->wrapper.get_left_margin();
		        this->wrapper.set_left_margin(2);
		        this->wrapper.wrap_newline(out);

		        for(twopf_kconfig_database::const_config_iterator t = twopf_db.config_begin(); t != twopf_db.config_end(); t++)
			        {
		            std::vector< unsigned int >::iterator s = std::find(this->kconfig_sample_sns.begin(), this->kconfig_sample_sns.end(), t->serial);

		            if(s != this->kconfig_sample_sns.end())
			            {
		                std::ostringstream line;
		                line << std::setprecision(4);

		                line << count << ". ";
				            line << "serial = " << t->serial << ", ";
		                line << "k = " << t->k_conventional;

		                this->wrapper.wrap_out(out, line.str());
		                this->wrapper.wrap_newline(out);

				            count++;
			            }
			        }

		        this->wrapper.set_left_margin(saved_left_margin);
			    }


		    template <typename number>
		    void wavenumber_series<number>::write_kconfig_list(std::ostream& out, const threepf_kconfig_database& threepf_db)
			    {
				    unsigned int count = 0;

				    unsigned int saved_left_margin = this->wrapper.get_left_margin();
				    this->wrapper.set_left_margin(2);
				    this->wrapper.wrap_newline(out);

				    for(threepf_kconfig_database::const_config_iterator t = threepf_db.config_begin(); t != threepf_db.config_end(); t++)
					    {
				        std::vector< unsigned int >::iterator s = std::find(this->kconfig_sample_sns.begin(), this->kconfig_sample_sns.end(), t->serial);

						    if(s != this->kconfig_sample_sns.end())
							    {
						        std::ostringstream line;
						        line << std::setprecision(4);

						        line << count << ". ";
								    line << "serial = " << t->serial << ", ";
						        line << "k_t = " << t->k_t_conventional << ", ";
						        line << "alpha = " << t->alpha << ", ";
						        line << "beta = " << t->beta << ", ";
						        line << "k1/k_t = " << t->k1_conventional/t->k_t_conventional << ", ";
						        line << "k2/k_t = " << t->k2_conventional/t->k_t_conventional << ", ";
						        line << "k3/k_t = " << t->k3_conventional/t->k_t_conventional;

						        this->wrapper.wrap_out(out, line.str());
						        this->wrapper.wrap_newline(out);

								    count++;
							    }
					    }

				    this->wrapper.set_left_margin(saved_left_margin);
			    }


	    }   // namespace derived_data

}   // namespace transport


#endif //__wavenumber_series_H_
