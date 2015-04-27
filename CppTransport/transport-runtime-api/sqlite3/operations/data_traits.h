//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __batching_traits_H_
#define __batching_traits_H_


#import "transport-runtime-api/sqlite3/operations/data_manager_common.h"


namespace transport
	{

		namespace sqlite3_operations
			{


				template <typename number, typename ValueType> struct data_traits;

				// set up partial specializations for the different types of data handled by writers:

				template<typename number> struct data_traits<number, typename integration_items<number>::twopf_re_item>
					{
						static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields); }
						static const std::string sqlite_table() { return(__CPP_TRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE); }
						static const std::string error_msg() { return(__CPP_TRANSPORT_DATACTR_TWOPF_DATATAB_FAIL); }
					};


		    template<typename number> struct data_traits<number, typename integration_items<number>::twopf_im_item>
			    {
		        static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields); }
		        static const std::string sqlite_table() { return(__CPP_TRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE); }
		        static const std::string error_msg() { return(__CPP_TRANSPORT_DATACTR_TWOPF_DATATAB_FAIL); }
			    };


				template<typename number> struct data_traits<number, typename integration_items<number>::tensor_twopf_item>
					{
						static int number_elements(unsigned int Nfields) { return(4); }
						static const std::string sqlite_table() { return(__CPP_TRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE); }
						static const std::string error_msg() { return(__CPP_TRANSPORT_DATACTR_TENSOR_TWOPF_DATATAB_FAIL); }
					};


				template<typename number> struct data_traits<number, typename integration_items<number>::threepf_item>
					{
						static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields * 2*Nfields); }
						static const std::string sqlite_table() { return(__CPP_TRANSPORT_SQLITE_THREEPF_VALUE_TABLE); }
						static const std::string error_msg() { return(__CPP_TRANSPORT_DATACTR_THREEPF_DATATAB_FAIL); }
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_twopf_item>
					{
						static const char* sqlite_table() { return(__CPP_TRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE); }
						static const char* error_msg() { return(__CPP_TRANSPORT_DATACTR_ZETA_TWOPF_DATATAB_FAIL); }
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_threepf_item>
					{
						static const std::string sqlite_table() { return(__CPP_TRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE); }
						static const std::string error_msg() { return(__CPP_TRANSPORT_DATACTR_ZETA_THREEPF_DATATAB_FAIL); }
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_redbsp_item>
					{
						static const std::string sqlite_table() { return(__CPP_TRANSPORT_SQLITE_ZETA_REDUCED_BISPECTRUM_VALUE_TABLE); }
						static const std::string error_msg() { return(__CPP_TRANSPORT_DATACTR_ZETA_REDBSP_DATATAB_FAIL); }
					};

			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //__batching_traits_H_
