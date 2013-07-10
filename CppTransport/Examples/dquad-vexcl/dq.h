//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY CppTransport 0.02
//
// 'dq.h' generated from 'dq.model'
// processed on 13:54:21 on 10 07 2013

#ifndef __CPP_TRANSPORT_dquad_H_   // avoid multiple inclusion
#define __CPP_TRANSPORT_dquad_H_

#include <math.h>

#include "vexcl/vexcl.hpp"

#include "boost/numeric/odeint.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl_resize.hpp"
#include "boost/numeric/odeint/external/vexcl/vexcl_norm_inf.hpp"

#include "transport/transport.h"

namespace transport
  {
      static std::vector<std::string> dquad_field_names = { "phi", "chi" };
      static std::vector<std::string> dquad_latex_names = { "\\phi", "\\chi" };
      static std::vector<std::string> dquad_param_names = { "M_phi", "M_chi" };
      static std::vector<std::string> dquad_platx_names = { "M_{\\phi}", "M_{\\chi}" };
      static std::vector<std::string> dquad_state_names = { "phi", "chi", "__dphi", "__dchi" };

      // set up a state type for GPU integration
      typedef vex::multivector<double, 2*2 + (2*2)*(2*2)> twopf_state;

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

            // Integrate background and 2-point function on the GPU
            transport::twopf<number>
              twopf(vex::Context& ctx, const std::vector<double>& ks, double Nstar,
                const std::vector<number>& ics, const std::vector<double>& times);

          protected:
            void
              fix_initial_conditions(const std::vector<number>& __ics, std::vector<number>& __rics);
            void
              write_initial_conditions(const std::vector<number>& rics, std::ostream& stream,
              double abs_err, double rel_err, double step_size);
            void
              make_tpf_ic(unsigned int i, unsigned int j, const std::vector<double>& __ks, double __Nstar,
                const std::vector<number>& __fields, std::vector<double>& __tpf);
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
            dquad_twopf_functor(const std::vector<number>& p, const number Mp,
              const vex::vector<double>& ks,
              vex::multivector<double, (2*2)*(2*2)>& u2)
              : parameters(p), M_Planck(Mp), k_list(ks), u2_tensor(u2) {}
            void operator()(const twopf_state& __x, twopf_state& __dxdt, double __t);

          private:
            const number						                                               M_Planck;
            const std::vector<number>&                                             parameters;
            const vex::vector<double>&                                             k_list;
            vex::multivector<double, (2*2)*(2*2)>& u2_tensor;
        };


      // integration - observer object for 2pf
      template <typename number>
      class dquad_twopf_observer
        {
          public:
            dquad_twopf_observer(std::vector<double>& s,
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

          number rval =  (M_chi*M_chi)*(chi*chi)/2.0+(M_phi*M_phi)*(phi*phi)/2.0;

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


      // Integrate the 2pf - on the GPU


      template <typename number>
      transport::twopf<number> dquad<number>::twopf(vex::Context& ctx,
        const std::vector<double>& ks, double Nstar,
        const std::vector<number>& ics, const std::vector<double>& times)
        {
          using namespace boost::numeric::odeint;

          // validate initial conditions (or set up ics for momenta if necessary)
          std::vector<number> hst_bg = ics;
          this->fix_initial_conditions(ics, hst_bg);
          this->write_initial_conditions(hst_bg, std::cout, 1e-08, 1e-08, 0.001);

          // set up vector of ks corresponding to honest comoving momenta
          std::vector<double> real_ks(ks.size());
          this->rescale_ks(ks, real_ks, Nstar, hst_bg);

          // initialize device copy of k list
          vex::vector<double> dev_ks(ctx.queue(), real_ks);
          
          // set up space for the u2-tensor
          vex::multivector<double, (2*2)*(2*2)> u2_tensor(ctx.queue(), real_ks.size());

          // set up a functor to evolve this system
          dquad_twopf_functor<number> system(this->parameters, this->M_Planck, dev_ks, u2_tensor);

          twopf_state dev_x(ctx.queue(), real_ks.size());

          // fix initial conditions for the background + 2pf
          // -- background first
          dev_x(0) =  hst_bg[0];
          dev_x(1) =  hst_bg[1];
          dev_x(2) =  hst_bg[2];
          dev_x(3) =  hst_bg[3];

          // now for 2pf
          std::vector<double> hst_tp(real_ks.size());
          for(int i = 0; i < 2*2; i++)
            {
              for(int j = 0; j < 2*2; j++)
                {
                  this->make_tpf_ic(i, j, real_ks, Nstar, hst_bg, hst_tp);
                  vex::copy(hst_tp, dev_x(2*2+(2*2*i)+j));
                }
            }

          // set up functor to observe the integration
          std::vector<double>                               slices;
          std::vector< std::vector<number> >                background_history;
          std::vector< std::vector< std::vector<number> > > twopf_history;
          dquad_twopf_observer<number>                  obs(slices, background_history, twopf_history, ks.size());

          typedef runge_kutta_dopri5<twopf_state, double, twopf_state, double,
            boost::numeric::odeint::vector_space_algebra,
            boost::numeric::odeint::default_operations> stepper;

          integrate_times( make_dense_output<stepper>(1e-08, 1e-08),
            system, dev_x, times.begin(), times.end(), 0.001, obs);

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

              __rics.push_back(-2.0*(__Mp*__Mp)*(M_phi*M_phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*phi);
              __rics.push_back(-2.0*(__Mp*__Mp)*(M_chi*M_chi)*chi/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi)));;
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
      void dquad<number>::make_tpf_ic(unsigned int __i, unsigned int __j,
        const std::vector<double>& __ks, double __Nstar,
        const std::vector<number>& __fields, std::vector<double>& __tpf)
        {
          const auto M_phi  = this->parameters[0];
          const auto M_chi  = this->parameters[1];;
          const auto phi = __fields[0];
          const auto chi = __fields[1];
          const auto __dphi = __fields[2];
          const auto __dchi = __fields[3];;
          const auto __Mp              = this->M_Planck;

          const auto __Hsq             = -1.0/(__Mp*__Mp)*( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))/( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0);

          for(int __n = 0; __n < __ks.size(); __n++)
            {
              if(__i < 2 && __j < 2)        // field-field correlation function
                {
                  if(__i == __j)
                    {
                      __tpf[__n] = (1.0/(2.0*__ks[__n])) * exp(2.0 * __Nstar);
                    }
                  else
                    {
                      __tpf[__n] = 0.0;
                    }
                }
              else if((__i < 2 && __j >= 2)   // field-momentum correlation function
                      || (__i >= 2 && __j < 2))
                {
                  if(__i >= 2) __i -= 2;
                  if(__j >= 2) __j -= 2;

                  if(__i == __j)
                    {
                      __tpf[__n] = -(1.0/(2.0*__ks[__n])) * exp(2.0 * __Nstar);
                    }
                  else
                    {
                      __tpf[__n] = 0.0;
                    }
                }
              else if(__i >= 2 && __j >= 2)   // momentum-momentum correlation function
                {
                  if(__i == __j)
                    {
                      __tpf[__n] = (__ks[__n]/(2.0*__Hsq)) * exp(4.0 * __Nstar);
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

          const auto __Hsq             = -1.0/(__Mp*__Mp)*( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))/( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0);

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

          __dxdt[0]                = __dphi;
          __dxdt[1]                = __dchi;
          __dxdt[2]                =  __dphi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0+(__Mp*__Mp)*(M_phi*M_phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*phi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0);
          __dxdt[3]                =  (__Mp*__Mp)*(M_chi*M_chi)*chi/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+__dchi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0;;
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
          const auto M_phi  = vex::tag<0>(this->parameters[0]);
          const auto M_chi  = vex::tag<1>(this->parameters[1]);;
          const auto phi = vex::tag<2>(__x(0));
          const auto chi = vex::tag<3>(__x(1));
          const auto __dphi = vex::tag<4>(__x(2));
          const auto __dchi = vex::tag<5>(__x(3));;
          const auto __Mp              = vex::tag<6>(this->M_Planck);
          const auto __k               = vex::tag<7>(this->k_list);
          const auto __a               = vex::tag<8>(exp(__t));

          const unsigned int start_background = 0;
          const unsigned int start_twopf      = 2*2;

          const auto __tpf_0_0 =  vex::tag<9>(__x(start_twopf+(2*2*0)+0));
          const auto __tpf_1_0 =  vex::tag<10>(__x(start_twopf+(2*2*1)+0));
          const auto __tpf_2_0 =  vex::tag<11>(__x(start_twopf+(2*2*2)+0));
          const auto __tpf_3_0 =  vex::tag<12>(__x(start_twopf+(2*2*3)+0));
          const auto __tpf_0_1 =  vex::tag<13>(__x(start_twopf+(2*2*0)+1));
          const auto __tpf_1_1 =  vex::tag<14>(__x(start_twopf+(2*2*1)+1));
          const auto __tpf_2_1 =  vex::tag<15>(__x(start_twopf+(2*2*2)+1));
          const auto __tpf_3_1 =  vex::tag<16>(__x(start_twopf+(2*2*3)+1));
          const auto __tpf_0_2 =  vex::tag<17>(__x(start_twopf+(2*2*0)+2));
          const auto __tpf_1_2 =  vex::tag<18>(__x(start_twopf+(2*2*1)+2));
          const auto __tpf_2_2 =  vex::tag<19>(__x(start_twopf+(2*2*2)+2));
          const auto __tpf_3_2 =  vex::tag<20>(__x(start_twopf+(2*2*3)+2));
          const auto __tpf_0_3 =  vex::tag<21>(__x(start_twopf+(2*2*0)+3));
          const auto __tpf_1_3 =  vex::tag<22>(__x(start_twopf+(2*2*1)+3));
          const auto __tpf_2_3 =  vex::tag<23>(__x(start_twopf+(2*2*2)+3));
          const auto __tpf_3_3 =  vex::tag<24>(__x(start_twopf+(2*2*3)+3));
          auto __u2_0_0        =  vex::tag<25>(this->u2_tensor((2*2*0)+0));
          auto __u2_1_0        =  vex::tag<26>(this->u2_tensor((2*2*1)+0));
          auto __u2_2_0        =  vex::tag<27>(this->u2_tensor((2*2*2)+0));
          auto __u2_3_0        =  vex::tag<28>(this->u2_tensor((2*2*3)+0));
          auto __u2_0_1        =  vex::tag<29>(this->u2_tensor((2*2*0)+1));
          auto __u2_1_1        =  vex::tag<30>(this->u2_tensor((2*2*1)+1));
          auto __u2_2_1        =  vex::tag<31>(this->u2_tensor((2*2*2)+1));
          auto __u2_3_1        =  vex::tag<32>(this->u2_tensor((2*2*3)+1));
          auto __u2_0_2        =  vex::tag<33>(this->u2_tensor((2*2*0)+2));
          auto __u2_1_2        =  vex::tag<34>(this->u2_tensor((2*2*1)+2));
          auto __u2_2_2        =  vex::tag<35>(this->u2_tensor((2*2*2)+2));
          auto __u2_3_2        =  vex::tag<36>(this->u2_tensor((2*2*3)+2));
          auto __u2_0_3        =  vex::tag<37>(this->u2_tensor((2*2*0)+3));
          auto __u2_1_3        =  vex::tag<38>(this->u2_tensor((2*2*1)+3));
          auto __u2_2_3        =  vex::tag<39>(this->u2_tensor((2*2*2)+3));
          auto __u2_3_3        =  vex::tag<40>(this->u2_tensor((2*2*3)+3));

          #define __background(a) __dxdt(start_background + a)
          #define __dtwopf(a,b)   __dxdt(start_twopf + (2*2*a) + b)

          // evolve the background
          __background(0) = __dphi;
          __background(1) = __dchi;
          __background(2) =  __dphi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0+(__Mp*__Mp)*(M_phi*M_phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*phi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0);
          __background(3) =  (__Mp*__Mp)*(M_chi*M_chi)*chi/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+__dchi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0;;

          // set up a k-dependent u2 tensor
          __u2_0_0 = 0.0;
          __u2_1_0 = 0.0;
          __u2_2_0 =  (__Mp*__Mp)*(M_phi*M_phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+2.0*__dphi*(M_phi*M_phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*phi*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+1.0/(__a*__a)*(__Mp*__Mp)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*(__k*__k)*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+(__dphi*__dphi)/(__Mp*__Mp)*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0;
          __u2_3_0 =  ( __dphi*(M_chi*M_chi)*chi+__dchi*(M_phi*M_phi)*phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+__dchi*__dphi/(__Mp*__Mp)*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0;
          __u2_0_1 = 0.0;
          __u2_1_1 = 0.0;
          __u2_2_1 =  ( __dphi*(M_chi*M_chi)*chi+__dchi*(M_phi*M_phi)*phi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+__dchi*__dphi/(__Mp*__Mp)*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0;
          __u2_3_1 =  (__dchi*__dchi)/(__Mp*__Mp)*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)/2.0+2.0*__dchi*(M_chi*M_chi)*chi/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)+(__Mp*__Mp)/(__a*__a)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0)*(__k*__k)+(__Mp*__Mp)*(M_chi*M_chi)/( (M_chi*M_chi)*(chi*chi)+(M_phi*M_phi)*(phi*phi))*( 1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))-6.0);
          __u2_0_2 = 1.0;
          __u2_1_2 = 0.0;
          __u2_2_2 =  1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))/2.0-3.0;
          __u2_3_2 = 0.0;
          __u2_0_3 = 0.0;
          __u2_1_3 = 1.0;
          __u2_2_3 = 0.0;
          __u2_3_3 =  1.0/(__Mp*__Mp)*( (__dphi*__dphi)+(__dchi*__dchi))/2.0-3.0;;

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
          std::vector<number>                hst_background_state(2*2);
          std::vector< std::vector<number> > hst_state(2*2 * 2*2);

          // copy device state into local storage, and then push it into the history
          // (** TODO how slow is this?)

          // first, background
          for(int i = 0; i < 2*2; i++)
            {
              hst_background_state[i] = x(i)[0];  // only need to make a copy for one k-mode; the rest are all the same
            }
          this->background_history.push_back(hst_background_state);

          // then, two pf
          for(int i = 0; i < 2*2 * 2*2; i++)
            {
              // ensure destination is sufficiently large
              hst_state[i].resize(this->k_size);
              vex::copy(x(2*2 + i), hst_state[i]);
            }

          this->twopf_history.push_back(hst_state);
        }


  }   // namespace transport


#endif  // __CPP_TRANSPORT_dquad_H_

