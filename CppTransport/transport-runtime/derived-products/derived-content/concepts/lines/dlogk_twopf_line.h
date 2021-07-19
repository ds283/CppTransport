//
// Created by David Seery on 19/07/2021.
// --@@
// Copyright (c) 2021 University of Sussex. All rights reserved.
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


#ifndef CPPTRANSPORT_TWOPF_DLOGK_LINE_H
#define CPPTRANSPORT_TWOPF_DLOGK_LINE_H


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


namespace transport
	{

    namespace derived_data
	    {

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_DLOGK_TWOPF_LINE_ROOT = "dlogk-twopf-line-settings";

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_DLOGK_TWOPF_LINE_DIMENSIONLESS = "dimensionless";



        //! general field twopf content producer, suitable
        //! for producing content usable in eg. a 2d plot or table.
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and twopf_line<>
		    template <typename number>
        class dlogk_twopf_line: public virtual derived_line<number>
	        {

          public:

	          // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor
            dlogk_twopf_line(const twopf_db_task<number>& tk, index_selector<2> sel);

		        //! Deserialization constructor
            dlogk_twopf_line(Json::Value& reader, task_finder<number>& finder);

            //! Destructor is default
		        virtual ~dlogk_twopf_line() = default;


            // MANAGE SETTINGS

          public:

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

		        //! compute the dimensionless correlation function?
		        bool dimensionless;

	        };


		    template <typename number>
        dlogk_twopf_line<number>::dlogk_twopf_line(const twopf_db_task<number>& tk, index_selector<2> sel)
		      : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
		        gadget(tk),
		        active_indices(sel),
		        dimensionless(true)
			    {
            if(!tk.get_collect_spectral_data())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_TWOPF_DLOGK_LINE_NO_SPECTRAL_DATA << ": '" << tk.get_name() << "'";
                throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
              }

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
        dlogk_twopf_line<number>::dlogk_twopf_line(Json::Value& reader, task_finder<number>& finder)
		      : derived_line<number>(reader),  // not called because of virtual inheritance; here to silence Intel compiler warning
            gadget(derived_line<number>::parent_tasks), // safe, will always be constructed after derived_line<number>()
		        active_indices(reader)
			    {
            const auto& root = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_DLOGK_TWOPF_LINE_ROOT];

            dimensionless = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_DLOGK_TWOPF_LINE_DIMENSIONLESS].asBool();
			    }


		    template <typename number>
		    std::string dlogk_twopf_line<number>::make_LaTeX_label(unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

            label << CPPTRANSPORT_LATEX_DLOGK_SYMBOL << " ";

				    if(this->dimensionless) label << CPPTRANSPORT_LATEX_DIMENSIONLESS_TWOPF_SYMBOL << "(";

		        label << CPPTRANSPORT_LATEX_RE_SYMBOL << " ";

		        const auto& field_names = this->gadget.get_model()->get_f_latex_names();

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

				    if(this->dimensionless) label << ")";

		        return (label.str());
			    }


		    template <typename number>
		    std::string dlogk_twopf_line<number>::make_non_LaTeX_label(unsigned int m, unsigned int n) const
			    {
		        std::ostringstream label;

		        unsigned int N_fields = this->gadget.get_N_fields();

            label << CPPTRANSPORT_NONLATEX_DLOGK_SYMBOL << " ";

				    if(this->dimensionless) label << CPPTRANSPORT_NONLATEX_DIMENSIONLESS_TWOPF_SYMBOL << "[";

            label << CPPTRANSPORT_NONLATEX_RE_SYMBOL << " ";

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
		    void dlogk_twopf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);

            auto& root = writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_DLOGK_TWOPF_LINE_ROOT];

            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_DLOGK_TWOPF_LINE_DIMENSIONLESS] = this->dimensionless;
			    }


		    template <typename number>
		    void dlogk_twopf_line<number>::write(std::ostream& out)
			    {
		        out << "  " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TWOPF << '\n';
		        out << "  " << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, this->gadget.get_model()->get_state_names());
		        out << '\n';
			    }


	    }   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_TWOPF_DLOGK_LINE_H
