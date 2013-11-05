//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// OpenCL implementation using the VexCL library

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
      #define BACKG_SIZE   (2*$$__NUMBER_FIELDS)
      #define TWOPF_SIZE   ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS))
      #define THREEPF_SIZE ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS))

      #define TWOPF_STATE_SIZE   (BACKG_SIZE + TWOPF_SIZE)
      #define THREEPF_STATE_SIZE (BACKG_SIZE + 3*TWOPF_SIZE + THREEPF_SIZE)

      #define U2_SIZE            ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS))
      #define U3_SIZE            ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS))

      // set up a state type for 2pf integration on an OpenCL device
      typedef vex::multivector<double, TWOPF_STATE_SIZE> twopf_state;

      // set up a state type for 3pf integration on an OpenCL device
      typedef vex::multivector<double, THREEPF_STATE_SIZE> threepf_state;


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
            void populate_twopf_ic(twopf_state& x, unsigned int start, std::vector<double>& kmodes, double Ninit,
                                   const std::vector<number>& ic, bool imaginary = false);

            void resize_twopf_history(std::vector <std::vector< std::vector<number> >>& twopf_history,
                                      const std::vector<double>& times, const std::vector<double>& ks);

        };


      // integration - 2pf functor
      template <typename number>
      class $$__MODEL_vexcl_twopf_functor
        {
          public:
            $$__MODEL_vexcl_twopf_functor(const std::vector<number>& p, const number Mp,
                                          const vex::vector<double>& ks,
                                          vex::multivector<double, U2_SIZE>& u2)
              : parameters(p), M_Planck(Mp), k_list(ks), u2_tensor(u2)
              {
              }

            void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
            const number						           M_Planck;
            const std::vector<number>&         parameters;
            const vex::vector<double>&         k_list;
            vex::multivector<double, U2_SIZE>& u2_tensor;
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
          vex::multivector<double, U2_SIZE> u2_tensor(ctx.queue(), com_ks.size());

          // set up state vector, and populate it with initial conditions for the background and twopf
          twopf_state dev_x(ctx.queue(), com_ks.size());

          const auto background_start = 0;
          const auto twopf_start      = background_start + BACKG_SIZE;
        
          // 1 - background
          dev_x(background_start + $$__A) = $$// hst_bg[$$__A];
        
          // 2 - twopf
          this->populate_twopf_ic(dev_x, twopf_start, com_ks, *times.begin(), hst_bg);

          // set up a functor to evolve this system
          $$__MODEL_vexcl_twopf_functor<number> rhs(this->parameters, this->M_Planck, dev_ks, u2_tensor);
        
          // set up a functor to observe the integration
          $$__MODEL_vexcl_twopf_observer<number> obs(background_history, twopf_history, ks.size());

          using namespace boost::numeric::odeint;
          typedef runge_kutta_dopri5<twopf_state, double, twopf_state, double, vector_space_algebra, default_operations> stepper;

          integrate_times( make_controlled<stepper>($$__PERT_ABS_ERR, $$__PERT_REL_ERR), rhs, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

          transport::$$__MODEL_gauge_xfm_gadget<number>* gauge_xfm = new $$__MODEL_gauge_xfm_gadget<number>(this->M_Planck, this->parameters);
          transport::$$__MODEL_tensor_gadget<number>*    tensor    = new $$__MODEL_tensor_gadget<number>(this->M_Planck, this->parameters);

          transport::twopf<number> tpf($$__NUMBER_FIELDS, $$__MODEL_state_names, $$__MODEL_latex_names, ks, com_ks, Nstar,
                                       times, background_history, twopf_history, gauge_xfm, tensor);

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
    void $$__MODEL_vexcl<number>::populate_twopf_ic(twopf_state& x, unsigned int start, std::vector<double>& kmodes, double Ninit, const std::vector<number>& ics, bool imaginary)
      {
      // TODO - need some form of introspection to determine size of state vector
      // but here assume it is ok
//        assert(x.size() >= start);
//        assert(x.size() >= start + TWOPF_SIZE);

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

                vex::copy(hst_tp_ic, x(start + (2*$$__NUMBER_FIELDS)*i + j));
              }
          }
      }


      // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


      template <typename number>
      void $$__MODEL_vexcl_twopf_functor<number>::operator()(const twopf_state& __x, twopf_state& __dxdt, double __t)
        {
          const unsigned int __start_background = 0;
          const unsigned int __start_twopf      = 2*$$__NUMBER_FIELDS;

          #undef $$__PARAMETER[1]
          #undef $$__COORDINATE[A]
          #undef __Mp
          #undef __k

          #define $$__PARAMETER[1]  (vex::tag<$$__UNIQUE>(this->parameters[$$__1]))
          #define $$__COORDINATE[A] (vex::tag<$$__UNIQUE>(__x($$__A)))
          #define __Mp              (vex::tag<$$__UNIQUE>(this->M_Planck))
          #define __k               (vex::tag<$$__UNIQUE>(this->k_list))

          const auto __a               = vex::make_temp<$$__UNIQUE>(exp(__t));
          const auto __Hsq             = vex::make_temp<$$__UNIQUE>($$__HUBBLE_SQ);
          const auto __eps             = vex::make_temp<$$__UNIQUE>($$__EPSILON);

          #undef __tpf_$$__A_$$__B $$//
          #undef __u2_$$__A_$$__B  $$//
          #undef __u2

          #define __tpf_$$__A_$$__B $$// (vex::tag<$$__UNIQUE>(__x(__start_twopf + (2*$$__NUMBER_FIELDS*$$__A)+$$__B)))

          #define __u2_$$__A_$$__B  $$// (vex::tag<$$__UNIQUE>(this->u2_tensor((2*$$__NUMBER_FIELDS*$$__A)+$$__B)))

          #define __u2(a,b)         $$// this->u2_tensor((2*$$__NUMBER_FIELDS*a)+b)

          #undef __background
          #undef __dtwopf
          #define __background(a) __dxdt(__start_background + a)
          #define __dtwopf(a,b)   __dxdt(__start_twopf      + (2*$$__NUMBER_FIELDS*a) + b)

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
          std::vector<number>                hst_background_state(BACKG_SIZE);
          std::vector< std::vector<number> > hst_state(TWOPF_SIZE);

          // copy device state into local storage, and then push it into the history
          // (** TODO work out how slow this really is)

          // first, background
          for(int i = 0; i < BACKG_SIZE; i++)
            {
              hst_background_state[i] = x(i)[0];  // only need to make a copy for one k-mode; the rest are all the same
            }
          this->background_history.push_back(hst_background_state);

          // then, two pf
          for(int i = 0; i < TWOPF_SIZE; i++)
            {
              // ensure destination is sufficiently large
              hst_state[i].resize(this->k_size);
              vex::copy(x(BACKG_SIZE + i), hst_state[i]);
            }

          this->twopf_history.push_back(hst_state);
        }


  }   // namespace transport


#endif  // $$__GUARD
