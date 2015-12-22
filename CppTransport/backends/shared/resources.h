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

      }   // namespace RESOURCES


    class set_params : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_params(std::string n, resource_manager& m, language_printer& prn)
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_params() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::PARAMETERS_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_coordinates() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::COORDINATES_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_phase_flatten() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::PHASE_FLATTEN_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_field_flatten() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::FIELD_FLATTEN_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_dV() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::DV_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_ddV() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::DDV_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_dddV() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::DDDV_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_connexion() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::CONNEXION_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_Riemann() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::RIEMANN_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~release() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::RELEASE_TOTAL_ARGUMENTS); }


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
          : replacement_rule_simple(std::move(n)),
            mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~release_flatteners() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


        // INTERFACE -- QUERY FOR DATA

      public:

        //! get number of arguments associated with this macro
        virtual unsigned int get_number_args() const override { return(RESOURCES::RELEASE_FLATTENERS_TOTAL_ARGUMENTS); }


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
        resources(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~resources() = default;


        // INTERFACE

      public:

        //! return index- macros package
        const std::vector<index_rule>  get_index_rules() override;


        // INTERNAL DATA

      private:

        //! cache reference to resource manager
        resource_manager& mgr;

      };

  }


#endif //CPPTRANSPORT_MACROS_RESOURCES_H
