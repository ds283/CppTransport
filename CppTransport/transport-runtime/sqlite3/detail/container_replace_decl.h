//
// Created by David Seery on 26/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_DECL_H
#define CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_DECL_H


#include "transport-runtime/data/batchers/container_replace_function.h"

#include "transport-runtime/derived-products/derived-content/correlation-functions/template_types.h"


namespace transport
  {

    // forward declare data_manager_sqlite3
    template <typename number> class data_manager_sqlite3;

    template <typename number>
    class sqlite3_container_replace_twopf: public container_replace_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures data
        sqlite3_container_replace_twopf(data_manager_sqlite3<number>& dm, boost::filesystem::path p, unsigned int w, model<number>* m, bool ic)
          : data_mgr(dm),
            tempdir(std::move(p)),
            worker(w),
            mdl(m),
            ics(ic)
          {
          }

        //! destructor is default
        ~sqlite3_container_replace_twopf() = default;


        // INTERFACE

      public:

        //! replace
        void operator()(generic_batcher& batcher, replacement_action action) override;


        // INTERNAL DATA

      private:

        //! reference to data manager
        data_manager_sqlite3<number>& data_mgr;

        //! temporary directory
        boost::filesystem::path tempdir;

        //! worker number
        unsigned int worker;

        //! model pointer
        model<number>* mdl;

        //! flag indicating whether we are collecting initial conditions
        bool ics;

      };


    template <typename number>
    class sqlite3_container_replace_threepf: public container_replace_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures data
        sqlite3_container_replace_threepf(data_manager_sqlite3<number>& dm, boost::filesystem::path p, unsigned int w, model<number>* m, bool ic)
          : data_mgr(dm),
            tempdir(std::move(p)),
            worker(w),
            mdl(m),
            ics(ic)
          {
          }

        //! destructor is default
        ~sqlite3_container_replace_threepf() = default;


        // INTERFACE

      public:

        //! replace
        void operator()(generic_batcher& batcher, replacement_action action) override;


        // INTERNAL DATA

      private:

        //! reference to data manager
        data_manager_sqlite3<number>& data_mgr;

        //! temporary directory
        boost::filesystem::path tempdir;

        //! worker number
        unsigned int worker;

        //! model pointer
        model<number>* mdl;

        //! flag indicating whether we are collecting initial conditions
        bool ics;

      };


    template <typename number>
    class sqlite3_container_replace_zeta_twopf: public container_replace_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures data
        sqlite3_container_replace_zeta_twopf(data_manager_sqlite3<number>& dm, boost::filesystem::path p, unsigned int w, model<number>* m)
          : data_mgr(dm),
            tempdir(std::move(p)),
            worker(w),
            mdl(m)
          {
          }

        //! destructor is default
        ~sqlite3_container_replace_zeta_twopf() = default;


        // INTERFACE

      public:

        //! replace
        void operator()(generic_batcher& batcher, replacement_action action) override;


        // INTERNAL DATA

      private:

        //! reference to data manager
        data_manager_sqlite3<number>& data_mgr;

        //! temporary directory
        boost::filesystem::path tempdir;

        //! worker number
        unsigned int worker;

        //! model pointer
        model<number>* mdl;

      };


    template <typename number>
    class sqlite3_container_replace_zeta_threepf: public container_replace_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures data
        sqlite3_container_replace_zeta_threepf(data_manager_sqlite3<number>& dm, boost::filesystem::path p, unsigned int w, model<number>* m)
          : data_mgr(dm),
            tempdir(std::move(p)),
            worker(w),
            mdl(m)
          {
          }

        //! destructor is default
        ~sqlite3_container_replace_zeta_threepf() = default;


        // INTERFACE

      public:

        //! replace
        void operator()(generic_batcher& batcher, replacement_action action) override;


        // INTERNAL DATA

      private:

        //! reference to data manager
        data_manager_sqlite3<number>& data_mgr;

        //! temporary directory
        boost::filesystem::path tempdir;

        //! worker number
        unsigned int worker;

        //! model pointer
        model<number>* mdl;

      };


    template <typename number>
    class sqlite3_container_replace_fNL: public container_replace_function
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures data
        sqlite3_container_replace_fNL(data_manager_sqlite3<number>& dm, boost::filesystem::path p, unsigned int w, derived_data::template_type t)
          : data_mgr(dm),
            tempdir(std::move(p)),
            worker(w),
            type(t)
          {
          }

        //! destructor is default
        ~sqlite3_container_replace_fNL() = default;


        // INTERFACE

      public:

        //! replace
        void operator()(generic_batcher& batcher, replacement_action action) override;


        // INTERNAL DATA

      private:

        //! reference to data manager
        data_manager_sqlite3<number>& data_mgr;

        //! temporary directory
        boost::filesystem::path tempdir;

        //! worker number
        unsigned int worker;

        //! fNL template type
        derived_data::template_type type;

      };

  }   // namespace transport


#endif //CPPTRANSPORT_SQLITE3_CONTAINER_REPLACE_DECL_H
