//
// Created by David Seery on 28/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
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
        sqlite3_twopf_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_twopf_writer_aggregate() = default;


        // INTERFACE

      public:

        //! commit
        bool operator()(integration_writer<number>& writer, const std::string& product) override;


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
        sqlite3_twopf_writer_integrity(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        virtual ~sqlite3_twopf_writer_integrity() = default;


        // INTERFACE

      public:

        //! commit
        void operator()(integration_writer<number>& writer, integration_task<number>* task) override;


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
        sqlite3_threepf_writer_aggregate(data_manager_sqlite3<number>& m)
          : mgr(m)
          {
          }
        
        //! destructor is default
        virtual ~sqlite3_threepf_writer_aggregate() = default;
        
        
        // INTERFACE
      
      public:
        
        //! commit
        bool operator()(integration_writer<number>& writer, const std::string& product) override;
        
        
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
        void operator()(integration_writer<number>& writer, integration_task<number>* task) override;
        
        
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
        bool operator()(postintegration_writer<number>& writer, const std::string& product) override;


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
        void operator()(postintegration_writer<number>& writer, postintegration_task<number>* task) override;


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
        bool operator()(postintegration_writer<number>& writer, const std::string& product) override;
        
        
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
        void operator()(postintegration_writer<number>& writer, postintegration_task<number>* task) override;
        
        
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
        sqlite3_fNL_writer_aggregate(data_manager_sqlite3<number>& m, derived_data::template_type t)
          : mgr(m),
            type(t)
          {
          }

        //! destructor is default
        virtual ~sqlite3_fNL_writer_aggregate() = default;


        // INTERFACE

      public:

        //! commit
        bool operator()(postintegration_writer<number>& writer, const std::string& product) override;


        // INTERNAL DATA

      private:

        //! reference to repository object
        data_manager_sqlite3<number>& mgr;

        //! fNL template type
        derived_data::template_type type;

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
        void operator()(postintegration_writer<number>& writer, postintegration_task<number>* task) override;


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
