//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __record_finder_H_
#define __record_finder_H_


#include <string>
#include <functional>


namespace transport
	{

    // SERVICES

    template <typename number> class package_record;
    template <typename number> class task_record;
    template <typename number> class derived_product_record;

    template <typename number>
    class repository_finder
	    {

      public:

        //! Package finder service
        typedef std::function<package_record<number>*(const std::string&)> package_finder;

        //! Task finder service
        typedef std::function<task_record<number>*(const std::string&)> task_finder;

        //! Derived product finder service
        typedef std::function<derived_product_record<number>*(const std::string&)> derived_product_finder;

	    };

	}   // namespace transport


#endif //__record_finder_H_
