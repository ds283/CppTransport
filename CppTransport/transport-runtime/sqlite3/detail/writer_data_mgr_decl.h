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

#ifndef CPPTRANSPORT_SQLITE3_WRITER_DATA_MGR_DECL_H
#define CPPTRANSPORT_SQLITE3_WRITER_DATA_MGR_DECL_H


namespace transport
  {

    template <typename number>
    class sqlite3_twopf_writer_aggregate: public integration_writer_aggregate<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        explicit sqlite3_twopf_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_twopf_writer_aggregate() = default;


        // INTERFACE

      public:

        //! commit
        bool operator()(integration_writer<number>& writer, const boost::filesystem::path& product) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_twopf_writer_integrity: public integration_writer_integrity<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        explicit sqlite3_twopf_writer_integrity(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_twopf_writer_integrity() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(integration_writer<number>& writer, integration_task<number>& task) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_twopf_writer_finalize: public integration_writer_finalize<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        explicit sqlite3_twopf_writer_finalize(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_twopf_writer_finalize() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(integration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };

    
    template <typename number>
    class sqlite3_threepf_writer_aggregate: public integration_writer_aggregate<number>
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor captures repository_sqlite3 object
        explicit sqlite3_threepf_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }
        
        //! destructor is default
        virtual ~sqlite3_threepf_writer_aggregate() = default;
        
        
        // INTERFACE
      
      public:
        
        //! commit
        bool operator()(integration_writer<number>& writer, const boost::filesystem::path& product) override;
        
        
        // INTERNAL DATA
      
      private:
        
        //! reference to repository object
        data_manager_sqlite3<number>& mgr;
        
      };
    
    
    template <typename number>
    class sqlite3_threepf_writer_integrity: public integration_writer_integrity<number>
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor captures repository_sqlite3 object
        sqlite3_threepf_writer_integrity(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }
        
        //! destructor is default
        virtual ~sqlite3_threepf_writer_integrity() = default;
        
        
        // INTERFACE
      
      public:
        
        //! commit
        void operator()(integration_writer<number>& writer, integration_task<number>& task) override;
        
        
        // INTERNAL DATA
      
      private:
        
        //! reference to repository object
        data_manager_sqlite3<number>& mgr;
        
      };


    template <typename number>
    class sqlite3_threepf_writer_finalize: public integration_writer_finalize<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_threepf_writer_finalize(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_threepf_writer_finalize() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(integration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_zeta_twopf_writer_aggregate: public postintegration_writer_aggregate<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_zeta_twopf_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_zeta_twopf_writer_aggregate() = default;


        // INTERFACE

      public:

        //! commit
        bool operator()(postintegration_writer<number>& writer, const boost::filesystem::path& product) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_zeta_twopf_writer_integrity: public postintegration_writer_integrity<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_zeta_twopf_writer_integrity(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_zeta_twopf_writer_integrity() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer, postintegration_task<number>& task) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_zeta_twopf_writer_finalize: public postintegration_writer_finalize<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_zeta_twopf_writer_finalize(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_zeta_twopf_writer_finalize() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };

    
    template <typename number>
    class sqlite3_zeta_threepf_writer_aggregate: public postintegration_writer_aggregate<number>
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor captures repository_sqlite3 object
        sqlite3_zeta_threepf_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }
        
        //! destructor is default
        virtual ~sqlite3_zeta_threepf_writer_aggregate() = default;
        
        
        // INTERFACE
      
      public:
        
        //! commit
        bool operator()(postintegration_writer<number>& writer, const boost::filesystem::path& product) override;
        
        
        // INTERNAL DATA
      
      private:
        
        //! reference to repository object
        data_manager_sqlite3<number>& mgr;
        
      };
    
    
    template <typename number>
    class sqlite3_zeta_threepf_writer_integrity: public postintegration_writer_integrity<number>
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor captures repository_sqlite3 object
        sqlite3_zeta_threepf_writer_integrity(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }
        
        //! destructor is default
        virtual ~sqlite3_zeta_threepf_writer_integrity() = default;
        
        
        // INTERFACE
      
      public:
        
        //! commit
        void operator()(postintegration_writer<number>& writer, postintegration_task<number>& task) override;
        
        
        // INTERNAL DATA
      
      private:
        
        //! reference to repository object
        data_manager_sqlite3<number>& mgr;
        
      };


    template <typename number>
    class sqlite3_zeta_threepf_writer_finalize: public postintegration_writer_finalize<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_zeta_threepf_writer_finalize(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_zeta_threepf_writer_finalize() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_fNL_writer_aggregate: public postintegration_writer_aggregate<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_fNL_writer_aggregate(data_manager_sqlite3<number>& m, derived_data::bispectrum_template t)
          : mgr(m),
            type(t)
          {
          }

        //! destructor is default
        virtual ~sqlite3_fNL_writer_aggregate() = default;


        // INTERFACE

      public:

        //! commit
        bool operator()(postintegration_writer<number>& writer, const boost::filesystem::path& product) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

        //! fNL template type
        derived_data::bispectrum_template type;

      };


    template <typename number>
    class sqlite3_fNL_writer_integrity: public postintegration_writer_integrity<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_fNL_writer_integrity(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_fNL_writer_integrity() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer, postintegration_task<number>& task) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_fNL_writer_finalize: public postintegration_writer_finalize<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_fNL_writer_finalize(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_fNL_writer_finalize() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(postintegration_writer<number>& writer) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };


    template <typename number>
    class sqlite3_derived_content_writer_aggregate: public derived_content_writer_aggregate<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository_sqlite3 object
        sqlite3_derived_content_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_derived_content_writer_aggregate() = default;


        // INTERFACE

      public:

        //! commit
        bool operator()(derived_content_writer<number>& writer, const std::string& product, const std::list<std::string>& used_groups) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_SQLITE3_WRITER_DATA_MGR_DECL_H
