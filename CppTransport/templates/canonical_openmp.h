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
#import "threepf.h"

#define SPECIES(z)     (z >= $$__NUMBER_FIELDS ? z-$$__NUMBER_FIELDS : z)
#define MOMENTUM(z)    (SPECIES(z) + $$__NUMBER_FIELDS)
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


      // CLASS FOR $$__MODEL MODEL
      template <typename number>
      class $$__MODEL : public canonical_model<number>
        {
          public:
            $$__MODEL(number Mp, const std::vector<number>& ps);
            ~$$__MODEL();

            // Functions inherited from canonical_model
            number V(std::vector<number> fields);

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
            transport::threepf<number>
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
            void                threepf_kmode             (double kmode_1, double kmode_2, double kmode_3, const std::vector<double>& times,
                                                           const std::vector<number>& ics, std::vector<double>& slices,
                                                           std::vector< std::vector<number> >& background_history,
                                                           std::vector< std::vector<number> >& twopf_re_history, std::vector< std::vector<number> >& twopf_im_history,
                                                           std::vector< std::vector<number> >& threepf_history);

            void                fix_initial_conditions    (const std::vector<number>& __ics, std::vector<number>& __rics);

            void                write_initial_conditions  (const std::vector<number>& rics, std::ostream& stream,
                                                           double abs_err, double rel_err, double step_size, std::string stepper_name);

            void                populate_twopf_ic         (twopf_state& x, unsigned int start, double kmode, double Ninit,
                                                           const std::vector<number>& ic, bool imaginary=false);
            void                populate_threepf_ic       (threepf_state& x, unsigned int start, double kmode_1, double kmode_2, double kmode_3,
                                                           double Ninit, const std::vector<number>& ic);

            number              make_twopf_re_ic          (unsigned int __i, unsigned int __j, double __k, double __Ninit, const std::vector<number>& __fields);
            number              make_twopf_im_ic          (unsigned int __i, unsigned int __j, double __k, double __Ninit, const std::vector<number>& __fields);

            number              make_threepf_ic           (unsigned int __i, unsigned int __j, unsigned int __k,
                                                           double kmode_1, double kmode_2, double kmode_3, double __Ninit, const std::vector<number>& __fields);

            void                validate_times            (const std::vector<double>& times);
            std::vector<double> normalize_comoving_ks     (const std::vector<number>& ics, const std::vector<double>& ks, double Nstar);
            void                rescale_ks                (const std::vector<double>& __ks, std::vector<double>& __com_ks,
                                                           double __Nstar, const std::vector<number>& __fields);

            void                resize_twopf_history      (std::vector< std::vector< std::vector<number> > >& twopf_history,
				                                                   const std::vector<double>& times, const std::vector<double>& ks);
            void                resize_threepf_history    (std::vector< std::vector< std::vector<number> > >& threepf_history,
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
    
    
      // integration - observer object for 3pf
      template <typename number>
      class $$__MODEL_threepf_observer
        {
          public:
            $$__MODEL_threepf_observer(std::vector<double>& s,
              std::vector< std::vector<number> >& bh,
              std::vector< std::vector<number> >& twopfh_re,
              std::vector< std::vector<number> >& twopfh_im,
              std::vector< std::vector<number> >& threepfh)
              : slices(s), background_history(bh), twopf_re_history(twopfh_re), twopf_im_history(twopfh_im), threepf_history(threepfh) {}
          
            void operator()(const threepf_state& x, double t);
          
          private:
            std::vector<double>&                slices;
            std::vector< std::vector<number> >& background_history;
            std::vector< std::vector<number> >& twopf_re_history;
            std::vector< std::vector<number> >& twopf_im_history;
            std::vector< std::vector<number> >& threepf_history;
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

          transport::$$__MODEL_tensor_gadget<number>* tensor = new $$__MODEL_tensor_gadget<number>(this->M_Planck, this->parameters);

          transport::background<number> backg($$__NUMBER_FIELDS, $$__MODEL_state_names,
            $$__MODEL_latex_names, slices, history, tensor);

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

              if(i == 0)  // store the background for the first k-mode only (it's the same each time)
                {
                  slices = kmode_slices;
                  background_history = kmode_background_history;
                }

              // store this twopf history in the twopf_history object
              for(int j = 0; j < kmode_twopf_history.size(); j++)             // j steps through the time-slices
                {
                  for(int k = 0; k < kmode_twopf_history[j].size(); k++)      // k steps through the components
                    {
                      twopf_history[j][k][i] = kmode_twopf_history[j][k];
                    }
                }
            }

          transport::$$__MODEL_gauge_xfm_gadget<number>* gauge_xfm = new $$__MODEL_gauge_xfm_gadget<number>(this->M_Planck, this->parameters);
          transport::$$__MODEL_tensor_gadget<number>*    tensor    = new $$__MODEL_tensor_gadget<number>(this->M_Planck, this->parameters);

          transport::twopf<number> tpf($$__NUMBER_FIELDS, $$__MODEL_state_names, $$__MODEL_latex_names, ks, com_ks, Nstar,
            slices, background_history, twopf_history, gauge_xfm, tensor);

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
          
          const auto twopf_state_size = background_size + twopf_size;

          // set up a state vector
          twopf_state x;
          x.resize(twopf_state_size);

          // fix initial conditions - background
          x[background_start + $$__A] = $$// ics[$$__A];

          // fix initial conditions - 2pf
          this->populate_twopf_ic(x, twopf_start, kmode, *times.begin(), ics);

          integrate_times( $$__MAKE_PERT_STEPPER{twopf_state}, system, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);
        }

    
      template <typename number>
      void $$__MODEL<number>::populate_twopf_ic(twopf_state& x, unsigned int start, double kmode, double Ninit, const std::vector<number>& ics, bool imaginary)
        {
          assert(x.size() >= start);
          assert(x.size() >= start + 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS);

          for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
            {
              for(int k = 0; k < 2*$$__NUMBER_FIELDS; k++)
                {
                  x[start + (2*$$__NUMBER_FIELDS*j)+k] = imaginary ? make_twopf_im_ic(j, k, kmode, Ninit, ics) : make_twopf_re_ic(j, k, kmode, Ninit, ics);
                }
            }
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

          // set up vector of ks corresponding to the correctly-normalized comoving momenta
          // (remember we use the convention a = exp(t), for whatever values of t the user supplies)
          std::vector<double> com_ks(ks.size());

          this->rescale_ks(ks, com_ks, Nstar, backg_evo.__INTERNAL_ONLY_get_value(0));

          return(com_ks);
        }


      template <typename number>
      void $$__MODEL<number>::resize_twopf_history(std::vector< std::vector< std::vector<number> > >& twopf_history, const std::vector<double>& times,
        const std::vector<double>& ks)
        {
          const auto twopf_components_size = 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS;
          const auto twopf_kmodes_size     = ks.size();

          twopf_history.resize(times.size());

          for(int i = 0; i < times.size(); i++)
            {
              twopf_history[i].resize(twopf_components_size);

              for(int j = 0; j < twopf_components_size; j++)
                {
                  // we need one copy of the components for each k
                  twopf_history[i][j].resize(twopf_kmodes_size);
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
      number $$__MODEL<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j,
        double __k, double __Ninit, const std::vector<number>& __fields)
        {
          const auto $$__PARAMETER[1]  = this->parameters[$$__1];
          const auto $$__COORDINATE[A] = __fields[$$__A];
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = $$__HUBBLE_SQ;
          const auto __ainit           = exp(__Ninit);

          number     __tpf             = 0.0;

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
    number $$__MODEL<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j,
      double __k, double __Ninit, const std::vector<number>& __fields)
      {
        const auto $$__PARAMETER[1]  = this->parameters[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = this->M_Planck;

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __ainit           = exp(__Ninit);

        number     __tpf             = 0.0;

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
      transport::threepf<number> $$__MODEL<number>::threepf(const std::vector<double>& ks, double Nstar,
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
          std::vector< std::vector< std::vector<number> > > twopf_re_history;
          std::vector< std::vector< std::vector<number> > > twopf_im_history;
          std::vector< std::vector< std::vector<number> > > threepf_history;
          std::vector< struct threepf_kconfig >             kconfig_list;

          // ensure there is enough space to store the solution
          // the index convention is
          //   first index  - time
          //   second index - component number
          //   third index  - k mode
          //                  for the 2pf this corresponds to the list in ks (real_ks)
          //                  for the 3pf this is an index into the lattice ks^3 (real_ks)^3
			    this->resize_twopf_history(twopf_re_history, times, ks);
			    this->resize_twopf_history(twopf_im_history, times, ks);
          this->resize_threepf_history(threepf_history, times, ks);
          
          // step through the lattice of k-modes, solving the for three-point function on each go
          bool stored_background = false;   // have we stored the background yet?
          unsigned int loc = 0;             // keep track of where we are in the ordered set of (k1,k2,k3)
          for(int i = 0; i < ks.size(); i++)
            {
              bool stored_twopf = false;

              for(int j = 0; j <= i; j++)
                {
                  for(int k = 0; k <= j; k++)
                    {
                      std::vector<double>                kmode_slices;
                      std::vector< std::vector<number> > kmode_background_history;
                      std::vector< std::vector<number> > kmode_twopf_re_history;
                      std::vector< std::vector<number> > kmode_twopf_im_history;
                      std::vector< std::vector<number> > kmode_threepf_history;

                      struct threepf_kconfig kconfig;

                      kconfig.k_t        = com_ks[i] + com_ks[j] + com_ks[k];

                      auto maxij  = (com_ks[i] > com_ks[j] ? com_ks[i] : com_ks[j]);
                      auto maxijk = (maxij     > com_ks[k] ? maxij     : com_ks[k]);

                      if(kconfig.k_t >= 2.0*maxijk)   // impose the triangle conditions
                        {
                          kconfig.indices[0] = i;
                          kconfig.indices[1] = j;
                          kconfig.indices[2] = k;

                          kconfig.beta  = 1.0 - 2.0*com_ks[k]/kconfig.k_t;
                          kconfig.alpha = 4.0*com_ks[i]/kconfig.k_t - 1.0 - kconfig.beta;

                          // write the time history for this k-mode
                          this->threepf_kmode(com_ks[i], com_ks[j], com_ks[k], times, real_ics,
                                              kmode_slices, kmode_background_history, kmode_twopf_re_history, kmode_twopf_im_history, kmode_threepf_history);

                          if(stored_background == false)    // store the background for the first lattice point only (it's the same each time)
                            {
                              slices = kmode_slices;
                              background_history = kmode_background_history;
                              stored_background = true;
                            }

                          if(stored_twopf == false)         // store the twopf corresponding to com_ks[i] (it's the same for all j and k)
                            {
                              for(int m = 0; m < kmode_twopf_re_history.size(); m++)           // m steps through the time-slices
                                {
                                  for(int n = 0; n < kmode_twopf_re_history[m].size(); n++)    // n steps through the components
                                    {
                                      twopf_re_history[m][n][i] = kmode_twopf_re_history[m][n];
                                      twopf_im_history[m][n][i] = kmode_twopf_im_history[m][n];
                                    }
                                }
                              stored_twopf = true;
                            }

                          for(int m = 0; m < kmode_threepf_history.size(); m++)             // m steps through the time-slices
                            {
                              for(int n = 0; n < kmode_threepf_history[m].size(); n++)      // n steps through the components
                                {
                                  threepf_history[m][n][loc] = kmode_threepf_history[m][n];
                                }
                            }
                          loc++;  // increment location in the array of (k1,k2,k3) values
                          kconfig_list.push_back(kconfig);
                        }
                    }
                }

              if(stored_twopf == false)
                {
                  // error
                  exit(1);
                }
            }

          if(stored_background == false)
            {
              // error
              exit(1);
            }
        
          transport::$$__MODEL_gauge_xfm_gadget<number>* gauge_xfm = new $$__MODEL_gauge_xfm_gadget<number>(this->M_Planck, this->parameters);
          transport::$$__MODEL_tensor_gadget<number>*    tensor    = new $$__MODEL_tensor_gadget<number>(this->M_Planck, this->parameters);

          transport::threepf<number> tpf($$__NUMBER_FIELDS, $$__MODEL_state_names, $$__MODEL_latex_names, ks, com_ks, Nstar,
            slices, background_history, twopf_re_history, twopf_im_history, threepf_history, kconfig_list, gauge_xfm, tensor);
        
          return(tpf);
        }
  
  
      template <typename number>
      void $$__MODEL<number>::threepf_kmode(double kmode_1, double kmode_2, double kmode_3, const std::vector<double>& times,
        const std::vector<number>& ics, std::vector<double>& slices,
        std::vector< std::vector<number> >& background_history,
        std::vector< std::vector<number> >& twopf_re_history, std::vector< std::vector<number> >& twopf_im_history,
        std::vector< std::vector<number> >& threepf_history)
        {
          using namespace boost::numeric::odeint;
          
          // set up a functor to evolve this system
          $$__MODEL_threepf_functor<number>  system(this->parameters, this->M_Planck, kmode_1, kmode_2, kmode_3);
          
          // set up a functor to observe the integration
          $$__MODEL_threepf_observer<number> obs(slices, background_history, twopf_re_history, twopf_im_history, threepf_history);
          
          // we have to store:
          //   - 1 copy of the background
          //   - the real 2pf for 3 kmodes
          //   - the imaginary 2pf for 3 kmodes
          //   - the real 3pf for (k1,k2,k3)
          const auto background_start   = 0;
          const auto background_size    = 2*$$__NUMBER_FIELDS;
          const auto twopf_size         = 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS;
          const auto twopf_re_k1_start  = background_start  + background_size;
          const auto twopf_im_k1_start  = twopf_re_k1_start + twopf_size;
          const auto twopf_re_k2_start  = twopf_im_k1_start + twopf_size;
          const auto twopf_im_k2_start  = twopf_re_k2_start + twopf_size;
          const auto twopf_re_k3_start  = twopf_im_k2_start + twopf_size;
          const auto twopf_im_k3_start  = twopf_re_k3_start + twopf_size;
          const auto threepf_start      = twopf_im_k3_start + twopf_size;
          const auto threepf_size       = 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS;
          
          const auto threepf_state_size = background_size + 6*twopf_size + threepf_size;
          
          // set up a state vector
          threepf_state x;
          x.resize(threepf_state_size);
          
          // fix initial conditions - background
          x[background_start + $$__A] = $$// ics[$$__A];
          
          // fix initial conditions - real 2pfs
          this->populate_twopf_ic(x, twopf_re_k1_start, kmode_1, *times.begin(), ics, false);
          this->populate_twopf_ic(x, twopf_re_k2_start, kmode_2, *times.begin(), ics, false);
          this->populate_twopf_ic(x, twopf_re_k3_start, kmode_3, *times.begin(), ics, false);
          
          // fix initial conditions - imaginary 2pfs
          this->populate_twopf_ic(x, twopf_im_k1_start, kmode_1, *times.begin(), ics, true);
          this->populate_twopf_ic(x, twopf_im_k2_start, kmode_2, *times.begin(), ics, true);
          this->populate_twopf_ic(x, twopf_im_k3_start, kmode_3, *times.begin(), ics, true);
          
          // fix initial conditions - threepf
          this->populate_threepf_ic(x, threepf_start, kmode_1, kmode_2, kmode_3, *times.begin(), ics);
          
          integrate_times( $$__MAKE_PERT_STEPPER{threepf_state}, system, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);
        }


      template <typename number>
      void $$__MODEL<number>::resize_threepf_history(std::vector< std::vector< std::vector<number> > >& threepf_history, const std::vector<double>& times,
        const std::vector<double>& ks)
        {
          // the index convention for the threepf history is:
          //   first index  - time
          //   second index - component number
          //   third index  - k mode
          //                  this is an index into the lattice ks^3, remembering that we insist on the k-modes being ordered
          //                  in that case, there are N(N+1)(N+2)/6 distinct k-modes
          const auto threepf_components_size = 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS;
          const auto threepf_kmodes_size     = ks.size() * (ks.size() + 1) * (ks.size() + 2) / 6;

          threepf_history.resize(times.size());

          for(int i = 0; i < times.size(); i++)
            {
              threepf_history[i].resize(threepf_components_size);
              
              for(int j = 0; j < threepf_components_size; j++)
                {
                  threepf_history[i][j].resize(threepf_kmodes_size);
                }
            }
        }
    
    
      template <typename number>
      void $$__MODEL<number>::populate_threepf_ic(threepf_state& x, unsigned int start, double kmode_1, double kmode_2, double kmode_3,
                                                  double Ninit, const std::vector<number>& ics)
        {
          assert(x.size() >= start);
          assert(x.size() >= start + 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS);
          
          for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
            {
              for(int j = 0; j < 2*$$__NUMBER_FIELDS; j++)
                {
                  for(int k = 0; k < 2*$$__NUMBER_FIELDS; k++)
                    {
                      x[start + (2*$$__NUMBER_FIELDS * 2*$$__NUMBER_FIELDS)*i + (2*$$__NUMBER_FIELDS)*j + k] =
                        make_threepf_ic(i, j, k, kmode_1, kmode_2, kmode_3, Ninit, ics);
                    }
                }
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

          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k3;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k3;

          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k2;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k2;

          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __A_k1;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __B_k1;
          std::array< std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __C_k1;

          __A_k3[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_1, __kmode_2, __kmode_3, __ainit, __Hsq, __eps};
          __B_k3[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_1, __kmode_2, __kmode_3, __ainit, __Hsq, __eps};
          __C_k3[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_1, __kmode_2, __kmode_3, __ainit, __Hsq, __eps};

          __A_k2[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_1, __kmode_3, __kmode_2, __ainit, __Hsq, __eps};
          __B_k2[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_1, __kmode_3, __kmode_2, __ainit, __Hsq, __eps};
          __C_k2[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_1, __kmode_3, __kmode_2, __ainit, __Hsq, __eps};

          __A_k1[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__kmode_2, __kmode_3, __kmode_1, __ainit, __Hsq, __eps};
          __B_k1[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__kmode_2, __kmode_3, __kmode_1, __ainit, __Hsq, __eps};
          __C_k1[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__kmode_2, __kmode_3, __kmode_1, __ainit, __Hsq, __eps};

          unsigned int total_fields  = (IS_FIELD(__i)    ? 1 : 0) + (IS_FIELD(__j)    ? 1 : 0) + (IS_FIELD(__k)    ? 1 : 0);
          unsigned int total_momenta = (IS_MOMENTUM(__i) ? 1 : 0) + (IS_MOMENTUM(__j) ? 1 : 0) + (IS_MOMENTUM(__k) ? 1 : 0);

          assert(total_fields + total_momenta == 3);

          switch(total_fields)
            {
              case 3:   // field-field-field correlation function
                {
                  // prefactor here is dimension 5
                  auto __prefactor = (__kmode_1*__kmode_1) * (__kmode_2*__kmode_2) * (__kmode_3*__kmode_3) / (__kt * __ainit*__ainit*__ainit*__ainit);

                  // these components are dimension 3, so suppress by two powers of Mp
                  __tpf  = (__j == __k ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (__i == __k ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (__i == __j ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                  // these components are dimension 1
                  __tpf -= (__C_k3[__i][__j][__k] + __C_k3[__j][__i][__k])*__kmode_1*__kmode_2/2.0;
                  __tpf -= (__C_k2[__i][__k][__j] + __C_k2[__i][__k][__j])*__kmode_1*__kmode_3/2.0;
                  __tpf -= (__C_k1[__j][__k][__i] + __C_k1[__k][__j][__i])*__kmode_2*__kmode_3/2.0;

                  __tpf *= __prefactor / __kprod3;
                  break;
                }

              case 2:   // field-field-momentum correlation function
                {
                  auto __momentum_k = (IS_MOMENTUM(__i) ? __kmode_1 : 0.0) + (IS_MOMENTUM(__j) ? __kmode_2 : 0.0) + (IS_MOMENTUM(__k) ? __kmode_3 : 0.0);

                  // note the leading + sign, switched from written notes, from d/dN = d/(H dt) = d/(aH d\tau) = -\tau d/d\tau
                  // this prefactor has dimension 2
                  auto __prefactor_1 = __momentum_k*__momentum_k*(__kt-__momentum_k) / (__kt * __ainit*__ainit*__ainit*__ainit);
                  
                  // these components are dimension 6, so suppress by two powers of Mp
                  auto __tpf_1  = (__j == __k ? __fields[MOMENTUM(__i)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k2dotk3 / (2.0*__Mp*__Mp);
                       __tpf_1 += (__i == __k ? __fields[MOMENTUM(__j)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k1dotk3 / (2.0*__Mp*__Mp);
                       __tpf_1 += (__i == __j ? __fields[MOMENTUM(__k)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k1dotk2 / (2.0*__Mp*__Mp);
                  
                  // these components are dimension 4
                       __tpf_1 -= (__C_k3[__i][__j][__k] + __C_k3[__j][__i][__k])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*__kmode_3;
                       __tpf_1 -= (__C_k2[__i][__k][__j] + __C_k2[__k][__i][__j])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*__kmode_2;
                       __tpf_1 -= (__C_k1[__j][__k][__i] + __C_k1[__k][__j][__i])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*__kmode_1;
                  
                  __tpf = __prefactor_1 * __tpf_1;
                  
                  // this prefactor has dimension 3; the leading minus sign is again switched
                  auto __prefactor_2 = __momentum_k*__kmode_1*__kmode_2*__kmode_3 / (__kt * __ainit*__ainit*__ainit*__ainit);
                  
                  // these components are dimension 5, so suppress by two powers of Mp
                  auto __tpf_2  = (__j == __k ? __fields[MOMENTUM(__i)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                       __tpf_2 += (__i == __k ? __fields[MOMENTUM(__j)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                       __tpf_2 += (__i == __j ? __fields[MOMENTUM(__k)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);
                  
                  // these componennts are dimension 3
                       __tpf_2 += (__C_k3[__i][__j][__k] + __C_k3[__j][__i][__k])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*(1.0+__kmode_3/__kt) / 2.0;
                       __tpf_2 += (__C_k2[__i][__k][__j] + __C_k2[__k][__i][__j])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*(1.0+__kmode_2/__kt) / 2.0;
                       __tpf_2 += (__C_k1[__j][__i][__k] + __C_k1[__k][__j][__i])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*(1.0+__kmode_1/__kt) / 2.0;
                  
                  // these components are dimension 3
                       __tpf_2 += (__B_k1[__j][__k][__i] + __B_k1[__k][__j][__i])*__kmode_1*__kmode_1*__kmode_2*__kmode_3;
                       __tpf_2 += (__B_k2[__i][__k][__j] + __B_k2[__k][__i][__j])*__kmode_2*__kmode_2*__kmode_1*__kmode_3;
                       __tpf_2 += (__B_k3[__i][__j][__k] + __B_k3[__j][__i][__k])*__kmode_3*__kmode_3*__kmode_1*__kmode_2;
                  
                  __tpf += __prefactor_2 * __tpf_2;
                  
                  __tpf *= (1.0 / __kprod3);
                  break;
                }

              case 1:   // field-momentum-momentum correlation function
                {
                  auto __field_k = (IS_FIELD(__i) ? __kmode_1 : 0.0) + (IS_FIELD(__j) ? __kmode_2 : 0.0) + (IS_FIELD(__k) ? __kmode_3 : 0.0);

                  // this prefactor has dimension 3
                  auto __prefactor = - (__kmode_1*__kmode_1 * __kmode_2*__kmode_2 * __kmode_3*__kmode_3) / (__kt * __Hsq * __ainit*__ainit*__ainit*__ainit*__ainit*__ainit);

                  // this term (really another part of the prefactor) has dimension 2)
                  auto __mom_factor = __kmode_1*__kmode_2*__kmode_3 / __field_k;

                  // these components have dimension 3, so suppress by two powers of Mp
                  __tpf  = (__j == __k ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (__i == __k ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                  __tpf += (__i == __j ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);
                
                  // these components have dimension 1
                  __tpf -= (__C_k3[__i][__j][__k] + __C_k3[__j][__i][__k]) * __kmode_1*__kmode_2;
                  __tpf -= (__C_k2[__i][__k][__j] + __C_k2[__k][__i][__j]) * __kmode_1*__kmode_3;
                  __tpf -= (__C_k1[__j][__k][__i] + __C_k1[__k][__j][__i]) * __kmode_2*__kmode_3;
                
                  __tpf *= __prefactor * __mom_factor / __kprod3;
                  break;
                }

              case 0:   // momentum-momentum-momentum correlation function
                {
                  // prefactor is dimension 3; note the leading - sign, switched from written notes, from d/dN = d/(H dt) = d/(aH d\tau) = -\tau d/d\tau
                  auto __prefactor = -(__kmode_1*__kmode_1) * (__kmode_2*__kmode_2) * (__kmode_3*__kmode_3) / (__kt * __Hsq * __ainit*__ainit*__ainit*__ainit*__ainit*__ainit);

                  // these components are dimension 5, so suppress by two powers of Mp
                  __tpf  = (__j == __k ? __fields[MOMENTUM(__i)] : 0.0) * (-__Ksq*__k2dotk3 / (__Mp*__Mp));
                  __tpf += (__i == __k ? __fields[MOMENTUM(__j)] : 0.0) * (-__Ksq*__k1dotk3 / (__Mp*__Mp));
                  __tpf += (__i == __j ? __fields[MOMENTUM(__k)] : 0.0) * (-__Ksq*__k1dotk2 / (__Mp*__Mp));

                  // these components are dimension 2
                  __tpf += (__C_k3[__i][__j][__k] + __C_k3[__j][__i][__k])*__kmode_1*__kmode_1*__kmode_2*__kmode_2/2.0;
                  __tpf += (__C_k2[__i][__k][__j] + __C_k2[__k][__i][__j])*__kmode_1*__kmode_1*__kmode_3*__kmode_3/2.0;
                  __tpf += (__C_k1[__j][__k][__i] + __C_k2[__k][__j][__i])*__kmode_2*__kmode_2*__kmode_3*__kmode_3/2.0;

                  // these components are dimension 2
                  __tpf += (__B_k1[__j][__k][__i] + __B_k1[__k][__j][__i])*__kmode_1*__kmode_1*__kmode_2*__kmode_3;
                  __tpf += (__B_k2[__i][__k][__j] + __B_k2[__k][__i][__j])*__kmode_2*__kmode_2*__kmode_1*__kmode_3;
                  __tpf += (__B_k3[__i][__j][__k] + __B_k3[__j][__i][__k])*__kmode_3*__kmode_3*__kmode_1*__kmode_2;

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

          const unsigned int size_background   = 2*$$__NUMBER_FIELDS;
          const unsigned int size_twopf        = (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS);

          const unsigned int start_background  = 0;
          const unsigned int start_twopf       = start_background + size_background;

          // allocate storage for state
          std::vector<number> bg_x (size_background);
          std::vector<number> tpf_x(size_twopf);

          // first, background
          for(int i = 0; i < size_background; i++)
            {
              bg_x[i] = x[i];
            }
          this->background_history.push_back(bg_x);

          // then, 2pf
          for(int i = 0; i < size_twopf; i++)
            {
              tpf_x[i] = x[start_twopf + i];
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
          
          const auto __threepf_$$__A_$$__B_$$__C = $$// __x[__start_threepf     + (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*$$__A)+(2*$$__NUMBER_FIELDS*$$__B)+$$__C];

          #undef __background
          #undef __dtwopf_re_k1
          #undef __dtwopf_im_k1
          #undef __dtwopf_re_k2
          #undef __dtwopf_im_k2
          #undef __dtwopf_re_k3
          #undef __dtwopf_im_k3
          #undef __dthreepf
          #define __background(a)      __dxdt[__start_background  + a]
          #define __dtwopf_re_k1(a,b)  __dxdt[__start_twopf_re_k1 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_im_k1(a,b)  __dxdt[__start_twopf_im_k1 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_re_k2(a,b)  __dxdt[__start_twopf_re_k2 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_im_k2(a,b)  __dxdt[__start_twopf_im_k2 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_re_k3(a,b)  __dxdt[__start_twopf_re_k3 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dtwopf_im_k3(a,b)  __dxdt[__start_twopf_im_k3 + (2*$$__NUMBER_FIELDS*a) + b]
          #define __dthreepf(a,b,c)    __dxdt[__start_threepf     + (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)*a + (2*$$__NUMBER_FIELDS)*b + c]

          // evolve the background
          __background($$__A) = $$__U1_PREDEF[A]{__Hsq,__eps};

          // set up components of the u2 tensor for k1, k2, k3
          const auto __u2_k1_$$__A_$$__B = $$__U2_PREDEF[AB]{__k1, __a, __Hsq, __eps};
          const auto __u2_k2_$$__A_$$__B = $$__U2_PREDEF[AB]{__k2, __a, __Hsq, __eps};
          const auto __u2_k3_$$__A_$$__B = $$__U2_PREDEF[AB]{__k3, __a, __Hsq, __eps};

          // set up components of the u3 tensor
          const auto __u3_k1_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};
          const auto __u3_k2_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k2, __k1, __k3, __a, __Hsq, __eps};
          const auto __u3_k3_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k3, __k1, __k2, __a, __Hsq, __eps};

          const auto __u3_k1s_$$__A_$$__B_$$__C = $$__U3_PREDEF[ACB]{__k1, __k3, __k2, __a, __Hsq, __eps};
          const auto __u3_k2s_$$__A_$$__B_$$__C = $$__U3_PREDEF[ACB]{__k2, __k3, __k1, __a, __Hsq, __eps};
          const auto __u3_k3s_$$__A_$$__B_$$__C = $$__U3_PREDEF[ACB]{__k3, __k2, __k1, __a, __Hsq, __eps};

          // check that the u3 tensor is sufficiently symmetric
          assert(fabs(__u3_k1_$$__A_$$__B_$$__C - __u3_k1s_$$__A_$$__B_$$__C) < 1E-10) $$// ;
          assert(fabs(__u3_k2_$$__A_$$__B_$$__C - __u3_k2s_$$__A_$$__B_$$__C) < 1E-10) $$// ;
          assert(fabs(__u3_k3_$$__A_$$__B_$$__C - __u3_k3s_$$__A_$$__B_$$__C) < 1E-10) $$// ;

          // evolve the real and imaginary components of the 2pf
          // for the imaginary parts, index placement does matter
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
          __dthreepf($$__A, $$__B, $$__C) = 0 $$// + $$__U2_NAME[AM]{__u2_k1}*__threepf_$$__M_$$__B_$$__C
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[AMN]{__u3_k1}*__twopf_re_k2_$$__M_$$__B*__twopf_re_k3_$$__N_$$__C
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[AMN]{__u3_k1}*__twopf_im_k2_$$__M_$$__B*__twopf_im_k3_$$__N_$$__C
          
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U2_NAME[BM]{__u2_k2}*__threepf_$$__A_$$__M_$$__C
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[BMN]{__u3_k2}*__twopf_re_k1_$$__A_$$__M*__twopf_re_k3_$$__N_$$__C
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[BMN]{__u3_k2}*__twopf_im_k1_$$__A_$$__M*__twopf_im_k3_$$__N_$$__C
          
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U2_NAME[CM]{__u2_k3}*__threepf_$$__A_$$__B_$$__M
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[CMN]{__u3_k3}*__twopf_re_k1_$$__A_$$__M*__twopf_re_k2_$$__B_$$__N
          __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[CMN]{__u3_k3}*__twopf_im_k1_$$__A_$$__M*__twopf_im_k2_$$__B_$$__N
        }


      // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


      template <typename number>
      void $$__MODEL_threepf_observer<number>::operator()(const twopf_state& x, double t)
        {
          this->slices.push_back(t);

          const unsigned int size_background   = 2*$$__NUMBER_FIELDS;
          const unsigned int size_twopf        = (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS);
          const unsigned int size_threepf      = (2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS);

          const unsigned int start_background  = 0;
          const unsigned int start_twopf_re_k1 = start_background  + size_background;
          const unsigned int start_twopf_im_k1 = start_twopf_re_k1 + size_twopf;
          const unsigned int start_twopf_re_k2 = start_twopf_im_k1 + size_twopf;
          const unsigned int start_twopf_im_k2 = start_twopf_re_k2 + size_twopf;
          const unsigned int start_twopf_re_k3 = start_twopf_im_k2 + size_twopf;
          const unsigned int start_twopf_im_k3 = start_twopf_re_k3 + size_twopf;
          const unsigned int start_threepf     = start_twopf_im_k3 + size_twopf;
          
          const unsigned int start_twopf_re    = start_twopf_re_k1;     // the twopf we want to store is the one associated with k1
          const unsigned int start_twopf_im    = start_twopf_im_k1;

          // allocate storage for state
          std::vector<number> bg_x      (size_background);
          std::vector<number> twopf_re_x(size_twopf);
          std::vector<number> twopf_im_x(size_twopf);
          std::vector<number> threepf_x (size_threepf);

          // first, background
          for(int i = 0; i < size_background; i++)
            {
              bg_x[i] = x[i];
            }
          this->background_history.push_back(bg_x);

          // then, real part of the 2pf
          for(int i = 0; i < size_twopf; i++)
            {
              twopf_re_x[i] = x[start_twopf_re + i];
            }
          this->twopf_re_history.push_back(twopf_re_x);
        
          // then, the imaginary part of the 2pf
          for(int i = 0; i < size_twopf; i++)
            {
              twopf_im_x[i] = x[start_twopf_im + i];
            }
          this->twopf_im_history.push_back(twopf_im_x);
          
          // finally, 3pf
          for(int i = 0; i < size_threepf; i++)
            {
              threepf_x[i] = x[start_threepf + i];
            }
          this->threepf_history.push_back(threepf_x);
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

          std::vector< std::vector<number> > __u2($$__NUMBER_FIELDS);

          for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
            {
              __u2[__i].resize($$__NUMBER_FIELDS);
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

          std::vector< std::vector< std::vector<number> > > __u3($$__NUMBER_FIELDS);

          for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
            {
              __u3[__i].resize($$__NUMBER_FIELDS);
              for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
                {
                  __u3[__i][__j].resize($$__NUMBER_FIELDS);
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
          
          __B[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};

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
          
          __C[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};

          return(__C);
        }


  }   // namespace transport


#endif  // $$__GUARD
