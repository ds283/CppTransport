//
// Created by David Seery on 27/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __batching_traits_H_
#define __batching_traits_H_


#include "transport-runtime-api/sqlite3/operations/data_manager_common.h"


namespace transport
	{

		namespace sqlite3_operations
			{


				template <typename number, typename ValueType> struct data_traits;

				// set up partial specializations for the different types of data handled by writers:

				template<typename number> struct data_traits<number, typename integration_items<number>::twopf_re_item>
					{
						static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields); }
						static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE); }
						static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
				    static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TWOPF_DATATAB_FAIL); }
						static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TWOPF_COPY); }
					};


		    template<typename number> struct data_traits<number, typename integration_items<number>::twopf_im_item>
			    {
		        static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields); }
		        static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE); }
		        static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TWOPF_DATATAB_FAIL); }
		        static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
		        static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TWOPF_COPY); }
			    };


				template<typename number> struct data_traits<number, typename integration_items<number>::tensor_twopf_item>
					{
						static int number_elements(unsigned int Nfields) { return(4); }
						static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE); }
						static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TENSOR_TWOPF_DATATAB_FAIL); }
				    static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
				    static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TENSOR_TWOPF_COPY); }
					};


				template<typename number> struct data_traits<number, typename integration_items<number>::threepf_item>
					{
						static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields * 2*Nfields); }
						static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_THREEPF_VALUE_TABLE); }
						static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_THREEPF_DATATAB_FAIL); }
				    static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
				    static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_THREEPF_COPY); }
					};


		    template<typename number> struct data_traits<number, typename integration_items<number>::ics_item>
			    {
				    static int number_elements(unsigned int Nfields) { return(2*Nfields); }
				    static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ICS_TABLE); }
				    static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ICS_INSERT_FAIL); }
				    static const std::string sqlite_serial_column()  { return("kserial"); }
				    static const bool has_texit = true;
			    };


		    template<typename number> struct data_traits<number, typename integration_items<number>::ics_kt_item>
			    {
		        static int number_elements(unsigned int Nfields) { return(2*Nfields); }
		        static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_KT_ICS_TABLE); }
		        static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ICS_INSERT_FAIL); }
		        static const std::string sqlite_serial_column()  { return("kserial"); }
				    static const bool has_texit = true;
			    };


				template<typename number> struct data_traits<number, typename integration_items<number>::backg_item>
					{
						static int number_elements(unsigned int Nfields) { return(2*Nfields); }
						static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE); }
						static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_BACKG_DATATAB_FAIL); }
				    static const std::string sqlite_serial_column()  { return("tserial"); }
						static const bool has_texit = false;
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_twopf_item>
					{
						static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE); }
						static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ZETA_TWOPF_DATATAB_FAIL); }
				    static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
				    static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_ZETA_TWOPF_COPY); }
            static const std::string column_name()           { return("twopf"); }
            static const bool has_redbsp = false;
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_threepf_item>
					{
						static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE); }
						static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ZETA_THREEPF_DATATAB_FAIL); }
				    static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
				    static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_ZETA_THREEPF_COPY); }
            static const std::string column_name()           { return("threepf"); }
            static const bool has_redbsp = true;
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_redbsp_item>
					{
            static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE); }
            static const std::string column_name()           { return("redbsp"); }
					};


        template <typename number> struct data_traits<number, typename postintegration_items<number>::gauge_xfm1_item>
          {
            static int number_elements(unsigned int Nfields) { return(2*Nfields); }
            static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE); }
            static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_GAUGE1_DATATAB_FAIL); }
            static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
            static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_GAUGE_XFM1_COPY); }
          };


        template <typename number> struct data_traits<number, typename postintegration_items<number>::gauge_xfm2_item>
          {
            static int number_elements(unsigned int Nfields) { return(2*Nfields * 2*Nfields); }
            static const std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_GAUGE_XFM2_VALUE_TABLE); }
            static const std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_GAUGE2_DATATAB_FAIL); }
            static const std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
            static const std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_GAUGE_XFM2_COPY); }
          };


			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //__batching_traits_H_
