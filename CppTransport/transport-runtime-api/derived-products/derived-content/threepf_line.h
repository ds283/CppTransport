//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
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
// need repository in order to get the details of a repository<number>::output_group
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
		        threepf_line(const threepf_task<number>& tk, model<number>* m, index_selector<3>& sel, filter::threepf_kconfig_filter& kfilter);

		        //! Deserialization constructor
		        threepf_line(serialization_reader* reader);

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

            //! handle cross-delegation from wavenumber_series class to lookup wavenumber axis data
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

		        //! record which indices are active in this group
		        index_selector<3> active_indices;

            //! use k_t on line labels?
            bool use_kt_label;

            //! use alpha on line labels?
            bool use_alpha_label;

            //! use beta on line labels?
            bool use_beta_label;

	        };


        template <typename number>
        threepf_line<number>::threepf_line(const threepf_task<number>& tk, model<number>* m,
                                           index_selector<3>& sel, filter::threepf_kconfig_filter& kfilter)
	        : active_indices(sel), use_kt_label(true), use_alpha_label(false), use_beta_label(false)
	        {
            assert(m != nullptr);
            if(m == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_DERIVED_LINE_NULL_MODEL);

            if(active_indices.get_number_fields() != m->get_N_fields())
	            {
                std::ostringstream msg;
                msg << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
	                << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
	                << __CPP_TRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << m->get_N_fields() << ")";
                throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
	            }

            // set up a list of serial numbers corresponding to the sample kconfigs for this derived line
            this->f.filter_threepf_kconfig_sample(kfilter, tk.get_sample(), this->kconfig_sample_sns);
	        }


		    // Deserialization constructor DOESN'T CALL derived_line<> deserialization constructor
		    // because of virtual inheritance; concrete classes must call it themselves
		    template <typename number>
		    threepf_line<number>::threepf_line(serialization_reader* reader)
			    : active_indices(reader)
			    {
				    assert(reader != nullptr);
		        if(reader == nullptr) throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_NULL_READER);

		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT, this->use_kt_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA, this->use_alpha_label);
		        reader->read_value(__CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA, this->use_beta_label);
			    }


		    template <typename number>
		    std::string threepf_line<number>::make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        const std::vector<std::string>& field_names = this->mdl->get_f_latex_names();

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

		        unsigned int N_fields = this->mdl->get_N_fields();

		        label << std::setprecision(this->precision);

		        const std::vector<std::string>& field_names = this->mdl->get_field_names();

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
        void threepf_line<number>::pull_wavenumber_axis(typename data_manager<number>::datapipe& pipe, std::vector<double>& axis) const
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
		    void threepf_line<number>::serialize(serialization_writer& writer) const
			    {
				    this->active_indices.serialize(writer);

		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT, this->use_kt_label);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA, this->use_alpha_label);
		        this->write_value_node(writer, __CPP_TRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA, this->use_beta_label);
			    }


		    template <typename number>
		    void threepf_line<number>::write(std::ostream& out)
			    {
		        out << "  " << __CPP_TRANSPORT_PRODUCT_TIME_SERIES_LABEL_THREEPF << std::endl;
		        out << "  " << __CPP_TRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->mdl->get_state_names());
		        out << std::endl;
			    }

	    }   // namespace derived_data

	}   // namespace transport


#endif //__threepf_line_H_
