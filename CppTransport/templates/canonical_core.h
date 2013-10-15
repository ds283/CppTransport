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

#define CHECK_ZERO(c,k1,k2,k3) _Pragma("omp critical") { if(fabs(c) > 1E-5) { std::cerr << "CHECK_ZERO fail: " #c " not zero (value=" << c << ", k1 = " << k1 << ", k2 = " << k2 << ", k3 = " << k3 << ")" << std::endl; exit(1); } }

#define SPECIES(z)             ((z) >= $$__NUMBER_FIELDS ? ((z)-$$__NUMBER_FIELDS) : (z))
#define MOMENTUM(z)            ((z) >= $$__NUMBER_FIELDS ? (z) : ((z)+$$__NUMBER_FIELDS))
#define IS_FIELD(z)            (((z) >= 0)                 && ((z) < $$__NUMBER_FIELDS))
#define IS_MOMENTUM(z)         (((z) >= $$__NUMBER_FIELDS) && ((z) < 2*$$__NUMBER_FIELDS))

#define DEFAULT_ICS_GAP_TOLERANCE (1E-8)

namespace transport
  {
      static std::vector<std::string> $$__MODEL_field_names = $$__FIELD_NAME_LIST;
      static std::vector<std::string> $$__MODEL_latex_names = $$__LATEX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_param_names = $$__PARAM_NAME_LIST;
      static std::vector<std::string> $$__MODEL_platx_names = $$__PLATX_NAME_LIST;
      static std::vector<std::string> $$__MODEL_state_names = $$__STATE_NAME_LIST;


      // *********************************************************************************************


      // gauge transformation gadget
      template <typename number>
      class $$__MODEL_gauge_xfm_gadget : public gauge_xfm_gadget<number>
        {
          public:
            $$__MODEL_gauge_xfm_gadget(number Mp, const std::vector<number>& ps) : gauge_xfm_gadget<number>(Mp, ps) {}

            $$__MODEL_gauge_xfm_gadget* clone() { return(new $$__MODEL_gauge_xfm_gadget(this->M_Planck, this->parameters)); }

            void compute_gauge_xfm_1(const std::vector<number>& __state, std::vector<number>& __dN);
            void compute_gauge_xfm_2(const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN);
        };


      // tensor calculation gadget
      template <typename number>
      class $$__MODEL_tensor_gadget : public tensor_gadget<number>
        {
          public:
            $$__MODEL_tensor_gadget(number Mp, const std::vector<number>& ps) : tensor_gadget<number>(Mp, ps) {}

            $$__MODEL_tensor_gadget* clone() { return(new $$__MODEL_tensor_gadget(this->M_Planck, this->parameters)); }

            std::vector< std::vector<number> >
              u2(const std::vector<number>& __fields, double __k, double __N);
            std::vector< std::vector< std::vector<number> > >
              u3(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N);

            std::vector< std::vector< std::vector<number> > >
              A(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N);
            std::vector< std::vector< std::vector<number> > >
              B(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N);
            std::vector< std::vector< std::vector<number> > >
              C(const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N);
        };


      // *********************************************************************************************


      // CLASS FOR $$__MODEL CORE
      // specific implementations (MPI, OpenMP, VexCL, ...) are later derived from this  common core
      template <typename number>
      class $$__MODEL : public canonical_model<number>
        {
        public:

          $$__MODEL(number Mp, const std::vector<number>& ps);

          ~$$__MODEL();

          // Functions inherited from canonical_model
          number V(std::vector<number> fields);

          // Integrate the background (on the CPU; can be overridden later if desired)
          transport::background<number> background(const std::vector<number>& ics, const std::vector<double>& times, bool silent=false);

          // Compute initial conditions corresponding to some *fixed* set of ics at early times,
          // plus a variable number of e-folds prior to horizon exit
          //  Nstar -- take horizon-crossing at time Nstar
          //  Npre  -- want to have Npre e-folds before horizon-crossing
          // so this amounts to computing initial conditions at time N = Nstar - Npre, taking N=0 at the supplied ics
          std::vector<number> find_ics(const std::vector<number>& ics, double Nstar, double Npre, double tolerance=DEFAULT_ICS_GAP_TOLERANCE);

          // Calculation of gauge-transformation coefficients (to zeta)
          void compute_gauge_xfm_1(const std::vector<number>& __state, std::vector<number>& __dN);

          void compute_gauge_xfm_2(const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN);

        protected:

          void fix_initial_conditions(const std::vector<number>& __ics, std::vector<number>& __rics);

          void write_initial_conditions(const std::vector<number>& rics, std::ostream& stream,
                                        double abs_err, double rel_err, double step_size, std::string stepper_name);

          number make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const std::vector<number>& __fields);

          number make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const std::vector<number>& __fields);

          number make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                                 double kmode_1, double kmode_2, double kmode_3, double __Ninit, const std::vector<number>& __fields);

          void validate_times(const std::vector<double>& times, double Nstar);

          std::vector<double> normalize_comoving_ks(const std::vector<number>& ics, const std::vector<double>& ks,
                                                    double Ninit, double Nstar, bool silent=false);

          void rescale_ks(const std::vector<double>& __ks, std::vector<double>& __com_ks,
                          double __Nstar, const std::vector<number>& __fields, std::ostream& __stream, bool __silent=false);

          $$__MODEL_gauge_xfm_gadget<number> gauge_xfm;
        };


      // integration - background functor
      template <typename number>
      class $$__MODEL_background_functor
        {
        public:
          $$__MODEL_background_functor(const std::vector<number>& p, number Mp) : parameters(p), M_Planck(Mp)
            {
            }

          void operator ()(const std::vector<number>& __x, std::vector<number>& __dxdt, double __t);

        private:
          const number              M_Planck;
          const std::vector<number> parameters;
        };


      // integration - observer object for background only
      template <typename number>
      class $$__MODEL_background_observer
        {
        public:
          $$__MODEL_background_observer(std::vector< std::vector<number> >& h) : history(h)
            {
            }

          void operator ()(const std::vector<number>& x, double t);

        private:
          std::vector< std::vector< number> >& history;
        };


      // IMPLEMENTATION -- CLASS $$__MODEL


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


      // Integrate the background
      // this default implementation just uses the CPU, but it can be overridden in a specific implementation class


      template <typename number>
      transport::background<number> $$__MODEL<number>::background(const std::vector<number>& ics,
        const std::vector<double>& times, bool silent)
        {
          using namespace boost::numeric::odeint;

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> x = ics;
          this->fix_initial_conditions(ics, x);
          if(!silent)
            {
              this->write_initial_conditions(x, std::cout, $$__BACKG_ABS_ERR, $$__BACKG_REL_ERR, $$__BACKG_STEP_SIZE, "$$__BACKG_STEPPER");
            }

          // set up an observer which writes to this history vector
          // I'd prefer to encapsulate the history within the observer object, but for some reason
          // that doesn't seem to work (maybe related to the way odeint uses templates?)
          std::vector< std::vector<number> > history;
          $$__MODEL_background_observer<number> obs(history);

          // set up a functor to evolve this system
          $$__MODEL_background_functor<number> system(this->parameters, this->M_Planck);

          integrate_times($$__MAKE_BACKG_STEPPER{std::vector<number>}, system, x, times.begin(), times.end(), $$__BACKG_STEP_SIZE, obs);

          transport::$$__MODEL_tensor_gadget<number>* tensor = new $$__MODEL_tensor_gadget<number>(this->M_Planck, this->parameters);

          transport::background<number> backg($$__NUMBER_FIELDS, $$__MODEL_state_names,
            $$__MODEL_latex_names, times, history, tensor);

          return(backg);
        }


      template <typename number>
      std::vector<number> $$__MODEL<number>::find_ics(const std::vector<number>& ics, double Ncross, double Npre, double tolerance)
        {
          using namespace boost::numeric::odeint;

          assert(Ncross >= Npre);

          std::vector<number> rval;

          if(Ncross-Npre < tolerance)
            {
              rval = ics;
            }
          else
            {
              // validate initial conditions
              std::vector<number> x = ics;
              this->fix_initial_conditions(ics, x);

              // set up observer
              std::vector< std::vector<number> > history;
              $$__MODEL_background_observer<number> obs(history);

              // set up functor
              $$__MODEL_background_functor<number> system(this->parameters, this->M_Planck);

              // set up times
              std::vector<double> times;
              const auto default_time_steps = 20;

              for(int i = 0; i <= default_time_steps; i++)
                {
                  times.push_back(0 + (Ncross-Npre)*((double)i/(double)default_time_steps));
                }

              transport::background<number> backg_evo = this->background(ics, times, true); // enforce silent mode

              rval = backg_evo.__INTERNAL_ONLY_get_value(default_time_steps);
            }

          return(rval);
        }


      template <typename number>
      void $$__MODEL<number>::validate_times(const std::vector<double>& times, double Nstar)
        {
          if(times.size() == 0)
            {
              std::cerr << __CPP_TRANSPORT_NO_TIMES << std::endl;
              exit(1);
            }

          if(*(times.begin()) > Nstar)
            {
              std::cerr << __CPP_TRANSPORT_NSTAR_TOO_EARLY << " (Ninit = " << *(times.begin()) << ", Nstar = " << Nstar << ")" << std::endl;
              exit(1);
            }

          // remember that end() returns an iterator pointing past the end of the vector, so
          // it must be decremented before it can be used
          if(*(--times.end()) < Nstar)
            {
              std::cerr << __CPP_TRANSPORT_NSTAR_TOO_LATE << " (Nend = " << *(--times.end()) << ", Nstar = " << Nstar << ")" << std::endl;
              exit(1);
            }
        }


      template <typename number>
      std::vector<double> $$__MODEL<number>::normalize_comoving_ks(const std::vector<number>& ics, const std::vector<double>& ks,
        double Ninit, double Nstar, bool silent)
        {
          assert(Nstar > Ninit);

          // solve for the background, so that we get a good estimate of
          // H_exit -- needed to normalize the comoving momenta k
          std::vector<double> backg_times;
          const auto default_time_steps = 20;

          for(int i = 0; i <= default_time_steps; i++)
            {
              backg_times.push_back(Ninit + (Nstar-Ninit)*((double)i/(double)default_time_steps));
            }

          transport::background<number> backg_evo = this->background(ics, backg_times, true); // enforce silent mode

          // set up vector of ks corresponding to the correctly-normalized comoving momenta
          // (remember we use the convention a = exp(t), for whatever values of t the user supplies)
          std::vector<double> com_ks(ks.size());

          // the field configuration at time Nstar should be stored as the *second* entry
          // in the background container
          this->rescale_ks(ks, com_ks, Nstar, backg_evo.__INTERNAL_ONLY_get_value(default_time_steps), std::cout, silent);

          return(com_ks);
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
      // __i,__j  -- label component of the twopf for which we wish to compute initial conditions
      // __k      -- *comoving normalized* wavenumber for which we wish to assign initial conditions
      // __Ninit  -- initial time
      // __fields -- vector of initial conditions for the background fields (or fields+momenta)
      template <typename number>
      number $$__MODEL<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j,
        double __k, double __Ninit, const std::vector<number>& __fields)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __fields[$$__A];
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = $$__HUBBLE_SQ;
          const auto __eps             = $$__EPSILON;
          const auto __ainit           = exp(__Ninit);

          const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

          number     __tpf             = 0.0;

          std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __M;

          __M[$$__a][$$__b] = $$__M_PREDEF[ab]{__Hsq, __eps};

          // NOTE - conventions for the scale factor are
          //   a = exp(t), where t is the user-defined time (usually = 0 at the start of the integration)
          //   so usually this is zero

          if(IS_FIELD(__i) && IS_FIELD(__j))              // field-field correlation function
            {
              __tpf = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N)) / (2.0*__k*__ainit*__ainit);
            }
          else if((IS_FIELD(__i) && IS_MOMENTUM(__j))     // field-momentum or momentum-field correlation function
                  || (IS_MOMENTUM(__i) && IS_FIELD(__j)))
            {
              auto __leading   = M_PI*__M[SPECIES(__i)][SPECIES(__j)];
              auto __subl      = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__N*__eps);
                                 -(__M[SPECIES(__i)][SPECIES(__j)] - (SPECIES(__i) == SPECIES(__j) ? __eps : 0.0));

              __tpf = -__leading/(2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit);
                      -__subl   /(2.0*__k*__ainit*__ainit);
            }
          else if(IS_MOMENTUM(__i) && IS_MOMENTUM(__j))   // momentum-momentum correlation function
            {
              __tpf = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N)) * __k / (2.0*__Hsq*__ainit*__ainit*__ainit*__ainit);
            }
          else
            {
              assert(false);
            }

          return(__tpf);
        }


    // set up initial conditions for the imaginary part of the equal-time two-point function
    template <typename number>
    number $$__MODEL<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j,
      double __k, double __Ninit, const std::vector<number>& __fields)
      {
        const auto $$__PARAMETER[1]  = this->parameters[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = this->M_Planck;

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __ainit           = exp(__Ninit);

        const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

        number     __tpf             = 0.0;

        std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __M;

        __M[$$__a][$$__b] = $$__M_PREDEF[ab]{__Hsq, __eps};

        // only the field-momentum and momentum-field correlation functions have imgainary parts
        if(IS_FIELD(__i) && IS_MOMENTUM(__j))
          {
            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2*__eps*(1.0-__N))
                             - 4.0*(__M[SPECIES(__i)][SPECIES(__j)] - (SPECIES(__i) == SPECIES(__j) ? __eps : 0.0));
            auto __subl    = M_PI*__M[SPECIES(__i)][SPECIES(__j)];

            __tpf = +__leading/(2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit)
                    -__subl   /(2.0*__k*__ainit*__ainit);
          }
        else if(IS_MOMENTUM(__i) && IS_FIELD(__j))
          {
            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2*__eps*(1.0-__N))
              - 4.0*(__M[SPECIES(__i)][SPECIES(__j)] - (SPECIES(__i) == SPECIES(__j) ? __eps : 0.0));
            auto __subl    = M_PI*__M[SPECIES(__i)][SPECIES(__j)];

            __tpf = -__leading/(2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit)
                    +__subl   /(2.0*__k*__ainit*__ainit);
          }

        return(__tpf);
      }


      template <typename number>
      void $$__MODEL<number>::rescale_ks(const std::vector<double>& __ks, std::vector<double>& __com_ks,
        double __Nstar, const std::vector<number>& __fields, std::ostream& __stream, bool __silent)
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

          if(!__silent)
            {
              __stream << "Normalization of comoving wavenumbers" << std::endl;

              for(int i = 0; i < this->N_fields; i++)
                {
                  __stream << "  " << this->field_names[i] << " = " << __fields[i]
                           << "; d(" << this->field_names[i] << ")/dN = " << __fields[this->N_fields+i] << std::endl;
                }
              __stream << "  H = " << sqrt(__Hsq) << std::endl << std::endl;
            }

          // the normalization convention for the 'unscaled'
          // or comoving mode is that k-comoving = 1
          // corresponds to the mode which crosses the horizon at N=Nstar
          for(int __n = 0; __n < __ks.size(); __n++)
            {
              __com_ks[__n] = __ks[__n] * sqrt(__Hsq) * exp(__Nstar);
            }
        }

    
      template <typename number>
      number $$__MODEL<number>::make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
        double __kmode_1, double __kmode_2, double __kmode_3, double __Ninit, const std::vector<number>& __fields)
        {
          const auto $$__PARAMETER[1]       = this->parameters[$$__1];
          const auto $$__COORDINATE[A]      = __fields[$$__A];
          const auto __Mp                   = this->M_Planck;
          
          const auto __Hsq                  = $$__HUBBLE_SQ;
          const auto __eps                  = $$__EPSILON;
          const auto __ainit                = exp(__Ninit);

          const auto __kt                   = __kmode_1 + __kmode_2 + __kmode_3;
          const auto __Ksq                  = __kmode_1*__kmode_2 + __kmode_1*__kmode_3 + __kmode_2*__kmode_3;
          const auto __kprod3               = 4.0 * __kmode_1*__kmode_1*__kmode_1 * __kmode_2*__kmode_2*__kmode_2 * __kmode_3*__kmode_3*__kmode_3;

          const auto __k2dotk3              = (__kmode_1*__kmode_1 - __kmode_2*__kmode_2 - __kmode_3*__kmode_3)/2.0;
          const auto __k1dotk3              = (__kmode_2*__kmode_2 - __kmode_1*__kmode_1 - __kmode_3*__kmode_3)/2.0;
          const auto __k1dotk2              = (__kmode_3*__kmode_3 - __kmode_1*__kmode_1 - __kmode_2*__kmode_2)/2.0;

          number     __tpf                  = 0.0;

          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k1k2k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k1k3k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k1k2k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k1k3k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k1k2k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k1k3k2;

          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k2k1k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k2k3k1;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k2k1k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k2k3k1;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k2k1k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k2k3k1;

          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k3k1k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k3k2k1;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k3k1k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k3k2k1;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k3k1k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k3k2k1;

          __A_k1k2k3[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_1, __kmode_2, __kmode_3, __ainit, __Hsq, __eps};
          __A_k1k3k2[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_1, __kmode_3, __kmode_2, __ainit, __Hsq, __eps};
          __B_k1k2k3[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_1, __kmode_2, __kmode_3, __ainit, __Hsq, __eps};
          __B_k1k3k2[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_1, __kmode_3, __kmode_2, __ainit, __Hsq, __eps};
          __C_k1k2k3[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_1, __kmode_2, __kmode_3, __ainit, __Hsq, __eps};
          __C_k1k3k2[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_1, __kmode_3, __kmode_2, __ainit, __Hsq, __eps};

          __A_k2k3k1[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_2, __kmode_3, __kmode_1, __ainit, __Hsq, __eps};
          __A_k2k1k3[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_2, __kmode_1, __kmode_3, __ainit, __Hsq, __eps};
          __B_k2k3k1[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_2, __kmode_3, __kmode_1, __ainit, __Hsq, __eps};
          __B_k2k1k3[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_2, __kmode_1, __kmode_3, __ainit, __Hsq, __eps};
          __C_k2k3k1[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_2, __kmode_3, __kmode_1, __ainit, __Hsq, __eps};
          __C_k2k1k3[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_2, __kmode_1, __kmode_3, __ainit, __Hsq, __eps};

          __A_k3k1k2[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_3, __kmode_1, __kmode_2, __ainit, __Hsq, __eps};
          __A_k3k2k1[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_3, __kmode_2, __kmode_1, __ainit, __Hsq, __eps};
          __B_k3k1k2[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_3, __kmode_1, __kmode_2, __ainit, __Hsq, __eps};
          __B_k3k2k1[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_3, __kmode_2, __kmode_1, __ainit, __Hsq, __eps};
          __C_k3k1k2[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_3, __kmode_1, __kmode_2, __ainit, __Hsq, __eps};
          __C_k3k2k1[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_3, __kmode_2, __kmode_1, __ainit, __Hsq, __eps};

          // check symmetries of the A, B, C tensors
          // division by zero errors may occur if any of these components are not zero, but only if
          // the compiler is not able to optimize away the check
          // that itself indicates an error, since both terms should be zero giving 0-0
#ifdef CHECK_IC_SYMMETRIES
          // A tensor, first pair of indices
          CHECK_ZERO((__A_k1k2k3[$$__a][$$__b][$$__c] - __A_k2k1k3[$$__b][$$__a][$$__c])/__A_k1k2k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__A_k1k3k2[$$__a][$$__b][$$__c] - __A_k3k1k2[$$__b][$$__a][$$__c])/__A_k2k1k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__A_k2k3k1[$$__a][$$__b][$$__c] - __A_k3k2k1[$$__b][$$__a][$$__c])/__A_k3k2k1[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;

          // A tensor, second pair of indices
          CHECK_ZERO((__A_k1k2k3[$$__a][$$__b][$$__c] - __A_k1k3k2[$$__a][$$__c][$$__b])/__A_k1k2k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__A_k2k1k3[$$__a][$$__b][$$__c] - __A_k2k3k1[$$__a][$$__c][$$__b])/__A_k2k1k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__A_k3k1k2[$$__a][$$__b][$$__c] - __A_k3k2k1[$$__a][$$__c][$$__b])/__A_k3k2k1[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;

          // A tensor, third pair of indices
          CHECK_ZERO((__A_k2k1k3[$$__a][$$__b][$$__c] - __A_k3k1k2[$$__c][$$__b][$$__a])/__A_k1k2k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__A_k1k2k3[$$__a][$$__b][$$__c] - __A_k3k2k1[$$__c][$$__b][$$__a])/__A_k2k1k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__A_k1k3k2[$$__a][$$__b][$$__c] - __A_k2k3k1[$$__c][$$__b][$$__a])/__A_k3k2k1[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;

          // B tensor: only symmetric on first pair of indices
          CHECK_ZERO((__B_k1k2k3[$$__a][$$__b][$$__c] - __B_k2k1k3[$$__b][$$__a][$$__c])/__B_k1k2k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__B_k1k3k2[$$__a][$$__b][$$__c] - __B_k3k1k2[$$__b][$$__a][$$__c])/__B_k2k1k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__B_k2k3k1[$$__a][$$__b][$$__c] - __B_k3k2k1[$$__b][$$__a][$$__c])/__B_k3k2k1[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;

          // C tensor: only symmetric on first pair of indices
          CHECK_ZERO((__C_k1k2k3[$$__a][$$__b][$$__c] - __C_k2k1k3[$$__b][$$__a][$$__c])/__C_k1k2k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__C_k1k3k2[$$__a][$$__b][$$__c] - __C_k3k1k2[$$__b][$$__a][$$__c])/__C_k2k1k3[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
          CHECK_ZERO((__C_k2k3k1[$$__a][$$__b][$$__c] - __C_k3k2k1[$$__b][$$__a][$$__c])/__C_k3k2k1[$$__a][$$__b][$$__c],__kmode_1,__kmode_2,__kmode_3) $$// ;
#endif

          unsigned int total_fields  = (IS_FIELD(__i)    ? 1 : 0) + (IS_FIELD(__j)    ? 1 : 0) + (IS_FIELD(__k)    ? 1 : 0);
          unsigned int total_momenta = (IS_MOMENTUM(__i) ? 1 : 0) + (IS_MOMENTUM(__j) ? 1 : 0) + (IS_MOMENTUM(__k) ? 1 : 0);

          assert(total_fields + total_momenta == 3);

          switch(total_fields)
            {
              case 3:   // field-field-field correlation function
                {
                  assert(total_fields == 3);
                  // prefactor here is dimension 5
                  auto __prefactor = (__kmode_1*__kmode_1) * (__kmode_2*__kmode_2) * (__kmode_3*__kmode_3) / (__kt * __ainit*__ainit*__ainit*__ainit);

                  // these components are dimension 3, so suppress by two powers of Mp
                  // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                  __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                  // these components are dimension 1
                  __tpf += - (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_1*__kmode_2/2.0;
                  __tpf += - (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_1*__kmode_3/2.0;
                  __tpf += - (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_2*__kmode_3/2.0;

                  __tpf *= __prefactor / __kprod3;
                  break;
                }

              case 2:   // field-field-momentum correlation function
                {
                  assert(total_fields == 2);

                  auto __momentum_k = (IS_MOMENTUM(__i) ? __kmode_1 : 0.0) + (IS_MOMENTUM(__j) ? __kmode_2 : 0.0) + (IS_MOMENTUM(__k) ? __kmode_3 : 0.0);

                  // note the leading + sign, switched from written notes, from d/dN = d/(H dt) = d/(aH d\tau) = -\tau d/d\tau
                  // this prefactor has dimension 2
                  auto __prefactor_1 = __momentum_k*__momentum_k*(__kt-__momentum_k) / (__kt * __ainit*__ainit*__ainit*__ainit);
                  
                  // these components are dimension 6, so suppress by two powers of Mp
                  // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                  auto __tpf_1  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k2dotk3 / (2.0*__Mp*__Mp);
                       __tpf_1 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k1dotk3 / (2.0*__Mp*__Mp);
                       __tpf_1 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k1dotk2 / (2.0*__Mp*__Mp);
                  
                  // these components are dimension 4
                       __tpf_1 += - (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*__kmode_3;
                       __tpf_1 += - (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*__kmode_2;
                       __tpf_1 += - (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*__kmode_1;
                  
                  __tpf = __prefactor_1 * __tpf_1;
                  
                  // this prefactor has dimension 3; the leading minus sign is again switched
                  auto __prefactor_2 = __momentum_k*__kmode_1*__kmode_2*__kmode_3 / (__kt * __ainit*__ainit*__ainit*__ainit);
                  
                  // these components are dimension 5, so suppress by two powers of Mp
                  // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                  auto __tpf_2  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                       __tpf_2 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                       __tpf_2 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);
                  
                  // these componennts are dimension 3
                       __tpf_2 += (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*(1.0+__kmode_3/__kt) / 2.0;
                       __tpf_2 += (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*(1.0+__kmode_2/__kt) / 2.0;
                       __tpf_2 += (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*(1.0+__kmode_1/__kt) / 2.0;

                  // these components are dimension 3
                       __tpf_2 += (__B_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __B_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_1*__kmode_1*__kmode_2*__kmode_3/2.0;
                       __tpf_2 += (__B_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __B_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_2*__kmode_2*__kmode_1*__kmode_3/2.0;
                       __tpf_2 += (__B_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __B_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_3*__kmode_3*__kmode_1*__kmode_2/2.0;
                  
                  __tpf += __prefactor_2 * __tpf_2;
                  
                  __tpf *= (1.0 / __kprod3);
                  break;
                }

              case 1:   // field-momentum-momentum correlation function
                {
                  assert(total_fields == 1);

                  // this prefactor has dimension 3
                  auto __prefactor = - (__kmode_1*__kmode_1 * __kmode_2*__kmode_2 * __kmode_3*__kmode_3) / (__kt * __Hsq * __ainit*__ainit*__ainit*__ainit*__ainit*__ainit);

                  // this term (really another part of the prefactor -- but shouldn't be symmetrized) has dimension 2)
                  auto __mom_factor = (IS_FIELD(__i) ? __kmode_2*__kmode_3 : 0.0) + (IS_FIELD(__j) ? __kmode_1*__kmode_3 : 0.0) + (IS_FIELD(__k) ? __kmode_1*__kmode_2 : 0.0);

                  // these components have dimension 3, so suppress by two powers of Mp
                  // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                  __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);
                
                  // these components have dimension 1
                  __tpf += - (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)]) * __kmode_1*__kmode_2;
                  __tpf += - (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)]) * __kmode_1*__kmode_3;
                  __tpf += - (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)]) * __kmode_2*__kmode_3;
                
                  __tpf *= __prefactor * __mom_factor / __kprod3;
                  break;
                }

              case 0:   // momentum-momentum-momentum correlation function
                {
                  assert(total_fields == 0);

                  // prefactor is dimension 3; note the leading - sign, switched from written notes, from d/dN = d/(H dt) = d/(aH d\tau) = -\tau d/d\tau
                  auto __prefactor = -(__kmode_1*__kmode_1) * (__kmode_2*__kmode_2) * (__kmode_3*__kmode_3) / (__kt * __Hsq * __ainit*__ainit*__ainit*__ainit*__ainit*__ainit);

                  // these components are dimension 5, so suppress by two powers of Mp
                  // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                  __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);

                  // these components are dimension 2
                  __tpf += (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*(1.0+__kmode_3/__kt) / 2.0;
                  __tpf += (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*(1.0+__kmode_2/__kt) / 2.0;
                  __tpf += (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*(1.0+__kmode_1/__kt) / 2.0;

                  // these components are dimension 2
                  __tpf += (__B_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __B_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_1*__kmode_1*__kmode_2*__kmode_3/2.0;
                  __tpf += (__B_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __B_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_2*__kmode_2*__kmode_1*__kmode_3/2.0;
                  __tpf += (__B_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __B_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_3*__kmode_3*__kmode_1*__kmode_2/2.0;

                  __tpf *= __prefactor / __kprod3;
                  break;
                }

              default:
                assert(false);
            }

          return(__tpf);
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
          this->history.push_back(x);
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


      // IMPLEMENTATION - TENSOR CALCULATION GADGET


      template <typename number>
      std::vector< std::vector<number> > $$__MODEL_tensor_gadget<number>::u2(const std::vector<number>& __fields,
        double __k, double __N)
        {
          const auto $$__PARAMETER[1]       = this->parameters[$$__1];
          const auto $$__COORDINATE[A]      = __fields[$$__A];
          const auto __Mp                   = this->M_Planck;

          const auto __Hsq                  = $$__HUBBLE_SQ;
          const auto __eps                  = $$__EPSILON;
          const auto __a                    = exp(__N);

          std::vector< std::vector<number> > __u2(2*$$__NUMBER_FIELDS);

          for(int __i = 0; __i < 2*$$__NUMBER_FIELDS; __i++)
            {
              __u2[__i].resize(2*$$__NUMBER_FIELDS);
            }

          __u2[$$__a][$$__b] = $$__U2_PREDEF[ab]{__k, __a, __Hsq, __eps};

          return(__u2);
        }


      template <typename number>
      std::vector< std::vector< std::vector<number> > > $$__MODEL_tensor_gadget<number>::u3(const std::vector<number>& __fields,
        double __km, double __kn, double __kr, double __N)
        {
          const auto $$__PARAMETER[1]       = this->parameters[$$__1];
          const auto $$__COORDINATE[A]      = __fields[$$__A];
          const auto __Mp                   = this->M_Planck;

          const auto __Hsq                  = $$__HUBBLE_SQ;
          const auto __eps                  = $$__EPSILON;
          const auto __a                    = exp(__N);

          std::vector< std::vector< std::vector<number> > > __u3(2*$$__NUMBER_FIELDS);

          for(int __i = 0; __i < 2*$$__NUMBER_FIELDS; __i++)
            {
              __u3[__i].resize(2*$$__NUMBER_FIELDS);
              for(int __j = 0; __j < 2*$$__NUMBER_FIELDS; __j++)
                {
                  __u3[__i][__j].resize(2*$$__NUMBER_FIELDS);
                }
            }

          __u3[$$__a][$$__b][$$__c] = $$__U3_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};

          return(__u3);
        }


      template <typename number>
      std::vector< std::vector< std::vector<number> > > $$__MODEL_tensor_gadget<number>::A(const std::vector<number>& __fields,
        double __km, double __kn, double __kr, double __N)
        {
          const auto $$__PARAMETER[1]       = this->parameters[$$__1];
          const auto $$__COORDINATE[A]      = __fields[$$__A];
          const auto __Mp                   = this->M_Planck;

          const auto __Hsq                  = $$__HUBBLE_SQ;
          const auto __eps                  = $$__EPSILON;
          const auto __a                    = exp(__N);

          std::vector< std::vector< std::vector<number> > > __A($$__NUMBER_FIELDS);
          
          for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
            {
              __A[__i].resize($$__NUMBER_FIELDS);
              for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
                {
                  __A[__i][__j].resize($$__NUMBER_FIELDS);
                }
            }
          
          __A[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};

          return(__A);
        }


      template <typename number>
      std::vector< std::vector< std::vector<number> > > $$__MODEL_tensor_gadget<number>::B(const std::vector<number>& __fields,
        double __km, double __kn, double __kr, double __N)
        {
          const auto $$__PARAMETER[1]       = this->parameters[$$__1];
          const auto $$__COORDINATE[A]      = __fields[$$__A];
          const auto __Mp                   = this->M_Planck;

          const auto __Hsq                  = $$__HUBBLE_SQ;
          const auto __eps                  = $$__EPSILON;
          const auto __a                    = exp(__N);

          std::vector< std::vector< std::vector<number> > > __B($$__NUMBER_FIELDS);
          
          for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
            {
              __B[__i].resize($$__NUMBER_FIELDS);
              for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
                {
                  __B[__i][__j].resize($$__NUMBER_FIELDS);
                }
            }
          
          __B[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};

          return(__B);
        }


      template <typename number>
      std::vector< std::vector< std::vector<number> > > $$__MODEL_tensor_gadget<number>::C(const std::vector<number>& __fields,
        double __km, double __kn, double __kr, double __N)
        {
          const auto $$__PARAMETER[1]       = this->parameters[$$__1];
          const auto $$__COORDINATE[A]      = __fields[$$__A];
          const auto __Mp                   = this->M_Planck;

          const auto __Hsq                  = $$__HUBBLE_SQ;
          const auto __eps                  = $$__EPSILON;
          const auto __a                    = exp(__N);

          std::vector< std::vector< std::vector<number> > > __C($$__NUMBER_FIELDS);
          
          for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
            {
              __C[__i].resize($$__NUMBER_FIELDS);
              for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
                {
                  __C[__i][__j].resize($$__NUMBER_FIELDS);
                }
            }
          
          __C[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};

          return(__C);
        }


  }   // namespace transport


#endif  // $$__GUARD
