//
// Created by David Seery on 28/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#if !defined(__tags_forward_declare_H_) && !defined(__tags_H_)
#define __tags_forward_declare_H_


namespace transport
	{

		template <typename number> class time_config_tag;
		template <typename number> class twopf_kconfig_tag;
		template <typename number> class threepf_kconfig_tag;
		template <typename number> class data_tag;
		template <typename number> class background_time_data_tag;
		template <typename number> class cf_time_data_tag;
		template <typename number> class cf_kconfig_data_tag;
		template <typename number> class zeta_twopf_time_data_tag;
		template <typename number> class zeta_threepf_time_data_tag;
		template <typename number> class zeta_reduced_bispectrum_time_data_tag;
		template <typename number> class zeta_twopf_kconfig_data_tag;
		template <typename number> class zeta_threepf_kconfig_data_tag;
		template <typename number> class zeta_reduced_bispectrum_kconfig_data_tag;
		template <typename number> class fNL_time_data_tag;
		template <typename number> class BT_time_data_tag;
		template <typename number> class TT_time_data_tag;

	}   // namespace transport


#endif //__tags_forward_declare_H_
