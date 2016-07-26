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


#ifndef CPPTRANSPORT_TENSOR_TWOPF_LINE_H
#define CPPTRANSPORT_TENSOR_TWOPF_LINE_H


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


#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TENSOR_TWOPF_LINE_ROOT          "tensor-twopf-line-settings"

#define CPPTRANSPORT_NODE_PRODUCT_DERIVED_TENSOR_TWOPF_LINE_DIMENSIONLESS "dimensionless"

namespace transport
	{

    namespace derived_data
	    {

		    //! general tensor twopf content producer, suitable
		    //! for producing content usable in eg. a 2d plot or table
		    //! Note that we derive virtually from derived_line<> to solve the diamond
		    //! problem -- concrete classes may inherit several derived_line<> attributes,
		    //! eg. wavenumber_series<> and tensor_twopf_line<>
		    template <typename number>
		    class tensor_twopf_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! Basic user-facing constructor
				    tensor_twopf_line(const twopf_db_task<number>& tk, index_selector<2> sel);

				    //! Deserialization constructor
				    tensor_twopf_line(Json::Value& reader, task_finder<number>& finder);

				    virtual ~tensor_twopf_line() = default;


            // MANAGE SETTINGS

          public:

            //! is this dimensionles?
            bool is_dimensionless() const { return(this->dimensionless); }

            //! set dimensionless
            tensor_twopf_line<number>& set_dimensionless(bool g) { this->dimensionless = g; return *this; }


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label
				    std::string make_LaTeX_label(unsigned int m, unsigned int n) const;

				    //! make a non-LaTeX label
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
		        integration_task_gadget<number> gadget;

				    //! record which indices are active in this group
				    index_selector<2> active_indices;

            //! compute the dimensionless twopf?
            bool dimensionless;

			    };


		    template <typename number>
		    tensor_twopf_line<number>::tensor_twopf_line(const twopf_db_task<number>& tk, index_selector<2> sel)
		      : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
		        gadget(tk),
		        active_indices(sel),
            dimensionless(true)
			    {
				    if(active_indices.get_number_fields() != 2)
					    {
				        std::ostringstream msg;
						    msg << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH << " ("
								    << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_A << " " << active_indices.get_number_fields() << ", "
								    << CPPTRANSPORT_PRODUCT_INDEX_MISMATCH_B << " 2)";
				        throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
					    }
			    }


		    template <typename number>
		    tensor_twopf_line<number>::tensor_twopf_line(Json::Value& reader, task_finder<number>& finder)
			    : derived_line<number>(reader),  // not called because of virtual inheritance; here to silence Intel compiler warning
			      gadget(),
		        active_indices(reader)
			    {
				    assert(this->parent_task != nullptr);
		        gadget.set_task(this->parent_task, finder);

            dimensionless = reader[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TENSOR_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TENSOR_TWOPF_LINE_DIMENSIONLESS].asBool();
			    }


		    template <typename number>
		    std::string tensor_twopf_line<number>::make_LaTeX_label(unsigned int m, unsigned int n) const
			    {
				    assert(m <= 1);
				    assert(n <= 1);
				    if(m > 1 || n > 1)
					    {
				        std::ostringstream msg;
						    msg << CPPTRANSPORT_PRODUCT_INDEX_BOUNDS << " (" << m << "," << n << ")";
						    throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
					    }

		        std::ostringstream label;
		        label << std::setprecision(this->precision);

		        if(m == 0)      label << CPPTRANSPORT_LATEX_TENSOR_SYMBOL;
				    else if(m == 1) label << CPPTRANSPORT_LATEX_TENSOR_MOMENTUM_SYMBOL;

				    label << " ";

				    if(n == 0)      label << CPPTRANSPORT_LATEX_TENSOR_SYMBOL;
				    else if(n == 1) label << CPPTRANSPORT_LATEX_TENSOR_MOMENTUM_SYMBOL;

				    return(label.str());
			    }


        template <typename number>
        std::string tensor_twopf_line<number>::make_non_LaTeX_label(unsigned int m, unsigned int n) const
	        {
            assert(m <= 1);
            assert(n <= 1);
            if(m > 1 || n > 1)
	            {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_INDEX_BOUNDS << " (" << m << "," << n << ")";
                throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
	            }

            std::ostringstream label;
            label << std::setprecision(this->precision);

            if(m == 0)      label << CPPTRANSPORT_NONLATEX_TENSOR_SYMBOL;
            else if(m == 1) label << CPPTRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL;

            label << " ";

            if(n == 0)      label << CPPTRANSPORT_NONLATEX_TENSOR_SYMBOL;
            else if(n == 1) label << CPPTRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL;

            return(label.str());
	        }


		    template <typename number>
		    void tensor_twopf_line<number>::serialize(Json::Value& writer) const
			    {
				    this->active_indices.serialize(writer);

            writer[CPPTRANSPORT_NODE_PRODUCT_DERIVED_TENSOR_TWOPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_TENSOR_TWOPF_LINE_DIMENSIONLESS] = this->dimensionless;
			    }


		    template <typename number>
		    void tensor_twopf_line<number>::write(std::ostream& out)
			    {
		        std::vector<std::string> names = { CPPTRANSPORT_NONLATEX_TENSOR_SYMBOL, CPPTRANSPORT_NONLATEX_TENSOR_MOMENTUM_SYMBOL };

				    out << " " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_TENSOR_TWOPF << '\n';
		        out << "  " << CPPTRANSPORT_PRODUCT_LINE_COLLECTION_LABEL_INDICES << " ";
		        this->active_indices.write(out, names);
		        out << '\n';
			    }

	    }
	}

#endif //CPPTRANSPORT_TENSOR_TWOPF_LINE_H
