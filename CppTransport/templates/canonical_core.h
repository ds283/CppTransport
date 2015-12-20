// backend=cpp minver=0.13
//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY $TOOL $VERSION
//
// '$HEADER' generated from '$SOURCE'
// processed on $DATE

#ifndef $GUARD   // avoid multiple inclusion
#define $GUARD

#include <assert.h>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <memory>

#include "boost/numeric/odeint.hpp"
#include "boost/range/algorithm.hpp"

#include "transport-runtime-api/transport.h"


namespace transport
  {

    template <typename number>
    using backg_state = std::vector<number>;

    // Literal data pool
    namespace $MODEL_pool
      {
        static std::vector<std::string> field_names = $FIELD_NAME_LIST;
        static std::vector<std::string> latex_names = $LATEX_NAME_LIST;
        static std::vector<std::string> param_names = $PARAM_NAME_LIST;
        static std::vector<std::string> platx_names = $PLATX_NAME_LIST;
        static std::vector<std::string> state_names = $STATE_NAME_LIST;

        static std::string              name        = "$NAME";
        static std::string              author      = "$AUTHOR";
        static std::string              tag         = "$TAG";
        static std::string              unique_id   = "$UNIQUE_ID";

        constexpr unsigned int backg_size         = (2*$NUMBER_FIELDS);
        constexpr unsigned int twopf_size         = ((2*$NUMBER_FIELDS)*(2*$NUMBER_FIELDS));
        constexpr unsigned int tensor_size        = (4);
        constexpr unsigned int threepf_size       = ((2*$NUMBER_FIELDS)*(2*$NUMBER_FIELDS)*(2*$NUMBER_FIELDS));

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

        constexpr unsigned int u2_size            = ((2*$NUMBER_FIELDS)*(2*$NUMBER_FIELDS));
        constexpr unsigned int u3_size            = ((2*$NUMBER_FIELDS)*(2*$NUMBER_FIELDS)*(2*$NUMBER_FIELDS));
      }


    // FLATTENNING FUNCTIONS


    constexpr unsigned int SPECIES       (unsigned int a)                                 { return flatten_impl::species(a, $NUMBER_FIELDS); }
    constexpr unsigned int MOMENTUM      (unsigned int a)                                 { return flatten_impl::momentum(a, $NUMBER_FIELDS); }
    constexpr unsigned int IS_FIELD      (unsigned int a)                                 { return flatten_impl::is_field(a, $NUMBER_FIELDS); }
    constexpr unsigned int IS_MOMENTUM   (unsigned int a)                                 { return flatten_impl::is_momentum(a, $NUMBER_FIELDS); }

    constexpr unsigned int FLATTEN       (unsigned int a)                                 { return flatten_impl::flatten(a, $NUMBER_FIELDS); }
    constexpr unsigned int FLATTEN       (unsigned int a, unsigned int b)                 { return flatten_impl::flatten(a, b, $NUMBER_FIELDS); }
    constexpr unsigned int FLATTEN       (unsigned int a, unsigned int b, unsigned int c) { return flatten_impl::flatten(a, b, c, $NUMBER_FIELDS); }

    constexpr unsigned int FIELDS_FLATTEN(unsigned int a)                                 { return flatten_impl::fields_flatten(a, $NUMBER_FIELDS); }
    constexpr unsigned int FIELDS_FLATTEN(unsigned int a, unsigned int b)                 { return flatten_impl::fields_flatten(a, b, $NUMBER_FIELDS); }
    constexpr unsigned int FIELDS_FLATTEN(unsigned int a, unsigned int b, unsigned int c) { return flatten_impl::fields_flatten(a, b, c, $NUMBER_FIELDS); }

    constexpr unsigned int TENSOR_FLATTEN(unsigned int a, unsigned int b)                 { return flatten_impl::tensor_flatten(a, b); }

    $PHASE_FLATTEN{FLATTEN}
    $FIELD_FLATTEN{FLATTEN}


    // *********************************************************************************************


    // CLASS FOR $MODEL CORE
    // contains code and functionality shared by all the compute backends (OpenMP, MPI, OpenCL, CUDA, ...)
    // these backends are implemented by classes which inherit from this common core
    template <typename number>
    class $MODEL : public canonical_model<number>
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        $MODEL();

        //! destructor
		    virtual ~$MODEL();


        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        virtual const std::string& get_name() const override { return($MODEL_pool::name); }

        virtual const std::string& get_author() const override { return($MODEL_pool::author); }

        virtual const std::string& get_tag() const override { return($MODEL_pool::tag); }

        virtual unsigned int get_N_fields() const override { return($NUMBER_FIELDS); }

        virtual unsigned int get_N_params() const override { return($NUMBER_PARAMS); }

        virtual const std::vector< std::string >& get_field_names() const override { return($MODEL_pool::field_names); }

        virtual const std::vector< std::string >& get_f_latex_names() const override { return($MODEL_pool::latex_names); }

        virtual const std::vector< std::string >& get_param_names() const override { return($MODEL_pool::param_names); }

        virtual const std::vector< std::string >& get_p_latex_names() const override { return($MODEL_pool::platx_names); }

        virtual const std::vector< std::string >& get_state_names() const override { return($MODEL_pool::state_names); }


        // INDEX FLATTENING FUNCTIONS -- implements an 'abstract_flattener' interface

      public:

        virtual unsigned int flatten(unsigned int a)                                         const override { return(a); };
        virtual unsigned int flatten(unsigned int a, unsigned int b)                         const override { return(2*$NUMBER_FIELDS*a + b); };
        virtual unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)         const override { return(2*$NUMBER_FIELDS*2*$NUMBER_FIELDS*a + 2*$NUMBER_FIELDS*b + c); };

        virtual unsigned int fields_flatten(unsigned int a)                                  const override { return(a); };
        virtual unsigned int fields_flatten(unsigned int a, unsigned int b)                  const override { return($NUMBER_FIELDS*a + b); };
        virtual unsigned int fields_flatten(unsigned int a, unsigned int b, unsigned int c)  const override { return($NUMBER_FIELDS*$NUMBER_FIELDS*a + $NUMBER_FIELDS*b + c); };

        virtual unsigned int tensor_flatten(unsigned int a, unsigned int b)                  const override { return(2*a + b); }


        // INDEX TRAITS -- implements an 'abstract_flattener' interface

      public:

        virtual unsigned int species(unsigned int a)                                         const override { return((a >= $NUMBER_FIELDS) ? a-$NUMBER_FIELDS : a); };
        virtual unsigned int momentum(unsigned int a)                                        const override { return((a >= $NUMBER_FIELDS) ? a : a+$NUMBER_FIELDS); };
        virtual unsigned int is_field(unsigned int a)                                        const override { return(a < $NUMBER_FIELDS); }
        virtual unsigned int is_momentum(unsigned int a)                                     const override { return(a >= $NUMBER_FIELDS && a <= 2*$NUMBER_FIELDS); }


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
        virtual void compute_gauge_xfm_2(const twopf_list_task<number>* __task, const std::vector<number>& __state, double __k, double __k1, double __k2, double __N, std::vector<number>& __ddN) override;

        virtual void compute_deltaN_xfm_1(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector<number>& __dN) override;
        virtual void compute_deltaN_xfm_2(const twopf_list_task<number>* __task, const std::vector<number>& __state, std::vector<number>& __ddN) override;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        virtual void u2(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __k, double __N, std::vector<number>& __u2) override;
        virtual void u3(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector<number>& __u3) override;

        virtual void A(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector<number>& __A) override;
        virtual void B(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector<number>& __B) override;
        virtual void C(const twopf_list_task<number>* __task, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector<number>& __C) override;


        // BACKEND INTERFACE (PARTIAL IMPLEMENTATION -- WE PROVIDE A COMMON BACKGROUND INTEGRATOR)

      public:

        virtual void backend_process_backg(const background_task<number>* tk, typename model<number>::backg_history& solution, bool silent=false) override;

        virtual double compute_end_of_inflation(const integration_task<number>* tk, double search_time=CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH) override;

		    virtual void compute_aH(const twopf_list_task<number>* tk, std::vector<double>& N, std::vector<number>& aH, double largest_k) override;


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


        // INTERNAL DATA

      private:

        number* __A_k1k2k3;
        number* __A_k1k3k2;
        number* __B_k1k2k3;
        number* __B_k1k3k2;
        number* __C_k1k2k3;
        number* __C_k1k3k2;

        number* __A_k2k1k3;
        number* __A_k2k3k1;
        number* __B_k2k1k3;
        number* __B_k2k3k1;
        number* __C_k2k1k3;
        number* __C_k2k3k1;

        number* __A_k3k1k2;
        number* __A_k3k2k1;
        number* __B_k3k1k2;
        number* __B_k3k2k1;
        number* __C_k3k1k2;
        number* __C_k3k2k1;

        number* __dV;
        number* __ddV;
        number* __dddV;

        number* __raw_params;

      };


    // integration - background functor
    template <typename number>
    class $MODEL_background_functor
      {

      public:

        $MODEL_background_functor(const parameters<number>& p)
          : __params(p)
          {
            __Mp = p.get_Mp();
          }

        void set_up_workspace()
          {
            $RESOURCE_RELEASE
            __dV = new number[$NUMBER_FIELDS];
            __raw_params = new number[$NUMBER_PARAMS];

            __raw_params[$1] = __params.get_vector()[$1];
          }

        void close_down_workspace()
          {
            delete[] __dV;
            delete[] __raw_params;
          }

        void operator ()(const backg_state<number>& __x, backg_state<number>& __dxdt, double __t);

      protected:

        const parameters<number>& __params;

        number* __dV;

        number* __raw_params;

        number __Mp;

      };


    // integration - observer object for background only
    template <typename number>
    class $MODEL_background_observer
      {

      public:

        $MODEL_background_observer(typename model<number>::backg_history& h, const time_config_database& t)
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


    // CLASS $MODEL -- CONSTRUCTORS, DESTRUCTORS


    template <typename number>
    $MODEL<number>::$MODEL()
      : canonical_model<number>("$UNIQUE_ID", static_cast<unsigned int>(100*$VERSION))
      {
        __A_k1k2k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __A_k1k3k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k1k2k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k1k3k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k1k2k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k1k3k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        __A_k2k1k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __A_k2k3k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k2k1k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k2k3k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k2k1k3 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k2k3k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        __A_k3k1k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __A_k3k2k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k3k1k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __B_k3k2k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k3k1k2 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        __C_k3k2k1 = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        __dV = new number[$NUMBER_FIELDS];
        __ddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
        __dddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];

        __raw_params = new number[$NUMBER_PARAMS];
      }


    template <typename number>
    $MODEL<number>::~$MODEL()
      {
        delete[] __A_k1k2k3;
        delete[] __A_k1k3k2;
        delete[] __B_k1k2k3;
        delete[] __B_k1k3k2;
        delete[] __C_k1k2k3;
        delete[] __C_k1k3k2;

        delete[] __A_k2k1k3;
        delete[] __A_k2k3k1;
        delete[] __B_k2k1k3;
        delete[] __B_k2k3k1;
        delete[] __C_k2k1k3;
        delete[] __C_k2k3k1;

        delete[] __A_k3k1k2;
        delete[] __A_k3k2k1;
        delete[] __B_k3k1k2;
        delete[] __B_k3k2k1;
        delete[] __C_k3k1k2;
        delete[] __C_k3k2k1;

        delete[] __dV;
        delete[] __ddV;
        delete[] __dddV;

        delete[] __raw_params;
      }


    // INTERFACE: COMPUTE BASIC PHYSICAL QUANTITIES


    template <typename number>
    number $MODEL<number>::H(const parameters<number>& __params, const std::vector<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);

        if(__coords.size() == 2*$NUMBER_FIELDS)
          {
            $RESOURCE_RELEASE
            const auto __Mp = __params.get_Mp();
            const std::vector<number>& __param_vector = __params.get_vector();

            $RESOURCE_PARAMETERS{__params_vector}
            $RESOURCE_COORDINATES{__coords}

            $TEMP_POOL{"const auto $1 = $2;"}

            return std::sqrt($HUBBLE_SQ);
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_ICS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << ")";
            throw std::out_of_range(msg.str());
          }
      }


    template <typename number>
    number $MODEL<number>::epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);

        if(__coords.size() == 2*$NUMBER_FIELDS)
          {
            $RESOURCE_RELEASE
            const auto __Mp = __params.get_Mp();

            $RESOURCE_COORDINATES{__coords}

            $TEMP_POOL{"const auto $1 = $2;"}

            return $EPSILON;
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_ICS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << ")";
            throw std::out_of_range(msg.str());
          }
      }


    template <typename number>
    number $MODEL<number>::V(const parameters<number>& __params, const std::vector<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);

        if(__coords.size() == 2*$NUMBER_FIELDS)
          {
            $RESOURCE_RELEASE
            const auto __Mp = __params.get_Mp();
            const std::vector<number>& __param_vector = __params.get_vector();

            $RESOURCE_PARAMETERS{__param_vector}
            $RESOURCE_COORDINATES{__coords}

            $TEMP_POOL{"const auto $1 = $2;"}

            return $POTENTIAL;
          }
        else
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_ICS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << ")";
            throw std::out_of_range(msg.str());
          }
      }


    template <typename number>
    void $MODEL_compute_dV(number* raw_params, const std::vector<number>& __x, number* dV)
      {
        $RESOURCE_RELEASE
        $RESOURCE_PARAMETERS{raw_params}
        $RESOURCE_COORDINATES{__x}
        $TEMP_POOL{"const auto $1 = $2;"}

        dV[FIELDS_FLATTEN($a)] = $DV[a];
      }


    template <typename number>
    void $MODEL_compute_ddV(number* raw_params, const std::vector<number>& __x, number* ddV)
      {
        $RESOURCE_RELEASE
        $RESOURCE_PARAMETERS{raw_params}
        $RESOURCE_COORDINATES{__x}
        $TEMP_POOL{"const auto $1 = $2;"}

        ddV[FIELDS_FLATTEN($a,$b)] = $DDV[ab];
      }


    template <typename number>
    void $MODEL_compute_dddV(number* raw_params, const std::vector<number>& __x, number* dddV)
      {
        $RESOURCE_RELEASE
        $RESOURCE_PARAMETERS{raw_params}
        $RESOURCE_COORDINATES{__x}
        $TEMP_POOL{"const auto $1 = $2;"}

        dddV[FIELDS_FLATTEN($a,$b,$c)] = $DDDV[abc];
      }


    // Handle initial conditions


    template <typename number>
    void $MODEL<number>::validate_ics(const parameters<number>& __params, const std::vector<number>& __input, std::vector<number>& __output)
      {
        __output.clear();
        __output.reserve(2*$NUMBER_FIELDS);
        __output.insert(__output.end(), __input.begin(), __input.end());

        if(__input.size() == $NUMBER_FIELDS)  // initial conditions for momenta *were not* supplied -- need to compute them
          {
            $RESOURCE_RELEASE

            // supply the missing initial conditions using a slow-roll approximation
            const auto __Mp = __params.get_Mp();

            $RESOURCE_PARAMETERS{__params.get_vector()}
            $RESOURCE_COORDINATES{__input}

            $TEMP_POOL{"const auto $1 = $2;"}

            __output.push_back($SR_VELOCITY[a]);
          }
        else if(__input.size() == 2*$NUMBER_FIELDS)  // initial conditions for momenta *were* supplied
          {
            // need do nothing
          }
        else
          {
            std::ostringstream msg;

            msg << CPPTRANSPORT_WRONG_ICS_A << __input.size() << "]"
                << CPPTRANSPORT_WRONG_ICS_B << $NUMBER_FIELDS
                << CPPTRANSPORT_WRONG_ICS_C << 2*$NUMBER_FIELDS << "]";

            throw std::out_of_range(msg.str());
          }
      }


    // Handle parameters


    template <typename number>
    void $MODEL<number>::validate_params(const std::vector<number>& input, std::vector<number>& output)
      {
        output.clear();

        if(input.size() == $NUMBER_PARAMS)
          {
            output.assign(input.begin(), input.end());
          }
        else
          {
            std::ostringstream msg;

            msg << CPPTRANSPORT_WRONG_PARAMS_A << input.size() << CPPTRANSPORT_WRONG_PARAMS_B << $NUMBER_PARAMS << ")";

            throw std::out_of_range(msg.str());
          }
      }


    // set up initial conditions for the real part of the equal-time two-point function
    // __i,__j  -- label component of the twopf for which we wish to compute initial conditions
    // __k      -- *comoving normalized* wavenumber for which we wish to assign initial conditions
    // __Ninit  -- initial time
    // __fields -- vector of initial conditions for the background fields (or fields+momenta)
    template <typename number>
    number $MODEL<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                               const twopf_list_task<number>* __task, const std::vector<number>& __fields)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

//        $MODEL_compute_dV(__raw_params, __fields, __dV);
//        $MODEL_compute_ddV(__raw_params, __fields, __ddV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}

        $TEMP_POOL{"const auto $1 = $2;"}

        const auto __Hsq = $HUBBLE_SQ;
        const auto __N = std::log(__k / (__a * std::sqrt(__Hsq)));

        number __tpf = 0.0;

//        std::array< std::array<number, $NUMBER_FIELDS>, $NUMBER_FIELDS> __M;
//
//        __M[$a][$b] = $M_PREDEF[ab]{__raw_params, __fields, __dV, __ddV, FLATTEN, FIELDS_FLATTEN};

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
  number $MODEL<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                             const twopf_list_task<number>* __task, const std::vector<number>& __fields)
    {
      $RESOURCE_RELEASE
      __raw_params[$1] = __task->get_params().get_vector()[$1];

      const auto __Mp = __task->get_params().get_Mp();
      const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

//      $MODEL_compute_dV(__raw_params, __fields, __dV);
//      $MODEL_compute_ddV(__raw_params, __fields, __ddV);

      $RESOURCE_PARAMETERS{__raw_params}
      $RESOURCE_COORDINATES{__fields}

      $TEMP_POOL{"const auto $1 = $2;"}

      const auto __Hsq = $HUBBLE_SQ;
      const auto __N = std::log(__k / (__a * std::sqrt(__Hsq)));

      number __tpf = 0.0;

//      std::array< std::array<number, $NUMBER_FIELDS>, $NUMBER_FIELDS> __M;
//
//      __M[$a][$b] = $M_PREDEF[ab]{__raw_params, __fields, __dV, __ddV, FLATTEN, FIELDS_FLATTEN};

      // only the field-momentum and momentum-field correlation functions have imaginary parts
      if(IS_FIELD(__i) && IS_MOMENTUM(__j))
        {
          // LEADING-ORDER INITIAL CONDITION
          auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);

          // NEXT-ORDER INITIAL CONDITION
//            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));

          __tpf = + __leading / (2.0*std::sqrt(__Hsq)*__a*__a*__a);
        }
      else if(IS_MOMENTUM(__i) && IS_FIELD(__j))
        {
          // LEADING-ORDER INITIAL CONDITION
          auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0);

          // NEXT-ORDER INITIAL CONDITION
//            auto __leading = (SPECIES(__i) == SPECIES(__j) ? 1.0 : 0.0) * (1.0 - 2.0*__eps*(1.0-__N));

          __tpf = - __leading / (2.0*std::sqrt(__Hsq)*__a*__a*__a);
        }

      return(__tpf);
    }


    // set up initial conditions for the real part of the equal-time tensor two-point function
    template <typename number>
    number $MODEL<number>::make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                                   const twopf_list_task<number>* __task, const std::vector<number>& __fields)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $TEMP_POOL{"const auto $1 = $2;"}

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}

        const auto __Hsq = $HUBBLE_SQ;
        const auto __N = std::log(__k / (__a * std::sqrt(__Hsq)));

        number __tpf = 0.0;

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
    number $MODEL<number>::make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                                              double __k1, double __k2, double __k3, double __Ninit,
                                              const twopf_list_task<number>* __task, const std::vector<number>& __fields)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $MODEL_compute_dV(__raw_params, __fields, __dV);

        $TEMP_POOL{"const auto $1 = $2;"}

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $RESOURCE_DV{__dV}

        const auto __Hsq = $HUBBLE_SQ;

        const auto __kt      = __k1 + __k2 + __k3;
        const auto __Ksq     = __k1*__k2 + __k1*__k3 + __k2*__k3;
        const auto __kprod3  = 4.0 * __k1*__k1*__k1 * __k2*__k2*__k2 * __k3*__k3*__k3;

        const auto __k2dotk3 = (__k1*__k1 - __k2*__k2 - __k3*__k3)/2.0;
        const auto __k1dotk3 = (__k2*__k2 - __k1*__k1 - __k3*__k3)/2.0;
        const auto __k1dotk2 = (__k3*__k3 - __k1*__k1 - __k2*__k2)/2.0;

        number __tpf = 0.0;

        __B_k1k2k3[FIELDS_FLATTEN($a, $b, $c)] = $B_PREDEF[abc]{__k1, __k2, __k3, __a};
        __B_k1k3k2[FIELDS_FLATTEN($a, $b, $c)] = $B_PREDEF[abc]{__k1, __k3, __k2, __a};
        __C_k1k2k3[FIELDS_FLATTEN($a, $b, $c)] = $C_PREDEF[abc]{__k1, __k2, __k3, __a};
        __C_k1k3k2[FIELDS_FLATTEN($a, $b, $c)] = $C_PREDEF[abc]{__k1, __k3, __k2, __a};

        __B_k2k3k1[FIELDS_FLATTEN($a, $b, $c)] = $B_PREDEF[abc]{__k2, __k3, __k1, __a};
        __B_k2k1k3[FIELDS_FLATTEN($a, $b, $c)] = $B_PREDEF[abc]{__k2, __k1, __k3, __a};
        __C_k2k3k1[FIELDS_FLATTEN($a, $b, $c)] = $C_PREDEF[abc]{__k2, __k3, __k1, __a};
        __C_k2k1k3[FIELDS_FLATTEN($a, $b, $c)] = $C_PREDEF[abc]{__k2, __k1, __k3, __a};

        __B_k3k1k2[FIELDS_FLATTEN($a, $b, $c)] = $B_PREDEF[abc]{__k3, __k1, __k2, __a};
        __B_k3k2k1[FIELDS_FLATTEN($a, $b, $c)] = $B_PREDEF[abc]{__k3, __k2, __k1, __a};
        __C_k3k1k2[FIELDS_FLATTEN($a, $b, $c)] = $C_PREDEF[abc]{__k3, __k1, __k2, __a};
        __C_k3k2k1[FIELDS_FLATTEN($a, $b, $c)] = $C_PREDEF[abc]{__k3, __k2, __k1, __a};

        unsigned int total_fields  = (IS_FIELD(__i)    ? 1 : 0) + (IS_FIELD(__j)    ? 1 : 0) + (IS_FIELD(__k)    ? 1 : 0);
        unsigned int total_momenta = (IS_MOMENTUM(__i) ? 1 : 0) + (IS_MOMENTUM(__j) ? 1 : 0) + (IS_MOMENTUM(__k) ? 1 : 0);

        assert(total_fields + total_momenta == 3);

        switch(total_fields)
          {
            case 3:   // field-field-field correlation function
              {
                assert(total_fields == 3);
                // prefactor here is dimension 5
                auto __prefactor = (__k1*__k1) * (__k2*__k2) * (__k3*__k3) / (__kt * __a*__a*__a*__a);

                // these components are dimension 3, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 1
                __tpf += - (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k2/2.0;
                __tpf += - (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k3/2.0;
                __tpf += - (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k3/2.0;

                __tpf *= __prefactor / __kprod3;
                break;
              }

            case 2:   // field-field-momentum correlation function
              {
                assert(total_fields == 2);

                auto __momentum_k = (IS_MOMENTUM(__i) ? __k1 : 0.0) + (IS_MOMENTUM(__j) ? __k2 : 0.0) + (IS_MOMENTUM(__k) ? __k3 : 0.0);

                // note the leading + sign, switched from written notes, from d/dN = d/(H dt) = d/(aH d\tau) = -\tau d/d\tau
                // this prefactor has dimension 2
                auto __prefactor_1 = __momentum_k*__momentum_k*(__kt-__momentum_k) / (__kt * __a*__a*__a*__a);

                // these components are dimension 6, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_1  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k1*__k2*__k3 * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1*__k2*__k3 * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1*__k2*__k3 * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 4
                     __tpf_1 += - (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k1*__k2*__k2*__k3 / 2.0;
                     __tpf_1 += - (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k1*__k3*__k3*__k2 / 2.0;
                     __tpf_1 += - (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k2*__k3*__k3*__k1 / 2.0;

                __tpf = __prefactor_1 * __tpf_1;

                // this prefactor has dimension 3; the leading minus sign is again switched
                auto __prefactor_2 = __momentum_k*__k1*__k2*__k3 / (__kt * __a*__a*__a*__a);

                // these components are dimension 5, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_2  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * -(__Ksq + __k1*__k2*__k3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_2 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * -(__Ksq + __k1*__k2*__k3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_2 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * -(__Ksq + __k1*__k2*__k3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these componennts are dimension 3
                     __tpf_2 += (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k1*__k2*__k2*(1.0+__k3/__kt) / 2.0;
                     __tpf_2 += (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k1*__k3*__k3*(1.0+__k2/__kt) / 2.0;
                     __tpf_2 += (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k2*__k3*__k3*(1.0+__k1/__kt) / 2.0;

                // these components are dimension 3
                     __tpf_2 += (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k1*__k1*__k2*__k3 / 2.0;
                     __tpf_2 += (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k2*__k2*__k1*__k3 / 2.0;
                     __tpf_2 += (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k3*__k3*__k1*__k2 / 2.0;

                __tpf += __prefactor_2 * __tpf_2;

                __tpf *= (1.0 / __kprod3);
                break;
              }

            case 1:   // field-momentum-momentum correlation function
              {
                assert(total_fields == 1);

                // this prefactor has dimension 3
                auto __prefactor = - (__k1*__k1 * __k2*__k2 * __k3*__k3) / (__kt * __Hsq * __a*__a*__a*__a*__a*__a);

                // this term (really another part of the prefactor -- but shouldn't be symmetrized) has dimension 2)
                auto __mom_factor = (IS_FIELD(__i) ? __k2*__k3 : 0.0) + (IS_FIELD(__j) ? __k1*__k3 : 0.0) + (IS_FIELD(__k) ? __k1*__k2 : 0.0);

                // these components have dimension 3, so suppressed by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components have dimension 1
                __tpf += - (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))]) * __k1*__k2 / 2.0;
                __tpf += - (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))]) * __k1*__k3 / 2.0;
                __tpf += - (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * __k2*__k3 / 2.0;

                __tpf *= __prefactor * __mom_factor / __kprod3;
                break;
              }

            case 0:   // momentum-momentum-momentum correlation function
              {
                assert(total_fields == 0);

                // prefactor is dimension 3; note the leading - sign, switched from written notes, from d/dN = d/(H dt) = d/(aH d\tau) = -\tau d/d\tau
                auto __prefactor = -(__k1*__k1) * (__k2*__k2) * (__k3*__k3) / (__kt * __Hsq * __a*__a*__a*__a*__a*__a);

                // these components are dimension 5, so suppress by two powers of Mp
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * -(__Ksq + __k1*__k2*__k3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * -(__Ksq + __k1*__k2*__k3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * -(__Ksq + __k1*__k2*__k3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 2
                __tpf += (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k1*__k2*__k2*(1.0+__k3/__kt) / 2.0;
                __tpf += (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k1*__k3*__k3*(1.0+__k2/__kt) / 2.0;
                __tpf += (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k2*__k3*__k3*(1.0+__k1/__kt) / 2.0;

                // these components are dimension 2
                __tpf += (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k1*__k1*__k2*__k3/2.0;
                __tpf += (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k2*__k2*__k1*__k3/2.0;
                __tpf += (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k3*__k3*__k1*__k2/2.0;

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
    void $MODEL<number>::compute_gauge_xfm_1(const twopf_list_task<number>* __task,
                                                const std::vector<number>& __state,
                                                std::vector<number>& __dN)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();

        $MODEL_compute_dV(__raw_params, __state, __dV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}

        $TEMP_POOL{"const auto $1 = $2;"}

        __dN[FLATTEN($A)] = $ZETA_XFM_1[A];
      }


    template <typename number>
    void $MODEL<number>::compute_gauge_xfm_2(const twopf_list_task<number>* __task,
                                                const std::vector<number>& __state,
                                                double __k, double __k1, double __k2, double __N,
                                                std::vector<number>& __ddN)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $MODEL_compute_dV(__raw_params, __state, __dV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $RESOURCE_DV{__dV}

        $TEMP_POOL{"const auto $1 = $2;"}

        __ddN[FLATTEN($A,$B)] = $ZETA_XFM_2[AB]{__k, __k1, __k2, __a};
      }


    template <typename number>
    void $MODEL<number>::compute_deltaN_xfm_1(const twopf_list_task<number>* __task,
                                                 const std::vector<number>& __state,
                                                 std::vector<number>& __dN)
	    {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}

        $TEMP_POOL{"const auto $1 = $2;"}

        __dN[FLATTEN($A)] = $DELTAN_XFM_1[A];
	    }


    template <typename number>
    void $MODEL<number>::compute_deltaN_xfm_2(const twopf_list_task<number>* __task,
                                                 const std::vector<number>& __state,
                                                 std::vector<number>& __ddN)
	    {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();

        $MODEL_compute_dV(__raw_params, __state, __dV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}

        $TEMP_POOL{"const auto $1 = $2;"}

        __ddN[FLATTEN($A,$B)] = $DELTAN_XFM_2[AB];
	    }


    // CALCULATE TENSOR QUANTITIES


    template <typename number>
    void $MODEL<number>::u2(const twopf_list_task<number>* __task,
                               const std::vector<number>& __fields, double __k, double __N,
                               std::vector<number>& __u2)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $MODEL_compute_dV(__raw_params, __fields, __dV);
        $MODEL_compute_ddV(__raw_params, __fields, __ddV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $RESOURCE_DV{__dV}
        $RESOURCE_DDV{__ddV}

        $TEMP_POOL{"const auto $1 = $2;"}

        __u2[FLATTEN($A,$B)] = $U2_PREDEF[AB]{__k, __a}};
      }


    template <typename number>
    void $MODEL<number>::u3(const twopf_list_task<number>* __task,
                               const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                               std::vector<number>& __u3)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $MODEL_compute_dV(__raw_params, __fields, __dV);
        $MODEL_compute_ddV(__raw_params, __fields, __ddV);
        $MODEL_compute_dddV(__raw_params, __fields, __dddV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $RESOURCE_DV{__dV}
        $RESOURCE_DDV{__ddV}
        $RESOURCE_DDDV{__dddV}

        $TEMP_POOL{"const auto $1 = $2;"}

        __u3[FLATTEN($A,$B,$C)] = $U3_PREDEF[ABC]{__k1, __k1, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::A(const twopf_list_task<number>* __task,
                              const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                              std::vector<number>& __A)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $MODEL_compute_dV(__raw_params, __fields, __dV);
        $MODEL_compute_ddV(__raw_params, __fields, __ddV);
        $MODEL_compute_dddV(__raw_params, __fields, __dddV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $RESOURCE_DV{__dV}
        $RESOURCE_DDV{__ddV}
        $RESOURCE_DDDV{__dddV}

        $TEMP_POOL{"const auto $1 = $2;"}

        __A[FIELDS_FLATTEN($a,$b,$c)] = $A_PREDEF[abc]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::B(const twopf_list_task<number>* __task,
                              const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                              std::vector<number>& __B)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $MODEL_compute_dV(__raw_params, __fields, __dV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $RESOURCE_DV{__dV}

        $TEMP_POOL{"const auto $1 = $2;"}

        __B[FIELDS_FLATTEN($a,$b,$c)] = $B_PREDEF[abc]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::C(const twopf_list_task<number>* __task,
                              const std::vector<number>& __fields, double __k1, double __k2, double __k3, double __N,
                              std::vector<number>& __C)
      {
        $RESOURCE_RELEASE
        __raw_params[$1] = __task->get_params().get_vector()[$1];
        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $RESOURCE_DV{__dV}

        $TEMP_POOL{"const auto $1 = $2;"}

        __C[FIELDS_FLATTEN($a,$b,$c)] = $C_PREDEF[abc]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::backend_process_backg(const background_task<number>* tk, typename model<number>::backg_history& solution, bool silent)
      {
        assert(tk != nullptr);

        const time_config_database time_db = tk->get_time_config_database();

        solution.clear();
        solution.reserve(time_db.size());

//        if(!silent) this->write_task_data(tk, std::cout, $BACKG_ABS_ERR, $BACKG_REL_ERR, $BACKG_STEP_SIZE, "$BACKG_STEPPER");

        // set up an observer which writes to this history vector
        $MODEL_background_observer<number> obs(solution, time_db);

        // set up a functor to evolve this system
        $MODEL_background_functor<number> system(tk->get_params());
        system.set_up_workspace();

        auto ics = tk->get_ics_vector();

        backg_state<number> x($MODEL_pool::backg_state_size);
        x[FLATTEN($A)] = ics[$A];

        boost::numeric::odeint::integrate_times($MAKE_BACKG_STEPPER{backg_state<number>}, system, x, time_db.value_begin(), time_db.value_end(), $BACKG_STEP_SIZE, obs);
        system.close_down_workspace();
      }


    namespace $MODEL_impl
      {

        template <typename number>
        class EpsilonUnityPredicate
          {
          public:
            EpsilonUnityPredicate(const parameters<number>& p)
              {
                __Mp = p.get_Mp();
              }

            bool operator()(const std::pair< backg_state<number>, double >& __x)
              {
                $RESOURCE_RELEASE
                $TEMP_POOL{"const auto $1 = $2;"}

                $RESOURCE_COORDINATES{__x.first}

                const auto __eps = $EPSILON;

                return (__eps >= 1.0 || __eps < 0.0);
              }

          private:

            //! cache Planck mass
            number __Mp;

          };

      }   // namespace $MODEL_impl


    template <typename number>
    double $MODEL<number>::compute_end_of_inflation(const integration_task<number>* tk, double search_time)
      {
        assert(tk != nullptr);

        // set up a functor to evolve this system
        $MODEL_background_functor<number> system(tk->get_params());
        system.set_up_workspace();

        auto ics = tk->get_ics_vector();

        backg_state<number> x($MODEL_pool::backg_state_size);
        x[FLATTEN($A)] = ics[$A];

		    // find point where epsilon = 1
        auto stepper = $MAKE_BACKG_STEPPER{backg_state<number>};

        auto range = boost::numeric::odeint::make_adaptive_time_range(stepper, system, x, tk->get_N_initial(), tk->get_N_initial()+search_time, $BACKG_STEP_SIZE);

        // returns the first iterator in 'range' for which the predicate EpsilonUnityPredicate() is satisfied
        auto iter = boost::find_if(range, $MODEL_impl::EpsilonUnityPredicate<number>(tk->get_params()));

				if(iter == boost::end(range)) throw end_of_inflation_not_found();

        system.close_down_workspace();

		    return ((*iter).second);
      };


    namespace $MODEL_impl
      {

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
                param_vector = params.get_vector();
                __Mp = params.get_Mp();
              }

            bool operator()(const std::pair< backg_state<number>, double >& __x)
              {
                $RESOURCE_RELEASE
                $TEMP_POOL{"const auto $1 = $2;"}

                $RESOURCE_PARAMETERS{param_vector}
                $RESOURCE_COORDINATES{__x.first}

                const auto __Hsq = $HUBBLE_SQ{;
                const auto __H   = std::sqrt(__Hsq);

                const auto __a   = std::exp(__x.second - this->N_horizon_crossing + this->astar_normalization);

                this->N_vector.push_back(__x.second);
                this->log_aH_vector.push_back(std::log(__a*__H));

                // are we now at a point where we have comfortably covered the horizon crossing time for largest_k?
                if(std::log(largest_k) - std::log(__a*__H) < -0.5) return(true);
                return(false);
              }

          private:

            //! parameters for the model in use
            const parameters<number>& params;

            //! cache parameters vectors
            std::vector<number> param_vector;

            //! cache Planck mass
            number __Mp;

            //! output vector for times N
            std::vector<double>& N_vector;

            //! output vector for values log(aH)
            std::vector<number>& log_aH_vector;

            //! largest k-mode for which we are trying to find a horizon-exit time
            const double largest_k;

            //! time of horizon crossing
            const double N_horizon_crossing;

            //! normalization of ln(a) at horizon crossing time
            const double astar_normalization;

          };

      }   // namespace $MODEL_impl


		template <typename number>
		void $MODEL<number>::compute_aH(const twopf_list_task<number>* tk, std::vector<double>& N, std::vector<number>& log_aH, double largest_k)
			{
				N.clear();
				log_aH.clear();

				// set up a functor to evolve the system
				$MODEL_background_functor<number> system(tk->get_params());
        system.set_up_workspace();

				auto ics = tk->integration_task<number>::get_ics_vector();

				backg_state<number> x($MODEL_pool::backg_state_size);
				x[FLATTEN($A)] = ics[$A];

				auto stepper = $MAKE_BACKG_STEPPER{backg_state<number>};

        double N_range = 0.0;
        bool found_end = false;
        try
          {
            N_range   = tk->get_N_end_of_inflation();
            found_end = true;
          }
        catch (end_of_inflation_not_found& xe)
          {
            // try to fall back on a sensible default
            N_range = tk->get_N_initial() + CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH;
          }

        auto range = boost::numeric::odeint::make_adaptive_time_range(stepper, system, x, tk->get_N_initial(), N_range, $BACKG_STEP_SIZE);

        $MODEL_impl::aHAggregatorPredicate<number> aggregator(tk, N, log_aH, largest_k);

				// step through iterators, finding first point which is comfortably after time when largest_k has left
				// the horizon
				// aggregator writes N and log_aH into the output vectors at each iteration
				auto iter = boost::find_if(range, aggregator);

				// if we got to the end of the range, then we didn't cover all exit times up to largest_k
				// so something has gone wrong
				if(iter == boost::end(range))
					{
            throw failed_to_compute_horizon_exit(tk->get_N_initial(), N_range, found_end, log_aH.size(), (N.size() > 0 ? N.back() : 0.0), (log_aH.size() > 0 ? log_aH.back() : 0.0), largest_k);
					}

        system.close_down_workspace();
			}


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


    template <typename number>
    void $MODEL_background_functor<number>::operator()(const backg_state<number>& __x, backg_state<number>& __dxdt, double __t)
      {
        $RESOURCE_RELEASE
        $MODEL_compute_dV(__raw_params, __x, __dV);

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__x}
        $RESOURCE_DV{__dV}

        $TEMP_POOL{"const auto $1 = $2;"}

        const auto __Hsq = $HUBBLE_SQ;

        // check whether Hsq is positive
        if(__Hsq < 0) throw Hsq_is_negative(__t);

        // check for nan being produced
        if(std::isnan(__x[$A])) throw integration_produced_nan(__t);

        __dxdt[FLATTEN($A)] = $U1_PREDEF[A];
      }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


    template <typename number>
    void $MODEL_background_observer<number>::operator()(const backg_state<number>& x, double t)
      {
        if(this->current_step != this->time_db.record_end() && this->current_step->is_stored())
          {
            this->history.push_back(x);
          }
        this->current_step++;
      }


  };   // namespace transport


#endif  // $GUARD
