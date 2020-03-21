//
// Created by David Seery on 03/07/2014.
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


#ifndef CPPTRANSPORT_TEMPLATE_TYPES_H
#define CPPTRANSPORT_TEMPLATE_TYPES_H


#include <stdexcept>

#include "transport-runtime/localizations/messages_en.h"


namespace transport
	{

		namespace derived_data
			{

				enum class bispectrum_template { local, equilateral, orthogonal, DBI };

				inline std::string template_type_to_string(bispectrum_template type)
					{
						switch(type)
							{
						    case bispectrum_template::local: 				return(std::string(CPPTRANSPORT_FNL_LOCAL));
						    case bispectrum_template::equilateral:  return(std::string(CPPTRANSPORT_FNL_EQUI));
						    case bispectrum_template::orthogonal: 	return(std::string(CPPTRANSPORT_FNL_ORTHO));
                case bispectrum_template::DBI:   				return(std::string(CPPTRANSPORT_FNL_DBI));
                default: throw std::invalid_argument("Invalid bispectrum_template type");
							}
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //CPPTRANSPORT_TEMPLATE_TYPES_H
