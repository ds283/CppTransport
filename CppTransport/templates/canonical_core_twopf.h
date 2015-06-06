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
#include "boost/range/algorithm.hpp"

#include "transport-runtime-api/transport.h"


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
        constexpr unsigned int tensor_size        = (4);
        constexpr unsigned int threepf_size       = ((2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS)*(2*$$__NUMBER_FIELDS));

        constexpr unsigned int backg_start        = 0;
        constexpr unsigned int tensor_start       = backg_start + backg_size;         // for twopf state vector
        constexpr unsigned int tensor_k1_start    = tensor_start;                     // for threepf state vector
        constexpr unsigned int tensor_k2_start    = tensor_k1_start + tensor_size;
        constexpr unsigned int tensor_k3_start    = tensor_k2_start + tensor_size;
        constexpr unsigned int twopf_start        = tensor_k1_start + tensor_size;    // for twopf state vector
        constexpr unsigned int twopf_re_k1_start  = tensor_k3_start + tensor_size;    // for threepf state vector
        constexpr unsigned int twopf_im_k1_start  = twopf_re_k1_start + twopf_size;
        constexpr unsigned int twopf_re_k2_start  = twopf_im_k1_start + twopf_size;
        constexpr unsigned int twopf_im_k2_start  = twopf_re_k2_start + twopf_size;
        constexpr unsigned int twopf_re_k3_start  = twopf_im_k2_start + twopf_size;
        constexpr unsigned int twopf_im_k3_start  = twopf_re_k3_start + twopf_size;
        constexpr unsigned int threepf_start      = twopf_im_k3_start + twopf_size;

        constexpr unsigned int backg_state_size   = backg_size;
        constexpr unsigned int twopf_state_size   = backg_size + tensor_size + twopf_size;
        constexpr unsigned int threepf_state_size = backg_size + 3*tensor_size + 6*twopf_size + threepf_size;

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

        // CONSTRUCTOR, DESTRUCTOR

      public:

        $$__MODEL(instance_manager<number>* mgr);
        ~$$__MODEL() = default;


        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        virtual const std::string& get_name() const override { return($$__MODEL_pool::name); }

        virtual const std::string& get_author() const override { return($$__MODEL_pool::author); }

        virtual const std::string& get_tag() const override { return($$__MODEL_pool::tag); }

        virtual unsigned int get_N_fields() const override { return($$__NUMBER_FIELDS); }

        virtual unsigned int get_N_params() const override { return($$__NUMBER_PARAMS); }

        virtual const std::vector< std::string >& get_field_names() const override { return($$__MODEL_pool::field_names); }

        virtual const std::vector< std::string >& get_f_latex_names() const override { return($$__MODEL_pool::latex_names); }

        virtual const std::vector< std::string >& get_param_names() const override { return($$__MODEL_pool::param_names); }

        virtual const std::vector< std::string >& get_p_latex_names() const override { return($$__MODEL_pool::platx_names); }

        virtual const std::vector< std::string >& get_state_names() const override { return($$__MODEL_pool::state_names); }


        // INDEX FLATTENING FUNCTIONS -- implements an 'abstract_flattener' interface

      public:

        virtual unsigned int flatten(unsigned int a)                                  const override { return(a); };
        virtual unsigned int flatten(unsigned int a, unsigned int b)                  const override { return(2*$$__NUMBER_FIELDS*a + b); };
        virtual unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)  const override { return(2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c); };
        virtual unsigned int tensor_flatten(unsigned int a, unsigned int b)           const override { return(2*a + b); }


        // INDEX TRAITS -- implements an 'abstract_flattener' interface

      public:

        virtual unsigned int species(unsigned int a)      const override { return((a >= $$__NUMBER_FIELDS) ? a-$$__NUMBER_FIELDS : a); };
        virtual unsigned int momentum(unsigned int a)     const override { return((a >= $$__NUMBER_FIELDS) ? a : a+$$__NUMBER_FIELDS); };
        virtual unsigned int is_field(unsigned int a)     const override { return(a < $$__NUMBER_FIELDS); }
        virtual unsigned int is_momentum(unsigned int a)  const override { return(a >= $$__NUMBER_FIELDS && a <= 2*$$__NUMBER_FIELDS); }


        // COMPUTE BASIC PHYSICAL QUANTITIES -- implements a 'model'/'canonical_model' interface

      public:

        // Over-ride functions inherited from 'model'
        virtual number H(const parameters<number>& __params, const std::vector<number>& __coords) const override;
        virtual number epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const override;

        // Over-ride functions inherited from 'canonical_model'
        virtual number V(const parameters<number>& __params, const std::vector<number>& __coords) const override;


        // INITIAL CONDITIONS HANDLING -- implements a 'model' interface

      public:

        virtual void validate_ics(const parameters<number>& p, const std::vector<number>& input, std::vector<number>& output) override;


        // PARAMETER HANDLING -- implements a 'model' interface

      public:

        virtual void validate_params(const std::vector<number>& input, std::vector<number>& output) override;


        // CALCULATE MODEL-SPECIFIC QUANTITIES -- implements a 'model' interface

      public:

        // calculate gauge transformations to zeta
        virtual void compute_gauge_xfm_1(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector<number>& __dN) override;
        virtual void compute_gauge_xfm_2(const twopf_list_task<number>* __task, const std::vector<number>& __state, double __k, double __k1, double __k2, double __N, std::vector< std::vector<number> >& __ddN) override;

        virtual void compute_deltaN_xfm_1(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector<number>& __dN) override;
        virtual void compute_deltaN_xfm_2(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN) override;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        virtual void u2(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __k, double __N, std::vector< std::vector<number> >& __u2) override;
        virtual void u3(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __u3) override;

        virtual void A(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __A) override;
        virtual void B(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __B) override;
        virtual void C(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __C) override;


        // BACKEND INTERFACE (PARTIAL IMPLEMENTATION -- WE PROVIDE A COMMON BACKGROUND INTEGRATOR)

      public:

        virtual void backend_process_backg(const background_task<number>* tk, typename model<number>::backg_history& solution, bool silent=false) override;

        virtual double backend_compute_epsilon_unity(const integration_task<number>* tk, double search_time) override;


        // CALCULATE INITIAL CONDITIONS FOR N-POINT FUNCTIONS

      protected:

        number make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                const twopf_list_task<number>* __task, const std::vector<number>& __fields);

        number make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                const twopf_list_task<number>* __task, const std::vector<number>& __fields);

        number make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                    const twopf_list_task<number>* __task, const std::vector<number>& __fields);

        number make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                               double kmode_1, double kmode_2, double kmode_3, double __Ninit,
                               const twopf_list_task<number>* __task, const std::vector<number>& __fields);

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

      protected:

        const parameters<number> params;

	    };


    // integration - observer object for background only
    template <typename number>
    class $$__MODEL_background_observer
	    {

      public:

        $$__MODEL_background_observer(typename model<number>::backg_history& h, const time_config_database& t)
	        : history(h),
	          time_db(t)
	        {
            current_step = time_db.record_begin();
	        }

        void operator ()(const backg_state<number>& x, double t);

      private:

        typename model<number>::backg_history& history;

        const time_config_database& time_db;

        time_config_database::const_record_iterator current_step;

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
    void $$__MODEL<number>::validate_ics(const parameters<number>& __params, const std::vector<number>& __input, std::vector<number>& __output)
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
    void $$__MODEL<number>::validate_params(const std::vector<number>& input, std::vector<number>& output)
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
    number $$__MODEL<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                               const twopf_list_task<number>* __task, const std::vector<number>& __fields)
	    {
        const auto $$__PARAMETER[1]  = __task->get_params().get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = __task->get_params().get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __a               = exp(__Ninit - __task->get_N_horizon_crossing() + __CPP_TRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        const auto __N               = log(__k/(__a*sqrt(__Hsq)));

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

            __tpf = + __leading                             / (2.0*__k*__a*__a)
	            + __subl*__Hsq                          / (2.0*__k*__k*__k)
	            + __subsubl*__Hsq*__Hsq*__a*__a / (2.0*__k*__k*__k*__k*__k);
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

            __tpf = + __leading                             / (2.0*__k*__a*__a)
	            + __subl*__Hsq                          / (2.0*__k*__k*__k)
	            + __subsubl*__Hsq*__Hsq*__a*__a / (2.0*__k*__k*__k*__k*__k);
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

            __tpf = + __k*__leading   / (2.0*__Hsq*__a*__a*__a*__a)
	            + __subl          / (2.0*__k*__a*__a)
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
    number $$__MODEL<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                               const twopf_list_task<number>* __task, const std::vector<number>& __fields)
	    {
        const auto $$__PARAMETER[1]  = __task->get_params().get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = __task->get_params().get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __a               = exp(__Ninit - __task->get_N_horizon_crossing() + __CPP_TRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        const auto __N               = log(__k/(__a*sqrt(__Hsq)));

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

            __tpf = + __leading / (2.0*sqrt(__Hsq)*__a*__a*__a);
	        }
        else if(IS_MOMENTUM(__i) && IS_FIELD(__j))
	        {
            // LEADING-ORDER INITIAL CONDITION
            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);

            // NEXT-ORDER INITIAL CONDITION
//            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));

            __tpf = - __leading / (2.0*sqrt(__Hsq)*__a*__a*__a);
	        }

        return(__tpf);
	    }


    // set up initial conditions for the real part of the equal-time tensor two-point function
    template <typename number>
    number $$__MODEL<number>::make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                                   const twopf_list_task<number>* __task, const std::vector<number>& __fields)
	    {
        const auto $$__PARAMETER[1]  = __task->get_params().get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = __task->get_params().get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __a               = exp(__Ninit - __task->get_N_horizon_crossing() + __CPP_TRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        const auto __N               = log(__k/(__a*sqrt(__Hsq)));

        number     __tpf             = 0.0;

        $$__TEMP_POOL{"const auto $1 = $2;"}

        if(__i == 0 && __j == 0)                                      // h-h correlation function
	        {
            // LEADING-ORDER INITIAL CONDITION
            __tpf = 1.0 / (__Mp*__Mp*__k*__a*__a);
//            __tpf = 1.0 / (2.0*__k*__a*__a);
	        }
        else if((__i == 0 && __j == 1) || (__i == 1 && __j == 0))     // h-dh or dh-h correlation function
	        {
            // LEADING ORDER INITIAL CONDITION
            __tpf = -1.0 / (__Mp*__Mp*__k*__a*__a);
//            __tpf = -1.0 / (2.0*__k*__a*__a);
	        }
        else if(__i == 1 && __j == 1)                                 // dh-dh correlation function
	        {
            // LEADING ORDER INITIAL CONDITION
            __tpf = __k / (__Mp*__Mp*__Hsq*__a*__a*__a*__a);
//            __tpf = __k / (2.0*__Hsq*__a*__a*__a*__a);
	        }
        else
	        {
            assert(false);
	        }

        return(__tpf);
	    }


    // set up initial conditions for the real part of the equal-time three-point function
    template <typename number>
    number $$__MODEL<number>::make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                                              double __k1, double __k2, double __k3, double __Ninit,
                                              const twopf_list_task<number>* __task, const std::vector<number>& __fields)
	    {
		    return(0.0);
	    }


    // CALCULATE GAUGE TRANSFORMATIONS


    template <typename number>
    void $$__MODEL<number>::compute_gauge_xfm_1(const twopf_list_task<number>* __task,
                                                const std::vector<number>& __state,
                                                std::vector<number>& __dN)
	    {
        const auto $$__PARAMETER[1]  = __task->get_params().get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __state[$$__A];
        const auto __Mp              = __task->get_params().get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __dN.clear();
        __dN.resize(2*$$__NUMBER_FIELDS); // ensure enough space
        __dN[$$__A] = $$__ZETA_XFM_1[A]{__Hsq, __eps};
	    }


    template <typename number>
    void $$__MODEL<number>::compute_gauge_xfm_2(const twopf_list_task<number>* __task,
                                                const std::vector<number>& __state,
                                                double __k, double __k1, double __k2, double __N,
                                                std::vector< std::vector<number> >& __ddN)
	    {
	    }


    template <typename number>
    void $$__MODEL<number>::compute_deltaN_xfm_1(const twopf_list_task<number>* __task,
                                                 const std::vector<number>& __state,
                                                 std::vector<number>& __dN)
	    {
        const auto $$__PARAMETER[1]  = __task->get_params().get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __state[$$__A];
        const auto __Mp              = __task->get_params().get_Mp();

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __dN.clear();
        __dN.resize(2*$$__NUMBER_FIELDS); // ensure enough space
        __dN[$$__A] = $$__DELTAN_XFM_1[A];
	    }


    template <typename number>
    void $$__MODEL<number>::compute_deltaN_xfm_2(const twopf_list_task<number>* __task,
                                                 const std::vector<number>& __state,
                                                 std::vector< std::vector<number> >& __ddN)
	    {
	    }


    // CALCULATE TENSOR QUANTITIES


    template <typename number>
    void $$__MODEL<number>::u2(const twopf_list_task<number>* __task,
                               const std::vector<number>& __fields, double __k, double __N,
                               std::vector< std::vector<number> >& __u2)
	    {
        const auto $$__PARAMETER[1]  = __task->get_params().get_vector()[$$__1];
        const auto $$__COORDINATE[A] = __fields[$$__A];
        const auto __Mp              = __task->get_params().get_Mp();

        const auto __Hsq             = $$__HUBBLE_SQ;
        const auto __eps             = $$__EPSILON;
        const auto __a               = exp(__N - __task->get_N_horizon_crossing() + __CPP_TRANSPORT_DEFAULT_ASTAR_NORMALIZATION);

        $$__TEMP_POOL{"const auto $1 = $2;"}

        __u2.clear();
        __u2.resize(2*$$__NUMBER_FIELDS);

        for(int __i = 0; __i < 2*$$__NUMBER_FIELDS; ++__i)
	        {
            __u2[__i].resize(2*$$__NUMBER_FIELDS);
	        }

        __u2[$$__a][$$__b] = $$__U2_PREDEF[ab]{__k, __a, __Hsq, __eps};
	    }


    template <typename number>
    void $$__MODEL<number>::u3(const twopf_list_task<number>* __task,
                               const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                               std::vector< std::vector< std::vector<number> > >& __u3)
	    {
	    }


    template <typename number>
    void $$__MODEL<number>::A(const twopf_list_task<number>* __task,
                              const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                              std::vector< std::vector< std::vector<number> > >& __A)
	    {
	    }


    template <typename number>
    void $$__MODEL<number>::B(const twopf_list_task<number>* __task,
                              const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                              std::vector< std::vector< std::vector<number> > >& __B)
	    {
	    }


    template <typename number>
    void $$__MODEL<number>::C(const twopf_list_task<number>* __task,
                              const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                              std::vector< std::vector< std::vector<number> > >& __C)
	    {
	    }


    template <typename number>
    void $$__MODEL<number>::backend_process_backg(const background_task<number>* tk, typename model<number>::backg_history& solution, bool silent)
	    {
        assert(tk != nullptr);

        const time_config_database time_db = tk->get_time_config_database();

        solution.clear();
        solution.reserve(time_db.size());

//        if(!silent) this->write_task_data(tk, std::cout, $$__BACKG_ABS_ERR, $$__BACKG_REL_ERR, $$__BACKG_STEP_SIZE, "$$__BACKG_STEPPER");

        // set up an observer which writes to this history vector
        $$__MODEL_background_observer<number> obs(solution, time_db);

        // set up a functor to evolve this system
        $$__MODEL_background_functor<number> system(tk->get_params());

        auto ics = tk->get_ics_vector();

        backg_state<number> x($$__MODEL_pool::backg_state_size);
        x[this->flatten($$__A)] = $$// ics[$$__A];

        boost::numeric::odeint::integrate_times($$__MAKE_BACKG_STEPPER{backg_state<number>}, system, x, time_db.value_begin(), time_db.value_end(), $$__BACKG_STEP_SIZE, obs);
	    }




    template <typename number>
    class EpsilonUnityPredicate
      {
      public:
        EpsilonUnityPredicate(const parameters<number>& p)
          : params(p)
          {
          }

        bool operator()(const std::pair< backg_state<number>, double >& __x)
          {
            const auto $$__PARAMETER[1]  = this->params.get_vector()[$$__1];
            const auto $$__COORDINATE[A] = __x.first[$$__A];
            const auto __Mp              = this->params.get_Mp();

            const auto __eps = $$__EPSILON;

            return (__eps > 1.0);
          }

      private:
        const parameters<number>& params;
      };


    template <typename number>
    double $$__MODEL<number>::compute_end_of_inflation(const integration_task<number>* tk, double search_time)
      {
        assert(tk != nullptr);

        // set up a functor to evolve this system
        $$__MODEL_background_functor<number> system(tk->get_params());

        auto ics = tk->get_ics_vector();

        backg_state<number> x($$__MODEL_pool::backg_state_size);
        x[this->flatten($$__A)] = $$// ics[$$__A];

        // find point where epsilon = 1
        auto stepper = $$__MAKE_BACKG_STEPPER{backg_state<number>};

        auto range = boost::numeric::odeint::make_adaptive_time_range(stepper, system, x, tk->get_N_initial(), tk->get_N_initial()+search_time, $$__BACKG_STEP_SIZE);

        // returns the first iterator in 'range' for which the predicate EpsilonUnityPredicate() is satisfied
        auto iter = boost::find_if(range, EpsilonUnityPredicate<number>(tk->get_params()));

        if(iter == boost::end(range)) throw end_of_inflation_not_found();

        return ((*iter).second);
      };


    template <typename number>
    class aHAggregatorPredicate
      {
      public:
        aHAggregatorPredicate(const twopf_list_task<number>* tk, std::vector<double>& N, std::vector<number>& log_aH, double lk)
          : params(tk->get_params()),
            N_vector(N),
            log_aH_vector(log_aH),
            largest_k(lk),
            N_horizon_crossing(tk->get_N_horizon_crossing()),
            astar_normalization(tk->get_astar_normalization())
          {
          }

        bool operator()(const std::pair< backg_state<number>, double >& __x)
          {
            const auto $$__PARAMETER[1]  = this->params.get_vector()[$$__1];
            const auto $$__COORDINATE[A] = __x.first[$$__A];
            const auto __Mp              = this->params.get_Mp();

            const auto __Hsq = $$__HUBBLE_SQ;
            const auto __H   = sqrt(__Hsq);

            const auto __a   = exp(__x.second - this->N_horizon_crossing + this->astar_normalization);

            this->N_vector.push_back(__x.second);
            this->log_aH_vector.push_back(log(__a*__H));

            // are we now at a point where we have comfortably covered the horizon crossing time for largest_k?
            if(largest_k / (__a*__H) < 0.01) return(true);
            return(false);
          }

      private:
        const parameters<number>& params;
        std::vector<double>& N_vector;
        std::vector<number>& log_aH_vector;
        const double largest_k;
        const double N_horizon_crossing;
        const double astar_normalization;
      };


    template <typename number>
    void $$__MODEL<number>::compute_aH(const twopf_list_task<number>* tk, std::vector<double>& N, std::vector<number>& log_aH, double largest_k)
      {
        N.clear();
        log_aH.clear();

        // set up a functor to evolve the system
        $$__MODEL_background_functor<number> system(tk->get_params());

        auto ics = tk->integration_task<number>::get_ics_vector();

        backg_state<number> x($$__MODEL_pool::backg_state_size);
        x[this->flatten($$__A)] = $$// ics[$$__A];

        auto stepper = $$__MAKE_BACKG_STEPPER{backg_state<number>};

        double N_range = 0.0;
        try
          {
            N_range = tk->get_N_end_of_inflation();
          }
        catch (end_of_inflation_not_found& xe)
          {
            // try to fall back on a sensible default
            N_range = tk->get_N_initial() + __CPP_TRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH;
          }

        auto range = boost::numeric::odeint::make_const_step_time_range(stepper, system, x, tk->get_N_initial(), N_range, 0.01);

        aHAggregatorPredicate<number> aggregator(tk, N, log_aH, largest_k);

        // step through iterators, finding first point which is comfortably after time when largest_k has left
        // the horizon
        // aggregator writes N and log_aH into the output vectors at each iteration
        auto iter = boost::find_if(range, aggregator);

        // if we got to the end of the range, then we didn't cover all exit times up to largest_k
        // so something has gone wrong
        if(iter == boost::end(range))
          {
            assert(false);
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_FAIL_COMPUTE_T_EXIT);
          }
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

        // check whether Hsq is positive
        if(__Hsq < 0) throw Hsq_is_negative();

        $$__TEMP_POOL{"const auto $1 = $2;"}

        // check for nan being produced
        if(std::isnan($$__COORDINATE[A])) throw integration_produced_nan();

        __dxdt[this->flatten($$__A)] = $$__U1_PREDEF[A]{__Hsq,__eps};
	    }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


    template <typename number>
    void $$__MODEL_background_observer<number>::operator()(const backg_state<number>& x, double t)
	    {
        if(this->current_step != this->time_db.record_end() && this->current_step->is_stored())
	        {
            this->history.push_back(x);
	        }
        this->current_step++;
	    }


	};   // namespace transport


#endif  // $$__GUARD
