//
// Created by David Seery on 18/12/2015.
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

#ifndef CPPTRANSPORT_MACROS_RESOURCES_H
#define CPPTRANSPORT_MACROS_RESOURCES_H


#include "directive_package.h"
#include "resource_manager.h"


namespace macro_packages
  {

    namespace RESOURCES
      {

        constexpr unsigned int PARAMETERS_KERNEL_ARGUMENT = 0;
        constexpr unsigned int PARAMETERS_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int COORDINATES_KERNEL_ARGUMENT = 0;
        constexpr unsigned int COORDINATES_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int PHASE_FLATTEN_KERNEL_ARGUMENT = 0;
        constexpr unsigned int PHASE_FLATTEN_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int FIELD_FLATTEN_KERNEL_ARGUMENT = 0;
        constexpr unsigned int FIELD_FLATTEN_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int RELEASE_FLATTENERS_TOTAL_ARGUMENTS = 0;

        constexpr unsigned int DV_KERNEL_ARGUMENT = 0;
        constexpr unsigned int DV_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int DDV_KERNEL_ARGUMENT = 0;
        constexpr unsigned int DDV_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int DDDV_KERNEL_ARGUMENT = 0;
        constexpr unsigned int DDDV_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int CONNEXION_KERNEL_ARGUMENT = 0;
        constexpr unsigned int CONNEXION_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int RIEMANN_KERNEL_ARGUMENT = 0;
        constexpr unsigned int RIEMANN_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int RELEASE_TOTAL_ARGUMENTS = 0;

        constexpr unsigned int WORKING_TYPE_KERNEL_ARGUMENT = 0;
        constexpr unsigned int WORKING_TYPE_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int RELEASE_WORKING_TYPE_TOTAL_ARGUMENTS = 0;

      }   // namespace RESOURCES


    class set_params : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_params(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::PARAMETERS_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_params() = default;


        // INTERNAL API

      protected:

        //! evaluate
        std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_coordinates : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_coordinates(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::COORDINATES_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_coordinates() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_phase_flatten : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_phase_flatten(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::PHASE_FLATTEN_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_phase_flatten() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_field_flatten : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_field_flatten(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::FIELD_FLATTEN_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_field_flatten() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_dV : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_dV(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::DV_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_dV() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_ddV : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_ddV(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::DDV_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_ddV() = default;


        // INTERNAL  API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_dddV : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_dddV(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::DDDV_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_dddV() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_connexion : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_connexion(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::CONNEXION_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_connexion() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_Riemann : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_Riemann(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RIEMANN_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_Riemann() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class release : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RELEASE_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~release() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class release_flatteners : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release_flatteners(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RELEASE_FLATTENERS_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~release_flatteners() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_working_type : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_working_type(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::WORKING_TYPE_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_working_type() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class release_working_type : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release_working_type(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RELEASE_WORKING_TYPE_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~release_working_type() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class resources : public directive_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        resources(translator_data& p, resource_manager& m);

        //! destructor is default
        virtual ~resources() = default;

      };

  }


#endif //CPPTRANSPORT_MACROS_RESOURCES_H
