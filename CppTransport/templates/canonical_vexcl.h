//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include <math.h>

#include "vexcl/vexcl.hpp"

#include "boost/numeric/odeint.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl_resize.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl_norm_inf.hpp"

#include "transport/transport.h"

namespace transport
  {
      static std::vector<std::string> $$__MODEL_field_names = $$__FIELD_NAME_LIST;
      static std::vector<std::string> $$__MODEL_latex_names = $$__LATEX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_param_names = $$__PARAM_NAME_LIST;
      static std::vector<std::string> $$__MODEL_platx_names = $$__PLATX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_state_names = $$__STATE_NAME_LIST;

      // set up a state type for GPU integration
      typedef vex::multivector<double, 2*$$__NUMBER_FIELDS + (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)> twopf_state;


      // *********************************************************************************************


      // gauge transformation gadget
      template <typename number>
      class $$__MODEL_gauge_xfm_gadget : public gauge_xfm_gadget<number>
        {
      public:
          $$__MODEL_gauge_xfm_gadget(number Mp, const std::vector<number>& ps) : gauge_xfm_gadget<number>(Mp, ps) {}

          void compute_gauge_xfm_1(const std::vector<number>& __state,
            std::vector<number>& __dN);
          void compute_gauge_xfm_2(const std::vector<number>& __state,
            std::vector< std::vector<number> >& __ddN);
          void compute_gauge_xfm_3(const std::vector<number>& __state,
            std::vector< std::vector< std::vector<number> > >& __dddN);
        };


      // *********************************************************************************************


      template <typename number>
      class $$__MODEL : public canonical_model<number>
        {
          public:
            $$__MODEL(number Mp, const std::vector<number>& ps);
            ~$$__MODEL();

            // Functions inherited from canonical_model
            number
              V(std::vector<number> fields);

            // Integration of the model
            // ========================

            // Integrate the background on the CPU
            transport::background<number>
              background(const std::vector<number>& ics, const std::vector<double>& times);

            // Integrate background and 2-point function on the GPU
            transport::twopf<number>
              twopf(vex::Context& ctx, const std::vector<double>& ks, double Nstar,
                const std::vector<number>& ics, const std::vector<double>& times);

            // Calculation of gauge-transformation coefficients (to zeta)
            // ==========================================================

            void compute_gauge_xfm_1(const std::vector<number>& __state, std::vector<number>& __dN);
            void compute_gauge_xfm_2(const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN);
            void compute_gauge_xfm_3(const std::vector<number>& __state, std::vector< std::vector< std::vector<number> > >& __dddN);

          protected:
            void
              fix_initial_conditions(const std::vector<number>& __ics, std::vector<number>& __rics);
            void
              write_initial_conditions(const std::vector<number>& rics, std::ostream& stream,
              double abs_err, double rel_err, double step_size, std::string stepper_name);
            void
              make_tpf_ic(unsigned int i, unsigned int j, const std::vector<double>& __ks, double __Nstar,
                const std::vector<number>& __fields, std::vector<double>& __tpf);
            void
              rescale_ks(const std::vector<double>& __ks, std::vector<double>& __com_ks,
                double __Nstar, const std::vector<number>& __fields);

            $$__MODEL_gauge_xfm_gadget<number> gauge_xfm;
        };


      // integration - background functor
      template <typename number>
      class $$__MODEL_background_functor
        {
          public:
            $$__MODEL_background_functor(const std::vector<number>& p, number Mp) : parameters(p), M_Planck(Mp) {}
            void operator()(const std::vector<number>& __x, std::vector<number>& __dxdt, double __t);

          private:
            const number              M_Planck;
            const std::vector<number> parameters;
        };


      // integration - observer object for background only
      template <typename number>
      class $$__MODEL_background_observer
        {
          public:
            $$__MODEL_background_observer(std::vector<double>& s, std::vector< std::vector<number> >& h) : slices(s), history(h) {}
            void operator()(const std::vector<number>& x, double t);

          private:
            std::vector<double>&                slices;
            std::vector< std::vector<number> >& history;
        };


      // integration - 2pf functor
      template <typename number>
      class $$__MODEL_twopf_functor
        {
          public:
            $$__MODEL_twopf_functor(const std::vector<number>& p, const number Mp,
              const vex::vector<double>& ks,
              vex::multivector<double, (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)>& u2)
              : parameters(p), M_Planck(Mp), k_list(ks), u2_tensor(u2) {}
            void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
            const number						                                               M_Planck;
            const std::vector<number>&                                             parameters;
            const vex::vector<double>&                                             k_list;
            vex::multivector<double, (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)>& u2_tensor;
        };


      // integration - observer object for 2pf
      template <typename number>
      class $$__MODEL_twopf_observer
        {
          public:
            $$__MODEL_twopf_observer(std::vector<double>& s,
              std::vector< std::vector<number> >& bh,
              std::vector< std::vector< std::vector<number> > >& tpfh, unsigned int ks)
              : slices(s), background_history(bh), twopf_history(tpfh), k_size(ks) {}
            void operator()(const twopf_state& x, double t);

          private:
            std::vector<double>&                               slices;
            std::vector< std::vector<number> >&                background_history;
            std::vector< std::vector< std::vector<number> > >& twopf_history;

            const unsigned int                                 k_size;    // number of k-modes we are integrating
        };


      // IMPLEMENTATION -- CLASS $$__MODEL
      // ==============


      template <typename number>
      $$__MODEL<number>::$$__MODEL(number Mp, const std::vector<number>& ps)
        : canonical_model<number>("$$__NAME", "$$__AUTHOR", "$$__TAG", Mp,
            $$__NUMBER_FIELDS, $$__NUMBER_PARAMS,
            $$__MODEL_field_names, $$__MODEL_latex_names,
            $$__MODEL_param_names, $$__MODEL_platx_names, ps),
          gauge_xfm(Mp, ps)
        {
          return;
        }


      template <typename number>
      $$__MODEL<number>::~$$__MODEL()
        {
          return;
        }


      template <typename number>
      number $$__MODEL<number>::V(std::vector<number> fields)
        {
          assert(fields.size() == $$__NUMBER_FIELDS);

          const auto $$__PARAMETER[1] = this->parameters[$$__1];
          const auto $$__FIELD[a]     = fields[$$__a];
          const auto __Mp             = this->M_Planck;

          number rval = $$__V;

          return(rval);
        }


      // Integrate the background - on the CPU


      template <typename number>
      transport::background<number> $$__MODEL<number>::background(const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> x = ics;
          this->fix_initial_conditions(ics, x);
          this->write_initial_conditions(x, std::cout, $$__BACKG_ABS_ERR, $$__BACKG_REL_ERR, $$__BACKG_STEP_SIZE, "$$__BACKG_STEPPER");

          // set up an observer which writes to this history vector
          // I'd prefer to encapsulate the history within the observer object, but for some reason
          // that doesn't seem to work (maybe related to the way odeint uses templates?)
          std::vector<double>                slices;
          std::vector< std::vector<number> > history;
          $$__MODEL_background_observer<number> obs(slices, history);

          // set up a functor to evolve this system
          $$__MODEL_background_functor<number>  system(this->parameters, this->M_Planck);

          integrate_times( make_dense_output< $$__BACKG_STEPPER< std::vector<number> > >($$__BACKG_ABS_ERR, $$__BACKG_REL_ERR),
            system, x, times.begin(), times.end(), $$__BACKG_STEP_SIZE, obs);

          transport::background<number> backg($$__NUMBER_FIELDS, $$__MODEL_state_names,
            $$__MODEL_latex_names, slices, history);

          return(backg);
        }


      // Integrate the 2pf - on the GPU


      template <typename number>
      transport::twopf<number> $$__MODEL<number>::twopf(vex::Context& ctx,
        const std::vector<double>& ks, double Nstar,
        const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> hst_bg = ics;
          this->fix_initial_conditions(ics, hst_bg);
          this->write_initial_conditions(hst_bg, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

          // solve for the background, so that we get a good estimate of
          // H_exit -- needed to normalize the comoving momenta k
          std::vector<number> backg_times;
          backg_times.push_back(Nstar);
          transport::background<number> backg_evo = this->background(hst_bg, backg_times);

          // set up vector of ks corresponding to honest comoving momenta
          std::vector<double> com_ks(ks.size());
          this->rescale_ks(ks, com_ks, Nstar, backg_evo.get_value(0));

          // initialize device copy of k list
          vex::vector<double> dev_ks(ctx.queue(), com_ks);
          
          // set up space for the u2-tensor
          vex::multivector<double, (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)> u2_tensor(ctx.queue(), com_ks.size());

          // set up a functor to evolve this system
          $$__MODEL_twopf_functor<number> system(this->parameters, this->M_Planck, dev_ks, u2_tensor);

          twopf_state dev_x(ctx.queue(), com_ks.size());

          // fix initial conditions for the background + 2pf
          // -- background first
          dev_x($$__A) = $$// hst_bg[$$__A];

          // now for 2pf
          std::vector<double> hst_tp(com_ks.size());
          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
                {
                  this->make_tpf_ic(i, j, com_ks, Nstar, hst_bg, hst_tp);
                  vex::copy(hst_tp, dev_x(2*$$__NUMBER_FIELDS+(2*$$__NUMBER_FIELDS*i)+j));
                }
            }

          // set up functor to observe the integration
          std::vector<double>                               slices;
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_history;
          $$__MODEL_twopf_observer<number>                  obs(slices, background_history, twopf_history, ks.size());

          typedef runge_kutta_dopri5<twopf_state, double, twopf_state, double,
            boost::numeric::odeint::vector_space_algebra,
            boost::numeric::odeint::default_operations> stepper;

          integrate_times( make_dense_output<stepper>($$__PERT_ABS_ERR, $$__PERT_REL_ERR),
            system, dev_x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);

          transport::$$__MODEL_gauge_xfm_gadget<number>* gauge_xfm = new $$__MODEL_gauge_xfm_gadget<number>(this->M_Planck, this->parameters);
          transport::twopf<number> tpf($$__NUMBER_FIELDS, $$__MODEL_state_names, $$__MODEL_latex_names, ks, com_ks, Nstar,
            slices, background_history, twopf_history, gauge_xfm);

          return(tpf);
        }


      // Handle initial conditions


      template <typename number>
      void $$__MODEL<number>::fix_initial_conditions(const std::vector<number>& __ics, std::vector<number>& __rics)
        {
          if(__ics.size() == this->N_fields)  // initial conditions for momenta *were not* supplied -- need to compute them
            {
              // supply the missing initial conditions using a slow-roll approximation
              const auto $$__PARAMETER[1] = this->parameters[$$__1];
              const auto $$__FIELD[a]     = __ics[$$__a];
              const auto __Mp             = this->M_Planck;

              __rics.push_back($$__SR_VELOCITY[a]);
            }
          else if(__ics.size() == 2*this->N_fields)  // initial conditions for momenta *were* supplied
            {
              // need do nothing
            }
          else
            {
              std::cerr << __CPP_TRANSPORT_WRONG_ICS_A << __ics.size()
                        << __CPP_TRANSPORT_WRONG_ICS_B << $$__NUMBER_FIELDS
                        << __CPP_TRANSPORT_WRONG_ICS_C << 2*$$__NUMBER_FIELDS << ")" << std::endl;
              exit(EXIT_FAILURE);
            }
        }


      template <typename number>
      void $$__MODEL<number>::write_initial_conditions(const std::vector<number>& ics, std::ostream& stream,
        double abs_err, double rel_err, double step_size, std::string stepper_name)
        {
          stream << __CPP_TRANSPORT_SOLVING_ICS_MESSAGE << std::endl;

          assert(ics.size() == 2*this->N_fields);

          for(int i = 0; i < this->N_fields; i++)
            {
              stream << "  " << this->field_names[i] << " = " << ics[i]
                     << "; d(" << this->field_names[i] << ")/dN = " << ics[this->N_fields+i] << std::endl;
            }

          stream << __CPP_TRANSPORT_STEPPER_MESSAGE    << " '"  << stepper_name
            << "', " << __CPP_TRANSPORT_ABS_ERR   << " = " << abs_err
            << ", "  << __CPP_TRANSPORT_REL_ERR   << " = " << rel_err
            << ", "  << __CPP_TRANSPORT_STEP_SIZE << " = " << step_size << std::endl;

          stream << std::endl;
        }


      template <typename number>
      void $$__MODEL<number>::make_tpf_ic(unsigned int __i, unsigned int __j,
        const std::vector<double>& __ks, double __Nstar,
        const std::vector<number>& __fields, std::vector<double>& __tpf)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __fields[$$__A];
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = $$__HUBBLE_SQ;

          for(int __n = 0; __n < __ks.size(); __n++)
            {
              if(__i < $$__NUMBER_FIELDS && __j < $$__NUMBER_FIELDS)        // field-field correlation function
                {
                  if(__i == __j)
                    {
                      __tpf[__n] = (1.0/(2.0*__ks[__n]));
                    }
                  else
                    {
                      __tpf[__n] = 0.0;
                    }
                }
              else if((__i < $$__NUMBER_FIELDS && __j >= $$__NUMBER_FIELDS)   // field-momentum correlation function
                      || (__i >= $$__NUMBER_FIELDS && __j < $$__NUMBER_FIELDS))
                {
                  if(__i >= $$__NUMBER_FIELDS) __i -= $$__NUMBER_FIELDS;
                  if(__j >= $$__NUMBER_FIELDS) __j -= $$__NUMBER_FIELDS;

                  if(__i == __j)
                    {
                      __tpf[__n] = -(1.0/(2.0*__ks[__n]));
                    }
                  else
                    {
                      __tpf[__n] = 0.0;
                    }
                }
              else if(__i >= $$__NUMBER_FIELDS && __j >= $$__NUMBER_FIELDS)   // momentum-momentum correlation function
                {
                  if(__i == __j)
                    {
                      __tpf[__n] = (__ks[__n]/(2.0*__Hsq));
                    }
                  else
                    {
                      __tpf[__n] = 0.0;
                    }
                }
              else
                {
                  assert(false);
                }
            }
        }


      template <typename number>
      void $$__MODEL<number>::rescale_ks(const std::vector<double>& __ks, std::vector<double>& __com_ks,
        double __Nstar, const std::vector<number>& __fields)
        {
          assert(__fields.size() == 2*$$__NUMBER_FIELDS);
          assert(__ks.size() == __com_ks.size());

          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __fields[$$__A];
          const auto __Mp              = this->M_Planck;

          // __fields should be the field configuration at horizon
          // exit, so that __Hsq gives the Hubble rate there
          // (not at the start of the integration)
          const auto __Hsq             = $$__HUBBLE_SQ;

          for(int __n = 0; __n < __ks.size(); __n++)
            {
              __com_ks[__n] = __ks[__n] * sqrt(__Hsq) * exp(__Nstar);
            }
        }

      // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


      template <typename number>
      void $$__MODEL_background_functor<number>::operator()(const std::vector<number>& __x, std::vector<number>& __dxdt, double __t)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __x[$$__A];
          const auto __Mp              = this->M_Planck;

          __dxdt[$$__A]                = $$__U1_TENSOR[A];
        }


      // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


      template <typename number>
      void $$__MODEL_background_observer<number>::operator()(const std::vector<number>& x, double t)
        {
          this->slices.push_back(t);
          this->history.push_back(x);
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


      template <typename number>
      void $$__MODEL_twopf_functor<number>::operator()(const twopf_state& __x, twopf_state& __dxdt, double __t)
        {
          const auto $$__PARAMETER[1]  = vex::tag<$$__UNIQUE>(this->parameters[$$__1]);
          const auto $$__COORDINATE[A] = vex::tag<$$__UNIQUE>(__x($$__A));
          const auto __Mp              = vex::tag<$$__UNIQUE>(this->M_Planck);
          const auto __k               = vex::tag<$$__UNIQUE>(this->k_list);
          const auto __a               = vex::tag<$$__UNIQUE>(exp(__t));

          const unsigned int start_background = 0;
          const unsigned int start_twopf      = 2*$$__NUMBER_FIELDS;

          const auto __tpf_$$__A_$$__B = $$// vex::tag<$$__UNIQUE>(__x(start_twopf+(2*$$__NUMBER_FIELDS*$$__A)+$$__B));
          auto __u2_$$__A_$$__B        = $$// vex::tag<$$__UNIQUE>(this->u2_tensor((2*$$__NUMBER_FIELDS*$$__A)+$$__B));

          #define __background(a) __dxdt(start_background + a)
          #define __dtwopf(a,b)   __dxdt(start_twopf + (2*$$__NUMBER_FIELDS*a) + b)

          // evolve the background
          __background($$__A) = $$__U1_TENSOR[A];

          // set up a k-dependent u2 tensor
          __u2_$$__A_$$__B = $$__U2_TENSOR[AB]{__k,__a};

          // evolve the 2pf
          __dtwopf($$__A,$$__B) = 0 $$// + $$__U2_NAME[AC]{__u2}*__tpf_$$__C_$$__B
          __dtwopf($$__A,$$__B) += 0 $$// + $$__U2_NAME[BD]{__u2}*__tpf_$$__A_$$__D
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


      template <typename number>
      void $$__MODEL_twopf_observer<number>::operator()(const twopf_state& x, double t)
        {
          this->slices.push_back(t);

          // allocate storage for state
          std::vector<number>                hst_background_state(2*$$__NUMBER_FIELDS);
          std::vector< std::vector<number> > hst_state(2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS);

          // copy device state into local storage, and then push it into the history
          // (** TODO how slow is this?)

          // first, background
          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              hst_background_state[i] = x(i)[0];  // only need to make a copy for one k-mode; the rest are all the same
            }
          this->background_history.push_back(hst_background_state);

          // then, two pf
          for(int i = 0; i < 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS; i++)
            {
              // ensure destination is sufficiently large
              hst_state[i].resize(this->k_size);
              vex::copy(x(2*$$__NUMBER_FIELDS + i), hst_state[i]);
            }

          this->twopf_history.push_back(hst_state);
        }


      // IMPLEMENTATION -- GAUGE TRANSFORMATIONS


      template <typename number>
      void $$__MODEL<number>::compute_gauge_xfm_1(const std::vector<number>& __state,
        std::vector<number>& __dN)
        {
          this->gauge_xfm.compute_gauge_xfm_1(__state, __dN);
        }


      template <typename number>
      void $$__MODEL<number>::compute_gauge_xfm_2(const std::vector<number>& __state,
        std::vector< std::vector<number> >& __ddN)
        {
          this->gauge_xfm.compute_gauge_xfm_2(__state, __ddN);
        }


      template <typename number>
      void $$__MODEL<number>::compute_gauge_xfm_3(const std::vector<number>& __state,
        std::vector< std::vector< std::vector<number> > >& __dddN)
        {
          this->gauge_xfm.compute_gauge_xfm_3(__state, __dddN);
        }


      // IMPLEMENTATION - GAUGE TRANSFORMATION GADGET


      template <typename number>
      void $$__MODEL_gauge_xfm_gadget<number>::compute_gauge_xfm_1(const std::vector<number>& __state,
        std::vector<number>& __dN)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __state[$$__A];
          const auto __Mp              = this->M_Planck;

          __dN.resize(2*$$__NUMBER_FIELDS); // ensure enough space
          __dN[$$__A] = $$__ZETA_XFM_1[A];
        }


      template <typename number>
      void $$__MODEL_gauge_xfm_gadget<number>::compute_gauge_xfm_2(const std::vector<number>& __state,
        std::vector< std::vector<number> >& __ddN)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __state[$$__A];
          const auto __Mp              = this->M_Planck;

          __ddN.resize(2*$$__NUMBER_FIELDS);
          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              __ddN[i].resize(2*$$__NUMBER_FIELDS);
            }

          __ddN[$$__A][$$__B] = $$__ZETA_XFM_2[AB];
        }


      template <typename number>
      void $$__MODEL_gauge_xfm_gadget<number>::compute_gauge_xfm_3(const std::vector<number>& __state,
        std::vector< std::vector< std::vector<number> > >& __dddN)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __state[$$__A];
          const auto __Mp              = this->M_Planck;

          __dddN.resize(2*$$__NUMBER_FIELDS);
          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              __dddN[i].resize(2*$$__NUMBER_FIELDS);
              for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
                {
                  __dddN[i][j].resize(2*$$__NUMBER_FIELDS);
                }
            }

          __dddN[$$__A][$$__B][$$__C] = $$__ZETA_XFM_3[ABC];
        }


  }   // namespace transport


#endif  // $$__GUARD
