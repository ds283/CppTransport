// backend=vexcl minver=0.06
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// custom OpenCL implementation using the VexCL library and custom kernels

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

// required for the fehlberg78 stepper to build
#define VEXCL_SPLIT_MULTIEXPRESSIONS

#include "transport/transport.h"

#include "$$__CORE"

#include "vexcl/vexcl.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl.hpp"


namespace transport
  {
      // set up a state type for 2pf integration on an OpenCL device
      typedef vex::multivector<double, $$__MODEL_pool::twopf_state_size> twopf_state;

      // set up a state type for 3pf integration on an OpenCL device
      typedef vex::multivector<double, $$__MODEL_pool::threepf_state_size> threepf_state;


      // *********************************************************************************************


      template <typename number>
      class $$__MODEL_vexcl : public $$__MODEL<number>
        {
          public:
            $$__MODEL_vexcl(number Mp, const std::vector<number>& ps)
              : $$__MODEL<number>(Mp, ps)
              {
              }

            // Integrate background and 2-point function on an OpenCL device
            transport::twopf<number>
              twopf(vex::Context& ctx, const std::vector<double>& ks, double Nstar,
                    const std::vector<number>& ics, const std::vector<double>& times,
                    bool silent=false);

            // Integrate background, 2-point function and 3-point function on an OpenCL device
            // this sample implementation works on a cubic lattice of k-modes
            transport::threepf<number>
              threepf(vex::Context& ctx, const std::vector<double>& ks, double Nstar,
                      const std::vector<number>& ics, const std::vector<double>& times,
                      bool silent=false);

          protected:
            template <typename State>
            void populate_twopf_ic(State& x, unsigned int start, const std::vector<double>& kmodes, double Ninit,
                                   const std::vector<number>& ic, bool imaginary = false);

            template <typename State>
            void populate_threepf_ic(State& x, unsigned int start,
                                     const std::vector<double>& k1s, const std::vector<double>& k2s, const std::vector<double>& k3s,
                                     double Ninit, const std::vector<number>& ic);

            void populate_threepf_state_ic(threepf_state& x, const std::vector< struct threepf_kconfig >& kconfig_list,
                                           const std::vector<double>& k1s, const std::vector<double>& k2s, const std::vector<double>& k3s,
                                           double Ninit, const std::vector<number>& ic);
        };


      // integration - 2pf functor
      template <typename number>
      class $$__MODEL_vexcl_twopf_functor
        {
          public:
            $$__MODEL_vexcl_twopf_functor(vex::Context& c,
                                          const std::vector<number>& p, const number Mp,
                                          const vex::vector<double>& ks,
                                          vex::multivector<double, $$__MODEL_pool::u2_size>& u2)
              : ctx(c), parameters(p), M_Planck(Mp), k_list(ks), u2_tensor(u2)
              {
              }

            void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
            // constexpr version for rapid evaluation during integration
            constexpr unsigned int flatten(unsigned int a)                                 { return(a); }
            constexpr unsigned int flatten(unsigned int a, unsigned int b)                 { return(2*$$__NUMBER_FIELDS*a + b); }
            constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); }

            vex::Context&                                      ctx;

            const number						                           M_Planck;
            const std::vector<number>&                         parameters;
            const vex::vector<double>&                         k_list;
            vex::multivector<double, $$__MODEL_pool::u2_size>& u2_tensor;
        };


      // integration - observer object for 2pf
      template <typename number>
      class $$__MODEL_vexcl_twopf_observer
        {
          public:
            $$__MODEL_vexcl_twopf_observer(std::vector< std::vector<number> >& bh,
                                           std::vector< std::vector< std::vector<number> > >& tpfh, unsigned int ks)
              : background_history(bh), twopf_history(tpfh), k_size(ks) {}
            void operator()(const twopf_state& x, double t);

          private:
            std::vector< std::vector<number> >&                background_history;
            std::vector< std::vector< std::vector<number> > >& twopf_history;

            const unsigned int                                 k_size;    // number of k-modes we are integrating
        };


    // integration - 3pf functor
    template <typename number>
    class $$__MODEL_vexcl_threepf_functor
      {
      public:
        $$__MODEL_vexcl_threepf_functor(vex::Context& c,
          const std::vector<number>& p, const number Mp,
          const vex::vector<double>& k1s, const vex::vector<double>& k2s, const vex::vector<double>& k3s,
          vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k1, vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k2, vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k3,
          vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k1k2k3, vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k2k1k3, vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k3k1k2)
          : ctx(c), parameters(p), M_Planck(Mp), k1_list(k1s), k2_list(k2s), k3_list(k3s),
            u2_k1_tensor(u2_k1), u2_k2_tensor(u2_k2), u2_k3_tensor(u2_k3),
            u3_k1k2k3_tensor(u3_k1k2k3), u3_k2k1k3_tensor(u3_k2k1k3), u3_k3k1k2_tensor(u3_k3k1k2)
          {
          }

        void operator()(const threepf_state& __x, threepf_state& __dxdt, double __t);

      private:
        // constexpr version for rapid evaluation during integration
        constexpr unsigned int flatten(unsigned int a)                                 { return(a); }
        constexpr unsigned int flatten(unsigned int a, unsigned int b)                 { return(2*$$__NUMBER_FIELDS*a + b); }
        constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); }

        vex::Context&                                      ctx;

        const number						                           M_Planck;
        const std::vector<number>&                         parameters;

        const vex::vector<double>&                         k1_list;
        const vex::vector<double>&                         k2_list;
        const vex::vector<double>&                         k3_list;

        vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k1_tensor;
        vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k2_tensor;
        vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k3_tensor;

        vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k1k2k3_tensor;
        vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k2k1k3_tensor;
        vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k3k1k2_tensor;
      };


    // integration - observer object for 3pf
    template <typename number>
    class $$__MODEL_vexcl_threepf_observer
      {
      public:
        $$__MODEL_vexcl_threepf_observer(std::vector< std::vector<number> >& bh,
          std::vector< std::vector< std::vector<number> > >& tpf_re_h, std::vector< std::vector< std::vector<number> > >& tpf_im_h,
          std::vector< std::vector< std::vector<number> > >& thpf_h,
          const std::vector< struct threepf_kconfig >& kc_l)
          : background_history(bh), twopf_re_history(tpf_re_h), twopf_im_history(tpf_im_h), threepf_history(thpf_h), kconfig_list(kc_l)
          {
          }

        void operator()(const threepf_state& x, double t);

      private:
        std::vector< std::vector<number> >&                background_history;
        std::vector< std::vector< std::vector<number> > >& twopf_re_history;
        std::vector< std::vector< std::vector<number> > >& twopf_im_history;
        std::vector< std::vector< std::vector<number> > >& threepf_history;

        const std::vector< struct threepf_kconfig >&       kconfig_list;
      };


      // TWO-POINT FUNCTION INTEGRATION


      // ctx    -- VexCL compute context
      // ks     -- vector of *conventionally-normalized* wavenumbers for which we wish
      //           to compute the twopf
      //           "conventional-normalization" means k=1 is the mode which crosses the horizon at N=Nstar
      // Nstar  -- horizon-exit of the mode with k-comoving = 1 takes place at Nstar e-folds
      // ics    -- vector of initial conditions for background fields (or fields+momenta)
      // times  -- vector of times at which the solution will be recorded
      // silent -- set to true to suppress verbose output
      template <typename number>
      transport::twopf<number> $$__MODEL_vexcl<number>::twopf(vex::Context& ctx,
                                                              const std::vector<double>& ks, double Nstar,
                                                              const std::vector<number>& ics, const std::vector<double>& times,
                                                              bool silent)
        {
          this->validate_times(times, Nstar);

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> hst_bg = ics;
          this->fix_initial_conditions(ics, hst_bg);
          if(!silent)
            {
              this->write_initial_conditions(hst_bg, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");
            }

          // convert conventionally-normalized wavenumbers to
          // properly normalized comoving wavenumbers
          std::vector<double> com_ks = this->normalize_comoving_ks(hst_bg, ks, *(times.begin()), Nstar, silent);

          // allocate space for storing the solution
          // the index convention is:
          //   first index  - time
          //   second index - component number
          //   third index  - k mode
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_history;

          // SET UP DATA ON THE OPENCL DEVICE
        
          // initialize the device's copy of the k-modes
          vex::vector<double> dev_ks(ctx.queue(), com_ks);
        
          // allocate space on the device for the u2-tensor
          vex::multivector<double, $$__MODEL_pool::u2_size> u2_tensor(ctx.queue(), com_ks.size());

          // set up state vector, and populate it with initial conditions for the background and twopf
          twopf_state dev_x(ctx.queue(), com_ks.size());

          // 1 - background
          dev_x($$__MODEL_pool::backg_start + this->flatten($$__A)) = $$// hst_bg[this->fast_flatten($$__A)];
        
          // 2 - twopf
          this->populate_twopf_ic(dev_x, $$__MODEL_pool::twopf_start, com_ks, *times.begin(), hst_bg);

          // set up a functor to evolve this system
          $$__MODEL_vexcl_twopf_functor<number> rhs(ctx, this->parameters, this->M_Planck, dev_ks, u2_tensor);
        
          // set up a functor to observe the integration
          $$__MODEL_vexcl_twopf_observer<number> obs(background_history, twopf_history, ks.size());

          using namespace boost::numeric::odeint;
          integrate_times($$__MAKE_PERT_STEPPER{twopf_state}, rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

          transport::twopf<number> tpf(ks, com_ks, Nstar, times, background_history, twopf_history, this);

          return(tpf);
        }


      // make initial conditions for each component of the 2pf
      // x         - state vector *containing* space for the 2pf (doesn't have to be entirely the 2pf)
      // start     - starting position of twopf components within the state vector
      // kmodes    - *comoving normalized* wavenumber for which we will compute the twopf
      // Ninit     - initial time
      // ics       - iniitial conditions for the background fields (or fields+momenta)
      // imaginary - whether to populate using real or imaginary components of the 2pf
      template <typename number>
      template <typename State>
      void $$__MODEL_vexcl<number>::populate_twopf_ic(State& x, unsigned int start, const std::vector<double>& kmodes,
        double Ninit, const std::vector<number>& ics, bool imaginary)
        {
        // TODO - need some form of introspection to determine size of state vector
        // but here assume it is ok
//          assert(x.size() >= start);
//          assert(x.size() >= start + $$__MODEL_pool::twopf_size);

          std::vector<double> hst_tp_ic(kmodes.size());

          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
                {
                  // populate hst_tp_ic with appropriate ics for the (i,j)th component, for all k modes
                  for(int k = 0; k < kmodes.size(); k++)
                    {
                      hst_tp_ic[k] = imaginary ? this->make_twopf_im_ic(i, j, kmodes[k], Ninit, ics) : this->make_twopf_re_ic(i, j, kmodes[k], Ninit, ics);
                    }

                  vex::copy(hst_tp_ic, x(start + this->flatten(i,j)));
                }
            }
        }


      // THREE-POINT FUNCTION INTEGRATION


      template <typename number>
      transport::threepf<number> $$__MODEL_vexcl<number>::threepf(vex::Context& ctx, const std::vector<double>& ks, double Nstar,
                                                                  const std::vector<number>& ics, const std::vector<double>& times,
                                                                  bool silent)
        {
          this->validate_times(times, Nstar);

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> hst_bg = ics;
          this->fix_initial_conditions(ics, hst_bg);
          if(!silent)
            {
              this->write_initial_conditions(hst_bg, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");
            }

          // convert conventionally-normalized wavenumbers to
          // properly normalized comoving wavenumbers
          std::vector<double> com_ks = this->normalize_comoving_ks(hst_bg, ks, *(times.begin()), Nstar, silent);

          // allocate space for storing the solution
          // the index convention is
          //   first index  - time
          //   second index - component number
          //   third index  = k mode
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_re_history;
          std::vector< std::vector< std::vector<number> > > twopf_im_history;
          std::vector< std::vector< std::vector<number> > > threepf_history;

          std::vector< struct threepf_kconfig >             kconfig_list;
          this->populate_kconfig_list(kconfig_list, com_ks);

          // SET UP DATA ON THE OPENCL DEVICE

          // initialize the device's copy of the k-modes
          vex::vector<double> dev_k1s(ctx.queue(), kconfig_list.size());
          vex::vector<double> dev_k2s(ctx.queue(), kconfig_list.size());
          vex::vector<double> dev_k3s(ctx.queue(), kconfig_list.size());

          std::vector<double> hst_k1s(kconfig_list.size());
          std::vector<double> hst_k2s(kconfig_list.size());
          std::vector<double> hst_k3s(kconfig_list.size());
          for(int i = 0; i < kconfig_list.size(); i++)
            {
              hst_k1s[i] = com_ks[kconfig_list[i].index[0]];
              hst_k2s[i] = com_ks[kconfig_list[i].index[1]];
              hst_k3s[i] = com_ks[kconfig_list[i].index[2]];
            }
          vex::copy(hst_k1s, dev_k1s);
          vex::copy(hst_k2s, dev_k2s);
          vex::copy(hst_k3s, dev_k3s);

          // allocate space on the device for the u2- and u3-tensors
          vex::multivector<double, $$__MODEL_pool::u2_size> u2_k1_tensor(ctx.queue(), kconfig_list.size());
          vex::multivector<double, $$__MODEL_pool::u2_size> u2_k2_tensor(ctx.queue(), kconfig_list.size());
          vex::multivector<double, $$__MODEL_pool::u2_size> u2_k3_tensor(ctx.queue(), kconfig_list.size());

          vex::multivector<double, $$__MODEL_pool::u3_size> u3_k1k2k3_tensor(ctx.queue(), kconfig_list.size());
          vex::multivector<double, $$__MODEL_pool::u3_size> u3_k2k1k3_tensor(ctx.queue(), kconfig_list.size());
          vex::multivector<double, $$__MODEL_pool::u3_size> u3_k3k1k2_tensor(ctx.queue(), kconfig_list.size());

          // set up state vector, and populate it with initial conditions for the background, twopf and threepf
          threepf_state dev_x(ctx.queue(), kconfig_list.size());
          this->populate_threepf_state_ic(dev_x, kconfig_list, hst_k1s, hst_k2s, hst_k3s, *times.begin(), hst_bg);

          // set up a functor to evolve this system
          $$__MODEL_vexcl_threepf_functor<number> rhs(ctx, this->parameters, this->M_Planck, dev_k1s, dev_k2s, dev_k3s,
                                                      u2_k1_tensor, u2_k2_tensor, u2_k3_tensor,
                                                      u3_k1k2k3_tensor, u3_k2k1k3_tensor, u3_k3k1k2_tensor);

          // set up a functor to observe the integration
          $$__MODEL_vexcl_threepf_observer<number> obs(background_history, twopf_re_history, twopf_im_history, threepf_history, kconfig_list);

          using namespace boost::numeric::odeint;
          integrate_times($$__MAKE_PERT_STEPPER{threepf_state}, rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

          transport::threepf<number> tpf(ks, com_ks, Nstar, times, background_history, twopf_re_history, twopf_im_history, threepf_history, kconfig_list, this);

          return(tpf);
        }


      template <typename number>
      template <typename State>
      void $$__MODEL_vexcl<number>::populate_threepf_ic(State& x, unsigned int start,
        const std::vector<double>& k1s, const std::vector<double>& k2s, const std::vector<double>& k3s,
        double Ninit, const std::vector<number>& ic)
        {
          // TODO - need some form of introspection to determine size of state vector
          // but here assume it is ok
//          assert(x.size() >= start);
//          assert(x.size() >= start + $$__MODEL_pool::threepf_size);
          assert(k1s.size() == k2s.size());
          assert(k1s.size() == k3s.size());

          std::vector<double> hst_tp_ic(k1s.size());

          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
                {
                  for(int k = 0; k < 2*$$__NUMBER_FIELDS; k++)
                    {
                      // populate hst_tp_ic with appropriate ics for the (i,j,k)th component
                      for(int m = 0; m < k1s.size(); m++)
                        {
                          hst_tp_ic[m] = this->make_threepf_ic(i, j, k, k1s[m], k2s[m], k3s[m], Ninit, ic);
                        }

                      vex::copy(hst_tp_ic, x(start + this->flatten(i,j,k)));
                    }
                }
            }
        }


      template <typename number>
      void $$__MODEL_vexcl<number>::populate_threepf_state_ic(threepf_state& x, const std::vector< struct threepf_kconfig >& kconfig_list,
        const std::vector<double>& k1s, const std::vector<double>& k2s, const std::vector<double>& k3s,
        double Ninit, const std::vector<number>& ics)
        {
          // we have to store:
          //   - 1 copy of the background, the same for each k
          //   - the real 2pf, for each of the kmodes in k1s, k2s, k3s
          //   - the imaginary 2pf, for each of the kmodes in k1s, k2s, k3s
          //   - the real 3pf

          // fix background initial conditions
          x($$__MODEL_pool::backg_start + this->flatten($$__A)) = $$// ics[$$__A];

          // fix initial conditions - real 2pfs
          this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k1_start, k1s, Ninit, ics, false);
          this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k2_start, k2s, Ninit, ics, false);
          this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k3_start, k3s, Ninit, ics, false);

          // fix initial conditions - imaginary 3pfs
          this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k1_start, k1s, Ninit, ics, true);
          this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k2_start, k2s, Ninit, ics, true);
          this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k3_start, k3s, Ninit, ics, true);

          // fix initial conditions - threepf
          this->populate_threepf_ic(x, $$__MODEL_pool::threepf_start, k1s, k2s, k3s, Ninit, ics);
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


      template <typename number>
      void $$__MODEL_vexcl_twopf_functor<number>::operator()(const twopf_state& __x, twopf_state& __dxdt, double __t)
        {
          const auto __a = exp(__t);

          std::vector<vex::backend::kernel> u2_kernel;
          std::vector<vex::backend::kernel> backg_kernel;
          std::vector<vex::backend::kernel> twopf_kernel;

          // build a kernel to construct the components of u2
          for(unsigned int d = 0; d < this->ctx.size(); d++)
            {
              u2_kernel.emplace_back(this->ctx.queue(d),
                                     $$__IMPORT_KERNEL{vexcl-opencl/u2fused.cl, u2fused, );}
            }

          // Apply the u2 kernel
          for(unsigned int d = 0; d < this->ctx.size(); d++)
            {
              u2_kernel[d].push_arg<cl_ulong>(this->u2_tensor(0).part_size(d));
              u2_kernel[d].push_arg(this->parameters[$$__1]); $$//
              u2_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
              u2_kernel[d].push_arg((this->u2_tensor(this->flatten($$__A,$$__B)))(d)); $$//
              u2_kernel[d].push_arg(this->k_list(d));
              u2_kernel[d].push_arg(__a);
              u2_kernel[d].push_arg(this->M_Planck);

              u2_kernel[d](this->ctx.queue(d));
            }

          // build a kernel to evolve the background
          for(unsigned int d = 0; d < this->ctx.size(); d++)
            {
              backg_kernel.emplace_back(this->ctx.queue(d),
                                        $$__IMPORT_KERNEL{vexcl-opencl/backg.cl, backg, );}
            }

          // apply the background kernel
          for(unsigned int d = 0; d < this->ctx.size(); d++)
            {
              backg_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
              backg_kernel[d].push_arg(this->parameters[$$__1]); $$//
              backg_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
              backg_kernel[d].push_arg((__dxdt(this->flatten($$__A)))(d)); $$//
              backg_kernel[d].push_arg(this->M_Planck);

              backg_kernel[d](this->ctx.queue(d));
            }

          // build a kernel to evolve the twopf
          for(unsigned int d = 0; d < this->ctx.size(); d++)
            {
              twopf_kernel.emplace_back(this->ctx.queue(d),
                                        $$__IMPORT_KERNEL{vexcl-opencl/twopf.cl, twopffused, );}
            }

          // apply the twopf kernel
          for(unsigned int d = 0; d < this->ctx.size(); d++)
            {
              twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
              twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_start + this->flatten($$__A,$$__B)))(d)); $$//
              twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_start + this->flatten($$__A,$$__B)))(d)); $$//
              twopf_kernel[d].push_arg((this->u2_tensor(this->flatten($$__A,$$__B)))(d)); $$//

              twopf_kernel[d](this->ctx.queue(d));
            }
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


      template <typename number>
      void $$__MODEL_vexcl_twopf_observer<number>::operator()(const twopf_state& x, double t)
        {
          // allocate storage for state
          std::vector<number>                hst_background_state($$__MODEL_pool::backg_size);
          std::vector< std::vector<number> > hst_state($$__MODEL_pool::twopf_size);

          // copy device state into local storage, and then push it into the history
          // (** TODO work out how slow this really is)

          // first, background
          for(int i = 0; i < $$__MODEL_pool::backg_size; i++)
            {
              hst_background_state[i] = x(i)[0];  // only need to make a copy for one k-mode; the rest are all the same
            }
          this->background_history.push_back(hst_background_state);

          // then, two pf
          for(int i = 0; i < $$__MODEL_pool::twopf_size; i++)
            {
              // ensure destination is sufficiently large
              hst_state[i].resize(this->k_size);
              vex::copy(x($$__MODEL_pool::backg_size + i), hst_state[i]);
            }
          this->twopf_history.push_back(hst_state);
        }


    // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


    template <typename number>
    void $$__MODEL_vexcl_threepf_functor<number>::operator()(const threepf_state& __x, threepf_state& __dxdt, double __t)
      {
        const auto __a = exp(__t);

        std::vector<vex::backend::kernel> u2_kernel;
        std::vector<vex::backend::kernel> u3_kernel;
        std::vector<vex::backend::kernel> backg_kernel;
        std::vector<vex::backend::kernel> twopf_kernel;
        std::vector<vex::backend::kernel> threepf_kernel;

        // build a kernel to construct the components of u2
        try
          {
            for(unsigned int d = 0; d < this->ctx.size(); d++)
              {
                u2_kernel.emplace_back(this->ctx.queue(d),
                                       $$__IMPORT_KERNEL{vexcl-opencl/u2fused.cl, u2fused, );}
              }
          }
        catch (const cl::Error &err)
          {
            std::cerr << "Setting up u2 kernel: " << err.what() << " " << err.err() << std::endl;
            exit(1);
          }

        // Apply the u2 kernel: we need to build *three* copies, corresponding to the k-choices k1, k2, k3
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            u2_kernel[d].push_arg<cl_ulong>(this->u2_k1_tensor(0).part_size(d));
            u2_kernel[d].push_arg(this->parameters[$$__1]); $$//
            u2_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            u2_kernel[d].push_arg((this->u2_k1_tensor(this->flatten($$__A,$$__B)))(d)); $$//
            u2_kernel[d].push_arg(this->k1_list(d));
            u2_kernel[d].push_arg(__a);
            u2_kernel[d].push_arg(this->M_Planck);

            u2_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            u2_kernel[d].push_arg<cl_ulong>(this->u2_k2_tensor(0).part_size(d));
            u2_kernel[d].push_arg(this->parameters[$$__1]); $$//
            u2_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            u2_kernel[d].push_arg((this->u2_k2_tensor(this->flatten($$__A,$$__B)))(d)); $$//
            u2_kernel[d].push_arg(this->k2_list(d));
            u2_kernel[d].push_arg(__a);
            u2_kernel[d].push_arg(this->M_Planck);

            u2_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            u2_kernel[d].push_arg<cl_ulong>(this->u2_k3_tensor(0).part_size(d));
            u2_kernel[d].push_arg(this->parameters[$$__1]); $$//
            u2_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            u2_kernel[d].push_arg((this->u2_k3_tensor(this->flatten($$__A,$$__B)))(d)); $$//
            u2_kernel[d].push_arg(this->k3_list(d));
            u2_kernel[d].push_arg(__a);
            u2_kernel[d].push_arg(this->M_Planck);

            u2_kernel[d](this->ctx.queue(d));
          }

        // build a kernel to construct the components of u3
        try
          {
            for(unsigned int d = 0; d < this->ctx.size(); d++)
              {
                u3_kernel.emplace_back(this->ctx.queue(d),
                                       $$__IMPORT_KERNEL{vexcl-opencl/u3fused.cl, u3fused, );}
              }
          }
        catch (const cl::Error &err)
          {
            std::cerr << "Setting up u3 kernel: " << err.what() << " " << err.err() << std::endl;
            exit(1);
          }

        // Apply the u3 kernel: we need to build *three* copies, corresponding to the index permutations of k1, k2, k3
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            u3_kernel[d].push_arg<cl_ulong>(this->u3_k1k2k3_tensor(0).part_size(d));
            u3_kernel[d].push_arg(this->parameters[$$__1]); $$//
            u3_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            u3_kernel[d].push_arg((this->u3_k1k2k3_tensor(this->flatten($$__A,$$__B,$$__C)))(d)); $$//
            u3_kernel[d].push_arg(this->k1_list(d));
            u3_kernel[d].push_arg(this->k2_list(d));
            u3_kernel[d].push_arg(this->k3_list(d));
            u3_kernel[d].push_arg(__a);
            u3_kernel[d].push_arg(this->M_Planck);

            u3_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            u3_kernel[d].push_arg<cl_ulong>(this->u3_k2k1k3_tensor(0).part_size(d));
            u3_kernel[d].push_arg(this->parameters[$$__1]); $$//
            u3_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            u3_kernel[d].push_arg((this->u3_k2k1k3_tensor(this->flatten($$__A,$$__B,$$__C)))(d)); $$//
            u3_kernel[d].push_arg(this->k2_list(d));
            u3_kernel[d].push_arg(this->k1_list(d));
            u3_kernel[d].push_arg(this->k3_list(d));
            u3_kernel[d].push_arg(__a);
            u3_kernel[d].push_arg(this->M_Planck);

            u3_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            u3_kernel[d].push_arg<cl_ulong>(this->u3_k3k1k2_tensor(0).part_size(d));
            u3_kernel[d].push_arg(this->parameters[$$__1]); $$//
            u3_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            u3_kernel[d].push_arg((this->u3_k3k1k2_tensor(this->flatten($$__A,$$__B,$$__C)))(d)); $$//
            u3_kernel[d].push_arg(this->k3_list(d));
            u3_kernel[d].push_arg(this->k1_list(d));
            u3_kernel[d].push_arg(this->k2_list(d));
            u3_kernel[d].push_arg(__a);
            u3_kernel[d].push_arg(this->M_Planck);

            u3_kernel[d](this->ctx.queue(d));
          }

        // build a kernel to evolve the background
        try
          {
            for(unsigned int d = 0; d < this->ctx.size(); d++)
              {
                backg_kernel.emplace_back(this->ctx.queue(d),
                                          $$__IMPORT_KERNEL{vexcl-opencl/backg.cl, backg, );}
              }
          }
        catch (const cl::Error &err)
          {
            std::cerr << "Setting up backg kernel: " << err.what() << " " << err.err() << std::endl;
            exit(1);
          }

        // apply the background kernel
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            backg_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            backg_kernel[d].push_arg(this->parameters[$$__1]); $$//
            backg_kernel[d].push_arg((__x(this->flatten($$__A)))(d)); $$//
            backg_kernel[d].push_arg((__dxdt(this->flatten($$__A)))(d)); $$//
            backg_kernel[d].push_arg(this->M_Planck);

            backg_kernel[d](this->ctx.queue(d));
          }

        // build a kernel to evolve the twopf
        try
          {
            for(unsigned int d = 0; d < this->ctx.size(); d++)
              {
                twopf_kernel.emplace_back(this->ctx.queue(d),
                                          $$__IMPORT_KERNEL{vexcl-opencl/twopf.cl, twopffused, );}
              }
          }
        catch (const cl::Error &err)
          {
            std::cerr << "Setting up twopf kernel: " << err.what() << " " << err.err() << std::endl;
            exit(1);
          }

        // apply the twopf kernel to evolve to real and imaginary components of each twopf
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_re_k1_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_re_k1_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((this->u2_k1_tensor(this->flatten($$__A,$$__B)))(d)); $$//

            twopf_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_im_k1_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_im_k1_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((this->u2_k1_tensor(this->flatten($$__A,$$__B)))(d)); $$//

            twopf_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_re_k2_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_re_k2_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((this->u2_k2_tensor(this->flatten($$__A,$$__B)))(d)); $$//

            twopf_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_im_k2_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_im_k2_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((this->u2_k2_tensor(this->flatten($$__A,$$__B)))(d)); $$//

            twopf_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_re_k3_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_re_k3_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((this->u2_k3_tensor(this->flatten($$__A,$$__B)))(d)); $$//

            twopf_kernel[d](this->ctx.queue(d));
          }
        for(unsigned int d = 0; d < this->ctx.size(); d++)
          {
            twopf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
            twopf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_im_k3_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((__dxdt($$__MODEL_pool::twopf_im_k3_start + this->flatten($$__A,$$__B)))(d)); $$//
            twopf_kernel[d].push_arg((this->u2_k3_tensor(this->flatten($$__A,$$__B)))(d)); $$//

            twopf_kernel[d](this->ctx.queue(d));
          }

        // build a kernel to evolve the threepf
        try
          {
            for(unsigned int d = 0; d < this->ctx.size(); d++)
              {
                threepf_kernel.emplace_back(this->ctx.queue(d),
                                            $$__IMPORT_KERNEL{vexcl-opencl/threepf.cl, threepffused, );}
              }
          }
        catch (const cl::Error &err)
          {
            std::cerr << "Setting up threepf kernel: " << err.what() << " " << err.err() << std::endl;
            exit(1);
          }

        // apply the threepf kernel
        try
          {
            for(unsigned int d = 0; d < this->ctx.size(); d++)
              {
                threepf_kernel[d].push_arg<cl_ulong>(__x(0).part_size(d));
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_re_k1_start + this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_im_k1_start + this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_re_k2_start + this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_im_k2_start + this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_re_k3_start + this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::twopf_im_k3_start + this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((__x($$__MODEL_pool::threepf_start     + this->flatten($$__A,$$__B,$$__C)))(d)); $$//
                threepf_kernel[d].push_arg((__dxdt($$__MODEL_pool::threepf_start  + this->flatten($$__A,$$__B,$$__C)))(d)); $$//
                threepf_kernel[d].push_arg((this->u2_k1_tensor(this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((this->u2_k2_tensor(this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((this->u2_k3_tensor(this->flatten($$__A,$$__B)))(d)); $$//
                threepf_kernel[d].push_arg((this->u3_k1k2k3_tensor(this->flatten($$__A,$$__B,$$__C)))(d)); $$//
                threepf_kernel[d].push_arg((this->u3_k2k1k3_tensor(this->flatten($$__A,$$__B,$$__C)))(d)); $$//
                threepf_kernel[d].push_arg((this->u3_k3k1k2_tensor(this->flatten($$__A,$$__B,$$__C)))(d)); $$//

              threepf_kernel[d](this->ctx.queue(d));
              }
          }
        catch (const cl::Error &err)
          {
            std::cerr << "Calling the threepf kernel: " << err.what() << " " << err.err() << std::endl;
            exit(1);
          }

      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename number>
    void $$__MODEL_vexcl_threepf_observer<number>::operator()(const threepf_state& x, double t)
      {
        // allocate storage for state
        std::vector<number>                hst_background_state($$__MODEL_pool::backg_size);
        std::vector< std::vector<number> > hst_twopf_re_state  ($$__MODEL_pool::twopf_size);
        std::vector< std::vector<number> > hst_twopf_im_state  ($$__MODEL_pool::twopf_size);
        std::vector< std::vector<number> > hst_threepf_state   ($$__MODEL_pool::threepf_size);

        std::cout << __CPP_TRANSPORT_OBSERVER_TIME << " = " << t << std::endl;

        // first, background
        for(int i = 0; i < $$__MODEL_pool::backg_size; i++)
          {
            std::vector<number> hst_bg(this->kconfig_list.size());

            vex::copy(x($$__MODEL_pool::backg_start + i), hst_bg);
            for(int j = 0; j < this->kconfig_list.size(); j++)
              {
                if(this->kconfig_list[j].store_background)
                  {
                    hst_background_state[i] = hst_bg[j];
                  }
              }
          }
        this->background_history.push_back(hst_background_state);

        // then, twopf
        for(int i = 0; i < $$__MODEL_pool::twopf_size; i++)
          {
            std::vector<number> hst_re(this->kconfig_list.size());
            std::vector<number> hst_im(this->kconfig_list.size());

            vex::copy(x($$__MODEL_pool::twopf_re_k1_start + i), hst_re);
            vex::copy(x($$__MODEL_pool::twopf_im_k1_start + i), hst_im);
            for(int j = 0; j < this->kconfig_list.size(); j++)
              {
                if(this->kconfig_list[j].store_twopf)
                  {
                    hst_twopf_re_state[i].push_back(hst_re[j]);
                    hst_twopf_im_state[i].push_back(hst_im[j]);
                  }
              }
          }
        this->twopf_re_history.push_back(hst_twopf_re_state);
        this->twopf_im_history.push_back(hst_twopf_im_state);

        // finally, threepf
        for(int i = 0; i < $$__MODEL_pool::threepf_size; i++)
          {
            // ensure destination is sufficiently large
            hst_threepf_state[i].resize(this->kconfig_list.size());
            vex::copy(x($$__MODEL_pool::threepf_start + i), hst_threepf_state[i]);
          }
        this->threepf_history.push_back(hst_threepf_state);
      }


  }   // namespace transport


#endif  // $$__GUARD