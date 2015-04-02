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

            threepf_kconfig_shift<number> shifter;

	        };


        // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
		    template <typename number>
		    wavenumber_series<number>::wavenumber_series(const derivable_task<number>& tk, filter::time_filter tfilter)
          : derived_line<number>(tk)
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
			    }


		    template <typename number>
		    void wavenumber_series<number>::write(std::ostream& out)
			    {
			    }


	    }   // namespace derived_data

}   // namespace transport


#endif //__wavenumber_series_H_
