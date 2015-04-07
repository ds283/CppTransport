//
// Created by David Seery on 06/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __r_line_H_
#define __r_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/integration_task_gadget.h"


namespace transport
	{

		namespace derived_data
			{

				//! general tensor-to-scalar ratio content producer, suitable
				//! for producing content usable in eg. a 2d plot or table
				//! Note that we derive virtually from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<> attributes,
				//! eg. wavenumber_series<> and r_line<>
				template <typename number>
		    class r_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    r_line(const zeta_twopf_list_task<number>& tk, filter::twopf_kconfig_filter& kfilter);

				    //! Deserialization constructor
				    r_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

				    virtual ~r_line() = default;


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label
				    std::string make_LaTeX_label() const;

				    //! make a non-LaTeX label
				    std::string make_non_LaTeX_label() const;


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    //! Serialize this object
				    virtual void serialize(Json::Value& writer) const override;


				    // INTERNAL DATA

		      protected:

				    //! integration task gadget
				    integration_task_gadget<number> gadget;

			    };


				// constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
				template <typename number>
				r_line<number>::r_line(const zeta_twopf_list_task<number>& tk, filter::twopf_kconfig_filter& kfilter)
					: derived_line<number>(tk),
						gadget(dynamic_cast< integration_task<number>& >(*(tk.get_parent_task())))
					{
						// set up a list of serial numbers corresponding to the k-configurations for this derived line
				    try
					    {
				        this->f.filter_twopf_kconfig_sample(kfilter, tk.get_twopf_kconfig_list(), this->kconfig_sample_sns);
					    }
				    catch(runtime_exception& xe)
					    {
						    if(xe.get_exception_code() == runtime_exception::FILTER_EMPTY)
							    {
						        std::ostringstream msg;
						        msg << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_EMPTY_FILTER << " '" << this->get_parent_task()->get_name() << "'";
						        throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
							    }
						    else throw xe;
					    }
					}


				// deserialization constructor DOESN'T CALL the correct derived_line<> deserialization constructor
				// because of virtual inheritance; concrete classes must call it for themselves
				template <typename number>
				r_line<number>::r_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
					: derived_line<number>(reader),
						gadget()
					{
						assert(this->parent_task != nullptr);

						postintegration_task<number>* ptk = dynamic_cast< postintegration_task<number>* >(this->parent_task);
						assert(ptk != nullptr);

				    gadget.set_task(ptk->get_parent_task(), finder);
					}


				template <typename number>
				std::string r_line<number>::make_LaTeX_label() const
					{
				    std::ostringstream label;
						label << std::setprecision(this->precision);

						label << __CPP_TRANSPORT_LATEX_R_SYMBOL;

						return(label.str());
					}


		    template <typename number>
		    std::string r_line<number>::make_non_LaTeX_label() const
			    {
		        std::ostringstream label;
		        label << std::setprecision(this->precision);

		        label << __CPP_TRANSPORT_NONLATEX_R_SYMBOL;

		        return(label.str());
			    }


				template <typename number>
				void r_line<number>::serialize(Json::Value& writer) const
					{
					}


				template <typename number>
				void r_line<number>::write(std::ostream& out)
					{
				    out << " " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_R << std::endl;
						out << std::endl;
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__r_line_H_
