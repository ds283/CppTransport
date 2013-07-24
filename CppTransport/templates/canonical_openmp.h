//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include <assert.h>
#include <math.h>

#include "boost/numeric/odeint.hpp"

#include "transport/transport.h"

#define SPECIES(z)     (z >= $$__NUMBER_FIELDS ? z-$$__NUMBER_FIELDS : z)
#define IS_FIELD(z)    (z >= 0 && z < $$__NUMBER_FIELDS)
#define IS_MOMENTUM(z) (z >= $$__NUMBER_FIELDS && z < 2*$$__NUMBER_FIELDS)

namespace transport
  {
      static std::vector<std::string> $$__MODEL_field_names = $$__FIELD_NAME_LIST;
      static std::vector<std::string> $$__MODEL_latex_names = $$__LATEX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_param_names = $$__PARAM_NAME_LIST;
      static std::vector<std::string> $$__MODEL_platx_names = $$__PLATX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_state_names = $$__STATE_NAME_LIST;

      // set up a state type for 2pf integration
      typedef std::vector<double> twopf_state;

      // set up a state type for 3pf integration
      typedef std::vector<double> threepf_state;

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
        };


      // *********************************************************************************************


      // CLASS FOR $$__MODEL MODEL
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

              // Integrate background and 2-point function on the CPU, using OpenMP
              transport::twopf<number>
                twopf(const std::vector<double>& ks, double Nstar,
                const std::vector<number>& ics, const std::vector<double>& times);

              // Integrate background, 2-point function and 3-point function on the CPU, using OpenMP
              // this simple implementation works on a cubic lattice of k-modes
//              transport::threepf<number>
              void
                threepf(const std::vector<double>& ks, double Nstar,
                const std::vector<number>& ics, const std::vector<double>& times);

              // Calculation of gauge-transformation coefficients (to zeta)
              // ==========================================================

              void compute_gauge_xfm_1(const std::vector<number>& __state, std::vector<number>& __dN);
              void compute_gauge_xfm_2(const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN);

          protected:
            void                twopf_kmode               (double kmode, const std::vector<double>& times,
                                                           const std::vector<number>& ics, std::vector<double>& slices,
                                                           std::vector< std::vector<number> >& background_history, std::vector< std::vector<number> >& twopf_history);

            void                fix_initial_conditions    (const std::vector<number>& __ics, std::vector<number>& __rics);

            void                write_initial_conditions  (const std::vector<number>& rics, std::ostream& stream,
                                                           double abs_err, double rel_err, double step_size, std::string stepper_name);

            double              make_twopf_re_ic          (unsigned int i, unsigned int j, double k, double __Ninit, const std::vector<number>& __fields);

            double              make_twopf_im_ic          (unsigned int i, unsigned int j, double k, double __Ninit, const std::vector<number>& __fields);

            void                validate_times            (const std::vector<double>& times);
            std::vector<double> normalize_comoving_ks     (const std::vector<number>& ics, const std::vector<double>& ks, double Nstar);
            void                rescale_ks                (const std::vector<double>& __ks, std::vector<double>& __com_ks,
                                                           double __Nstar, const std::vector<number>& __fields);

				    void                resize_twopf_history      (std::vector< std::vector< std::vector<number> > >& twopf_history,
				                                                   const std::vector<double>& times, const std::vector<double>& ks);

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
            $$__MODEL_twopf_functor(const std::vector<number>& p, const number Mp, double k)
              : parameters(p), M_Planck(Mp), k_mode(k) {}

            void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
            const number						   M_Planck;
            const std::vector<number>& parameters;
            const double               k_mode;
        };


      // integration - observer object for 2pf
      template <typename number>
      class $$__MODEL_twopf_observer
        {
          public:
            $$__MODEL_twopf_observer(std::vector<double>& s,
              std::vector< std::vector<number> >& bh,
              std::vector< std::vector<number> >& tpfh)
            : slices(s), background_history(bh), twopf_history(tpfh) {}

            void operator()(const twopf_state& x, double t);

          private:
            std::vector<double>&                slices;
            std::vector< std::vector<number> >& background_history;
            std::vector< std::vector<number> >& twopf_history;
        };


      // integration - 3pf functor
      template <typename number>
      class $$__MODEL_threepf_functor
        {
          public:
            $$__MODEL_threepf_functor(const std::vector<number>& p, const number Mp, double k1, double k2, double k3)
              : parameters(p), M_Planck(Mp), kmode_1(k1), kmode_2(k2), kmode_3(k3) {}

            void operator()(const threepf_state& __x, threepf_state& __dxdt, double __dt);

          private:
            const number               M_Planck;
            const std::vector<number>& parameters;
            const double               kmode_1;
            const double               kmode_2;
            const double               kmode_3;
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

          integrate_times( $$__MAKE_BACKG_STEPPER{std::vector<number>}, system, x, times.begin(), times.end(), $$__BACKG_STEP_SIZE, obs);

          transport::background<number> backg($$__NUMBER_FIELDS, $$__MODEL_state_names,
            $$__MODEL_latex_names, slices, history);

          return(backg);
        }


      // Integrate the 2pf - on the CPU, using OpenMP


      template <typename number>
      transport::twopf<number> $$__MODEL<number>::twopf(const std::vector<double>& ks, double Nstar,
        const std::vector<number>& ics, const std::vector<double>& times)
        {
          this->validate_times(times);

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> real_ics = ics;
          this->fix_initial_conditions(ics, real_ics);
          this->write_initial_conditions(real_ics, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

          std::vector<double> com_ks = this->normalize_comoving_ks(real_ics, ks, Nstar);

          // space for storing the solution
          std::vector<double>                               slices;               // record times at which we sample the solution
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_history;

          // ensure there is sufficient space for the solution
          // the index convention is:
          //   first index  - time
          //   second index - component number
          //   third index  - k mode
			    this->resize_twopf_history(twopf_history, times, ks);

          for(int i = 0; i < ks.size(); i++)
            {
              std::vector<double>                kmode_slices;
              std::vector< std::vector<number> > kmode_background_history;
              std::vector< std::vector<number> > kmode_twopf_history;

              // write the time history for this particular k-mode into kmode_background_history, kmode_twopf_history
              this->twopf_kmode(com_ks[i], times, real_ics, kmode_slices, kmode_background_history, kmode_twopf_history);

              if(i == 0)  // store the background
                {
                  slices = kmode_slices;
                  background_history = kmode_background_history;
                }

              // store this twopf history in the twopf_history object
              for(int j = 0; j < kmode_twopf_history.size(); j++)
                {
                  for(int k = 0; k < kmode_twopf_history[j].size(); k++)
                    {
                      twopf_history[j][k][i] = kmode_twopf_history[j][k];
                    }
                }
            }

          transport::$$__MODEL_gauge_xfm_gadget<number>* gauge_xfm = new $$__MODEL_gauge_xfm_gadget<number>(this->M_Planck, this->parameters);
          transport::twopf<number> tpf($$__NUMBER_FIELDS, $$__MODEL_state_names, $$__MODEL_latex_names, ks, com_ks, Nstar,
            slices, background_history, twopf_history, gauge_xfm);

          return(tpf);
        }


      template <typename number>
      void $$__MODEL<number>::twopf_kmode(double kmode, const std::vector<double>& times,
        const std::vector<number>& ics, std::vector<double>& slices,
        std::vector< std::vector<number> >& background_history, std::vector< std::vector<number> >& twopf_history)
        {
          using namespace boost::numeric::odeint;

          // set up a functor to evolve this system
          $$__MODEL_twopf_functor<number>  system(this->parameters, this->M_Planck, kmode);

          // set up a functor to observe the integration
          $$__MODEL_twopf_observer<number> obs(slices, background_history, twopf_history);

          const auto background_start = 0;
          const auto background_size  = 2*$$__NUMBER_FIELDS;
          const auto twopf_start      = background_start + background_size;
          const auto twopf_size       = (2*$$__NUMBER_FIELDS) * (2*$$__NUMBER_FIELDS);

          // set up a state vector
          twopf_state x;
          x.resize(background_size + twopf_size);

          // fix initial conditions - background
          x[background_start + $$__A] = $$// ics[$$__A];

          // fix initial conditions - 2pf
          for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
            {
              for(int k = 0; k < 2*$$__NUMBER_FIELDS; k++)
                {
                  x[twopf_start + (2*$$__NUMBER_FIELDS*j)+k] = make_twopf_re_ic(j, k, kmode, *times.begin(), ics);
                }
            }

          integrate_times( $$__MAKE_PERT_STEPPER{twopf_state}, system, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);
        }

      template <typename number>
      void $$__MODEL<number>::validate_times(const std::vector<double>& times)
        {
          if(times.size() == 0)
            {
              std::cout << __CPP_TRANSPORT_NO_TIMES << std::endl;
              exit(1);
            }
        }


      template <typename number>
      std::vector<double> $$__MODEL<number>::normalize_comoving_ks(const std::vector<number>& ics, const std::vector<double>& ks,
        double Nstar)
        {
          // solve for the background, so that we get a good estimate of
          // H_exit -- needed to normalize the comoving momenta k
          std::vector<number> backg_times;
          backg_times.push_back(Nstar);
          transport::background<number> backg_evo = this->background(ics, backg_times);

          // set up vector of ks corresponding to honest comoving momenta
          std::vector<double> com_ks(ks.size());

          this->rescale_ks(ks, com_ks, Nstar, backg_evo.get_value(0));

          return(com_ks);
        }


      template <typename number>
      void $$__MODEL<number>::resize_twopf_history(std::vector< std::vector< std::vector<number> > >& twopf_history, const std::vector<double>& times,
        const std::vector<double>& ks)
        {
          twopf_history.resize(times.size());
          for(int i = 0; i < times.size(); i++)
            {
              twopf_history[i].resize(2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS);
              for(int j = 0; j < 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS; j++)
                {
                  twopf_history[i][j].resize(ks.size());
                }
            }
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


      // set up initial conditions for the real part of the equal-time two-point function
      template <typename number>
      double $$__MODEL<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j,
        double __k, double __Ninit, const std::vector<number>& __fields)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __fields[$$__A];
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = $$__HUBBLE_SQ;
          const auto __ainit           = exp(__Ninit);

          double     __tpf             = 0.0;

          // NOTE - conventions for the scale factor are
          //   a = exp(t), where t is the user-defined time (usually = 0 at the start of the integration)
          //   so usually this is zero

          if(IS_FIELD(__i) && IS_FIELD(__j))              // field-field correlation function
            {
              __tpf = (SPECIES(__i) == SPECIES(__j) ? 1.0/(2.0*__k*__ainit*__ainit) : 0.0);
            }
          else if((IS_FIELD(__i) && IS_MOMENTUM(__j))     // field-momentum correlation function
                  || (IS_MOMENTUM(__i) && IS_FIELD(__j)))
            {
              __tpf = (SPECIES(__i) == SPECIES(__j) ? -1.0/(2.0*__k*__ainit*__ainit) : 0.0);
            }
          else if(IS_MOMENTUM(__i) && IS_MOMENTUM(__j))   // momentum-momentum correlation function
            {
              __tpf = (SPECIES(__i) == SPECIES(__j) ? __k/(2.0*__Hsq) : 0.0);
            }
          else
            {
              assert(false);
            }

          return(__tpf);
        }


    // set up initial conditions for the imaginary part of the equal-time two-point function
    template <typename number>
    double $$__MODEL<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j,
      double __k, double __Ninit, const std::vector<number>& __fields)
      {
        const auto $$__PARAMETER[1]  = this->parameters[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = this->M_Planck;

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __ainit           = exp(__Ninit);

        double     __tpf             = 0.0;

        // only the field-momentum and momentum-field correlation functions have imgainary parts
        if(IS_FIELD(__i) && IS_MOMENTUM(__j))
          {
            __tpf = (SPECIES(__i) == SPECIES(__j) ?  1.0/(2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit) : 0.0);
          }
        else if(IS_MOMENTUM(__i) && IS_FIELD(__j))
          {
            __tpf = (SPECIES(__i) == SPECIES(__j) ? -1.0/(2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit) : 0.0);
          }

        return(__tpf);
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


      // Integrate the 3pf - on the CPU, using OpenMP
      // This version does a cubic lattice of k-modes


      template <typename number>
//      transport::threepf<number> $$__MODEL<number>::threepf(const std::vector<double>& ks, double Nstar,
      void $$__MODEL<number>::threepf(const std::vector<double>& ks, double Nstar,
        const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          this->validate_times(times);

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> real_ics = ics;
          this->fix_initial_conditions(ics, real_ics);
          this->write_initial_conditions(real_ics, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");

          std::vector<double> com_ks = this->normalize_comoving_ks(real_ics, ks, Nstar);

          // space for storing the solution
          std::vector<double>                               slices;                 // record times at which we sample the solution
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_history;
          std::vector< std::vector< std::vector<number> > > threepf_history;

          // ensure there is enough space to store the solution
          // the index convention is
          //   first index  - time
          //   second index - component number
          //   third index  - k mode
          //                  for the 2pf this corresponds to the list in ks (real_ks)
          //                  for the 3pf this is an index into the lattice ks^3 (real_ks)^3
			    this->resize_twopf_history(twopf_history, times, ks);
        }


      // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


      template <typename number>
      void $$__MODEL_background_functor<number>::operator()(const std::vector<number>& __x, std::vector<number>& __dxdt, double __t)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __x[$$__A];
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = $$__HUBBLE_SQ;
          const auto __eps             = $$__EPSILON;

          __dxdt[$$__A]                = $$__U1_PREDEF[A]{__Hsq,__eps};
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
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __x[$$__A];
          const auto __Mp              = this->M_Planck;
          const auto __k               = this->k_mode;
          const auto __a               = exp(__t);
          const auto __Hsq             = $$__HUBBLE_SQ;
          const auto __eps             = $$__EPSILON;

          const unsigned int __size_background  = 2*$$__NUMBER_FIELDS;

          const unsigned int __start_background = 0;
          const unsigned int __start_twopf      = __start_background + __size_background;

          const auto __tpf_$$__A_$$__B = $$// __x[__start_twopf + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];

          #undef __background
          #undef __dtwopf
          #define __background(a)   __dxdt[__start_background + a]
          #define __dtwopf(a,b)     __dxdt[__start_twopf      + (2*$$__NUMBER_FIELDS*a) + b]

          // evolve the background
          __background($$__A) = $$__U1_PREDEF[A]{__Hsq, __eps};

          // set up components of the u2 tensor
          const auto __u2_$$__A_$$__B = $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};

          // evolve the 2pf
          // here, we are dealing only with the real part - which is symmetric.
          // so the index placement is not important
          __dtwopf($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2}*__tpf_$$__C_$$__B
          __dtwopf($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2}*__tpf_$$__A_$$__C
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


      template <typename number>
      void $$__MODEL_twopf_observer<number>::operator()(const twopf_state& x, double t)
        {
          this->slices.push_back(t);

          // allocate storage for state
          std::vector<number> bg_x (2*$$__NUMBER_FIELDS);
          std::vector<number> tpf_x(2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS);

          // first, background
          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              bg_x[i] = x[i];
            }
          this->background_history.push_back(bg_x);

          // then, 2pf - we do this for every k number, and we're entitled to assume
          // that there is enough space in this->twopf_history
          for(int i = 0; i < 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS; i++)
            {
              tpf_x[i] = x[2*$$__NUMBER_FIELDS + i];
            }
          this->twopf_history.push_back(tpf_x);
        }


      // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


      template <typename number>
      void $$__MODEL_threepf_functor<number>::operator()(const threepf_state& __x, threepf_state& __dxdt, double __t)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __x[$$__A];
          const auto __Mp              = this->M_Planck;
          const auto __k1              = this->kmode_1;
          const auto __k2              = this->kmode_2;
          const auto __k3              = this->kmode_3;
          const auto __a               = exp(__t);
          const auto __Hsq             = $$__HUBBLE_SQ;
          const auto __eps             = $$__EPSILON;

          const unsigned int __size_background   = 2*$$__NUMBER_FIELDS;
          const unsigned int __size_twopf        = (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS);

          const unsigned int __start_background  = 0;
          const unsigned int __start_twopf_re_k1 = __start_background  + __size_background;
          const unsigned int __start_twopf_im_k1 = __start_twopf_re_k1 + __size_twopf;
          const unsigned int __start_twopf_re_k2 = __start_twopf_im_k1 + __size_twopf;
          const unsigned int __start_twopf_im_k2 = __start_twopf_re_k2 + __size_twopf;
          const unsigned int __start_twopf_re_k3 = __start_twopf_im_k2 + __size_twopf;
          const unsigned int __start_twopf_im_k3 = __start_twopf_re_k3 + __size_twopf;
          const unsigned int __start_threepf     = __start_twopf_im_k3 + __size_twopf;

          const auto __twopf_re_k1_$$__A_$$__B   = $$// __x[__start_twopf_re_k1 + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];
          const auto __twopf_im_k1_$$__A_$$__B   = $$// __x[__start_twopf_im_k1 + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];
          const auto __twopf_re_k2_$$__A_$$__B   = $$// __x[__start_twopf_re_k2 + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];
          const auto __twopf_im_k2_$$__A_$$__B   = $$// __x[__start_twopf_im_k2 + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];
          const auto __twopf_re_k3_$$__A_$$__B   = $$// __x[__start_twopf_re_k3 + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];
          const auto __twopf_im_k3_$$__A_$$__B   = $$// __x[__start_twopf_im_k3 + (2*$$__NUMBER_FIELDS*$$__A)+$$__B];

          #undef __background
          #undef __dtwopf_re_k1
          #undef __dtwopf_im_k1
          #undef __dtwopf_re_k2
          #undef __dtwopf_im_k2
          #undef __dtwopf_re_k3
          #undef __dtwopf_im_k3
          #define __background(a)      __dxdt[__start_background  + a]
          #define __dtwopf_re_k1(a,b)  __dxdt[__start_twopf_re_k1 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_im_k1(a,b)  __dxdt[__start_twopf_im_k1 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_re_k2(a,b)  __dxdt[__start_twopf_re_k2 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_im_k2(a,b)  __dxdt[__start_twopf_im_k2 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_re_k3(a,b)  __dxdt[__start_twopf_re_k3 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_im_k3(a,b)  __dxdt[__start_twopf_im_k3 + (2*$$__NUMBER_FIELDS*a) + b]

          // evolve the background
          __background($$__A) = $$__U1_PREDEF[A]{__Hsq,__eps};

          // set up components of the u2 tensor for k1, k2, k3
          const auto __u2_k1_$$__A_$$__B = $$__U2_PREDEF[AB]{__k1, __a, __Hsq, __eps};
          const auto __u2_k2_$$__A_$$__B = $$__U2_PREDEF[AB]{__k2, __a, __Hsq, __eps};
          const auto __u2_k3_$$__A_$$__B = $$__U2_PREDEF[AB]{__k3, __a, __Hsq, __eps};

          // set up components of the u3 tensor
          const auto __u3_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};

          // evolve the real and imaginary components of the 2pf
          __dtwopf_re_k1($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2_k1}*__twopf_re_k1_$$__C_$$__B
          __dtwopf_re_k1($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k1}*__twopf_re_k1_$$__A_$$__C

          __dtwopf_im_k1($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2_k1}*__twopf_im_k1_$$__C_$$__B
          __dtwopf_im_k1($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k1}*__twopf_im_k1_$$__A_$$__C

          __dtwopf_re_k2($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2_k2}*__twopf_re_k2_$$__C_$$__B
          __dtwopf_re_k2($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k2}*__twopf_re_k2_$$__A_$$__C

          __dtwopf_im_k2($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2_k2}*__twopf_im_k2_$$__C_$$__B
          __dtwopf_im_k2($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k2}*__twopf_im_k2_$$__A_$$__C

          __dtwopf_re_k3($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2_k3}*__twopf_re_k3_$$__C_$$__B
          __dtwopf_re_k3($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k3}*__twopf_re_k3_$$__A_$$__C

          __dtwopf_im_k3($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2_k3}*__twopf_im_k3_$$__C_$$__B
          __dtwopf_im_k3($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2_k3}*__twopf_im_k3_$$__A_$$__C

          // evolve the components of the 3pf
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


  }   // namespace transport


#endif  // $$__GUARD
