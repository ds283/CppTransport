//
// Created by David Seery on 26/03/15.
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


#if !defined(CPPTRANSPORT_REPOSITORY_RECORDS_FORWARD_DECLARE_H) && !defined(CPPTRANSPORT_REPOSITORY_RECORDS_H)
#define CPPTRANSPORT_REPOSITORY_RECORDS_FORWARD_DECLARE_H


namespace transport
	{

		class record_metadata;
		class repository_record;
		template <typename number> class package_record;
		template <typename number> class task_record;
		template <typename number> class integration_task_record;
		template <typename number> class postintegration_task_record;
		template <typename number> class derived_product_record;
		template <typename number> class output_task_record;
    template <typename Payload> class content_group_record;

		class integration_metadata;
		class derived_content;
		class precomputed_products;
		class content_group_specifier;
		class integration_payload;
		class postintegration_payload;
		class output_payload;

	}   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_RECORDS_FORWARD_DECLARE_H
