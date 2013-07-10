//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY CppTransport 0.02
//
// 'dq.h' generated from 'dq.model'
// processed on 13:54:36 on 10 07 2013

#ifndef __CPP_TRANSPORT_dquad_H_   // avoid multiple inclusion
#define __CPP_TRANSPORT_dquad_H_

#include <assert.h>
#include <math.h>

#include "boost/numeric/odeint.hpp"

#include "transport/transport.h"

namespace transport
  {
      static std::vector<std::string> dquad_field_names = { "phi", "chi" };
      static std::vector<std::string> dquad_latex_names = { "\\phi", "\\chi" };
      static std::vector<std::string> dquad_param_names = { "M_phi", "M_chi" };
      static std::vector<std::string> dquad_platx_names = { "M_{\\phi}", "M_{\\chi}" };
      static std::vector<std::string> dquad_state_names = { "phi", "chi", "__dphi", "__dchi" };

      // set up a state type for 2pf integration
      typedef std::vector<double> twopf_state;

      template <typename number>
      class dquad : public canonical_model<number>
        {
          public:
              dquad(number Mp, const std::vector<number>& ps);
              ~dquad();

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

          protected:
              void
                fix_initial_conditions(const std::vector<number>& __ics, std::vector<number>& __rics);
              void
                write_initial_conditions(const std::vector<number>& rics, std::ostream& stream,
                  double abs_err, double rel_err, double step_size);
              double
                make_tpf_ic(unsigned int i, unsigned int j, double k, double __Nstar, const std::vector<number>& __fields);
              void
                rescale_ks(const std::vector<double>& __ks, std::vector<double>& __real_ks,
                  double __Nstar, const std::vector<number>& __fields);
        };


      // integration - background functor
      template <typename number>
      class dquad_background_functor
        {
          public:
              dquad_background_functor(const std::vector<number>& p, number Mp) : parameters(p), M_Planck(Mp) {}
              void operator()(const std::vector<number>& __x, std::vector<number>& __dxdt, double __t);

          private:
              const number              M_Planck;
              const std::vector<number> parameters;
        };


      // integration - observer object for background only
      template <typename number>
      class dquad_background_observer
        {
          public:
              dquad_background_observer(std::vector<double>& s, std::vector< std::vector<number> >& h) : slices(s), history(h) {}
              void operator()(const std::vector<number>& x, double t);

          private:
              std::vector<double>&                slices;
              std::vector< std::vector<number> >& history;
        };


      // integration - 2pf functor
      template <typename number>
      class dquad_twopf_functor
        {
          public:
              dquad_twopf_functor(const std::vector<number>& p, const number Mp, double k)
                : parameters(p), M_Planck(Mp), k_mode(k) {}

              void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
              const number						   M_Planck;
              const std::vector<number>& parameters;
              double                     k_mode;
        };


      // integration - observer object for 2pf
      template <typename number>
      class dquad_twopf_observer
        {
          public:
              dquad_twopf_observer(std::vector<double>& s,
                std::vector< std::vector<number> >& bh,
                std::vector< std::vector<number> >& tpfh)
              : slices(s), background_history(bh), twopf_history(tpfh) {}

              void operator()(const twopf_state& x, double t);

          private:
              std::vector<double>&                slices;
              std::vector< std::vector<number> >& background_history;
              std::vector< std::vector<number> >& twopf_history;
        };


      // IMPLEMENTATION -- CLASS dquad
      // ==============


      template <typename number>
      dquad<number>::dquad(number Mp, const std::vector<number>& ps)
      : canonical_model<number>("Double Quadratic Inflation", "David Seery and others 2013", "please cite arXiv:XXXX.YYYY", Mp,
        2, 2,
        dquad_field_names, dquad_latex_names,
        dquad_param_names, dquad_platx_names, ps)
        {
          return;
        }


      template <typename number>
      dquad<number>::~dquad()
        {
          return;
        }


      template <typename number>
      number dquad<number>::V(std::vector<number> fields)
        {
          assert(fields.size() == 2);

          const auto M_phi = this->parameters[0];
          const auto M_chi = this->parameters[1];;
          const auto phi     = fields[0];
          const auto chi     = fields[1];;
          const auto __Mp             = this->M_Planck;

          number rval =  (phi*phi)*(M_phi*M_phi)/2.0+(chi*chi)*(M_chi*M_chi)/2.0;

          return(rval);
        }


      // Integrate the background - on the CPU


      template <typename number>
      transport::background<number> dquad<number>::background(const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> x = ics;
          this->fix_initial_conditions(ics, x);
          this->write_initial_conditions(x, std::cout, 1e-06, 1e-06, 0.01);

          // set up an observer which writes to this history vector
          // I'd prefer to encapsulate the history within the observer object, but for some reason
          // that doesn't seem to work (maybe related to the way odeint uses templates?)
          std::vector<double>                slices;
          std::vector< std::vector<number> > history;
          dquad_background_observer<number> obs(slices, history);

          // set up a functor to evolve this system
          dquad_background_functor<number>  system(this->parameters, this->M_Planck);

          integrate_times( make_dense_output< runge_kutta_dopri5< std::vector<number> > >(1e-06, 1e-06),
            system, x, times.begin(), times.end(), 0.01, obs);

          transport::background<number> backg(2*2, dquad_state_names, slices, history);

          return(backg);
        }


      // Integrate the 2pf - on the CPU, using OpenMP


      template <typename number>
      transport::twopf<number> dquad<number>::twopf(const std::vector<double>& ks, double Nstar,
        const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> real_ics = ics;
          this->fix_initial_conditions(ics, real_ics);
          this->write_initial_conditions(real_ics, std::cout, 1e-08, 1e-08, 0.0001);

          // set up vector of ks corresponding to honest comoving momenta
          std::vector<double> real_ks(ks.size());
          this->rescale_ks(ks, real_ks, Nstar, real_ics);

          // space for storing the solution
          std::vector<double>                               slices;
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_history;

          // ensure there is sufficient space for the solution
          // the index convention there is:
          //   first index  - time
          //   second index - component number
          //   third index  - k mode
          twopf_history.resize(times.size());
          for(int i = 0; i < times.size(); i++)
            {
              twopf_history[i].resize(2*2 * 22);
              for(int j = 0; j < 2*2 * 2*2; j++)
                {
                  twopf_history[i][j].resize(real_ks.size());
                }
            }

          for(int i = 0; i < ks.size(); i++)
            {
              // set up a functor to evolve this system
              dquad_twopf_functor<number> system(this->parameters, this->M_Planck, real_ks[i]);

              // set up a functor to observe the integration
              std::vector<double>                kmode_slices;
              std::vector< std::vector<number> > kmode_background_history;
              std::vector< std::vector<number> > kmode_twopf_history;

              // ensure there is enough space in twopf_history
              dquad_twopf_observer<number>   obs(kmode_slices, kmode_background_history, kmode_twopf_history);

              // set up a state vector
              twopf_state x;
              x.resize(2*2 + 2*2*2*2);

              // fix initial conditions - background
              x[0] =  real_ics[0];
              x[1] =  real_ics[1];
              x[2] =  real_ics[2];
              x[3] =  real_ics[3];

              // fix initial conditions - 2pf
              for(int j = 0; j < 2*2; j++)
                {
                  for(int k = 0; k < 2*2; k++)
                    {
                      x[2*2 + (2*2*j)+k] = make_tpf_ic(j, k, real_ks[i], Nstar, real_ics);
                    }
                }

              integrate_times( make_dense_output< runge_kutta_dopri5< twopf_state > >(1e-08, 1e-08),
                system, x, times.begin(), times.end(), 0.0001, obs);

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

          transport::twopf<number> tpf(2*2, dquad_state_names, ks, slices, background_history, twopf_history);

          return(tpf);
        }


      // Handle initial conditions


      template <typename number>
      void dquad<number>::fix_initial_conditions(const std::vector<number>& __ics, std::vector<number>& __rics)
        {
          if(__ics.size() == this->N_fields)  // initial conditions for momenta *were not* supplied -- need to compute them
            {
              // supply the missing initial conditions using a slow-roll approximation
              const auto M_phi = this->parameters[0];
              const auto M_chi = this->parameters[1];;
              const auto phi     = __ics[0];
              const auto chi     = __ics[1];;
              const auto __Mp             = this->M_Planck;

              __rics.push_back(-2.0*phi*(__Mp*__Mp)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*(M_phi*M_phi));
              __rics.push_back(-2.0*chi*(__Mp*__Mp)*(M_chi*M_chi)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi)));;
            }
          else if(__ics.size() == 2*this->N_fields)  // initial conditions for momenta *were* supplied
            {
              // need do nothing
            }
          else
            {
              std::cerr << __CPP_TRANSPORT_WRONG_ICS_A << __ics.size()
                << __CPP_TRANSPORT_WRONG_ICS_B << 2
                << __CPP_TRANSPORT_WRONG_ICS_C << 2*2 << ")" << std::endl;
              exit(EXIT_FAILURE);
            }
        }


      template <typename number>
      void dquad<number>::write_initial_conditions(const std::vector<number>& ics, std::ostream& stream,
        double abs_err, double rel_err, double step_size)
        {
          stream << __CPP_TRANSPORT_SOLVING_ICS_MESSAGE << std::endl;

          assert(ics.size() == 2*this->N_fields);

          for(int i = 0; i < this->N_fields; i++)
            {
              stream << "  " << this->field_names[i] << " = " << ics[i]
                << "; d(" << this->field_names[i] << ")/dN = " << ics[this->N_fields+i] << std::endl;
            }

          stream << __CPP_TRANSPORT_STEPPER_MESSAGE    << " '"  << "runge_kutta_dopri5"
            << "', " << __CPP_TRANSPORT_ABS_ERR   << " = " << abs_err
            << ", "  << __CPP_TRANSPORT_REL_ERR   << " = " << rel_err
            << ", "  << __CPP_TRANSPORT_STEP_SIZE << " = " << step_size << std::endl;

          stream << std::endl;
        }


      template <typename number>
      double dquad<number>::make_tpf_ic(unsigned int __i, unsigned int __j,
        double __k, double __Nstar, const std::vector<number>& __fields)
        {
          const auto M_phi  = this->parameters[0];
          const auto M_chi  = this->parameters[1];;
          const auto phi = __fields[0];
          const auto chi = __fields[1];
          const auto __dphi = __fields[2];
          const auto __dchi = __fields[3];;
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = -1.0/(__Mp*__Mp)/( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)*( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi));

          double __tpf = 0.0;

          if(__i < 2 && __j < 2)        // field-field correlation function
            {
              if(__i == __j)
                {
                  __tpf = (1.0/(2.0*__k)) * exp(2.0 * __Nstar);
                }
              else
                {
                  __tpf = 0.0;
                }
            }
          else if((__i < 2 && __j >= 2)   // field-momentum correlation function
                  || (__i >= 2 && __j < 2))
            {
              if(__i >= 2) __i -= 2;
              if(__j >= 2) __j -= 2;

              if(__i == __j)
                {
                  __tpf = -(1.0/(2.0*__k)) * exp(2.0 * __Nstar);
                }
              else
                {
                  __tpf = 0.0;
                }
            }
          else if(__i >= 2 && __j >= 2)   // momentum-momentum correlation function
            {
              if(__i == __j)
                {
                  __tpf = (__k/(2.0*__Hsq)) * exp(4.0 * __Nstar);
                }
              else
                {
                  __tpf = 0.0;
                }
            }
          else
            {
              assert(false);
            }

          return(__tpf);
        }


      template <typename number>
      void dquad<number>::rescale_ks(const std::vector<double>& __ks, std::vector<double>& __real_ks,
        double __Nstar, const std::vector<number>& __fields)
        {
          const auto M_phi  = this->parameters[0];
          const auto M_chi  = this->parameters[1];;
          const auto phi = __fields[0];
          const auto chi = __fields[1];
          const auto __dphi = __fields[2];
          const auto __dchi = __fields[3];;
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = -1.0/(__Mp*__Mp)/( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)*( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi));

          assert(__ks.size() == __real_ks.size());

          for(int __n = 0; __n < __ks.size(); __n++)
            {
              __real_ks[__n] = __ks[__n] * sqrt(__Hsq) * exp(__Nstar);
            }
        }

      // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


      template <typename number>
      void dquad_background_functor<number>::operator()(const std::vector<number>& __x, std::vector<number>& __dxdt, double __t)
        {
          const auto M_phi  = this->parameters[0];
          const auto M_chi  = this->parameters[1];;
          const auto phi = __x[0];
          const auto chi = __x[1];
          const auto __dphi = __x[2];
          const auto __dchi = __x[3];;
          const auto __Mp              = this->M_Planck;

          __dxdt[0]          = __dphi;
          __dxdt[1]          = __dchi;
          __dxdt[2]          =  phi*(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*(M_phi*M_phi)+__dphi*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0;
          __dxdt[3]          =  __dchi*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0+chi*(__Mp*__Mp)*(M_chi*M_chi)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi));;
        }


      // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


      template <typename number>
      void dquad_background_observer<number>::operator()(const std::vector<number>& x, double t)
        {
          this->slices.push_back(t);
          this->history.push_back(x);
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


      template <typename number>
      void dquad_twopf_functor<number>::operator()(const twopf_state& __x, twopf_state& __dxdt, double __t)
        {
          const auto M_phi  = this->parameters[0];
          const auto M_chi  = this->parameters[1];;
          const auto phi = __x[0];
          const auto chi = __x[1];
          const auto __dphi = __x[2];
          const auto __dchi = __x[3];;
          const auto __Mp              = this->M_Planck;
          const auto __k               = this->k_mode;
          const auto __a               = exp(__t);

          const unsigned int start_background = 0;
          const unsigned int start_twopf      = 2*2;

          const auto __tpf_0_0 =  __x[start_twopf+(2*2*0)+0];
          const auto __tpf_1_0 =  __x[start_twopf+(2*2*1)+0];
          const auto __tpf_2_0 =  __x[start_twopf+(2*2*2)+0];
          const auto __tpf_3_0 =  __x[start_twopf+(2*2*3)+0];
          const auto __tpf_0_1 =  __x[start_twopf+(2*2*0)+1];
          const auto __tpf_1_1 =  __x[start_twopf+(2*2*1)+1];
          const auto __tpf_2_1 =  __x[start_twopf+(2*2*2)+1];
          const auto __tpf_3_1 =  __x[start_twopf+(2*2*3)+1];
          const auto __tpf_0_2 =  __x[start_twopf+(2*2*0)+2];
          const auto __tpf_1_2 =  __x[start_twopf+(2*2*1)+2];
          const auto __tpf_2_2 =  __x[start_twopf+(2*2*2)+2];
          const auto __tpf_3_2 =  __x[start_twopf+(2*2*3)+2];
          const auto __tpf_0_3 =  __x[start_twopf+(2*2*0)+3];
          const auto __tpf_1_3 =  __x[start_twopf+(2*2*1)+3];
          const auto __tpf_2_3 =  __x[start_twopf+(2*2*2)+3];
          const auto __tpf_3_3 =  __x[start_twopf+(2*2*3)+3];

          #define __background(a)   __dxdt[start_background + a]
          #define __dtwopf(a,b)     __dxdt[start_twopf + (2*2*a) + b]

          // evolve the background
          __background(0) = __dphi;
          __background(1) = __dchi;
          __background(2) =  phi*(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*(M_phi*M_phi)+__dphi*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0;
          __background(3) =  __dchi*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0+chi*(__Mp*__Mp)*(M_chi*M_chi)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi));;

          // set up components of the u2 tensor
          const auto __u2_0_0 = 0.0;
          const auto __u2_1_0 = 0.0;
          const auto __u2_2_0 =  2.0*phi*__dphi*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*(M_phi*M_phi)+(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*(M_phi*M_phi)+(__dphi*__dphi)/(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0+(__k*__k)/(__a*__a)*(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi));
          const auto __u2_3_0 =  ( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*( chi*__dphi*(M_chi*M_chi)+phi*__dchi*(M_phi*M_phi))+__dchi*__dphi/(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0;
          const auto __u2_0_1 = 0.0;
          const auto __u2_1_1 = 0.0;
          const auto __u2_2_1 =  ( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))*( chi*__dphi*(M_chi*M_chi)+phi*__dchi*(M_phi*M_phi))+__dchi*__dphi/(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0;
          const auto __u2_3_1 =  (__dchi*__dchi)/(__Mp*__Mp)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/2.0+(__Mp*__Mp)*(M_chi*M_chi)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))+(__k*__k)*(__Mp*__Mp)/(__a*__a)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi))+2.0*chi*__dchi*(M_chi*M_chi)*( ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)-6.0)/( (phi*phi)*(M_phi*M_phi)+(chi*chi)*(M_chi*M_chi));
          const auto __u2_0_2 = 1.0;
          const auto __u2_1_2 = 0.0;
          const auto __u2_2_2 =  ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)/2.0-3.0;
          const auto __u2_3_2 = 0.0;
          const auto __u2_0_3 = 0.0;
          const auto __u2_1_3 = 1.0;
          const auto __u2_2_3 = 0.0;
          const auto __u2_3_3 =  ( (__dphi*__dphi)+(__dchi*__dchi))/(__Mp*__Mp)/2.0-3.0;;

          // evolve the 2pf
          __dtwopf(0,0) = 0  + __u2_0_0*__tpf_0_0
 + __u2_0_1*__tpf_1_0
 + __u2_0_2*__tpf_2_0
 + __u2_0_3*__tpf_3_0;
          __dtwopf(1,0) = 0  + __u2_1_0*__tpf_0_0
 + __u2_1_1*__tpf_1_0
 + __u2_1_2*__tpf_2_0
 + __u2_1_3*__tpf_3_0;
          __dtwopf(2,0) = 0  + __u2_2_0*__tpf_0_0
 + __u2_2_1*__tpf_1_0
 + __u2_2_2*__tpf_2_0
 + __u2_2_3*__tpf_3_0;
          __dtwopf(3,0) = 0  + __u2_3_0*__tpf_0_0
 + __u2_3_1*__tpf_1_0
 + __u2_3_2*__tpf_2_0
 + __u2_3_3*__tpf_3_0;
          __dtwopf(0,1) = 0  + __u2_0_0*__tpf_0_1
 + __u2_0_1*__tpf_1_1
 + __u2_0_2*__tpf_2_1
 + __u2_0_3*__tpf_3_1;
          __dtwopf(1,1) = 0  + __u2_1_0*__tpf_0_1
 + __u2_1_1*__tpf_1_1
 + __u2_1_2*__tpf_2_1
 + __u2_1_3*__tpf_3_1;
          __dtwopf(2,1) = 0  + __u2_2_0*__tpf_0_1
 + __u2_2_1*__tpf_1_1
 + __u2_2_2*__tpf_2_1
 + __u2_2_3*__tpf_3_1;
          __dtwopf(3,1) = 0  + __u2_3_0*__tpf_0_1
 + __u2_3_1*__tpf_1_1
 + __u2_3_2*__tpf_2_1
 + __u2_3_3*__tpf_3_1;
          __dtwopf(0,2) = 0  + __u2_0_0*__tpf_0_2
 + __u2_0_1*__tpf_1_2
 + __u2_0_2*__tpf_2_2
 + __u2_0_3*__tpf_3_2;
          __dtwopf(1,2) = 0  + __u2_1_0*__tpf_0_2
 + __u2_1_1*__tpf_1_2
 + __u2_1_2*__tpf_2_2
 + __u2_1_3*__tpf_3_2;
          __dtwopf(2,2) = 0  + __u2_2_0*__tpf_0_2
 + __u2_2_1*__tpf_1_2
 + __u2_2_2*__tpf_2_2
 + __u2_2_3*__tpf_3_2;
          __dtwopf(3,2) = 0  + __u2_3_0*__tpf_0_2
 + __u2_3_1*__tpf_1_2
 + __u2_3_2*__tpf_2_2
 + __u2_3_3*__tpf_3_2;
          __dtwopf(0,3) = 0  + __u2_0_0*__tpf_0_3
 + __u2_0_1*__tpf_1_3
 + __u2_0_2*__tpf_2_3
 + __u2_0_3*__tpf_3_3;
          __dtwopf(1,3) = 0  + __u2_1_0*__tpf_0_3
 + __u2_1_1*__tpf_1_3
 + __u2_1_2*__tpf_2_3
 + __u2_1_3*__tpf_3_3;
          __dtwopf(2,3) = 0  + __u2_2_0*__tpf_0_3
 + __u2_2_1*__tpf_1_3
 + __u2_2_2*__tpf_2_3
 + __u2_2_3*__tpf_3_3;
          __dtwopf(3,3) = 0  + __u2_3_0*__tpf_0_3
 + __u2_3_1*__tpf_1_3
 + __u2_3_2*__tpf_2_3
 + __u2_3_3*__tpf_3_3;
          __dtwopf(0,0) += 0  + __u2_0_0*__tpf_0_0
 + __u2_0_1*__tpf_0_1
 + __u2_0_2*__tpf_0_2
 + __u2_0_3*__tpf_0_3;
          __dtwopf(1,0) += 0  + __u2_0_0*__tpf_1_0
 + __u2_0_1*__tpf_1_1
 + __u2_0_2*__tpf_1_2
 + __u2_0_3*__tpf_1_3;
          __dtwopf(2,0) += 0  + __u2_0_0*__tpf_2_0
 + __u2_0_1*__tpf_2_1
 + __u2_0_2*__tpf_2_2
 + __u2_0_3*__tpf_2_3;
          __dtwopf(3,0) += 0  + __u2_0_0*__tpf_3_0
 + __u2_0_1*__tpf_3_1
 + __u2_0_2*__tpf_3_2
 + __u2_0_3*__tpf_3_3;
          __dtwopf(0,1) += 0  + __u2_1_0*__tpf_0_0
 + __u2_1_1*__tpf_0_1
 + __u2_1_2*__tpf_0_2
 + __u2_1_3*__tpf_0_3;
          __dtwopf(1,1) += 0  + __u2_1_0*__tpf_1_0
 + __u2_1_1*__tpf_1_1
 + __u2_1_2*__tpf_1_2
 + __u2_1_3*__tpf_1_3;
          __dtwopf(2,1) += 0  + __u2_1_0*__tpf_2_0
 + __u2_1_1*__tpf_2_1
 + __u2_1_2*__tpf_2_2
 + __u2_1_3*__tpf_2_3;
          __dtwopf(3,1) += 0  + __u2_1_0*__tpf_3_0
 + __u2_1_1*__tpf_3_1
 + __u2_1_2*__tpf_3_2
 + __u2_1_3*__tpf_3_3;
          __dtwopf(0,2) += 0  + __u2_2_0*__tpf_0_0
 + __u2_2_1*__tpf_0_1
 + __u2_2_2*__tpf_0_2
 + __u2_2_3*__tpf_0_3;
          __dtwopf(1,2) += 0  + __u2_2_0*__tpf_1_0
 + __u2_2_1*__tpf_1_1
 + __u2_2_2*__tpf_1_2
 + __u2_2_3*__tpf_1_3;
          __dtwopf(2,2) += 0  + __u2_2_0*__tpf_2_0
 + __u2_2_1*__tpf_2_1
 + __u2_2_2*__tpf_2_2
 + __u2_2_3*__tpf_2_3;
          __dtwopf(3,2) += 0  + __u2_2_0*__tpf_3_0
 + __u2_2_1*__tpf_3_1
 + __u2_2_2*__tpf_3_2
 + __u2_2_3*__tpf_3_3;
          __dtwopf(0,3) += 0  + __u2_3_0*__tpf_0_0
 + __u2_3_1*__tpf_0_1
 + __u2_3_2*__tpf_0_2
 + __u2_3_3*__tpf_0_3;
          __dtwopf(1,3) += 0  + __u2_3_0*__tpf_1_0
 + __u2_3_1*__tpf_1_1
 + __u2_3_2*__tpf_1_2
 + __u2_3_3*__tpf_1_3;
          __dtwopf(2,3) += 0  + __u2_3_0*__tpf_2_0
 + __u2_3_1*__tpf_2_1
 + __u2_3_2*__tpf_2_2
 + __u2_3_3*__tpf_2_3;
          __dtwopf(3,3) += 0  + __u2_3_0*__tpf_3_0
 + __u2_3_1*__tpf_3_1
 + __u2_3_2*__tpf_3_2
 + __u2_3_3*__tpf_3_3;
        }


      // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


      template <typename number>
      void dquad_twopf_observer<number>::operator()(const twopf_state& x, double t)
        {
          this->slices.push_back(t);

          // allocate storage for state
          std::vector<number> bg_x (2*2);
          std::vector<number> tpf_x(2*2 * 2*2);

          // first, background
          for(int i = 0; i < 2*2; i++)
            {
              bg_x[i] = x[i];
            }
          this->background_history.push_back(bg_x);

          // then, 2pf - we do this for every k number, and we're entitled to assume
          // that there is enough space in this->twopf_history
          for(int i = 0; i < 2*2 * 2*2; i++)
            {
              tpf_x[i] = x[2*2 + i];
            }
          this->twopf_history.push_back(tpf_x);
        }


  }   // namespace transport


#endif  // __CPP_TRANSPORT_dquad_H_

