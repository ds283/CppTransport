//
// Created by David Seery on 28/03/15.
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


#if !defined(CPPTRANSPORT_TAGS_FORWARD_DECLARE_H) && !defined(CPPTRANSPORT_TAGS_H)
#define CPPTRANSPORT_TAGS_FORWARD_DECLARE_H


namespace transport
	{

    enum class cf_data_type;

		template <typename number> class time_config_tag;
		template <typename number> class twopf_kconfig_tag;
		template <typename number> class threepf_kconfig_tag;
		template <typename number> class data_tag;
		template <typename number> class background_time_data_tag;
		template <typename number> class cf_time_data_tag;
		template <typename number> class cf_kconfig_data_tag;
		template <typename number> class zeta_twopf_time_data_tag;
		template <typename number> class zeta_ns_time_data_tag;
		template <typename number> class zeta_threepf_time_data_tag;
		template <typename number> class zeta_reduced_bispectrum_time_data_tag;
		template <typename number> class zeta_twopf_kconfig_data_tag;
		template <typename number> class zeta_ns_kconfig_data_tag;
		template <typename number> class zeta_threepf_kconfig_data_tag;
		template <typename number> class zeta_reduced_bispectrum_kconfig_data_tag;
		template <typename number> class fNL_time_data_tag;
		template <typename number> class BT_time_data_tag;
		template <typename number> class TT_time_data_tag;
    template <typename number> class k_statistics_tag;

	}   // namespace transport


#endif //CPPTRANSPORT_TAGS_FORWARD_DECLARE_H
