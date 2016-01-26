//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CONTENT_FINDER_DECL_H
#define CPPTRANSPORT_CONTENT_FINDER_DECL_H


#include <string>
#include <functional>
#include <list>

// forward-declare repository records if necessary
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"


namespace transport
  {


    // forward-declare repository class
    template <typename number> class repository;


    template <typename number>
    class integration_content_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        integration_content_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~integration_content_finder() = default;


        // INTERFACE

      public:

        //! find output group
        std::unique_ptr< output_group_record<integration_payload> > operator()(const std::string& name, const std::list<std::string>& tags);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class postintegration_content_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        postintegration_content_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~postintegration_content_finder() = default;


        // INTERFACE

      public:

        //! find output group
        std::unique_ptr< output_group_record<postintegration_payload> > operator()(const std::string& name, const std::list<std::string>& tags);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_FINDER_DECL_H
