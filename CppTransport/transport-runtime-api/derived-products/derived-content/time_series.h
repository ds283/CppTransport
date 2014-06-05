//
// Created by David Seery on 19/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __time_series_H_
#define __time_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/threepf_time_shift.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"

#include "transport-runtime-api/utilities/latex_output.h"



namespace transport
	{

    // forward-declare model class
    template <typename number> class model;

    // forward-declare task classes.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;


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
		        time_series(const integration_task<number>& tk, filter::time_filter tfilter);

		        //! Deserialization constructor
		        time_series(serialization_reader* reader);

		        virtual ~time_series() = default;


		        // DATAPIPE SERVICES

		      public:

		        //! extract axis data, corresponding to our sample times, from datapipe
		        const std::vector<double>& pull_time_axis(typename data_manager<number>::datapipe& pipe) const;


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label tag for a twopf k-configuration
				    std::string make_LaTeX_tag(const typename data_manager<number>::twopf_configuration& config) const;

				    //! make a non-LaTeX label tag for a twopf k-configuration
				    std::string make_non_LaTeX_tag(const typename data_manager<number>::twopf_configuration& config) const;

		        //! make a LaTeX label tag for a threepf k-configuration
		        std::string make_LaTeX_tag(const typename data_manager<number>::threepf_configuration& config,
		                                   bool use_kt, bool use_alpha, bool use_beta) const;

				    //! make a non-LaTeX label tag for a threepf k-configuration
				    std::string make_non_LaTeX_tag(const typename data_manager<number>::threepf_configuration& config,
				                                   bool use_kt, bool use_alpha, bool use_beta) const;


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out);


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! Serialize this object
		        virtual void serialize(serialization_writer& writer) const override;


				    // INTERNAL DATA

		      protected:

				    threepf_time_shift<number> shifter;

			    };


		    template <typename number>
		    time_series<number>::time_series(const integration_task<number>& tk, filter::time_filter tfilter)
			    {
		        // set up a list of serial numbers corresponding to the sample times for this derived line
		        this->f.filter_time_sample(tfilter, tk.get_time_config_sample(), this->time_sample_sns);
			    }


				// Deserialization constructor DOESN'T CALL derived_line<> deserialization constructor
				// because of virtual inheritance; concrete classes must call it themselves
		    template <typename number>
		    time_series<number>::time_series(serialization_reader* reader)
			    {
		        assert(reader != nullptr);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
			    }


		    template <typename number>
		    const std::vector<double>& time_series<number>::pull_time_axis(typename data_manager<number>::datapipe& pipe) const
			    {
		        if(!pipe.validate()) throw runtime_exception(runtime_exception::DATAPIPE_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_DATAPIPE);

		        // set-up time sample data
				    typename data_manager<number>::datapipe::time_config_handle& handle = pipe.new_time_config_handle(this->time_sample_sns);
				    typename data_manager<number>::datapipe::time_config_tag tag = pipe.new_time_config_tag();

				    return handle.lookup_tag(tag);
			    }


				template <typename number>
				std::string time_series<number>::make_non_LaTeX_tag(const typename data_manager<number>::twopf_configuration& config) const
					{
				    std::ostringstream label;

				    label << std::setprecision(this->precision);

				    label << __CPP_TRANSPORT_NONLATEX_K_SYMBOL << "=";
				    if(this->get_klabel_meaning() == derived_line<number>::conventional) label << config.k_conventional;
				    else label << config.k_comoving;

				    return(label.str());
					}


				template <typename number>
				std::string time_series<number>::make_LaTeX_tag(const typename data_manager<number>::twopf_configuration& config) const
					{
				    std::ostringstream label;

				    label << __CPP_TRANSPORT_LATEX_K_SYMBOL << "=";
				    if(this->get_klabel_meaning() == derived_line<number>::conventional) label << output_latex_number(config.k_conventional, this->precision);
				    else label << output_latex_number(config.k_comoving, this->precision);

            return(label.str());
					}


		    template <typename number>
		    std::string time_series<number>::make_LaTeX_tag(const typename data_manager<number>::threepf_configuration& config,
		                                                    bool use_kt, bool use_alpha, bool use_beta) const
			    {
		        std::ostringstream label;

		        unsigned int count = 0;
		        if(use_kt)
			        {
		            label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_KT_SYMBOL << "=";
		            if(this->get_klabel_meaning() == derived_line<number>::conventional) label << output_latex_number(config.kt_conventional, this->precision);
		            else label << output_latex_number(config.kt_comoving, this->precision);
		            count++;
			        }
		        if(use_alpha)
			        {
		            label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_ALPHA_SYMBOL << "=" << output_latex_number(config.alpha, this->precision);
		            count++;
			        }
		        if(use_beta)
			        {
		            label << (count > 0 ? ",\\, " : "") << __CPP_TRANSPORT_LATEX_BETA_SYMBOL << "=" << output_latex_number(config.beta, this->precision);
		            count++;
			        }

		        return (label.str());
			    }


				template <typename number>
				std::string time_series<number>::make_non_LaTeX_tag(const typename data_manager<number>::threepf_configuration& config,
				                                                    bool use_kt, bool use_alpha, bool use_beta) const
					{
				    std::ostringstream label;

				    unsigned int count = 0;
				    if(use_kt)
					    {
				        label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_KT_SYMBOL << "=";
				        if(this->get_klabel_meaning() == derived_line<number>::conventional) label << config.kt_conventional;
				        else label << config.kt_comoving;
				        count++;
					    }
				    if(use_alpha)
					    {
				        label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_ALPHA_SYMBOL << "=" << config.alpha;
				        count++;
					    }
				    if(use_beta)
					    {
				        label << (count > 0 ? ", " : "") << __CPP_TRANSPORT_NONLATEX_BETA_SYMBOL << "=" << config.beta;
				        count++;
					    }

				    return (label.str());
					}


		    template <typename number>
		    void time_series<number>::serialize(serialization_writer& writer) const
			    {
				    // DON'T CALL derived_line<> serialization because of virtual inheritance;
				    // concrete classes must call it themselves
			    }


		    template <typename number>
		    void time_series<number>::write(std::ostream& out)
			    {
			    }


			}   // namespace derived_data

	}   // namespace transport


#endif //__time_series_H_
