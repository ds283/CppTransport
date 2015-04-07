//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __zeta_reduced_bispectrum_line_H_
#define __zeta_reduced_bispectrum_line_H_


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

				//! general zeta threepf content producer, suitable for producing
				//! content usable in eg. a 2d plot or table.
				//! Note that we derive virtuall from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<> attributes,
				//! eg. wavenumber_series<> and zeta_threepf_line<>
				template <typename number>
		    class zeta_reduced_bispectrum_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    zeta_reduced_bispectrum_line(const zeta_threepf_task<number>& tk, filter::threepf_kconfig_filter& kfilter);

				    //! Deserialization constructor
				    zeta_reduced_bispectrum_line(Json::Value& reader);

				    virtual ~zeta_reduced_bispectrum_line() = default;


		        // MANAGE LABEL OPTIONS

          public:

		        //! get k_t label setting
		        bool get_use_kt_label() const { return(this->use_kt_label); }
		        //! set k_t label setting
		        void set_use_kt_label(bool g) { this->use_kt_label = g; }
		        //! get alpha label setting
		        bool get_use_alpha_label() const { return(this->use_alpha_label); }
		        //! set alpha label setting
		        void set_use_alpha_label(bool g) { this->use_alpha_label = g; }
		        //! get beta label setting
		        bool get_use_beta_label() const { return(this->use_beta_label); }
		        //! set beta label setting
		        void set_use_beta_label(bool g) { this->use_beta_label = g; }


		        // LABELLING SERVICES

          public:

		        //! make a LaTeX label for one of our lines
		        std::string make_LaTeX_label(void) const;

		        //! make a non-LaTeX label for one of our lines
		        std::string make_non_LaTeX_label(void) const;


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

		        //! use k_t on line labels?
		        bool use_kt_label;

		        //! use alpha on line labels?
		        bool use_alpha_label;

		        //! use beta on line labels?
		        bool use_beta_label;

			    };


		    // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
        template <typename number>
        zeta_reduced_bispectrum_line<number>::zeta_reduced_bispectrum_line(const zeta_threepf_task<number>& tk, filter::threepf_kconfig_filter& kfilter)
          : derived_line<number>(tk),
            use_kt_label(true),
            use_alpha_label(false),
            use_beta_label(false)
			    {
		        // set up a list of serial numbers corresponding to the sample kconfigs for this derived line
            try
              {
                this->f.filter_threepf_kconfig_sample(kfilter, tk.get_threepf_kconfig_list(), this->kconfig_sample_sns);
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
				zeta_reduced_bispectrum_line<number>::zeta_reduced_bispectrum_line(Json::Value& reader)
					: derived_line<number>(reader)
					{
				    use_kt_label    = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT].asBool();
				    use_alpha_label = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA].asBool();
				    use_beta_label  = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA].asBool();
					}


				template <typename number>
				std::string zeta_reduced_bispectrum_line<number>::make_LaTeX_label(void) const
					{
				    return( std::string(__CPP_TRANSPORT_LATEX_REDUCED_BISPECTRUM_SYMBOL) );
					}


				template <typename number>
				std::string zeta_reduced_bispectrum_line<number>::make_non_LaTeX_label(void) const
					{
						return( std::string(__CPP_TRANSPORT_NONLATEX_REDUCED_BISPECTRUM_SYMBOL) );
					}


				template <typename number>
				void zeta_reduced_bispectrum_line<number>::serialize(Json::Value& writer) const
					{
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT] = this->use_kt_label;
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA] = this->use_alpha_label;
				    writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA] = this->use_beta_label;
					}


				template <typename number>
				void zeta_reduced_bispectrum_line<number>::write(std::ostream& out)
					{
				    out << "  " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_REDUCED_BISPECTRUM << std::endl;
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__zeta_reduced_bispectrum_line_H_
