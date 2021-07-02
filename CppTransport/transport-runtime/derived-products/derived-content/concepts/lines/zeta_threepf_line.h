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


#ifndef CPPTRANSPORT_ZETA_THREEPF_LINE_H
#define CPPTRANSPORT_ZETA_THREEPF_LINE_H


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


namespace transport
	{

		namespace derived_data
			{

        constexpr auto CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT          = "zeta-threepf-line-settings";
        constexpr auto CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_DIMENSIONLESS = "dimensionless";


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
				    explicit zeta_threepf_line(const zeta_threepf_task<number>& tk);

				    //! Deserialization constructor
				    explicit zeta_threepf_line(Json::Value& reader);

				    virtual ~zeta_threepf_line() = default;


            // SETTINGS

          public:

            //! is this dimensionles?
            bool is_dimensionless() const { return(this->dimensionless); }

            //! set dimensionless
            zeta_threepf_line<number>& set_dimensionless(bool g) { this->dimensionless = g; return *this; }


		        // MANAGE LABEL OPTIONS

          public:

		        //! get k_t label setting
		        bool get_use_kt_label() const { return(this->use_kt_label); }

		        //! set k_t label setting
		        zeta_threepf_line<number>& set_use_kt_label(bool g) { this->use_kt_label = g; return *this; }

		        //! get alpha label setting
		        bool get_use_alpha_label() const { return(this->use_alpha_label); }

		        //! set alpha label setting
		        zeta_threepf_line<number>& set_use_alpha_label(bool g) { this->use_alpha_label = g; return *this; }

		        //! get beta label setting
		        bool get_use_beta_label() const { return(this->use_beta_label); }

		        //! set beta label setting
		        zeta_threepf_line<number>& set_use_beta_label(bool g) { this->use_beta_label = g; return *this; }


		        // LABELLING SERVICES

          public:

		        //! make a LaTeX label for one of our lines
		        std::string make_LaTeX_label() const;

		        //! make a non-LaTeX label for one of our lines
		        std::string make_non_LaTeX_label() const;


		        // WRITE TO A STREAM

		      public:

		        //! write self-details to a stream
		        void write(std::ostream& out) override;


		        // SERIALIZATION -- implements a 'serializable' interface

		      public:

		        //! Serialize this object
		        void serialize(Json::Value& writer) const override;


		        // INTERNAL DATA

		      protected:

		        //! use k_t on line labels?
		        bool use_kt_label;

		        //! use alpha on line labels?
		        bool use_alpha_label;

		        //! use beta on line labels?
		        bool use_beta_label;

            //! compute the dimensionless threepf?
            bool dimensionless;

			    };


		    template <typename number>
		    zeta_threepf_line<number>::zeta_threepf_line(const zeta_threepf_task<number>& tk)
		      : derived_line<number>(tk),  // not called because of virtual inheritance; here to silence Intel compiler warning
		        use_kt_label(true),
            use_alpha_label(false),
            use_beta_label(false),
            dimensionless(true)
			    {
          }


				template <typename number>
				zeta_threepf_line<number>::zeta_threepf_line(Json::Value& reader)
          : derived_line<number>(reader)  // not called because of virtual inheritance; here to silence Intel compiler warning
					{
            use_kt_label    = reader[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT].asBool();
            use_alpha_label = reader[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA].asBool();
            use_beta_label  = reader[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA].asBool();

            dimensionless   = reader[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_DIMENSIONLESS].asBool();
					}


				template <typename number>
				std::string zeta_threepf_line<number>::make_LaTeX_label() const
					{
            std::ostringstream label;
            if(this->dimensionless) label << CPPTRANSPORT_LATEX_SHAPE_FACTOR << " ";
            label << CPPTRANSPORT_LATEX_ZETA_SYMBOL << " " << CPPTRANSPORT_LATEX_ZETA_SYMBOL << " " << CPPTRANSPORT_LATEX_ZETA_SYMBOL;

            return(label.str());
					}


				template <typename number>
				std::string zeta_threepf_line<number>::make_non_LaTeX_label() const
					{
            std::ostringstream label;
            if(this->dimensionless) label << CPPTRANSPORT_NONLATEX_SHAPE_FACTOR << " ";
            label << CPPTRANSPORT_NONLATEX_ZETA_SYMBOL << " " << CPPTRANSPORT_NONLATEX_ZETA_SYMBOL << " " << CPPTRANSPORT_NONLATEX_ZETA_SYMBOL;

            return(label.str());
					}


				template <typename number>
				void zeta_threepf_line<number>::serialize(Json::Value& writer) const
					{
            writer[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_KT]    = this->use_kt_label;
            writer[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_ALPHA] = this->use_alpha_label;
            writer[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_DERIVED_LINE_THREEPF_LABEL_BETA]  = this->use_beta_label;

            writer[CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_ROOT][CPPTRANSPORT_NODE_PRODUCT_ZETA_THREEPF_LINE_DIMENSIONLESS]  = this->dimensionless;
					}


				template <typename number>
				void zeta_threepf_line<number>::write(std::ostream& out)
					{
				    out << "  " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_THREEPF << '\n';
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_ZETA_THREEPF_LINE_H
