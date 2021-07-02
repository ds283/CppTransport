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


#ifndef CPPTRANSPORT_ZETA_NS_LINE_H
#define CPPTRANSPORT_ZETA_NS_LINE_H


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

			  constexpr auto CPPTRANSPORT_NODE_PRODUCT_ZETA_NS_LINE_ROOT = "zeta-ns-line-settings";


				//! general zeta twopf spectral content producer, suitable
				//! for producing content usable in eg. a 2d plot or table.
				//! Note that we derive virtually from derived_line<> to solve the diamond
				//! problem -- concrete classes may inherit several derived_line<> attributes,
				//! eg. wavenumber_series<> and zeta_ns_line<>
				template <typename number>
		    class zeta_ns_line: public virtual derived_line<number>
			    {

			      // CONSTRUCTOR, DESTRUCTOR

          public:

				    //! Basic user-facing constructor
				    explicit zeta_ns_line(const zeta_twopf_db_task<number>& tk);

				    //! Deserialization constructor
				    explicit zeta_ns_line(Json::Value& reader);

				    virtual ~zeta_ns_line() = default;


				    // LABELLING SERVICES

		      public:

				    //! make a LaTeX label
				    std::string make_LaTeX_label() const;

				    //! make a non-LaTeX label
				    std::string make_non_LaTeX_label() const;


				    // WRITE TO A STREAM

		      public:

				    //! write self-details to a stream
				    void write(std::ostream& out) override;


				    // SERIALIZATION -- implements a 'serializable' interface

		      public:

				    //! Serialize this object
				    void serialize(Json::Value& writer) const override;

			    };


				template <typename number>
				zeta_ns_line<number>::zeta_ns_line(const zeta_twopf_db_task<number>& tk)
		      : derived_line<number>(tk)  // not called because of virtual inheritance; here to silence Intel compiler warning
					{
					  if(!tk.get_collect_spectral_data())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_ZETA_NS_LINE_NO_SPECTRAL_DATA << ": '" << tk.get_name() << "'";
                throw runtime_exception(exception_type::DERIVED_PRODUCT_ERROR, msg.str());
              }
					}


				template <typename number>
				zeta_ns_line<number>::zeta_ns_line(Json::Value& reader)
					: derived_line<number>(reader)  // not called because of virtual inheritance; here to silence Intel compiler warning
					{
					}


				template <typename number>
				std::string zeta_ns_line<number>::make_LaTeX_label() const
					{
            return( std::string(CPPTRANSPORT_LATEX_PZETA_NS_SYMBOL) );
					}


				template <typename number>
				std::string zeta_ns_line<number>::make_non_LaTeX_label() const
					{
            return( std::string(CPPTRANSPORT_NONLATEX_PZETA_NS_SYMBOL) );
					}


				template <typename number>
				void zeta_ns_line<number>::serialize(Json::Value& writer) const
					{
					}


				template <typename number>
				void zeta_ns_line<number>::write(std::ostream& out)
					{
				    out << "  " << CPPTRANSPORT_PRODUCT_WAVENUMBER_SERIES_LABEL_ZETA_TWOPF_SPECTRAL << '\n';
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_ZETA_NS_LINE_H
