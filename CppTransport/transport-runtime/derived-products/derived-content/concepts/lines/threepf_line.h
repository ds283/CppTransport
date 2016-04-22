//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_THREEPF_LINE_H
#define CPPTRANSPORT_THREEPF_LINE_H


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime/data/datapipe/datapipe_decl.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/utilities/integration_task_gadget.h"


#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT          "threepf-line-settings"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_DIMENSIONLESS "dimensionless"


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
		        threepf_line(const threepf_task<number>& tk, index_selector<3>& sel);

		        //! Deserialization constructor
		        threepf_line(Json::Value& reader, task_finder<number>& finder);

		        virtual ~threepf_line() = default;


            // MANAGE SETTING

            //! is this dimensionles?
            bool is_dimensionless() const { return(this->dimensionless); }

            //! set dimensionless
            void set_dimensionless(bool g) { this->dimensionless = g; }


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

		        //! make a LaTeX label for one of our lines
		        std::string make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const;

		        //! make a non-LaTeX label for one of our lines
		        std::string make_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const;


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

            //! compute the dimensionless threepf (ie. (k1 k2 k3)^2 * threepf)
            bool dimensionless;

	        };


        template <typename number>
        threepf_line<number>::threepf_line(const threepf_task<number>& tk, index_selector<3>& sel)
	        : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
	          gadget(tk),
	          active_indices(sel),
	          use_kt_label(true),
	          use_alpha_label(false),
	          use_beta_label(false)
	        {
            if(active_indices.get_number_fields() != this->gadget.get_N_fields())
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
	                << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
	                << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << this->gadget.get_N_fields() << ")";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	            }
	        }


		    template <typename number>
		    threepf_line<number>::threepf_line(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader),  // not called because of virtual inheritance; here to silence Intel compiler warning
			      gadget(),
			      active_indices(reader)
			    {
				    assert(this->parent_task != nullptr);
		        gadget.set_task(this->parent_task, finder);

		        use_kt_label    = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT].asBool();
		        use_alpha_label = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA].asBool();
		        use_beta_label  = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA].asBool();

            dimensionless   = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_DIMENSIONLESS].asBool();
			    }


		    template <typename number>
		    std::string threepf_line<number>::make_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

		        const std::vector<std::string>& field_names = this->gadget.get_model()->get_f_latex_names();

            if(this->dimensionless) label << CPPTRANSPORT_LATEX_SHAPE_FACTOR << " ";

            switch(this->get_dot_meaning())
              {
                case dot_type::derivatives:
                  label
                    << field_names[l % N_fields] << (l >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
                    << field_names[m % N_fields] << (m >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "") << " "
                    << field_names[n % N_fields] << (n >= N_fields ? "^{" CPPTRANSPORT_LATEX_PRIME_SYMBOL "}" : "");
                  break;

                case dot_type::momenta:
                  label
                    << (l >= N_fields ? "p_{" : "") << field_names[l % N_fields] << (l >= N_fields ? "}" : "") << " "
                    << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
                    << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
                  break;
              }

				    return(label.str());
			    }


		    template <typename number>
		    std::string threepf_line<number>::make_non_LaTeX_label(unsigned int l, unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

            if(this->dimensionless) label << CPPTRANSPORT_NONLATEX_SHAPE_FACTOR << " ";

		        const std::vector<std::string>& field_names = this->gadget.get_model()->get_field_names();

            switch(this->get_dot_meaning())
              {
                case dot_type::derivatives:
                  label
                    << field_names[l % N_fields] << (l >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
                    << field_names[m % N_fields] << (m >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
                    << field_names[n % N_fields] << (n >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "");
                  break;

                case dot_type::momenta:
                  label
                    << (l >= N_fields ? "p_{" : "") << field_names[l % N_fields] << (l >= N_fields ? "}" : "") << ", "
                    << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << ", "
                    << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
                  break;
              }

				    return(label.str());
			    }


        template <typename number>
		    void threepf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);

            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT]      = this->use_kt_label;
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA]   = this->use_alpha_label;
            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA]    = this->use_beta_label;

            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_THREEPF_LINE_DIMENSIONLESS] = this->dimensionless;
			    }


		    template <typename number>
		    void threepf_line<number>::write(std::ostream& out)
			    {
		        out << "  " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_THREEPF << '\n';
		        out << "  " << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->gadget.get_model()->get_state_names());
		        out << '\n';
			    }

	    }   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_THREEPF_LINE_H
