//
// Created by David Seery on 27/04/15.
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


#ifndef CPPTRANSPORT_BATCHING_TRAITS_H
#define CPPTRANSPORT_BATCHING_TRAITS_H


#include "transport-runtime/sqlite3/operations/data_manager_common.h"


namespace transport
	{

		namespace sqlite3_operations
			{


				template <typename number, typename ValueType> struct data_traits;

				// set up partial specializations for the different types of data handled by writers:

				template<typename number> struct data_traits<number, typename integration_items<number>::twopf_re_item>
					{
						static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields)); }
						static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TWOPF_RE_VALUE_TABLE); }
						static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
				    static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TWOPF_DATATAB_FAIL); }
						static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TWOPF_COPY); }
					};


		    template<typename number> struct data_traits<number, typename integration_items<number>::twopf_im_item>
			    {
		        static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields)); }
		        static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TWOPF_IM_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
		        static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TWOPF_DATATAB_FAIL); }
		        static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
		        static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TWOPF_COPY); }
			    };


        template<typename number> struct data_traits<number, typename integration_items<number>::twopf_si_re_item>
          {
            static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields)); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TWOPF_SI_RE_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TWOPF_SI_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TWOPF_SI_COPY); }
          };


				template<typename number> struct data_traits<number, typename integration_items<number>::tensor_twopf_item>
					{
						static int number_elements(unsigned int Nfields) { return(4); }
						static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TENSOR_TWOPF_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
						static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TENSOR_TWOPF_DATATAB_FAIL); }
				    static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
				    static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TENSOR_TWOPF_COPY); }
					};


        template<typename number> struct data_traits<number, typename integration_items<number>::tensor_twopf_si_item>
          {
            static int number_elements(unsigned int Nfields) { return(4); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_TENSOR_TWOPF_SI_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_TENSOR_TWOPF_SI_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_TENSOR_TWOPF_SI_COPY); }
          };


				template<typename number> struct data_traits<number, typename integration_items<number>::threepf_momentum_item>
					{
						static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields * 2*Nfields)); }
						static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_THREEPF_MOMENTUM_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
						static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_THREEPF_MOMENTUM_DATATAB_FAIL); }
				    static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
				    static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_THREEPF_MOMENTUM_COPY); }
					};


        template<typename number> struct data_traits<number, typename integration_items<number>::threepf_Nderiv_item>
          {
            static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields * 2*Nfields)); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_THREEPF_DERIV_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_THREEPF_DERIV_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_THREEPF_DERIV_COPY); }
          };


        template<typename number> struct data_traits<number, typename integration_items<number>::configuration_statistics>
          {
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_STATS_TABLE); }
          };


		    template<typename number> struct data_traits<number, typename integration_items<number>::ics_item>
			    {
				    static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields)); }
				    static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ICS_TABLE); }
				    static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ICS_INSERT_FAIL); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
				    static std::string sqlite_serial_column()  { return("kserial"); }
				    static const bool has_texit = true;
            static const bool requires_primary_key = false;
			    };


		    template<typename number> struct data_traits<number, typename integration_items<number>::ics_kt_item>
			    {
		        static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields)); }
		        static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_KT_ICS_TABLE); }
		        static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ICS_INSERT_FAIL); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
		        static std::string sqlite_serial_column()  { return("kserial"); }
				    static const bool has_texit = true;
            static const bool requires_primary_key = false;
			    };


				template<typename number> struct data_traits<number, typename integration_items<number>::backg_item>
					{
						static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields)); }
						static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_BACKG_VALUE_TABLE); }
						static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_BACKG_DATATAB_FAIL); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
				    static std::string sqlite_serial_column()  { return("tserial"); }
						static const bool has_texit = false;
            static const bool requires_primary_key = true;
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_twopf_item>
					{
						static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ZETA_TWOPF_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
						static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ZETA_TWOPF_DATATAB_FAIL); }
				    static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
				    static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_ZETA_TWOPF_COPY); }
            static std::string column_name()           { return("twopf"); }
            static const bool has_redbsp = false;
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_threepf_item>
					{
						static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
						static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_ZETA_THREEPF_DATATAB_FAIL); }
				    static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
				    static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_ZETA_THREEPF_COPY); }
            static std::string column_name()           { return("threepf"); }
            static const bool has_redbsp = true;
					};


				template<typename number> struct data_traits<number, typename postintegration_items<number>::zeta_redbsp_item>
					{
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_ZETA_THREEPF_VALUE_TABLE); }
            static std::string column_name()           { return("redbsp"); }
					};


        template <typename number> struct data_traits<number, typename postintegration_items<number>::gauge_xfm1_item>
          {
            static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields)); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_GAUGE_XFM1_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_GAUGE1_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_TWOPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_GAUGE_XFM1_COPY); }
          };


        template <typename number> struct data_traits<number, typename postintegration_items<number>::gauge_xfm2_123_item>
          {
            static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields)); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_GAUGE_XFM2_123_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_GAUGE2_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_GAUGE_XFM2_COPY); }
          };


        template <typename number> struct data_traits<number, typename postintegration_items<number>::gauge_xfm2_213_item>
          {
            static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields)); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_GAUGE_XFM2_213_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_GAUGE2_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_GAUGE_XFM2_COPY); }
          };


        template <typename number> struct data_traits<number, typename postintegration_items<number>::gauge_xfm2_312_item>
          {
            static int number_elements(unsigned int Nfields) { return(static_cast<int>(2*Nfields * 2*Nfields)); }
            static std::string sqlite_table()          { return(CPPTRANSPORT_SQLITE_GAUGE_XFM2_312_VALUE_TABLE); }
            static std::string sqlite_unique_column()  { return("unique_id"); }
            static std::string write_error_msg()       { return(CPPTRANSPORT_DATACTR_GAUGE2_DATATAB_FAIL); }
            static std::string sqlite_sample_table()   { return(CPPTRANSPORT_SQLITE_THREEPF_SAMPLE_TABLE); }
            static std::string copy_error_msg()        { return(CPPTRANSPORT_DATACTR_GAUGE_XFM2_COPY); }
          };


			}   // namespace sqlite3_operations

	}   // namespace transport


#endif //CPPTRANSPORT_BATCHING_TRAITS_H
