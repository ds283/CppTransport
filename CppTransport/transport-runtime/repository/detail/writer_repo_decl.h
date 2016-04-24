//
// Created by David Seery on 28/01/2016.
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

#ifndef CPPTRANSPORT_WRITER_REPO_DECL_H
#define CPPTRANSPORT_WRITER_REPO_DECL_H


namespace transport
  {

    // forward-declare repository
    template <typename number> class repository;


    template <typename number>
    class repository_integration_writer_commit: public integration_writer_commit<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        repository_integration_writer_commit(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        virtual ~repository_integration_writer_commit() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(integration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class repository_integration_writer_abort: public integration_writer_abort<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        repository_integration_writer_abort(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        virtual ~repository_integration_writer_abort() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(integration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class repository_postintegration_writer_commit: public postintegration_writer_commit<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        repository_postintegration_writer_commit(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        virtual ~repository_postintegration_writer_commit() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class repository_postintegration_writer_abort: public postintegration_writer_abort<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        repository_postintegration_writer_abort(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        virtual ~repository_postintegration_writer_abort() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class repository_derived_content_writer_commit: public derived_content_writer_commit<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        repository_derived_content_writer_commit(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        virtual ~repository_derived_content_writer_commit() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(derived_content_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class repository_derived_content_writer_abort: public derived_content_writer_abort<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        repository_derived_content_writer_abort(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        virtual ~repository_derived_content_writer_abort() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(derived_content_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_WRITER_REPO_DECL_H
