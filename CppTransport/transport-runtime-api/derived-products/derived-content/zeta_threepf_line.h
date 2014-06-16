//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __zeta_threepf_line_H_
#define __zeta_threepf_line_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group_record
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"


namespace transport
	{

    // forward-declare model class
    template <typename number> class model;

    // forward-declare task classes.
    // task.h includes this header, so we cannot include task.h otherwise we create
    // a circular dependency.
    template <typename number> class task;
    template <typename number> class integration_task;
    template <typename number> class threepf_task;

		namespace derived_data
			{

				//! general zeta threepf content producer, suitable for producing
				//! content usable in eg. a 2d plot or table.
				//! Note that we derive virtuall from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<> attributes,
				//! eg. wavenumber_series<> and zeta_threepf_line<>
				template <typename number>
		    class zeta_threepf_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    zeta_threepf_line(const threepf_task<number>& tk, model<number>* m, filter::threepf_kconfig_filter& kfilter);

				    //! Deserialization constructor
				    zeta_threepf_line(serialization_reader* reader);

				    virtual ~zeta_threepf_line() = default;


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


		        // K-CONFIGURATION SERVICES

		      public:

		        //! lookup wavenumber axis data
		        void pull_wavenumber_axis(typename data_manager<number>::datapipe& pipe, std::vector<double>& axis) const;


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        virtual void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! Serialize this object
		        virtual void serialize(serialization_writer& writer) const override;


		        // INTERNAL DATA

		      protected:

		        //! use k_t on line labels?
		        bool use_kt_label;

		        //! use alpha on line labels?
		        bool use_alpha_label;

		        //! use beta on line labels?
		        bool use_beta_label;

			    };


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>.
        // Concrete classes must call it themselves
		    template <typename number>
		    zeta_threepf_line<number>::zeta_threepf_line(const threepf_task<number>& tk, model<number>* m, filter::threepf_kconfig_filter& kfilter)
		      : use_kt_label(true), use_alpha_label(false), use_beta_label(false)
			    {
		        assert(m != nullptr);
		        if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_MODEL);

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


        // note that because time_series<> inherits virtually from derived_line<>, the constructor for
        // derived_line<> is *not* called from time_series<>.
        // Concrete classes must call it themselves
				template <typename number>
				zeta_threepf_line<number>::zeta_threepf_line(serialization_reader* reader)
					{
				    assert(reader != nullptr);
				    if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_NULL_READER);

				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT, this->use_kt_label);
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA, this->use_alpha_label);
				    reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA, this->use_beta_label);
					}


				template <typename number>
				std::string zeta_threepf_line<number>::make_LaTeX_label(void) const
					{
				    return( std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_LATEX_ZETA_SYMBOL) );
					}


				template <typename number>
				std::string zeta_threepf_line<number>::make_non_LaTeX_label(void) const
					{
						return( std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL) + std::string(" ") + std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL + std::string(" ") + std::string(__CPP_TRANSPORT_NONLATEX_ZETA_SYMBOL)) );
					}


		    template <typename number>
		    void zeta_threepf_line<number>::pull_wavenumber_axis(typename data_manager<number>::datapipe& pipe, std::vector<double>& axis) const
			    {
		        typename data_manager<number>::datapipe::threepf_kconfig_handle& handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
		        typename data_manager<number>::datapipe::threepf_kconfig_tag tag = pipe.new_threepf_kconfig_tag();

		        const std::vector< typename data_manager<number>::threepf_configuration >& configs = handle.lookup_tag(tag);

		        axis.clear();
		        for(typename std::vector< typename data_manager<number>::threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
			        {
		            if(this->klabel_meaning == derived_line<number>::comoving) axis.push_back((*t).kt_comoving);
		            else if(this->klabel_meaning == derived_line<number>::conventional) axis.push_back((*t).kt_conventional);
		            else
			            {
		                assert(false);
		                throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_KLABEL_TYPE_UNKNOWN);
			            }
			        }
			    }


				template <typename number>
				void zeta_threepf_line<number>::serialize(serialization_writer& writer) const
					{
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT, this->use_kt_label);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA, this->use_alpha_label);
				    this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA, this->use_beta_label);
					}


				template <typename number>
				void zeta_threepf_line<number>::write(std::ostream& out)
					{
				    out << "  " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_THREEPF << std::endl;
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__zeta_threepf_line_H_
