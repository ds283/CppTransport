// backend=vexcl minver=0.06
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// CUDA implementation using the VexCL library and custom kernels

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include "transport-runtime-api/transport.h"
#include "transport-runtime-api/utilities/formatter.h"

#include "$$__CORE"

#define VEXCL_BACKEND_CUDA
// required for the fehlberg78 stepper to build
#define VEXCL_SPLIT_MULTIEXPRESSIONS

#include "vexcl/vexcl.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl.hpp"

#include <cuda.h>
#include <cuda_runtime_api.h>


namespace transport
  {
    // set up a state type for 2pf integration on an CUDA device
    // to avoid passing an excessive number of parameters to the CUDA kernels,
    // we pack everything into a single VexCL vector
    // of dimension $$__MODEL_pool::twopf_state_size * #k-configurations
    // NOTE - these are packed in groups, all k-configurations together, so that
    // the GPU can coalesce reads where possible
    typedef vex::vector<double> twopf_state;

    // define convenience macros to access the components of the twopf state
    // THESE MUST MATCH THE MACROS DEFINED IN EACH CUDA KERNEL, SO CHANGES HERE MUST BE REFLECTED THERE

    // background component i, for configuration number c out of n
    #define __TWOPF_BACKG(state, i, c, n)    state[($$__MODEL_pool::backg_start + i)*n + c]
    // twopf component (i,j), for configuration number c out of n
    #define __TWOPF_TWOPF(state, i, j, c, n) state[($$__MODEL_pool::twopf_start + i*2*$$__NUMBER_FIELDS + j)*n + c]

    // set up a state type for 3pf integration on an CUDA device
    // to avoid passing an excessive number of parameters to the CUDA kernels,
    // we pack everything into a single VexCL vector
    // of dimension $$__MODEL_pool::threepf_state_size * #k-configurations
    // NOTE - these are packed in groups, all k-configurations together, so that
    // the GPU can coalesce reads where possible
    typedef vex::vector<double> threepf_state;

    // define convenience macros to access the components of the threepf state
    // THESE MUST MATCH THE MACROS DEFINED IN EACH CUDA KERNEL, SO CHANGES HERE MUST BE REFLECTED THERE

    // background component i, for configuration number c out of n
    #define __THREEPF_BACKG(state, i, c, n)          state[($$__MODEL_pool::backg_start + i)*n + c]
    // real part of twopf for mode k1, component (i,j), for configuration c out of n
    #define __THREEPF_TWOPF_RE_K1(state, i, j, c, n) state[($$__MODEL_pool::twopf_re_k1_start + i*2*$$__NUMBER_FIELDS + j)*n + c]
    // imaginary part of twopf for mode k1, component (i,j), for configuration c out of n
    #define __THREEPF_TWOPF_IM_K1(state, i, j, c, n) state[($$__MODEL_pool::twopf_im_k1_start + i*2*$$__NUMBER_FIELDS + j)*n + c]
    // real part of twopf for mode k2, component (i,j), for configuration c out of n
    #define __THREEPF_TWOPF_RE_K2(state, i, j, c, n) state[($$__MODEL_pool::twopf_re_k2_start + i*2*$$__NUMBER_FIELDS + j)*n + c]
    // imaginary part of twopf for mode k2, component (i,j), for configuration c out of n
    #define __THREEPF_TWOPF_IM_K2(state, i, j, c, n) state[($$__MODEL_pool::twopf_im_k2_start + i*2*$$__NUMBER_FIELDS + j)*n + c]
    // real part of twopf for mode k3, component (i,j), for configuration c out of n
    #define __THREEPF_TWOPF_RE_K3(state, i, j, c, n) state[($$__MODEL_pool::twopf_re_k3_start + i*2*$$__NUMBER_FIELDS + j)*n + c]
    // imaginary part of twopf for mode k3, component (i,j), for configuration c out of n
    #define __THREEPF_TWOPF_IM_K3(state, i, j, c, n) state[($$__MODEL_pool::twopf_im_k3_start + i*2*$$__NUMBER_FIELDS + j)*n + c]
    // threepf, component (i,j,k), for configuration c out of n
    #define __THREEPF_THREEPF(state, i, j, k, c, n)  state[($$__MODEL_pool::threepf_start + i*2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS + j*2*$$__NUMBER_FIELDS + k)*n + c]

    // generic access to a twopf block with supplied start position, element (i,j), configuration c out of n
    #define __GENERIC_TWOPF(state, start, i, j, c, n) state[(start + i*2*$$__NUMBER_FIELDS + j)*n + c]

    // generic access to a threepf block with supplied start position, element (i,k,k), configuration c out of n
    #define __GENERIC_THREEPF(state, start, i, j, k, c, n) state[(start + i*2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS + j*2*$$__NUMBER_FIELDS + k)*n + c]


#define __CPP_TRANSPORT_VEXCL_CUDA "VexCL/CUDA"
    namespace $$__MODEL_pool
      {
        static std::string backend = "";
      }


    // *********************************************************************************************


    template <typename number>
    class $$__MODEL_vexcl : public $$__MODEL<number>
      {
      public:
        // worker number is used to pick a particular GPU out of the context
        // this means that, on machines with multiple GPUs, different MPI
        // workers can access the different GPUs
        // the snag is that the master node, worker number 0, must still
        // be initialized even though it does no work
        // to handle that, we make it share the first GPU even though it won't queue any work to it
        $$__MODEL_vexcl(instance_manager<number>* mgr, unsigned int w_number)
          : $$__MODEL<number>(mgr), ctx(vex::Filter::Any && vex::Filter::Position(w_number > 0 ? w_number-1 : 0))
          {
            if(this->ctx.size() != 1) throw runtime_exception(runtime_exception::BACKEND_ERROR, __CPP_TRANSPORT_SINGLE_GPU_ONLY);

            cudaGetDeviceProperties(&cuda_device_properties, ctx.device(0).raw());

            std::ostringstream backend_string;
            backend_string << __CPP_TRANSPORT_VEXCL_CUDA << " " << cuda_device_properties.name;
            $$__MODEL_pool::backend = backend_string.str();
          }

        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        virtual const std::string& get_backend() const override { return($$__MODEL_pool::backend); }

        // BACKEND INTERFACE

      public:

        // Set up a context
        virtual context backend_get_context();

        // Integrate background and 2-point function on a CUDA device
        virtual void backend_process_queue(work_queue<twopf_kconfig>& work, const integration_task<number>* tk,
                                           twopf_batcher<number>& batcher,
                                           bool silent = false) override;

        // Integrate background, 2-point function and 3-point function on a CUDA device
        virtual void backend_process_queue(work_queue<threepf_kconfig>& work, const integration_task<number>* tk,
                                           threepf_batcher<number>& batcher,
                                           bool silent = false) override;

        virtual unsigned int backend_twopf_state_size(void)   const override { return($$__MODEL_pool::twopf_state_size); }
        virtual unsigned int backend_threepf_state_size(void) const override { return($$__MODEL_pool::threepf_state_size); }

        virtual bool supports_per_configuration_statistics(void) const override { return(false); }


        // INTERNAL API

        protected:
          template <typename State, typename Config>
          void populate_twopf_ic(State& x, unsigned int start,
                                 const typename work_queue<Config>::device_work_list& list,
                                 std::function<double(const Config&)> visitor,
                                 double Ninit,
                                 const parameters<number>& p, const std::vector<number>& ics, bool imaginary=false);

          template <typename State>
          void populate_threepf_ic(State& x, unsigned int start,
                                   const work_queue<threepf_kconfig>::device_work_list& list, double Ninit,
                                   const parameters<number>& p, const std::vector<number>& ics);

          void populate_threepf_state_ic(threepf_state& x, const std::vector< struct threepf_kconfig >& kconfig_list,
                                         const std::vector<double>& k1s, const std::vector<double>& k2s, const std::vector<double>& k3s,
                                         double Ninit, const std::vector<number>& ic);

      private:
        vex::Context   ctx;

        cudaDeviceProp cuda_device_properties;
      };


    // integration - 2pf functor
    template <typename number>
    class $$__MODEL_vexcl_twopf_functor: public constexpr_flattener<$$__NUMBER_FIELDS>
      {

        public:

          $$__MODEL_vexcl_twopf_functor(vex::Context& c, const parameters<number>& p, const vex::vector<double>& ks)
            : ctx(c), params(p), k_list(ks)
            {
            }

          void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

        private:

          vex::Context&              ctx;
          const parameters<number>   params;

          const vex::vector<double>& k_list;

      };


    // integration - observer object for 2pf
    template <typename number>
    class $$__MODEL_vexcl_twopf_observer: public twopf_groupconfig_batch_observer<number>
      {

        public:

          $$__MODEL_vexcl_twopf_observer(twopf_batcher<number>& b,
                                         const work_queue<twopf_kconfig>::device_work_list& c,
                                         const std::vector<time_config>& l,
                                         double t_int=1.0, bool s=false, unsigned int p=3)
            : twopf_groupconfig_batch_observer<number>(b, c, l,
                                                       $$__MODEL_pool::backg_size, $$__MODEL_pool::twopf_size,
                                                       $$__MODEL_pool::backg_start, $$__MODEL_pool::twopf_start,
                                                       t_int, s, p)
            {
            }

          void operator()(const twopf_state& x, double t);

      };


    // integration - 3pf functor
    template <typename number>
    class $$__MODEL_vexcl_threepf_functor: public constexpr_flattener<$$__NUMBER_FIELDS>
      {

      public:

        $$__MODEL_vexcl_threepf_functor(vex::Context& c, const parameters<number>& p,
                                        const vex::vector<double>& k1s, const vex::vector<double>& k2s, const vex::vector<double>& k3s)
          : ctx(c), params(p), k1_list(k1s), k2_list(k2s), k3_list(k3s)
          {
          }

        void operator()(const threepf_state& __x, threepf_state& __dxdt, double __t);

      private:

        vex::Context&              ctx;
        const parameters<number>   params;

        const vex::vector<double>& k1_list;
        const vex::vector<double>& k2_list;
        const vex::vector<double>& k3_list;

      };


    // integration - observer object for 3pf
    template <typename number>
    class $$__MODEL_vexcl_threepf_observer: public threepf_groupconfig_batch_observer<number>
      {

      public:

        $$__MODEL_vexcl_threepf_observer(threepf_batcher<number>& b,
                                         const work_queue<threepf_kconfig>::device_work_list& c,
                                         const std::vector<time_config>& l,
                                         double t_int=1.0, bool s=false, unsigned int p=3)
          : threepf_groupconfig_batch_observer<number>(b, c, l
                                                       $$__MODEL_pool::backg_size, $$__MODEL_pool::twopf_size, $$__MODEL_pool::threepf_size,
                                                       $$__MODEL_pool::backg_start,
                                                       $$__MODEL_pool::twopf_re_k1_start, $$__MODEL_pool::twopf_im_k1_start,
                                                       $$__MODEL_pool::twopf_re_k2_start, $$__MODEL_pool::twopf_im_k2_start,
                                                       $$__MODEL_pool::twopf_re_k3_start, $$__MODEL_pool::twopf_im_k3_start,
                                                       $$__MODEL_pool::threepf_start,
                                                       t_int, s, p)
          {
          }

        void operator()(const threepf_state& x, double t);

      };


    // BACKEND INTERFACE


    // generate a context
    template <typename number>
    context $$__MODEL_vexcl<number>::backend_get_context(void)
      {
        context work_ctx;

        // add our CUDA device to the integration context
        // assume we can use all the memory on the device for storage -- this may require revisting
        // we divide by two because we need at least two copies of the current state vector
        // (actually, shared mem per block may be a bigger concern)
        work_ctx.add_device(this->cuda_device_properties.name, this->cuda_device_properties.totalGlobalMem/2,
                            context::device::bounded);

        return(work_ctx);
      }


    // process work queue for twopf
    template <typename number>
    void $$__MODEL_vexcl<number>::backend_process_queue(work_queue<twopf_kconfig>& work, const integration_task<number>* tk,
                                                        twopf_batcher<number>& batcher,
                                                        bool silent)
      {
        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal)
            << "** VexCL/CUDA compute backend processing twopf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << work_msg.str();
        std::cout << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const work_queue<twopf_kconfig>::device_queue queues = work[0];

        // there may be more than one queue if there are too many configurations to integrate
        // with our current GPU memory capacity
        for(unsigned int i = 0; i < queues.size(); i++)
          {
            try
              {
                const work_queue<twopf_kconfig>::device_work_list list = queues[i];

		            // set up list of time steps, and storage list
                std::vector<typename integration_task<number>::time_storage_record> slist;
                const std::vector<double>& times = tk->get_raw_integration_step_times(slist);

                // set up a functor to observe this system
                // also starts the timers running, so we do it as early as possible
                $$__MODEL_vexcl_twopf_observer<number> obs(batcher, list, slist);

                // integrate all the items on this work list

                // set up a state vector
                twopf_state dev_x(this->ctx.queue(), $$__MODEL_pool::twopf_state_size*list.size());

                // set up a host vector to store the initial conditions
                // it's not quite clear what the best way to set ics on the device, but here we aggregate everything
                // on the host and then copy to the device
                std::vector<double> hst_x($$__MODEL_pool::twopf_state_size*list.size());

                // intialize the device's copy of the k-modes
                vex::vector<double> dev_ks(this->ctx.queue(), list.size());

                std::vector<double> hst_ks(list.size());
                for(unsigned int j = 0; j < list.size(); j++)
                  {
                    hst_ks[j] = list[j].k_comoving;
                  }
                vex::copy(hst_ks, dev_ks);

                // set up a functor to evolve this system
                $$__MODEL_vexcl_twopf_functor<number> rhs(this->ctx, tk->get_params(), dev_ks);

                // fix initial conditions - background
                const std::vector<number> ics = tk->get_ics_vector();
                for(unsigned int j = 0; j < list.size(); j++)
                  {
                    __TWOPF_BACKG(hst_x, $$__A, j, list.size()) = $$// ics[$$__A];
                  }

                // fix initial conditions - 2pf
                std::function<double(const twopf_kconfig&)> v = [](const twopf_kconfig& c) -> double { return(c.k_comoving); };
                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_start,
                                        list, v, times.front(), tk->get_params(), ics);

                // copy ics to device
                vex::copy(hst_x, dev_x);

                using namespace boost::numeric::odeint;
                integrate_times($$__MAKE_PERT_STEPPER{twopf_state}, rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

                obs.stop_timers();
              }
            catch(std::overflow_error& xe)
              {
                batcher.report_integration_failure();

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::error) << "!! Integration failure in work list " << i;
              }
          }
      }


    // make initial conditions for each component of the 2pf
    // x         - state vector *containing* space for the 2pf (doesn't have to be entirely the 2pf)
    // start     - starting position of twopf components within the state vector
    // list      - work list of 2pf configurations
    // Ninit     - initial time
    // p         - parameters
    // ics       - iniitial conditions for the background fields (or fields+momenta)
    // imaginary - whether to populate using real or imaginary components of the 2pf
    template <typename number>
    template <typename State, typename Config>
    void $$__MODEL_vexcl<number>::populate_twopf_ic(State& x, unsigned int start,
                                                    const typename work_queue<Config>::device_work_list& list,
                                                    std::function<double(const Config&)> visitor,
                                                    double Ninit,
                                                    const parameters<number>& p, const std::vector<number>& ics, bool imaginary)
      {
        // scan through k-modes, assigning values to the (i,j)-th element
        for(unsigned int c = 0; c < list.size(); c++)
          {
            __GENERIC_TWOPF(x, start, $$__A, $$__B, c, list.size()) = imaginary ? this->make_twopf_im_ic($$__A, $$__B, visitor(list[c]), Ninit, p, ics) : this->make_twopf_re_ic($$__A, $$__B, visitor(list[c]), Ninit, p, ics) $$// ;
          }
      }


    // THREE-POINT FUNCTION INTEGRATION


    template <typename number>
    void $$__MODEL_vexcl<number>::backend_process_queue(work_queue<threepf_kconfig>& work, const integration_task<number>* tk,
                                                        threepf_batcher<number>& batcher,
                                                        bool silent)
      {
        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal)
            << "** VexCL/CUDA compute backend processing threepf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal) << work_msg.str();
        std::cout << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const work_queue<threepf_kconfig>::device_queue queues = work[0];

        // there may be more than one work list if there are too many configurations to integrate
        // with our current GPU memory capacity
        for(unsigned int i = 0; i < queues.size(); i++)
          {
            try
              {
                const work_queue<threepf_kconfig>::device_work_list list = queues[i];

		            // get list of time steps, and storage list
                std::vector<typename integration_task<number>::time_storage_record> slist;
                const std::vector<double>& times = tk->get_raw_integration_step_times(slist);

                // set up a functor to observe this system
                // this also starts the timers running, so we do it as early as possible
                $$__MODEL_vexcl_threepf_observer<number> obs(batcher, list, slist);

                // integrate all items on this work list
                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal)
                    << "** VexCL/CUDA compute backend: integrating triangles from work list " << i;
                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::normal)
                    << "**   " << list.size() << " items in this work list, GPU memory for state vector = " << format_memory($$__MODEL_pool::threepf_state_size*list.size());

                // initialize the device's copy of the k-modes
                vex::vector<double> dev_k1s(ctx.queue(), list.size());
                vex::vector<double> dev_k2s(ctx.queue(), list.size());
                vex::vector<double> dev_k3s(ctx.queue(), list.size());

                std::vector<double> hst_k1s(list.size());
                std::vector<double> hst_k2s(list.size());
                std::vector<double> hst_k3s(list.size());
                for(unsigned int j = 0; j < list.size(); j++)
                  {
                    hst_k1s[j] = list[j].k1_comoving;
                    hst_k2s[j] = list[j].k2_comoving;
                    hst_k3s[j] = list[j].k3_comoving;
                  }
                vex::copy(hst_k1s, dev_k1s);
                vex::copy(hst_k2s, dev_k2s);
                vex::copy(hst_k3s, dev_k3s);

                // set up a functor to evolve this system
                $$__MODEL_vexcl_threepf_functor<number> rhs(this->ctx, tk->get_params(), dev_k1s, dev_k2s, dev_k3s);

                // set up a state vector
                threepf_state dev_x(this->ctx.queue(), $$__MODEL_pool::threepf_state_size*list.size());

                // set up a host vector to store the initial conditions
                // it's not quite clear what the best way to set ics on the device, but here we aggregate everything
                // on the host and then copy to the device
                std::vector<double> hst_x($$__MODEL_pool::threepf_state_size*list.size());

                // fix initial conditions - background
                const std::vector<number> ics = tk->get_ics_vector();
                for(unsigned int j = 0; j < list.size(); j++)
                  {
                    __TWOPF_BACKG(hst_x, $$__A, j, list.size()) = $$// ics[$$__A];
                  }

                // fix initial conditions - real 2pfs
                std::function<double(const threepf_kconfig&)> v1 = [](const threepf_kconfig& c) -> double { return(c.k1_comoving); };
                std::function<double(const threepf_kconfig&)> v2 = [](const threepf_kconfig& c) -> double { return(c.k2_comoving); };
                std::function<double(const threepf_kconfig&)> v3 = [](const threepf_kconfig& c) -> double { return(c.k3_comoving); };

                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_re_k1_start,
                                        list, v1, times.front(), tk->get_params(), ics, false);
                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_re_k2_start,
                                        list, v2, times.front(), tk->get_params(), ics, false);
                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_re_k3_start,
                                        list, v3, times.front(), tk->get_params(), ics, false);

                // fix initial conditions - imaginary 2pfs
                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_im_k1_start,
                                        list, v1, times.front(), tk->get_params(), ics, true);
                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_im_k2_start,
                                        list, v2, times.front(), tk->get_params(), ics, true);
                this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_im_k3_start,
                                        list, v3, times.front(), tk->get_params(), ics, true);

                // fix initial conditions - 3pfs
                this->populate_threepf_ic(hst_x, $$__MODEL_pool::threepf_start, list, times.front(), tk->get_params(), ics);

                // copy ics to device
                vex::copy(hst_x, dev_x);

                using namespace boost::numeric::odeint;
                integrate_times($$__MAKE_PERT_STEPPER{threepf_state}, rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

                obs.stop_timers();
              }
            catch(std::overflow_error& xe)
              {
                batcher.report_integration_failure();

                BOOST_LOG_SEV(batcher.get_log(), generic_batcher::error) << "!! Integration failure in work list " << i;
              }
          }
      }


    template <typename number>
    template <typename State>
    void $$__MODEL_vexcl<number>::populate_threepf_ic(State& x, unsigned int start,
                                                      const work_queue<threepf_kconfig>::device_work_list& list, double Ninit,
                                                      const parameters<number>& p, const std::vector<number>& ics)
      {
        // scan through k-modes, assigning values to the (i,j,k)-th element
        for(int c = 0; c < list.size(); c++)
          {
            __GENERIC_THREEPF(x, start, $$__A, $$__B, $$__C, c, list.size()) = this->make_threepf_ic($$__A, $$__B, $$__C, list[c].k1_comoving, list[c].k2_comoving, list[c].k3_comoving, Ninit, p, ics) $$// ;
          }
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


    template <typename number>
    void $$__MODEL_vexcl_twopf_functor<number>::operator()(const twopf_state& __x, twopf_state& __dxdt, double __t)
      {
        const auto __a = exp(__t);

        std::vector<vex::backend::kernel> twopf_kernel;

        assert(this->ctx.size() == 1);

        // build a kernel to evolve the twopf and background combined
        twopf_kernel.emplace_back(this->ctx.queue(0),
                                  $$__IMPORT_KERNEL{vexcl-cuda/twopf.cu, twopffused, );}

        struct
          {
            size_t operator()(size_t w_size)
              {
                return(1 + $$__MODEL_pool::twopf_size + $$__MODEL_pool::u2_size)*w_size*sizeof(double);
              }
          } twopf_kernel_smem_functor;

        // apply the twopf kernel
        twopf_kernel[0].push_arg<unsigned long long>(this->k_list.part_size(0));
        twopf_kernel[0].push_arg(this->params.get_Mp());
        twopf_kernel[0].push_arg(this->params.get_vector()[$$__1]); $$//
        twopf_kernel[0].push_arg(__x(0));
        twopf_kernel[0].push_arg(__dxdt(0));
        twopf_kernel[0].push_arg(this->k_list(0));
        twopf_kernel[0].push_arg(__a);

        // set amount of shared memory required per block
        // VexCL passes us the block dimension, so we have to multiply by the required shared memory per thread
        twopf_kernel[0].config(ctx.queue(0), twopf_kernel_smem_functor);
        twopf_kernel[0].set_smem(twopf_kernel_smem_functor);

        twopf_kernel[0](this->ctx.queue(0));
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


    template <typename number>
    void $$__MODEL_vexcl_twopf_observer<number>::operator()(const twopf_state& x, double t)
      {
        this->start_batching(t, this->get_log(), generic_batcher::normal);

        // allocate storage for state, then copy device vector
        std::vector<double> hst_x($$__MODEL_pool::twopf_state_size*this->group_size());
        vex::copy(x, hst_x);

        // push to batcher
        this->push(hst_x);

        this->stop_batching();
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


    template <typename number>
    void $$__MODEL_vexcl_threepf_functor<number>::operator()(const threepf_state& __x, threepf_state& __dxdt, double __t)
      {
        const auto __a = exp(__t);

        std::vector<vex::backend::kernel> threepf_kernel;

        assert(this->ctx.size() == 1);

        // build a kernel to evolve the threepf, twopf and backg together
        threepf_kernel.emplace_back(this->ctx.queue(0),
                                    $$__IMPORT_KERNEL{vexcl-cuda/threepf.cu, threepffused, );}

        struct
          {
            size_t operator()(size_t w_size)
              {
                return(3*(1 + 2*$$__MODEL_pool::twopf_size + $$__MODEL_pool::u2_size + $$__MODEL_pool::u3_size) + $$__MODEL_pool::threepf_size)*w_size*sizeof(double);
              }
          } threepf_kernel_smem_functor;

        // apply the threepf kernel
        threepf_kernel[0].push_arg<unsigned long long>(this->k1_list.part_size(0));
        threepf_kernel[0].push_arg(this->params.get_Mp());
        threepf_kernel[0].push_arg(this->params.get_vector()[$$__1]); $$//
        threepf_kernel[0].push_arg(__x(0));
        threepf_kernel[0].push_arg(__dxdt(0));
        threepf_kernel[0].push_arg(this->k1_list(0));
        threepf_kernel[0].push_arg(this->k2_list(0));
        threepf_kernel[0].push_arg(this->k3_list(0));
        threepf_kernel[0].push_arg(__a);

        // set amount of shared memory required per block
        // VexCL passes us the block dimension, so we have to multiply by the required shared memory per thread
        threepf_kernel[0].config(ctx.queue(0), threepf_kernel_smem_functor);
        threepf_kernel[0].set_smem(threepf_kernel_smem_functor);

        threepf_kernel[0](this->ctx.queue(0));
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename number>
    void $$__MODEL_vexcl_threepf_observer<number>::operator()(const threepf_state& x, double t)
      {
        this->start_batching(t, this->get_log(), generic_batcher::normal);

        // allocate storage, then copy device vector to host in one shot
        std::vector<double> hst_x($$__MODEL_pool::threepf_state_size*this->group_size());
        vex::copy(x, hst_x);

        // push to the batcher
        this->push(hst_x);

        this->stop_batching();
      }


  }   // namespace transport


#endif  // $$__GUARD
