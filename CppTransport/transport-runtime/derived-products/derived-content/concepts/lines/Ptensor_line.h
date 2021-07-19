//
// Created by David Seery on 15/12/14.
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


#ifndef CPPTRANSPORT_PTENSOR_LINE_H
#define CPPTRANSPORT_PTENSOR_LINE_H


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

#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime/derived-products/derived-content/utilities/twopf_db_task_gadget.h"


namespace transport
	{

    namespace derived_data
	    {

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_PTENSOR_LINE_ROOT          = "Ptensor-line-settings";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_DERIVED_PTENSOR_LINE_DIMENSIONLESS = "dimensionless";


		    //! general tensor power spectrum content producer, suitable
		    //! for producing content usable in eg. a 2d plot or table
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and Ptensor_line<>
		    template <typename number>
		    class Ptensor_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    Ptensor_line(const twopf_db_task<number>& tk);

				    //! Deserialization constructor
				    Ptensor_line(Json::Value& reader, task_finder<number>& finder);

				    //! destructor is default
				    virtual ~Ptensor_line() = default;


            // MANAGE SETTINGS

          public:

            //! is this dimensionles?
            bool is_dimensionless() const { return(this->dimensionless); }

            //! set dimensionless
            Ptensor_line<number>& set_dimensionless(bool g) { this->dimensionless = g; return *this; }


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
		        twopf_db_task_gadget<number> gadget;

            //! compute the dimensionless twopf?
            bool dimensionless;

			    };


		    template <typename number>
		    Ptensor_line<number>::Ptensor_line(const twopf_db_task<number>& tk)
		      : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
		        gadget(tk),
            dimensionless(true)
			    {
			    }


		    template <typename number>
		    Ptensor_line<number>::Ptensor_line(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader),  // not called because of virtual inheritance; here to silence Intel compiler warning
            gadget(derived_line<number>::parent_tasks) // safe, will always be constructed after derived_line<number>()
			    {
            const auto& root = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_PTENSOR_LINE_ROOT];

            dimensionless = root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_PTENSOR_LINE_DIMENSIONLESS].asBool();
			    }


		    template <typename number>
		    std::string Ptensor_line<number>::make_LaTeX_label() const
			    {
		        std::ostringstream label;
		        label << std::setprecision(this->precision);

            label << CPPTRANSPORT_LATEX_PTENSOR_SYMBOL;

				    return(label.str());
			    }


        template <typename number>
        std::string Ptensor_line<number>::make_non_LaTeX_label() const
	        {
            std::ostringstream label;
            label << std::setprecision(this->precision);

            label << CPPTRANSPORT_NONLATEX_PTENSOR_SYMBOL;

            return(label.str());
	        }


		    template <typename number>
		    void Ptensor_line<number>::serialize(Json::Value& writer) const
			    {
            auto& root = writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_PTENSOR_LINE_ROOT];
            root[CPPTRANSPORT_NODE_PRODUCT_DERIVED_PTENSOR_LINE_DIMENSIONLESS] = this->dimensionless;
			    }


		    template <typename number>
		    void Ptensor_line<number>::write(std::ostream& out)
			    {
				    out << " " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_PTENSOR << '\n';
			    }

	    }
	}

#endif //CPPTRANSPORT_PTENSOR_LINE_H
