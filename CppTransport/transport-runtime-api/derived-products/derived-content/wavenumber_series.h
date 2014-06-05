//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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
// need repository in order to get the details of a repository<number>::output_group
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/threepf_kconfig_shift.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"

#include "transport-runtime-api/utilities/latex_output.h"


namespace transport
	{

		// forward-declare model class
		template <typename number> class model;

		// forward-declare task classes
		// task.h includes this header, so we cannot include task.h without
		// creating a circular dependency
		template <typename number> class task;
		template <typename number> class integration_task;


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
		        wavenumber_series(const integration_task<number>& tk, filter::time_filter tfilter);

		        //! Deserialization constructor
		        wavenumber_series(serialization_reader* reader);

		        virtual ~wavenumber_series() = default;


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
		        virtual void serialize(serialization_writer& writer) const override;


            // INTERNAL DATA

          protected:

            threepf_kconfig_shift<number> shifter;

	        };


		    template <typename number>
		    wavenumber_series<number>::wavenumber_series(const integration_task<number>& tk, filter::time_filter tfilter)
			    {
				    // set up list of serial numbers corresponding to the sample times for this derived line
				    this->f.filter_time_sample(tfilter, tk.get_time_config_sample(), this->time_sample_sns);
			    }


		    template <typename number>
		    wavenumber_series<number>::wavenumber_series(serialization_reader* reader)
			    {
				    assert(reader != nullptr);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);
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
			  void wavenumber_series<number>::serialize(serialization_writer& writer) const
			    {
				    // DON'T CALL derived_line<> serialization because of virtual inheritance;
				    // concrete classes must call it themselves
			    }


		    template <typename number>
		    void wavenumber_series<number>::write(std::ostream& out)
			    {
			    }


	    }   // namespace derived_data

}   // namespace transport


#endif //__wavenumber_series_H_
