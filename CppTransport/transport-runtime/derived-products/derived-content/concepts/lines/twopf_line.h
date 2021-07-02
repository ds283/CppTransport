//
// Created by David Seery on 02/06/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_TWOPF_LINE_H
#define CPPTRANSPORT_TWOPF_LINE_H


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
#include "transport-runtime/derived-products/derived-content/utilities/twopf_db_task_gadget.h"


#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_ROOT          "twopf-line-settings"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE          "components"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_REAL          "real"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_IMAGINARY     "imaginary"
#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_DIMENSIONLESS "dimensionless"



namespace transport
	{

    namespace derived_data
	    {

        //! general field twopf content producer, suitable
        //! for producing content usable in eg. a 2d plot or table.
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and twopf_line<>
		    template <typename number>
        class twopf_line: public virtual derived_line<number>
	        {

          public:

	          // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor
		        twopf_line(const twopf_db_task<number>& tk, index_selector<2> sel);

		        //! Deserialization constructor
		        twopf_line(Json::Value& reader, task_finder<number>& finder);

		        virtual ~twopf_line() = default;


            // MANAGE SETTINGS

          public:

            //! get twopf type setting
            twopf_type get_type() const { return(this->twopf_meaning); }

            //! set twopf type setting
            twopf_line<number>& set_type(twopf_type m) { this->twopf_meaning = m; return *this; }

            //! query type of twopf - is it real?
            bool is_real_twopf() const { return(this->twopf_meaning == twopf_type::real); }

            //! query type of twopf - is it imaginary?
            bool is_imag_twopf() const { return(this->twopf_meaning == twopf_type::imaginary); }

            //! is this dimensionless?
            bool is_dimensionless() const { return(this->dimensionless); }

            //! set dimensionless
            twopf_line<number>& set_dimensionless(bool g) { this->dimensionless = g; return *this; }


		        // LABELLING SERVICES

          public:

		        //! make a LaTeX label for one of our lines
		        std::string make_LaTeX_label(unsigned int m, unsigned int n) const;

		        //! make a non-LaTeX label for one of our lines
		        std::string make_non_LaTeX_label(unsigned int m, unsigned int n) const;


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
            twopf_db_task_gadget<number> gadget;

            //! record which indices are active in this group
            index_selector<2> active_indices;

            //! record which type of 2pf we are plotting
            twopf_type twopf_meaning;

		        //! compute the dimensionless twopf?
		        bool dimensionless;

	        };


		    template <typename number>
		    twopf_line<number>::twopf_line(const twopf_db_task<number>& tk, index_selector<2> sel)
		      : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
		        gadget(tk),
		        active_indices(sel),
		        twopf_meaning(twopf_type::real),
		        dimensionless(true)
			    {
		        if(active_indices.get_number_fields() != gadget.get_N_fields())
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
			              << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
			              << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B << " " << gadget.get_N_fields() << ")";
		            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
			        }
			    }


		    template <typename number>
		    twopf_line<number>::twopf_line(Json::Value& reader, task_finder<number>& finder)
		      : derived_line<number>(reader),  // not called because of virtual inheritance; here to silence Intel compiler warning
            gadget(derived_line<number>::parent_tasks), // safe, will always be constructed after derived_line<number>()
		        active_indices(reader)
			    {
				    dimensionless = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_DIMENSIONLESS].asBool();

		        std::string tpf_type = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE].asString();

		        if(tpf_type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_REAL) twopf_meaning = twopf_type::real;
		        else if(tpf_type == CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_IMAGINARY) twopf_meaning = twopf_type::imaginary;
		        else
			        {
		            std::ostringstream msg;
		            msg << CPPTRANSPORT_PRODUCT_DERIVED_LINE_TWOPF_TYPE_UNKNOWN << " '" << tpf_type << "'";
		            throw runtime_exception(exception_type::SERIALIZATION_ERROR, msg.str());
			        }
			    }


		    template <typename number>
		    std::string twopf_line<number>::make_LaTeX_label(unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

				    if(this->dimensionless) label << CPPTRANSPORT_LATEX_DIMENSIONLESS_TWOPF_SYMBOL << "_{";

		        label << (this->twopf_meaning == twopf_type::real ? CPPTRANSPORT_LATEX_RE_SYMBOL : CPPTRANSPORT_LATEX_IM_SYMBOL) << " ";

		        const std::vector<std::string>& field_names = this->gadget.get_model()->get_f_latex_names();

            switch(this->get_dot_meaning())
              {
                case dot_type::derivatives:
                  label
                    << field_names[m % N_fields] << (m >= N_fields ? CPPTRANSPORT_LATEX_PRIME_SYMBOL_SUPERSCRIPT : "") << " "
                    << field_names[n % N_fields] << (n >= N_fields ? CPPTRANSPORT_LATEX_PRIME_SYMBOL_SUPERSCRIPT : "");
                  break;

                case dot_type::momenta:
                  label
                    << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
                    << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
                  break;
              }

				    if(this->dimensionless) label << "}";

		        return (label.str());
			    }


		    template <typename number>
		    std::string twopf_line<number>::make_non_LaTeX_label(unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

				    if(this->dimensionless) label << CPPTRANSPORT_NONLATEX_DIMENSIONLESS_TWOPF_SYMBOL << "[";

		        label << (this->twopf_meaning == twopf_type::real ? CPPTRANSPORT_NONLATEX_RE_SYMBOL : CPPTRANSPORT_NONLATEX_IM_SYMBOL) << " ";

		        const std::vector<std::string>& field_names = this->gadget.get_model()->get_field_names();

            switch(this->get_dot_meaning())
              {
                case dot_type::derivatives:
                  label
                    << field_names[m % N_fields] << (m >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "") << ", "
                    << field_names[n % N_fields] << (n >= N_fields ? CPPTRANSPORT_NONLATEX_PRIME_SYMBOL : "");
                  break;

                case dot_type::momenta:
                  label
                    << (m >= N_fields ? "p_{" : "") << field_names[m % N_fields] << (m >= N_fields ? "}" : "") << " "
                    << (n >= N_fields ? "p_{" : "") << field_names[n % N_fields] << (n >= N_fields ? "}" : "");
                  break;
              }

				    if(this->dimensionless) label << "]";

				    return(label.str());
			    }


		    template <typename number>
		    void twopf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);

		        switch(this->twopf_meaning)
			        {
                case twopf_type::real:
		                writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_REAL);
		                break;

                case twopf_type::imaginary:
		                writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_TYPE] = std::string(CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_IMAGINARY);
		                break;
              }

				    writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TWOPF_LINE_DIMENSIONLESS] = this->dimensionless;
			    }


		    template <typename number>
		    void twopf_line<number>::write(std::ostream& out)
			    {
		        out << "  " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TWOPF << '\n';
		        out << "  " << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->gadget.get_model()->get_state_names());
		        out << '\n';
			    }


	    }   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_TWOPF_LINE_H
