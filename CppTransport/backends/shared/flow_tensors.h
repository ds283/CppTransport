//
// Created by David Seery on 04/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
#define CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H


#include "replacement_rule_package.h"
#include "cse_map_field_indices.h"


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
        : replacement_rule_simple(std::move(n), EPSILON_TOTAL_ARGUMENTS),
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


    class replace_parameter : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_parameter(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : replacement_rule_index(std::move(n), PARAMETER_TOTAL_ARGUMENTS, PARAMETER_TOTAL_INDICES, index_class::parameter),
            cse_worker(cw),
            printer(prn),
            shared(f.get_shared_resources())
          {
          }

        //! destructor
        virtual ~replace_parameter() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return(this->shared.roll_parameters() ? unroll_behaviour::allow : unroll_behaviour::force); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args, const assignment_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

      };


    class replace_field : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_field(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : replacement_rule_index(std::move(n), FIELD_TOTAL_ARGUMENTS, FIELD_TOTAL_INDICES, index_class::field_only),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources())
          {
          }

        //! destructor
        virtual ~replace_field() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return(this->shared.roll_parameters() ? unroll_behaviour::allow : unroll_behaviour::force); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args, const assignment_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

      };


    class replace_coordinate : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_coordinate(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : replacement_rule_index(std::move(n), COORDINATE_TOTAL_ARGUMENTS, COORDINATE_TOTAL_INDICES, index_class::full),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources())
          {
          }

        //! destructor
        virtual ~replace_coordinate() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return(this->shared.roll_parameters() ? unroll_behaviour::allow : unroll_behaviour::force); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string evaluate(const macro_argument_list& args, const assignment_list& indices) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

      };


    class replace_SR_velocity : public cse_map_field1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_SR_velocity(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : cse_map_field1(std::move(n), SR_VELOCITY_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources())
          {
            SR_velocity_tensor = f.make_SR_velocity(prn);
          }

        //! destructor
        virtual ~replace_SR_velocity() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->SR_velocity_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! SR_velocity object
        std::unique_ptr<SR_velocity> SR_velocity_tensor;

      };


    class replace_dV : public cse_map_field1
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_dV(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : cse_map_field1(std::move(n), DV_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources())
          {
            dV_tensor = f.make_dV(prn);
          }

        //! destructor
        virtual ~replace_dV() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->dV_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! dV object
        std::unique_ptr<dV> dV_tensor;

      };


    class replace_ddV : public cse_map_field2
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_ddV(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : cse_map_field2(std::move(n), DDV_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources())
          {
            ddV_tensor = f.make_ddV(prn);
          }

        //! destructor
        virtual ~replace_ddV() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->ddV_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! ddV object
        std::unique_ptr<ddV> ddV_tensor;

      };


    class replace_dddV : public cse_map_field3
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        replace_dddV(std::string n, tensor_factory& f, cse& cw, language_printer& prn)
          : cse_map_field3(std::move(n), DDDV_TOTAL_ARGUMENTS, f.get_shared_resources().get_number_parameters(), f.get_shared_resources().get_number_field()),
            printer(prn),
            cse_worker(cw),
            shared(f.get_shared_resources())
          {
            dddV_tensor = f.make_dddV(prn);
          }

        //! destructor
        virtual ~replace_dddV() = default;


        // INTERFACE

      public:

        //! determine unroll status
        enum unroll_behaviour get_unroll() const override { return this->dddV_tensor->get_unroll(); }


        // INTERNAL API

      protected:

        //! evaluate
        virtual void pre_evaluate(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! reference to shared resource
        shared_resources& shared;

        //! CSE worker
        cse& cse_worker;

        //! language printer
        language_printer& printer;

        //! dddV object
        std::unique_ptr<dddV> dddV_tensor;

      };


    class flow_tensors : public replacement_rule_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        flow_tensors(tensor_factory& f, cse& cw, translator_data& p, language_printer& prn);

        //! destructor is default
        virtual ~flow_tensors() = default;


        // INTERNAL DATA

      private:

      };

  } // namespace macro_packages


#endif //CPPTRANSPORT_MACROPACKAGE_FLOW_TENSORS_H
