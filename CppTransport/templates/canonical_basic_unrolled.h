// backend=cpp minver=0.13
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// MPI implementation

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include "transport-runtime-api/transport.h"

#include "$$__CORE"


namespace transport
  {

    // set up a state type for 2pf integration
    template <typename number>
    using twopf_state = std::vector<number>;

    // set up a state type for 3pf integration
    template <typename number>
    using threepf_state = std::vector<number>;

    namespace $$__MODEL_pool
      {
        const static std::string backend = "MPI";
        const static std::string pert_stepper = "$$__PERT_STEPPER";
        const static std::string back_stepper = "$$__BACKG_STEPPER";
      }


    // *********************************************************************************************


    // CLASS FOR $$__MODEL 'basic', ie., MPI implementation
    template <typename number>
    class $$__MODEL_basic : public $$__MODEL<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        $$__MODEL_basic()
          : $$__MODEL<number>()
          {
          }

        //! destructor is default
        virtual ~$$__MODEL_basic() = default;


        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        virtual const std::string& get_backend() const override { return($$__MODEL_pool::backend); }

        virtual const std::string& get_back_stepper() const override { return($$__MODEL_pool::back_stepper); }

        virtual const std::string& get_pert_stepper() const override { return($$__MODEL_pool::pert_stepper); }

        virtual std::pair< double, double > get_back_tol() const override { return std::make_pair($$__BACKG_ABS_ERR, $$__BACKG_REL_ERR); }

        virtual std::pair< double, double > get_pert_tol() const override { return std::make_pair($$__PERT_ABS_ERR, $$__PERT_REL_ERR); }


        // BACKEND INTERFACE

      public:

        // Set up a context
        virtual context backend_get_context() override;

        // Get backend type
        virtual worker_type get_backend_type() override;

        //! Get backend memory capacity
        virtual unsigned int get_backend_memory() override;

        //! Get backend priority
        virtual unsigned int get_backend_priority() override;

        // Integrate background and 2-point function on the CPU
        virtual void backend_process_queue(work_queue<twopf_kconfig_record>& work, const twopf_list_task<number>* tk,
                                           twopf_batcher<number>& batcher,
                                           bool silent = false) override;

        // Integrate background, 2-point function and 3-point function on the CPU
        virtual void backend_process_queue(work_queue<threepf_kconfig_record>& work, const threepf_task<number>* tk,
                                           threepf_batcher<number>& batcher,
                                           bool silent = false) override;

        virtual unsigned int backend_twopf_state_size(void)   const override { return($$__MODEL_pool::twopf_state_size); }
        virtual unsigned int backend_threepf_state_size(void) const override { return($$__MODEL_pool::threepf_state_size); }

        virtual bool supports_per_configuration_statistics(void) const override { return(true); }


        // INTERNAL API

      protected:

        void twopf_kmode(const twopf_kconfig_record& kconfig, const twopf_list_task<number>* tk,
                         twopf_batcher<number>& batcher, unsigned int refinement_level);

        void threepf_kmode(const threepf_kconfig_record&, const threepf_task<number>* tk,
                           threepf_batcher<number>& batcher, unsigned int refinement_level);

        void populate_twopf_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                               const twopf_list_task<number>* tk, const std::vector<number>& ic, bool imaginary = false);

        void populate_tensor_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                                const twopf_list_task<number>* tk, const std::vector<number>& ic);

        void populate_threepf_ic(threepf_state<number>& x, unsigned int start, const threepf_kconfig& kconfig,
                                 double Ninit, const twopf_list_task<number>* tk, const std::vector<number>& ic);

      };


    // integration - 2pf functor
    template <typename number>
    class $$__MODEL_basic_twopf_functor
      {

      public:

        $$__MODEL_basic_twopf_functor(const twopf_list_task<number>* tk, const twopf_kconfig& k)
          : params(tk->get_params()),
            Mp(tk->get_params().get_Mp()),
            N_horizon_exit(tk->get_N_horizon_crossing()),
            astar_normalization(tk->get_astar_normalization()),
            config(k),
            u2(nullptr),
            dV(nullptr),
            ddV(nullptr),
            raw_params(nullptr)
          {
          }

        void set_up_workspace()
          {
            this->u2 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];

            this->dV = new number[$$__NUMBER_FIELDS];
            this->ddV = new number[$$__NUMBER_FIELDS * $$__NUMBER_FIELDS];

            this->raw_params = new number[$$__NUMBER_PARAMS];

            this->raw_params[$$__1] = this->params.get_vector()[$$__1];
          }

        void close_down_workspace()
          {
            delete[] this->u2;

            delete[] this->dV;
            delete[] this->ddV;

            delete[] this->raw_params;
          }

        void operator ()(const twopf_state<number>& __x, twopf_state<number>& __dxdt, double __t);

        // adjust horizon exit time, given an initial time N_init which we wish to move to zero
        void rebase_horizon_exit_time(double N_init) { this->N_horizon_exit -= N_init; }


        // INTERNAL DATA

      private:

        const parameters<number>& params;

        number Mp;

        double N_horizon_exit;

        double astar_normalization;

        const twopf_kconfig config;

        // manage memory ourselves, rather than via an STL container, for maximum performance
        // also avoids copying overheads (the Boost odeint library copies the functor by value)
        number* u2;

        number* dV;
        number* ddV;

        number* raw_params;

      };


    // integration - observer object for 2pf
    template <typename number>
    class $$__MODEL_basic_twopf_observer: public twopf_singleconfig_batch_observer<number>
      {

      public:

        $$__MODEL_basic_twopf_observer(twopf_batcher<number>& b, const twopf_kconfig_record& c,
                                       const time_config_database& t)
          : twopf_singleconfig_batch_observer<number>(b, c, t,
                                                      $$__MODEL_pool::backg_size, $$__MODEL_pool::tensor_size, $$__MODEL_pool::twopf_size,
                                                      $$__MODEL_pool::backg_start, $$__MODEL_pool::tensor_start, $$__MODEL_pool::twopf_start)
          {
          }

        void operator ()(const twopf_state<number>& x, double t);

      };


    // integration - 3pf functor
    template <typename number>
    class $$__MODEL_basic_threepf_functor
      {

      public:

        $$__MODEL_basic_threepf_functor(const twopf_list_task<number>* tk, const threepf_kconfig& k)
          : params(tk->get_params()),
            Mp(tk->get_params().get_Mp()),
            N_horizon_exit(tk->get_N_horizon_crossing()),
            astar_normalization(tk->get_astar_normalization()),
            config(k),
            u2_k1(nullptr),
            u2_k2(nullptr),
            u2_k3(nullptr),
            u3_k1k2k3(nullptr),
            u3_k2k1k3(nullptr),
            u3_k3k1k2(nullptr),
            dV(nullptr),
            ddV(nullptr),
            dddV(nullptr),
            raw_params(nullptr)
          {
          }

        void set_up_workspace()
          {
            this->u2_k1 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];
            this->u2_k2 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];
            this->u2_k3 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];

            this->u3_k1k2k3 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];
            this->u3_k2k1k3 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];
            this->u3_k3k1k2 = new number[2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS];

            this->dV = new number[$$__NUMBER_FIELDS];
            this->ddV = new number[$$__NUMBER_FIELDS * $$__NUMBER_FIELDS];
            this->dddV = new number[$$__NUMBER_FIELDS * $$__NUMBER_FIELDS * $$__NUMBER_FIELDS];

            this->raw_params = new number[$$__NUMBER_PARAMS];

            this->raw_params[$$__1] = this->params.get_vector()[$$__1];
          }

        void close_down_workspace()
          {
            delete[] this->u2_k1;
            delete[] this->u2_k2;
            delete[] this->u2_k3;

            delete[] this->u3_k1k2k3;
            delete[] this->u3_k2k1k3;
            delete[] this->u3_k3k1k2;

            delete[] this->dV;
            delete[] this->ddV;
            delete[] this->dddV;

            delete[] this->raw_params;
          }

        void operator ()(const threepf_state<number>& __x, threepf_state<number>& __dxdt, double __dt);

        // adjust horizon exit time, given an initial time N_init which we wish to move to zero
        void rebase_horizon_exit_time(double N_init) { this->N_horizon_exit -= N_init; }

      private:

        const parameters<number>& params;

        number Mp;

        double N_horizon_exit;

        double astar_normalization;

        const threepf_kconfig config;

        // manage memory ourselves, rather than via an STL container, for maximum performance
        // also avoids copying overheads (the Boost odeint library copies the functor by value)

        number* u2_k1;
        number* u2_k2;
        number* u2_k3;

        number* u3_k1k2k3;
        number* u3_k2k1k3;
        number* u3_k3k1k2;

        number* dV;
        number* ddV;
        number* dddV;

        number* raw_params;

      };


    // integration - observer object for 3pf
    template <typename number>
    class $$__MODEL_basic_threepf_observer: public threepf_singleconfig_batch_observer<number>
      {

      public:
        $$__MODEL_basic_threepf_observer(threepf_batcher<number>& b, const threepf_kconfig_record& c,
                                         const time_config_database& t)
          : threepf_singleconfig_batch_observer<number>(b, c, t,
                                                        $$__MODEL_pool::backg_size, $$__MODEL_pool::tensor_size,
                                                        $$__MODEL_pool::twopf_size, $$__MODEL_pool::threepf_size,
                                                        $$__MODEL_pool::backg_start,
                                                        $$__MODEL_pool::tensor_k1_start, $$__MODEL_pool::tensor_k2_start, $$__MODEL_pool::tensor_k3_start,
                                                        $$__MODEL_pool::twopf_re_k1_start, $$__MODEL_pool::twopf_im_k1_start,
                                                        $$__MODEL_pool::twopf_re_k2_start, $$__MODEL_pool::twopf_im_k2_start,
                                                        $$__MODEL_pool::twopf_re_k3_start, $$__MODEL_pool::twopf_im_k3_start,
                                                        $$__MODEL_pool::threepf_start)
          {
          }

        void operator ()(const threepf_state<number>& x, double t);

      };


    // BACKEND INTERFACE


    // generate a context
    template <typename number>
    context $$__MODEL_basic<number>::backend_get_context(void)
      {
        context ctx;

        // set up just one device
        ctx.add_device($$__MODEL_pool::backend);

        return(ctx);
      }


    template <typename number>
    worker_type $$__MODEL_basic<number>::get_backend_type(void)
      {
        return(worker_type::cpu);
      }


    template <typename number>
    unsigned int $$__MODEL_basic<number>::get_backend_memory(void)
      {
        return(0);
      }


    template <typename number>
    unsigned int $$__MODEL_basic<number>::get_backend_priority(void)
      {
        return(1);
      }


    // process work queue for twopf
    template <typename number>
    void $$__MODEL_basic<number>::backend_process_queue(work_queue<twopf_kconfig_record>& work, const twopf_list_task<number>* tk,
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
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const work_queue<twopf_kconfig_record>::device_queue queues = work[0];

        // we expect only one queue on this device
        assert(queues.size() == 1);
        const work_queue<twopf_kconfig_record>::device_work_list list = queues[0];

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

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::log_severity_level::error)
                    << "!! " CPPTRANSPORT_FAILED_CONFIG << " " << list[i]->serial << " (" << i+1
                    << " " CPPTRANSPORT_OF << " " << list.size() << ") | " << list[i];
              }
          }
      }


    template <typename number>
    void $$__MODEL_basic<number>::twopf_kmode(const twopf_kconfig_record& kconfig, const twopf_list_task<number>* tk,
                                              twopf_batcher<number>& batcher, unsigned int refinement_level)
      {
        if(refinement_level > tk->get_max_refinements()) throw runtime_exception(exception_type::REFINEMENT_FAILURE, CPPTRANSPORT_REFINEMENT_TOO_DEEP);

        // get time configuration database
        const time_config_database time_db = tk->get_time_config_database(*kconfig);

        // set up a functor to observe the integration
        // this also starts the timers running, so we do it as early as possible
        $$__MODEL_basic_twopf_observer<number> obs(batcher, kconfig, time_db);

        // set up a functor to evolve this system
        $$__MODEL_basic_twopf_functor<number> rhs(tk, *kconfig);
        rhs.set_up_workspace();

        // set up a state vector
        twopf_state<number> x;
        x.resize($$__MODEL_pool::twopf_state_size);

        // fix initial conditions - background
        const std::vector<number> ics = tk->get_ics_vector(*kconfig);
        x[$$__MODEL_pool::backg_start + FLATTEN($$__A)] = ics[$$__A];

        if(batcher.is_collecting_initial_conditions())
          {
            const std::vector<number> ics_1 = tk->get_ics_exit_vector(*kconfig);
            double t_exit = tk->get_ics_exit_time(*kconfig);
            batcher.push_ics(kconfig->serial, t_exit, ics_1);
          }

        // fix initial conditions - tensors
        this->populate_tensor_ic(x, $$__MODEL_pool::tensor_start, kconfig->k_comoving, *(time_db.value_begin()), tk, ics);

        // fix initial conditions - 2pf
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_start, kconfig->k_comoving, *(time_db.value_begin()), tk, ics);

        // up to this point the calculation has been done in the user-supplied time variable.
        // However, the integrator apparently performs much better if times are measured from zero (but not yet clear why)
        // TODO: would be nice to remove this in future
        rhs.rebase_horizon_exit_time(tk->get_ics().get_N_initial());
        time_config_database::const_value_iterator begin_iterator = time_db.value_begin(tk->get_ics().get_N_initial());
        time_config_database::const_value_iterator end_iterator   = time_db.value_end(tk->get_ics().get_N_initial());

        size_t steps = boost::numeric::odeint::integrate_times($$__MAKE_PERT_STEPPER{twopf_state<number>}, rhs, x, begin_iterator, end_iterator, $$__PERT_STEP_SIZE/pow(4.0,refinement_level), obs);

        obs.stop_timers(steps, refinement_level);
        rhs.close_down_workspace();
      }


    // make initial conditions for each component of the 2pf
    // x         - state vector *containing* space for the 2pf (doesn't have to be entirely the 2pf)
    // start     - starting position of twopf components within the state vector
    // kmode     - *comoving normalized* wavenumber for which we will compute the twopf
    // Ninit     - initial time
    // p         - parameters
    // ics       - iniitial conditions for the background fields (or fields+momenta)
    // imaginary - whether to populate using real or imaginary components of the 2pf
    template <typename number>
    void $$__MODEL_basic<number>::populate_twopf_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                                                    const twopf_list_task<number>* tk, const std::vector<number>& ics, bool imaginary)
      {
        assert(x.size() >= start);
        assert(x.size() >= start + $$__MODEL_pool::twopf_size);

        x[start + FLATTEN($$__A,$$__B)] = imaginary ? this->make_twopf_im_ic($$__A, $$__B, kmode, Ninit, tk, ics) : this->make_twopf_re_ic($$__A, $$__B, kmode, Ninit, tk, ics);
      }


    // make initial conditions for the tensor twopf
    template <typename number>
    void $$__MODEL_basic<number>::populate_tensor_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                                                     const twopf_list_task<number>* tk, const std::vector<number>& ics)
      {
        assert(x.size() >= start);
        assert(x.size() >= start + $$__MODEL_pool::tensor_size);

        x[start + TENSOR_FLATTEN(0,0)] = this->make_twopf_tensor_ic(0, 0, kmode, Ninit, tk, ics);
        x[start + TENSOR_FLATTEN(0,1)] = this->make_twopf_tensor_ic(0, 1, kmode, Ninit, tk, ics);
        x[start + TENSOR_FLATTEN(1,0)] = this->make_twopf_tensor_ic(1, 0, kmode, Ninit, tk, ics);
        x[start + TENSOR_FLATTEN(1,1)] = this->make_twopf_tensor_ic(1, 1, kmode, Ninit, tk, ics);
      }


    // THREE-POINT FUNCTION INTEGRATION


    template <typename number>
    void $$__MODEL_basic<number>::backend_process_queue(work_queue<threepf_kconfig_record>& work, const threepf_task<number>* tk,
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
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be only one device with this backend)
        assert(work.size() == 1);
        const work_queue<threepf_kconfig_record>::device_queue queues = work[0];

        // we expect only one queue on this device
        assert(queues.size() == 1);
        const work_queue<threepf_kconfig_record>::device_work_list list = queues[0];

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


    template <typename number>
    void $$__MODEL_basic<number>::threepf_kmode(const threepf_kconfig_record& kconfig, const threepf_task<number>* tk,
                                                threepf_batcher<number>& batcher, unsigned int refinement_level)
      {
        if(refinement_level > tk->get_max_refinements()) throw runtime_exception(exception_type::REFINEMENT_FAILURE, CPPTRANSPORT_REFINEMENT_TOO_DEEP);

        // get list of time steps, and storage list
        const time_config_database time_db = tk->get_time_config_database(*kconfig);

        // set up a functor to observe the integration
        // this also starts the timers running, so we do it as early as possible
        $$__MODEL_basic_threepf_observer<number> obs(batcher, kconfig, time_db);

        // set up a functor to evolve this system
        $$__MODEL_basic_threepf_functor<number>  rhs(tk, *kconfig);
        rhs.set_up_workspace();

        // set up a state vector
        threepf_state<number> x;
        x.resize($$__MODEL_pool::threepf_state_size);

        // fix initial conditions - background
        // use fast-forwarding if enabled
        // (don't need explicit FLATTEN since it would appear on both sides)
        const std::vector<number> ics = tk->get_ics_vector(*kconfig);
        x[$$__MODEL_pool::backg_start + $$__A] = ics[$$__A];

        if(batcher.is_collecting_initial_conditions())
          {
            const std::vector<number> ics_1 = tk->get_ics_exit_vector(*kconfig, threepf_ics_exit_type::smallest_wavenumber_exit);
            const std::vector<number> ics_2 = tk->get_ics_exit_vector(*kconfig, threepf_ics_exit_type::kt_wavenumber_exit);
            double t_exit_1 = tk->get_ics_exit_time(*kconfig, threepf_ics_exit_type::smallest_wavenumber_exit);
            double t_exit_2 = tk->get_ics_exit_time(*kconfig, threepf_ics_exit_type::kt_wavenumber_exit);
            batcher.push_ics(kconfig->serial, t_exit_1, ics_1);
            batcher.push_kt_ics(kconfig->serial, t_exit_2, ics_2);
          }

        // fix initial conditions - tensors
        this->populate_tensor_ic(x, $$__MODEL_pool::tensor_k1_start, kconfig->k1_comoving, *(time_db.value_begin()), tk, ics);
        this->populate_tensor_ic(x, $$__MODEL_pool::tensor_k2_start, kconfig->k2_comoving, *(time_db.value_begin()), tk, ics);
        this->populate_tensor_ic(x, $$__MODEL_pool::tensor_k3_start, kconfig->k3_comoving, *(time_db.value_begin()), tk, ics);

        // fix initial conditions - real 2pfs
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k1_start, kconfig->k1_comoving, *(time_db.value_begin()), tk, ics, false);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k2_start, kconfig->k2_comoving, *(time_db.value_begin()), tk, ics, false);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k3_start, kconfig->k3_comoving, *(time_db.value_begin()), tk, ics, false);

        // fix initial conditions - imaginary 2pfs
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k1_start, kconfig->k1_comoving, *(time_db.value_begin()), tk, ics, true);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k2_start, kconfig->k2_comoving, *(time_db.value_begin()), tk, ics, true);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k3_start, kconfig->k3_comoving, *(time_db.value_begin()), tk, ics, true);

        // fix initial conditions - threepf
        this->populate_threepf_ic(x, $$__MODEL_pool::threepf_start, *kconfig, *(time_db.value_begin()), tk, ics);

        // up to this point the calculation has been done in the user-supplied time variable.
        // However, the integrator apparently performs much better if times are measured from zero (but not yet clear why)
        // TODO: would be nice to remove this in future
        rhs.rebase_horizon_exit_time(tk->get_ics().get_N_initial());
        time_config_database::const_value_iterator begin_iterator = time_db.value_begin(tk->get_ics().get_N_initial());
        time_config_database::const_value_iterator end_iterator   = time_db.value_end(tk->get_ics().get_N_initial());

        size_t steps = boost::numeric::odeint::integrate_times( $$__MAKE_PERT_STEPPER{threepf_state<number>}, rhs, x, begin_iterator, end_iterator, $$__PERT_STEP_SIZE/pow(4.0,refinement_level), obs);

        obs.stop_timers(steps, refinement_level);
        rhs.close_down_workspace();
      }


    template <typename number>
    void $$__MODEL_basic<number>::populate_threepf_ic(threepf_state<number>& x, unsigned int start,
                                                      const threepf_kconfig& kconfig, double Ninit,
                                                      const twopf_list_task<number>* tk, const std::vector<number>& ics)
      {
        assert(x.size() >= start);
        assert(x.size() >= start + $$__MODEL_pool::threepf_size);

        x[start + FLATTEN($$__A,$$__B,$$__C)] = this->make_threepf_ic($$__A, $$__B, $$__C, kconfig.k1_comoving, kconfig.k2_comoving, kconfig.k3_comoving, Ninit, tk, ics);
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


    template <typename number>
    void $$__MODEL_basic_twopf_functor<number>::operator()(const twopf_state<number>& __x, twopf_state<number>& __dxdt, double __t)
      {
        const auto __Mp = this->Mp;
        const auto __k = this->config.k_comoving;
        const auto __a = std::exp(__t - this->N_horizon_exit + this->astar_normalization);

        // calculation of dV, ddV, dddV has to occur above the temporary pool
        $$__MODEL_compute_dV(this->raw_params, __x, this->dV);
        $$__MODEL_compute_ddV(this->raw_params, __x, this->ddV);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        // check FLATTEN functions are being evaluated at compile time
        static_assert(TENSOR_FLATTEN(0,0) == 0, "TENSOR_FLATTEN failure");
        static_assert(FLATTEN(0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0,0) == 0, "FLATTEN failure");

        const auto __tensor_twopf_ff = __x[$$__MODEL_pool::tensor_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_twopf_fp = __x[$$__MODEL_pool::tensor_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_twopf_pf = __x[$$__MODEL_pool::tensor_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_twopf_pp = __x[$$__MODEL_pool::tensor_start + TENSOR_FLATTEN(1,1)];

#undef __twopf
#define __twopf(a,b) __x[$$__MODEL_pool::twopf_start + FLATTEN(a,b)]

#undef __background
#undef __dtwopf
#undef __dtwopf_tensor
#define __background(a)      __dxdt[$$__MODEL_pool::backg_start + FLATTEN(a)]
#define __dtwopf_tensor(a,b) __dxdt[$$__MODEL_pool::tensor_start + TENSOR_FLATTEN(a,b)]
#define __dtwopf(a,b)        __dxdt[$$__MODEL_pool::twopf_start + FLATTEN(a,b)]

        // evolve the background
        __background($$__A) = $$__U1_PREDEF[A]{raw_params, __x, dV, FLATTEN, FIELDS_FLATTEN};

        const auto __Hsq = $$__HUBBLE_SQ{raw_params, __x, FLATTEN};
        const auto __eps = $$__EPSILON{__x, FLATTEN};

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
        this->u2[FLATTEN($$__A,$$__B)] = $$__U2_PREDEF[AB]{__k, __a, raw_params, __x, dV, ddV, FLATTEN, FIELDS_FLATTEN};

        // evolve the 2pf
        // here, we are dealing only with the real part - which is symmetric.
        // so the index placement is not important
        __dtwopf($$__A, $$__B) $$=  + this->u2[FLATTEN($$__A, $$__C)] * __twopf($$__C, $$__B);
        __dtwopf($$__A, $$__B) $$+= + this->u2[FLATTEN($$__B, $$__C)] * __twopf($$__A, $$__C);
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


    template <typename number>
    void $$__MODEL_basic_twopf_observer<number>::operator()(const twopf_state<number>& x, double t)
      {
        this->start_batching(t, this->get_log(), generic_batcher::log_severity_level::normal);
        this->push(x);
        this->stop_batching();
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


    template <typename number>
    void $$__MODEL_basic_threepf_functor<number>::operator()(const threepf_state<number>& __x, threepf_state<number>& __dxdt, double __t)
      {
        const auto __Mp = this->Mp;
        const auto __k1 = this->config.k1_comoving;
        const auto __k2 = this->config.k2_comoving;
        const auto __k3 = this->config.k3_comoving;
        const auto __a = std::exp(__t - this->N_horizon_exit + this->astar_normalization);

        // calculation of dV, ddV, dddV has to occur above the temporary pool
        $$__MODEL_compute_dV(this->raw_params, __x, this->dV);
        $$__MODEL_compute_ddV(this->raw_params, __x, this->ddV);
        $$__MODEL_compute_dddV(this->raw_params, __x, this->dddV);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        // check FLATTEN functions are being evaluated at compile time
        static_assert(TENSOR_FLATTEN(0,0) == 0, "TENSOR_FLATTEN failure");
        static_assert(FLATTEN(0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0) == 0, "FLATTEN failure");
        static_assert(FLATTEN(0,0,0) == 0, "FLATTEN failure");

        const auto __tensor_k1_twopf_ff = __x[$$__MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_k1_twopf_fp = __x[$$__MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_k1_twopf_pf = __x[$$__MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_k1_twopf_pp = __x[$$__MODEL_pool::tensor_k1_start + TENSOR_FLATTEN(1,1)];

        const auto __tensor_k2_twopf_ff = __x[$$__MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_k2_twopf_fp = __x[$$__MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_k2_twopf_pf = __x[$$__MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_k2_twopf_pp = __x[$$__MODEL_pool::tensor_k2_start + TENSOR_FLATTEN(1,1)];

        const auto __tensor_k3_twopf_ff = __x[$$__MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(0,0)];
        const auto __tensor_k3_twopf_fp = __x[$$__MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(0,1)];
        const auto __tensor_k3_twopf_pf = __x[$$__MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(1,0)];
        const auto __tensor_k3_twopf_pp = __x[$$__MODEL_pool::tensor_k3_start + TENSOR_FLATTEN(1,1)];

#undef __twopf_re_k1
#undef __twopf_re_k2
#undef __twopf_re_k3
#undef __twopf_im_k1
#undef __twopf_im_k2
#undef __twopf_im_k3

#undef __threepf

#define __twopf_re_k1(a,b) __x[$$__MODEL_pool::twopf_re_k1_start + FLATTEN(a,b)]
#define __twopf_im_k1(a,b) __x[$$__MODEL_pool::twopf_im_k1_start + FLATTEN(a,b)]
#define __twopf_re_k2(a,b) __x[$$__MODEL_pool::twopf_re_k2_start + FLATTEN(a,b)]
#define __twopf_im_k2(a,b) __x[$$__MODEL_pool::twopf_im_k2_start + FLATTEN(a,b)]
#define __twopf_re_k3(a,b) __x[$$__MODEL_pool::twopf_re_k3_start + FLATTEN(a,b)]
#define __twopf_im_k3(a,b) __x[$$__MODEL_pool::twopf_im_k3_start + FLATTEN(a,b)]

#define __threepf(a,b,c)	 __x[$$__MODEL_pool::threepf_start  + FLATTEN(a,b,c)]

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
#define __background(a)         __dxdt[$$__MODEL_pool::backg_start       + FLATTEN(a)]
#define __dtwopf_k1_tensor(a,b) __dxdt[$$__MODEL_pool::tensor_k1_start   + TENSOR_FLATTEN(a,b)]
#define __dtwopf_k2_tensor(a,b) __dxdt[$$__MODEL_pool::tensor_k2_start   + TENSOR_FLATTEN(a,b)]
#define __dtwopf_k3_tensor(a,b) __dxdt[$$__MODEL_pool::tensor_k3_start   + TENSOR_FLATTEN(a,b)]
#define __dtwopf_re_k1(a,b)     __dxdt[$$__MODEL_pool::twopf_re_k1_start + FLATTEN(a,b)]
#define __dtwopf_im_k1(a,b)     __dxdt[$$__MODEL_pool::twopf_im_k1_start + FLATTEN(a,b)]
#define __dtwopf_re_k2(a,b)     __dxdt[$$__MODEL_pool::twopf_re_k2_start + FLATTEN(a,b)]
#define __dtwopf_im_k2(a,b)     __dxdt[$$__MODEL_pool::twopf_im_k2_start + FLATTEN(a,b)]
#define __dtwopf_re_k3(a,b)     __dxdt[$$__MODEL_pool::twopf_re_k3_start + FLATTEN(a,b)]
#define __dtwopf_im_k3(a,b)     __dxdt[$$__MODEL_pool::twopf_im_k3_start + FLATTEN(a,b)]
#define __dthreepf(a,b,c)       __dxdt[$$__MODEL_pool::threepf_start     + FLATTEN(a,b,c)]

        // evolve the background
        __background($$__A) = $$__U1_PREDEF[A]{raw_params, __x, dV, FLATTEN, FIELDS_FLATTEN};

        const auto __Hsq = $$__HUBBLE_SQ{raw_params, __x, FLATTEN};
        const auto __eps = $$__EPSILON{__x, FLATTEN};

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
        this->u2_k1[FLATTEN($$__A,$$__B)] = $$__U2_PREDEF[AB]{__k1, __a, raw_params, __x, dV, ddV, FLATTEN, FIELDS_FLATTEN};
        this->u2_k2[FLATTEN($$__A,$$__B)] = $$__U2_PREDEF[AB]{__k2, __a, raw_params, __x, dV, ddV, FLATTEN, FIELDS_FLATTEN};
        this->u2_k3[FLATTEN($$__A,$$__B)] = $$__U2_PREDEF[AB]{__k3, __a, raw_params, __x, dV, ddV, FLATTEN, FIELDS_FLATTEN};

        // set up components of the u3 tensor
        this->u3_k1k2k3[FLATTEN($$__A,$$__B,$$__C)] = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, raw_params, __x, dV, ddV, dddV, FLATTEN, FIELDS_FLATTEN};
        this->u3_k2k1k3[FLATTEN($$__A,$$__B,$$__C)] = $$__U3_PREDEF[ABC]{__k2, __k1, __k3, __a, raw_params, __x, dV, ddV, dddV, FLATTEN, FIELDS_FLATTEN};
        this->u3_k3k1k2[FLATTEN($$__A,$$__B,$$__C)] = $$__U3_PREDEF[ABC]{__k3, __k1, __k2, __a, raw_params, __x, dV, ddV, dddV, FLATTEN, FIELDS_FLATTEN};

        // evolve the real and imaginary components of the 2pf
        // for the imaginary parts, index placement *does* matter so we must take care
        __dtwopf_re_k1($$__A, $$__B) $$=  + this->u2_k1[FLATTEN($$__A, $$__C)] * __twopf_re_k1($$__C, $$__B);
        __dtwopf_re_k1($$__A, $$__B) $$+= + this->u2_k1[FLATTEN($$__B, $$__C)] * __twopf_re_k1($$__A, $$__C);

        __dtwopf_im_k1($$__A, $$__B) $$=  + this->u2_k1[FLATTEN($$__A, $$__C)] * __twopf_im_k1($$__C, $$__B);
        __dtwopf_im_k1($$__A, $$__B) $$+= + this->u2_k1[FLATTEN($$__B, $$__C)] * __twopf_im_k1($$__A, $$__C);

        __dtwopf_re_k2($$__A, $$__B) $$=  + this->u2_k2[FLATTEN($$__A, $$__C)] * __twopf_re_k2($$__C, $$__B);
        __dtwopf_re_k2($$__A, $$__B) $$+= + this->u2_k2[FLATTEN($$__B, $$__C)] * __twopf_re_k2($$__A, $$__C);

        __dtwopf_im_k2($$__A, $$__B) $$=  + this->u2_k2[FLATTEN($$__A, $$__C)] * __twopf_im_k2($$__C, $$__B);
        __dtwopf_im_k2($$__A, $$__B) $$+= + this->u2_k2[FLATTEN($$__B, $$__C)] * __twopf_im_k2($$__A, $$__C);

        __dtwopf_re_k3($$__A, $$__B) $$=  + this->u2_k3[FLATTEN($$__A, $$__C)] * __twopf_re_k3($$__C, $$__B);
        __dtwopf_re_k3($$__A, $$__B) $$+= + this->u2_k3[FLATTEN($$__B, $$__C)] * __twopf_re_k3($$__A, $$__C);

        __dtwopf_im_k3($$__A, $$__B) $$=  + this->u2_k3[FLATTEN($$__A, $$__C)] * __twopf_im_k3($$__C, $$__B);
        __dtwopf_im_k3($$__A, $$__B) $$+= + this->u2_k3[FLATTEN($$__B, $$__C)] * __twopf_im_k3($$__A, $$__C);

        // evolve the components of the 3pf
        // index placement matters, partly because of the k-dependence
        // but also in the source terms from the imaginary components of the 2pf

        __dthreepf($$__A, $$__B, $$__C) $$=  + this->u2_k1[FLATTEN($$__A, $$__M)] * __threepf($$__M, $$__B, $$__C);
        __dthreepf($$__A, $$__B, $$__C) $$+= + this->u3_k1k2k3[FLATTEN($$__A, $$__M, $$__N)] * __twopf_re_k2($$__M, $$__B) * __twopf_re_k3($$__N, $$__C);
        __dthreepf($$__A, $$__B, $$__C) $$+= - this->u3_k1k2k3[FLATTEN($$__A, $$__M, $$__N)] * __twopf_im_k2($$__M, $$__B) * __twopf_im_k3($$__N, $$__C);

        __dthreepf($$__A, $$__B, $$__C) $$+= + this->u2_k2[FLATTEN($$__B, $$__M)] * __threepf($$__A, $$__M, $$__C);
        __dthreepf($$__A, $$__B, $$__C) $$+= + this->u3_k2k1k3[FLATTEN($$__B, $$__M, $$__N)] * __twopf_re_k1($$__A, $$__M) * __twopf_re_k3($$__N, $$__C);
        __dthreepf($$__A, $$__B, $$__C) $$+= - this->u3_k2k1k3[FLATTEN($$__B, $$__M, $$__N)] * __twopf_im_k1($$__A, $$__M) * __twopf_im_k3($$__N, $$__C);

        __dthreepf($$__A, $$__B, $$__C) $$+= + this->u2_k3[FLATTEN($$__C, $$__M)] * __threepf($$__A, $$__B, $$__M);
        __dthreepf($$__A, $$__B, $$__C) $$+= + this->u3_k3k1k2[FLATTEN($$__C, $$__M, $$__N)] * __twopf_re_k1($$__A, $$__M) * __twopf_re_k2($$__B, $$__N);
        __dthreepf($$__A, $$__B, $$__C) $$+= - this->u3_k3k1k2[FLATTEN($$__C, $$__M, $$__N)] * __twopf_im_k1($$__A, $$__M) * __twopf_im_k2($$__B, $$__N);
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename number>
    void $$__MODEL_basic_threepf_observer<number>::operator()(const threepf_state<number>& x, double t)
      {
        this->start_batching(t, this->get_log(), generic_batcher::log_severity_level::normal);
        this->push(x);
        this->stop_batching();
      }


    }   // namespace transport


#endif  // $$__GUARD
