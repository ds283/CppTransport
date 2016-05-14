//
// Created by David Seery on 26/05/2014.
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


#ifndef CPPTRANSPORT_WAVENUMBER_SERIES_TABLE_H
#define CPPTRANSPORT_WAVENUMBER_SERIES_TABLE_H


#include "transport-runtime/derived-products/line-collections/line_asciitable.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"


namespace transport
	{

		namespace derived_data
			{

				//! wavenumber-series-table is a convenience class which defines a line-asciitable with
				//! sensible defaults for wavenumber series

				template <typename number=default_number_type>
		    class wavenumber_series_table: public line_asciitable<number>
			    {

		      public:

				    // CONSTRUCTOR, DESTRUCTOR

						//! Basic user-facing constructor. Accepts a filename.
				    wavenumber_series_table(const std::string& name, const boost::filesystem::path& filename);

				    virtual ~wavenumber_series_table() = default;


				    // SETTING DEFAULTS

		      public:

				    //! (re-)set a default set of labels
				    virtual void apply_default_labels(bool x_label_set=true) override;

			    };


				template <typename number>
				wavenumber_series_table<number>::wavenumber_series_table(const std::string& name, const boost::filesystem::path& filename)
					: line_asciitable<number>(name, filename)
					{
						apply_default_labels();
					}


				template <typename number>
				void wavenumber_series_table<number>::apply_default_labels(bool x_label_set)
					{
				    axis_value x_axis = this->get_x_axis_value();

				    if(x_label_set)
					    {
				        switch(x_axis)
					        {
				            case axis_value::unset:
				            case axis_value::k:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_K_VALUE_TABLE_LABEL_NOLATEX);
												break;
											}

				            case axis_value::efolds_exit:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_EFOLDS_EXIT_TABLE_LABEL_NOLATEX);
												break;
											}

				            case axis_value::alpha:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_ALPHA_TABLE_LABEL_NOLATEX);
												break;
											}

				            case axis_value::beta:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_BETA_TABLE_LABEL_NOLATEX);
												break;
											}

				            case axis_value::squeeze_k1:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K1_TABLE_LABEL_NOLATEX);
												break;
											}

				            case axis_value::squeeze_k2:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K2_TABLE_LABEL_NOLATEX);
												break;
											}

				            case axis_value::squeeze_k3:
											{
												this->internal_set_x_label(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K3_TABLE_LABEL_NOLATEX);
												break;
											}

				            default:
					            assert(false);
					        }
					    }
					}

			}   // namespace derived_data

	}   // namespace transport



#endif //CPPTRANSPORT_WAVENUMBER_SERIES_TABLE_H
