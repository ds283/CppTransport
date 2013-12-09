// backend=vexcl minver=0.06
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// OpenCL/CUDA implementation using the VexCL library

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
      // set up a state type for 2pf integration on an OpenCL/CUDA device
      typedef vex::multivector<double, $$__MODEL_pool::twopf_state_size> twopf_state;

      // set up a state type for 3pf integration on an OpenCL/CUDA device
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

            // Integrate background and 2-point function on an OpenCL/CUDA device
            transport::twopf<number>
              twopf(vex::Context& ctx, const std::vector<double>& ks, double Nstar,
                    const std::vector<number>& ics, const std::vector<double>& times,
                    bool silent=false);

            // Integrate background, 2-point function and 3-point function on an OpenCL/CUDA device
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
            $$__MODEL_vexcl_twopf_functor(const std::vector<number>& p, const number Mp,
                                          const vex::vector<double>& ks,
                                          vex::multivector<double, $$__MODEL_pool::u2_size>& u2)
              : parameters(p), M_Planck(Mp), k_list(ks), u2_tensor(u2)
              {
              }

            void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
            // constexpr version for rapid evaluation during integration
            constexpr unsigned int flatten(unsigned int a)                                 { return(a); }
            constexpr unsigned int flatten(unsigned int a, unsigned int b)                 { return(2*$$__NUMBER_FIELDS*a + b); }
            constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); }

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
        $$__MODEL_vexcl_threepf_functor(const std::vector<number>& p, const number Mp,
          const vex::vector<double>& k1s, const vex::vector<double>& k2s, const vex::vector<double>& k3s,
          vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k1, vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k2, vex::multivector<double, $$__MODEL_pool::u2_size>& u2_k3,
          vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k1k2k3, vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k2k1k3, vex::multivector<double, $$__MODEL_pool::u3_size>& u3_k3k1k2)
        : parameters(p), M_Planck(Mp), k1_list(k1s), k2_list(k2s), k3_list(k3s),
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
          dev_x($$__MODEL_pool::backg_start + this->flatten($$__A)) = $$// hst_bg[this->flatten($$__A)];
        
          // 2 - twopf
          this->populate_twopf_ic(dev_x, $$__MODEL_pool::twopf_start, com_ks, *times.begin(), hst_bg);

          // set up a functor to evolve this system
          $$__MODEL_vexcl_twopf_functor<number> rhs(this->parameters, this->M_Planck, dev_ks, u2_tensor);
        
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
          using namespace boost::numeric::odeint;

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
              hst_k1s[i] = com_ks[kconfig_list[i].indices[0]];
              hst_k2s[i] = com_ks[kconfig_list[i].indices[1]];
              hst_k3s[i] = com_ks[kconfig_list[i].indices[2]];
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
          this->populate_threepf_state_ic(dev_x, kconfig_list, hst_k1s, hst_k2s, hst_k3s, *(times.begin()), hst_bg);

          // set up a functor to evolve this system
          $$__MODEL_vexcl_threepf_functor<number> rhs(this->parameters, this->M_Planck, dev_k1s, dev_k2s, dev_k3s,
                                                      u2_k1_tensor, u2_k2_tensor, u2_k3_tensor,
                                                      u3_k1k2k3_tensor, u3_k2k1k3_tensor, u3_k3k1k2_tensor);

          // set up a functor to observe the integration
          $$__MODEL_vexcl_threepf_observer<number> obs(background_history, twopf_re_history, twopf_im_history, threepf_history, kconfig_list);

          using namespace boost::numeric::odeint;
          integrate_times($$__MAKE_PERT_STEPPER{twopf_state}, rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

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
          #undef $$__PARAMETER[1]
          #undef $$__COORDINATE[A]
          #undef __Mp
          #undef __k

          #define $$__PARAMETER[1]  (vex::tag<$$__UNIQUE>(this->parameters[$$__1]))
          #define $$__COORDINATE[A] (vex::tag<$$__UNIQUE>(__x(this->flatten($$__A))))
          #define __Mp              (vex::tag<$$__UNIQUE>(this->M_Planck))
          #define __k               (vex::tag<$$__UNIQUE>(this->k_list))

          const auto __a   = vex::make_temp<$$__UNIQUE>(exp(__t));
          const auto __Hsq = vex::make_temp<$$__UNIQUE>($$__HUBBLE_SQ);
          const auto __eps = vex::make_temp<$$__UNIQUE>($$__EPSILON);

          #undef __tpf_$$__A_$$__B $$//
          #undef __u2_$$__A_$$__B  $$//
          #undef __u2

          #define __tpf_$$__A_$$__B $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_start + this->flatten($$__A,$$__B))))

          #define __u2_$$__A_$$__B  $$// (vex::tag<$$__UNIQUE>(this->u2_tensor(this->flatten($$__A,$$__B))))

          #define __u2(a,b)         $$// this->u2_tensor(this->flatten(a,b))

          #undef __background
          #undef __dtwopf
          #define __background(a) __dxdt($$__MODEL_pool::backg_start + this->flatten(a))
          #define __dtwopf(a,b)   __dxdt($$__MODEL_pool::twopf_start + this->flatten(a,b))

          $$__TEMP_POOL{"const auto $1 = vex::make_temp<$$__UNIQUE>($2);"}

          // evolve the background
          __background($$__A)    = $$// $$__U1_PREDEF[A]{__Hsq, __eps};

          // set up a k-dependent u2 tensor
          __u2($$__A,$$__B)      = $$// $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};

          // evolve the 2pf
          // here, we are dealing only with the real part - which is symmetric
          // so the index placement is not important
          __dtwopf($$__A,$$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2} * __tpf_$$__C_$$__B;
          __dtwopf($$__A,$$__B) += 0 $$// + $$__U2_NAME[BC]{__u2} * __tpf_$$__A_$$__C;

          #undef $$__PARAMETER[1]
          #undef $$__COORDINATE[A]
          #undef __Mp
          #undef __k

          #undef __tpf_$$__A_$$__B $$//
          #undef __u2_$$__A_$$__B  $$//
          #undef __u2
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
        #undef $$__PARAMETER[1]
        #undef $$__COORDINATE[A]
        #undef __Mp
        #undef __k1
        #undef __k2
        #undef __k3

        #define $$__PARAMETER[1]  (vex::tag<$$__UNIQUE>(this->parameters[$$__1]))
        #define $$__COORDINATE[A] (vex::tag<$$__UNIQUE>(__x(this->flatten($$__A))))
        #define __Mp              (vex::tag<$$__UNIQUE>(this->M_Planck))
        #define __k1              (vex::tag<$$__UNIQUE>(this->k1_list))
        #define __k2              (vex::tag<$$__UNIQUE>(this->k2_list))
        #define __k3              (vex::tag<$$__UNIQUE>(this->k3_list))

        const auto __a               = vex::make_temp<$$__UNIQUE>(exp(__t));
        const auto __Hsq             = vex::make_temp<$$__UNIQUE>($$__HUBBLE_SQ);
        const auto __eps             = vex::make_temp<$$__UNIQUE>($$__EPSILON);

        #undef __twopf_re_k1_$$__A_$$__B      $$//
        #undef __twopf_im_k1_$$__A_$$__B      $$//
        #undef __twopf_re_k2_$$__A_$$__B      $$//
        #undef __twopf_im_k2_$$__A_$$__A      $$//
        #undef __twopf_re_k3_$$__A_$$__A      $$//
        #undef __twopf_im_k3_$$__A_$$__A      $$//
        #undef __threepf_$$__A_$$__B_$$__C    $$//

        #undef __u2_k1_$$__A_$$__B            $$//
        #undef __u2_k2_$$__A_$$__B            $$//
        #undef __u2_k3_$$__A_$$__B            $$//
        #undef __u2_k1
        #undef __u2_k2
        #undef __u2_k3

        #undef __u3_k1k2k3_$$__A_$$__B_$$__C  $$//
        #undef __u3_k2k1k3_$$__A_$$__B_$$__C  $$//
        #undef __u3_k3k1k2_$$__A_$$__B_$$__C  $$//
        #undef __u3_k1k2k3
        #undef __u3_k2k1k3
        #undef __u3_k3k1k2

        #define __twopf_re_k1_$$__A_$$__B     $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_re_k1_start + this->flatten($$__A,$$__B))))
        #define __twopf_im_k1_$$__A_$$__B     $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_im_k1_start + this->flatten($$__A,$$__B))))
        #define __twopf_re_k2_$$__A_$$__B     $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_re_k2_start + this->flatten($$__A,$$__B))))
        #define __twopf_im_k2_$$__A_$$__B     $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_im_k2_start + this->flatten($$__A,$$__B))))
        #define __twopf_re_k3_$$__A_$$__B     $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_re_k3_start + this->flatten($$__A,$$__B))))
        #define __twopf_im_k3_$$__A_$$__B     $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::twopf_im_k3_start + this->flatten($$__A,$$__B))))
        #define __threepf_$$__A_$$__B_$$__C   $$// (vex::tag<$$__UNIQUE>(__x($$__MODEL_pool::threepf_start     + this->flatten($$__A,$$__B,$$__C))))

        #define __u2_k1_$$__A_$$__B           $$// (vex::tag<$$__UNIQUE>(this->u2_k1_tensor(this->flatten($$__A,$$__B))))
        #define __u2_k2_$$__A_$$__B           $$// (vex::tag<$$__UNIQUE>(this->u2_k2_tensor(this->flatten($$__A,$$__B))))
        #define __u2_k3_$$__A_$$__B           $$// (vex::tag<$$__UNIQUE>(this->u2_k3_tensor(this->flatten($$__A,$$__B))))

        #define __u3_k1k2k3_$$__A_$$__B_$$__C $$// (vex::tag<$$__UNIQUE>(this->flatten($$__A,$$__B,$$__C)))
        #define __u3_k2k1k3_$$__A_$$__B_$$__C $$// (vex::tag<$$__UNIQUE>(this->flatten($$__A,$$__B,$$__C)))
        #define __u3_k3k1k2_$$__A_$$__B_$$__C $$// (vex::tag<$$__UNIQUE>(this->flatten($$__A,$$__B,$$__C)))

        #define __u2_k1(a,b)                       this->u2_k1_tensor(this->flatten(a,b))
        #define __u2_k2(a,b)                       this->u2_k2_tensor(this->flatten(a,b))
        #define __u2_k3(a,b)                       this->u2_k3_tensor(this->flatten(a,b))
        #define __u3_k1k2k3(a,b,c)                 this->u3_k1k2k3_tensor(this->flatten(a,b,c))
        #define __u3_k2k1k3(a,b,c)                 this->u3_k2k1k3_tensor(this->flatten(a,b,c))
        #define __u3_k3k1k2(a,b,c)                 this->u3_k3k1k2_tensor(this->flatten(a,b,c))

        #undef __background
        #undef __dtwopf_re_k1
        #undef __dtwopf_im_k1
        #undef __dtwopf_re_k2
        #undef __dtwopf_im_k2
        #undef __dtwopf_re_k3
        #undef __dtwopf_im_k3
        #undef __dthreepf
        #define __background(a)     __dxdt($$__MODEL_pool::backg_start       + this->flatten(a))
        #define __dtwopf_re_k1(a,b) __dxdt($$__MODEL_pool::twopf_re_k1_start + this->flatten(a,b))
        #define __dtwopf_im_k1(a,b) __dxdt($$__MODEL_pool::twopf_im_k1_start + this->flatten(a,b))
        #define __dtwopf_re_k2(a,b) __dxdt($$__MODEL_pool::twopf_re_k2_start + this->flatten(a,b))
        #define __dtwopf_im_k2(a,b) __dxdt($$__MODEL_pool::twopf_im_k2_start + this->flatten(a,b))
        #define __dtwopf_re_k3(a,b) __dxdt($$__MODEL_pool::twopf_re_k3_start + this->flatten(a,b))
        #define __dtwopf_im_k3(a,b) __dxdt($$__MODEL_pool::twopf_im_k3_start + this->flatten(a,b))
        #define __dthreepf(a,b,c)   __dxdt($$__MODEL_pool::threepf_start     + this->flatten(a,b,c))

        $$__TEMP_POOL{"const auto $1 = vex::make_temp<$$__UNIQUE>($2);"}

        // evolve the background
        __background($$__A)            = $$// $$__U1_PREDEF[A]{__Hsq, __eps};

        // set up k-dependent u2 tensors
        __u2_k1($$__A,$$__B)           = $$// $$__U2_PREDEF[AB]{__k1, __a, __Hsq, __eps};
        __u2_k2($$__A,$$__B)           = $$// $$__U2_PREDEF[AB]{__k2, __a, __Hsq, __eps};
        __u2_k3($$__A,$$__B)           = $$// $$__U2_PREDEF[AB]{__k3, __a, __Hsq, __eps};

        // set up k-dependent u3 tensors
        __u3_k1k2k3($$__A,$$__B,$$__C) = $$// $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};
        __u3_k2k1k3($$__A,$$__B,$$__C) = $$// $$__U3_PREDEF[ABC]{__k2, __k1, __k3, __a, __Hsq, __eps};
        __u3_k3k1k2($$__A,$$__B,$$__C) = $$// $$__U3_PREDEF[ABC]{__k3, __k1, __k2, __a, __Hsq, __eps};

        // evolve the real and imaginary components of the 2pf
        __dtwopf_re_k1($$__A, $$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2_k1}*__twopf_re_k1_$$__C_$$__B;
        __dtwopf_re_k1($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k1}*__twopf_re_k1_$$__A_$$__C;

        __dtwopf_im_k1($$__A, $$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2_k1}*__twopf_im_k1_$$__C_$$__B;
        __dtwopf_im_k1($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k1}*__twopf_im_k1_$$__A_$$__C;

        __dtwopf_re_k2($$__A, $$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2_k2}*__twopf_re_k2_$$__C_$$__B;
        __dtwopf_re_k2($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k2}*__twopf_re_k2_$$__A_$$__C;

        __dtwopf_im_k2($$__A, $$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2_k2}*__twopf_im_k2_$$__C_$$__B;
        __dtwopf_im_k2($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k2}*__twopf_im_k2_$$__A_$$__C;

        __dtwopf_re_k3($$__A, $$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2_k3}*__twopf_re_k3_$$__C_$$__B;
        __dtwopf_re_k3($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k3}*__twopf_re_k3_$$__A_$$__C;

        __dtwopf_im_k3($$__A, $$__B)  = 0 $$// + $$__U2_NAME[AC]{__u2_k3}*__twopf_im_k3_$$__C_$$__B;
        __dtwopf_im_k3($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k3}*__twopf_im_k3_$$__A_$$__C;

        // evolve the components of the 3pf

        __dthreepf($$__A, $$__B, $$__C)  = 0 $$// + $$__U2_NAME[AM]{__u2_k1}*__threepf_$$__M_$$__B_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[AMN]{__u3_k1k2k3}*__twopf_re_k2_$$__M_$$__B*__twopf_re_k3_$$__N_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[AMN]{__u3_k1k2k3}*__twopf_im_k2_$$__M_$$__B*__twopf_im_k3_$$__N_$$__C;

        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U2_NAME[BM]{__u2_k2}*__threepf_$$__A_$$__M_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[BMN]{__u3_k2k1k3}*__twopf_re_k1_$$__A_$$__M*__twopf_re_k3_$$__N_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[BMN]{__u3_k2k1k3}*__twopf_im_k1_$$__A_$$__M*__twopf_im_k3_$$__N_$$__C;

        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U2_NAME[CM]{__u2_k3}*__threepf_$$__A_$$__B_$$__M;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[CMN]{__u3_k3k1k2}*__twopf_re_k1_$$__A_$$__M*__twopf_re_k2_$$__B_$$__N;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[CMN]{__u3_k3k1k2}*__twopf_im_k1_$$__A_$$__M*__twopf_im_k2_$$__B_$$__N;

        #undef $$__PARAMETER[1]
        #undef $$__COORDINATE[A]
        #undef __Mp
        #undef __k1
        #undef __k2
        #undef __k3

        #undef __twopf_re_k1_$$__A_$$__B      $$//
        #undef __twopf_im_k1_$$__A_$$__B      $$//
        #undef __twopf_re_k2_$$__A_$$__B      $$//
        #undef __twopf_im_k2_$$__A_$$__A      $$//
        #undef __twopf_re_k3_$$__A_$$__A      $$//
        #undef __twopf_im_k3_$$__A_$$__A      $$//
        #undef __threepf_$$__A_$$__B_$$__C    $$//

        #undef __u2_k1_$$__A_$$__B            $$//
        #undef __u2_k2_$$__A_$$__B            $$//
        #undef __u2_k3_$$__A_$$__B            $$//
        #undef __u2_k1
        #undef __u2_k2
        #undef __u2_k3

        #undef __u3_k1k2k3_$$__A_$$__B_$$__C  $$//
        #undef __u3_k2k1k3_$$__A_$$__B_$$__C  $$//
        #undef __u3_k3k1k2_$$__A_$$__B_$$__C  $$//
        #undef __u3_k1k2k3
        #undef __u3_k2k1k3
        #undef __u3_k3k1k2

        #undef __background
        #undef __dtwopf_re_k1
        #undef __dtwopf_im_k1
        #undef __dtwopf_re_k2
        #undef __dtwopf_im_k2
        #undef __dtwopf_re_k3
        #undef __dtwopf_im_k3
        #undef __dthreepf
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename number>
    void $$__MODEL_vexcl_threepf_observer<number>::operator()(const threepf_state& x, double t)
      {
        // allocate storage for state
        std::vector<number>                hst_background_state($$__MODEL_pool::backg_size);
        std::vector< std::vector<number> > hst_twopf_re_state($$__MODEL_pool::twopf_size);
        std::vector< std::vector<number> > hst_twopf_im_state($$__MODEL_pool::twopf_size);
        std::vector< std::vector<number> > hst_threepf_state($$__MODEL_pool::threepf_size);

        // first, background
        for(int i = 0; i < $$__MODEL_pool::backg_size; i++)
          {
            hst_background_state[i] = x($$__MODEL_pool::backg_start + i)[0];  // only need to make a copy for one k-mode; the rest are all the same
          }
        this->background_history.push_back(hst_background_state);

        // then, two pf
        for(int i = 0; i < $$__MODEL_pool::twopf_size; i++)
          {
            std::vector<number> hst_re(this->kconfig_list.size());
            std::vector<number> hst_im(this->kconfig_list.size());

            vex::copy(x($$__MODEL_pool::twopf_re_k1_start + i), hst_re);
            vex::copy(x($$__MODEL_pool::twopf_im_k1_start + i), hst_im);
            for(int j = 0; j < this->kconfig_list.size(); j++)
              {
                if(this->kconfig_list[j].store_background)
                  {
                    hst_twopf_re_state[i].push_back(hst_re[j]);
                    hst_twopf_im_state[i].push_back(hst_im[j]);
                  }
              }
          }
        this->twopf_re_history.push_back(hst_twopf_re_state);
        this->twopf_im_history.push_back(hst_twopf_im_state);

        // finally, three pf
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
