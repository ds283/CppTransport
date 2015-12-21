//
// Created by David Seery on 18/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MACROS_RESOURCES_H
#define CPPTRANSPORT_MACROS_RESOURCES_H


#include "replacement_rule_package.h"


namespace macro_packages
  {

    class set_params : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_params(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_params() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_coordinates(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_coordinates() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_phase_flatten(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_phase_flatten() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_field_flatten(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_field_flatten() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_dV(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_dV() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_ddV(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_ddV() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_dddV(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_dddV() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_connexion(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_connexion() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        set_Riemann(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~set_Riemann() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        release(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~release() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
        release_flatteners(resource_manager& m, language_printer& prn)
          : mgr(m),
            printer(prn)
          {
          }

        //! destructor
        virtual ~release_flatteners() = default;


        // INTERFACE

      public:

        //! evaluate
        virtual std::string operator()(const macro_argument_list& args) override;


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
