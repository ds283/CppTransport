//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_view_H_
#define __data_view_H_


#include <vector>

namespace transport
	{

		//! Forward-declare the derivation_data_delegate class.
		//! This depends on repository.h to specify an integration_reader object,
		//! which it uses to suck output from the database; but
		//! repository.h depends on the derived products,
		//! in order to get them in and out of the database.
		//! The solution is to forward-declare the delegate class here,
		//! so that this file doesn't have to include repository.h
    template <typename number>
    class derivation_data_delegate;


		namespace derived_data
			{

		    //! A data_view is a 'view' onto the database.
		    //! It represents a single-valued datastream which is a function of
		    //! a single variable (usually, a k-configuration or a time)

		    template <typename number>
		    class data_view
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        data_view() = default;
		        virtual ~data_view() = default;


		        // EXTRACT DATA

		        virtual std::vector<number> slice() = 0;

			    };

			}   // namespace derived_data

	}   // namespace transport



#endif //__data_view_H_
