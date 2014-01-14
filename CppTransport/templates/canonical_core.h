// backend=cpp minver=0.06
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $$__TOOL $$__VERSION
//
// '$$__HEADER' generated from '$$__SOURCE'
// processed on $$__DATE

#ifndef $$__GUARD   // avoid multiple inclusion
#define $$__GUARD

#include <assert.h>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "boost/numeric/odeint.hpp"
#include "transport/transport.h"


namespace transport
  {
    template <typename number>
    using backg_state = std::vector<number>;

    // Literal data pool
    namespace $$__MODEL_pool
      {
        static std::vector<std::string> field_names = $$__FIELD_NAME_LIST;
        static std::vector<std::string> latex_names = $$__LATEX_NAME_LIST;
        static std::vector<std::string> param_names = $$__PARAM_NAME_LIST;
        static std::vector<std::string> platx_names = $$__PLATX_NAME_LIST;
        static std::vector<std::string> state_names = $$__STATE_NAME_LIST;

        static std::string              name        = "$$__NAME";
        static std::string              author      = "$$__AUTHOR";
        static std::string              tag         = "$$__TAG";
        static std::string              unique_id   = "$$__UNIQUE_ID";

        constexpr unsigned int backg_size         = (2*$$__NUMBER_FIELDS);
        constexpr unsigned int twopf_size         = ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS));
        constexpr unsigned int threepf_size       = ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS));
     
        constexpr unsigned int backg_start        = 0;
        constexpr unsigned int twopf_start        = backg_start + backg_size;
        constexpr unsigned int twopf_re_k1_start  = twopf_start;
        constexpr unsigned int twopf_im_k1_start  = twopf_re_k1_start + twopf_size;
        constexpr unsigned int twopf_re_k2_start  = twopf_im_k1_start + twopf_size;
        constexpr unsigned int twopf_im_k2_start  = twopf_re_k2_start + twopf_size;
        constexpr unsigned int twopf_re_k3_start  = twopf_im_k2_start + twopf_size;
        constexpr unsigned int twopf_im_k3_start  = twopf_re_k3_start + twopf_size;
        constexpr unsigned int threepf_start      = twopf_im_k3_start + twopf_size;

        constexpr unsigned int backg_state_size   = backg_size;
        constexpr unsigned int twopf_state_size   = backg_size + twopf_size;
        constexpr unsigned int threepf_state_size = backg_size + 6*twopf_size + threepf_size;

        constexpr unsigned int u2_size            = ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS));
        constexpr unsigned int u3_size            = ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS));
      }


    // *********************************************************************************************


    // CLASS FOR $$__MODEL CORE
    // contains code and functionality shared by all the compute backends (OpenMP, MPI, OpenCL, CUDA, ...)
    // these backends are implemented by classes which inherit from this common core
    template <typename number>
    class $$__MODEL : public canonical_model<number>
      {
      public:
        // CONSTRUCTOR, DESTRUCTOR

        $$__MODEL(instance_manager<number>* mgr);

        // INTERFACE: EXTRACT MODEL INFORMATION

        const std::string& get_name() const { return($$__MODEL_pool::name); }

        const std::string& get_author() const { return($$__MODEL_pool::author); }

        const std::string& get_tag() const { return($$__MODEL_pool::tag); }

        unsigned int get_N_fields() const { return($$__NUMBER_FIELDS); }

        unsigned int get_N_params() const { return($$__NUMBER_PARAMS); }

        const std::vector< std::string >& get_field_names() const { return($$__MODEL_pool::field_names); }

        const std::vector< std::string >& get_f_latex_names() const { return($$__MODEL_pool::latex_names); }

        const std::vector< std::string >& get_param_names() const { return($$__MODEL_pool::param_names); }

        const std::vector< std::string >& get_p_latex_names() const { return($$__MODEL_pool::platx_names); }

        const std::vector< std::string >& get_state_names() const { return($$__MODEL_pool::state_names); }

        // INTERFACE: INDEX FLATTENING FUNCTIONS

        unsigned int flatten(unsigned int a)                                  const { return(a); };
        unsigned int flatten(unsigned int a, unsigned int b)                  const { return(2*$$__NUMBER_FIELDS*a + b); };
        unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)  const { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); };

        // INTERFACE: INDEX TRAITS

        unsigned int species(unsigned int a)      const { return((a >= $$__NUMBER_FIELDS) ? a-$$__NUMBER_FIELDS : a); };
        unsigned int momentum(unsigned int a)     const { return((a >= $$__NUMBER_FIELDS) ? a : a+$$__NUMBER_FIELDS); };
        unsigned int is_field(unsigned int a)     const { return(a < $$__NUMBER_FIELDS); }
        unsigned int is_momentum(unsigned int a)  const { return(a >= $$__NUMBER_FIELDS && a <= 2*$$__NUMBER_FIELDS); }

        // INTERFACE: COMPUTE BASIC PHYSICAL QUANTITIES

      public:
        // Over-ride functions inherited from 'model'
        number H(const parameters<number>& __params, const std::vector<number>& __coords) const;
        number epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const;

        // Over-ride functions inherited from 'canonical_model'
        number V(const parameters<number>& __params, const std::vector<number>& __coords) const;

        // INITIAL CONDITIONS HANDLING

      protected:
        void validate_initial_conditions(const parameters<number>& p, const std::vector<number>& input, std::vector<number>& output);

      public:
        typename initial_conditions<number>::ics_validator ics_validator_factory()
          {
            return(std::bind(&$$__MODEL::validate_initial_conditions, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
          }

        // PARAMETERS HANDLING

      protected:
        void validate_parameters(const std::vector<number>& input, std::vector<number>& output);

      public:
        typename parameters<number>::params_validator params_validator_factory()
          {
            return(std::bind(&$$__MODEL::validate_parameters, this, std::placeholders::_1, std::placeholders::_2));
          }

        // CALCULATE MODEL-SPECIFIC QUANTITIES

      public:
        // calculate gauge transformations to zeta
        void compute_gauge_xfm_1(const parameters<number>& params, const std::vector<number>& __state, std::vector<number>& __dN);
        void compute_gauge_xfm_2(const parameters<number>& params, const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN);

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        void u2(const parameters<number>& params, const std::vector<number>& __fields, double __k, double __N, std::vector< std::vector<number> >& __u2);
        void u3(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __u3);

        void A(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __A);
        void B(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __B);
        void C(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __C);

        // INITIAL CONDITIONS FOR N-POINT FUNCTIONS

      protected:
        number make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const parameters<number>& params, const std::vector<number>& __fields);

        number make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const parameters<number>& params, const std::vector<number>& __fields);

        number make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                               double kmode_1, double kmode_2, double kmode_3, double __Ninit, const parameters<number>& params, const std::vector<number>& __fields);

        // BACKEND INTERFACE (PARTIAL IMPLEMENTATION -- WE PROVIDE A COMMON BACKGROUND INTEGRATOR)

      public:
        void backend_process_backg(const task<number>* tk, typename model<number>::backg_history& solution, bool silent=false);
      };


    // integration - background functor
    template <typename number>
    class $$__MODEL_background_functor: public constexpr_flattener<$$__NUMBER_FIELDS>
      {
      public:
        $$__MODEL_background_functor(const parameters<number>& p)
          : params(p)
          {
          }

        void operator ()(const backg_state<number>& __x, backg_state<number>& __dxdt, double __t);

        const parameters<number> params;
      };


    // integration - observer object for background only
    template <typename number>
    class $$__MODEL_background_observer
      {
      public:
        $$__MODEL_background_observer(typename model<number>::backg_history& h)
          : history(h)
          {
          }

        void operator ()(const backg_state<number>& x, double t);

      private:
        typename model<number>::backg_history& history;
      };


    // CLASS $$__MODEL -- CONSTRUCTORS, DESTRUCTORS


    template <typename number>
    $$__MODEL<number>::$$__MODEL(instance_manager<number>* mgr)
      : canonical_model<number>(mgr, "$$__UNIQUE_ID", static_cast<unsigned int>(100*$$__VERSION))
      {
      }


    // INTERFACE: COMPUTE BASIC PHYSICAL QUANTITIES


    template <typename number>
    number $$__MODEL<number>::H(const parameters<number>& __params, const std::vector<number>& __coords) const
      {
        assert(__coords.size() == 2*$$__NUMBER_FIELDS);

        if(__coords.size() == 2*$$__NUMBER_FIELDS)
          {
            const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
            const auto $$__COORDINATE[A] = __coords[$$__A];
            const auto __Mp              = __params.get_Mp();

            $$__TEMP_POOL{"const auto $1 = $2;"}

            return(sqrt($$__HUBBLE_SQ));
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_ICS_A << __coords.size() << __CPP_TRANSPORT_WRONG_ICS_B << 2*$$__NUMBER_FIELDS << ")";
            throw std::out_of_range(msg.str());
          }
      }


    template <typename number>
    number $$__MODEL<number>::epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const
      {
        assert(__coords.size() == 2*$$__NUMBER_FIELDS);

        if(__coords.size() == 2*$$__NUMBER_FIELDS)
          {
            const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
            const auto $$__COORDINATE[A] = __coords[$$__A];
            const auto __Mp              = __params.get_Mp();

            $$__TEMP_POOL{"const auto $1 = $2;"}

            return($$__EPSILON);
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_ICS_A << __coords.size() << __CPP_TRANSPORT_WRONG_ICS_B << 2*$$__NUMBER_FIELDS << ")";
            throw std::out_of_range(msg.str());
          }
      }


    template <typename number>
    number $$__MODEL<number>::V(const parameters<number>& __params, const std::vector<number>& __coords) const
      {
        assert(__coords.size() == 2*$$__NUMBER_FIELDS);

        if(__coords.size() == 2*$$__NUMBER_FIELDS)
          {
            const auto $$__PARAMETER[1] = __params.get_vector()[$$__1];
            const auto $$__FIELD[a]     = __coords[$$__a];
            const auto __Mp             = __params.get_Mp();

            $$__TEMP_POOL{"const auto $1 = $2;"}

            return $$__POTENTIAL;
          }
        else
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_ICS_A << __coords.size() << __CPP_TRANSPORT_WRONG_ICS_B << 2*$$__NUMBER_FIELDS << ")";
            throw std::out_of_range(msg.str());
          }
      }


    // Handle initial conditions


    template <typename number>
    void $$__MODEL<number>::validate_initial_conditions(const parameters<number>& __params, const std::vector<number>& __input, std::vector<number>& __output)
      {
        __output.clear();
        __output.reserve(2*$$__NUMBER_FIELDS);
        __output.insert(__output.end(), __input.begin(), __input.end());

        if(__input.size() == $$__NUMBER_FIELDS)  // initial conditions for momenta *were not* supplied -- need to compute them
          {
            // supply the missing initial conditions using a slow-roll approximation
            const auto $$__PARAMETER[1] = __params.get_vector()[$$__1];
            const auto $$__FIELD[a]     = __input[$$__a];
            const auto __Mp             = __params.get_Mp();

            $$__TEMP_POOL{"const auto $1 = $2;"}

            __output.push_back($$__SR_VELOCITY[a]);
          }
        else if(__input.size() == 2*$$__NUMBER_FIELDS)  // initial conditions for momenta *were* supplied
          {
            // need do nothing
          }
        else
          {
            std::ostringstream msg;

            msg << __CPP_TRANSPORT_WRONG_ICS_A << __input.size() << "]"
                << __CPP_TRANSPORT_WRONG_ICS_B << $$__NUMBER_FIELDS
                << __CPP_TRANSPORT_WRONG_ICS_C << 2*$$__NUMBER_FIELDS << "]";

            throw std::out_of_range(msg.str());
          }
      }


    // Handle parameters


    template <typename number>
    void $$__MODEL<number>::validate_parameters(const std::vector<number>& input, std::vector<number>& output)
      {
        output.clear();

        if(input.size() == $$__NUMBER_PARAMS)
          {
            output.assign(input.begin(), input.end());
          }
        else
          {
            std::ostringstream msg;

            msg << __CPP_TRANSPORT_WRONG_PARAMS_A << input.size() << __CPP_TRANSPORT_WRONG_PARAMS_B << $$__NUMBER_PARAMS << ")";

            throw std::out_of_range(msg.str());
          }
      }


    // set up initial conditions for the real part of the equal-time two-point function
    // __i,__j  -- label component of the twopf for which we wish to compute initial conditions
    // __k      -- *comoving normalized* wavenumber for which we wish to assign initial conditions
    // __Ninit  -- initial time
    // __fields -- vector of initial conditions for the background fields (or fields+momenta)
    template <typename number>
    number $$__MODEL<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j,
                                               double __k, double __Ninit,
                                               const parameters<number>& __params,
                                               const std::vector<number>& __fields)
      {
        const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __ainit           = exp(__Ninit);

        const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

        number     __tpf             = 0.0;

        $$__TEMP_POOL{"const auto $1 = $2;"}

//        std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __M;
//
//        __M[$$__a][$$__b] = $$__M_PREDEF[ab]{__Hsq, __eps};

        // NOTE - conventions for the scale factor are
        //   a = exp(t), where t is the user-defined time (usually = 0 at the start of the integration)
        //   so usually this is zero

        if(IS_FIELD(__i) && IS_FIELD(__j))              // field-field correlation function
          {
            // LEADING-ORDER INITIAL CONDITION
            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);
            auto __subl    = 0.0;
            auto __subsubl = 0.0;

            // NEXT-ORDER INITIAL CONDITION - induces rapid onset of subhorizon oscillations
//              auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));
//              auto __subl    = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N))
//                               + (3.0/2.0)*__M[SPECIES(__i)][SPECIES(__j)];
//              auto __subsubl = (9.0/4.0)*__M[SPECIES(__i)][SPECIES(__j)];

            __tpf = + __leading                             / (2.0*__k*__ainit*__ainit)
                    + __subl*__Hsq                          / (2.0*__k*__k*__k)
                    + __subsubl*__Hsq*__Hsq*__ainit*__ainit / (2.0*__k*__k*__k*__k*__k);
          }
        else if((IS_FIELD(__i) && IS_MOMENTUM(__j))     // field-momentum or momentum-field correlation function
                || (IS_MOMENTUM(__i) && IS_FIELD(__j)))
          {
            // LEADING-ORDER INITIAL CONDITION
            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (-1.0);
            auto __subl    = 0.0;
            auto __subsubl = 0.0;

            // NEXT-ORDER INITIAL CONDITION - induces slow onset of subhorizon oscillations
//              auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (-1.0 + __eps*(1.0-2.0*__N));
//              auto __subl    = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (- __eps);
//              auto __subsubl = (9.0/4.0)*__M[SPECIES(__i)][SPECIES(__j)];

            __tpf = + __leading                             / (2.0*__k*__ainit*__ainit)
                    + __subl*__Hsq                          / (2.0*__k*__k*__k)
                    + __subsubl*__Hsq*__Hsq*__ainit*__ainit / (2.0*__k*__k*__k*__k*__k);
          }
        else if(IS_MOMENTUM(__i) && IS_MOMENTUM(__j))   // momentum-momentum correlation function
          {
            // LEADING-ORDER INITIAL CONDITION
            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);
            auto __subl    = 0.0;
            auto __subsubl = 0.0;

            // NEXT-ORDER INITIAL CONDITION - induces rapid onset of subhorizon oscillations
//              auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));
//              auto __subl    = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * 2.0*__eps
//                               - (3.0/2.0)*__M[SPECIES(__i)][SPECIES(__j)];
//              auto __subsubl = - (3.0/4.0)*__M[SPECIES(__i)][SPECIES(__j)];

            __tpf = + __k*__leading   / (2.0*__Hsq*__ainit*__ainit*__ainit*__ainit)
                    + __subl          / (2.0*__k*__ainit*__ainit)
                    + __subsubl*__Hsq / (2.0*__k*__k*__k);
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
                                             double __k, double __Ninit,
                                             const parameters<number>& __params,
                                             const std::vector<number>& __fields)
    {
      const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
      const auto $$__COORDINATE[A] = __fields[$$__A];
      const auto __Mp              = __params.get_Mp();

      const auto __Hsq             = $$__HUBBLE_SQ;
      const auto __eps             = $$__EPSILON;
      const auto __ainit           = exp(__Ninit);

      const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

      number     __tpf             = 0.0;

      $$__TEMP_POOL{"const auto $1 = $2;"}

//      std::array< std::array<number, $$__NUMBER_FIELDS>, $$__NUMBER_FIELDS> __M;
//
//      __M[$$__a][$$__b] = $$__M_PREDEF[ab]{__Hsq, __eps};

      // only the field-momentum and momentum-field correlation functions have imaginary parts
      if(IS_FIELD(__i) && IS_MOMENTUM(__j))
        {
          // LEADING-ORDER INITIAL CONDITION
          auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);

          // NEXT-ORDER INITIAL CONDITION
//            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));

          __tpf = + __leading / (2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit);
        }
      else if(IS_MOMENTUM(__i) && IS_FIELD(__j))
        {
          // LEADING-ORDER INITIAL CONDITION
          auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);

          // NEXT-ORDER INITIAL CONDITION
//            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));

          __tpf = - __leading / (2.0*sqrt(__Hsq)*__ainit*__ainit*__ainit);
        }

      return(__tpf);
    }


    template <typename number>
    number $$__MODEL<number>::make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                                              double __kmode_1, double __kmode_2, double __kmode_3, double __Ninit,
                                              const parameters<number>& __params,
                                              const std::vector<number>& __fields)
      {
        const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __ainit           = exp(__Ninit);

        const auto __kt              = __kmode_1 + __kmode_2 + __kmode_3;
        const auto __Ksq             = __kmode_1*__kmode_2 + __kmode_1*__kmode_3 + __kmode_2*__kmode_3;
        const auto __kprod3          = 4.0 * __kmode_1*__kmode_1*__kmode_1 * __kmode_2*__kmode_2*__kmode_2 * __kmode_3*__kmode_3*__kmode_3;

        const auto __k2dotk3         = (__kmode_1*__kmode_1 - __kmode_2*__kmode_2 - __kmode_3*__kmode_3)/2.0;
        const auto __k1dotk3         = (__kmode_2*__kmode_2 - __kmode_1*__kmode_1 - __kmode_3*__kmode_3)/2.0;
        const auto __k1dotk2         = (__kmode_3*__kmode_3 - __kmode_1*__kmode_1 - __kmode_2*__kmode_2)/2.0;

        number     __tpf             = 0.0;

        $$__TEMP_POOL{"const auto $1 = $2;"}

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

                // these components are dimension 3, so suppressed by two powers of Mp
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

                // these components are dimension 6, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_1  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __kmode_1*__kmode_2*__kmode_3 * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 4
                     __tpf_1 += - (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*__kmode_3 / 2.0;
                     __tpf_1 += - (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*__kmode_2 / 2.0;
                     __tpf_1 += - (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*__kmode_1 / 2.0;

                __tpf = __prefactor_1 * __tpf_1;

                // this prefactor has dimension 3; the leading minus sign is again switched
                auto __prefactor_2 = __momentum_k*__kmode_1*__kmode_2*__kmode_3 / (__kt * __ainit*__ainit*__ainit*__ainit);

                // these components are dimension 5, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_2  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_2 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_2 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * -(__Ksq + __kmode_1*__kmode_2*__kmode_3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these componennts are dimension 3
                     __tpf_2 += (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_1*__kmode_1*__kmode_2*__kmode_2*(1.0+__kmode_3/__kt) / 2.0;
                     __tpf_2 += (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_1*__kmode_1*__kmode_3*__kmode_3*(1.0+__kmode_2/__kt) / 2.0;
                     __tpf_2 += (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_2*__kmode_2*__kmode_3*__kmode_3*(1.0+__kmode_1/__kt) / 2.0;

                // these components are dimension 3
                     __tpf_2 += (__B_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __B_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)])*__kmode_1*__kmode_1*__kmode_2*__kmode_3 / 2.0;
                     __tpf_2 += (__B_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __B_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)])*__kmode_2*__kmode_2*__kmode_1*__kmode_3 / 2.0;
                     __tpf_2 += (__B_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __B_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)])*__kmode_3*__kmode_3*__kmode_1*__kmode_2 / 2.0;

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

                // these components have dimension 3, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components have dimension 1
                __tpf += - (__C_k1k2k3[SPECIES(__i)][SPECIES(__j)][SPECIES(__k)] + __C_k2k1k3[SPECIES(__j)][SPECIES(__i)][SPECIES(__k)]) * __kmode_1*__kmode_2 / 2.0;
                __tpf += - (__C_k1k3k2[SPECIES(__i)][SPECIES(__k)][SPECIES(__j)] + __C_k3k1k2[SPECIES(__k)][SPECIES(__i)][SPECIES(__j)]) * __kmode_1*__kmode_3 / 2.0;
                __tpf += - (__C_k2k3k1[SPECIES(__j)][SPECIES(__k)][SPECIES(__i)] + __C_k3k2k1[SPECIES(__k)][SPECIES(__j)][SPECIES(__i)]) * __kmode_2*__kmode_3 / 2.0;

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


    // CALCULATE GAUGE TRANSFORMATIONS


    template <typename number>
    void $$__MODEL<number>::compute_gauge_xfm_1(const parameters<number>& __params,
                                                const std::vector<number>& __state,
                                                std::vector<number>& __dN)
      {
        const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __state[$$__A];
        const auto __Mp              = __params.get_Mp();

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __dN.resize(2*$$__NUMBER_FIELDS); // ensure enough space
        __dN[$$__A] = $$__ZETA_XFM_1[A];
      }


    template <typename number>
    void $$__MODEL<number>::compute_gauge_xfm_2(const parameters<number>& __params,
                                                const std::vector<number>& __state,
                                                std::vector< std::vector<number> >& __ddN)
      {
        const auto $$__PARAMETER[1]  = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __state[$$__A];
        const auto __Mp              = __params.get_Mp();

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __ddN.resize(2*$$__NUMBER_FIELDS);
        for(int i = 0; i < 2*$$__NUMBER_FIELDS; i++)
          {
            __ddN[i].resize(2*$$__NUMBER_FIELDS);
          }

        __ddN[$$__A][$$__B] = $$__ZETA_XFM_2[AB];
      }


    // CALCULATE TENSOR QUANTITIES


    template <typename number>
    void $$__MODEL<number>::u2(const parameters<number>& __params,
                               const std::vector<number>& __fields, double __k, double __N,
                               std::vector< std::vector<number> >& __u2)
      {
        const auto $$__PARAMETER[1]       = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A]      = __fields[$$__A];
        const auto __Mp                   = __params.get_Mp();

        const auto __Hsq                  = $$__HUBBLE_SQ;
        const auto __eps                  = $$__EPSILON;
        const auto __a                    = exp(__N);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __u2.resize(2*$$__NUMBER_FIELDS);

        for(int __i = 0; __i < 2*$$__NUMBER_FIELDS; __i++)
          {
            __u2[__i].resize(2*$$__NUMBER_FIELDS);
          }

        __u2[$$__a][$$__b] = $$__U2_PREDEF[ab]{__k, __a, __Hsq, __eps};
      }


    template <typename number>
    void $$__MODEL<number>::u3(const parameters<number>& __params,
                               const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                               std::vector< std::vector< std::vector<number> > >& __u3)
      {
        const auto $$__PARAMETER[1]       = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A]      = __fields[$$__A];
        const auto __Mp                   = __params.get_Mp();

        const auto __Hsq                  = $$__HUBBLE_SQ;
        const auto __eps                  = $$__EPSILON;
        const auto __a                    = exp(__N);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __u3.resize(2*$$__NUMBER_FIELDS);

        for(int __i = 0; __i < 2*$$__NUMBER_FIELDS; __i++)
          {
            __u3[__i].resize(2*$$__NUMBER_FIELDS);
            for(int __j = 0; __j < 2*$$__NUMBER_FIELDS; __j++)
              {
                __u3[__i][__j].resize(2*$$__NUMBER_FIELDS);
              }
          }

        __u3[$$__a][$$__b][$$__c] = $$__U3_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};
      }


    template <typename number>
    void $$__MODEL<number>::A(const parameters<number>& __params,
                              const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                              std::vector< std::vector< std::vector<number> > >& __A)
      {
        const auto $$__PARAMETER[1]       = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A]      = __fields[$$__A];
        const auto __Mp                   = __params.get_Mp();

        const auto __Hsq                  = $$__HUBBLE_SQ;
        const auto __eps                  = $$__EPSILON;
        const auto __a                    = exp(__N);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __A.resize($$__NUMBER_FIELDS);

        for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
          {
            __A[__i].resize($$__NUMBER_FIELDS);
            for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
              {
                __A[__i][__j].resize($$__NUMBER_FIELDS);
              }
          }

        __A[$$__a][$$__b][$$__c] = $$__A_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};
      }


    template <typename number>
    void $$__MODEL<number>::B(const parameters<number>& __params,
                              const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                              std::vector< std::vector< std::vector<number> > >& __B)
      {
        const auto $$__PARAMETER[1]       = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A]      = __fields[$$__A];
        const auto __Mp                   = __params.get_Mp();

        const auto __Hsq                  = $$__HUBBLE_SQ;
        const auto __eps                  = $$__EPSILON;
        const auto __a                    = exp(__N);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __B.resize($$__NUMBER_FIELDS);

        for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
          {
            __B[__i].resize($$__NUMBER_FIELDS);
            for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
              {
                __B[__i][__j].resize($$__NUMBER_FIELDS);
              }
          }

        __B[$$__a][$$__b][$$__c] = $$__B_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};
      }


    template <typename number>
    void $$__MODEL<number>::C(const parameters<number>& __params,
                              const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                              std::vector< std::vector< std::vector<number> > >& __C)
      {
        const auto $$__PARAMETER[1]       = __params.get_vector()[$$__1];
        const auto $$__COORDINATE[A]      = __fields[$$__A];
        const auto __Mp                   = __params.get_Mp();

        const auto __Hsq                  = $$__HUBBLE_SQ;
        const auto __eps                  = $$__EPSILON;
        const auto __a                    = exp(__N);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __C.resize($$__NUMBER_FIELDS);

        for(int __i = 0; __i < $$__NUMBER_FIELDS; __i++)
          {
            __C[__i].resize($$__NUMBER_FIELDS);
            for(int __j = 0; __j < $$__NUMBER_FIELDS; __j++)
              {
                __C[__i][__j].resize($$__NUMBER_FIELDS);
              }
          }

        __C[$$__a][$$__b][$$__c] = $$__C_PREDEF[abc]{__km, __kn, __kr, __a, __Hsq, __eps};
      }


    template <typename number>
    void $$__MODEL<number>::backend_process_backg(const task<number>* tk, typename model<number>::backg_history& solution, bool silent)
      {
        assert(tk != nullptr);

//        if(!silent) this->write_task_data(tk, std::cout, $$__BACKG_ABS_ERR, $$__BACKG_REL_ERR, $$__BACKG_STEP_SIZE, "$$__BACKG_STEPPER");

        solution.clear();
        solution.reserve(tk->get_N_sample_times());

        // set up an observer which writes to this history vector
        $$__MODEL_background_observer<number> obs(solution);

        // set up a functor to evolve this system
        $$__MODEL_background_functor<number> system(tk->get_params());

        auto ics = tk->get_initial_conditions();

        backg_state<number> x($$__MODEL_pool::backg_state_size);
        x[this->flatten($$__A)] = $$// ics[$$__A];

        auto times = tk->get_sample_times();

        using namespace boost::numeric::odeint;
        integrate_times($$__MAKE_BACKG_STEPPER{backg_state<number>}, system, x, times.begin(), times.end(), $$__BACKG_STEP_SIZE, obs);
      }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


    template <typename number>
    void $$__MODEL_background_functor<number>::operator()(const backg_state<number>& __x, backg_state<number>& __dxdt, double __t)
      {
        const auto $$__PARAMETER[1]  = this->params.get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __x[$$__A];
        const auto __Mp              = this->params.get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __dxdt[this->flatten($$__A)] = $$__U1_PREDEF[A]{__Hsq,__eps};
      }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


    template <typename number>
    void $$__MODEL_background_observer<number>::operator()(const backg_state<number>& x, double t)
      {
        this->history.push_back(x);
      }


  }   // namespace transport


#endif  // $$__GUARD
