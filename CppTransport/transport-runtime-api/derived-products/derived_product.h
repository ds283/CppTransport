//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __derived_product_H_
#define __derived_product_H_


#include "transport-runtime-api/serialization/serializable.h"


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

		//! A derived product represents some particular post-processing
		//! of the integration data, perhaps to produce a plot,
		//! extract the data in some suitable format, or even to decompose
		//! in terms of basis functions.
		template <typename number>
    class derived_product: public serializable
	    {

      public:

		    // CONSTRUCTOR, DESTRUCTOR

		    derived_product() = default;
		    virtual ~derived_product() = default;


				// INTERFACE

		    //! Apply the analysis represented by this derived product to a given
		    //! output group
		    virtual void apply(derivation_data_delegate<number>& delegate) = 0;


		    // INTERNAL DATA

      protected:

		    //! Name of this derived product
		    std::string name;

		    //! Standardized filename
		    std::string filename;
	    };

	}   // namespace transport


#endif //__derived_product_H_
