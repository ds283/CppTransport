//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __threepf_line_H_
#define __threepf_line_H_


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

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/integration_task_gadget.h"


namespace transport
	{

    namespace derived_data
	    {

				//! general field threepf content producer, suitable for producing
		    //! content usable in eg. a 2d plot or table.
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attribiutes,
		    //! eg. wavenumber_series<> and threepf_line<>
		    template <typename number>
        class threepf_line: public virtual derived_line<number>
	        {

	          // CONSTRUCTOR, DESTRUCTOR

          public:

		        //! Basic user-facing constructor
		        threepf_line(const threepf_task<number>& tk, index_selector<3>& sel, filter::threepf_kconfig_filter& kfilter);

		        //! Deserialization constructor
		        threepf_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

		        virtual ~threepf_line() = default;


            // MANAGE LABEL OPTIONS

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

		        //! make a LaTeX label for one of our lines
		        std::string make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const;

		        //! make a non-LaTeX label for one of our lines
		        std::string make_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const;


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


		        // INTERNAL DATA

          protected:

		        //! integration task gadget
            integration_task_gadget<number> gadget;

		        //! record which indices are active in this group
		        index_selector<3> active_indices;

            //! use k_t on line labels?
            bool use_kt_label;

            //! use alpha on line labels?
            bool use_alpha_label;

            //! use beta on line labels?
            bool use_beta_label;

	        };


        // constructor DOESN'T CALL the correct derived_line<> constructor; concrete classes must call it for themselves
        template <typename number>
        threepf_line<number>::threepf_line(const threepf_task<number>& tk, index_selector<3>& sel, filter::threepf_kconfig_filter& kfilter)
	        : derived_line<number>(tk),
	          gadget(tk),
	          active_indices(sel),
	          use_kt_label(true),
	          use_alpha_label(false),
	          use_beta_label(false)
	        {
            if(active_indices.get_number_fields() != this->gadget.get_N_fields())
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
	                << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
	                << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << this->gadget.get_N_fields() << ")";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }

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
		    threepf_line<number>::threepf_line(Json::Value& reader, typename repository_finder<number>::task_finder& finder)
			    : derived_line<number>(reader),
			      gadget(),
			      active_indices(reader)
			    {
				    assert(this->parent_task != nullptr);
		        gadget.set_task(this->parent_task, finder);

		        use_kt_label    = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT].asBool();
		        use_alpha_label = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA].asBool();
		        use_beta_label  = reader[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA].asBool();
			    }


		    template <typename number>
		    std::string threepf_line<number>::make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

		        label << std::setprecision(this->precision);

		        const std::vector<std::string>& field_names = this->gadget.get_model()->get_f_latex_names();

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[l % N_fields] << (l >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
			                << field_names[m % N_fields] << (m >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
			                << field_names[n % N_fields] << (n >= N_fields ? "^{" __CPP_TRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
			        }
		        else
			        {
		            label << (l >= N_fields ? "p_{" : "") << field_names[l % N_fields] << (l >= N_fields ? "}" : "") << " "
			                << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
			                << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

				    return(label.str());
			    }


		    template <typename number>
		    std::string threepf_line<number>::make_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

		        label << std::setprecision(this->precision);

		        const std::vector<std::string>& field_names = this->gadget.get_model()->get_field_names();

		        if(this->get_dot_meaning() == derived_line<number>::derivatives)
			        {
		            label << field_names[l % N_fields] << (l >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
			                << field_names[m % N_fields] << (m >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
			                << field_names[n % N_fields] << (n >= N_fields ? __CPP_TRANSPORT_NONLATEX_PRIME_SYMBOL : "");
			        }
		        else
			        {
		            label << (l >= N_fields ? "p_{" : "") << field_names[l % N_fields] << (l >= N_fields ? "}" : "") << ", "
			                << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << ", "
			                << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
			        }

				    return(label.str());
			    }


        template <typename number>
        void threepf_line<number>::pull_wavenumber_axis(datapipe<number>& pipe, std::vector<double>& axis) const
	        {
            typename datapipe<number>::threepf_kconfig_handle& handle = pipe.new_threepf_kconfig_handle(this->kconfig_sample_sns);
            threepf_kconfig_tag<number> tag = pipe.new_threepf_kconfig_tag();

            // safe to take a reference here
            const std::vector< threepf_configuration >& configs = handle.lookup_tag(tag);

            axis.clear();
            for(typename std::vector< threepf_configuration >::const_iterator t = configs.begin(); t != configs.end(); t++)
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
		    void threepf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);

		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT] = this->use_kt_label;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA] = this->use_alpha_label;
		        writer[__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA] = this->use_beta_label;
			    }


		    template <typename number>
		    void threepf_line<number>::write(std::ostream& out)
			    {
		        out << "  " << __CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_THREEPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->gadget.get_model()->get_state_names());
		        out << std::endl;
			    }

	    }   // namespace derived_data

	}   // namespace transport


#endif //__threepf_line_H_
