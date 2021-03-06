//
// Created by David Seery on 22/03/2016.
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

#ifndef CPPTRANSPORT_REPOSITORY_CACHE_H
#define CPPTRANSPORT_REPOSITORY_CACHE_H


#include "transport-runtime/repository/repository.h"

#include "boost/optional.hpp"


namespace transport
  {

    //! repository objects can enumerate their contents, but this can be costly
    //! because even though the repository has an internal cache, it does have to
    //! duplicate records and this involves a copy
    //! To reduce wasteful multiple enumerations, the repository_cache class caches
    //! these enumerations using boost::optional.
    //! NOTE: REPOSITORY_CACHE<> SHOULD ONLY BE USED FOR REPORTING PURPOSES TO
    //! AVOID THE RISK OF RECORDS BECOMING STALE
    template <typename number>
    class repository_cache
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        repository_cache(repository<number>& r)
          : repo(r)
          {
          }

        //! disable copying to prevent aliasing
        repository_cache(const repository_cache<number>& obj) = delete;

        //! destructor
        ~repository_cache() = default;


        // GET REPOSITORY ENUMERATIONS

      public:

        //! get package database
        typename package_db<number>::type& get_package_db();

        //! get task database
        typename task_db<number>::type& get_task_db();

        //! get derived product database
        typename derived_product_db<number>::type& get_derived_product_db();


        //! get integration content database
        integration_content_db& get_integration_content_db();

        //! get postintegration content database
        postintegration_content_db& get_postintegration_content_db();

        //! get output content database
        output_content_db& get_output_content_db();


        //! get inlight content
        inflight_db& get_inflight_db();


        // RESET ALL CACHES

      public:

        //! reset
        void reset();


        // INTERNAL DATA

      private:

        // REPOSITORY REFERENCE

        //! repository for which we are caching
        repository<number>& repo;


        // REPOSITORY RECORDS


        //! cache package database
        boost::optional< typename package_db<number>::type > pkg_db;

        //! cache task database
        boost::optional< typename task_db<number>::type > tk_db;

        //! cache derived products
        boost::optional< typename derived_product_db<number>::type > prod_db;


        // OUTPUT GROUPS

        //! cache integration content
        boost::optional< integration_content_db > integration_db;

        //! cache postintegration content
        boost::optional< postintegration_content_db > postintegration_db;

        //! cache output content
        boost::optional< output_content_db > output_db;


        // IN FLIGHT INTEGRATIONS

        //! overall in-flight groups
        boost::optional< inflight_db > inflight;

      };


    template <typename number>
    void repository_cache<number>::reset()
      {
        this->pkg_db.reset();
        this->tk_db.reset();
        this->prod_db.reset();

        this->integration_db.reset();
        this->postintegration_db.reset();
        this->output_db.reset();

        this->inflight.reset();
      }


    template <typename number>
    typename package_db<number>::type& repository_cache<number>::get_package_db()
      {
        if(!this->pkg_db) this->pkg_db = this->repo.enumerate_packages();
        return *this->pkg_db;
      }


    template <typename number>
    typename task_db<number>::type& repository_cache<number>::get_task_db()
      {
        if(!this->tk_db) this->tk_db = this->repo.enumerate_tasks();
        return *this->tk_db;
      }


    template <typename number>
    typename derived_product_db<number>::type& repository_cache<number>::get_derived_product_db()
      {
        if(!this->prod_db) this->prod_db = this->repo.enumerate_derived_products();
        return *this->prod_db;
      }


    template <typename number>
    integration_content_db& repository_cache<number>::get_integration_content_db()
      {
        if(!this->integration_db) this->integration_db = this->repo.enumerate_integration_task_content();
        return *this->integration_db;
      }


    template <typename number>
    postintegration_content_db& repository_cache<number>::get_postintegration_content_db()
      {
        if(!this->postintegration_db) this->postintegration_db = this->repo.enumerate_postintegration_task_content();
        return *this->postintegration_db;
      }


    template <typename number>
    output_content_db& repository_cache<number>::get_output_content_db()
      {
        if(!this->output_db) this->output_db = this->repo.enumerate_output_task_content();
        return *this->output_db;
      }


    template <typename number>
    inflight_db& repository_cache<number>::get_inflight_db()
      {
        if(!this->inflight) this->inflight = this->repo.enumerate_inflight();
        return *this->inflight;
      }


  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_CACHE_H
