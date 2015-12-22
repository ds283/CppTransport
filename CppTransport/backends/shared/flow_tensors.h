//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
#define CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H


#include "replacement_rule_package.h"


namespace macro_packages
  {

    constexpr unsigned int POTENTIAL_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int HUBBLESQ_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int EPSILON_TOTAL_ARGUMENTS = 0;

    constexpr unsigned int PARAMETER_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int PARAMETER_TOTAL_INDICES = 1;

    constexpr unsigned int FIELD_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int FIELD_TOTAL_INDICES = 1;

    constexpr unsigned int COORDINATE_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int COORDINATE_TOTAL_INDICES = 1;

    constexpr unsigned int SR_VELOCITY_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int SR_VELOCITY_TOTAL_INDICES = 1;

    constexpr unsigned int DV_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DV_TOTAL_INDICES = 1;

    constexpr unsigned int DDV_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DDV_TOTAL_INDICES = 2;

    constexpr unsigned int DDDV_TOTAL_ARGUMENTS = 0;
    constexpr unsigned int DDDV_TOTAL_INDICES = 3;


    class replace_V : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_V(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : replacement_rule_simple(std::move(n), POTENTIAL_TOTAL_ARGUMENTS),
            cse_worker(cw)
          {
            Hubble_obj = f.make_Hubble(prn);
          }

        //! destructor
        virtual ~replace_V() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

      };


    class replace_Hsq : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_Hsq(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
        : replacement_rule_simple(std::move(n), HUBBLESQ_TOTAL_ARGUMENTS),
          cse_worker(cw)
          {
            Hubble_obj = f.make_Hubble(prn);
          }

        //! destructor
        virtual ~replace_Hsq() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

      };


    class replace_eps : public replacement_rule_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_eps(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
        : replacement_rule_simple(std::move(n), PARAMETER_TOTAL_ARGUMENTS),
          cse_worker(cw)
          {
            Hubble_obj = f.make_Hubble(prn);
          }

        //! destructor
        virtual ~replace_eps() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! Hubble object
        std::unique_ptr<Hubble> Hubble_obj;

        //! CSE worker
        cse& cse_worker;

      };


    class flow_tensors : public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        flow_tensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~flow_tensors() = default;


        // INTERFACE

      public:

        const std::vector<index_rule>  get_index_rules();


        // INTERNAL API

      protected:

        std::string replace_parameter(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_field(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::string replace_coordinate(const macro_argument_list& args, const assignment_list& indices, cse_map* map);

        std::unique_ptr<cse_map> pre_sr_velocity(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_dV(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_ddV(const macro_argument_list& args);

        std::unique_ptr<cse_map> pre_dddV(const macro_argument_list& args);


        // INTERNAL DATA

      private:

        //! dV object
        std::unique_ptr<dV> dV_tensor;

        //! ddV object
        std::unique_ptr<ddV> ddV_tensor;

        //! dddV object
        std::unique_ptr<dddV> dddV_tensor;

        //! SR_velocity object
        std::unique_ptr<SR_velocity> SR_velocity_tensor;

        //! reference to shared resource
        shared_resources& shared;

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
