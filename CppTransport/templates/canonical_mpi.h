// backend = cpp, minver = 201801, lagrangian = canonical
//
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This template file is part of the CppTransport platform.
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
// As a special exception, you may create a larger work that contains
// part or all of this template file and distribute that work
// under terms of your choice.  Alternatively, if you modify or redistribute
// this template file itself, you may (at your option) remove this
// special exception, which will cause the template and the resulting
// CppTransport output files to be licensed under the GNU General Public
// License without this special exception.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $TOOL $VERSION
//
// '$HEADER' generated from '$SOURCE'
// processed on $DATE

// MPI implementation

#ifndef $GUARD   // avoid multiple inclusion
#define $GUARD

#include "transport-runtime/transport.h"

#include "$CORE"

namespace transport
  {

    $PHASE_FLATTEN{FLATTEN}
    $FIELD_FLATTEN{FIELDS_FLATTEN}

    $WORKING_TYPE{number}

    $IF{fast}

      $SET[MN]{U2_DECLARE, "const auto __u2_$M_$N"}

      $SET[MN]{U2_k1_DECLARE, "const auto __u2_k1_$M_$N"}
      $SET[MN]{U2_k2_DECLARE, "const auto __u2_k2_$M_$N"}
      $SET[MN]{U2_k3_DECLARE, "const auto __u2_k3_$M_$N"}

      $SET[LMN]{U3_k1k2k3_DECLARE, "const auto __u3_k1k2k3_$L_$M_$N"}
      $SET[LMN]{U3_k2k1k3_DECLARE, "const auto __u3_k2k1k3_$L_$M_$N"}
      $SET[LMN]{U3_k3k1k2_DECLARE, "const auto __u3_k3k1k2_$L_$M_$N"}

      $SET[MN]{U2_CONTAINER, "__u2_$M_$N"}

      $SET[MN]{U2_k1_CONTAINER, "__u2_k1_$M_$N"}
      $SET[MN]{U2_k2_CONTAINER, "__u2_k2_$M_$N"}
      $SET[MN]{U2_k3_CONTAINER, "__u2_k3_$M_$N"}

      $SET[LMN]{U3_k1k2k3_CONTAINER, "__u3_k1k2k3_$L_$M_$N"}
      $SET[LMN]{U3_k2k1k3_CONTAINER, "__u3_k2k1k3_$L_$M_$N"}
      $SET[LMN]{U3_k3k1k2_CONTAINER, "__u3_k3k1k2_$L_$M_$N"}

    $ELSE

      $SET[MN]{U2_DECLARE, "__u2[FLATTEN($M,$N)]"}

      $SET[MN]{U2_k1_DECLARE, "__u2_k1[FLATTEN($M,$N)]"}
      $SET[MN]{U2_k2_DECLARE, "__u2_k2[FLATTEN($M,$N)]"}
      $SET[MN]{U2_k3_DECLARE, "__u2_k3[FLATTEN($M,$N)]"}

      $SET[LMN]{U3_k1k2k3_DECLARE, "__u3_k1k2k3[FLATTEN($L,$M,$N)]"}
      $SET[LMN]{U3_k2k1k3_DECLARE, "__u3_k2k1k3[FLATTEN($L,$M,$N)]"}
      $SET[LMN]{U3_k3k1k2_DECLARE, "__u3_k3k1k2[FLATTEN($L,$M,$N)]"}

      $SET[MN]{U2_CONTAINER, "__u2[FLATTEN($M,$N)]"}

      $SET[MN]{U2_k1_CONTAINER, "__u2_k1[FLATTEN($M,$N)]"}
      $SET[MN]{U2_k2_CONTAINER, "__u2_k2[FLATTEN($M,$N)]"}
      $SET[MN]{U2_k3_CONTAINER, "__u2_k3[FLATTEN($M,$N)]"}

      $SET[LMN]{U3_k1k2k3_CONTAINER, "__u3_k1k2k3[FLATTEN($L,$M,$N)]"}
      $SET[LMN]{U3_k2k1k3_CONTAINER, "__u3_k2k1k3[FLATTEN($L,$M,$N)]"}
      $SET[LMN]{U3_k3k1k2_CONTAINER, "__u3_k3k1k2[FLATTEN($L,$M,$N)]"}

    $ENDIF
    
    namespace $MODEL_pool
      {
        const static std::string backend = "MPI";
        const static std::string pert_stepper = "$PERT_STEPPER";
        const static std::string back_stepper = "$BACKG_STEPPER";
      }


    // *********************************************************************************************


    // CLASS FOR $MODEL '*_mpi', ie., an MPI-based implementation
    template <typename number = default_number_type, typename StateType = std::vector<number> >
    class $MODEL_mpi : public $MODEL<number>
      {
        
        // TYPES
        
      public:

        //! expose floating point value type
        using value_type = number;
        
        //! expose 2pf/3pf integration state type
        using twopf_state = StateType;
        using threepf_state = StateType;

        
        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        $MODEL_mpi(local_environment& e, argument_cache& a)
          : $MODEL<number>(e, a)
          {
#ifdef CPPTRANSPORT_INSTRUMENT
            twopf_setup_timer.stop();
            twopf_u_tensor_timer.stop();
            twopf_transport_eq_timer.stop();

            threepf_setup_timer.stop();
            threepf_u_tensor_timer.stop();
            threepf_transport_eq_timer.stop();

            twopf_items = 0;
            threepf_items = 0;

            twopf_invokations = 0;
            threepf_invokations = 0;
#endif
          }

        // destructor is default unless instrumented
#ifdef CPPTRANSPORT_INSTRUMENT
        //! instrumented destructor
        ~$MODEL_mpi()
          {
            if(this->twopf_items > 0)
              {
                std::cout << '\n' << "TWOPF INSTRUMENTATION REPORT" << '\n';
                std::cout << "* TOTALS" << '\n';
                std::cout << "  -- setup: " << format_time(twopf_setup_timer.elapsed().user) << " user, " << format_time(twopf_setup_timer.elapsed().system) << " system, " << format_time(twopf_setup_timer.elapsed().wall) << " wall" << '\n';
                std::cout << "  -- U tensors: " << format_time(twopf_u_tensor_timer.elapsed().user) << " user, " << format_time(twopf_u_tensor_timer.elapsed().system) << " system, " << format_time(twopf_u_tensor_timer.elapsed().wall) << " wall" << '\n';
                std::cout << "  -- transport equations: " << format_time(twopf_transport_eq_timer.elapsed().user) << " user, " << format_time(twopf_transport_eq_timer.elapsed().system) << " system, " << format_time(twopf_transport_eq_timer.elapsed().wall) << " wall" << '\n';
                std::cout << "* PER ITEM" << '\n';
                std::cout << "  -- setup: " << format_time(twopf_setup_timer.elapsed().user/this->twopf_items) << " user, " << format_time(twopf_setup_timer.elapsed().system/this->twopf_items) << " system, " << format_time(twopf_setup_timer.elapsed().wall/this->twopf_items) << " wall" << '\n';
                std::cout << "  -- U tensors: " << format_time(twopf_u_tensor_timer.elapsed().user/this->twopf_items) << " user, " << format_time(twopf_u_tensor_timer.elapsed().system/this->twopf_items) << " system, " << format_time(twopf_u_tensor_timer.elapsed().wall/this->twopf_items) << " wall" << '\n';
                std::cout << "  -- transport equations: " << format_time(twopf_transport_eq_timer.elapsed().user/this->twopf_items) << " user, " << format_time(twopf_transport_eq_timer.elapsed().system/this->twopf_items) << " system, " << format_time(twopf_transport_eq_timer.elapsed().wall/this->twopf_items) << " wall" << '\n';
                std::cout << "* PER INVOKATION" << '\n';
                std::cout << "  -- setup: " << format_time(twopf_setup_timer.elapsed().user/this->twopf_invokations) << " user, " << format_time(twopf_setup_timer.elapsed().system/this->twopf_invokations) << " system, " << format_time(twopf_setup_timer.elapsed().wall/this->twopf_invokations) << " wall" << '\n';
                std::cout << "  -- U tensors: " << format_time(twopf_u_tensor_timer.elapsed().user/this->twopf_invokations) << " user, " << format_time(twopf_u_tensor_timer.elapsed().system/this->twopf_invokations) << " system, " << format_time(twopf_u_tensor_timer.elapsed().wall/this->twopf_invokations) << " wall" << '\n';
                std::cout << "  -- transport equations: " << format_time(twopf_transport_eq_timer.elapsed().user/this->twopf_invokations) << " user, " << format_time(twopf_transport_eq_timer.elapsed().system/this->twopf_invokations) << " system, " << format_time(twopf_transport_eq_timer.elapsed().wall/this->twopf_invokations) << " wall" << '\n';
              }

            if(this->threepf_items > 0)
              {
                std::cout << '\n' << "THREEPF INSTRUMENTATION REPORT" << '\n';
                std::cout << "* TOTALS" << '\n';
                std::cout << "  -- setup: " << format_time(threepf_setup_timer.elapsed().user) << " user, " << format_time(threepf_setup_timer.elapsed().system) << " system, " << format_time(threepf_setup_timer.elapsed().wall) << " wall" << '\n';
                std::cout << "  -- U tensors: " << format_time(threepf_u_tensor_timer.elapsed().user) << " user, " << format_time(threepf_u_tensor_timer.elapsed().system) << " system, " << format_time(threepf_u_tensor_timer.elapsed().wall) << " wall" << '\n';
                std::cout << "  -- transport equations: " << format_time(threepf_transport_eq_timer.elapsed().user) << " user, " << format_time(threepf_transport_eq_timer.elapsed().system) << " system, " << format_time(threepf_transport_eq_timer.elapsed().wall) << " wall" << '\n';
                std::cout << "* PER ITEM" << '\n';
                std::cout << "  -- setup: " << format_time(threepf_setup_timer.elapsed().user/this->threepf_items) << " user, " << format_time(threepf_setup_timer.elapsed().system/this->threepf_items) << " system, " << format_time(threepf_setup_timer.elapsed().wall/this->threepf_items) << " wall" << '\n';
                std::cout << "  -- U tensors: " << format_time(threepf_u_tensor_timer.elapsed().user/this->threepf_items) << " user, " << format_time(threepf_u_tensor_timer.elapsed().system/this->threepf_items) << " system, " << format_time(threepf_u_tensor_timer.elapsed().wall/this->threepf_items) << " wall" << '\n';
                std::cout << "  -- transport equations: " << format_time(threepf_transport_eq_timer.elapsed().user/this->threepf_items) << " user, " << format_time(threepf_transport_eq_timer.elapsed().system/this->threepf_items) << " system, " << format_time(threepf_transport_eq_timer.elapsed().wall/this->threepf_items) << " wall" << '\n';
                std::cout << "* PER INVOKATION" << '\n';
                std::cout << "  -- setup: " << format_time(threepf_setup_timer.elapsed().user/this->threepf_invokations) << " user, " << format_time(threepf_setup_timer.elapsed().system/this->threepf_invokations) << " system, " << format_time(threepf_setup_timer.elapsed().wall/this->threepf_invokations) << " wall" << '\n';
                std::cout << "  -- U tensors: " << format_time(threepf_u_tensor_timer.elapsed().user/this->threepf_invokations) << " user, " << format_time(threepf_u_tensor_timer.elapsed().system/this->threepf_invokations) << " system, " << format_time(threepf_u_tensor_timer.elapsed().wall/this->threepf_invokations) << " wall" << '\n';
                std::cout << "  -- transport equations: " << format_time(threepf_transport_eq_timer.elapsed().user/this->threepf_invokations) << " user, " << format_time(threepf_transport_eq_timer.elapsed().system/this->threepf_invokations) << " system, " << format_time(threepf_transport_eq_timer.elapsed().wall/this->threepf_invokations) << " wall" << '\n';
              }
          }
#else
        //! uninstrumented destructor
        virtual ~$MODEL_mpi() = default;
#endif


        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        //! return backend name
        const std::string& get_backend() const override { return($MODEL_pool::backend); }

        //! return background stepper name
        const std::string& get_back_stepper() const override { return($MODEL_pool::back_stepper); }

        //! return perturbations stepper name
        const std::string& get_pert_stepper() const override { return($MODEL_pool::pert_stepper); }

        //! return background tolerances
        std::pair< double, double > get_back_tol() const override { return std::make_pair($BACKG_ABS_ERR, $BACKG_REL_ERR); }

        //! return perturbations tolerances
        std::pair< double, double > get_pert_tol() const override { return std::make_pair($PERT_ABS_ERR, $PERT_REL_ERR); }


        // BACKEND INTERFACE

      public:

        //! set up a context
        compute_context backend_get_context() override;

        //! report backend type
        worker_type get_backend_type() override;

        //! report backend memory capacity
        unsigned int get_backend_memory() override;

        //! report backend priority
        unsigned int get_backend_priority() override;

        //! integrate background and 2-point function on the CPU
        void backend_process_queue(device_queue_manager<twopf_kconfig_record>& work, const twopf_db_task<number>* tk,
                                   twopf_batcher<number>& batcher, bool silent = false) override;

        //! integrate background, 2-point function and 3-point function on the CPU
        void backend_process_queue(device_queue_manager<threepf_kconfig_record>& work, const threepf_task<number>* tk,
                                   threepf_batcher<number>& batcher, bool silent = false) override;

        //! report 2pf integrator state size
        unsigned int backend_twopf_state_size() const override { return($MODEL_pool::twopf_state_size); }

        //! report 3pf integrator state size
        unsigned int backend_threepf_state_size() const override { return($MODEL_pool::threepf_state_size); }

        //! query whether backend support collection of per-configuration statistics
        virtual bool supports_per_configuration_statistics() const override { return(true); }


        // INTERNAL API

      public:

        //! integrate a single 2pf k-configuration
        template <typename BatchObject>
        void twopf_kmode(const twopf_kconfig_record& kconfig, const twopf_db_task<number>* tk,
                         BatchObject& batcher, unsigned int refinement_level);

        //! integrate a single 3pf k-configuration
        template <typename BatchObject>
        void threepf_kmode(const threepf_kconfig_record&, const threepf_task<number>* tk,
                           BatchObject& batcher, unsigned int refinement_level);

      protected:

        //! populate initial values for a 2pf configuration
        void populate_twopf_ic(twopf_state& x, unsigned int start, double kmode, double Ninit,
                               const twopf_db_task<number>* tk, const std::vector<number>& ic, double k_normalize=1.0, bool imaginary = false);

        //! populate initial values for a tensor 2pf configuration
        void populate_tensor_ic(twopf_state& x, unsigned int start, double kmode, double Ninit,
                                const twopf_db_task<number>* tk, const std::vector<number>& ic, double k_normalize=1.0);

        //! populate initial values for a 3pf configuration
        void populate_threepf_ic(threepf_state& x, unsigned int start, const threepf_kconfig& kconfig,
                                 double Ninit, const twopf_db_task<number>* tk, const std::vector<number>& ic, double k_normalize=1.0);


        // INTERNAL DATA

      private:

#ifdef CPPTRANSPORT_INSTRUMENT
        boost::timer::cpu_timer twopf_setup_timer;
        boost::timer::cpu_timer twopf_u_tensor_timer;
        boost::timer::cpu_timer twopf_transport_eq_timer;

        unsigned int twopf_items;
        unsigned int twopf_invokations;

        boost::timer::cpu_timer threepf_setup_timer;
        boost::timer::cpu_timer threepf_u_tensor_timer;
        boost::timer::cpu_timer threepf_transport_eq_timer;

        unsigned int threepf_items;
        unsigned int threepf_invokations;
#endif

      };


    // integration - 2pf functor
    template <typename Model>
    class $MODEL_mpi_twopf_functor
      {
      
      public:
        
        //! inherit number type from Model
        using number = typename Model::value_type;
        
        //! inherit state type from model
        using twopf_state = typename Model::twopf_state;

        
      public:

        $MODEL_mpi_twopf_functor(const twopf_db_task<number>* tk, const twopf_kconfig& k
#ifdef CPPTRANSPORT_INSTRUMENT
          ,
            boost::timer::cpu_timer& st,
            boost::timer::cpu_timer& ut,
            boost::timer::cpu_timer& tt,
            unsigned int& in
#endif
        )
          : __params(tk->get_params()),
            __Mp(tk->get_params().get_Mp()),
            __N_horizon_exit(tk->get_N_horizon_crossing()),
            __astar_normalization(tk->get_astar_normalization()),
            __config(k),
            __k(k.k_comoving),

            $IF{!fast}
              __u2(nullptr),
              __dV(nullptr),
              __ddV(nullptr),
            $ENDIF

            __raw_params(nullptr)
#ifdef CPPTRANSPORT_INSTRUMENT
            ,
                __setup_timer(st),
                __u_tensor_timer(ut),
                __transport_eq_timer(tt),
                __invokations(in)
#endif
          {
          }

        void set_up_workspace()
          {
            $IF{!fast}
              $RESOURCE_RELEASE
              this->__u2 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];

              this->__dV = new number[$NUMBER_FIELDS];
              this->__ddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
            $ENDIF

            this->__raw_params = new number[$NUMBER_PARAMS];
    
            const auto& __pvector = __params.get_vector();
            this->__raw_params[$1] = __pvector[$1];
          }

        void close_down_workspace()
          {
            $IF{!fast}
              delete[] this->__u2;

              delete[] this->__dV;
              delete[] this->__ddV;
            $ENDIF

            delete[] this->__raw_params;
          }

        void operator()(const twopf_state& __x, twopf_state& __dxdt, number __t);

        // adjust horizon exit time, given an initial time N_init which we wish to move to zero
        void rebase_horizon_exit_time(double N_init) { this->__N_horizon_exit -= N_init; }


        // INTERNAL DATA

      private:

        const parameters<number>& __params;

        number __Mp;

        double __N_horizon_exit;

        double __astar_normalization;

        const twopf_kconfig __config;

        const double __k;

        // manage memory ourselves, rather than via an STL container, for maximum performance
        // also avoids copying overheads (the Boost odeint library copies the functor by value)

        $IF{!fast}
          number* __u2;

          number* __dV;
          number* __ddV;
        $ENDIF

        number* __raw_params;

#ifdef CPPTRANSPORT_INSTRUMENT
        boost::timer::cpu_timer& __setup_timer;
        boost::timer::cpu_timer& __u_tensor_timer;
        boost::timer::cpu_timer& __transport_eq_timer;
        unsigned int& __invokations;
#endif

      };


    // integration - observer object for 2pf
    template <typename Model, typename BatchObject>
    class $MODEL_mpi_twopf_observer: public twopf_singleconfig_batch_observer<typename Model::value_type, BatchObject>
      {
  
      public:
    
        //! inherit number type from Model
        using number = typename Model::value_type;
    
        //! inherit state type from model
        using twopf_state = typename Model::twopf_state;

        
      public:

        $MODEL_mpi_twopf_observer(BatchObject& b, const twopf_kconfig_record& c,
                                  double t_ics, const time_config_database& t)
          : twopf_singleconfig_batch_observer<number, BatchObject>
              (b, c, t_ics, t,
               $MODEL_pool::backg_size, $MODEL_pool::tensor_size, $MODEL_pool::twopf_size,
               $MODEL_pool::backg_start, $MODEL_pool::tensor_start, $MODEL_pool::twopf_start)
          {
          }

        void operator()(const twopf_state& x, number t);

      };


    // integration - 3pf functor
    template <typename Model>
    class $MODEL_mpi_threepf_functor
      {
  
      public:
    
        //! inherit number type from Model
        using number = typename Model::value_type;
    
        //! inherit state type from model
        using threepf_state = typename Model::threepf_state;

        
      public:

        $MODEL_mpi_threepf_functor(const twopf_db_task<number>* tk, const threepf_kconfig& k
#ifdef CPPTRANSPORT_INSTRUMENT
          ,
          boost::timer::cpu_timer& st,
          boost::timer::cpu_timer& ut,
          boost::timer::cpu_timer& tt,
          unsigned int& in
#endif
        )
          : __params(tk->get_params()),
            __Mp(tk->get_params().get_Mp()),
            __N_horizon_exit(tk->get_N_horizon_crossing()),
            __astar_normalization(tk->get_astar_normalization()),
            __config(k),
            __k1(k.k1_comoving),
            __k2(k.k2_comoving),
            __k3(k.k3_comoving),

            $IF{!fast}
              __u2_k1(nullptr),
              __u2_k2(nullptr),
              __u2_k3(nullptr),
              __u3_k1k2k3(nullptr),
              __u3_k2k1k3(nullptr),
              __u3_k3k1k2(nullptr),
              __dV(nullptr),
              __ddV(nullptr),
              __dddV(nullptr),
            $ENDIF

            __raw_params(nullptr)
#ifdef CPPTRANSPORT_INSTRUMENT
            ,
            __setup_timer(st),
            __u_tensor_timer(ut),
            __transport_eq_timer(tt),
            __invokations(in)
#endif
          {
          }

        void set_up_workspace()
          {
            $IF{!fast}
              $RESOURCE_RELEASE
              this->__u2_k1 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];
              this->__u2_k2 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];
              this->__u2_k3 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];

              this->__u3_k1k2k3 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];
              this->__u3_k2k1k3 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];
              this->__u3_k3k1k2 = new number[2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS * 2*$NUMBER_FIELDS];

              this->__dV = new number[$NUMBER_FIELDS];
              this->__ddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
              this->__dddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
            $ENDIF

            this->__raw_params = new number[$NUMBER_PARAMS];
    
            const auto& __pvector = __params.get_vector();
            this->__raw_params[$1] = __pvector[$1];
          }

        void close_down_workspace()
          {
            $IF{!fast}
              delete[] this->__u2_k1;
              delete[] this->__u2_k2;
              delete[] this->__u2_k3;

              delete[] this->__u3_k1k2k3;
              delete[] this->__u3_k2k1k3;
              delete[] this->__u3_k3k1k2;

              delete[] this->__dV;
              delete[] this->__ddV;
              delete[] this->__dddV;
            $ENDIF

            delete[] this->__raw_params;
          }

        void operator()(const threepf_state& __x, threepf_state& __dxdt, number __dt);

        // adjust horizon exit time, given an initial time N_init which we wish to move to zero
        void rebase_horizon_exit_time(double N_init) { this->__N_horizon_exit -= N_init; }

      private:

        const parameters<number>& __params;

        number __Mp;

        double __N_horizon_exit;

        double __astar_normalization;

        const threepf_kconfig __config;

        const double __k1;
        const double __k2;
        const double __k3;

        // manage memory ourselves, rather than via an STL container, for maximum performance
        // also avoids copying overheads (the Boost odeint library copies the functor by value)

        $IF{!fast}
          number* __u2_k1;
          number* __u2_k2;
          number* __u2_k3;

          number* __u3_k1k2k3;
          number* __u3_k2k1k3;
          number* __u3_k3k1k2;

          number* __dV;
          number* __ddV;
          number* __dddV;
        $ENDIF

        number* __raw_params;

#ifdef CPPTRANSPORT_INSTRUMENT
        boost::timer::cpu_timer& __setup_timer;
        boost::timer::cpu_timer& __u_tensor_timer;
        boost::timer::cpu_timer& __transport_eq_timer;
        unsigned int& __invokations;
#endif

      };


    // integration - observer object for 3pf
    template <typename Model, typename BatchObject>
    class $MODEL_mpi_threepf_observer: public threepf_singleconfig_batch_observer<typename Model::value_type, BatchObject>
      {
  
      public:
    
        //! inherit number type from Model
        using number = typename Model::value_type;
    
        //! inherit state type from model
        using threepf_state = typename Model::threepf_state;

        
      public:
        $MODEL_mpi_threepf_observer(BatchObject& b, const threepf_kconfig_record& c,
                                    double t_ics, const time_config_database& t)
          : threepf_singleconfig_batch_observer<number, BatchObject>
              (b, c, t_ics, t,
               $MODEL_pool::backg_size, $MODEL_pool::tensor_size,
               $MODEL_pool::twopf_size, $MODEL_pool::threepf_size,
               $MODEL_pool::backg_start,
               $MODEL_pool::tensor_k1_start, $MODEL_pool::tensor_k2_start, $MODEL_pool::tensor_k3_start,
               $MODEL_pool::twopf_re_k1_start, $MODEL_pool::twopf_im_k1_start,
               $MODEL_pool::twopf_re_k2_start, $MODEL_pool::twopf_im_k2_start,
               $MODEL_pool::twopf_re_k3_start, $MODEL_pool::twopf_im_k3_start,
               $MODEL_pool::threepf_start)
          {
          }

        void operator()(const threepf_state& x, number t);

      };


    // BACKEND INTERFACE


    // generate a compute context
    template <typename number, typename StateType>
    compute_context $MODEL_mpi<number, StateType>::backend_get_context(void)
      {
        compute_context ctx;

        // set up just one device
        ctx.add_device($MODEL_pool::backend);

        return(ctx);
      }


    template <typename number, typename StateType>
    worker_type $MODEL_mpi<number, StateType>::get_backend_type(void)
      {
        return(worker_type::cpu);
      }


    template <typename number, typename StateType>
    unsigned int $MODEL_mpi<number, StateType>::get_backend_memory(void)
      {
        return(0);
      }


    template <typename number, typename StateType>
    unsigned int $MODEL_mpi<number, StateType>::get_backend_priority(void)
      {
        return(1);
      }


    // process work queue for twopf
    template <typename number, typename StateType>
    void $MODEL_mpi<number, StateType>::backend_process_queue(device_queue_manager<twopf_kconfig_record>& work,
                                                              const twopf_db_task<number>* tk,
                                                              twopf_batcher<number>& batcher, bool silent)
      {
        // set batcher to delayed flushing mode so that we have a chance to unwind failed integrations
        batcher.set_flush_mode(generic_batcher::flush_mode::flush_delayed);

        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
            << "** MPI compute backend processing twopf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << work_msg.str();
//        std::cerr << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $PERT_ABS_ERR, $PERT_REL_ERR, $PERT_STEP_SIZE, "$PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const device_queue_manager<twopf_kconfig_record>::device_queue queues = work[0];

        // we expect only one queue on this device
        assert(queues.size() == 1);
        const device_queue_manager<twopf_kconfig_record>::device_work_list list = queues[0];

        for(unsigned int i = 0; i < list.size(); ++i)
          {
            bool success = false;
            unsigned int refinement_level = 0;

            while(!success)
            try
              {
                // write the time history for this k-configuration
                this->twopf_kmode(list[i], tk, batcher, refinement_level);    // logging and report of successful integration are wrapped up in the observer stop_timers() method
                success = true;
               }
            catch(std::overflow_error& xe)
              {
                // unwind any batched results before trying again with a refined mesh
                if(refinement_level == 0) batcher.report_refinement();
                batcher.unbatch(list[i]->serial);
                refinement_level++;

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::warning)
                    << "** " << CPPTRANSPORT_RETRY_CONFIG << " " << list[i]->serial << " (" << i+1
                    << " " CPPTRANSPORT_OF << " " << list.size() << "), "
                    << CPPTRANSPORT_REFINEMENT_LEVEL << " = " << refinement_level
                    << " (" << CPPTRANSPORT_REFINEMENT_INTERNAL << xe.what() << ")";
              }
            catch(runtime_exception& xe)
              {
                batcher.report_integration_failure(list[i]->serial);
                batcher.unbatch(list[i]->serial);
                success = true;

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::error)
                    << "!! " CPPTRANSPORT_FAILED_CONFIG << " " << list[i]->serial << " (" << i+1
                    << " " CPPTRANSPORT_OF << " " << list.size() << ") | " << list[i];
              }
          }
      }


    template <typename number, typename StateType>
    template <typename BatchObject>
    void $MODEL_mpi<number, StateType>::twopf_kmode(const twopf_kconfig_record& kconfig,
                                                    const twopf_db_task<number>* tk, BatchObject& batcher,
                                                    unsigned int refinement_level)
      {
        DEFINE_INDEX_TOOLS

        if(refinement_level > tk->get_max_refinements()) throw runtime_exception(exception_type::REFINEMENT_FAILURE, CPPTRANSPORT_REFINEMENT_TOO_DEEP);

        // get time configuration database
        const time_config_database time_db = tk->get_time_config_database(*kconfig);

        // set up a functor to observe the integration
        // this also starts the timers running, so we do it as early as possible
        $MODEL_mpi_twopf_observer< $MODEL_mpi<number, StateType>, BatchObject > obs(batcher, kconfig, tk->get_initial_time(*kconfig), time_db);

        // set up a functor to evolve this system
        $MODEL_mpi_twopf_functor< $MODEL_mpi<number, StateType> > rhs(tk, *kconfig
#ifdef CPPTRANSPORT_INSTRUMENT
          ,
            this->twopf_setup_timer, this->twopf_u_tensor_timer, this->twopf_transport_eq_timer, this->twopf_invokations
#endif
          );
        rhs.set_up_workspace();

        // set up a state vector
        twopf_state x;
        x.resize($MODEL_pool::twopf_state_size);

        // fix initial conditions - background
        const std::vector<number> ics = tk->get_ics_vector(*kconfig);
        x[$MODEL_pool::backg_start + FLATTEN($A)] = ics[$A];

        if(batcher.is_collecting_initial_conditions())
          {
            const std::vector<number> ics_1 = tk->get_ics_exit_vector(*kconfig);
            double t_exit = tk->get_ics_exit_time(*kconfig);
            batcher.push_ics(kconfig->serial, t_exit, ics_1);
          }
    
        // observers expect all correlation functions to be dimensionless and rescaled by the same factors
    
        // fix initial conditions - tensors (use dimensionless correlation functions)
        this->populate_tensor_ic(x, $MODEL_pool::tensor_start, kconfig->k_comoving, *(time_db.value_begin()), tk, ics, kconfig->k_comoving);

        // fix initial conditions - 2pf (use dimensionless correlation functions)
        this->populate_twopf_ic(x, $MODEL_pool::twopf_start, kconfig->k_comoving, *(time_db.value_begin()), tk, ics, kconfig->k_comoving);

        // up to this point the calculation has been done in the user-supplied time variable.
        // However, the integrator apparently performs much better if times are measured from zero (but not yet clear why)
        // TODO: would be nice to remove this in future
        rhs.rebase_horizon_exit_time(tk->get_ics().get_N_initial());
        auto begin_iterator = time_db.value_begin(tk->get_ics().get_N_initial());
        auto end_iterator   = time_db.value_end(tk->get_ics().get_N_initial());

        using boost::numeric::odeint::integrate_times;
        
        auto stepper = $MAKE_PERT_STEPPER{twopf_state, number, number, CPPTRANSPORT_ALGEBRA_NAME(twopf_state), CPPTRANSPORT_OPERATIONS_NAME(twopf_state)};
        size_t steps = integrate_times(stepper, rhs, x, begin_iterator, end_iterator,
                                       static_cast<number>($PERT_STEP_SIZE/pow(4.0,refinement_level)), obs);

        obs.stop_timers(steps, refinement_level);
        rhs.close_down_workspace();

#ifdef CPPTRANSPORT_INSTRUMENT
        ++this->twopf_items;
#endif
      }


    // make initial conditions for each component of the 2pf
    // x           - state vector *containing* space for the 2pf (doesn't have to be entirely the 2pf)
    // start       - starting position of twopf components within the state vector
    // kmode       - *comoving normalized* wavenumber for which we will compute the twopf
    // Ninit       - initial time
    // tk          - parent task
    // ics         - initial conditions for the background fields (or fields+momenta)
    // k_normalize - used to adjust ics to be dimensionless, or just 1.0 to get raw correlation function
    // imaginary   - whether to populate using real or imaginary components of the 2pf
    template <typename number, typename StateType>
    void $MODEL_mpi<number, StateType>::populate_twopf_ic(twopf_state& x, unsigned int start, double kmode,
                                                          double Ninit, const twopf_db_task<number>* tk,
                                                          const std::vector<number>& ics, double k_normalize,
                                                          bool imaginary)
      {
        DEFINE_INDEX_TOOLS

        assert(x.size() >= start);
        assert(x.size() >= start + $MODEL_pool::twopf_size);

        // populate components of the 2pf
        x[start + FLATTEN($A,$B)] = imaginary ? this->make_twopf_im_ic($A, $B, kmode, Ninit, tk, ics, k_normalize) : this->make_twopf_re_ic($A, $B, kmode, Ninit, tk, ics, k_normalize);
      }


    // make initial conditions for the tensor twopf
    template <typename number, typename StateType>
    void $MODEL_mpi<number, StateType>::populate_tensor_ic(twopf_state& x, unsigned int start, double kmode,
                                                           double Ninit, const twopf_db_task<number>* tk,
                                                           const std::vector<number>& ics, double k_normalize)
      {
        DEFINE_INDEX_TOOLS

        assert(x.size() >= start);
        assert(x.size() >= start + $MODEL_pool::tensor_size);

        // populate components of the 2pf
        x[start + TENSOR_FLATTEN(0,0)] = this->make_twopf_tensor_ic(0, 0, kmode, Ninit, tk, ics, k_normalize);
        x[start + TENSOR_FLATTEN(0,1)] = this->make_twopf_tensor_ic(0, 1, kmode, Ninit, tk, ics, k_normalize);
        x[start + TENSOR_FLATTEN(1,0)] = this->make_twopf_tensor_ic(1, 0, kmode, Ninit, tk, ics, k_normalize);
        x[start + TENSOR_FLATTEN(1,1)] = this->make_twopf_tensor_ic(1, 1, kmode, Ninit, tk, ics, k_normalize);
      }


    // THREE-POINT FUNCTION INTEGRATION


    template <typename number, typename StateType>
    void $MODEL_mpi<number, StateType>::backend_process_queue(device_queue_manager<threepf_kconfig_record>& work,
                                                              const threepf_task<number>* tk,
                                                              threepf_batcher<number>& batcher, bool silent)
      {
        // set batcher to delayed flushing mode so that we have a chance to unwind failed integrations
        batcher.set_flush_mode(generic_batcher::flush_mode::flush_delayed);

        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
          << "** MPI compute backend processing threepf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal) << work_msg.str();
//        std::cerr << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $PERT_ABS_ERR, $PERT_REL_ERR, $PERT_STEP_SIZE, "$PERT_STEPPER");

        // get work queue for the zeroth device (should be only one device with this backend)
        assert(work.size() == 1);
        const device_queue_manager<threepf_kconfig_record>::device_queue queues = work[0];

        // we expect only one queue on this device
        assert(queues.size() == 1);
        const device_queue_manager<threepf_kconfig_record>::device_work_list list = queues[0];

        // step through the queue, solving for the three-point functions in each case
        for(unsigned int i = 0; i < list.size(); ++i)
          {
            bool success = false;
            unsigned int refinement_level = 0;

            while(!success)
            try
              {
                // write the time history for this k-configuration
                this->threepf_kmode(list[i], tk, batcher, refinement_level);    // logging and report of successful integration are wrapped up in the observer stop_timers() method
                success = true;
              }
            catch(std::overflow_error& xe)
              {
                // unwind any batched results before trying again with a refined mesh
                if(refinement_level == 0) batcher.report_refinement();
                batcher.unbatch(list[i]->serial);
                refinement_level++;

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::warning)
                    << "** " << CPPTRANSPORT_RETRY_CONFIG << " " << list[i]->serial << " (" << i+1
                    << " " << CPPTRANSPORT_OF << " " << list.size() << "), "
                    << CPPTRANSPORT_REFINEMENT_LEVEL << " = " << refinement_level
                    << " (" << CPPTRANSPORT_REFINEMENT_INTERNAL << xe.what() << ")";
              }
            catch(runtime_exception& xe)
              {
                batcher.report_integration_failure(list[i]->serial);
                batcher.unbatch(list[i]->serial);
                success = true;

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::normal)
                    << "!! " CPPTRANSPORT_FAILED_CONFIG << " " << list[i]->serial << " (" << i+1
                    << " " << CPPTRANSPORT_OF << " " << list.size() << ") | " << list[i]
                    << " (" << CPPTRANSPORT_FAILED_INTERNAL << xe.what() << ")";
              }
          }
      }


    template <typename number, typename StateType>
    template <typename BatchObject>
    void $MODEL_mpi<number, StateType>::threepf_kmode(const threepf_kconfig_record& kconfig,
                                                      const threepf_task<number>* tk,
                                                      BatchObject& batcher, unsigned int refinement_level)
      {
        DEFINE_INDEX_TOOLS

        if(refinement_level > tk->get_max_refinements()) throw runtime_exception(exception_type::REFINEMENT_FAILURE, CPPTRANSPORT_REFINEMENT_TOO_DEEP);

        // get list of time steps, and storage list
        const time_config_database time_db = tk->get_time_config_database(*kconfig);

        // set up a functor to observe the integration
        // this also starts the timers running, so we do it as early as possible
        $MODEL_mpi_threepf_observer< $MODEL_mpi<number, StateType>, BatchObject > obs(batcher, kconfig, tk->get_initial_time(*kconfig), time_db);

        // set up a functor to evolve this system
        $MODEL_mpi_threepf_functor< $MODEL_mpi<number, StateType> >  rhs(tk, *kconfig
#ifdef CPPTRANSPORT_INSTRUMENT
          ,
            this->threepf_setup_timer, this->threepf_u_tensor_timer, this->threepf_transport_eq_timer, this->threepf_invokations
#endif
          );
        rhs.set_up_workspace();

        // set up a state vector
        threepf_state x;
        x.resize($MODEL_pool::threepf_state_size);

        // fix initial conditions - background
        // use adaptive ics if enabled
        // (don't need explicit FLATTEN since it would appear on both sides)
        const std::vector<number> ics = tk->get_ics_vector(*kconfig);
        x[$MODEL_pool::backg_start + FLATTEN($A)] = ics[$A];

        if(batcher.is_collecting_initial_conditions())
          {
            const std::vector<number> ics_1 = tk->get_ics_exit_vector(*kconfig, threepf_ics_exit_type::smallest_wavenumber_exit);
            const std::vector<number> ics_2 = tk->get_ics_exit_vector(*kconfig, threepf_ics_exit_type::kt_wavenumber_exit);
            double t_exit_1 = tk->get_ics_exit_time(*kconfig, threepf_ics_exit_type::smallest_wavenumber_exit);
            double t_exit_2 = tk->get_ics_exit_time(*kconfig, threepf_ics_exit_type::kt_wavenumber_exit);
            batcher.push_ics(kconfig->serial, t_exit_1, ics_1);
            batcher.push_kt_ics(kconfig->serial, t_exit_2, ics_2);
          }

        // observers expect all correlation functions to be dimensionless and rescaled by the same factors
        
        // fix initial conditions - tensors (use dimensionless correlation functions, all rescaled by k_t to be consistent)
        this->populate_tensor_ic(x, $MODEL_pool::tensor_k1_start, kconfig->k1_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving);
        this->populate_tensor_ic(x, $MODEL_pool::tensor_k2_start, kconfig->k2_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving);
        this->populate_tensor_ic(x, $MODEL_pool::tensor_k3_start, kconfig->k3_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving);

        // fix initial conditions - real 2pfs (use dimensionless correlation functions, all rescaled by k_t to be consistent)
        this->populate_twopf_ic(x, $MODEL_pool::twopf_re_k1_start, kconfig->k1_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving, false);
        this->populate_twopf_ic(x, $MODEL_pool::twopf_re_k2_start, kconfig->k2_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving, false);
        this->populate_twopf_ic(x, $MODEL_pool::twopf_re_k3_start, kconfig->k3_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving, false);

        // fix initial conditions - imaginary 2pfs (use dimensionless correlation functions, all rescaled by k_t to be consistent)
        this->populate_twopf_ic(x, $MODEL_pool::twopf_im_k1_start, kconfig->k1_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving, true);
        this->populate_twopf_ic(x, $MODEL_pool::twopf_im_k2_start, kconfig->k2_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving, true);
        this->populate_twopf_ic(x, $MODEL_pool::twopf_im_k3_start, kconfig->k3_comoving, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving, true);

        // fix initial conditions - threepf (use dimensionless correlation functions)
        this->populate_threepf_ic(x, $MODEL_pool::threepf_start, *kconfig, *(time_db.value_begin()), tk, ics, kconfig->kt_comoving);

        // up to this point the calculation has been done in the user-supplied time variable.
        // However, the integrator apparently performs much better if times are measured from zero (but not yet clear why)
        // TODO: would be nice to remove this in future
        rhs.rebase_horizon_exit_time(tk->get_ics().get_N_initial());
        auto begin_iterator = time_db.value_begin(tk->get_ics().get_N_initial());
        auto end_iterator   = time_db.value_end(tk->get_ics().get_N_initial());
    
        using boost::numeric::odeint::integrate_times;

        auto stepper = $MAKE_PERT_STEPPER{threepf_state, number, number, CPPTRANSPORT_ALGEBRA_NAME(threepf_state), CPPTRANSPORT_OPERATIONS_NAME(threepf_state)};
        size_t steps = integrate_times(stepper, rhs, x, begin_iterator, end_iterator,
                                       static_cast<number>($PERT_STEP_SIZE/pow(4.0,refinement_level)), obs);

        obs.stop_timers(steps, refinement_level);
        rhs.close_down_workspace();

#ifdef CPPTRANSPORT_INSTRUMENT
        ++this->threepf_items;
#endif
      }


    template <typename number, typename StateType>
    void $MODEL_mpi<number, StateType>::populate_threepf_ic(threepf_state& x, unsigned int start,
                                                            const threepf_kconfig& kconfig, double Ninit,
                                                            const twopf_db_task<number>* tk,
                                                            const std::vector<number>& ics, double k_normalize)
      {
        DEFINE_INDEX_TOOLS

        assert(x.size() >= start);
        assert(x.size() >= start + $MODEL_pool::threepf_size);

        x[start + FLATTEN($A,$B,$C)] = this->make_threepf_ic($A, $B, $C, kconfig.k1_comoving, kconfig.k2_comoving, kconfig.k3_comoving, Ninit, tk, ics, k_normalize);
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


    template <typename Model>
    void $MODEL_mpi_twopf_functor<Model>::operator()(const twopf_state& __x, twopf_state& __dxdt, number __t)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE

        const auto __a = std::exp(__t - this->__N_horizon_exit + this->__astar_normalization);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__x}

        // calculation of dV, ddV, dddV has to occur above the temporary pool
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __x, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __x, __Mp, __ddV);

          // capture resources for transport tensors
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
        $ENDIF

#ifdef CPPTRANSPORT_INSTRUMENT
        __setup_timer.resume();
#endif

        $TEMP_POOL{"const auto $1 = $2;"}

        // check FLATTEN functions are being evaluated at compile time
        static_assert(TENSOR_FLATTEN(0,0) == 0, "TENSOR_FLATTEN failure");
        static_assert(FLATTEN(0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0,0) == 0, "FLATTEN failure");

        const auto __tensor_twopf_ff = __x[$MODEL_pool::tensor_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_twopf_fp = __x[$MODEL_pool::tensor_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_twopf_pf = __x[$MODEL_pool::tensor_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_twopf_pp = __x[$MODEL_pool::tensor_start + TENSOR_FLATTEN(1,1)];

#undef __twopf
#define __twopf(a,b) __x[$MODEL_pool::twopf_start + FLATTEN(a,b)]

#undef __background
#undef __dtwopf
#undef __dtwopf_tensor
#define __background(a)      __dxdt[$MODEL_pool::backg_start + FLATTEN(a)]
#define __dtwopf_tensor(a,b) __dxdt[$MODEL_pool::tensor_start + TENSOR_FLATTEN(a,b)]
#define __dtwopf(a,b)        __dxdt[$MODEL_pool::twopf_start + FLATTEN(a,b)]

#ifdef CPPTRANSPORT_INSTRUMENT
        __setup_timer.stop();
        __u_tensor_timer.resume();
#endif

        // evolve the background
        __background($A) = $U1_TENSOR[A];

        const auto __Hsq = $HUBBLE_SQ;
        const auto __eps = $EPSILON;

        // evolve the tensor modes
        const auto __ff = 0.0;
        const auto __fp = 1.0;
        const auto __pf = -__k*__k/(__a*__a*__Hsq);
        const auto __pp = __eps-3.0;
        __dtwopf_tensor(0,0) = __ff*__tensor_twopf_ff + __fp*__tensor_twopf_pf + __ff*__tensor_twopf_ff + __fp*__tensor_twopf_fp;
        __dtwopf_tensor(0,1) = __ff*__tensor_twopf_fp + __fp*__tensor_twopf_pp + __pf*__tensor_twopf_ff + __pp*__tensor_twopf_fp;
        __dtwopf_tensor(1,0) = __pf*__tensor_twopf_ff + __pp*__tensor_twopf_pf + __ff*__tensor_twopf_pf + __fp*__tensor_twopf_pp;
        __dtwopf_tensor(1,1) = __pf*__tensor_twopf_fp + __pp*__tensor_twopf_pp + __pf*__tensor_twopf_pf + __pp*__tensor_twopf_pp;

        // set up components of the u2 tensor
        $U2_DECLARE[AB] = $U2_TENSOR[AB]{__k, __a};

#ifdef CPPTRANSPORT_INSTRUMENT
        __u_tensor_timer.stop();
        __transport_eq_timer.resume();
#endif

        // evolve the 2pf
        // here, we are dealing only with the real part - which is symmetric.
        // so the index placement is not important
        __dtwopf($A, $B) $=  + $U2_CONTAINER[AC] * __twopf($C, $B);
        __dtwopf($A, $B) $+= + $U2_CONTAINER[BC] * __twopf($A, $C);
        
#ifdef CPPTRANSPORT_STRICT_FP_TEST
        if(std::isnan(__background($A)) || std::isinf(__background($A))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);

        if(std::isnan(__dtwopf_tensor(0,0)) || std::isinf(__dtwopf_tensor(0,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__dtwopf_tensor(0,1)) || std::isinf(__dtwopf_tensor(0,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__dtwopf_tensor(1,0)) || std::isinf(__dtwopf_tensor(1,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__dtwopf_tensor(1,1)) || std::isinf(__dtwopf_tensor(1,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);

        if(std::isnan(__dtwopf($A, $B)) || std::isinf(__dtwopf($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
#endif

#ifdef CPPTRANSPORT_INSTRUMENT
        __transport_eq_timer.stop();
        ++__invokations;
#endif
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


    template <typename Model, typename BatchObject>
    void $MODEL_mpi_twopf_observer<Model, BatchObject>::operator()(const twopf_state& x, number t)
      {
        DEFINE_INDEX_TOOLS
        
#undef __background
#undef __twopf
#undef __twopf_tensor

#define __background(a)      x[$MODEL_pool::backg_start + FLATTEN(a)]
#define __twopf(a,b)         x[$MODEL_pool::twopf_start + FLATTEN(a,b)]
#define __twopf_tensor(a,b) x[$MODEL_pool::tensor_start + TENSOR_FLATTEN(a,b)]

#ifndef CPPTRANSPORT_NO_STRICT_FP_TEST
        if(std::isnan(__background($A)) || std::isinf(__background($A))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_tensor(0,0)) || std::isinf(__twopf_tensor(0,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_tensor(0,1)) || std::isinf(__twopf_tensor(0,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_tensor(1,0)) || std::isinf(__twopf_tensor(1,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_tensor(1,1)) || std::isinf(__twopf_tensor(1,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf($A, $B)) || std::isinf(__twopf($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
#endif

        this->start_batching(static_cast<double>(t), this->get_log(), BatchObject::log_severity_level::normal);
        this->push(x);
        this->stop_batching();
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


    template <typename Model>
    void $MODEL_mpi_threepf_functor<Model>::operator()(const threepf_state& __x, threepf_state& __dxdt, number __t)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE

        const auto __a = std::exp(__t - this->__N_horizon_exit + this->__astar_normalization);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__x}

        // calculation of dV, ddV, dddV has to occur above the temporary pool
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __x, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __x, __Mp, __ddV);
          $MODEL_compute_dddV(__raw_params, __x, __Mp, __dddV);

          // capture resources for transport tensors
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
          $RESOURCE_DDDV{__dddV}
        $ENDIF

#ifdef CPPTRANSPORT_INSTRUMENT
        __setup_timer.resume();
#endif

        $TEMP_POOL{"const auto $1 = $2;"}

        // check FLATTEN functions are being evaluated at compile time
        static_assert(TENSOR_FLATTEN(0,0) == 0, "TENSOR_FLATTEN failure");
        static_assert(FLATTEN(0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0,0) == 0, "FLATTEN failure");

        const auto __tensor_k1_twopf_ff = __x[$MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_k1_twopf_fp = __x[$MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_k1_twopf_pf = __x[$MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_k1_twopf_pp = __x[$MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(1,1)];

        const auto __tensor_k2_twopf_ff = __x[$MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_k2_twopf_fp = __x[$MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_k2_twopf_pf = __x[$MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_k2_twopf_pp = __x[$MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(1,1)];

        const auto __tensor_k3_twopf_ff = __x[$MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_k3_twopf_fp = __x[$MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_k3_twopf_pf = __x[$MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_k3_twopf_pp = __x[$MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(1,1)];

#undef __twopf_re_k1
#undef __twopf_re_k2
#undef __twopf_re_k3
#undef __twopf_im_k1
#undef __twopf_im_k2
#undef __twopf_im_k3

#undef __threepf

#define __twopf_re_k1(a,b) __x[$MODEL_pool::twopf_re_k1_start + FLATTEN(a,b)]
#define __twopf_im_k1(a,b) __x[$MODEL_pool::twopf_im_k1_start + FLATTEN(a,b)]
#define __twopf_re_k2(a,b) __x[$MODEL_pool::twopf_re_k2_start + FLATTEN(a,b)]
#define __twopf_im_k2(a,b) __x[$MODEL_pool::twopf_im_k2_start + FLATTEN(a,b)]
#define __twopf_re_k3(a,b) __x[$MODEL_pool::twopf_re_k3_start + FLATTEN(a,b)]
#define __twopf_im_k3(a,b) __x[$MODEL_pool::twopf_im_k3_start + FLATTEN(a,b)]

#define __threepf(a,b,c)	 __x[$MODEL_pool::threepf_start  + FLATTEN(a,b,c)]

#undef __background
#undef __dtwopf_k1_tensor
#undef __dtwopf_k2_tensor
#undef __dtwopf_k3_tensor
#undef __dtwopf_re_k1
#undef __dtwopf_im_k1
#undef __dtwopf_re_k2
#undef __dtwopf_im_k2
#undef __dtwopf_re_k3
#undef __dtwopf_im_k3
#undef __dthreepf
#define __background(a)         __dxdt[$MODEL_pool::backg_start       + FLATTEN(a)]
#define __dtwopf_k1_tensor(a,b) __dxdt[$MODEL_pool::tensor_k1_start   + TENSOR_FLATTEN(a,b)]
#define __dtwopf_k2_tensor(a,b) __dxdt[$MODEL_pool::tensor_k2_start   + TENSOR_FLATTEN(a,b)]
#define __dtwopf_k3_tensor(a,b) __dxdt[$MODEL_pool::tensor_k3_start   + TENSOR_FLATTEN(a,b)]
#define __dtwopf_re_k1(a,b)     __dxdt[$MODEL_pool::twopf_re_k1_start + FLATTEN(a,b)]
#define __dtwopf_im_k1(a,b)     __dxdt[$MODEL_pool::twopf_im_k1_start + FLATTEN(a,b)]
#define __dtwopf_re_k2(a,b)     __dxdt[$MODEL_pool::twopf_re_k2_start + FLATTEN(a,b)]
#define __dtwopf_im_k2(a,b)     __dxdt[$MODEL_pool::twopf_im_k2_start + FLATTEN(a,b)]
#define __dtwopf_re_k3(a,b)     __dxdt[$MODEL_pool::twopf_re_k3_start + FLATTEN(a,b)]
#define __dtwopf_im_k3(a,b)     __dxdt[$MODEL_pool::twopf_im_k3_start + FLATTEN(a,b)]
#define __dthreepf(a,b,c)       __dxdt[$MODEL_pool::threepf_start     + FLATTEN(a,b,c)]

#ifdef CPPTRANSPORT_INSTRUMENT
        __setup_timer.stop();
        __u_tensor_timer.resume();
#endif

        // evolve the background
        __background($A) = $U1_TENSOR[A];

        const auto __Hsq = $HUBBLE_SQ;
        const auto __eps = $EPSILON;

        // evolve the tensor modes
        const auto __ff = 0.0;
        const auto __fp = 1.0;
        const auto __pp = __eps-3.0;

        auto __pf = -__k1*__k1/(__a*__a*__Hsq);
        __dtwopf_k1_tensor(0,0) = __ff*__tensor_k1_twopf_ff + __fp*__tensor_k1_twopf_pf + __ff*__tensor_k1_twopf_ff + __fp*__tensor_k1_twopf_fp;
        __dtwopf_k1_tensor(0,1) = __ff*__tensor_k1_twopf_fp + __fp*__tensor_k1_twopf_pp + __pf*__tensor_k1_twopf_ff + __pp*__tensor_k1_twopf_fp;
        __dtwopf_k1_tensor(1,0) = __pf*__tensor_k1_twopf_ff + __pp*__tensor_k1_twopf_pf + __ff*__tensor_k1_twopf_pf + __fp*__tensor_k1_twopf_pp;
        __dtwopf_k1_tensor(1,1) = __pf*__tensor_k1_twopf_fp + __pp*__tensor_k1_twopf_pp + __pf*__tensor_k1_twopf_pf + __pp*__tensor_k1_twopf_pp;

        __pf = -__k2*__k2/(__a*__a*__Hsq);
        __dtwopf_k2_tensor(0,0) = __ff*__tensor_k2_twopf_ff + __fp*__tensor_k2_twopf_pf + __ff*__tensor_k2_twopf_ff + __fp*__tensor_k2_twopf_fp;
        __dtwopf_k2_tensor(0,1) = __ff*__tensor_k2_twopf_fp + __fp*__tensor_k2_twopf_pp + __pf*__tensor_k2_twopf_ff + __pp*__tensor_k2_twopf_fp;
        __dtwopf_k2_tensor(1,0) = __pf*__tensor_k2_twopf_ff + __pp*__tensor_k2_twopf_pf + __ff*__tensor_k2_twopf_pf + __fp*__tensor_k2_twopf_pp;
        __dtwopf_k2_tensor(1,1) = __pf*__tensor_k2_twopf_fp + __pp*__tensor_k2_twopf_pp + __pf*__tensor_k2_twopf_pf + __pp*__tensor_k2_twopf_pp;

        __pf = -__k3*__k3/(__a*__a*__Hsq);
        __dtwopf_k3_tensor(0,0) = __ff*__tensor_k3_twopf_ff + __fp*__tensor_k3_twopf_pf + __ff*__tensor_k3_twopf_ff + __fp*__tensor_k3_twopf_fp;
        __dtwopf_k3_tensor(0,1) = __ff*__tensor_k3_twopf_fp + __fp*__tensor_k3_twopf_pp + __pf*__tensor_k3_twopf_ff + __pp*__tensor_k3_twopf_fp;
        __dtwopf_k3_tensor(1,0) = __pf*__tensor_k3_twopf_ff + __pp*__tensor_k3_twopf_pf + __ff*__tensor_k3_twopf_pf + __fp*__tensor_k3_twopf_pp;
        __dtwopf_k3_tensor(1,1) = __pf*__tensor_k3_twopf_fp + __pp*__tensor_k3_twopf_pp + __pf*__tensor_k3_twopf_pf + __pp*__tensor_k3_twopf_pp;

        // set up components of the u2 tensor for k1, k2, k3
        $U2_k1_DECLARE[AB] = $U2_TENSOR[AB]{__k1, __a};
        $U2_k2_DECLARE[AB] = $U2_TENSOR[AB]{__k2, __a};
        $U2_k3_DECLARE[AB] = $U2_TENSOR[AB]{__k3, __a};

        // set up components of the u3 tensor
        $U3_k1k2k3_DECLARE[ABC] = $U3_TENSOR[ABC]{__k1, __k2, __k3, __a};
        $U3_k2k1k3_DECLARE[ABC] = $U3_TENSOR[ABC]{__k2, __k1, __k3, __a};
        $U3_k3k1k2_DECLARE[ABC] = $U3_TENSOR[ABC]{__k3, __k1, __k2, __a};

#ifdef CPPTRANSPORT_INSTRUMENT
        __u_tensor_timer.stop();
        __transport_eq_timer.resume();
#endif

        // evolve the real and imaginary components of the 2pf
        // for the imaginary parts, index placement *does* matter so we must take care
        __dtwopf_re_k1($A, $B) $=  + $U2_k1_CONTAINER[AC] * __twopf_re_k1($C, $B);
        __dtwopf_re_k1($A, $B) $+= + $U2_k1_CONTAINER[BC] * __twopf_re_k1($A, $C);

        __dtwopf_im_k1($A, $B) $=  + $U2_k1_CONTAINER[AC] * __twopf_im_k1($C, $B);
        __dtwopf_im_k1($A, $B) $+= + $U2_k1_CONTAINER[BC] * __twopf_im_k1($A, $C);

        __dtwopf_re_k2($A, $B) $=  + $U2_k2_CONTAINER[AC] * __twopf_re_k2($C, $B);
        __dtwopf_re_k2($A, $B) $+= + $U2_k2_CONTAINER[BC] * __twopf_re_k2($A, $C);

        __dtwopf_im_k2($A, $B) $=  + $U2_k2_CONTAINER[AC] * __twopf_im_k2($C, $B);
        __dtwopf_im_k2($A, $B) $+= + $U2_k2_CONTAINER[BC] * __twopf_im_k2($A, $C);

        __dtwopf_re_k3($A, $B) $=  + $U2_k3_CONTAINER[AC] * __twopf_re_k3($C, $B);
        __dtwopf_re_k3($A, $B) $+= + $U2_k3_CONTAINER[BC] * __twopf_re_k3($A, $C);

        __dtwopf_im_k3($A, $B) $=  + $U2_k3_CONTAINER[AC] * __twopf_im_k3($C, $B);
        __dtwopf_im_k3($A, $B) $+= + $U2_k3_CONTAINER[BC] * __twopf_im_k3($A, $C);

        // evolve the components of the 3pf
        // index placement matters, partly because of the k-dependence
        // but also in the source terms from the imaginary components of the 2pf

        __dthreepf($A, $B, $C) $=  + $U2_k1_CONTAINER[AM] * __threepf($M, $B, $C);
        __dthreepf($A, $B, $C) $+= + $U3_k1k2k3_CONTAINER[AMN] * __twopf_re_k2($M, $B) * __twopf_re_k3($N, $C);
        __dthreepf($A, $B, $C) $+= - $U3_k1k2k3_CONTAINER[AMN] * __twopf_im_k2($M, $B) * __twopf_im_k3($N, $C);

        __dthreepf($A, $B, $C) $+= + $U2_k2_CONTAINER[BM] * __threepf($A, $M, $C);
        __dthreepf($A, $B, $C) $+= + $U3_k2k1k3_CONTAINER[BMN] * __twopf_re_k1($A, $M) * __twopf_re_k3($N, $C);
        __dthreepf($A, $B, $C) $+= - $U3_k2k1k3_CONTAINER[BMN] * __twopf_im_k1($A, $M) * __twopf_im_k3($N, $C);

        __dthreepf($A, $B, $C) $+= + $U2_k3_CONTAINER[CM] * __threepf($A, $B, $M);
        __dthreepf($A, $B, $C) $+= + $U3_k3k1k2_CONTAINER[CMN] * __twopf_re_k1($A, $M) * __twopf_re_k2($B, $N);
        __dthreepf($A, $B, $C) $+= - $U3_k3k1k2_CONTAINER[CMN] * __twopf_im_k1($A, $M) * __twopf_im_k2($B, $N);
        
#ifdef CPPTRANSPORT_INSTRUMENT
        __transport_eq_timer.stop();
        ++__invokations;
#endif
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename Model, typename BatchObject>
    void $MODEL_mpi_threepf_observer<Model, BatchObject>::operator()(const threepf_state& x, number t)
      {
        DEFINE_INDEX_TOOLS

#undef __background
#undef __twopf_k1_tensor
#undef __twopf_k2_tensor
#undef __twopf_k3_tensor
#undef __twopf_re_k1
#undef __twopf_im_k1
#undef __twopf_re_k2
#undef __twopf_im_k2
#undef __twopf_re_k3
#undef __twopf_im_k3
#undef __threepf

#define __background(a)        x[$MODEL_pool::backg_start       + FLATTEN(a)]
#define __twopf_k1_tensor(a,b) x[$MODEL_pool::tensor_k1_start   + TENSOR_FLATTEN(a,b)]
#define __twopf_k2_tensor(a,b) x[$MODEL_pool::tensor_k2_start   + TENSOR_FLATTEN(a,b)]
#define __twopf_k3_tensor(a,b) x[$MODEL_pool::tensor_k3_start   + TENSOR_FLATTEN(a,b)]
#define __twopf_re_k1(a,b)     x[$MODEL_pool::twopf_re_k1_start + FLATTEN(a,b)]
#define __twopf_im_k1(a,b)     x[$MODEL_pool::twopf_im_k1_start + FLATTEN(a,b)]
#define __twopf_re_k2(a,b)     x[$MODEL_pool::twopf_re_k2_start + FLATTEN(a,b)]
#define __twopf_im_k2(a,b)     x[$MODEL_pool::twopf_im_k2_start + FLATTEN(a,b)]
#define __twopf_re_k3(a,b)     x[$MODEL_pool::twopf_re_k3_start + FLATTEN(a,b)]
#define __twopf_im_k3(a,b)     x[$MODEL_pool::twopf_im_k3_start + FLATTEN(a,b)]
#define __threepf(a,b,c)       x[$MODEL_pool::threepf_start     + FLATTEN(a,b,c)]

#ifndef CPPTRANSPORT_NO_STRICT_FP_TEST
        if(std::isnan(__background($A)) || std::isinf(__background($A))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_k1_tensor(0,0)) || std::isinf(__twopf_k1_tensor(0,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k1_tensor(0,1)) || std::isinf(__twopf_k1_tensor(0,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k1_tensor(1,0)) || std::isinf(__twopf_k1_tensor(1,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k1_tensor(1,1)) || std::isinf(__twopf_k1_tensor(1,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_k2_tensor(0,0)) || std::isinf(__twopf_k2_tensor(0,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k2_tensor(0,1)) || std::isinf(__twopf_k2_tensor(0,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k2_tensor(1,0)) || std::isinf(__twopf_k2_tensor(1,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k2_tensor(1,1)) || std::isinf(__twopf_k2_tensor(1,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_k3_tensor(0,0)) || std::isinf(__twopf_k3_tensor(0,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k3_tensor(0,1)) || std::isinf(__twopf_k3_tensor(0,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k3_tensor(1,0)) || std::isinf(__twopf_k3_tensor(1,0))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_k3_tensor(1,1)) || std::isinf(__twopf_k3_tensor(1,1))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_re_k1($A, $B)) || std::isinf(__twopf_re_k1($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_im_k1($A, $B)) || std::isinf(__twopf_im_k1($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_re_k2($A, $B)) || std::isinf(__twopf_re_k2($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_im_k2($A, $B)) || std::isinf(__twopf_im_k2($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__twopf_re_k3($A, $B)) || std::isinf(__twopf_re_k3($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
        if(std::isnan(__twopf_im_k3($A, $B)) || std::isinf(__twopf_im_k3($A, $B))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
    
        if(std::isnan(__threepf($A, $B, $C)) || std::isinf(__threepf($A, $B, $C))) throw runtime_exception(exception_type::INTEGRATION_FAILURE, CPPTRANSPORT_INTEGRATOR_NAN_OR_INF);
#endif

        this->start_batching(static_cast<double>(t), this->get_log(), BatchObject::log_severity_level::normal);
        this->push(x);
        this->stop_batching();
      }


    }   // namespace transport


#endif  // $GUARD
