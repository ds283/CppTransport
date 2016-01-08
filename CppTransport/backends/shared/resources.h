//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MACROS_RESOURCES_H
#define CPPTRANSPORT_MACROS_RESOURCES_H


#include "replacement_rule_package.h"


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


    class set_params : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_params(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::PARAMETERS_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_params() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

        //! language printer
        language_printer& printer;

      };


    class set_coordinates : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_coordinates(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::COORDINATES_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_phase_flatten : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_phase_flatten(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::PHASE_FLATTEN_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_field_flatten : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_field_flatten(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::FIELD_FLATTEN_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_dV : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_dV(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::DV_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_ddV : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_ddV(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::DDV_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_dddV : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_dddV(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::DDDV_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_connexion : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_connexion(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::CONNEXION_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_Riemann : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_Riemann(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::RIEMANN_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class release : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::RELEASE_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class release_flatteners : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release_flatteners(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::RELEASE_FLATTENERS_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class set_working_type : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_working_type(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::WORKING_TYPE_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class release_working_type : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release_working_type(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n), RESOURCES::RELEASE_WORKING_TYPE_TOTAL_ARGUMENTS),
            mgr(m),
            printer(prn)
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

        //! language printer
        language_printer& printer;

      };


    class resources : public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        resources(tensor_factory& f, cse& cw, lambda_manager& lm, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~resources() = default;

      };

  }


#endif //CPPTRANSPORT_MACROS_RESOURCES_H
