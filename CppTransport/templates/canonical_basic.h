//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

// OpenMP implementation

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include "transport/transport.h"

#include "$$__CORE"


namespace transport
  {
    // set up a state type for 2pf integration
    template <typename number>
    using twopf_state = std::vector<number>;

    // set up a state type for 3pf integration
    template <typename number>
    using threepf_state = std::vector<number>;


    // *********************************************************************************************


    // CLASS FOR $$__MODEL 'basic', ie., OpenMP implementation
    template <typename number>
    class $$__MODEL_basic : public $$__MODEL<number>
      {
      public:
        $$__MODEL_basic(number Mp, const std::vector<number>& ps)
        : $$__MODEL<number>(Mp, ps)
          {
          }

        // Integrate background and 2-point function on the CPU, using OpenMP
        transport::twopf<number>
          twopf(const std::vector<double>& ks, double Nstar,
                const std::vector<number>& ics, const std::vector<double>& times,
                bool silent=false);

        // Integrate background, 2-point function and 3-point function on the CPU, using OpenMP
        // this simple implementation works on a cubic lattice of k-modes
        transport::threepf<number>
          threepf(const std::vector<double>& ks, double Nstar,
                  const std::vector<number>& ics, const std::vector<double>& times,
                  bool silent=false);

      protected:
        void twopf_kmode(double kmode, const std::vector<double>& times,
                         const std::vector<number>& ics,
                         std::vector <std::vector<number>>& background_history, std::vector <std::vector<number>>& twopf_history);

        void threepf_kmode(double kmode_1, double kmode_2, double kmode_3, const std::vector<double>& times,
                           const std::vector<number>& ics,
                           std::vector <std::vector<number>>& background_history,
                           std::vector <std::vector<number>>& twopf_re_history, std::vector <std::vector<number>>& twopf_im_history,
                           std::vector <std::vector<number>>& threepf_history);

        void populate_twopf_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit,
                               const std::vector<number>& ic, bool imaginary = false);

        void populate_threepf_ic(threepf_state<number>& x, unsigned int start, double kmode_1, double kmode_2, double kmode_3,
                                 double Ninit, const std::vector<number>& ic);

        void resize_twopf_history(std::vector <std::vector< std::vector<number> >>& twopf_history,
                                  const std::vector<double>& times, const std::vector<double>& ks);

        void resize_threepf_history(std::vector <std::vector< std::vector<number> >>& threepf_history,
                                    const std::vector<double>& times, const std::vector< struct threepf_kconfig >& ks);

      };


    // integration - 2pf functor
    template <typename number>
    class $$__MODEL_basic_twopf_functor
      {
      public:
        $$__MODEL_basic_twopf_functor(const std::vector<number>& p, const number Mp, double k)
          : parameters(p), M_Planck(Mp), k_mode(k)
          {
          }

        void operator ()(const twopf_state<number>& __x, twopf_state<number>& __dxdt, double __t);

      private:
        // constexpr version for rapid evaluation during integration
        constexpr unsigned int flatten(unsigned int a)                                 { return(a); }
        constexpr unsigned int flatten(unsigned int a, unsigned int b)                 { return(2*$$__NUMBER_FIELDS*a + b); }
        constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); }

        const number M_Planck;
        const std::vector<number>& parameters;
        const double k_mode;
      };


    // integration - observer object for 2pf
    template <typename number>
    class $$__MODEL_basic_twopf_observer
      {
      public:
        $$__MODEL_basic_twopf_observer(std::vector <std::vector<number>>& bh,
                                       std::vector <std::vector<number>>& tpfh)
          : background_history(bh), twopf_history(tpfh)
          {
          }

        void operator ()(const twopf_state<number>& x, double t);

      private:
        std::vector <std::vector<number>>& background_history;
        std::vector <std::vector<number>>& twopf_history;
      };


    // integration - 3pf functor
    template <typename number>
    class $$__MODEL_basic_threepf_functor
      {
      public:
        $$__MODEL_basic_threepf_functor(const std::vector<number>& p, const number Mp, double k1, double k2, double k3)
          : parameters(p), M_Planck(Mp), kmode_1(k1), kmode_2(k2), kmode_3(k3)
          {
          }

        void operator ()(const threepf_state<number>& __x, threepf_state<number>& __dxdt, double __dt);

      private:
        // constexpr version for rapid evaluation during integration
        constexpr unsigned int flatten(unsigned int a)                                 { return(a); }
        constexpr unsigned int flatten(unsigned int a, unsigned int b)                 { return(2*$$__NUMBER_FIELDS*a + b); }
        constexpr unsigned int flatten(unsigned int a, unsigned int b, unsigned int c) { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); }

        const number M_Planck;
        const std::vector<number>& parameters;
        const double kmode_1;
        const double kmode_2;
        const double kmode_3;
      };


    // integration - observer object for 3pf
    template <typename number>
    class $$__MODEL_basic_threepf_observer
      {
      public:
        $$__MODEL_basic_threepf_observer(std::vector <std::vector<number>>& bh,
                                         std::vector <std::vector<number>>& twopfh_re,
                                         std::vector <std::vector<number>>& twopfh_im,
                                         std::vector <std::vector<number>>& threepfh)
        : background_history(bh), twopf_re_history(twopfh_re), twopf_im_history(twopfh_im), threepf_history(threepfh)
          {
          }

        void operator ()(const threepf_state<number>& x, double t);

      private:
        std::vector <std::vector<number>>& background_history;
        std::vector <std::vector<number>>& twopf_re_history;
        std::vector <std::vector<number>>& twopf_im_history;
        std::vector <std::vector<number>>& threepf_history;
      };


    // TWO-POINT FUNCTION INTEGRATION


    // ks     -- vector of *conventionally normalized* wavenumbers for which we wish to compute the twopf
    //           (conventional normalization means k=1 is the mode which crosses the horizon at Nstar)
    // Nstar  -- horizon-exit of the mode with k-comoving = 1 takes place at Nstar e-folds
    // ics    -- vector of initial conditions for background fields (or fields+momenta)
    // times  -- vector of times at which the solution will be recorded
    // silent -- set to true to suppress verbose output
    template <typename number>
    transport::twopf<number> $$__MODEL_basic<number>::twopf(const std::vector<double>& ks, double Nstar,
                                                            const std::vector<number>& ics, const std::vector<double>& times,
                                                            bool silent)
      {
        this->validate_times(times, Nstar);

        // validate initial conditions (or set up ics for momenta if necessary)
        std::vector<number> real_ics = ics;
        this->fix_initial_conditions(ics, real_ics);
        if(!silent)
          {
            this->write_initial_conditions(real_ics, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");
          }

        // convert conventionally-normalized wavenumbers to
        // properly normalized comoving wavenumbers
        std::vector<double> com_ks = this->normalize_comoving_ks(real_ics, ks, *(times.begin()), Nstar, silent);

        // allocate space for storing the solution
        // the index convention is:
        //   first index  - time
        //   second index - component number
        //   third index  - k mode
        std::vector< std::vector<number> >                background_history;
        std::vector< std::vector< std::vector<number> > > twopf_history;

        // ensure there is sufficient space for the solution
        this->resize_twopf_history(twopf_history, times, ks);

        bool stored_background = false;
#pragma omp parallel for schedule(dynamic)
        for(int i = 0; i < ks.size(); i++)
          {
            std::vector< std::vector<number> > kmode_background_history;
            std::vector< std::vector<number> > kmode_twopf_history;

            // write the time history for this particular k-mode into kmode_background_history, kmode_twopf_history
            this->twopf_kmode(com_ks[i], times, real_ics, kmode_background_history, kmode_twopf_history);

            if(!stored_background)
              {
                background_history = kmode_background_history;
                stored_background  = true;
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

        transport::twopf<number> tpf(ks, com_ks, Nstar, times, background_history, twopf_history, this);

        return(tpf);
      }


    // kmode              -- *comoving normalized* wavenumber for which we will compute twopf
    // times              -- vector of times at which the solution will be recorded
    // ics                -- vector of initial conditions for the background fields (or fields+momenta)
    // background_history -- allocated space for storing the time-history of the background
    // twopf_history      -- allocated space for storing the time-history of the 2pf
    template <typename number>
    void $$__MODEL_basic<number>::twopf_kmode(double kmode, const std::vector<double>& times,
                                              const std::vector<number>& ics,
                                              std::vector <std::vector<number>>& background_history,
                                              std::vector <std::vector<number>>& twopf_history)
      {
        using namespace boost::numeric::odeint;

        // set up a functor to evolve this system
        $$__MODEL_basic_twopf_functor<number> rhs(this->parameters, this->M_Planck, kmode);

        // set up a functor to observe the integration
        $$__MODEL_basic_twopf_observer<number> obs(background_history, twopf_history);

        // set up a state vector
        twopf_state<number> x;
        x.resize($$__MODEL_pool::twopf_state_size);

        // fix initial conditions - background
        x[$$__MODEL_pool::backg_start + this->flatten($$__A)] = $$// ics[$$__A];

        // fix initial conditions - 2pf
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_start, kmode, *times.begin(), ics);

        integrate_times($$__MAKE_PERT_STEPPER{twopf_state<number>}, rhs, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);
      }


    // make initial conditions for each component of the 2pf
    // x         - state vector *containing* space for the 2pf (doesn't have to be entirely the 2pf)
    // start     - starting position of twopf components within the state vector
    // kmode     - *comoving normalized* wavenumber for which we will compute the twopf
    // Ninit     - initial time
    // ics       - iniitial conditions for the background fields (or fields+momenta)
    // imaginary - whether to populate using real or imaginary components of the 2pf
    template <typename number>
    void $$__MODEL_basic<number>::populate_twopf_ic(twopf_state<number>& x, unsigned int start, double kmode, double Ninit, const std::vector<number>& ics, bool imaginary)
      {
        assert(x.size() >= start);
        assert(x.size() >= start + $$__MODEL_pool::twopf_size);

        x[start + this->flatten($$__A,$$__B)] = imaginary ? this->make_twopf_im_ic($$__A, $$__B, kmode, Ninit, ics) : this->make_twopf_re_ic($$__A, $$__B, kmode, Ninit, ics) $$// ;
      }


    template <typename number>
    void $$__MODEL_basic<number>::resize_twopf_history(std::vector< std::vector< std::vector<number> > >& twopf_history, const std::vector<double>& times,
                                                       const std::vector<double>& ks)
      {
        twopf_history.resize(times.size());

        for(int i = 0; i < times.size(); i++)
          {
            twopf_history[i].resize($$__MODEL_pool::twopf_size);

            for(int j = 0; j < $$__MODEL_pool::twopf_size; j++)
              {
                // we need one copy of the components for each k
                twopf_history[i][j].resize(ks.size());
              }
          }
      }


    // THREE-POINT FUNCTION INTEGRATION


    template <typename number>
    transport::threepf<number> $$__MODEL_basic<number>::threepf(const std::vector<double>& ks, double Nstar,
                                                                const std::vector<number>& ics, const std::vector<double>& times,
                                                                bool silent)
      {
        using namespace boost::numeric::odeint;

        this->validate_times(times, Nstar);

        // validate initial conditions (or set up ics for momenta if necessary)
        std::vector<number> real_ics = ics;
        this->fix_initial_conditions(ics, real_ics);
        if(!silent)
          {
            this->write_initial_conditions(real_ics, std::cout, $$__PERT_ABS_ERR, $$__PERT_REL_ERR, $$__PERT_STEP_SIZE, "$$__PERT_STEPPER");
          }

        std::vector<double> com_ks = this->normalize_comoving_ks(real_ics, ks, *(times.begin()), Nstar, silent);

        // space for storing the solution
        std::vector< std::vector<number> >                background_history;
        std::vector< std::vector< std::vector<number> > > twopf_re_history;
        std::vector< std::vector< std::vector<number> > > twopf_im_history;
        std::vector< std::vector< std::vector<number> > > threepf_history;

        std::vector< struct threepf_kconfig >             kconfig_list;
        this->populate_kconfig_list(kconfig_list, com_ks);

        // ensure there is enough space to store the solution
        // the index convention is
        //   first index  - time
        //   second index - component number
        //   third index  - k mode
        //                  for the 2pf this corresponds to the list in ks (real_ks)
        //                  for the 3pf this is an index into the lattice ks^3 (real_ks)^3
        this->resize_twopf_history(twopf_re_history, times, ks);
        this->resize_twopf_history(twopf_im_history, times, ks);
        this->resize_threepf_history(threepf_history, times, kconfig_list);

        // step through the queue, solving for the three-point functions in each case
#pragma omp parallel for schedule(dynamic)
        for(int i = 0; i < kconfig_list.size(); i++)
          {
#pragma omp critical
            std::cout << __CPP_TRANSPORT_SOLVING_CONFIG << " " << i+1
              << " " __CPP_TRANSPORT_OF << " " << kconfig_list.size()
              << std::endl;

            std::vector< std::vector<number> > kmode_background_history;
            std::vector< std::vector<number> > kmode_twopf_re_history;
            std::vector< std::vector<number> > kmode_twopf_im_history;
            std::vector< std::vector<number> > kmode_threepf_history;

            // write the time history for this k-configuration
            this->threepf_kmode(com_ks[kconfig_list[i].indices[0]],
                                com_ks[kconfig_list[i].indices[1]],
                                com_ks[kconfig_list[i].indices[2]],
                                times, real_ics,
                                kmode_background_history,
                                kmode_twopf_re_history, kmode_twopf_im_history,
                                kmode_threepf_history);

            if(kconfig_list[i].store_background)
              {
                background_history = kmode_background_history;
              }

            if(kconfig_list[i].store_twopf)
              {
                for(int m = 0; m < kmode_twopf_re_history.size(); m++)           // m steps through the time-slices
                  {
                    for(int n = 0; n < kmode_twopf_re_history[m].size(); n++)    // n steps through the components
                      {
                        twopf_re_history[m][n][kconfig_list[i].indices[0]] = kmode_twopf_re_history[m][n];
                        twopf_im_history[m][n][kconfig_list[i].indices[0]] = kmode_twopf_im_history[m][n];
                      }
                  }
              }

            for(int m = 0; m < kmode_threepf_history.size(); m++)             // m steps through the time-slices
              {
                for(int n = 0; n < kmode_threepf_history[m].size(); n++)      // n steps through the components
                  {
                    threepf_history[m][n][i] = kmode_threepf_history[m][n];
                  }
              }
          }

        transport::threepf<number> tpf(ks, com_ks, Nstar, times, background_history, twopf_re_history, twopf_im_history, threepf_history, kconfig_list, this);

        return(tpf);
      }


    template <typename number>
    void $$__MODEL_basic<number>::threepf_kmode(double kmode_1, double kmode_2, double kmode_3, const std::vector<double>& times,
                                                const std::vector<number>& ics,
                                                std::vector< std::vector<number> >& background_history,
                                                std::vector< std::vector<number> >& twopf_re_history, std::vector< std::vector<number> >& twopf_im_history,
                                                std::vector< std::vector<number> >& threepf_history)
      {
        using namespace boost::numeric::odeint;

        // set up a functor to evolve this system
        $$__MODEL_basic_threepf_functor<number>  rhs(this->parameters, this->M_Planck, kmode_1, kmode_2, kmode_3);

        // set up a functor to observe the integration
        $$__MODEL_basic_threepf_observer<number> obs(background_history, twopf_re_history, twopf_im_history, threepf_history);

        // set up a state vector
        threepf_state<number> x;
        x.resize($$__MODEL_pool::threepf_state_size);

        // fix initial conditions - background
        x[$$__MODEL_pool::backg_start + this->flatten($$__A)] = $$// ics[$$__A];

        // fix initial conditions - real 2pfs
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k1_start, kmode_1, *times.begin(), ics, false);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k2_start, kmode_2, *times.begin(), ics, false);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_re_k3_start, kmode_3, *times.begin(), ics, false);

        // fix initial conditions - imaginary 2pfs
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k1_start, kmode_1, *times.begin(), ics, true);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k2_start, kmode_2, *times.begin(), ics, true);
        this->populate_twopf_ic(x, $$__MODEL_pool::twopf_im_k3_start, kmode_3, *times.begin(), ics, true);

        // fix initial conditions - threepf
        this->populate_threepf_ic(x, $$__MODEL_pool::threepf_start, kmode_1, kmode_2, kmode_3, *times.begin(), ics);

        integrate_times( $$__MAKE_PERT_STEPPER{threepf_state<number>}, rhs, x, times.begin(), times.end(), $$__PERT_STEP_SIZE, obs);
      }


    template <typename number>
    void $$__MODEL_basic<number>::resize_threepf_history(std::vector< std::vector< std::vector<number> > >& threepf_history, const std::vector<double>& times,
                                                         const std::vector< struct threepf_kconfig >& kconfig_list)
      {
        // the index convention for the threepf history is:
        //   first index  - time
        //   second index - component number
        //   third index  - k mode
        //                  this is an index into the lattice ks^3, remembering that we insist on the k-modes being ordered
        //                  in that case, there are N(N+1)(N+2)/6 distinct k-modes

        threepf_history.resize(times.size());

        for(int i = 0; i < times.size(); i++)
          {
            threepf_history[i].resize($$__MODEL_pool::threepf_size);

            for(int j = 0; j < $$__MODEL_pool::threepf_size; j++)
              {
                threepf_history[i][j].resize(kconfig_list.size());
              }
          }
      }


    template <typename number>
    void $$__MODEL_basic<number>::populate_threepf_ic(threepf_state<number>& x, unsigned int start, double kmode_1, double kmode_2, double kmode_3,
                                                      double Ninit, const std::vector<number>& ics)
      {
        assert(x.size() >= start);
        assert(x.size() >= start + $$__MODEL_pool::threepf_size);

#undef  MAKE_THREEPF
#define MAKE_THREEPF(i,j,k,k1,k2,k3) this->make_threepf_ic(i,j,k,k1,k2,k3,Ninit,ics)
#ifdef CHECK_3PF_SYMMETRIES
        CHECK_ZERO((MAKE_THREEPF($$__A,$$__B,$$__C,kmode_1,kmode_2,kmode_3)-MAKE_THREEPF($$__B,$$__A,$$__C,kmode_2,kmode_1,kmode_3))/MAKE_THREEPF($$__A,$$__B,$$__C,kmode_1,kmode_2,kmode_3), kmode_1, kmode_2, kmode_3) $$// ;
        CHECK_ZERO((MAKE_THREEPF($$__A,$$__B,$$__C,kmode_1,kmode_2,kmode_3)-MAKE_THREEPF($$__A,$$__C,$$__B,kmode_1,kmode_3,kmode_2))/MAKE_THREEPF($$__A,$$__B,$$__C,kmode_1,kmode_2,kmode_3), kmode_1, kmode_2, kmode_3) $$// ;
        CHECK_ZERO((MAKE_THREEPF($$__A,$$__B,$$__C,kmode_1,kmode_2,kmode_3)-MAKE_THREEPF($$__C,$$__B,$$__A,kmode_3,kmode_2,kmode_1))/MAKE_THREEPF($$__A,$$__B,$$__C,kmode_1,kmode_2,kmode_3), kmode_1, kmode_2, kmode_3) $$// ;
#endif

        x[start + this->flatten($$__A,$$__B,$$__C)] = MAKE_THREEPF($$__A,$$__B,$$__C, kmode_1, kmode_2, kmode_3) $$// ;
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF INTEGRATION


    template <typename number>
    void $$__MODEL_basic_twopf_functor<number>::operator()(const twopf_state<number>& __x, twopf_state<number>& __dxdt, double __t)
      {
        const auto $$__PARAMETER[1]  = this->parameters[$$__1];
        const auto $$__COORDINATE[A] = __x[this->flatten($$__A)];
        const auto __Mp              = this->M_Planck;
        const auto __k               = this->k_mode;
        const auto __a               = exp(__t);
        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;

        const auto __tpf_$$__A_$$__B = $$// __x[$$__MODEL_pool::twopf_start + this->flatten($$__A,$$__B)];

        $$__TEMP_POOL{"const auto $1 = $2;"}

#undef __background
#undef __dtwopf
#define __background(a)   __dxdt[$$__MODEL_pool::backg_start + this->flatten(a)]
#define __dtwopf(a,b)     __dxdt[$$__MODEL_pool::twopf_start + this->flatten(a,b)]

        // evolve the background
        __background($$__A) = $$__U1_PREDEF[A]{__Hsq, __eps};

        // set up components of the u2 tensor
        const auto __u2_$$__A_$$__B = $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};

        // evolve the 2pf
        // here, we are dealing only with the real part - which is symmetric.
        // so the index placement is not important
        __dtwopf($$__A, $$__B) = 0 $$// + $$__U2_NAME[AC]{__u2}*__tpf_$$__C_$$__B;
        __dtwopf($$__A, $$__B) += 0 $$// + $$__U2_NAME[BC]{__u2}*__tpf_$$__A_$$__C;
      }


    // IMPLEMENTATION - FUNCTOR FOR 2PF OBSERVATION


    template <typename number>
    void $$__MODEL_basic_twopf_observer<number>::operator()(const twopf_state<number>& x, double t)
      {
        // allocate storage for state
        std::vector<number> bg_x ($$__MODEL_pool::backg_size);
        std::vector<number> tpf_x($$__MODEL_pool::twopf_size);

        // first, background
        for(int i = 0; i < $$__MODEL_pool::backg_size; i++)
          {
            bg_x[i] = x[i];
          }
        this->background_history.push_back(bg_x);

        // then, 2pf
        for(int i = 0; i < $$__MODEL_pool::twopf_size; i++)
          {
            tpf_x[i] = x[$$__MODEL_pool::twopf_start + i];
          }
        this->twopf_history.push_back(tpf_x);
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF INTEGRATION


    template <typename number>
    void $$__MODEL_basic_threepf_functor<number>::operator()(const threepf_state<number>& __x, threepf_state<number>& __dxdt, double __t)
      {
        const auto $$__PARAMETER[1]  = this->parameters[$$__1];
        const auto $$__COORDINATE[A] = __x[this->flatten($$__A)];
        const auto __Mp              = this->M_Planck;
        const auto __k1              = this->kmode_1;
        const auto __k2              = this->kmode_2;
        const auto __k3              = this->kmode_3;
        const auto __a               = exp(__t);
        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;

        const auto __twopf_re_k1_$$__A_$$__B   = $$// __x[$$__MODEL_pool::twopf_re_k1_start + this->flatten($$__A,$$__B)];
        const auto __twopf_im_k1_$$__A_$$__B   = $$// __x[$$__MODEL_pool::twopf_im_k1_start + this->flatten($$__A,$$__B)];
        const auto __twopf_re_k2_$$__A_$$__B   = $$// __x[$$__MODEL_pool::twopf_re_k2_start + this->flatten($$__A,$$__B)];
        const auto __twopf_im_k2_$$__A_$$__B   = $$// __x[$$__MODEL_pool::twopf_im_k2_start + this->flatten($$__A,$$__B)];
        const auto __twopf_re_k3_$$__A_$$__B   = $$// __x[$$__MODEL_pool::twopf_re_k3_start + this->flatten($$__A,$$__B)];
        const auto __twopf_im_k3_$$__A_$$__B   = $$// __x[$$__MODEL_pool::twopf_im_k3_start + this->flatten($$__A,$$__B)];

        const auto __threepf_$$__A_$$__B_$$__C = $$// __x[$$__MODEL_pool::threepf_start     + this->flatten($$__A,$$__B,$$__C)];

        $$__TEMP_POOL{"const auto $1 = $2;"}

#undef __background
#undef __dtwopf_re_k1
#undef __dtwopf_im_k1
#undef __dtwopf_re_k2
#undef __dtwopf_im_k2
#undef __dtwopf_re_k3
#undef __dtwopf_im_k3
#undef __dthreepf
#define __background(a)      __dxdt[$$__MODEL_pool::backg_start       + this->flatten(a)]
#define __dtwopf_re_k1(a,b)  __dxdt[$$__MODEL_pool::twopf_re_k1_start + this->flatten(a,b)]
#define __dtwopf_im_k1(a,b)  __dxdt[$$__MODEL_pool::twopf_im_k1_start + this->flatten(a,b)]
#define __dtwopf_re_k2(a,b)  __dxdt[$$__MODEL_pool::twopf_re_k2_start + this->flatten(a,b)]
#define __dtwopf_im_k2(a,b)  __dxdt[$$__MODEL_pool::twopf_im_k2_start + this->flatten(a,b)]
#define __dtwopf_re_k3(a,b)  __dxdt[$$__MODEL_pool::twopf_re_k3_start + this->flatten(a,b)]
#define __dtwopf_im_k3(a,b)  __dxdt[$$__MODEL_pool::twopf_im_k3_start + this->flatten(a,b)]
#define __dthreepf(a,b,c)    __dxdt[$$__MODEL_pool::threepf_start     + this->flatten(a,b,c)]

        // evolve the background
        __background($$__A) = $$__U1_PREDEF[A]{__Hsq,__eps};

        // set up components of the u2 tensor for k1, k2, k3
        const auto __u2_k1_$$__A_$$__B = $$__U2_PREDEF[AB]{__k1, __a, __Hsq, __eps};
        const auto __u2_k2_$$__A_$$__B = $$__U2_PREDEF[AB]{__k2, __a, __Hsq, __eps};
        const auto __u2_k3_$$__A_$$__B = $$__U2_PREDEF[AB]{__k3, __a, __Hsq, __eps};

        // set up components of the u3 tensor
        const auto __u3_k1k2k3_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};
        const auto __u3_k2k1k3_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k2, __k1, __k3, __a, __Hsq, __eps};
        const auto __u3_k3k1k2_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k3, __k1, __k2, __a, __Hsq, __eps};

        // if desired, check that the u3 tensor is properly symmetric on its final two indices
        // division by zero errors may occur if any of the u3 components are not zero, but only if
        // the compiler is not able to optimize away the check
        // that itself indicates an error, since both terms should be zero giving 0-0
#ifdef CHECK_U3_SYMMETRY
        const auto __u3_k1k3k2_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k1, __k3, __k2, __a, __Hsq, __eps};
        const auto __u3_k2k3k1_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k2, __k3, __k1, __a, __Hsq, __eps};
        const auto __u3_k3k2k1_$$__A_$$__B_$$__C = $$__U3_PREDEF[ABC]{__k3, __k2, __k1, __a, __Hsq, __eps};

        CHECK_ZERO((__u3_k1k2k3_$$__A_$$__B_$$__C - __u3_k1k3k2_$$__A_$$__C_$$__B)/__u3_k1k2k3_$$__A_$$__B_$$__C,__k1,__k2,__k3) $$// ;
        CHECK_ZERO((__u3_k2k1k3_$$__A_$$__B_$$__C - __u3_k2k3k1_$$__A_$$__C_$$__B)/__u3_k2k1k3_$$__A_$$__B_$$__C,__k1,__k2,__k3) $$// ;
        CHECK_ZERO((__u3_k3k1k2_$$__A_$$__B_$$__C - __u3_k3k2k1_$$__A_$$__C_$$__B)/__u3_k3k1k2_$$__A_$$__B_$$__C,__k1,__k2,__k3) $$// ;
#endif

        // evolve the real and imaginary components of the 2pf
        // for the imaginary parts, index placement *does* matter so we must take care
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
        // index placement matters, partly because of the k-dependence
        // but also in the source terms from the imaginary components of the 2pf

        __dthreepf($$__A, $$__B, $$__C)  = 0 $$// + $$__U2_NAME[AM]{__u2_k1}*__threepf_$$__M_$$__B_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[AMN]{__u3_k1k2k3}*__twopf_re_k2_$$__M_$$__B*__twopf_re_k3_$$__N_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[AMN]{__u3_k1k2k3}*__twopf_im_k2_$$__M_$$__B*__twopf_im_k3_$$__N_$$__C;

        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U2_NAME[BM]{__u2_k2}*__threepf_$$__A_$$__M_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[BMN]{__u3_k2k1k3}*__twopf_re_k1_$$__A_$$__M*__twopf_re_k3_$$__N_$$__C;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[BMN]{__u3_k2k1k3}*__twopf_im_k1_$$__A_$$__M*__twopf_im_k3_$$__N_$$__C;

        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U2_NAME[CM]{__u2_k3}*__threepf_$$__A_$$__B_$$__M;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// + $$__U3_NAME[CMN]{__u3_k3k1k2}*__twopf_re_k1_$$__A_$$__M*__twopf_re_k2_$$__B_$$__N;
        __dthreepf($$__A, $$__B, $$__C) += 0 $$// - $$__U3_NAME[CMN]{__u3_k3k1k2}*__twopf_im_k1_$$__A_$$__M*__twopf_im_k2_$$__B_$$__N;
      }


    // IMPLEMENTATION - FUNCTOR FOR 3PF OBSERVATION


    template <typename number>
    void $$__MODEL_basic_threepf_observer<number>::operator()(const threepf_state<number>& x, double t)
      {
        // allocate storage for state
        std::vector<number> bg_x      ($$__MODEL_pool::backg_size);
        std::vector<number> twopf_re_x($$__MODEL_pool::twopf_size);
        std::vector<number> twopf_im_x($$__MODEL_pool::twopf_size);
        std::vector<number> threepf_x ($$__MODEL_pool::threepf_size);

        // first, background
        for(int i = 0; i < $$__MODEL_pool::backg_size; i++)
          {
            bg_x[i] = x[i];
          }
        this->background_history.push_back(bg_x);

        // then, real part of the 2pf
        for(int i = 0; i < $$__MODEL_pool::twopf_size; i++)
          {
            twopf_re_x[i] = x[$$__MODEL_pool::twopf_re_k1_start + i];
          }
        this->twopf_re_history.push_back(twopf_re_x);

        // then, the imaginary part of the 2pf
        for(int i = 0; i < $$__MODEL_pool::twopf_size; i++)
          {
            twopf_im_x[i] = x[$$__MODEL_pool::twopf_im_k1_start + i];
          }
        this->twopf_im_history.push_back(twopf_im_x);

        // finally, 3pf
        for(int i = 0; i < $$__MODEL_pool::threepf_size; i++)
          {
            threepf_x[i] = x[$$__MODEL_pool::threepf_start + i];
          }
        this->threepf_history.push_back(threepf_x);
      }


    }   // namespace transport


#endif  // $$__GUARD
