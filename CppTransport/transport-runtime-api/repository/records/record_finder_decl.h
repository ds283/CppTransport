//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_RECORD_FINDER_DECL_H
#define CPPTRANSPORT_RECORD_FINDER_DECL_H


#include <string>
#include <functional>

// forward-declare repository records if necessary
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"


namespace transport
	{

    // forward-declare repository class
    template <typename number> class repository;


    template <typename number>
    class package_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        package_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~package_finder() = default;


        // INTERFACE

      public:

        //! find package
        package_record<number>* operator()(const std::string& name);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class task_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        task_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~task_finder() = default;


        // INTERFACE

      public:

        //! find package
        task_record<number>* operator()(const std::string& name);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class derived_product_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        derived_product_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~derived_product_finder() = default;


        // INTERFACE

      public:

        //! find package
        derived_product_record<number>* operator()(const std::string& name);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };

	}   // namespace transport


#endif //CPPTRANSPORT_RECORD_FINDER_DECL_H
