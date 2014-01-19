// backend=vexcl minver=0.06
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// OpenCL implementation using the VexCL library and custom kernels

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include "transport/utilities/formatter.h"

#include "transport/transport.h"

#include "$$__CORE"

#define VEXCL_BACKEND_OPENCL
// required for the fehlberg78 stepper to build
#define VEXCL_SPLIT_MULTIEXPRESSIONS

#include "vexcl/vexcl.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl.hpp"


namespace transport
  {
    // set up a state type for 2pf integration on an OpenCL device
    // to avoid passing an excessive number of parameters to the OpenCL kernels,
    // we pack everything into a single VexCL vector
    // of dimension $$__MODEL_pool::twopf_state_size * #k-configurations
    // NOTE - these are packed in groups, all k-configurations together, so that
    // the GPU can coalesce reads where possible
    typedef vex::vector<double> twopf_state;

    // define convenience macros to access the components of the twopf state
    // THESE MUST MATCH THE MACROS DEFINED IN EACH OpenCL KERNEL, SO CHANGES HERE MUST BE REFLECTED THERE

    // background component i, for configuration number c out of n
    #define __TWOPF_BACKG(state, i, c, n)    state[($$__MODEL_pool::backg_start + i)*n + c]
    // twopf component (i,j), for configuration number c out of n
    #define __TWOPF_TWOPF(state, i, j, c, n) state[($$__MODEL_pool::twopf_start + i*2*$$__NUMBER_FIELDS + j)*n + c]

    // set up a state type for 3pf integration on an OpenCL device
    // to avoid passing an excessive number of parameters to the OpenCL kernels,
    // we pack everything into a single VexCL vector
    // of dimension $$__MODEL_pool::threepf_state_size * #k-configurations
    // NOTE - these are packed in groups, all k-configurations together, so that
    // the GPU can coalesce reads where possible
    typedef vex::vector<double> threepf_state;

    // define convenience macros to access the components of the threepf state
    // THESE MUST MATCH THE MACROS DEFINED IN EACH OpenCL KERNEL, SO CHANGES HERE MUST BE REFLECTED THERE

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


      // *********************************************************************************************


      template <typename number>
      class $$__MODEL_vexcl : public $$__MODEL<number>
        {
        public:
          $$__MODEL_vexcl(instance_manager<number>* mgr)
            : $$__MODEL<number>(mgr), ctx(vex::Filter::Type(CL_DEVICE_TYPE_GPU) && vex::Filter::DoublePrecision)
            {
              if(this->ctx.size() != 1) throw runtime_exception(runtime_exception::BACKEND_ERROR, __CPP_TRANSPORT_SINGLE_GPU_ONLY);
            }

          // BACKEND INTERFACE

        public:
          // Set up a context
          context backend_get_context();

          // Integrate background and 2-point function on a OpenCL device
          void backend_process_twopf(work_queue<twopf_kconfig>& work, const task<number>* tk,
                                     typename data_manager<number>::twopf_batcher& batcher,
                                     bool silent=false);

          // Integrate background, 2-point function and 3-point function on a OpenCL device
          void backend_process_threepf(work_queue<threepf_kconfig>& work, const task<number>* tk,
                                       typename data_manager<number>::threepf_batcher& batcher,
                                       bool silent=false);

          unsigned int backend_twopf_state_size(void)   { return($$__MODEL_pool::twopf_state_size); }
          unsigned int backend_threepf_state_size(void) { return($$__MODEL_pool::threepf_state_size); }

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
        $$__MODEL_vexcl_twopf_observer(typename data_manager<number>::twopf_batcher& b,
                                       const work_queue<twopf_kconfig>::device_work_list& c,
                                       double t_int=1.0, bool s=false, unsigned int p=3)
          : twopf_groupconfig_batch_observer<number>(b, c, $$__MODEL_pool::backg_size, $$__MODEL_pool::twopf_size,
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
        $$__MODEL_vexcl_threepf_observer(typename data_manager<number>::threepf_batcher& b,
                                         const work_queue<threepf_kconfig>::device_work_list& c,
                                         double t_int=1.0, bool s=false, unsigned int p=3)
          : threepf_groupconfig_batch_observer<number>(b, c,
                                                       $$__MODEL_pool::backg_size, $$__MODEL_pool::twopf_size, $$__MODEL_pool::threepf_size,
                                                       $$__MODEL_pool::backg_start, $$__MODEL_pool::twopf_re_k1_start, $$__MODEL_pool::twopf_im_k1_start, $$__MODEL_pool::threepf_start,
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

        // add our OpenCL device to the integration context
        // assume we can use all the memory on the device for storage -- this may require revisting
        // we divide by two because we need at least two copies of the current state vector
        // (actually, shared mem per block may be a bigger concern)
        cl::Device dev = ctx.device(0);
        unsigned long global_mem = dev.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
        std::string device_name  = dev.getInfo<CL_DEVICE_NAME>();

        work_ctx.add_device(device_name, global_mem/2, context::device::bounded);

        return(work_ctx);
      }


    // process work queue for twopf
    template <typename number>
    void $$__MODEL_vexcl<number>::backend_process_twopf(work_queue<twopf_kconfig>& work, const task<number>* tk,
                                                        typename data_manager<number>::twopf_batcher& batcher,
                                                        bool silent)
      {
        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
            << "** VexCL/OpenCL compute backend processing twopf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << work_msg.str();
        std::cout << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const work_queue<twopf_kconfig>::device_queue queues = work[0];

        // there may be more than one queue if there are too many configurations to integrate
        // with our current GPU memory capacity
        for(unsigned int i = 0; i < queues.size(); i++)
          {
            const work_queue<twopf_kconfig>::device_work_list list = queues[i];

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
                hst_ks[j] = list[j].k;
              }
            vex::copy(hst_ks, dev_ks);

            // set up a functor to evolve this system
            $$__MODEL_vexcl_twopf_functor<number> rhs(this->ctx, tk->get_params(), dev_ks);

            // set up a functor to observe this system
            $$__MODEL_vexcl_twopf_observer<number> obs(batcher, list);

            // fix initial conditions - background
            const std::vector<number>& ics = tk->get_initial_conditions();
            for(unsigned int j = 0; j < list.size(); j++)
              {
                __TWOPF_BACKG(hst_x, $$__A, j, list.size()) = $$// ics[$$__A];
              }

            // fix initial conditions - 2pf
            const std::vector<double>& times = tk->get_sample_times();
            std::function<double(const twopf_kconfig&)> v = [](const twopf_kconfig& c) -> double { return(c.k); };
            this->populate_twopf_ic(hst_x, $$__MODEL_pool::twopf_start,
                                    list, v, times.front(), tk->get_params(), ics);

            // copy ics to device
            vex::copy(hst_x, dev_x);

            using namespace boost::numeric::odeint;
            integrate_times($$__MAKE_PERT_STEPPER{twopf_state}, rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);
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
    void $$__MODEL_vexcl<number>::backend_process_threepf(work_queue<threepf_kconfig>& work, const task<number>* tk,
                                                          typename data_manager<number>::threepf_batcher& batcher,
                                                          bool silent)
      {
        std::ostringstream work_msg;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal)
            << "** VexCL/OpenCL compute backend processing threepf task";
        work_msg << work;
        BOOST_LOG_SEV(batcher.get_log(), data_manager<number>::normal) << work_msg.str();
        std::cout << work_msg.str();
        if(!silent) this->write_task_data(tk, batcher, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

        // get work queue for the zeroth device (should be the only device in this backend)
        assert(work.size() == 1);
        const work_queue<threepf_kconfig>::device_queue queues = work[0];

        // there may be more than one queue if there are too many configurations to integrate
        // with our current GPU memory capacity
        for(unsigned int i = 0; i < queues.size(); i++)
          {
            const work_queue<threepf_kconfig>::device_work_list list = queues[i];

            // integrate all items on this work list

            // initialize the device's copy of the k-modes
            vex::vector<double> dev_k1s(ctx.queue(), list.size());
            vex::vector<double> dev_k2s(ctx.queue(), list.size());
            vex::vector<double> dev_k3s(ctx.queue(), list.size());

            std::vector<double> hst_k1s(list.size());
            std::vector<double> hst_k2s(list.size());
            std::vector<double> hst_k3s(list.size());
            for(unsigned int j = 0; j < list.size(); j++)
              {
                hst_k1s[j] = list[j].k1;
                hst_k2s[j] = list[j].k2;
                hst_k3s[j] = list[j].k3;
              }
            vex::copy(hst_k1s, dev_k1s);
            vex::copy(hst_k2s, dev_k2s);
            vex::copy(hst_k3s, dev_k3s);

            // set up a functor to evolve this system
            $$__MODEL_vexcl_threepf_functor<number> rhs(this->ctx, tk->get_params(), dev_k1s, dev_k2s, dev_k3s);

            // set up a functor to observe this system
            $$__MODEL_vexcl_threepf_observer<number> obs(batcher, list);

            // set up a state vector
            threepf_state dev_x(this->ctx.queue(), $$__MODEL_pool::threepf_state_size*list.size());

            // set up a host vector to store the initial conditions
            // it's not quite clear what the best way to set ics on the device, but here we aggregate everything
            // on the host and then copy to the device
            std::vector<double> hst_x($$__MODEL_pool::threepf_state_size*list.size());

            // fix initial conditions - background
            const std::vector<number>& ics = tk->get_initial_conditions();
            for(unsigned int j = 0; j < list.size(); j++)
              {
                __TWOPF_BACKG(hst_x, $$__A, j, list.size()) = $$// ics[$$__A];
              }

            // fix initial conditions - real 2pfs
            const std::vector<double>& times = tk->get_sample_times();

            std::function<double(const threepf_kconfig&)> v1 = [](const threepf_kconfig& c) -> double { return(c.k1); };
            std::function<double(const threepf_kconfig&)> v2 = [](const threepf_kconfig& c) -> double { return(c.k2); };
            std::function<double(const threepf_kconfig&)> v3 = [](const threepf_kconfig& c) -> double { return(c.k3); };

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
            __GENERIC_THREEPF(x, start, $$__A, $$__B, $$__C, c, list.size()) = this->make_threepf_ic($$__A, $$__B, $$__C, list[c].k1, list[c].k2, list[c].k3, Ninit, p, ics) $$// ;
          }
      }


      // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


      template <typename number>
      void $$__MODEL_vexcl_twopf_functor<number>::operator()(const twopf_state& __x, twopf_state& __dxdt, double __t)
        {
          const auto __a = exp(__t);

          std::vector<vex::backend::kernel> twopf_kernel;

          assert(this->ctx.size() == 1);

          std::cerr << "Emplacing kernel ..." << std::endl;
          // build a kernel to evolve the twopf and background combined
          twopf_kernel.emplace_back(this->ctx.queue(0),
                                    $$__IMPORT_KERNEL{vexcl-opencl/twopf.cl, twopffused, );}
          std::cerr << "... done" << std::endl;

          struct
            {
              size_t operator()(size_t w_size)
                {
                  return(1 + $$__MODEL_pool::twopf_size + $$__MODEL_pool::u2_size)*w_size*sizeof(cl_double);
                }
            } twopf_kernel_smem_functor;

          // apply the twopf kernel
          twopf_kernel[0].push_arg<cl_ulong>(this->k_list.part_size(0));
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
        this->start(t, this->get_log(), data_manager<number>::normal);

        // allocate storage for state, then copy device vector
        std::vector<double> hst_x($$__MODEL_pool::twopf_state_size*this->group_size());
        vex::copy(x, hst_x);

        // push to batcher
        this->push(hst_x);

        this->stop();
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
                                    $$__IMPORT_KERNEL{vexcl-opencl/threepf.cl, threepffused, );}

        struct
          {
            size_t operator()(size_t w_size)
              {
                return(3*(1 + 2*$$__MODEL_pool::twopf_size + $$__MODEL_pool::u2_size + $$__MODEL_pool::u3_size) + $$__MODEL_pool::threepf_size)*w_size*sizeof(cl_double);
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
        this->start(t, this->get_log(), data_manager<number>::normal);

        // allocate storage, then copy device vector to host in one shot
        std::vector<double> hst_x($$__MODEL_pool::threepf_state_size*this->group_size());
        vex::copy(x, hst_x);

        // push to the batcher
        this->push(hst_x);

        this->stop();
      }



  }   // namespace transport


#endif  // $$__GUARD
