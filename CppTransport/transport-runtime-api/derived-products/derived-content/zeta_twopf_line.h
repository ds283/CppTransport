//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_twopf_line_H_
#define __zeta_twopf_line_H_


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


namespace transport
	{

		namespace derived_data
			{

				//! general zeta twopf content producer, suitable
				//! for producing content usable in eg. a 2d plot or table.
				//! Note that we derive virtually from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<> attributes,
				//! eg. wavenumber_series<> and zeta_twopf_line<>
				template <typename number>
		    class zeta_twopf_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

          public:

				    //! Basic user-facing constructor
				    zeta_twopf_line(const zeta_twopf_list_task<number>& tk, filter::twopf_kconfig_filter& kfilter);

				    //! Deserialization constructor
				    zeta_twopf_line(Json::Value& reader);

				    virtual ~zeta_twopf_line() = default;


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label
				    std::string make_LaTeX_label(void) const;

				    //! make a non-LaTeX label
				    std::string make_non_LaTeX_label(void) const;


		        // K-CONFIGURATION SERVICES

		      public:

		        //! lookup wavenumber axis data
		        void pull_wavenumber_axis(datapipe<number>& pipe, std::vector<double>& axis) const;


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    virtual void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    //! Serialize this object
				    virtual void serialize(Json::Value& writer) const override;

			    };


		    // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
				template <typename number>
				zeta_twopf_line<number>::zeta_twopf_line(const zeta_twopf_list_task<number>& tk, filter::twopf_kconfig_filter& kfilter)
		      : derived_line<number>(tk)
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


				// Deserialization constructor DOESN'T CALL the correct derived_line<> deserialization constructor
				// because of virtual inheritance; concrete classes must call it themselves
				template <typename number>
				zeta_twopf_line<number>::zeta_twopf_line(Json::Value& reader)
					: derived_line<number>(reader)
					{
					}


				template <typename number>
				std::string zeta_twopf_line<number>::make_LaTeX_label(void) const
					{
				    return( std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) );
					}


				template <typename number>
				std::string zeta_twopf_line<number>::make_non_LaTeX_label(void) const
					{
				    return( std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL) );
					}


		    template <typename number>
		    void zeta_twopf_line<number>::pull_wavenumber_axis(datapipe<number>& pipe, std::vector<double>& axis) const
			    {
		        typename datapipe<number>::twopf_kconfig_handle& handle = pipe.new_twopf_kconfig_handle(this->kconfig_sample_sns);
		        twopf_kconfig_tag<number> tag = pipe.new_twopf_kconfig_tag();

            // safe to take a reference here and avoid a copy
		        const std::vector< twopf_configuration >& configs = handle.lookup_tag(tag);

		        axis.clear();
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
			    }


				template <typename number>
				void zeta_twopf_line<number>::serialize(Json::Value& writer) const
					{
					}


				template <typename number>
				void zeta_twopf_line<number>::write(std::ostream& out)
					{
				    out << "  " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_TWOPF << std::endl;
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__zeta_twopf_line_H_
