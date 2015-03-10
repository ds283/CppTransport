//
// DO NOT EDIT: GENERATED AUTOMATICALLY BY CppTransport 0.09
//
// 'quadratic10_basic_twopf.h' generated from 'quadratic10.model'
// processed on 2015-Mar-10 13:19:14

#ifndef __CPPTRANSPORT_QUADRATIC10_CORE_TWOPF_H_   // avoid multiple inclusion
#define __CPPTRANSPORT_QUADRATIC10_CORE_TWOPF_H_

#include <assert.h>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "boost/numeric/odeint.hpp"
#include "transport-runtime-api/transport.h"


namespace transport
	{

    template <typename number>
    using backg_state = std::vector<number>;

    // Literal data pool
    namespace quadratic10_pool
	    {
        static std::vector<std::string> field_names = { "phi1", "phi2", "phi3", "phi4", "phi5", "phi6", "phi7", "phi8", "phi9", "phi10" };
        static std::vector<std::string> latex_names = { "\\phi_1", "\\phi_2", "\\phi_3", "\\phi_4", "\\phi_5", "\\phi_6", "\\phi_7", "\\phi_8", "\\phi_9", "\\phi_{10}" };
        static std::vector<std::string> param_names = { "m1", "m2", "m3", "m4", "m5", "m6", "m7", "m8", "m9", "m10" };
        static std::vector<std::string> platx_names = { "m_1", "m_2", "m_3", "m_4", "m_5", "m_6", "m_7", "m_8", "m_9", "m_{10}" };
        static std::vector<std::string> state_names = { "phi1", "phi2", "phi3", "phi4", "phi5", "phi6", "phi7", "phi8", "phi9", "phi10", "__dphi1", "__dphi2", "__dphi3", "__dphi4", "__dphi5", "__dphi6", "__dphi7", "__dphi8", "__dphi9", "__dphi10" };

        static std::string              name        = "Jonny and Mafalda's Marcenko-Pastur 10-field model";
        static std::string              author      = "David Seery and others 2013-15";
        static std::string              tag         = "please cite arXiv:XXXX.YYYY";
        static std::string              unique_id   = "ffffffff-fffa-fafd-ffff-fafcffffffff";

        constexpr unsigned int backg_size         = (2*10);
        constexpr unsigned int twopf_size         = ((2*10)*(2*10));
        constexpr unsigned int tensor_size        = (4);
        constexpr unsigned int threepf_size       = ((2*10)*(2*10)*(2*10));

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

        constexpr unsigned int u2_size            = ((2*10)*(2*10));
        constexpr unsigned int u3_size            = ((2*10)*(2*10)*(2*10));
	    }


    // *********************************************************************************************


    // CLASS FOR quadratic10 CORE
    // contains code and functionality shared by all the compute backends (OpenMP, MPI, OpenCL, CUDA, ...)
    // these backends are implemented by classes which inherit from this common core
    template <typename number>
    class quadratic10 : public canonical_model<number>
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        quadratic10(instance_manager<number>* mgr);
        ~quadratic10() = default;


        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        virtual const std::string& get_name() const override { return(quadratic10_pool::name); }

        virtual const std::string& get_author() const override { return(quadratic10_pool::author); }

        virtual const std::string& get_tag() const override { return(quadratic10_pool::tag); }

        virtual unsigned int get_N_fields() const override { return(10); }

        virtual unsigned int get_N_params() const override { return(10); }

        virtual const std::vector< std::string >& get_field_names() const override { return(quadratic10_pool::field_names); }

        virtual const std::vector< std::string >& get_f_latex_names() const override { return(quadratic10_pool::latex_names); }

        virtual const std::vector< std::string >& get_param_names() const override { return(quadratic10_pool::param_names); }

        virtual const std::vector< std::string >& get_p_latex_names() const override { return(quadratic10_pool::platx_names); }

        virtual const std::vector< std::string >& get_state_names() const override { return(quadratic10_pool::state_names); }


        // INDEX FLATTENING FUNCTIONS -- implements an 'abstract_flattener' interface

      public:

        virtual unsigned int flatten(unsigned int a)                                  const override { return(a); };
        virtual unsigned int flatten(unsigned int a, unsigned int b)                  const override { return(2*10*a + b); };
        virtual unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)  const override { return(2*10*2*10*a + 2*10*b + c); };
        virtual unsigned int tensor_flatten(unsigned int a, unsigned int b)           const override { return(2*a + b); }


        // INDEX TRAITS -- implements an 'abstract_flattener' interface

      public:

        virtual unsigned int species(unsigned int a)      const override { return((a >= 10) ? a-10 : a); };
        virtual unsigned int momentum(unsigned int a)     const override { return((a >= 10) ? a : a+10); };
        virtual unsigned int is_field(unsigned int a)     const override { return(a < 10); }
        virtual unsigned int is_momentum(unsigned int a)  const override { return(a >= 10 && a <= 2*10); }


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
        virtual void compute_gauge_xfm_1(const parameters<number>& params, const std::vector<number>& __state, std::vector<number>& __dN) override;
        virtual void compute_gauge_xfm_2(const parameters<number>& params, const std::vector<number>& __state, double __k, double __k1, double __k2, double __N, std::vector< std::vector<number> >& __ddN) override;

        virtual void compute_deltaN_xfm_1(const parameters<number>& __params, const std::vector<number>& __state, std::vector<number>& __dN) override;
        virtual void compute_deltaN_xfm_2(const parameters<number>& __params, const std::vector<number>& __state, std::vector< std::vector<number> >& __ddN) override;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        virtual void u2(const parameters<number>& params, const std::vector<number>& __fields, double __k, double __N, std::vector< std::vector<number> >& __u2) override;
        virtual void u3(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __u3) override;

        virtual void A(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __A) override;
        virtual void B(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __B) override;
        virtual void C(const parameters<number>& params, const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N, std::vector< std::vector< std::vector<number> > >& __C) override;


        // BACKEND INTERFACE (PARTIAL IMPLEMENTATION -- WE PROVIDE A COMMON BACKGROUND INTEGRATOR)

      public:

        virtual void backend_process_backg(const background_task<number>* tk, typename model<number>::backg_history& solution, bool silent=false) override;


        // CALCULATE INITIAL CONDITIONS FOR N-POINT FUNCTIONS

      protected:

        number make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const parameters<number>& __params, const std::vector<number>& __fields);

        number make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const parameters<number>& __params, const std::vector<number>& __fields);

        number make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const parameters<number>& __params, const std::vector<number>& __fields);

        number make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                               double kmode_1, double kmode_2, double kmode_3, double __Ninit, const parameters<number>& __params, const std::vector<number>& __fields);
	    };


    // integration - background functor
    template <typename number>
    class quadratic10_background_functor: public constexpr_flattener<10>
	    {

      public:

        quadratic10_background_functor(const parameters<number>& p)
	        : params(p)
	        {
	        }

        void operator ()(const backg_state<number>& __x, backg_state<number>& __dxdt, double __t);

        const parameters<number> params;

	    };


    // integration - observer object for background only
    template <typename number>
    class quadratic10_background_observer
	    {

      public:

        quadratic10_background_observer(typename model<number>::backg_history& h, const std::vector<time_config>& l)
	        : history(h), storage_list(l), serial(0)
	        {
            current_config = storage_list.begin();
	        }

        void operator ()(const backg_state<number>& x, double t);

      private:

        typename model<number>::backg_history& history;

        const std::vector<time_config>& storage_list;

        std::vector<time_config>::const_iterator current_config;

        unsigned int serial;

	    };


    // CLASS quadratic10 -- CONSTRUCTORS, DESTRUCTORS


    template <typename number>
    quadratic10<number>::quadratic10(instance_manager<number>* mgr)
	    : canonical_model<number>(mgr, "ffffffff-fffa-fafd-ffff-fafcffffffff", static_cast<unsigned int>(100*0.09))
	    {
	    }


    // INTERFACE: COMPUTE BASIC PHYSICAL QUANTITIES


    template <typename number>
    number quadratic10<number>::H(const parameters<number>& __params, const std::vector<number>& __coords) const
	    {
        assert(__coords.size() == 2*10);

        if(__coords.size() == 2*10)
	        {
            const auto m1  = __params.get_vector()[0];
            const auto m2  = __params.get_vector()[1];
            const auto m3  = __params.get_vector()[2];
            const auto m4  = __params.get_vector()[3];
            const auto m5  = __params.get_vector()[4];
            const auto m6  = __params.get_vector()[5];
            const auto m7  = __params.get_vector()[6];
            const auto m8  = __params.get_vector()[7];
            const auto m9  = __params.get_vector()[8];
            const auto m10  = __params.get_vector()[9];
            const auto phi1 = __coords[0];
            const auto phi2 = __coords[1];
            const auto phi3 = __coords[2];
            const auto phi4 = __coords[3];
            const auto phi5 = __coords[4];
            const auto phi6 = __coords[5];
            const auto phi7 = __coords[6];
            const auto phi8 = __coords[7];
            const auto phi9 = __coords[8];
            const auto phi10 = __coords[9];
            const auto __dphi1 = __coords[10];
            const auto __dphi2 = __coords[11];
            const auto __dphi3 = __coords[12];
            const auto __dphi4 = __coords[13];
            const auto __dphi5 = __coords[14];
            const auto __dphi6 = __coords[15];
            const auto __dphi7 = __coords[16];
            const auto __dphi8 = __coords[17];
            const auto __dphi9 = __coords[18];
            const auto __dphi10 = __coords[19];
            const auto __Mp              = __params.get_Mp();

// -- START -- temporary pool (sequence=0) 
            // -- END -- temporary pool (sequence=0)

            return(sqrt(-( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0)));
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_ICS_A << __coords.size() << __CPP_TRANSPORT_WRONG_ICS_B << 2*10 << ")";
            throw std::out_of_range(msg.str());
	        }
	    }


    template <typename number>
    number quadratic10<number>::epsilon(const parameters<number>& __params, const std::vector<number>& __coords) const
	    {
        assert(__coords.size() == 2*10);

        if(__coords.size() == 2*10)
	        {
            const auto m1  = __params.get_vector()[0];
            const auto m2  = __params.get_vector()[1];
            const auto m3  = __params.get_vector()[2];
            const auto m4  = __params.get_vector()[3];
            const auto m5  = __params.get_vector()[4];
            const auto m6  = __params.get_vector()[5];
            const auto m7  = __params.get_vector()[6];
            const auto m8  = __params.get_vector()[7];
            const auto m9  = __params.get_vector()[8];
            const auto m10  = __params.get_vector()[9];
            const auto phi1 = __coords[0];
            const auto phi2 = __coords[1];
            const auto phi3 = __coords[2];
            const auto phi4 = __coords[3];
            const auto phi5 = __coords[4];
            const auto phi6 = __coords[5];
            const auto phi7 = __coords[6];
            const auto phi8 = __coords[7];
            const auto phi9 = __coords[8];
            const auto phi10 = __coords[9];
            const auto __dphi1 = __coords[10];
            const auto __dphi2 = __coords[11];
            const auto __dphi3 = __coords[12];
            const auto __dphi4 = __coords[13];
            const auto __dphi5 = __coords[14];
            const auto __dphi6 = __coords[15];
            const auto __dphi7 = __coords[16];
            const auto __dphi8 = __coords[17];
            const auto __dphi9 = __coords[18];
            const auto __dphi10 = __coords[19];
            const auto __Mp              = __params.get_Mp();

// -- START -- temporary pool (sequence=1) 
            // -- END -- temporary pool (sequence=1)

            return((1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9)));
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_ICS_A << __coords.size() << __CPP_TRANSPORT_WRONG_ICS_B << 2*10 << ")";
            throw std::out_of_range(msg.str());
	        }
	    }


    template <typename number>
    number quadratic10<number>::V(const parameters<number>& __params, const std::vector<number>& __coords) const
	    {
        assert(__coords.size() == 2*10);

        if(__coords.size() == 2*10)
	        {
            const auto m1 = __params.get_vector()[0];
            const auto m2 = __params.get_vector()[1];
            const auto m3 = __params.get_vector()[2];
            const auto m4 = __params.get_vector()[3];
            const auto m5 = __params.get_vector()[4];
            const auto m6 = __params.get_vector()[5];
            const auto m7 = __params.get_vector()[6];
            const auto m8 = __params.get_vector()[7];
            const auto m9 = __params.get_vector()[8];
            const auto m10 = __params.get_vector()[9];
            const auto phi1     = __coords[0];
            const auto phi2     = __coords[1];
            const auto phi3     = __coords[2];
            const auto phi4     = __coords[3];
            const auto phi5     = __coords[4];
            const auto phi6     = __coords[5];
            const auto phi7     = __coords[6];
            const auto phi8     = __coords[7];
            const auto phi9     = __coords[8];
            const auto phi10     = __coords[9];
            const auto __Mp             = __params.get_Mp();

// -- START -- temporary pool (sequence=2) 
            // -- END -- temporary pool (sequence=2)

            return  (phi1*phi1)*(m1*m1)/2.0+(phi4*phi4)*(m4*m4)/2.0+(phi9*phi9)*(m9*m9)/2.0+(phi7*phi7)*(m7*m7)/2.0+(m2*m2)*(phi2*phi2)/2.0+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))/2.0+(m6*m6)*(phi6*phi6)/2.0+(phi5*phi5)*(m5*m5)/2.0+(m3*m3)*(phi3*phi3)/2.0+(m8*m8)*(phi8*phi8)/2.0;
	        }
        else
	        {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_WRONG_ICS_A << __coords.size() << __CPP_TRANSPORT_WRONG_ICS_B << 2*10 << ")";
            throw std::out_of_range(msg.str());
	        }
	    }


    // Handle initial conditions


    template <typename number>
    void quadratic10<number>::validate_ics(const parameters<number>& __params, const std::vector<number>& __input, std::vector<number>& __output)
	    {
        __output.clear();
        __output.reserve(2*10);
        __output.insert(__output.end(), __input.begin(), __input.end());

        if(__input.size() == 10)  // initial conditions for momenta *were not* supplied -- need to compute them
	        {
            // supply the missing initial conditions using a slow-roll approximation
            const auto m1 = __params.get_vector()[0];
            const auto m2 = __params.get_vector()[1];
            const auto m3 = __params.get_vector()[2];
            const auto m4 = __params.get_vector()[3];
            const auto m5 = __params.get_vector()[4];
            const auto m6 = __params.get_vector()[5];
            const auto m7 = __params.get_vector()[6];
            const auto m8 = __params.get_vector()[7];
            const auto m9 = __params.get_vector()[8];
            const auto m10 = __params.get_vector()[9];
            const auto phi1     = __input[0];
            const auto phi2     = __input[1];
            const auto phi3     = __input[2];
            const auto phi4     = __input[3];
            const auto phi5     = __input[4];
            const auto phi6     = __input[5];
            const auto phi7     = __input[6];
            const auto phi8     = __input[7];
            const auto phi9     = __input[8];
            const auto phi10     = __input[9];
            const auto __Mp             = __params.get_Mp();

// -- START -- temporary pool (sequence=3) 
const auto __temp_3_0 = phi1;
const auto __temp_3_2 = __temp_3_0*__temp_3_0;
const auto __temp_3_3 = m1;
const auto __temp_3_4 = __temp_3_3*__temp_3_3;
const auto __temp_3_5 = __temp_3_2*__temp_3_4;
const auto __temp_3_6 = phi4;
const auto __temp_3_7 = __temp_3_6*__temp_3_6;
const auto __temp_3_8 = m4;
const auto __temp_3_9 = __temp_3_8*__temp_3_8;
const auto __temp_3_10 = __temp_3_7*__temp_3_9;
const auto __temp_3_11 = phi9;
const auto __temp_3_12 = __temp_3_11*__temp_3_11;
const auto __temp_3_13 = m9;
const auto __temp_3_14 = __temp_3_13*__temp_3_13;
const auto __temp_3_15 = __temp_3_12*__temp_3_14;
const auto __temp_3_16 = phi7;
const auto __temp_3_17 = __temp_3_16*__temp_3_16;
const auto __temp_3_18 = m7;
const auto __temp_3_19 = __temp_3_18*__temp_3_18;
const auto __temp_3_20 = __temp_3_17*__temp_3_19;
const auto __temp_3_21 = m2;
const auto __temp_3_22 = __temp_3_21*__temp_3_21;
const auto __temp_3_23 = phi2;
const auto __temp_3_24 = __temp_3_23*__temp_3_23;
const auto __temp_3_25 = __temp_3_22*__temp_3_24;
const auto __temp_3_26 = phi10;
const auto __temp_3_27 = __temp_3_26*__temp_3_26;
const auto __temp_3_28 = m10;
const auto __temp_3_30 = pow(__temp_3_28,10);
const auto __temp_3_31 = __temp_3_27*__temp_3_30;
const auto __temp_3_32 = m6;
const auto __temp_3_33 = __temp_3_32*__temp_3_32;
const auto __temp_3_34 = phi6;
const auto __temp_3_35 = __temp_3_34*__temp_3_34;
const auto __temp_3_36 = __temp_3_33*__temp_3_35;
const auto __temp_3_37 = phi5;
const auto __temp_3_38 = __temp_3_37*__temp_3_37;
const auto __temp_3_39 = m5;
const auto __temp_3_40 = __temp_3_39*__temp_3_39;
const auto __temp_3_41 = __temp_3_38*__temp_3_40;
const auto __temp_3_42 = m3;
const auto __temp_3_43 = __temp_3_42*__temp_3_42;
const auto __temp_3_44 = phi3;
const auto __temp_3_45 = __temp_3_44*__temp_3_44;
const auto __temp_3_46 = __temp_3_43*__temp_3_45;
const auto __temp_3_47 = m8;
const auto __temp_3_48 = __temp_3_47*__temp_3_47;
const auto __temp_3_49 = phi8;
const auto __temp_3_50 = __temp_3_49*__temp_3_49;
const auto __temp_3_51 = __temp_3_48*__temp_3_50;
const auto __temp_3_52 = __temp_3_5+__temp_3_10+__temp_3_15+__temp_3_20+__temp_3_25+__temp_3_31+__temp_3_36+__temp_3_41+__temp_3_46+__temp_3_51;
const auto __temp_3_54 = 1.0/__temp_3_52;
const auto __temp_3_55 = __Mp;
const auto __temp_3_56 = __temp_3_55*__temp_3_55;
const auto __temp_3_57 = -2.0;
const auto __temp_3_58 = __temp_3_54*__temp_3_56*__temp_3_0*__temp_3_4*__temp_3_57;
const auto __temp_3_59 = __temp_3_54*__temp_3_56*__temp_3_22*__temp_3_23*__temp_3_57;
const auto __temp_3_60 = __temp_3_54*__temp_3_56*__temp_3_43*__temp_3_44*__temp_3_57;
const auto __temp_3_61 = __temp_3_54*__temp_3_56*__temp_3_6*__temp_3_9*__temp_3_57;
const auto __temp_3_62 = __temp_3_54*__temp_3_56*__temp_3_37*__temp_3_40*__temp_3_57;
const auto __temp_3_63 = __temp_3_33*__temp_3_54*__temp_3_56*__temp_3_34*__temp_3_57;
const auto __temp_3_64 = __temp_3_54*__temp_3_56*__temp_3_16*__temp_3_19*__temp_3_57;
const auto __temp_3_65 = __temp_3_54*__temp_3_56*__temp_3_48*__temp_3_49*__temp_3_57;
const auto __temp_3_66 = __temp_3_54*__temp_3_56*__temp_3_11*__temp_3_14*__temp_3_57;
const auto __temp_3_67 = __temp_3_54*__temp_3_56*__temp_3_26*__temp_3_30*__temp_3_57;
            // -- END -- temporary pool (sequence=3)

            __output.push_back(__temp_3_58);
            __output.push_back(__temp_3_59);
            __output.push_back(__temp_3_60);
            __output.push_back(__temp_3_61);
            __output.push_back(__temp_3_62);
            __output.push_back(__temp_3_63);
            __output.push_back(__temp_3_64);
            __output.push_back(__temp_3_65);
            __output.push_back(__temp_3_66);
            __output.push_back(__temp_3_67);
	        }
        else if(__input.size() == 2*10)  // initial conditions for momenta *were* supplied
	        {
            // need do nothing
	        }
        else
	        {
            std::ostringstream msg;

            msg << __CPP_TRANSPORT_WRONG_ICS_A << __input.size() << "]"
	            << __CPP_TRANSPORT_WRONG_ICS_B << 10
	            << __CPP_TRANSPORT_WRONG_ICS_C << 2*10 << "]";

            throw std::out_of_range(msg.str());
	        }
	    }


    // Handle parameters


    template <typename number>
    void quadratic10<number>::validate_params(const std::vector<number>& input, std::vector<number>& output)
	    {
        output.clear();

        if(input.size() == 10)
	        {
            output.assign(input.begin(), input.end());
	        }
        else
	        {
            std::ostringstream msg;

            msg << __CPP_TRANSPORT_WRONG_PARAMS_A << input.size() << __CPP_TRANSPORT_WRONG_PARAMS_B << 10 << ")";

            throw std::out_of_range(msg.str());
	        }
	    }


    // set up initial conditions for the real part of the equal-time two-point function
    // __i,__j  -- label component of the twopf for which we wish to compute initial conditions
    // __k      -- *comoving normalized* wavenumber for which we wish to assign initial conditions
    // __Ninit  -- initial time
    // __fields -- vector of initial conditions for the background fields (or fields+momenta)
    template <typename number>
    number quadratic10<number>::make_twopf_re_ic(unsigned int __i, unsigned int __j,
                                               double __k, double __Ninit,
                                               const parameters<number>& __params,
                                               const std::vector<number>& __fields)
	    {
        const auto m1  = __params.get_vector()[0];
        const auto m2  = __params.get_vector()[1];
        const auto m3  = __params.get_vector()[2];
        const auto m4  = __params.get_vector()[3];
        const auto m5  = __params.get_vector()[4];
        const auto m6  = __params.get_vector()[5];
        const auto m7  = __params.get_vector()[6];
        const auto m8  = __params.get_vector()[7];
        const auto m9  = __params.get_vector()[8];
        const auto m10  = __params.get_vector()[9];
        const auto phi1 = __fields[0];
        const auto phi2 = __fields[1];
        const auto phi3 = __fields[2];
        const auto phi4 = __fields[3];
        const auto phi5 = __fields[4];
        const auto phi6 = __fields[5];
        const auto phi7 = __fields[6];
        const auto phi8 = __fields[7];
        const auto phi9 = __fields[8];
        const auto phi10 = __fields[9];
        const auto __dphi1 = __fields[10];
        const auto __dphi2 = __fields[11];
        const auto __dphi3 = __fields[12];
        const auto __dphi4 = __fields[13];
        const auto __dphi5 = __fields[14];
        const auto __dphi6 = __fields[15];
        const auto __dphi7 = __fields[16];
        const auto __dphi8 = __fields[17];
        const auto __dphi9 = __fields[18];
        const auto __dphi10 = __fields[19];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = -( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0);
        const auto __eps             = (1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9));
        const auto __ainit           = exp(__Ninit);

        const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

        number     __tpf             = 0.0;

// -- START -- temporary pool (sequence=4) 
const auto __temp_4_68 = __eps;
const auto __temp_4_69 = __Hsq;
const auto __temp_4_71 = 1.0/__temp_4_69;
const auto __temp_4_72 = m1;
const auto __temp_4_74 = __temp_4_72*__temp_4_72;
const auto __temp_4_75 = -(1.0/3.0);
const auto __temp_4_76 = __temp_4_71*__temp_4_74*__temp_4_75;
const auto __temp_4_77 = __Mp;
const auto __temp_4_79 = 1.0/(__temp_4_77*__temp_4_77);
const auto __temp_4_80 = -3.0;
const auto __temp_4_81 = __temp_4_68+__temp_4_80;
const auto __temp_4_82 = __dphi1;
const auto __temp_4_83 = __temp_4_82*__temp_4_82;
const auto __temp_4_84 = (1.0/3.0);
const auto __temp_4_85 = __temp_4_79*__temp_4_81*__temp_4_83*__temp_4_84;
const auto __temp_4_86 = phi1;
const auto __temp_4_87 = -(2.0/3.0);
const auto __temp_4_88 = __temp_4_79*__temp_4_86*__temp_4_71*__temp_4_74*__temp_4_82*__temp_4_87;
const auto __temp_4_89 = __temp_4_68+__temp_4_76+__temp_4_85+__temp_4_88;
const auto __temp_4_90 = __dphi2;
const auto __temp_4_91 = __temp_4_79*__temp_4_81*__temp_4_90*__temp_4_82*__temp_4_84;
const auto __temp_4_92 = __temp_4_86*__temp_4_90*__temp_4_74;
const auto __temp_4_93 = m2;
const auto __temp_4_94 = __temp_4_93*__temp_4_93;
const auto __temp_4_95 = phi2;
const auto __temp_4_96 = __temp_4_94*__temp_4_95*__temp_4_82;
const auto __temp_4_97 = __temp_4_92+__temp_4_96;
const auto __temp_4_98 = __temp_4_79*__temp_4_97*__temp_4_71*__temp_4_75;
const auto __temp_4_99 = __temp_4_91+__temp_4_98;
const auto __temp_4_100 = __dphi3;
const auto __temp_4_101 = __temp_4_79*__temp_4_81*__temp_4_100*__temp_4_82*__temp_4_84;
const auto __temp_4_102 = __temp_4_86*__temp_4_100*__temp_4_74;
const auto __temp_4_103 = m3;
const auto __temp_4_104 = __temp_4_103*__temp_4_103;
const auto __temp_4_105 = phi3;
const auto __temp_4_106 = __temp_4_104*__temp_4_105*__temp_4_82;
const auto __temp_4_107 = __temp_4_102+__temp_4_106;
const auto __temp_4_108 = __temp_4_79*__temp_4_71*__temp_4_107*__temp_4_75;
const auto __temp_4_109 = __temp_4_101+__temp_4_108;
const auto __temp_4_110 = __dphi4;
const auto __temp_4_111 = __temp_4_79*__temp_4_81*__temp_4_110*__temp_4_82*__temp_4_84;
const auto __temp_4_112 = phi4;
const auto __temp_4_113 = m4;
const auto __temp_4_114 = __temp_4_113*__temp_4_113;
const auto __temp_4_115 = __temp_4_112*__temp_4_114*__temp_4_82;
const auto __temp_4_116 = __temp_4_86*__temp_4_110*__temp_4_74;
const auto __temp_4_117 = __temp_4_115+__temp_4_116;
const auto __temp_4_118 = __temp_4_117*__temp_4_79*__temp_4_71*__temp_4_75;
const auto __temp_4_119 = __temp_4_111+__temp_4_118;
const auto __temp_4_120 = __dphi5;
const auto __temp_4_121 = __temp_4_86*__temp_4_120*__temp_4_74;
const auto __temp_4_122 = phi5;
const auto __temp_4_123 = m5;
const auto __temp_4_124 = __temp_4_123*__temp_4_123;
const auto __temp_4_125 = __temp_4_122*__temp_4_124*__temp_4_82;
const auto __temp_4_126 = __temp_4_121+__temp_4_125;
const auto __temp_4_127 = __temp_4_79*__temp_4_126*__temp_4_71*__temp_4_75;
const auto __temp_4_128 = __temp_4_79*__temp_4_81*__temp_4_120*__temp_4_82*__temp_4_84;
const auto __temp_4_129 = __temp_4_127+__temp_4_128;
const auto __temp_4_130 = __dphi6;
const auto __temp_4_131 = __temp_4_79*__temp_4_81*__temp_4_130*__temp_4_82*__temp_4_84;
const auto __temp_4_132 = m6;
const auto __temp_4_133 = __temp_4_132*__temp_4_132;
const auto __temp_4_134 = phi6;
const auto __temp_4_135 = __temp_4_133*__temp_4_134*__temp_4_82;
const auto __temp_4_136 = __temp_4_86*__temp_4_130*__temp_4_74;
const auto __temp_4_137 = __temp_4_135+__temp_4_136;
const auto __temp_4_138 = __temp_4_79*__temp_4_137*__temp_4_71*__temp_4_75;
const auto __temp_4_139 = __temp_4_131+__temp_4_138;
const auto __temp_4_140 = phi7;
const auto __temp_4_141 = m7;
const auto __temp_4_142 = __temp_4_141*__temp_4_141;
const auto __temp_4_143 = __temp_4_140*__temp_4_142*__temp_4_82;
const auto __temp_4_144 = __dphi7;
const auto __temp_4_145 = __temp_4_144*__temp_4_86*__temp_4_74;
const auto __temp_4_146 = __temp_4_143+__temp_4_145;
const auto __temp_4_147 = __temp_4_79*__temp_4_146*__temp_4_71*__temp_4_75;
const auto __temp_4_148 = __temp_4_79*__temp_4_81*__temp_4_144*__temp_4_82*__temp_4_84;
const auto __temp_4_149 = __temp_4_147+__temp_4_148;
const auto __temp_4_150 = __dphi8;
const auto __temp_4_151 = __temp_4_150*__temp_4_86*__temp_4_74;
const auto __temp_4_152 = m8;
const auto __temp_4_153 = __temp_4_152*__temp_4_152;
const auto __temp_4_154 = phi8;
const auto __temp_4_155 = __temp_4_153*__temp_4_154*__temp_4_82;
const auto __temp_4_156 = __temp_4_151+__temp_4_155;
const auto __temp_4_157 = __temp_4_79*__temp_4_156*__temp_4_71*__temp_4_75;
const auto __temp_4_158 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_82*__temp_4_84;
const auto __temp_4_159 = __temp_4_157+__temp_4_158;
const auto __temp_4_160 = __dphi9;
const auto __temp_4_161 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_82*__temp_4_84;
const auto __temp_4_162 = __temp_4_160*__temp_4_86*__temp_4_74;
const auto __temp_4_163 = phi9;
const auto __temp_4_164 = m9;
const auto __temp_4_165 = __temp_4_164*__temp_4_164;
const auto __temp_4_166 = __temp_4_163*__temp_4_82*__temp_4_165;
const auto __temp_4_167 = __temp_4_162+__temp_4_166;
const auto __temp_4_168 = __temp_4_79*__temp_4_167*__temp_4_71*__temp_4_75;
const auto __temp_4_169 = __temp_4_161+__temp_4_168;
const auto __temp_4_170 = __dphi10;
const auto __temp_4_171 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_82*__temp_4_84;
const auto __temp_4_172 = phi10;
const auto __temp_4_173 = m10;
const auto __temp_4_175 = pow(__temp_4_173,10);
const auto __temp_4_176 = __temp_4_172*__temp_4_175*__temp_4_82;
const auto __temp_4_177 = __temp_4_170*__temp_4_86*__temp_4_74;
const auto __temp_4_178 = __temp_4_176+__temp_4_177;
const auto __temp_4_179 = __temp_4_79*__temp_4_178*__temp_4_71*__temp_4_75;
const auto __temp_4_180 = __temp_4_171+__temp_4_179;
const auto __temp_4_181 = __temp_4_94*__temp_4_71*__temp_4_75;
const auto __temp_4_182 = __temp_4_79*__temp_4_94*__temp_4_95*__temp_4_90*__temp_4_71*__temp_4_87;
const auto __temp_4_183 = __temp_4_90*__temp_4_90;
const auto __temp_4_184 = __temp_4_79*__temp_4_81*__temp_4_183*__temp_4_84;
const auto __temp_4_185 = __temp_4_68+__temp_4_181+__temp_4_182+__temp_4_184;
const auto __temp_4_186 = __temp_4_79*__temp_4_81*__temp_4_100*__temp_4_90*__temp_4_84;
const auto __temp_4_187 = __temp_4_94*__temp_4_95*__temp_4_100;
const auto __temp_4_188 = __temp_4_104*__temp_4_105*__temp_4_90;
const auto __temp_4_189 = __temp_4_187+__temp_4_188;
const auto __temp_4_190 = __temp_4_79*__temp_4_189*__temp_4_71*__temp_4_75;
const auto __temp_4_191 = __temp_4_186+__temp_4_190;
const auto __temp_4_192 = __temp_4_79*__temp_4_81*__temp_4_110*__temp_4_90*__temp_4_84;
const auto __temp_4_193 = __temp_4_94*__temp_4_95*__temp_4_110;
const auto __temp_4_194 = __temp_4_112*__temp_4_114*__temp_4_90;
const auto __temp_4_195 = __temp_4_193+__temp_4_194;
const auto __temp_4_196 = __temp_4_79*__temp_4_195*__temp_4_71*__temp_4_75;
const auto __temp_4_197 = __temp_4_192+__temp_4_196;
const auto __temp_4_198 = __temp_4_94*__temp_4_95*__temp_4_120;
const auto __temp_4_199 = __temp_4_122*__temp_4_124*__temp_4_90;
const auto __temp_4_200 = __temp_4_198+__temp_4_199;
const auto __temp_4_201 = __temp_4_79*__temp_4_200*__temp_4_71*__temp_4_75;
const auto __temp_4_202 = __temp_4_79*__temp_4_81*__temp_4_120*__temp_4_90*__temp_4_84;
const auto __temp_4_203 = __temp_4_201+__temp_4_202;
const auto __temp_4_204 = __temp_4_79*__temp_4_81*__temp_4_130*__temp_4_90*__temp_4_84;
const auto __temp_4_205 = __temp_4_94*__temp_4_95*__temp_4_130;
const auto __temp_4_206 = __temp_4_133*__temp_4_134*__temp_4_90;
const auto __temp_4_207 = __temp_4_205+__temp_4_206;
const auto __temp_4_208 = __temp_4_79*__temp_4_207*__temp_4_71*__temp_4_75;
const auto __temp_4_209 = __temp_4_204+__temp_4_208;
const auto __temp_4_210 = __temp_4_79*__temp_4_81*__temp_4_144*__temp_4_90*__temp_4_84;
const auto __temp_4_211 = __temp_4_94*__temp_4_95*__temp_4_144;
const auto __temp_4_212 = __temp_4_140*__temp_4_142*__temp_4_90;
const auto __temp_4_213 = __temp_4_211+__temp_4_212;
const auto __temp_4_214 = __temp_4_79*__temp_4_213*__temp_4_71*__temp_4_75;
const auto __temp_4_215 = __temp_4_210+__temp_4_214;
const auto __temp_4_216 = __temp_4_94*__temp_4_150*__temp_4_95;
const auto __temp_4_217 = __temp_4_153*__temp_4_154*__temp_4_90;
const auto __temp_4_218 = __temp_4_216+__temp_4_217;
const auto __temp_4_219 = __temp_4_79*__temp_4_218*__temp_4_71*__temp_4_75;
const auto __temp_4_220 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_90*__temp_4_84;
const auto __temp_4_221 = __temp_4_219+__temp_4_220;
const auto __temp_4_222 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_90*__temp_4_84;
const auto __temp_4_223 = __temp_4_163*__temp_4_90*__temp_4_165;
const auto __temp_4_224 = __temp_4_160*__temp_4_94*__temp_4_95;
const auto __temp_4_225 = __temp_4_223+__temp_4_224;
const auto __temp_4_226 = __temp_4_225*__temp_4_79*__temp_4_71*__temp_4_75;
const auto __temp_4_227 = __temp_4_222+__temp_4_226;
const auto __temp_4_228 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_90*__temp_4_84;
const auto __temp_4_229 = __temp_4_170*__temp_4_94*__temp_4_95;
const auto __temp_4_230 = __temp_4_172*__temp_4_175*__temp_4_90;
const auto __temp_4_231 = __temp_4_229+__temp_4_230;
const auto __temp_4_232 = __temp_4_79*__temp_4_231*__temp_4_71*__temp_4_75;
const auto __temp_4_233 = __temp_4_228+__temp_4_232;
const auto __temp_4_234 = __temp_4_104*__temp_4_71*__temp_4_75;
const auto __temp_4_235 = __temp_4_100*__temp_4_100;
const auto __temp_4_236 = __temp_4_79*__temp_4_81*__temp_4_235*__temp_4_84;
const auto __temp_4_237 = __temp_4_79*__temp_4_104*__temp_4_105*__temp_4_100*__temp_4_71*__temp_4_87;
const auto __temp_4_238 = __temp_4_68+__temp_4_234+__temp_4_236+__temp_4_237;
const auto __temp_4_239 = __temp_4_112*__temp_4_114*__temp_4_100;
const auto __temp_4_240 = __temp_4_104*__temp_4_105*__temp_4_110;
const auto __temp_4_241 = __temp_4_239+__temp_4_240;
const auto __temp_4_242 = __temp_4_79*__temp_4_241*__temp_4_71*__temp_4_75;
const auto __temp_4_243 = __temp_4_79*__temp_4_81*__temp_4_110*__temp_4_100*__temp_4_84;
const auto __temp_4_244 = __temp_4_242+__temp_4_243;
const auto __temp_4_245 = __temp_4_122*__temp_4_124*__temp_4_100;
const auto __temp_4_246 = __temp_4_104*__temp_4_105*__temp_4_120;
const auto __temp_4_247 = __temp_4_245+__temp_4_246;
const auto __temp_4_248 = __temp_4_79*__temp_4_247*__temp_4_71*__temp_4_75;
const auto __temp_4_249 = __temp_4_79*__temp_4_81*__temp_4_120*__temp_4_100*__temp_4_84;
const auto __temp_4_250 = __temp_4_248+__temp_4_249;
const auto __temp_4_251 = __temp_4_104*__temp_4_105*__temp_4_130;
const auto __temp_4_252 = __temp_4_133*__temp_4_134*__temp_4_100;
const auto __temp_4_253 = __temp_4_251+__temp_4_252;
const auto __temp_4_254 = __temp_4_79*__temp_4_253*__temp_4_71*__temp_4_75;
const auto __temp_4_255 = __temp_4_79*__temp_4_81*__temp_4_130*__temp_4_100*__temp_4_84;
const auto __temp_4_256 = __temp_4_254+__temp_4_255;
const auto __temp_4_257 = __temp_4_79*__temp_4_81*__temp_4_144*__temp_4_100*__temp_4_84;
const auto __temp_4_258 = __temp_4_104*__temp_4_105*__temp_4_144;
const auto __temp_4_259 = __temp_4_140*__temp_4_142*__temp_4_100;
const auto __temp_4_260 = __temp_4_258+__temp_4_259;
const auto __temp_4_261 = __temp_4_79*__temp_4_260*__temp_4_71*__temp_4_75;
const auto __temp_4_262 = __temp_4_257+__temp_4_261;
const auto __temp_4_263 = __temp_4_104*__temp_4_105*__temp_4_150;
const auto __temp_4_264 = __temp_4_153*__temp_4_154*__temp_4_100;
const auto __temp_4_265 = __temp_4_263+__temp_4_264;
const auto __temp_4_266 = __temp_4_79*__temp_4_265*__temp_4_71*__temp_4_75;
const auto __temp_4_267 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_100*__temp_4_84;
const auto __temp_4_268 = __temp_4_266+__temp_4_267;
const auto __temp_4_269 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_100*__temp_4_84;
const auto __temp_4_270 = __temp_4_163*__temp_4_100*__temp_4_165;
const auto __temp_4_271 = __temp_4_104*__temp_4_160*__temp_4_105;
const auto __temp_4_272 = __temp_4_270+__temp_4_271;
const auto __temp_4_273 = __temp_4_79*__temp_4_272*__temp_4_71*__temp_4_75;
const auto __temp_4_274 = __temp_4_269+__temp_4_273;
const auto __temp_4_275 = __temp_4_172*__temp_4_175*__temp_4_100;
const auto __temp_4_276 = __temp_4_104*__temp_4_170*__temp_4_105;
const auto __temp_4_277 = __temp_4_275+__temp_4_276;
const auto __temp_4_278 = __temp_4_79*__temp_4_277*__temp_4_71*__temp_4_75;
const auto __temp_4_279 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_100*__temp_4_84;
const auto __temp_4_280 = __temp_4_278+__temp_4_279;
const auto __temp_4_281 = __temp_4_110*__temp_4_110;
const auto __temp_4_282 = __temp_4_79*__temp_4_81*__temp_4_281*__temp_4_84;
const auto __temp_4_283 = __temp_4_114*__temp_4_71*__temp_4_75;
const auto __temp_4_284 = __temp_4_79*__temp_4_112*__temp_4_110*__temp_4_114*__temp_4_71*__temp_4_87;
const auto __temp_4_285 = __temp_4_282+__temp_4_283+__temp_4_68+__temp_4_284;
const auto __temp_4_286 = __temp_4_122*__temp_4_124*__temp_4_110;
const auto __temp_4_287 = __temp_4_112*__temp_4_114*__temp_4_120;
const auto __temp_4_288 = __temp_4_286+__temp_4_287;
const auto __temp_4_289 = __temp_4_79*__temp_4_288*__temp_4_71*__temp_4_75;
const auto __temp_4_290 = __temp_4_79*__temp_4_81*__temp_4_110*__temp_4_120*__temp_4_84;
const auto __temp_4_291 = __temp_4_289+__temp_4_290;
const auto __temp_4_292 = __temp_4_112*__temp_4_130*__temp_4_114;
const auto __temp_4_293 = __temp_4_133*__temp_4_134*__temp_4_110;
const auto __temp_4_294 = __temp_4_292+__temp_4_293;
const auto __temp_4_295 = __temp_4_79*__temp_4_294*__temp_4_71*__temp_4_75;
const auto __temp_4_296 = __temp_4_79*__temp_4_81*__temp_4_130*__temp_4_110*__temp_4_84;
const auto __temp_4_297 = __temp_4_295+__temp_4_296;
const auto __temp_4_298 = __temp_4_112*__temp_4_144*__temp_4_114;
const auto __temp_4_299 = __temp_4_140*__temp_4_142*__temp_4_110;
const auto __temp_4_300 = __temp_4_298+__temp_4_299;
const auto __temp_4_301 = __temp_4_79*__temp_4_300*__temp_4_71*__temp_4_75;
const auto __temp_4_302 = __temp_4_79*__temp_4_81*__temp_4_144*__temp_4_110*__temp_4_84;
const auto __temp_4_303 = __temp_4_301+__temp_4_302;
const auto __temp_4_304 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_110*__temp_4_84;
const auto __temp_4_305 = __temp_4_153*__temp_4_154*__temp_4_110;
const auto __temp_4_306 = __temp_4_112*__temp_4_150*__temp_4_114;
const auto __temp_4_307 = __temp_4_305+__temp_4_306;
const auto __temp_4_308 = __temp_4_307*__temp_4_79*__temp_4_71*__temp_4_75;
const auto __temp_4_309 = __temp_4_304+__temp_4_308;
const auto __temp_4_310 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_110*__temp_4_84;
const auto __temp_4_311 = __temp_4_112*__temp_4_160*__temp_4_114;
const auto __temp_4_312 = __temp_4_163*__temp_4_110*__temp_4_165;
const auto __temp_4_313 = __temp_4_311+__temp_4_312;
const auto __temp_4_314 = __temp_4_79*__temp_4_313*__temp_4_71*__temp_4_75;
const auto __temp_4_315 = __temp_4_310+__temp_4_314;
const auto __temp_4_316 = __temp_4_170*__temp_4_112*__temp_4_114;
const auto __temp_4_317 = __temp_4_172*__temp_4_175*__temp_4_110;
const auto __temp_4_318 = __temp_4_316+__temp_4_317;
const auto __temp_4_319 = __temp_4_79*__temp_4_318*__temp_4_71*__temp_4_75;
const auto __temp_4_320 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_110*__temp_4_84;
const auto __temp_4_321 = __temp_4_319+__temp_4_320;
const auto __temp_4_322 = __temp_4_124*__temp_4_71*__temp_4_75;
const auto __temp_4_323 = __temp_4_79*__temp_4_122*__temp_4_124*__temp_4_120*__temp_4_71*__temp_4_87;
const auto __temp_4_324 = __temp_4_120*__temp_4_120;
const auto __temp_4_325 = __temp_4_79*__temp_4_81*__temp_4_324*__temp_4_84;
const auto __temp_4_326 = __temp_4_68+__temp_4_322+__temp_4_323+__temp_4_325;
const auto __temp_4_327 = __temp_4_79*__temp_4_81*__temp_4_130*__temp_4_120*__temp_4_84;
const auto __temp_4_328 = __temp_4_133*__temp_4_134*__temp_4_120;
const auto __temp_4_329 = __temp_4_122*__temp_4_124*__temp_4_130;
const auto __temp_4_330 = __temp_4_328+__temp_4_329;
const auto __temp_4_331 = __temp_4_79*__temp_4_330*__temp_4_71*__temp_4_75;
const auto __temp_4_332 = __temp_4_327+__temp_4_331;
const auto __temp_4_333 = __temp_4_79*__temp_4_81*__temp_4_144*__temp_4_120*__temp_4_84;
const auto __temp_4_334 = __temp_4_122*__temp_4_124*__temp_4_144;
const auto __temp_4_335 = __temp_4_140*__temp_4_142*__temp_4_120;
const auto __temp_4_336 = __temp_4_334+__temp_4_335;
const auto __temp_4_337 = __temp_4_79*__temp_4_336*__temp_4_71*__temp_4_75;
const auto __temp_4_338 = __temp_4_333+__temp_4_337;
const auto __temp_4_339 = __temp_4_122*__temp_4_124*__temp_4_150;
const auto __temp_4_340 = __temp_4_153*__temp_4_154*__temp_4_120;
const auto __temp_4_341 = __temp_4_339+__temp_4_340;
const auto __temp_4_342 = __temp_4_79*__temp_4_71*__temp_4_341*__temp_4_75;
const auto __temp_4_343 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_120*__temp_4_84;
const auto __temp_4_344 = __temp_4_342+__temp_4_343;
const auto __temp_4_345 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_120*__temp_4_84;
const auto __temp_4_346 = __temp_4_163*__temp_4_120*__temp_4_165;
const auto __temp_4_347 = __temp_4_122*__temp_4_124*__temp_4_160;
const auto __temp_4_348 = __temp_4_346+__temp_4_347;
const auto __temp_4_349 = __temp_4_79*__temp_4_348*__temp_4_71*__temp_4_75;
const auto __temp_4_350 = __temp_4_345+__temp_4_349;
const auto __temp_4_351 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_120*__temp_4_84;
const auto __temp_4_352 = __temp_4_172*__temp_4_175*__temp_4_120;
const auto __temp_4_353 = __temp_4_122*__temp_4_124*__temp_4_170;
const auto __temp_4_354 = __temp_4_352+__temp_4_353;
const auto __temp_4_355 = __temp_4_79*__temp_4_354*__temp_4_71*__temp_4_75;
const auto __temp_4_356 = __temp_4_351+__temp_4_355;
const auto __temp_4_357 = __temp_4_133*__temp_4_71*__temp_4_75;
const auto __temp_4_358 = __temp_4_130*__temp_4_130;
const auto __temp_4_359 = __temp_4_79*__temp_4_81*__temp_4_358*__temp_4_84;
const auto __temp_4_360 = __temp_4_133*__temp_4_79*__temp_4_134*__temp_4_130*__temp_4_71*__temp_4_87;
const auto __temp_4_361 = __temp_4_357+__temp_4_359+__temp_4_68+__temp_4_360;
const auto __temp_4_362 = __temp_4_140*__temp_4_142*__temp_4_130;
const auto __temp_4_363 = __temp_4_133*__temp_4_134*__temp_4_144;
const auto __temp_4_364 = __temp_4_362+__temp_4_363;
const auto __temp_4_365 = __temp_4_79*__temp_4_364*__temp_4_71*__temp_4_75;
const auto __temp_4_366 = __temp_4_79*__temp_4_81*__temp_4_144*__temp_4_130*__temp_4_84;
const auto __temp_4_367 = __temp_4_365+__temp_4_366;
const auto __temp_4_368 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_130*__temp_4_84;
const auto __temp_4_369 = __temp_4_153*__temp_4_154*__temp_4_130;
const auto __temp_4_370 = __temp_4_133*__temp_4_134*__temp_4_150;
const auto __temp_4_371 = __temp_4_369+__temp_4_370;
const auto __temp_4_372 = __temp_4_79*__temp_4_371*__temp_4_71*__temp_4_75;
const auto __temp_4_373 = __temp_4_368+__temp_4_372;
const auto __temp_4_374 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_130*__temp_4_84;
const auto __temp_4_375 = __temp_4_133*__temp_4_134*__temp_4_160;
const auto __temp_4_376 = __temp_4_163*__temp_4_130*__temp_4_165;
const auto __temp_4_377 = __temp_4_375+__temp_4_376;
const auto __temp_4_378 = __temp_4_79*__temp_4_377*__temp_4_71*__temp_4_75;
const auto __temp_4_379 = __temp_4_374+__temp_4_378;
const auto __temp_4_380 = __temp_4_133*__temp_4_134*__temp_4_170;
const auto __temp_4_381 = __temp_4_172*__temp_4_175*__temp_4_130;
const auto __temp_4_382 = __temp_4_380+__temp_4_381;
const auto __temp_4_383 = __temp_4_79*__temp_4_382*__temp_4_71*__temp_4_75;
const auto __temp_4_384 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_130*__temp_4_84;
const auto __temp_4_385 = __temp_4_383+__temp_4_384;
const auto __temp_4_386 = __temp_4_144*__temp_4_144;
const auto __temp_4_387 = __temp_4_79*__temp_4_81*__temp_4_386*__temp_4_84;
const auto __temp_4_388 = __temp_4_79*__temp_4_140*__temp_4_142*__temp_4_144*__temp_4_71*__temp_4_87;
const auto __temp_4_389 = __temp_4_142*__temp_4_71*__temp_4_75;
const auto __temp_4_390 = __temp_4_387+__temp_4_68+__temp_4_388+__temp_4_389;
const auto __temp_4_391 = __temp_4_79*__temp_4_81*__temp_4_150*__temp_4_144*__temp_4_84;
const auto __temp_4_392 = __temp_4_140*__temp_4_150*__temp_4_142;
const auto __temp_4_393 = __temp_4_153*__temp_4_154*__temp_4_144;
const auto __temp_4_394 = __temp_4_392+__temp_4_393;
const auto __temp_4_395 = __temp_4_79*__temp_4_394*__temp_4_71*__temp_4_75;
const auto __temp_4_396 = __temp_4_391+__temp_4_395;
const auto __temp_4_397 = __temp_4_140*__temp_4_160*__temp_4_142;
const auto __temp_4_398 = __temp_4_163*__temp_4_144*__temp_4_165;
const auto __temp_4_399 = __temp_4_397+__temp_4_398;
const auto __temp_4_400 = __temp_4_79*__temp_4_71*__temp_4_399*__temp_4_75;
const auto __temp_4_401 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_144*__temp_4_84;
const auto __temp_4_402 = __temp_4_400+__temp_4_401;
const auto __temp_4_403 = __temp_4_140*__temp_4_170*__temp_4_142;
const auto __temp_4_404 = __temp_4_172*__temp_4_175*__temp_4_144;
const auto __temp_4_405 = __temp_4_403+__temp_4_404;
const auto __temp_4_406 = __temp_4_79*__temp_4_405*__temp_4_71*__temp_4_75;
const auto __temp_4_407 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_144*__temp_4_84;
const auto __temp_4_408 = __temp_4_406+__temp_4_407;
const auto __temp_4_409 = __temp_4_153*__temp_4_71*__temp_4_75;
const auto __temp_4_410 = __temp_4_150*__temp_4_150;
const auto __temp_4_411 = __temp_4_79*__temp_4_81*__temp_4_410*__temp_4_84;
const auto __temp_4_412 = __temp_4_79*__temp_4_153*__temp_4_154*__temp_4_150*__temp_4_71*__temp_4_87;
const auto __temp_4_413 = __temp_4_409+__temp_4_68+__temp_4_411+__temp_4_412;
const auto __temp_4_414 = __temp_4_163*__temp_4_150*__temp_4_165;
const auto __temp_4_415 = __temp_4_153*__temp_4_154*__temp_4_160;
const auto __temp_4_416 = __temp_4_414+__temp_4_415;
const auto __temp_4_417 = __temp_4_79*__temp_4_416*__temp_4_71*__temp_4_75;
const auto __temp_4_418 = __temp_4_79*__temp_4_81*__temp_4_160*__temp_4_150*__temp_4_84;
const auto __temp_4_419 = __temp_4_417+__temp_4_418;
const auto __temp_4_420 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_150*__temp_4_84;
const auto __temp_4_421 = __temp_4_172*__temp_4_150*__temp_4_175;
const auto __temp_4_422 = __temp_4_153*__temp_4_154*__temp_4_170;
const auto __temp_4_423 = __temp_4_421+__temp_4_422;
const auto __temp_4_424 = __temp_4_79*__temp_4_423*__temp_4_71*__temp_4_75;
const auto __temp_4_425 = __temp_4_420+__temp_4_424;
const auto __temp_4_426 = __temp_4_79*__temp_4_163*__temp_4_160*__temp_4_71*__temp_4_165*__temp_4_87;
const auto __temp_4_427 = __temp_4_71*__temp_4_165*__temp_4_75;
const auto __temp_4_428 = __temp_4_160*__temp_4_160;
const auto __temp_4_429 = __temp_4_79*__temp_4_81*__temp_4_428*__temp_4_84;
const auto __temp_4_430 = __temp_4_426+__temp_4_68+__temp_4_427+__temp_4_429;
const auto __temp_4_431 = __temp_4_79*__temp_4_81*__temp_4_170*__temp_4_160*__temp_4_84;
const auto __temp_4_432 = __temp_4_172*__temp_4_160*__temp_4_175;
const auto __temp_4_433 = __temp_4_163*__temp_4_170*__temp_4_165;
const auto __temp_4_434 = __temp_4_432+__temp_4_433;
const auto __temp_4_435 = __temp_4_434*__temp_4_79*__temp_4_71*__temp_4_75;
const auto __temp_4_436 = __temp_4_431+__temp_4_435;
const auto __temp_4_437 = __temp_4_175*__temp_4_71*__temp_4_75;
const auto __temp_4_438 = __temp_4_170*__temp_4_170;
const auto __temp_4_439 = __temp_4_79*__temp_4_81*__temp_4_438*__temp_4_84;
const auto __temp_4_440 = __temp_4_79*__temp_4_172*__temp_4_170*__temp_4_175*__temp_4_71*__temp_4_87;
const auto __temp_4_441 = __temp_4_68+__temp_4_437+__temp_4_439+__temp_4_440;
        // -- END -- temporary pool (sequence=4)

//        std::array< std::array<number, 10>, 10> __M;
//
//        __M[0][0] = __temp_4_89;
//        __M[0][1] = __temp_4_99;
//        __M[0][2] = __temp_4_109;
//        __M[0][3] = __temp_4_119;
//        __M[0][4] = __temp_4_129;
//        __M[0][5] = __temp_4_139;
//        __M[0][6] = __temp_4_149;
//        __M[0][7] = __temp_4_159;
//        __M[0][8] = __temp_4_169;
//        __M[0][9] = __temp_4_180;
//        __M[1][0] = __temp_4_99;
//        __M[1][1] = __temp_4_185;
//        __M[1][2] = __temp_4_191;
//        __M[1][3] = __temp_4_197;
//        __M[1][4] = __temp_4_203;
//        __M[1][5] = __temp_4_209;
//        __M[1][6] = __temp_4_215;
//        __M[1][7] = __temp_4_221;
//        __M[1][8] = __temp_4_227;
//        __M[1][9] = __temp_4_233;
//        __M[2][0] = __temp_4_109;
//        __M[2][1] = __temp_4_191;
//        __M[2][2] = __temp_4_238;
//        __M[2][3] = __temp_4_244;
//        __M[2][4] = __temp_4_250;
//        __M[2][5] = __temp_4_256;
//        __M[2][6] = __temp_4_262;
//        __M[2][7] = __temp_4_268;
//        __M[2][8] = __temp_4_274;
//        __M[2][9] = __temp_4_280;
//        __M[3][0] = __temp_4_119;
//        __M[3][1] = __temp_4_197;
//        __M[3][2] = __temp_4_244;
//        __M[3][3] = __temp_4_285;
//        __M[3][4] = __temp_4_291;
//        __M[3][5] = __temp_4_297;
//        __M[3][6] = __temp_4_303;
//        __M[3][7] = __temp_4_309;
//        __M[3][8] = __temp_4_315;
//        __M[3][9] = __temp_4_321;
//        __M[4][0] = __temp_4_129;
//        __M[4][1] = __temp_4_203;
//        __M[4][2] = __temp_4_250;
//        __M[4][3] = __temp_4_291;
//        __M[4][4] = __temp_4_326;
//        __M[4][5] = __temp_4_332;
//        __M[4][6] = __temp_4_338;
//        __M[4][7] = __temp_4_344;
//        __M[4][8] = __temp_4_350;
//        __M[4][9] = __temp_4_356;
//        __M[5][0] = __temp_4_139;
//        __M[5][1] = __temp_4_209;
//        __M[5][2] = __temp_4_256;
//        __M[5][3] = __temp_4_297;
//        __M[5][4] = __temp_4_332;
//        __M[5][5] = __temp_4_361;
//        __M[5][6] = __temp_4_367;
//        __M[5][7] = __temp_4_373;
//        __M[5][8] = __temp_4_379;
//        __M[5][9] = __temp_4_385;
//        __M[6][0] = __temp_4_149;
//        __M[6][1] = __temp_4_215;
//        __M[6][2] = __temp_4_262;
//        __M[6][3] = __temp_4_303;
//        __M[6][4] = __temp_4_338;
//        __M[6][5] = __temp_4_367;
//        __M[6][6] = __temp_4_390;
//        __M[6][7] = __temp_4_396;
//        __M[6][8] = __temp_4_402;
//        __M[6][9] = __temp_4_408;
//        __M[7][0] = __temp_4_159;
//        __M[7][1] = __temp_4_221;
//        __M[7][2] = __temp_4_268;
//        __M[7][3] = __temp_4_309;
//        __M[7][4] = __temp_4_344;
//        __M[7][5] = __temp_4_373;
//        __M[7][6] = __temp_4_396;
//        __M[7][7] = __temp_4_413;
//        __M[7][8] = __temp_4_419;
//        __M[7][9] = __temp_4_425;
//        __M[8][0] = __temp_4_169;
//        __M[8][1] = __temp_4_227;
//        __M[8][2] = __temp_4_274;
//        __M[8][3] = __temp_4_315;
//        __M[8][4] = __temp_4_350;
//        __M[8][5] = __temp_4_379;
//        __M[8][6] = __temp_4_402;
//        __M[8][7] = __temp_4_419;
//        __M[8][8] = __temp_4_430;
//        __M[8][9] = __temp_4_436;
//        __M[9][0] = __temp_4_180;
//        __M[9][1] = __temp_4_233;
//        __M[9][2] = __temp_4_280;
//        __M[9][3] = __temp_4_321;
//        __M[9][4] = __temp_4_356;
//        __M[9][5] = __temp_4_385;
//        __M[9][6] = __temp_4_408;
//        __M[9][7] = __temp_4_425;
//        __M[9][8] = __temp_4_436;
//        __M[9][9] = __temp_4_441;

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
    number quadratic10<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j,
                                               double __k, double __Ninit,
                                               const parameters<number>& __params,
                                               const std::vector<number>& __fields)
	    {
        const auto m1  = __params.get_vector()[0];
        const auto m2  = __params.get_vector()[1];
        const auto m3  = __params.get_vector()[2];
        const auto m4  = __params.get_vector()[3];
        const auto m5  = __params.get_vector()[4];
        const auto m6  = __params.get_vector()[5];
        const auto m7  = __params.get_vector()[6];
        const auto m8  = __params.get_vector()[7];
        const auto m9  = __params.get_vector()[8];
        const auto m10  = __params.get_vector()[9];
        const auto phi1 = __fields[0];
        const auto phi2 = __fields[1];
        const auto phi3 = __fields[2];
        const auto phi4 = __fields[3];
        const auto phi5 = __fields[4];
        const auto phi6 = __fields[5];
        const auto phi7 = __fields[6];
        const auto phi8 = __fields[7];
        const auto phi9 = __fields[8];
        const auto phi10 = __fields[9];
        const auto __dphi1 = __fields[10];
        const auto __dphi2 = __fields[11];
        const auto __dphi3 = __fields[12];
        const auto __dphi4 = __fields[13];
        const auto __dphi5 = __fields[14];
        const auto __dphi6 = __fields[15];
        const auto __dphi7 = __fields[16];
        const auto __dphi8 = __fields[17];
        const auto __dphi9 = __fields[18];
        const auto __dphi10 = __fields[19];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = -( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0);
        const auto __eps             = (1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9));
        const auto __ainit           = exp(__Ninit);

        const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

        number     __tpf             = 0.0;

// -- START -- temporary pool (sequence=5) 
const auto __temp_5_442 = __Mp;
const auto __temp_5_444 = 1.0/(__temp_5_442*__temp_5_442);
const auto __temp_5_445 = __Hsq;
const auto __temp_5_447 = 1.0/__temp_5_445;
const auto __temp_5_448 = phi1;
const auto __temp_5_449 = m1;
const auto __temp_5_451 = __temp_5_449*__temp_5_449;
const auto __temp_5_452 = __dphi1;
const auto __temp_5_453 = -(2.0/3.0);
const auto __temp_5_454 = __temp_5_444*__temp_5_447*__temp_5_448*__temp_5_451*__temp_5_452*__temp_5_453;
const auto __temp_5_455 = -(1.0/3.0);
const auto __temp_5_456 = __temp_5_447*__temp_5_451*__temp_5_455;
const auto __temp_5_457 = __eps;
const auto __temp_5_458 = -3.0;
const auto __temp_5_459 = __temp_5_457+__temp_5_458;
const auto __temp_5_460 = __temp_5_452*__temp_5_452;
const auto __temp_5_461 = (1.0/3.0);
const auto __temp_5_462 = __temp_5_444*__temp_5_459*__temp_5_460*__temp_5_461;
const auto __temp_5_463 = __temp_5_454+__temp_5_456+__temp_5_457+__temp_5_462;
const auto __temp_5_464 = __dphi2;
const auto __temp_5_465 = __temp_5_448*__temp_5_464*__temp_5_451;
const auto __temp_5_466 = m2;
const auto __temp_5_467 = __temp_5_466*__temp_5_466;
const auto __temp_5_468 = phi2;
const auto __temp_5_469 = __temp_5_467*__temp_5_468*__temp_5_452;
const auto __temp_5_470 = __temp_5_465+__temp_5_469;
const auto __temp_5_471 = __temp_5_444*__temp_5_470*__temp_5_447*__temp_5_455;
const auto __temp_5_472 = __temp_5_444*__temp_5_459*__temp_5_464*__temp_5_452*__temp_5_461;
const auto __temp_5_473 = __temp_5_471+__temp_5_472;
const auto __temp_5_474 = __dphi3;
const auto __temp_5_475 = __temp_5_448*__temp_5_474*__temp_5_451;
const auto __temp_5_476 = m3;
const auto __temp_5_477 = __temp_5_476*__temp_5_476;
const auto __temp_5_478 = phi3;
const auto __temp_5_479 = __temp_5_477*__temp_5_478*__temp_5_452;
const auto __temp_5_480 = __temp_5_475+__temp_5_479;
const auto __temp_5_481 = __temp_5_444*__temp_5_447*__temp_5_480*__temp_5_455;
const auto __temp_5_482 = __temp_5_444*__temp_5_459*__temp_5_474*__temp_5_452*__temp_5_461;
const auto __temp_5_483 = __temp_5_481+__temp_5_482;
const auto __temp_5_484 = phi4;
const auto __temp_5_485 = m4;
const auto __temp_5_486 = __temp_5_485*__temp_5_485;
const auto __temp_5_487 = __temp_5_484*__temp_5_486*__temp_5_452;
const auto __temp_5_488 = __dphi4;
const auto __temp_5_489 = __temp_5_448*__temp_5_488*__temp_5_451;
const auto __temp_5_490 = __temp_5_487+__temp_5_489;
const auto __temp_5_491 = __temp_5_490*__temp_5_444*__temp_5_447*__temp_5_455;
const auto __temp_5_492 = __temp_5_444*__temp_5_488*__temp_5_459*__temp_5_452*__temp_5_461;
const auto __temp_5_493 = __temp_5_491+__temp_5_492;
const auto __temp_5_494 = __dphi5;
const auto __temp_5_495 = __temp_5_444*__temp_5_459*__temp_5_494*__temp_5_452*__temp_5_461;
const auto __temp_5_496 = __temp_5_448*__temp_5_494*__temp_5_451;
const auto __temp_5_497 = phi5;
const auto __temp_5_498 = m5;
const auto __temp_5_499 = __temp_5_498*__temp_5_498;
const auto __temp_5_500 = __temp_5_497*__temp_5_499*__temp_5_452;
const auto __temp_5_501 = __temp_5_496+__temp_5_500;
const auto __temp_5_502 = __temp_5_444*__temp_5_501*__temp_5_447*__temp_5_455;
const auto __temp_5_503 = __temp_5_495+__temp_5_502;
const auto __temp_5_504 = __dphi6;
const auto __temp_5_505 = __temp_5_444*__temp_5_504*__temp_5_459*__temp_5_452*__temp_5_461;
const auto __temp_5_506 = m6;
const auto __temp_5_507 = __temp_5_506*__temp_5_506;
const auto __temp_5_508 = phi6;
const auto __temp_5_509 = __temp_5_507*__temp_5_508*__temp_5_452;
const auto __temp_5_510 = __temp_5_448*__temp_5_504*__temp_5_451;
const auto __temp_5_511 = __temp_5_509+__temp_5_510;
const auto __temp_5_512 = __temp_5_444*__temp_5_447*__temp_5_511*__temp_5_455;
const auto __temp_5_513 = __temp_5_505+__temp_5_512;
const auto __temp_5_514 = __dphi7;
const auto __temp_5_515 = __temp_5_444*__temp_5_514*__temp_5_459*__temp_5_452*__temp_5_461;
const auto __temp_5_516 = phi7;
const auto __temp_5_517 = m7;
const auto __temp_5_518 = __temp_5_517*__temp_5_517;
const auto __temp_5_519 = __temp_5_516*__temp_5_518*__temp_5_452;
const auto __temp_5_520 = __temp_5_514*__temp_5_448*__temp_5_451;
const auto __temp_5_521 = __temp_5_519+__temp_5_520;
const auto __temp_5_522 = __temp_5_444*__temp_5_521*__temp_5_447*__temp_5_455;
const auto __temp_5_523 = __temp_5_515+__temp_5_522;
const auto __temp_5_524 = __dphi8;
const auto __temp_5_525 = __temp_5_524*__temp_5_448*__temp_5_451;
const auto __temp_5_526 = m8;
const auto __temp_5_527 = __temp_5_526*__temp_5_526;
const auto __temp_5_528 = phi8;
const auto __temp_5_529 = __temp_5_527*__temp_5_528*__temp_5_452;
const auto __temp_5_530 = __temp_5_525+__temp_5_529;
const auto __temp_5_531 = __temp_5_444*__temp_5_447*__temp_5_530*__temp_5_455;
const auto __temp_5_532 = __temp_5_444*__temp_5_524*__temp_5_459*__temp_5_452*__temp_5_461;
const auto __temp_5_533 = __temp_5_531+__temp_5_532;
const auto __temp_5_534 = __dphi9;
const auto __temp_5_535 = __temp_5_534*__temp_5_448*__temp_5_451;
const auto __temp_5_536 = phi9;
const auto __temp_5_537 = m9;
const auto __temp_5_538 = __temp_5_537*__temp_5_537;
const auto __temp_5_539 = __temp_5_536*__temp_5_452*__temp_5_538;
const auto __temp_5_540 = __temp_5_535+__temp_5_539;
const auto __temp_5_541 = __temp_5_444*__temp_5_540*__temp_5_447*__temp_5_455;
const auto __temp_5_542 = __temp_5_444*__temp_5_534*__temp_5_459*__temp_5_452*__temp_5_461;
const auto __temp_5_543 = __temp_5_541+__temp_5_542;
const auto __temp_5_544 = __dphi10;
const auto __temp_5_545 = __temp_5_444*__temp_5_544*__temp_5_459*__temp_5_452*__temp_5_461;
const auto __temp_5_546 = phi10;
const auto __temp_5_547 = m10;
const auto __temp_5_549 = pow(__temp_5_547,10);
const auto __temp_5_550 = __temp_5_546*__temp_5_549*__temp_5_452;
const auto __temp_5_551 = __temp_5_544*__temp_5_448*__temp_5_451;
const auto __temp_5_552 = __temp_5_550+__temp_5_551;
const auto __temp_5_553 = __temp_5_444*__temp_5_447*__temp_5_552*__temp_5_455;
const auto __temp_5_554 = __temp_5_545+__temp_5_553;
const auto __temp_5_555 = __temp_5_447*__temp_5_467*__temp_5_455;
const auto __temp_5_556 = __temp_5_464*__temp_5_464;
const auto __temp_5_557 = __temp_5_444*__temp_5_459*__temp_5_556*__temp_5_461;
const auto __temp_5_558 = __temp_5_444*__temp_5_447*__temp_5_467*__temp_5_468*__temp_5_464*__temp_5_453;
const auto __temp_5_559 = __temp_5_555+__temp_5_457+__temp_5_557+__temp_5_558;
const auto __temp_5_560 = __temp_5_444*__temp_5_459*__temp_5_474*__temp_5_464*__temp_5_461;
const auto __temp_5_561 = __temp_5_467*__temp_5_468*__temp_5_474;
const auto __temp_5_562 = __temp_5_477*__temp_5_478*__temp_5_464;
const auto __temp_5_563 = __temp_5_561+__temp_5_562;
const auto __temp_5_564 = __temp_5_444*__temp_5_447*__temp_5_563*__temp_5_455;
const auto __temp_5_565 = __temp_5_560+__temp_5_564;
const auto __temp_5_566 = __temp_5_467*__temp_5_468*__temp_5_488;
const auto __temp_5_567 = __temp_5_484*__temp_5_486*__temp_5_464;
const auto __temp_5_568 = __temp_5_566+__temp_5_567;
const auto __temp_5_569 = __temp_5_444*__temp_5_568*__temp_5_447*__temp_5_455;
const auto __temp_5_570 = __temp_5_444*__temp_5_488*__temp_5_459*__temp_5_464*__temp_5_461;
const auto __temp_5_571 = __temp_5_569+__temp_5_570;
const auto __temp_5_572 = __temp_5_444*__temp_5_459*__temp_5_494*__temp_5_464*__temp_5_461;
const auto __temp_5_573 = __temp_5_467*__temp_5_468*__temp_5_494;
const auto __temp_5_574 = __temp_5_497*__temp_5_499*__temp_5_464;
const auto __temp_5_575 = __temp_5_573+__temp_5_574;
const auto __temp_5_576 = __temp_5_444*__temp_5_575*__temp_5_447*__temp_5_455;
const auto __temp_5_577 = __temp_5_572+__temp_5_576;
const auto __temp_5_578 = __temp_5_444*__temp_5_504*__temp_5_459*__temp_5_464*__temp_5_461;
const auto __temp_5_579 = __temp_5_467*__temp_5_468*__temp_5_504;
const auto __temp_5_580 = __temp_5_507*__temp_5_508*__temp_5_464;
const auto __temp_5_581 = __temp_5_579+__temp_5_580;
const auto __temp_5_582 = __temp_5_444*__temp_5_581*__temp_5_447*__temp_5_455;
const auto __temp_5_583 = __temp_5_578+__temp_5_582;
const auto __temp_5_584 = __temp_5_444*__temp_5_514*__temp_5_459*__temp_5_464*__temp_5_461;
const auto __temp_5_585 = __temp_5_467*__temp_5_468*__temp_5_514;
const auto __temp_5_586 = __temp_5_516*__temp_5_518*__temp_5_464;
const auto __temp_5_587 = __temp_5_585+__temp_5_586;
const auto __temp_5_588 = __temp_5_444*__temp_5_447*__temp_5_587*__temp_5_455;
const auto __temp_5_589 = __temp_5_584+__temp_5_588;
const auto __temp_5_590 = __temp_5_467*__temp_5_524*__temp_5_468;
const auto __temp_5_591 = __temp_5_527*__temp_5_528*__temp_5_464;
const auto __temp_5_592 = __temp_5_590+__temp_5_591;
const auto __temp_5_593 = __temp_5_444*__temp_5_592*__temp_5_447*__temp_5_455;
const auto __temp_5_594 = __temp_5_444*__temp_5_524*__temp_5_459*__temp_5_464*__temp_5_461;
const auto __temp_5_595 = __temp_5_593+__temp_5_594;
const auto __temp_5_596 = __temp_5_536*__temp_5_464*__temp_5_538;
const auto __temp_5_597 = __temp_5_534*__temp_5_467*__temp_5_468;
const auto __temp_5_598 = __temp_5_596+__temp_5_597;
const auto __temp_5_599 = __temp_5_598*__temp_5_444*__temp_5_447*__temp_5_455;
const auto __temp_5_600 = __temp_5_444*__temp_5_534*__temp_5_459*__temp_5_464*__temp_5_461;
const auto __temp_5_601 = __temp_5_599+__temp_5_600;
const auto __temp_5_602 = __temp_5_444*__temp_5_544*__temp_5_459*__temp_5_464*__temp_5_461;
const auto __temp_5_603 = __temp_5_544*__temp_5_467*__temp_5_468;
const auto __temp_5_604 = __temp_5_546*__temp_5_549*__temp_5_464;
const auto __temp_5_605 = __temp_5_603+__temp_5_604;
const auto __temp_5_606 = __temp_5_444*__temp_5_605*__temp_5_447*__temp_5_455;
const auto __temp_5_607 = __temp_5_602+__temp_5_606;
const auto __temp_5_608 = __temp_5_477*__temp_5_447*__temp_5_455;
const auto __temp_5_609 = __temp_5_444*__temp_5_477*__temp_5_478*__temp_5_447*__temp_5_474*__temp_5_453;
const auto __temp_5_610 = __temp_5_474*__temp_5_474;
const auto __temp_5_611 = __temp_5_444*__temp_5_459*__temp_5_610*__temp_5_461;
const auto __temp_5_612 = __temp_5_608+__temp_5_609+__temp_5_611+__temp_5_457;
const auto __temp_5_613 = __temp_5_484*__temp_5_486*__temp_5_474;
const auto __temp_5_614 = __temp_5_477*__temp_5_478*__temp_5_488;
const auto __temp_5_615 = __temp_5_613+__temp_5_614;
const auto __temp_5_616 = __temp_5_444*__temp_5_447*__temp_5_615*__temp_5_455;
const auto __temp_5_617 = __temp_5_444*__temp_5_488*__temp_5_459*__temp_5_474*__temp_5_461;
const auto __temp_5_618 = __temp_5_616+__temp_5_617;
const auto __temp_5_619 = __temp_5_444*__temp_5_459*__temp_5_494*__temp_5_474*__temp_5_461;
const auto __temp_5_620 = __temp_5_497*__temp_5_499*__temp_5_474;
const auto __temp_5_621 = __temp_5_477*__temp_5_478*__temp_5_494;
const auto __temp_5_622 = __temp_5_620+__temp_5_621;
const auto __temp_5_623 = __temp_5_444*__temp_5_622*__temp_5_447*__temp_5_455;
const auto __temp_5_624 = __temp_5_619+__temp_5_623;
const auto __temp_5_625 = __temp_5_477*__temp_5_478*__temp_5_504;
const auto __temp_5_626 = __temp_5_507*__temp_5_508*__temp_5_474;
const auto __temp_5_627 = __temp_5_625+__temp_5_626;
const auto __temp_5_628 = __temp_5_444*__temp_5_447*__temp_5_627*__temp_5_455;
const auto __temp_5_629 = __temp_5_444*__temp_5_504*__temp_5_459*__temp_5_474*__temp_5_461;
const auto __temp_5_630 = __temp_5_628+__temp_5_629;
const auto __temp_5_631 = __temp_5_444*__temp_5_514*__temp_5_459*__temp_5_474*__temp_5_461;
const auto __temp_5_632 = __temp_5_477*__temp_5_478*__temp_5_514;
const auto __temp_5_633 = __temp_5_516*__temp_5_518*__temp_5_474;
const auto __temp_5_634 = __temp_5_632+__temp_5_633;
const auto __temp_5_635 = __temp_5_444*__temp_5_634*__temp_5_447*__temp_5_455;
const auto __temp_5_636 = __temp_5_631+__temp_5_635;
const auto __temp_5_637 = __temp_5_477*__temp_5_478*__temp_5_524;
const auto __temp_5_638 = __temp_5_527*__temp_5_528*__temp_5_474;
const auto __temp_5_639 = __temp_5_637+__temp_5_638;
const auto __temp_5_640 = __temp_5_444*__temp_5_447*__temp_5_639*__temp_5_455;
const auto __temp_5_641 = __temp_5_444*__temp_5_524*__temp_5_459*__temp_5_474*__temp_5_461;
const auto __temp_5_642 = __temp_5_640+__temp_5_641;
const auto __temp_5_643 = __temp_5_444*__temp_5_534*__temp_5_459*__temp_5_474*__temp_5_461;
const auto __temp_5_644 = __temp_5_536*__temp_5_474*__temp_5_538;
const auto __temp_5_645 = __temp_5_477*__temp_5_534*__temp_5_478;
const auto __temp_5_646 = __temp_5_644+__temp_5_645;
const auto __temp_5_647 = __temp_5_444*__temp_5_646*__temp_5_447*__temp_5_455;
const auto __temp_5_648 = __temp_5_643+__temp_5_647;
const auto __temp_5_649 = __temp_5_444*__temp_5_544*__temp_5_459*__temp_5_474*__temp_5_461;
const auto __temp_5_650 = __temp_5_546*__temp_5_549*__temp_5_474;
const auto __temp_5_651 = __temp_5_477*__temp_5_544*__temp_5_478;
const auto __temp_5_652 = __temp_5_650+__temp_5_651;
const auto __temp_5_653 = __temp_5_444*__temp_5_652*__temp_5_447*__temp_5_455;
const auto __temp_5_654 = __temp_5_649+__temp_5_653;
const auto __temp_5_655 = __temp_5_447*__temp_5_486*__temp_5_455;
const auto __temp_5_656 = __temp_5_444*__temp_5_484*__temp_5_447*__temp_5_488*__temp_5_486*__temp_5_453;
const auto __temp_5_657 = __temp_5_488*__temp_5_488;
const auto __temp_5_658 = __temp_5_444*__temp_5_657*__temp_5_459*__temp_5_461;
const auto __temp_5_659 = __temp_5_655+__temp_5_656+__temp_5_457+__temp_5_658;
const auto __temp_5_660 = __temp_5_444*__temp_5_488*__temp_5_459*__temp_5_494*__temp_5_461;
const auto __temp_5_661 = __temp_5_497*__temp_5_499*__temp_5_488;
const auto __temp_5_662 = __temp_5_484*__temp_5_486*__temp_5_494;
const auto __temp_5_663 = __temp_5_661+__temp_5_662;
const auto __temp_5_664 = __temp_5_444*__temp_5_447*__temp_5_663*__temp_5_455;
const auto __temp_5_665 = __temp_5_660+__temp_5_664;
const auto __temp_5_666 = __temp_5_444*__temp_5_504*__temp_5_488*__temp_5_459*__temp_5_461;
const auto __temp_5_667 = __temp_5_484*__temp_5_504*__temp_5_486;
const auto __temp_5_668 = __temp_5_507*__temp_5_508*__temp_5_488;
const auto __temp_5_669 = __temp_5_667+__temp_5_668;
const auto __temp_5_670 = __temp_5_444*__temp_5_669*__temp_5_447*__temp_5_455;
const auto __temp_5_671 = __temp_5_666+__temp_5_670;
const auto __temp_5_672 = __temp_5_484*__temp_5_514*__temp_5_486;
const auto __temp_5_673 = __temp_5_516*__temp_5_518*__temp_5_488;
const auto __temp_5_674 = __temp_5_672+__temp_5_673;
const auto __temp_5_675 = __temp_5_444*__temp_5_674*__temp_5_447*__temp_5_455;
const auto __temp_5_676 = __temp_5_444*__temp_5_514*__temp_5_488*__temp_5_459*__temp_5_461;
const auto __temp_5_677 = __temp_5_675+__temp_5_676;
const auto __temp_5_678 = __temp_5_444*__temp_5_524*__temp_5_488*__temp_5_459*__temp_5_461;
const auto __temp_5_679 = __temp_5_527*__temp_5_528*__temp_5_488;
const auto __temp_5_680 = __temp_5_484*__temp_5_524*__temp_5_486;
const auto __temp_5_681 = __temp_5_679+__temp_5_680;
const auto __temp_5_682 = __temp_5_681*__temp_5_444*__temp_5_447*__temp_5_455;
const auto __temp_5_683 = __temp_5_678+__temp_5_682;
const auto __temp_5_684 = __temp_5_484*__temp_5_534*__temp_5_486;
const auto __temp_5_685 = __temp_5_536*__temp_5_488*__temp_5_538;
const auto __temp_5_686 = __temp_5_684+__temp_5_685;
const auto __temp_5_687 = __temp_5_444*__temp_5_447*__temp_5_686*__temp_5_455;
const auto __temp_5_688 = __temp_5_444*__temp_5_534*__temp_5_488*__temp_5_459*__temp_5_461;
const auto __temp_5_689 = __temp_5_687+__temp_5_688;
const auto __temp_5_690 = __temp_5_544*__temp_5_484*__temp_5_486;
const auto __temp_5_691 = __temp_5_546*__temp_5_549*__temp_5_488;
const auto __temp_5_692 = __temp_5_690+__temp_5_691;
const auto __temp_5_693 = __temp_5_444*__temp_5_692*__temp_5_447*__temp_5_455;
const auto __temp_5_694 = __temp_5_444*__temp_5_544*__temp_5_488*__temp_5_459*__temp_5_461;
const auto __temp_5_695 = __temp_5_693+__temp_5_694;
const auto __temp_5_696 = __temp_5_499*__temp_5_447*__temp_5_455;
const auto __temp_5_697 = __temp_5_494*__temp_5_494;
const auto __temp_5_698 = __temp_5_444*__temp_5_459*__temp_5_697*__temp_5_461;
const auto __temp_5_699 = __temp_5_444*__temp_5_497*__temp_5_499*__temp_5_447*__temp_5_494*__temp_5_453;
const auto __temp_5_700 = __temp_5_696+__temp_5_698+__temp_5_699+__temp_5_457;
const auto __temp_5_701 = __temp_5_507*__temp_5_508*__temp_5_494;
const auto __temp_5_702 = __temp_5_497*__temp_5_499*__temp_5_504;
const auto __temp_5_703 = __temp_5_701+__temp_5_702;
const auto __temp_5_704 = __temp_5_444*__temp_5_447*__temp_5_703*__temp_5_455;
const auto __temp_5_705 = __temp_5_444*__temp_5_504*__temp_5_459*__temp_5_494*__temp_5_461;
const auto __temp_5_706 = __temp_5_704+__temp_5_705;
const auto __temp_5_707 = __temp_5_444*__temp_5_514*__temp_5_459*__temp_5_494*__temp_5_461;
const auto __temp_5_708 = __temp_5_497*__temp_5_499*__temp_5_514;
const auto __temp_5_709 = __temp_5_516*__temp_5_518*__temp_5_494;
const auto __temp_5_710 = __temp_5_708+__temp_5_709;
const auto __temp_5_711 = __temp_5_444*__temp_5_710*__temp_5_447*__temp_5_455;
const auto __temp_5_712 = __temp_5_707+__temp_5_711;
const auto __temp_5_713 = __temp_5_497*__temp_5_499*__temp_5_524;
const auto __temp_5_714 = __temp_5_527*__temp_5_528*__temp_5_494;
const auto __temp_5_715 = __temp_5_713+__temp_5_714;
const auto __temp_5_716 = __temp_5_444*__temp_5_447*__temp_5_715*__temp_5_455;
const auto __temp_5_717 = __temp_5_444*__temp_5_524*__temp_5_459*__temp_5_494*__temp_5_461;
const auto __temp_5_718 = __temp_5_716+__temp_5_717;
const auto __temp_5_719 = __temp_5_536*__temp_5_494*__temp_5_538;
const auto __temp_5_720 = __temp_5_497*__temp_5_499*__temp_5_534;
const auto __temp_5_721 = __temp_5_719+__temp_5_720;
const auto __temp_5_722 = __temp_5_444*__temp_5_721*__temp_5_447*__temp_5_455;
const auto __temp_5_723 = __temp_5_444*__temp_5_534*__temp_5_459*__temp_5_494*__temp_5_461;
const auto __temp_5_724 = __temp_5_722+__temp_5_723;
const auto __temp_5_725 = __temp_5_444*__temp_5_544*__temp_5_459*__temp_5_494*__temp_5_461;
const auto __temp_5_726 = __temp_5_546*__temp_5_549*__temp_5_494;
const auto __temp_5_727 = __temp_5_497*__temp_5_499*__temp_5_544;
const auto __temp_5_728 = __temp_5_726+__temp_5_727;
const auto __temp_5_729 = __temp_5_444*__temp_5_728*__temp_5_447*__temp_5_455;
const auto __temp_5_730 = __temp_5_725+__temp_5_729;
const auto __temp_5_731 = __temp_5_507*__temp_5_444*__temp_5_508*__temp_5_447*__temp_5_504*__temp_5_453;
const auto __temp_5_732 = __temp_5_504*__temp_5_504;
const auto __temp_5_733 = __temp_5_444*__temp_5_732*__temp_5_459*__temp_5_461;
const auto __temp_5_734 = __temp_5_507*__temp_5_447*__temp_5_455;
const auto __temp_5_735 = __temp_5_731+__temp_5_733+__temp_5_457+__temp_5_734;
const auto __temp_5_736 = __temp_5_516*__temp_5_518*__temp_5_504;
const auto __temp_5_737 = __temp_5_507*__temp_5_508*__temp_5_514;
const auto __temp_5_738 = __temp_5_736+__temp_5_737;
const auto __temp_5_739 = __temp_5_444*__temp_5_738*__temp_5_447*__temp_5_455;
const auto __temp_5_740 = __temp_5_444*__temp_5_514*__temp_5_504*__temp_5_459*__temp_5_461;
const auto __temp_5_741 = __temp_5_739+__temp_5_740;
const auto __temp_5_742 = __temp_5_444*__temp_5_524*__temp_5_504*__temp_5_459*__temp_5_461;
const auto __temp_5_743 = __temp_5_527*__temp_5_528*__temp_5_504;
const auto __temp_5_744 = __temp_5_507*__temp_5_508*__temp_5_524;
const auto __temp_5_745 = __temp_5_743+__temp_5_744;
const auto __temp_5_746 = __temp_5_444*__temp_5_447*__temp_5_745*__temp_5_455;
const auto __temp_5_747 = __temp_5_742+__temp_5_746;
const auto __temp_5_748 = __temp_5_507*__temp_5_508*__temp_5_534;
const auto __temp_5_749 = __temp_5_536*__temp_5_504*__temp_5_538;
const auto __temp_5_750 = __temp_5_748+__temp_5_749;
const auto __temp_5_751 = __temp_5_444*__temp_5_447*__temp_5_750*__temp_5_455;
const auto __temp_5_752 = __temp_5_444*__temp_5_534*__temp_5_504*__temp_5_459*__temp_5_461;
const auto __temp_5_753 = __temp_5_751+__temp_5_752;
const auto __temp_5_754 = __temp_5_507*__temp_5_508*__temp_5_544;
const auto __temp_5_755 = __temp_5_546*__temp_5_549*__temp_5_504;
const auto __temp_5_756 = __temp_5_754+__temp_5_755;
const auto __temp_5_757 = __temp_5_444*__temp_5_756*__temp_5_447*__temp_5_455;
const auto __temp_5_758 = __temp_5_444*__temp_5_544*__temp_5_504*__temp_5_459*__temp_5_461;
const auto __temp_5_759 = __temp_5_757+__temp_5_758;
const auto __temp_5_760 = __temp_5_447*__temp_5_518*__temp_5_455;
const auto __temp_5_761 = __temp_5_514*__temp_5_514;
const auto __temp_5_762 = __temp_5_444*__temp_5_761*__temp_5_459*__temp_5_461;
const auto __temp_5_763 = __temp_5_444*__temp_5_516*__temp_5_447*__temp_5_518*__temp_5_514*__temp_5_453;
const auto __temp_5_764 = __temp_5_760+__temp_5_762+__temp_5_763+__temp_5_457;
const auto __temp_5_765 = __temp_5_444*__temp_5_524*__temp_5_514*__temp_5_459*__temp_5_461;
const auto __temp_5_766 = __temp_5_516*__temp_5_524*__temp_5_518;
const auto __temp_5_767 = __temp_5_527*__temp_5_528*__temp_5_514;
const auto __temp_5_768 = __temp_5_766+__temp_5_767;
const auto __temp_5_769 = __temp_5_444*__temp_5_447*__temp_5_768*__temp_5_455;
const auto __temp_5_770 = __temp_5_765+__temp_5_769;
const auto __temp_5_771 = __temp_5_516*__temp_5_534*__temp_5_518;
const auto __temp_5_772 = __temp_5_536*__temp_5_514*__temp_5_538;
const auto __temp_5_773 = __temp_5_771+__temp_5_772;
const auto __temp_5_774 = __temp_5_444*__temp_5_447*__temp_5_773*__temp_5_455;
const auto __temp_5_775 = __temp_5_444*__temp_5_534*__temp_5_514*__temp_5_459*__temp_5_461;
const auto __temp_5_776 = __temp_5_774+__temp_5_775;
const auto __temp_5_777 = __temp_5_516*__temp_5_544*__temp_5_518;
const auto __temp_5_778 = __temp_5_546*__temp_5_549*__temp_5_514;
const auto __temp_5_779 = __temp_5_777+__temp_5_778;
const auto __temp_5_780 = __temp_5_444*__temp_5_779*__temp_5_447*__temp_5_455;
const auto __temp_5_781 = __temp_5_444*__temp_5_544*__temp_5_514*__temp_5_459*__temp_5_461;
const auto __temp_5_782 = __temp_5_780+__temp_5_781;
const auto __temp_5_783 = __temp_5_524*__temp_5_524;
const auto __temp_5_784 = __temp_5_444*__temp_5_783*__temp_5_459*__temp_5_461;
const auto __temp_5_785 = __temp_5_444*__temp_5_527*__temp_5_528*__temp_5_447*__temp_5_524*__temp_5_453;
const auto __temp_5_786 = __temp_5_527*__temp_5_447*__temp_5_455;
const auto __temp_5_787 = __temp_5_784+__temp_5_785+__temp_5_786+__temp_5_457;
const auto __temp_5_788 = __temp_5_444*__temp_5_534*__temp_5_524*__temp_5_459*__temp_5_461;
const auto __temp_5_789 = __temp_5_536*__temp_5_524*__temp_5_538;
const auto __temp_5_790 = __temp_5_527*__temp_5_528*__temp_5_534;
const auto __temp_5_791 = __temp_5_789+__temp_5_790;
const auto __temp_5_792 = __temp_5_444*__temp_5_791*__temp_5_447*__temp_5_455;
const auto __temp_5_793 = __temp_5_788+__temp_5_792;
const auto __temp_5_794 = __temp_5_546*__temp_5_524*__temp_5_549;
const auto __temp_5_795 = __temp_5_527*__temp_5_528*__temp_5_544;
const auto __temp_5_796 = __temp_5_794+__temp_5_795;
const auto __temp_5_797 = __temp_5_444*__temp_5_796*__temp_5_447*__temp_5_455;
const auto __temp_5_798 = __temp_5_444*__temp_5_544*__temp_5_524*__temp_5_459*__temp_5_461;
const auto __temp_5_799 = __temp_5_797+__temp_5_798;
const auto __temp_5_800 = __temp_5_444*__temp_5_536*__temp_5_534*__temp_5_447*__temp_5_538*__temp_5_453;
const auto __temp_5_801 = __temp_5_534*__temp_5_534;
const auto __temp_5_802 = __temp_5_444*__temp_5_801*__temp_5_459*__temp_5_461;
const auto __temp_5_803 = __temp_5_447*__temp_5_538*__temp_5_455;
const auto __temp_5_804 = __temp_5_800+__temp_5_457+__temp_5_802+__temp_5_803;
const auto __temp_5_805 = __temp_5_546*__temp_5_534*__temp_5_549;
const auto __temp_5_806 = __temp_5_536*__temp_5_544*__temp_5_538;
const auto __temp_5_807 = __temp_5_805+__temp_5_806;
const auto __temp_5_808 = __temp_5_807*__temp_5_444*__temp_5_447*__temp_5_455;
const auto __temp_5_809 = __temp_5_444*__temp_5_544*__temp_5_534*__temp_5_459*__temp_5_461;
const auto __temp_5_810 = __temp_5_808+__temp_5_809;
const auto __temp_5_811 = __temp_5_444*__temp_5_546*__temp_5_544*__temp_5_447*__temp_5_549*__temp_5_453;
const auto __temp_5_812 = __temp_5_447*__temp_5_549*__temp_5_455;
const auto __temp_5_813 = __temp_5_544*__temp_5_544;
const auto __temp_5_814 = __temp_5_444*__temp_5_813*__temp_5_459*__temp_5_461;
const auto __temp_5_815 = __temp_5_811+__temp_5_812+__temp_5_814+__temp_5_457;
        // -- END -- temporary pool (sequence=5)

//      std::array< std::array<number, 10>, 10> __M;
//
//      __M[0][0] = __temp_5_463;
//      __M[0][1] = __temp_5_473;
//      __M[0][2] = __temp_5_483;
//      __M[0][3] = __temp_5_493;
//      __M[0][4] = __temp_5_503;
//      __M[0][5] = __temp_5_513;
//      __M[0][6] = __temp_5_523;
//      __M[0][7] = __temp_5_533;
//      __M[0][8] = __temp_5_543;
//      __M[0][9] = __temp_5_554;
//      __M[1][0] = __temp_5_473;
//      __M[1][1] = __temp_5_559;
//      __M[1][2] = __temp_5_565;
//      __M[1][3] = __temp_5_571;
//      __M[1][4] = __temp_5_577;
//      __M[1][5] = __temp_5_583;
//      __M[1][6] = __temp_5_589;
//      __M[1][7] = __temp_5_595;
//      __M[1][8] = __temp_5_601;
//      __M[1][9] = __temp_5_607;
//      __M[2][0] = __temp_5_483;
//      __M[2][1] = __temp_5_565;
//      __M[2][2] = __temp_5_612;
//      __M[2][3] = __temp_5_618;
//      __M[2][4] = __temp_5_624;
//      __M[2][5] = __temp_5_630;
//      __M[2][6] = __temp_5_636;
//      __M[2][7] = __temp_5_642;
//      __M[2][8] = __temp_5_648;
//      __M[2][9] = __temp_5_654;
//      __M[3][0] = __temp_5_493;
//      __M[3][1] = __temp_5_571;
//      __M[3][2] = __temp_5_618;
//      __M[3][3] = __temp_5_659;
//      __M[3][4] = __temp_5_665;
//      __M[3][5] = __temp_5_671;
//      __M[3][6] = __temp_5_677;
//      __M[3][7] = __temp_5_683;
//      __M[3][8] = __temp_5_689;
//      __M[3][9] = __temp_5_695;
//      __M[4][0] = __temp_5_503;
//      __M[4][1] = __temp_5_577;
//      __M[4][2] = __temp_5_624;
//      __M[4][3] = __temp_5_665;
//      __M[4][4] = __temp_5_700;
//      __M[4][5] = __temp_5_706;
//      __M[4][6] = __temp_5_712;
//      __M[4][7] = __temp_5_718;
//      __M[4][8] = __temp_5_724;
//      __M[4][9] = __temp_5_730;
//      __M[5][0] = __temp_5_513;
//      __M[5][1] = __temp_5_583;
//      __M[5][2] = __temp_5_630;
//      __M[5][3] = __temp_5_671;
//      __M[5][4] = __temp_5_706;
//      __M[5][5] = __temp_5_735;
//      __M[5][6] = __temp_5_741;
//      __M[5][7] = __temp_5_747;
//      __M[5][8] = __temp_5_753;
//      __M[5][9] = __temp_5_759;
//      __M[6][0] = __temp_5_523;
//      __M[6][1] = __temp_5_589;
//      __M[6][2] = __temp_5_636;
//      __M[6][3] = __temp_5_677;
//      __M[6][4] = __temp_5_712;
//      __M[6][5] = __temp_5_741;
//      __M[6][6] = __temp_5_764;
//      __M[6][7] = __temp_5_770;
//      __M[6][8] = __temp_5_776;
//      __M[6][9] = __temp_5_782;
//      __M[7][0] = __temp_5_533;
//      __M[7][1] = __temp_5_595;
//      __M[7][2] = __temp_5_642;
//      __M[7][3] = __temp_5_683;
//      __M[7][4] = __temp_5_718;
//      __M[7][5] = __temp_5_747;
//      __M[7][6] = __temp_5_770;
//      __M[7][7] = __temp_5_787;
//      __M[7][8] = __temp_5_793;
//      __M[7][9] = __temp_5_799;
//      __M[8][0] = __temp_5_543;
//      __M[8][1] = __temp_5_601;
//      __M[8][2] = __temp_5_648;
//      __M[8][3] = __temp_5_689;
//      __M[8][4] = __temp_5_724;
//      __M[8][5] = __temp_5_753;
//      __M[8][6] = __temp_5_776;
//      __M[8][7] = __temp_5_793;
//      __M[8][8] = __temp_5_804;
//      __M[8][9] = __temp_5_810;
//      __M[9][0] = __temp_5_554;
//      __M[9][1] = __temp_5_607;
//      __M[9][2] = __temp_5_654;
//      __M[9][3] = __temp_5_695;
//      __M[9][4] = __temp_5_730;
//      __M[9][5] = __temp_5_759;
//      __M[9][6] = __temp_5_782;
//      __M[9][7] = __temp_5_799;
//      __M[9][8] = __temp_5_810;
//      __M[9][9] = __temp_5_815;

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


    // set up initial conditions for the real part of the equal-time tensor two-point function
    template <typename number>
    number quadratic10<number>::make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit, const parameters<number>& __params, const std::vector<number>& __fields)
	    {
        const auto m1  = __params.get_vector()[0];
        const auto m2  = __params.get_vector()[1];
        const auto m3  = __params.get_vector()[2];
        const auto m4  = __params.get_vector()[3];
        const auto m5  = __params.get_vector()[4];
        const auto m6  = __params.get_vector()[5];
        const auto m7  = __params.get_vector()[6];
        const auto m8  = __params.get_vector()[7];
        const auto m9  = __params.get_vector()[8];
        const auto m10  = __params.get_vector()[9];
        const auto phi1 = __fields[0];
        const auto phi2 = __fields[1];
        const auto phi3 = __fields[2];
        const auto phi4 = __fields[3];
        const auto phi5 = __fields[4];
        const auto phi6 = __fields[5];
        const auto phi7 = __fields[6];
        const auto phi8 = __fields[7];
        const auto phi9 = __fields[8];
        const auto phi10 = __fields[9];
        const auto __dphi1 = __fields[10];
        const auto __dphi2 = __fields[11];
        const auto __dphi3 = __fields[12];
        const auto __dphi4 = __fields[13];
        const auto __dphi5 = __fields[14];
        const auto __dphi6 = __fields[15];
        const auto __dphi7 = __fields[16];
        const auto __dphi8 = __fields[17];
        const auto __dphi9 = __fields[18];
        const auto __dphi10 = __fields[19];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = -( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0);
        const auto __eps             = (1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9));
        const auto __ainit           = exp(__Ninit);

        const auto __N               = log(__k/(__ainit*sqrt(__Hsq)));

        number     __tpf             = 0.0;

// -- START -- temporary pool (sequence=6) 
        // -- END -- temporary pool (sequence=6)

        if(__i == 0 && __j == 0)                                      // h-h correlation function
	        {
            // LEADING-ORDER INITIAL CONDITION
            __tpf = 1.0 / (__Mp*__Mp*__k*__ainit*__ainit);
//            __tpf = 1.0 / (2.0*__k*__ainit*__ainit);
	        }
        else if((__i == 0 && __j == 1) || (__i == 1 && __j == 0))     // h-dh or dh-h corelation function
	        {
            // LEADING ORDER INITIAL CONDITION
            __tpf = -1.0 / (__Mp*__Mp*__k*__ainit*__ainit);
//            __tpf = -1.0 / (2.0*__k*__ainit*__ainit);
	        }
        else if(__i == 1 && __j == 1)                                 // dh-dh correlation function
	        {
            // LEADING ORDER INITIAL CONDITION
            __tpf = __k / (__Mp*__Mp*__Hsq*__ainit*__ainit*__ainit*__ainit);
//            __tpf = __k / (2.0*__Hsq*__ainit*__ainit*__ainit*__ainit);
	        }
        else
	        {
            assert(false);
	        }

        return(__tpf);
	    }


    // set up initial conditions for the real part of the equal-time three-point function
    template <typename number>
    number quadratic10<number>::make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                                              double __kmode_1, double __kmode_2, double __kmode_3, double __Ninit,
                                              const parameters<number>& __params,
                                              const std::vector<number>& __fields)
	    {
        number     __tpf             = 0.0;
        return(__tpf);
	    }


    // CALCULATE GAUGE TRANSFORMATIONS


    template <typename number>
    void quadratic10<number>::compute_gauge_xfm_1(const parameters<number>& __params,
                                                const std::vector<number>& __state,
                                                std::vector<number>& __dN)
	    {
        const auto m1  = __params.get_vector()[0];
        const auto m2  = __params.get_vector()[1];
        const auto m3  = __params.get_vector()[2];
        const auto m4  = __params.get_vector()[3];
        const auto m5  = __params.get_vector()[4];
        const auto m6  = __params.get_vector()[5];
        const auto m7  = __params.get_vector()[6];
        const auto m8  = __params.get_vector()[7];
        const auto m9  = __params.get_vector()[8];
        const auto m10  = __params.get_vector()[9];
        const auto phi1 = __state[0];
        const auto phi2 = __state[1];
        const auto phi3 = __state[2];
        const auto phi4 = __state[3];
        const auto phi5 = __state[4];
        const auto phi6 = __state[5];
        const auto phi7 = __state[6];
        const auto phi8 = __state[7];
        const auto phi9 = __state[8];
        const auto phi10 = __state[9];
        const auto __dphi1 = __state[10];
        const auto __dphi2 = __state[11];
        const auto __dphi3 = __state[12];
        const auto __dphi4 = __state[13];
        const auto __dphi5 = __state[14];
        const auto __dphi6 = __state[15];
        const auto __dphi7 = __state[16];
        const auto __dphi8 = __state[17];
        const auto __dphi9 = __state[18];
        const auto __dphi10 = __state[19];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = -( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0);
        const auto __eps             = (1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9));

// -- START -- temporary pool (sequence=7) 
const auto __temp_7_816 = __Mp;
const auto __temp_7_818 = 1.0/(__temp_7_816*__temp_7_816);
const auto __temp_7_819 = __eps;
const auto __temp_7_821 = 1.0/__temp_7_819;
const auto __temp_7_822 = __dphi1;
const auto __temp_7_823 = -(1.0/2.0);
const auto __temp_7_824 = __temp_7_818*__temp_7_821*__temp_7_822*__temp_7_823;
const auto __temp_7_825 = __dphi2;
const auto __temp_7_826 = __temp_7_818*__temp_7_821*__temp_7_825*__temp_7_823;
const auto __temp_7_827 = __dphi3;
const auto __temp_7_828 = __temp_7_818*__temp_7_821*__temp_7_827*__temp_7_823;
const auto __temp_7_829 = __dphi4;
const auto __temp_7_830 = __temp_7_818*__temp_7_821*__temp_7_829*__temp_7_823;
const auto __temp_7_831 = __dphi5;
const auto __temp_7_832 = __temp_7_818*__temp_7_821*__temp_7_831*__temp_7_823;
const auto __temp_7_833 = __dphi6;
const auto __temp_7_834 = __temp_7_818*__temp_7_821*__temp_7_833*__temp_7_823;
const auto __temp_7_835 = __dphi7;
const auto __temp_7_836 = __temp_7_818*__temp_7_821*__temp_7_835*__temp_7_823;
const auto __temp_7_837 = __dphi8;
const auto __temp_7_838 = __temp_7_818*__temp_7_821*__temp_7_837*__temp_7_823;
const auto __temp_7_839 = __dphi9;
const auto __temp_7_840 = __temp_7_818*__temp_7_821*__temp_7_839*__temp_7_823;
const auto __temp_7_841 = __dphi10;
const auto __temp_7_842 = __temp_7_818*__temp_7_841*__temp_7_821*__temp_7_823;
const auto __temp_7_843 = 0.0;
        // -- END -- temporary pool (sequence=7)

        __dN.clear();
        __dN.resize(2*10); // ensure enough space
        __dN[0] = __temp_7_824;
        __dN[1] = __temp_7_826;
        __dN[2] = __temp_7_828;
        __dN[3] = __temp_7_830;
        __dN[4] = __temp_7_832;
        __dN[5] = __temp_7_834;
        __dN[6] = __temp_7_836;
        __dN[7] = __temp_7_838;
        __dN[8] = __temp_7_840;
        __dN[9] = __temp_7_842;
        __dN[10] = __temp_7_843;
        __dN[11] = __temp_7_843;
        __dN[12] = __temp_7_843;
        __dN[13] = __temp_7_843;
        __dN[14] = __temp_7_843;
        __dN[15] = __temp_7_843;
        __dN[16] = __temp_7_843;
        __dN[17] = __temp_7_843;
        __dN[18] = __temp_7_843;
        __dN[19] = __temp_7_843;
	    }


    template <typename number>
    void quadratic10<number>::compute_gauge_xfm_2(const parameters<number>& __params,
                                                const std::vector<number>& __state,
                                                double __k, double __k1, double __k2, double __N,
                                                std::vector< std::vector<number> >& __ddN)
	    {
	    }


    template <typename number>
    void quadratic10<number>::compute_deltaN_xfm_1(const parameters<number>& __params,
                                                 const std::vector<number>& __state,
                                                 std::vector<number>& __dN)
	    {
        const auto m1  = __params.get_vector()[0];
        const auto m2  = __params.get_vector()[1];
        const auto m3  = __params.get_vector()[2];
        const auto m4  = __params.get_vector()[3];
        const auto m5  = __params.get_vector()[4];
        const auto m6  = __params.get_vector()[5];
        const auto m7  = __params.get_vector()[6];
        const auto m8  = __params.get_vector()[7];
        const auto m9  = __params.get_vector()[8];
        const auto m10  = __params.get_vector()[9];
        const auto phi1 = __state[0];
        const auto phi2 = __state[1];
        const auto phi3 = __state[2];
        const auto phi4 = __state[3];
        const auto phi5 = __state[4];
        const auto phi6 = __state[5];
        const auto phi7 = __state[6];
        const auto phi8 = __state[7];
        const auto phi9 = __state[8];
        const auto phi10 = __state[9];
        const auto __dphi1 = __state[10];
        const auto __dphi2 = __state[11];
        const auto __dphi3 = __state[12];
        const auto __dphi4 = __state[13];
        const auto __dphi5 = __state[14];
        const auto __dphi6 = __state[15];
        const auto __dphi7 = __state[16];
        const auto __dphi8 = __state[17];
        const auto __dphi9 = __state[18];
        const auto __dphi10 = __state[19];
        const auto __Mp              = __params.get_Mp();

// -- START -- temporary pool (sequence=8) 
const auto __temp_8_844 = phi1;
const auto __temp_8_846 = __temp_8_844*__temp_8_844;
const auto __temp_8_847 = m1;
const auto __temp_8_848 = __temp_8_847*__temp_8_847;
const auto __temp_8_849 = __temp_8_846*__temp_8_848;
const auto __temp_8_850 = phi4;
const auto __temp_8_851 = __temp_8_850*__temp_8_850;
const auto __temp_8_852 = m4;
const auto __temp_8_853 = __temp_8_852*__temp_8_852;
const auto __temp_8_854 = __temp_8_851*__temp_8_853;
const auto __temp_8_855 = phi9;
const auto __temp_8_856 = __temp_8_855*__temp_8_855;
const auto __temp_8_857 = m9;
const auto __temp_8_858 = __temp_8_857*__temp_8_857;
const auto __temp_8_859 = __temp_8_856*__temp_8_858;
const auto __temp_8_860 = phi7;
const auto __temp_8_861 = __temp_8_860*__temp_8_860;
const auto __temp_8_862 = m7;
const auto __temp_8_863 = __temp_8_862*__temp_8_862;
const auto __temp_8_864 = __temp_8_861*__temp_8_863;
const auto __temp_8_865 = m2;
const auto __temp_8_866 = __temp_8_865*__temp_8_865;
const auto __temp_8_867 = phi2;
const auto __temp_8_868 = __temp_8_867*__temp_8_867;
const auto __temp_8_869 = __temp_8_866*__temp_8_868;
const auto __temp_8_870 = phi10;
const auto __temp_8_871 = __temp_8_870*__temp_8_870;
const auto __temp_8_872 = m10;
const auto __temp_8_874 = pow(__temp_8_872,10);
const auto __temp_8_875 = __temp_8_871*__temp_8_874;
const auto __temp_8_876 = m6;
const auto __temp_8_877 = __temp_8_876*__temp_8_876;
const auto __temp_8_878 = phi6;
const auto __temp_8_879 = __temp_8_878*__temp_8_878;
const auto __temp_8_880 = __temp_8_877*__temp_8_879;
const auto __temp_8_881 = phi5;
const auto __temp_8_882 = __temp_8_881*__temp_8_881;
const auto __temp_8_883 = m5;
const auto __temp_8_884 = __temp_8_883*__temp_8_883;
const auto __temp_8_885 = __temp_8_882*__temp_8_884;
const auto __temp_8_886 = m3;
const auto __temp_8_887 = __temp_8_886*__temp_8_886;
const auto __temp_8_888 = phi3;
const auto __temp_8_889 = __temp_8_888*__temp_8_888;
const auto __temp_8_890 = __temp_8_887*__temp_8_889;
const auto __temp_8_891 = m8;
const auto __temp_8_892 = __temp_8_891*__temp_8_891;
const auto __temp_8_893 = phi8;
const auto __temp_8_894 = __temp_8_893*__temp_8_893;
const auto __temp_8_895 = __temp_8_892*__temp_8_894;
const auto __temp_8_896 = __temp_8_849+__temp_8_854+__temp_8_859+__temp_8_864+__temp_8_869+__temp_8_875+__temp_8_880+__temp_8_885+__temp_8_890+__temp_8_895;
const auto __temp_8_898 = 1.0/__temp_8_896;
const auto __temp_8_899 = __Mp;
const auto __temp_8_900 = __temp_8_899*__temp_8_899;
const auto __temp_8_901 = __dphi8;
const auto __temp_8_902 = __temp_8_901*__temp_8_901;
const auto __temp_8_903 = __dphi7;
const auto __temp_8_904 = __temp_8_903*__temp_8_903;
const auto __temp_8_905 = __dphi6;
const auto __temp_8_906 = __temp_8_905*__temp_8_905;
const auto __temp_8_907 = __dphi4;
const auto __temp_8_908 = __temp_8_907*__temp_8_907;
const auto __temp_8_909 = __dphi5;
const auto __temp_8_910 = __temp_8_909*__temp_8_909;
const auto __temp_8_911 = __dphi3;
const auto __temp_8_912 = __temp_8_911*__temp_8_911;
const auto __temp_8_913 = __dphi2;
const auto __temp_8_914 = __temp_8_913*__temp_8_913;
const auto __temp_8_915 = __dphi10;
const auto __temp_8_916 = __temp_8_915*__temp_8_915;
const auto __temp_8_917 = __dphi1;
const auto __temp_8_918 = __temp_8_917*__temp_8_917;
const auto __temp_8_919 = __dphi9;
const auto __temp_8_920 = __temp_8_919*__temp_8_919;
const auto __temp_8_921 = __temp_8_902+__temp_8_904+__temp_8_906+__temp_8_908+__temp_8_910+__temp_8_912+__temp_8_914+__temp_8_916+__temp_8_918+__temp_8_920;
const auto __temp_8_922 = 1.0/__temp_8_921;
const auto __temp_8_845 = 2.0;
const auto __temp_8_923 = __temp_8_898*__temp_8_900*__temp_8_922*__temp_8_844*__temp_8_848*__temp_8_845;
const auto __temp_8_924 = __temp_8_898*__temp_8_900*__temp_8_922*__temp_8_866*__temp_8_867*__temp_8_845;
const auto __temp_8_925 = __temp_8_898*__temp_8_900*__temp_8_887*__temp_8_922*__temp_8_888*__temp_8_845;
const auto __temp_8_926 = __temp_8_898*__temp_8_900*__temp_8_922*__temp_8_850*__temp_8_853*__temp_8_845;
const auto __temp_8_927 = __temp_8_898*__temp_8_900*__temp_8_881*__temp_8_884*__temp_8_922*__temp_8_845;
const auto __temp_8_928 = __temp_8_877*__temp_8_898*__temp_8_900*__temp_8_878*__temp_8_922*__temp_8_845;
const auto __temp_8_929 = __temp_8_898*__temp_8_900*__temp_8_860*__temp_8_922*__temp_8_863*__temp_8_845;
const auto __temp_8_930 = __temp_8_898*__temp_8_900*__temp_8_892*__temp_8_893*__temp_8_922*__temp_8_845;
const auto __temp_8_931 = __temp_8_898*__temp_8_900*__temp_8_855*__temp_8_922*__temp_8_858*__temp_8_845;
const auto __temp_8_932 = __temp_8_898*__temp_8_900*__temp_8_870*__temp_8_922*__temp_8_874*__temp_8_845;
const auto __temp_8_934 = 1.0/(__temp_8_899*__temp_8_899);
const auto __temp_8_935 = __temp_8_934*__temp_8_921;
const auto __temp_8_936 = -6.0;
const auto __temp_8_937 = __temp_8_935+__temp_8_936;
const auto __temp_8_938 = 1.0/__temp_8_937;
const auto __temp_8_933 = -2.0;
const auto __temp_8_939 = __temp_8_922*__temp_8_938*__temp_8_917*__temp_8_933;
const auto __temp_8_940 = __temp_8_922*__temp_8_938*__temp_8_913*__temp_8_933;
const auto __temp_8_941 = __temp_8_922*__temp_8_911*__temp_8_938*__temp_8_933;
const auto __temp_8_942 = __temp_8_922*__temp_8_907*__temp_8_938*__temp_8_933;
const auto __temp_8_943 = __temp_8_922*__temp_8_909*__temp_8_938*__temp_8_933;
const auto __temp_8_944 = __temp_8_922*__temp_8_905*__temp_8_938*__temp_8_933;
const auto __temp_8_945 = __temp_8_922*__temp_8_903*__temp_8_938*__temp_8_933;
const auto __temp_8_946 = __temp_8_922*__temp_8_901*__temp_8_938*__temp_8_933;
const auto __temp_8_947 = __temp_8_922*__temp_8_919*__temp_8_938*__temp_8_933;
const auto __temp_8_948 = __temp_8_915*__temp_8_922*__temp_8_938*__temp_8_933;
        // -- END -- temporary pool (sequence=8)

        __dN.clear();
        __dN.resize(2*10); // ensure enough space
        __dN[0] = __temp_8_923;
        __dN[1] = __temp_8_924;
        __dN[2] = __temp_8_925;
        __dN[3] = __temp_8_926;
        __dN[4] = __temp_8_927;
        __dN[5] = __temp_8_928;
        __dN[6] = __temp_8_929;
        __dN[7] = __temp_8_930;
        __dN[8] = __temp_8_931;
        __dN[9] = __temp_8_932;
        __dN[10] = __temp_8_939;
        __dN[11] = __temp_8_940;
        __dN[12] = __temp_8_941;
        __dN[13] = __temp_8_942;
        __dN[14] = __temp_8_943;
        __dN[15] = __temp_8_944;
        __dN[16] = __temp_8_945;
        __dN[17] = __temp_8_946;
        __dN[18] = __temp_8_947;
        __dN[19] = __temp_8_948;
	    }


    template <typename number>
    void quadratic10<number>::compute_deltaN_xfm_2(const parameters<number>& __params,
                                                 const std::vector<number>& __state,
                                                 std::vector< std::vector<number> >& __ddN)
	    {
	    }


    // CALCULATE TENSOR QUANTITIES


    template <typename number>
    void quadratic10<number>::u2(const parameters<number>& __params,
                               const std::vector<number>& __fields, double __k, double __N,
                               std::vector< std::vector<number> >& __u2)
	    {
        const auto m1  = __params.get_vector()[0];
        const auto m2  = __params.get_vector()[1];
        const auto m3  = __params.get_vector()[2];
        const auto m4  = __params.get_vector()[3];
        const auto m5  = __params.get_vector()[4];
        const auto m6  = __params.get_vector()[5];
        const auto m7  = __params.get_vector()[6];
        const auto m8  = __params.get_vector()[7];
        const auto m9  = __params.get_vector()[8];
        const auto m10  = __params.get_vector()[9];
        const auto phi1 = __fields[0];
        const auto phi2 = __fields[1];
        const auto phi3 = __fields[2];
        const auto phi4 = __fields[3];
        const auto phi5 = __fields[4];
        const auto phi6 = __fields[5];
        const auto phi7 = __fields[6];
        const auto phi8 = __fields[7];
        const auto phi9 = __fields[8];
        const auto phi10 = __fields[9];
        const auto __dphi1 = __fields[10];
        const auto __dphi2 = __fields[11];
        const auto __dphi3 = __fields[12];
        const auto __dphi4 = __fields[13];
        const auto __dphi5 = __fields[14];
        const auto __dphi6 = __fields[15];
        const auto __dphi7 = __fields[16];
        const auto __dphi8 = __fields[17];
        const auto __dphi9 = __fields[18];
        const auto __dphi10 = __fields[19];
        const auto __Mp              = __params.get_Mp();

        const auto __Hsq             = -( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0);
        const auto __eps             = (1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9));
        const auto __a               = exp(__N);

// -- START -- temporary pool (sequence=9) 
const auto __temp_9_949 = 0.0;
const auto __temp_9_950 = 1.0;
const auto __temp_9_951 = __Mp;
const auto __temp_9_953 = 1.0/(__temp_9_951*__temp_9_951);
const auto __temp_9_954 = __Hsq;
const auto __temp_9_956 = 1.0/__temp_9_954;
const auto __temp_9_957 = phi1;
const auto __temp_9_958 = m1;
const auto __temp_9_960 = __temp_9_958*__temp_9_958;
const auto __temp_9_961 = __dphi1;
const auto __temp_9_952 = -2.0;
const auto __temp_9_962 = __temp_9_953*__temp_9_956*__temp_9_957*__temp_9_960*__temp_9_961*__temp_9_952;
const auto __temp_9_955 = -1.0;
const auto __temp_9_963 = __temp_9_956*__temp_9_960*__temp_9_955;
const auto __temp_9_964 = __a;
const auto __temp_9_965 = 1.0/(__temp_9_964*__temp_9_964);
const auto __temp_9_966 = __k;
const auto __temp_9_967 = __temp_9_966*__temp_9_966;
const auto __temp_9_968 = __temp_9_965*__temp_9_956*__temp_9_967*__temp_9_955;
const auto __temp_9_969 = __eps;
const auto __temp_9_970 = -3.0;
const auto __temp_9_971 = __temp_9_969+__temp_9_970;
const auto __temp_9_972 = __temp_9_961*__temp_9_961;
const auto __temp_9_973 = __temp_9_971*__temp_9_953*__temp_9_972;
const auto __temp_9_974 = __temp_9_962+__temp_9_963+__temp_9_968+__temp_9_973;
const auto __temp_9_975 = __dphi2;
const auto __temp_9_976 = __temp_9_971*__temp_9_953*__temp_9_975*__temp_9_961;
const auto __temp_9_977 = __temp_9_957*__temp_9_975*__temp_9_960;
const auto __temp_9_978 = m2;
const auto __temp_9_979 = __temp_9_978*__temp_9_978;
const auto __temp_9_980 = phi2;
const auto __temp_9_981 = __temp_9_979*__temp_9_980*__temp_9_961;
const auto __temp_9_982 = __temp_9_977+__temp_9_981;
const auto __temp_9_983 = __temp_9_953*__temp_9_982*__temp_9_956*__temp_9_955;
const auto __temp_9_984 = __temp_9_976+__temp_9_983;
const auto __temp_9_985 = __dphi3;
const auto __temp_9_986 = __temp_9_971*__temp_9_953*__temp_9_985*__temp_9_961;
const auto __temp_9_987 = __temp_9_957*__temp_9_985*__temp_9_960;
const auto __temp_9_988 = m3;
const auto __temp_9_989 = __temp_9_988*__temp_9_988;
const auto __temp_9_990 = phi3;
const auto __temp_9_991 = __temp_9_989*__temp_9_990*__temp_9_961;
const auto __temp_9_992 = __temp_9_987+__temp_9_991;
const auto __temp_9_993 = __temp_9_953*__temp_9_956*__temp_9_992*__temp_9_955;
const auto __temp_9_994 = __temp_9_986+__temp_9_993;
const auto __temp_9_995 = phi4;
const auto __temp_9_996 = m4;
const auto __temp_9_997 = __temp_9_996*__temp_9_996;
const auto __temp_9_998 = __temp_9_995*__temp_9_997*__temp_9_961;
const auto __temp_9_999 = __dphi4;
const auto __temp_9_1000 = __temp_9_957*__temp_9_999*__temp_9_960;
const auto __temp_9_1001 = __temp_9_998+__temp_9_1000;
const auto __temp_9_1002 = __temp_9_1001*__temp_9_953*__temp_9_956*__temp_9_955;
const auto __temp_9_1003 = __temp_9_971*__temp_9_953*__temp_9_999*__temp_9_961;
const auto __temp_9_1004 = __temp_9_1002+__temp_9_1003;
const auto __temp_9_1005 = __dphi5;
const auto __temp_9_1006 = __temp_9_971*__temp_9_953*__temp_9_1005*__temp_9_961;
const auto __temp_9_1007 = __temp_9_957*__temp_9_1005*__temp_9_960;
const auto __temp_9_1008 = phi5;
const auto __temp_9_1009 = m5;
const auto __temp_9_1010 = __temp_9_1009*__temp_9_1009;
const auto __temp_9_1011 = __temp_9_1008*__temp_9_1010*__temp_9_961;
const auto __temp_9_1012 = __temp_9_1007+__temp_9_1011;
const auto __temp_9_1013 = __temp_9_953*__temp_9_1012*__temp_9_956*__temp_9_955;
const auto __temp_9_1014 = __temp_9_1006+__temp_9_1013;
const auto __temp_9_1015 = m6;
const auto __temp_9_1016 = __temp_9_1015*__temp_9_1015;
const auto __temp_9_1017 = phi6;
const auto __temp_9_1018 = __temp_9_1016*__temp_9_1017*__temp_9_961;
const auto __temp_9_1019 = __dphi6;
const auto __temp_9_1020 = __temp_9_957*__temp_9_1019*__temp_9_960;
const auto __temp_9_1021 = __temp_9_1018+__temp_9_1020;
const auto __temp_9_1022 = __temp_9_953*__temp_9_956*__temp_9_1021*__temp_9_955;
const auto __temp_9_1023 = __temp_9_971*__temp_9_953*__temp_9_1019*__temp_9_961;
const auto __temp_9_1024 = __temp_9_1022+__temp_9_1023;
const auto __temp_9_1025 = phi7;
const auto __temp_9_1026 = m7;
const auto __temp_9_1027 = __temp_9_1026*__temp_9_1026;
const auto __temp_9_1028 = __temp_9_1025*__temp_9_1027*__temp_9_961;
const auto __temp_9_1029 = __dphi7;
const auto __temp_9_1030 = __temp_9_1029*__temp_9_957*__temp_9_960;
const auto __temp_9_1031 = __temp_9_1028+__temp_9_1030;
const auto __temp_9_1032 = __temp_9_953*__temp_9_1031*__temp_9_956*__temp_9_955;
const auto __temp_9_1033 = __temp_9_971*__temp_9_953*__temp_9_1029*__temp_9_961;
const auto __temp_9_1034 = __temp_9_1032+__temp_9_1033;
const auto __temp_9_1035 = __dphi8;
const auto __temp_9_1036 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_961;
const auto __temp_9_1037 = __temp_9_1035*__temp_9_957*__temp_9_960;
const auto __temp_9_1038 = m8;
const auto __temp_9_1039 = __temp_9_1038*__temp_9_1038;
const auto __temp_9_1040 = phi8;
const auto __temp_9_1041 = __temp_9_1039*__temp_9_1040*__temp_9_961;
const auto __temp_9_1042 = __temp_9_1037+__temp_9_1041;
const auto __temp_9_1043 = __temp_9_953*__temp_9_956*__temp_9_1042*__temp_9_955;
const auto __temp_9_1044 = __temp_9_1036+__temp_9_1043;
const auto __temp_9_1045 = __dphi9;
const auto __temp_9_1046 = __temp_9_1045*__temp_9_957*__temp_9_960;
const auto __temp_9_1047 = phi9;
const auto __temp_9_1048 = m9;
const auto __temp_9_1049 = __temp_9_1048*__temp_9_1048;
const auto __temp_9_1050 = __temp_9_1047*__temp_9_961*__temp_9_1049;
const auto __temp_9_1051 = __temp_9_1046+__temp_9_1050;
const auto __temp_9_1052 = __temp_9_953*__temp_9_1051*__temp_9_956*__temp_9_955;
const auto __temp_9_1053 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_961;
const auto __temp_9_1054 = __temp_9_1052+__temp_9_1053;
const auto __temp_9_1055 = phi10;
const auto __temp_9_1056 = m10;
const auto __temp_9_1058 = pow(__temp_9_1056,10);
const auto __temp_9_1059 = __temp_9_1055*__temp_9_1058*__temp_9_961;
const auto __temp_9_1060 = __dphi10;
const auto __temp_9_1061 = __temp_9_1060*__temp_9_957*__temp_9_960;
const auto __temp_9_1062 = __temp_9_1059+__temp_9_1061;
const auto __temp_9_1063 = __temp_9_953*__temp_9_956*__temp_9_1062*__temp_9_955;
const auto __temp_9_1064 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_961;
const auto __temp_9_1065 = __temp_9_1063+__temp_9_1064;
const auto __temp_9_1066 = __temp_9_975*__temp_9_975;
const auto __temp_9_1067 = __temp_9_971*__temp_9_953*__temp_9_1066;
const auto __temp_9_1068 = __temp_9_956*__temp_9_979*__temp_9_955;
const auto __temp_9_1069 = __temp_9_953*__temp_9_956*__temp_9_979*__temp_9_980*__temp_9_975*__temp_9_952;
const auto __temp_9_1070 = __temp_9_968+__temp_9_1067+__temp_9_1068+__temp_9_1069;
const auto __temp_9_1071 = __temp_9_979*__temp_9_980*__temp_9_985;
const auto __temp_9_1072 = __temp_9_989*__temp_9_990*__temp_9_975;
const auto __temp_9_1073 = __temp_9_1071+__temp_9_1072;
const auto __temp_9_1074 = __temp_9_953*__temp_9_956*__temp_9_1073*__temp_9_955;
const auto __temp_9_1075 = __temp_9_971*__temp_9_953*__temp_9_985*__temp_9_975;
const auto __temp_9_1076 = __temp_9_1074+__temp_9_1075;
const auto __temp_9_1077 = __temp_9_979*__temp_9_980*__temp_9_999;
const auto __temp_9_1078 = __temp_9_995*__temp_9_997*__temp_9_975;
const auto __temp_9_1079 = __temp_9_1077+__temp_9_1078;
const auto __temp_9_1080 = __temp_9_953*__temp_9_956*__temp_9_1079*__temp_9_955;
const auto __temp_9_1081 = __temp_9_971*__temp_9_953*__temp_9_999*__temp_9_975;
const auto __temp_9_1082 = __temp_9_1080+__temp_9_1081;
const auto __temp_9_1083 = __temp_9_971*__temp_9_953*__temp_9_1005*__temp_9_975;
const auto __temp_9_1084 = __temp_9_979*__temp_9_980*__temp_9_1005;
const auto __temp_9_1085 = __temp_9_1008*__temp_9_1010*__temp_9_975;
const auto __temp_9_1086 = __temp_9_1084+__temp_9_1085;
const auto __temp_9_1087 = __temp_9_953*__temp_9_956*__temp_9_1086*__temp_9_955;
const auto __temp_9_1088 = __temp_9_1083+__temp_9_1087;
const auto __temp_9_1089 = __temp_9_979*__temp_9_980*__temp_9_1019;
const auto __temp_9_1090 = __temp_9_1016*__temp_9_1017*__temp_9_975;
const auto __temp_9_1091 = __temp_9_1089+__temp_9_1090;
const auto __temp_9_1092 = __temp_9_953*__temp_9_1091*__temp_9_956*__temp_9_955;
const auto __temp_9_1093 = __temp_9_971*__temp_9_953*__temp_9_1019*__temp_9_975;
const auto __temp_9_1094 = __temp_9_1092+__temp_9_1093;
const auto __temp_9_1095 = __temp_9_979*__temp_9_980*__temp_9_1029;
const auto __temp_9_1096 = __temp_9_1025*__temp_9_1027*__temp_9_975;
const auto __temp_9_1097 = __temp_9_1095+__temp_9_1096;
const auto __temp_9_1098 = __temp_9_953*__temp_9_956*__temp_9_1097*__temp_9_955;
const auto __temp_9_1099 = __temp_9_971*__temp_9_953*__temp_9_1029*__temp_9_975;
const auto __temp_9_1100 = __temp_9_1098+__temp_9_1099;
const auto __temp_9_1101 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_975;
const auto __temp_9_1102 = __temp_9_979*__temp_9_1035*__temp_9_980;
const auto __temp_9_1103 = __temp_9_1039*__temp_9_1040*__temp_9_975;
const auto __temp_9_1104 = __temp_9_1102+__temp_9_1103;
const auto __temp_9_1105 = __temp_9_953*__temp_9_956*__temp_9_1104*__temp_9_955;
const auto __temp_9_1106 = __temp_9_1101+__temp_9_1105;
const auto __temp_9_1107 = __temp_9_1047*__temp_9_975*__temp_9_1049;
const auto __temp_9_1108 = __temp_9_1045*__temp_9_979*__temp_9_980;
const auto __temp_9_1109 = __temp_9_1107+__temp_9_1108;
const auto __temp_9_1110 = __temp_9_1109*__temp_9_953*__temp_9_956*__temp_9_955;
const auto __temp_9_1111 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_975;
const auto __temp_9_1112 = __temp_9_1110+__temp_9_1111;
const auto __temp_9_1113 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_975;
const auto __temp_9_1114 = __temp_9_1060*__temp_9_979*__temp_9_980;
const auto __temp_9_1115 = __temp_9_1055*__temp_9_1058*__temp_9_975;
const auto __temp_9_1116 = __temp_9_1114+__temp_9_1115;
const auto __temp_9_1117 = __temp_9_953*__temp_9_956*__temp_9_1116*__temp_9_955;
const auto __temp_9_1118 = __temp_9_1113+__temp_9_1117;
const auto __temp_9_1119 = __temp_9_953*__temp_9_989*__temp_9_956*__temp_9_990*__temp_9_985*__temp_9_952;
const auto __temp_9_1120 = __temp_9_985*__temp_9_985;
const auto __temp_9_1121 = __temp_9_971*__temp_9_953*__temp_9_1120;
const auto __temp_9_1122 = __temp_9_989*__temp_9_956*__temp_9_955;
const auto __temp_9_1123 = __temp_9_1119+__temp_9_1121+__temp_9_968+__temp_9_1122;
const auto __temp_9_1124 = __temp_9_971*__temp_9_953*__temp_9_999*__temp_9_985;
const auto __temp_9_1125 = __temp_9_995*__temp_9_997*__temp_9_985;
const auto __temp_9_1126 = __temp_9_989*__temp_9_990*__temp_9_999;
const auto __temp_9_1127 = __temp_9_1125+__temp_9_1126;
const auto __temp_9_1128 = __temp_9_953*__temp_9_956*__temp_9_1127*__temp_9_955;
const auto __temp_9_1129 = __temp_9_1124+__temp_9_1128;
const auto __temp_9_1130 = __temp_9_1008*__temp_9_1010*__temp_9_985;
const auto __temp_9_1131 = __temp_9_989*__temp_9_990*__temp_9_1005;
const auto __temp_9_1132 = __temp_9_1130+__temp_9_1131;
const auto __temp_9_1133 = __temp_9_953*__temp_9_1132*__temp_9_956*__temp_9_955;
const auto __temp_9_1134 = __temp_9_971*__temp_9_953*__temp_9_1005*__temp_9_985;
const auto __temp_9_1135 = __temp_9_1133+__temp_9_1134;
const auto __temp_9_1136 = __temp_9_971*__temp_9_953*__temp_9_1019*__temp_9_985;
const auto __temp_9_1137 = __temp_9_989*__temp_9_990*__temp_9_1019;
const auto __temp_9_1138 = __temp_9_1016*__temp_9_1017*__temp_9_985;
const auto __temp_9_1139 = __temp_9_1137+__temp_9_1138;
const auto __temp_9_1140 = __temp_9_953*__temp_9_956*__temp_9_1139*__temp_9_955;
const auto __temp_9_1141 = __temp_9_1136+__temp_9_1140;
const auto __temp_9_1142 = __temp_9_989*__temp_9_990*__temp_9_1029;
const auto __temp_9_1143 = __temp_9_1025*__temp_9_1027*__temp_9_985;
const auto __temp_9_1144 = __temp_9_1142+__temp_9_1143;
const auto __temp_9_1145 = __temp_9_953*__temp_9_1144*__temp_9_956*__temp_9_955;
const auto __temp_9_1146 = __temp_9_971*__temp_9_953*__temp_9_1029*__temp_9_985;
const auto __temp_9_1147 = __temp_9_1145+__temp_9_1146;
const auto __temp_9_1148 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_985;
const auto __temp_9_1149 = __temp_9_989*__temp_9_990*__temp_9_1035;
const auto __temp_9_1150 = __temp_9_1039*__temp_9_1040*__temp_9_985;
const auto __temp_9_1151 = __temp_9_1149+__temp_9_1150;
const auto __temp_9_1152 = __temp_9_953*__temp_9_956*__temp_9_1151*__temp_9_955;
const auto __temp_9_1153 = __temp_9_1148+__temp_9_1152;
const auto __temp_9_1154 = __temp_9_1047*__temp_9_985*__temp_9_1049;
const auto __temp_9_1155 = __temp_9_989*__temp_9_1045*__temp_9_990;
const auto __temp_9_1156 = __temp_9_1154+__temp_9_1155;
const auto __temp_9_1157 = __temp_9_953*__temp_9_956*__temp_9_1156*__temp_9_955;
const auto __temp_9_1158 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_985;
const auto __temp_9_1159 = __temp_9_1157+__temp_9_1158;
const auto __temp_9_1160 = __temp_9_1055*__temp_9_1058*__temp_9_985;
const auto __temp_9_1161 = __temp_9_989*__temp_9_1060*__temp_9_990;
const auto __temp_9_1162 = __temp_9_1160+__temp_9_1161;
const auto __temp_9_1163 = __temp_9_953*__temp_9_1162*__temp_9_956*__temp_9_955;
const auto __temp_9_1164 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_985;
const auto __temp_9_1165 = __temp_9_1163+__temp_9_1164;
const auto __temp_9_1166 = __temp_9_956*__temp_9_997*__temp_9_955;
const auto __temp_9_1167 = __temp_9_953*__temp_9_956*__temp_9_995*__temp_9_999*__temp_9_997*__temp_9_952;
const auto __temp_9_1168 = __temp_9_999*__temp_9_999;
const auto __temp_9_1169 = __temp_9_971*__temp_9_953*__temp_9_1168;
const auto __temp_9_1170 = __temp_9_1166+__temp_9_1167+__temp_9_968+__temp_9_1169;
const auto __temp_9_1171 = __temp_9_1008*__temp_9_1010*__temp_9_999;
const auto __temp_9_1172 = __temp_9_995*__temp_9_997*__temp_9_1005;
const auto __temp_9_1173 = __temp_9_1171+__temp_9_1172;
const auto __temp_9_1174 = __temp_9_953*__temp_9_956*__temp_9_1173*__temp_9_955;
const auto __temp_9_1175 = __temp_9_971*__temp_9_953*__temp_9_999*__temp_9_1005;
const auto __temp_9_1176 = __temp_9_1174+__temp_9_1175;
const auto __temp_9_1177 = __temp_9_971*__temp_9_953*__temp_9_1019*__temp_9_999;
const auto __temp_9_1178 = __temp_9_995*__temp_9_1019*__temp_9_997;
const auto __temp_9_1179 = __temp_9_1016*__temp_9_1017*__temp_9_999;
const auto __temp_9_1180 = __temp_9_1178+__temp_9_1179;
const auto __temp_9_1181 = __temp_9_953*__temp_9_956*__temp_9_1180*__temp_9_955;
const auto __temp_9_1182 = __temp_9_1177+__temp_9_1181;
const auto __temp_9_1183 = __temp_9_971*__temp_9_953*__temp_9_1029*__temp_9_999;
const auto __temp_9_1184 = __temp_9_995*__temp_9_1029*__temp_9_997;
const auto __temp_9_1185 = __temp_9_1025*__temp_9_1027*__temp_9_999;
const auto __temp_9_1186 = __temp_9_1184+__temp_9_1185;
const auto __temp_9_1187 = __temp_9_953*__temp_9_1186*__temp_9_956*__temp_9_955;
const auto __temp_9_1188 = __temp_9_1183+__temp_9_1187;
const auto __temp_9_1189 = __temp_9_1039*__temp_9_1040*__temp_9_999;
const auto __temp_9_1190 = __temp_9_995*__temp_9_1035*__temp_9_997;
const auto __temp_9_1191 = __temp_9_1189+__temp_9_1190;
const auto __temp_9_1192 = __temp_9_1191*__temp_9_953*__temp_9_956*__temp_9_955;
const auto __temp_9_1193 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_999;
const auto __temp_9_1194 = __temp_9_1192+__temp_9_1193;
const auto __temp_9_1195 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_999;
const auto __temp_9_1196 = __temp_9_995*__temp_9_1045*__temp_9_997;
const auto __temp_9_1197 = __temp_9_1047*__temp_9_999*__temp_9_1049;
const auto __temp_9_1198 = __temp_9_1196+__temp_9_1197;
const auto __temp_9_1199 = __temp_9_953*__temp_9_956*__temp_9_1198*__temp_9_955;
const auto __temp_9_1200 = __temp_9_1195+__temp_9_1199;
const auto __temp_9_1201 = __temp_9_1060*__temp_9_995*__temp_9_997;
const auto __temp_9_1202 = __temp_9_1055*__temp_9_1058*__temp_9_999;
const auto __temp_9_1203 = __temp_9_1201+__temp_9_1202;
const auto __temp_9_1204 = __temp_9_953*__temp_9_1203*__temp_9_956*__temp_9_955;
const auto __temp_9_1205 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_999;
const auto __temp_9_1206 = __temp_9_1204+__temp_9_1205;
const auto __temp_9_1207 = __temp_9_1005*__temp_9_1005;
const auto __temp_9_1208 = __temp_9_971*__temp_9_953*__temp_9_1207;
const auto __temp_9_1209 = __temp_9_956*__temp_9_1010*__temp_9_955;
const auto __temp_9_1210 = __temp_9_953*__temp_9_1008*__temp_9_956*__temp_9_1010*__temp_9_1005*__temp_9_952;
const auto __temp_9_1211 = __temp_9_1208+__temp_9_968+__temp_9_1209+__temp_9_1210;
const auto __temp_9_1212 = __temp_9_971*__temp_9_953*__temp_9_1019*__temp_9_1005;
const auto __temp_9_1213 = __temp_9_1016*__temp_9_1017*__temp_9_1005;
const auto __temp_9_1214 = __temp_9_1008*__temp_9_1010*__temp_9_1019;
const auto __temp_9_1215 = __temp_9_1213+__temp_9_1214;
const auto __temp_9_1216 = __temp_9_953*__temp_9_956*__temp_9_1215*__temp_9_955;
const auto __temp_9_1217 = __temp_9_1212+__temp_9_1216;
const auto __temp_9_1218 = __temp_9_1008*__temp_9_1010*__temp_9_1029;
const auto __temp_9_1219 = __temp_9_1025*__temp_9_1027*__temp_9_1005;
const auto __temp_9_1220 = __temp_9_1218+__temp_9_1219;
const auto __temp_9_1221 = __temp_9_953*__temp_9_1220*__temp_9_956*__temp_9_955;
const auto __temp_9_1222 = __temp_9_971*__temp_9_953*__temp_9_1029*__temp_9_1005;
const auto __temp_9_1223 = __temp_9_1221+__temp_9_1222;
const auto __temp_9_1224 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_1005;
const auto __temp_9_1225 = __temp_9_1008*__temp_9_1010*__temp_9_1035;
const auto __temp_9_1226 = __temp_9_1039*__temp_9_1040*__temp_9_1005;
const auto __temp_9_1227 = __temp_9_1225+__temp_9_1226;
const auto __temp_9_1228 = __temp_9_953*__temp_9_956*__temp_9_1227*__temp_9_955;
const auto __temp_9_1229 = __temp_9_1224+__temp_9_1228;
const auto __temp_9_1230 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_1005;
const auto __temp_9_1231 = __temp_9_1047*__temp_9_1005*__temp_9_1049;
const auto __temp_9_1232 = __temp_9_1008*__temp_9_1010*__temp_9_1045;
const auto __temp_9_1233 = __temp_9_1231+__temp_9_1232;
const auto __temp_9_1234 = __temp_9_953*__temp_9_1233*__temp_9_956*__temp_9_955;
const auto __temp_9_1235 = __temp_9_1230+__temp_9_1234;
const auto __temp_9_1236 = __temp_9_1055*__temp_9_1058*__temp_9_1005;
const auto __temp_9_1237 = __temp_9_1008*__temp_9_1010*__temp_9_1060;
const auto __temp_9_1238 = __temp_9_1236+__temp_9_1237;
const auto __temp_9_1239 = __temp_9_953*__temp_9_956*__temp_9_1238*__temp_9_955;
const auto __temp_9_1240 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_1005;
const auto __temp_9_1241 = __temp_9_1239+__temp_9_1240;
const auto __temp_9_1242 = __temp_9_1016*__temp_9_956*__temp_9_955;
const auto __temp_9_1243 = __temp_9_1016*__temp_9_953*__temp_9_956*__temp_9_1017*__temp_9_1019*__temp_9_952;
const auto __temp_9_1244 = __temp_9_1019*__temp_9_1019;
const auto __temp_9_1245 = __temp_9_971*__temp_9_953*__temp_9_1244;
const auto __temp_9_1246 = __temp_9_1242+__temp_9_968+__temp_9_1243+__temp_9_1245;
const auto __temp_9_1247 = __temp_9_971*__temp_9_953*__temp_9_1029*__temp_9_1019;
const auto __temp_9_1248 = __temp_9_1025*__temp_9_1027*__temp_9_1019;
const auto __temp_9_1249 = __temp_9_1016*__temp_9_1017*__temp_9_1029;
const auto __temp_9_1250 = __temp_9_1248+__temp_9_1249;
const auto __temp_9_1251 = __temp_9_953*__temp_9_1250*__temp_9_956*__temp_9_955;
const auto __temp_9_1252 = __temp_9_1247+__temp_9_1251;
const auto __temp_9_1253 = __temp_9_1039*__temp_9_1040*__temp_9_1019;
const auto __temp_9_1254 = __temp_9_1016*__temp_9_1017*__temp_9_1035;
const auto __temp_9_1255 = __temp_9_1253+__temp_9_1254;
const auto __temp_9_1256 = __temp_9_953*__temp_9_956*__temp_9_1255*__temp_9_955;
const auto __temp_9_1257 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_1019;
const auto __temp_9_1258 = __temp_9_1256+__temp_9_1257;
const auto __temp_9_1259 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_1019;
const auto __temp_9_1260 = __temp_9_1016*__temp_9_1017*__temp_9_1045;
const auto __temp_9_1261 = __temp_9_1047*__temp_9_1019*__temp_9_1049;
const auto __temp_9_1262 = __temp_9_1260+__temp_9_1261;
const auto __temp_9_1263 = __temp_9_953*__temp_9_956*__temp_9_1262*__temp_9_955;
const auto __temp_9_1264 = __temp_9_1259+__temp_9_1263;
const auto __temp_9_1265 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_1019;
const auto __temp_9_1266 = __temp_9_1016*__temp_9_1017*__temp_9_1060;
const auto __temp_9_1267 = __temp_9_1055*__temp_9_1058*__temp_9_1019;
const auto __temp_9_1268 = __temp_9_1266+__temp_9_1267;
const auto __temp_9_1269 = __temp_9_953*__temp_9_1268*__temp_9_956*__temp_9_955;
const auto __temp_9_1270 = __temp_9_1265+__temp_9_1269;
const auto __temp_9_1271 = __temp_9_956*__temp_9_1027*__temp_9_955;
const auto __temp_9_1272 = __temp_9_1029*__temp_9_1029;
const auto __temp_9_1273 = __temp_9_971*__temp_9_953*__temp_9_1272;
const auto __temp_9_1274 = __temp_9_953*__temp_9_1025*__temp_9_956*__temp_9_1027*__temp_9_1029*__temp_9_952;
const auto __temp_9_1275 = __temp_9_968+__temp_9_1271+__temp_9_1273+__temp_9_1274;
const auto __temp_9_1276 = __temp_9_1025*__temp_9_1035*__temp_9_1027;
const auto __temp_9_1277 = __temp_9_1039*__temp_9_1040*__temp_9_1029;
const auto __temp_9_1278 = __temp_9_1276+__temp_9_1277;
const auto __temp_9_1279 = __temp_9_953*__temp_9_956*__temp_9_1278*__temp_9_955;
const auto __temp_9_1280 = __temp_9_971*__temp_9_953*__temp_9_1035*__temp_9_1029;
const auto __temp_9_1281 = __temp_9_1279+__temp_9_1280;
const auto __temp_9_1282 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_1029;
const auto __temp_9_1283 = __temp_9_1025*__temp_9_1045*__temp_9_1027;
const auto __temp_9_1284 = __temp_9_1047*__temp_9_1029*__temp_9_1049;
const auto __temp_9_1285 = __temp_9_1283+__temp_9_1284;
const auto __temp_9_1286 = __temp_9_953*__temp_9_956*__temp_9_1285*__temp_9_955;
const auto __temp_9_1287 = __temp_9_1282+__temp_9_1286;
const auto __temp_9_1288 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_1029;
const auto __temp_9_1289 = __temp_9_1025*__temp_9_1060*__temp_9_1027;
const auto __temp_9_1290 = __temp_9_1055*__temp_9_1058*__temp_9_1029;
const auto __temp_9_1291 = __temp_9_1289+__temp_9_1290;
const auto __temp_9_1292 = __temp_9_953*__temp_9_956*__temp_9_1291*__temp_9_955;
const auto __temp_9_1293 = __temp_9_1288+__temp_9_1292;
const auto __temp_9_1294 = __temp_9_1039*__temp_9_956*__temp_9_955;
const auto __temp_9_1295 = __temp_9_953*__temp_9_1039*__temp_9_1040*__temp_9_956*__temp_9_1035*__temp_9_952;
const auto __temp_9_1296 = __temp_9_1035*__temp_9_1035;
const auto __temp_9_1297 = __temp_9_971*__temp_9_953*__temp_9_1296;
const auto __temp_9_1298 = __temp_9_1294+__temp_9_1295+__temp_9_968+__temp_9_1297;
const auto __temp_9_1299 = __temp_9_1047*__temp_9_1035*__temp_9_1049;
const auto __temp_9_1300 = __temp_9_1039*__temp_9_1040*__temp_9_1045;
const auto __temp_9_1301 = __temp_9_1299+__temp_9_1300;
const auto __temp_9_1302 = __temp_9_953*__temp_9_1301*__temp_9_956*__temp_9_955;
const auto __temp_9_1303 = __temp_9_971*__temp_9_953*__temp_9_1045*__temp_9_1035;
const auto __temp_9_1304 = __temp_9_1302+__temp_9_1303;
const auto __temp_9_1305 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_1035;
const auto __temp_9_1306 = __temp_9_1055*__temp_9_1035*__temp_9_1058;
const auto __temp_9_1307 = __temp_9_1039*__temp_9_1040*__temp_9_1060;
const auto __temp_9_1308 = __temp_9_1306+__temp_9_1307;
const auto __temp_9_1309 = __temp_9_953*__temp_9_956*__temp_9_1308*__temp_9_955;
const auto __temp_9_1310 = __temp_9_1305+__temp_9_1309;
const auto __temp_9_1311 = __temp_9_953*__temp_9_1047*__temp_9_956*__temp_9_1045*__temp_9_1049*__temp_9_952;
const auto __temp_9_1312 = __temp_9_956*__temp_9_1049*__temp_9_955;
const auto __temp_9_1313 = __temp_9_1045*__temp_9_1045;
const auto __temp_9_1314 = __temp_9_971*__temp_9_953*__temp_9_1313;
const auto __temp_9_1315 = __temp_9_1311+__temp_9_968+__temp_9_1312+__temp_9_1314;
const auto __temp_9_1316 = __temp_9_971*__temp_9_953*__temp_9_1060*__temp_9_1045;
const auto __temp_9_1317 = __temp_9_1055*__temp_9_1045*__temp_9_1058;
const auto __temp_9_1318 = __temp_9_1047*__temp_9_1060*__temp_9_1049;
const auto __temp_9_1319 = __temp_9_1317+__temp_9_1318;
const auto __temp_9_1320 = __temp_9_1319*__temp_9_953*__temp_9_956*__temp_9_955;
const auto __temp_9_1321 = __temp_9_1316+__temp_9_1320;
const auto __temp_9_1322 = __temp_9_956*__temp_9_1058*__temp_9_955;
const auto __temp_9_1323 = __temp_9_953*__temp_9_1055*__temp_9_956*__temp_9_1060*__temp_9_1058*__temp_9_952;
const auto __temp_9_1324 = __temp_9_1060*__temp_9_1060;
const auto __temp_9_1325 = __temp_9_971*__temp_9_953*__temp_9_1324;
const auto __temp_9_1326 = __temp_9_1322+__temp_9_1323+__temp_9_1325+__temp_9_968;
        // -- END -- temporary pool (sequence=9)

        __u2.clear();
        __u2.resize(2*10);

        for(int __i = 0; __i < 2*10; __i++)
	        {
            __u2[__i].resize(2*10);
	        }

        __u2[0][0] = __temp_9_949;
        __u2[0][1] = __temp_9_949;
        __u2[0][2] = __temp_9_949;
        __u2[0][3] = __temp_9_949;
        __u2[0][4] = __temp_9_949;
        __u2[0][5] = __temp_9_949;
        __u2[0][6] = __temp_9_949;
        __u2[0][7] = __temp_9_949;
        __u2[0][8] = __temp_9_949;
        __u2[0][9] = __temp_9_949;
        __u2[0][10] = __temp_9_950;
        __u2[0][11] = __temp_9_949;
        __u2[0][12] = __temp_9_949;
        __u2[0][13] = __temp_9_949;
        __u2[0][14] = __temp_9_949;
        __u2[0][15] = __temp_9_949;
        __u2[0][16] = __temp_9_949;
        __u2[0][17] = __temp_9_949;
        __u2[0][18] = __temp_9_949;
        __u2[0][19] = __temp_9_949;
        __u2[1][0] = __temp_9_949;
        __u2[1][1] = __temp_9_949;
        __u2[1][2] = __temp_9_949;
        __u2[1][3] = __temp_9_949;
        __u2[1][4] = __temp_9_949;
        __u2[1][5] = __temp_9_949;
        __u2[1][6] = __temp_9_949;
        __u2[1][7] = __temp_9_949;
        __u2[1][8] = __temp_9_949;
        __u2[1][9] = __temp_9_949;
        __u2[1][10] = __temp_9_949;
        __u2[1][11] = __temp_9_950;
        __u2[1][12] = __temp_9_949;
        __u2[1][13] = __temp_9_949;
        __u2[1][14] = __temp_9_949;
        __u2[1][15] = __temp_9_949;
        __u2[1][16] = __temp_9_949;
        __u2[1][17] = __temp_9_949;
        __u2[1][18] = __temp_9_949;
        __u2[1][19] = __temp_9_949;
        __u2[2][0] = __temp_9_949;
        __u2[2][1] = __temp_9_949;
        __u2[2][2] = __temp_9_949;
        __u2[2][3] = __temp_9_949;
        __u2[2][4] = __temp_9_949;
        __u2[2][5] = __temp_9_949;
        __u2[2][6] = __temp_9_949;
        __u2[2][7] = __temp_9_949;
        __u2[2][8] = __temp_9_949;
        __u2[2][9] = __temp_9_949;
        __u2[2][10] = __temp_9_949;
        __u2[2][11] = __temp_9_949;
        __u2[2][12] = __temp_9_950;
        __u2[2][13] = __temp_9_949;
        __u2[2][14] = __temp_9_949;
        __u2[2][15] = __temp_9_949;
        __u2[2][16] = __temp_9_949;
        __u2[2][17] = __temp_9_949;
        __u2[2][18] = __temp_9_949;
        __u2[2][19] = __temp_9_949;
        __u2[3][0] = __temp_9_949;
        __u2[3][1] = __temp_9_949;
        __u2[3][2] = __temp_9_949;
        __u2[3][3] = __temp_9_949;
        __u2[3][4] = __temp_9_949;
        __u2[3][5] = __temp_9_949;
        __u2[3][6] = __temp_9_949;
        __u2[3][7] = __temp_9_949;
        __u2[3][8] = __temp_9_949;
        __u2[3][9] = __temp_9_949;
        __u2[3][10] = __temp_9_949;
        __u2[3][11] = __temp_9_949;
        __u2[3][12] = __temp_9_949;
        __u2[3][13] = __temp_9_950;
        __u2[3][14] = __temp_9_949;
        __u2[3][15] = __temp_9_949;
        __u2[3][16] = __temp_9_949;
        __u2[3][17] = __temp_9_949;
        __u2[3][18] = __temp_9_949;
        __u2[3][19] = __temp_9_949;
        __u2[4][0] = __temp_9_949;
        __u2[4][1] = __temp_9_949;
        __u2[4][2] = __temp_9_949;
        __u2[4][3] = __temp_9_949;
        __u2[4][4] = __temp_9_949;
        __u2[4][5] = __temp_9_949;
        __u2[4][6] = __temp_9_949;
        __u2[4][7] = __temp_9_949;
        __u2[4][8] = __temp_9_949;
        __u2[4][9] = __temp_9_949;
        __u2[4][10] = __temp_9_949;
        __u2[4][11] = __temp_9_949;
        __u2[4][12] = __temp_9_949;
        __u2[4][13] = __temp_9_949;
        __u2[4][14] = __temp_9_950;
        __u2[4][15] = __temp_9_949;
        __u2[4][16] = __temp_9_949;
        __u2[4][17] = __temp_9_949;
        __u2[4][18] = __temp_9_949;
        __u2[4][19] = __temp_9_949;
        __u2[5][0] = __temp_9_949;
        __u2[5][1] = __temp_9_949;
        __u2[5][2] = __temp_9_949;
        __u2[5][3] = __temp_9_949;
        __u2[5][4] = __temp_9_949;
        __u2[5][5] = __temp_9_949;
        __u2[5][6] = __temp_9_949;
        __u2[5][7] = __temp_9_949;
        __u2[5][8] = __temp_9_949;
        __u2[5][9] = __temp_9_949;
        __u2[5][10] = __temp_9_949;
        __u2[5][11] = __temp_9_949;
        __u2[5][12] = __temp_9_949;
        __u2[5][13] = __temp_9_949;
        __u2[5][14] = __temp_9_949;
        __u2[5][15] = __temp_9_950;
        __u2[5][16] = __temp_9_949;
        __u2[5][17] = __temp_9_949;
        __u2[5][18] = __temp_9_949;
        __u2[5][19] = __temp_9_949;
        __u2[6][0] = __temp_9_949;
        __u2[6][1] = __temp_9_949;
        __u2[6][2] = __temp_9_949;
        __u2[6][3] = __temp_9_949;
        __u2[6][4] = __temp_9_949;
        __u2[6][5] = __temp_9_949;
        __u2[6][6] = __temp_9_949;
        __u2[6][7] = __temp_9_949;
        __u2[6][8] = __temp_9_949;
        __u2[6][9] = __temp_9_949;
        __u2[6][10] = __temp_9_949;
        __u2[6][11] = __temp_9_949;
        __u2[6][12] = __temp_9_949;
        __u2[6][13] = __temp_9_949;
        __u2[6][14] = __temp_9_949;
        __u2[6][15] = __temp_9_949;
        __u2[6][16] = __temp_9_950;
        __u2[6][17] = __temp_9_949;
        __u2[6][18] = __temp_9_949;
        __u2[6][19] = __temp_9_949;
        __u2[7][0] = __temp_9_949;
        __u2[7][1] = __temp_9_949;
        __u2[7][2] = __temp_9_949;
        __u2[7][3] = __temp_9_949;
        __u2[7][4] = __temp_9_949;
        __u2[7][5] = __temp_9_949;
        __u2[7][6] = __temp_9_949;
        __u2[7][7] = __temp_9_949;
        __u2[7][8] = __temp_9_949;
        __u2[7][9] = __temp_9_949;
        __u2[7][10] = __temp_9_949;
        __u2[7][11] = __temp_9_949;
        __u2[7][12] = __temp_9_949;
        __u2[7][13] = __temp_9_949;
        __u2[7][14] = __temp_9_949;
        __u2[7][15] = __temp_9_949;
        __u2[7][16] = __temp_9_949;
        __u2[7][17] = __temp_9_950;
        __u2[7][18] = __temp_9_949;
        __u2[7][19] = __temp_9_949;
        __u2[8][0] = __temp_9_949;
        __u2[8][1] = __temp_9_949;
        __u2[8][2] = __temp_9_949;
        __u2[8][3] = __temp_9_949;
        __u2[8][4] = __temp_9_949;
        __u2[8][5] = __temp_9_949;
        __u2[8][6] = __temp_9_949;
        __u2[8][7] = __temp_9_949;
        __u2[8][8] = __temp_9_949;
        __u2[8][9] = __temp_9_949;
        __u2[8][10] = __temp_9_949;
        __u2[8][11] = __temp_9_949;
        __u2[8][12] = __temp_9_949;
        __u2[8][13] = __temp_9_949;
        __u2[8][14] = __temp_9_949;
        __u2[8][15] = __temp_9_949;
        __u2[8][16] = __temp_9_949;
        __u2[8][17] = __temp_9_949;
        __u2[8][18] = __temp_9_950;
        __u2[8][19] = __temp_9_949;
        __u2[9][0] = __temp_9_949;
        __u2[9][1] = __temp_9_949;
        __u2[9][2] = __temp_9_949;
        __u2[9][3] = __temp_9_949;
        __u2[9][4] = __temp_9_949;
        __u2[9][5] = __temp_9_949;
        __u2[9][6] = __temp_9_949;
        __u2[9][7] = __temp_9_949;
        __u2[9][8] = __temp_9_949;
        __u2[9][9] = __temp_9_949;
        __u2[9][10] = __temp_9_949;
        __u2[9][11] = __temp_9_949;
        __u2[9][12] = __temp_9_949;
        __u2[9][13] = __temp_9_949;
        __u2[9][14] = __temp_9_949;
        __u2[9][15] = __temp_9_949;
        __u2[9][16] = __temp_9_949;
        __u2[9][17] = __temp_9_949;
        __u2[9][18] = __temp_9_949;
        __u2[9][19] = __temp_9_950;
        __u2[10][0] = __temp_9_974;
        __u2[10][1] = __temp_9_984;
        __u2[10][2] = __temp_9_994;
        __u2[10][3] = __temp_9_1004;
        __u2[10][4] = __temp_9_1014;
        __u2[10][5] = __temp_9_1024;
        __u2[10][6] = __temp_9_1034;
        __u2[10][7] = __temp_9_1044;
        __u2[10][8] = __temp_9_1054;
        __u2[10][9] = __temp_9_1065;
        __u2[10][10] = __temp_9_971;
        __u2[10][11] = __temp_9_949;
        __u2[10][12] = __temp_9_949;
        __u2[10][13] = __temp_9_949;
        __u2[10][14] = __temp_9_949;
        __u2[10][15] = __temp_9_949;
        __u2[10][16] = __temp_9_949;
        __u2[10][17] = __temp_9_949;
        __u2[10][18] = __temp_9_949;
        __u2[10][19] = __temp_9_949;
        __u2[11][0] = __temp_9_984;
        __u2[11][1] = __temp_9_1070;
        __u2[11][2] = __temp_9_1076;
        __u2[11][3] = __temp_9_1082;
        __u2[11][4] = __temp_9_1088;
        __u2[11][5] = __temp_9_1094;
        __u2[11][6] = __temp_9_1100;
        __u2[11][7] = __temp_9_1106;
        __u2[11][8] = __temp_9_1112;
        __u2[11][9] = __temp_9_1118;
        __u2[11][10] = __temp_9_949;
        __u2[11][11] = __temp_9_971;
        __u2[11][12] = __temp_9_949;
        __u2[11][13] = __temp_9_949;
        __u2[11][14] = __temp_9_949;
        __u2[11][15] = __temp_9_949;
        __u2[11][16] = __temp_9_949;
        __u2[11][17] = __temp_9_949;
        __u2[11][18] = __temp_9_949;
        __u2[11][19] = __temp_9_949;
        __u2[12][0] = __temp_9_994;
        __u2[12][1] = __temp_9_1076;
        __u2[12][2] = __temp_9_1123;
        __u2[12][3] = __temp_9_1129;
        __u2[12][4] = __temp_9_1135;
        __u2[12][5] = __temp_9_1141;
        __u2[12][6] = __temp_9_1147;
        __u2[12][7] = __temp_9_1153;
        __u2[12][8] = __temp_9_1159;
        __u2[12][9] = __temp_9_1165;
        __u2[12][10] = __temp_9_949;
        __u2[12][11] = __temp_9_949;
        __u2[12][12] = __temp_9_971;
        __u2[12][13] = __temp_9_949;
        __u2[12][14] = __temp_9_949;
        __u2[12][15] = __temp_9_949;
        __u2[12][16] = __temp_9_949;
        __u2[12][17] = __temp_9_949;
        __u2[12][18] = __temp_9_949;
        __u2[12][19] = __temp_9_949;
        __u2[13][0] = __temp_9_1004;
        __u2[13][1] = __temp_9_1082;
        __u2[13][2] = __temp_9_1129;
        __u2[13][3] = __temp_9_1170;
        __u2[13][4] = __temp_9_1176;
        __u2[13][5] = __temp_9_1182;
        __u2[13][6] = __temp_9_1188;
        __u2[13][7] = __temp_9_1194;
        __u2[13][8] = __temp_9_1200;
        __u2[13][9] = __temp_9_1206;
        __u2[13][10] = __temp_9_949;
        __u2[13][11] = __temp_9_949;
        __u2[13][12] = __temp_9_949;
        __u2[13][13] = __temp_9_971;
        __u2[13][14] = __temp_9_949;
        __u2[13][15] = __temp_9_949;
        __u2[13][16] = __temp_9_949;
        __u2[13][17] = __temp_9_949;
        __u2[13][18] = __temp_9_949;
        __u2[13][19] = __temp_9_949;
        __u2[14][0] = __temp_9_1014;
        __u2[14][1] = __temp_9_1088;
        __u2[14][2] = __temp_9_1135;
        __u2[14][3] = __temp_9_1176;
        __u2[14][4] = __temp_9_1211;
        __u2[14][5] = __temp_9_1217;
        __u2[14][6] = __temp_9_1223;
        __u2[14][7] = __temp_9_1229;
        __u2[14][8] = __temp_9_1235;
        __u2[14][9] = __temp_9_1241;
        __u2[14][10] = __temp_9_949;
        __u2[14][11] = __temp_9_949;
        __u2[14][12] = __temp_9_949;
        __u2[14][13] = __temp_9_949;
        __u2[14][14] = __temp_9_971;
        __u2[14][15] = __temp_9_949;
        __u2[14][16] = __temp_9_949;
        __u2[14][17] = __temp_9_949;
        __u2[14][18] = __temp_9_949;
        __u2[14][19] = __temp_9_949;
        __u2[15][0] = __temp_9_1024;
        __u2[15][1] = __temp_9_1094;
        __u2[15][2] = __temp_9_1141;
        __u2[15][3] = __temp_9_1182;
        __u2[15][4] = __temp_9_1217;
        __u2[15][5] = __temp_9_1246;
        __u2[15][6] = __temp_9_1252;
        __u2[15][7] = __temp_9_1258;
        __u2[15][8] = __temp_9_1264;
        __u2[15][9] = __temp_9_1270;
        __u2[15][10] = __temp_9_949;
        __u2[15][11] = __temp_9_949;
        __u2[15][12] = __temp_9_949;
        __u2[15][13] = __temp_9_949;
        __u2[15][14] = __temp_9_949;
        __u2[15][15] = __temp_9_971;
        __u2[15][16] = __temp_9_949;
        __u2[15][17] = __temp_9_949;
        __u2[15][18] = __temp_9_949;
        __u2[15][19] = __temp_9_949;
        __u2[16][0] = __temp_9_1034;
        __u2[16][1] = __temp_9_1100;
        __u2[16][2] = __temp_9_1147;
        __u2[16][3] = __temp_9_1188;
        __u2[16][4] = __temp_9_1223;
        __u2[16][5] = __temp_9_1252;
        __u2[16][6] = __temp_9_1275;
        __u2[16][7] = __temp_9_1281;
        __u2[16][8] = __temp_9_1287;
        __u2[16][9] = __temp_9_1293;
        __u2[16][10] = __temp_9_949;
        __u2[16][11] = __temp_9_949;
        __u2[16][12] = __temp_9_949;
        __u2[16][13] = __temp_9_949;
        __u2[16][14] = __temp_9_949;
        __u2[16][15] = __temp_9_949;
        __u2[16][16] = __temp_9_971;
        __u2[16][17] = __temp_9_949;
        __u2[16][18] = __temp_9_949;
        __u2[16][19] = __temp_9_949;
        __u2[17][0] = __temp_9_1044;
        __u2[17][1] = __temp_9_1106;
        __u2[17][2] = __temp_9_1153;
        __u2[17][3] = __temp_9_1194;
        __u2[17][4] = __temp_9_1229;
        __u2[17][5] = __temp_9_1258;
        __u2[17][6] = __temp_9_1281;
        __u2[17][7] = __temp_9_1298;
        __u2[17][8] = __temp_9_1304;
        __u2[17][9] = __temp_9_1310;
        __u2[17][10] = __temp_9_949;
        __u2[17][11] = __temp_9_949;
        __u2[17][12] = __temp_9_949;
        __u2[17][13] = __temp_9_949;
        __u2[17][14] = __temp_9_949;
        __u2[17][15] = __temp_9_949;
        __u2[17][16] = __temp_9_949;
        __u2[17][17] = __temp_9_971;
        __u2[17][18] = __temp_9_949;
        __u2[17][19] = __temp_9_949;
        __u2[18][0] = __temp_9_1054;
        __u2[18][1] = __temp_9_1112;
        __u2[18][2] = __temp_9_1159;
        __u2[18][3] = __temp_9_1200;
        __u2[18][4] = __temp_9_1235;
        __u2[18][5] = __temp_9_1264;
        __u2[18][6] = __temp_9_1287;
        __u2[18][7] = __temp_9_1304;
        __u2[18][8] = __temp_9_1315;
        __u2[18][9] = __temp_9_1321;
        __u2[18][10] = __temp_9_949;
        __u2[18][11] = __temp_9_949;
        __u2[18][12] = __temp_9_949;
        __u2[18][13] = __temp_9_949;
        __u2[18][14] = __temp_9_949;
        __u2[18][15] = __temp_9_949;
        __u2[18][16] = __temp_9_949;
        __u2[18][17] = __temp_9_949;
        __u2[18][18] = __temp_9_971;
        __u2[18][19] = __temp_9_949;
        __u2[19][0] = __temp_9_1065;
        __u2[19][1] = __temp_9_1118;
        __u2[19][2] = __temp_9_1165;
        __u2[19][3] = __temp_9_1206;
        __u2[19][4] = __temp_9_1241;
        __u2[19][5] = __temp_9_1270;
        __u2[19][6] = __temp_9_1293;
        __u2[19][7] = __temp_9_1310;
        __u2[19][8] = __temp_9_1321;
        __u2[19][9] = __temp_9_1326;
        __u2[19][10] = __temp_9_949;
        __u2[19][11] = __temp_9_949;
        __u2[19][12] = __temp_9_949;
        __u2[19][13] = __temp_9_949;
        __u2[19][14] = __temp_9_949;
        __u2[19][15] = __temp_9_949;
        __u2[19][16] = __temp_9_949;
        __u2[19][17] = __temp_9_949;
        __u2[19][18] = __temp_9_949;
        __u2[19][19] = __temp_9_971;
	    }


    template <typename number>
    void quadratic10<number>::u3(const parameters<number>& __params,
                               const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                               std::vector< std::vector< std::vector<number> > >& __u3)
	    {
	    }


    template <typename number>
    void quadratic10<number>::A(const parameters<number>& __params,
                              const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                              std::vector< std::vector< std::vector<number> > >& __A)
	    {
	    }


    template <typename number>
    void quadratic10<number>::B(const parameters<number>& __params,
                              const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                              std::vector< std::vector< std::vector<number> > >& __B)
	    {
	    }


    template <typename number>
    void quadratic10<number>::C(const parameters<number>& __params,
                              const std::vector<number>& __fields, double __km, double __kn, double __kr, double __N,
                              std::vector< std::vector< std::vector<number> > >& __C)
	    {
	    }


    template <typename number>
    void quadratic10<number>::backend_process_backg(const background_task<number>* tk, typename model<number>::backg_history& solution, bool silent)
	    {
        assert(tk != nullptr);

//        if(!silent) this->write_task_data(tk, std::cout, 9.9999999999999998e-13, 9.9999999999999998e-13, 1e-10, "runge_kutta_fehlberg78");

        solution.clear();
        solution.reserve(tk->get_time_config_list().size());

        // set up an observer which writes to this history vector
        quadratic10_background_observer<number> obs(solution, tk->get_time_config_list());

        // set up a functor to evolve this system
        quadratic10_background_functor<number> system(tk->get_params());

        auto ics = tk->get_ics_vector();

        backg_state<number> x(quadratic10_pool::backg_state_size);
        x[this->flatten(0)] =  ics[0];
        x[this->flatten(1)] =  ics[1];
        x[this->flatten(2)] =  ics[2];
        x[this->flatten(3)] =  ics[3];
        x[this->flatten(4)] =  ics[4];
        x[this->flatten(5)] =  ics[5];
        x[this->flatten(6)] =  ics[6];
        x[this->flatten(7)] =  ics[7];
        x[this->flatten(8)] =  ics[8];
        x[this->flatten(9)] =  ics[9];
        x[this->flatten(10)] =  ics[10];
        x[this->flatten(11)] =  ics[11];
        x[this->flatten(12)] =  ics[12];
        x[this->flatten(13)] =  ics[13];
        x[this->flatten(14)] =  ics[14];
        x[this->flatten(15)] =  ics[15];
        x[this->flatten(16)] =  ics[16];
        x[this->flatten(17)] =  ics[17];
        x[this->flatten(18)] =  ics[18];
        x[this->flatten(19)] =  ics[19];

        auto times = tk->get_integration_step_times();

        using namespace boost::numeric::odeint;
        integrate_times(make_controlled< runge_kutta_fehlberg78< backg_state<number> > >(1e-12, 1e-12), system, x, times.begin(), times.end(), 1e-10, obs);
	    }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


    template <typename number>
    void quadratic10_background_functor<number>::operator()(const backg_state<number>& __x, backg_state<number>& __dxdt, double __t)
	    {
        const auto m1  = this->params.get_vector()[0];
        const auto m2  = this->params.get_vector()[1];
        const auto m3  = this->params.get_vector()[2];
        const auto m4  = this->params.get_vector()[3];
        const auto m5  = this->params.get_vector()[4];
        const auto m6  = this->params.get_vector()[5];
        const auto m7  = this->params.get_vector()[6];
        const auto m8  = this->params.get_vector()[7];
        const auto m9  = this->params.get_vector()[8];
        const auto m10  = this->params.get_vector()[9];
        const auto phi1 = __x[0];
        const auto phi2 = __x[1];
        const auto phi3 = __x[2];
        const auto phi4 = __x[3];
        const auto phi5 = __x[4];
        const auto phi6 = __x[5];
        const auto phi7 = __x[6];
        const auto phi8 = __x[7];
        const auto phi9 = __x[8];
        const auto phi10 = __x[9];
        const auto __dphi1 = __x[10];
        const auto __dphi2 = __x[11];
        const auto __dphi3 = __x[12];
        const auto __dphi4 = __x[13];
        const auto __dphi5 = __x[14];
        const auto __dphi6 = __x[15];
        const auto __dphi7 = __x[16];
        const auto __dphi8 = __x[17];
        const auto __dphi9 = __x[18];
        const auto __dphi10 = __x[19];
        const auto __Mp              = this->params.get_Mp();

        const auto __Hsq             = -( (phi1*phi1)*(m1*m1)+(phi4*phi4)*(m4*m4)+(phi9*phi9)*(m9*m9)+(phi7*phi7)*(m7*m7)+(m2*m2)*(phi2*phi2)+(phi10*phi10)*((m10*(m10*m10)*(m10*m10))*(m10*(m10*m10)*(m10*m10)))+(m6*m6)*(phi6*phi6)+(phi5*phi5)*(m5*m5)+(m3*m3)*(phi3*phi3)+(m8*m8)*(phi8*phi8))/(__Mp*__Mp)/( 1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9))-6.0);
        const auto __eps             = (1.0/2.0)*1.0/(__Mp*__Mp)*( (__dphi8*__dphi8)+(__dphi7*__dphi7)+(__dphi6*__dphi6)+(__dphi4*__dphi4)+(__dphi5*__dphi5)+(__dphi3*__dphi3)+(__dphi2*__dphi2)+(__dphi10*__dphi10)+(__dphi1*__dphi1)+(__dphi9*__dphi9));

// -- START -- temporary pool (sequence=10) 
const auto __temp_10_1327 = __dphi1;
const auto __temp_10_1328 = __dphi2;
const auto __temp_10_1329 = __dphi3;
const auto __temp_10_1330 = __dphi4;
const auto __temp_10_1331 = __dphi5;
const auto __temp_10_1332 = __dphi6;
const auto __temp_10_1333 = __dphi7;
const auto __temp_10_1334 = __dphi8;
const auto __temp_10_1335 = __dphi9;
const auto __temp_10_1336 = __dphi10;
const auto __temp_10_1337 = __eps;
const auto __temp_10_1338 = -3.0;
const auto __temp_10_1339 = __temp_10_1337+__temp_10_1338;
const auto __temp_10_1340 = __temp_10_1339*__temp_10_1327;
const auto __temp_10_1341 = __Hsq;
const auto __temp_10_1343 = 1.0/__temp_10_1341;
const auto __temp_10_1344 = phi1;
const auto __temp_10_1345 = m1;
const auto __temp_10_1347 = __temp_10_1345*__temp_10_1345;
const auto __temp_10_1342 = -1.0;
const auto __temp_10_1348 = __temp_10_1343*__temp_10_1344*__temp_10_1347*__temp_10_1342;
const auto __temp_10_1349 = __temp_10_1340+__temp_10_1348;
const auto __temp_10_1350 = m2;
const auto __temp_10_1351 = __temp_10_1350*__temp_10_1350;
const auto __temp_10_1352 = phi2;
const auto __temp_10_1353 = __temp_10_1343*__temp_10_1351*__temp_10_1352*__temp_10_1342;
const auto __temp_10_1354 = __temp_10_1328*__temp_10_1339;
const auto __temp_10_1355 = __temp_10_1353+__temp_10_1354;
const auto __temp_10_1356 = __temp_10_1329*__temp_10_1339;
const auto __temp_10_1357 = m3;
const auto __temp_10_1358 = __temp_10_1357*__temp_10_1357;
const auto __temp_10_1359 = phi3;
const auto __temp_10_1360 = __temp_10_1343*__temp_10_1358*__temp_10_1359*__temp_10_1342;
const auto __temp_10_1361 = __temp_10_1356+__temp_10_1360;
const auto __temp_10_1362 = phi4;
const auto __temp_10_1363 = m4;
const auto __temp_10_1364 = __temp_10_1363*__temp_10_1363;
const auto __temp_10_1365 = __temp_10_1343*__temp_10_1362*__temp_10_1364*__temp_10_1342;
const auto __temp_10_1366 = __temp_10_1330*__temp_10_1339;
const auto __temp_10_1367 = __temp_10_1365+__temp_10_1366;
const auto __temp_10_1368 = __temp_10_1331*__temp_10_1339;
const auto __temp_10_1369 = phi5;
const auto __temp_10_1370 = m5;
const auto __temp_10_1371 = __temp_10_1370*__temp_10_1370;
const auto __temp_10_1372 = __temp_10_1343*__temp_10_1369*__temp_10_1371*__temp_10_1342;
const auto __temp_10_1373 = __temp_10_1368+__temp_10_1372;
const auto __temp_10_1374 = __temp_10_1332*__temp_10_1339;
const auto __temp_10_1375 = m6;
const auto __temp_10_1376 = __temp_10_1375*__temp_10_1375;
const auto __temp_10_1377 = phi6;
const auto __temp_10_1378 = __temp_10_1343*__temp_10_1376*__temp_10_1377*__temp_10_1342;
const auto __temp_10_1379 = __temp_10_1374+__temp_10_1378;
const auto __temp_10_1380 = phi7;
const auto __temp_10_1381 = m7;
const auto __temp_10_1382 = __temp_10_1381*__temp_10_1381;
const auto __temp_10_1383 = __temp_10_1343*__temp_10_1380*__temp_10_1382*__temp_10_1342;
const auto __temp_10_1384 = __temp_10_1333*__temp_10_1339;
const auto __temp_10_1385 = __temp_10_1383+__temp_10_1384;
const auto __temp_10_1386 = __temp_10_1334*__temp_10_1339;
const auto __temp_10_1387 = m8;
const auto __temp_10_1388 = __temp_10_1387*__temp_10_1387;
const auto __temp_10_1389 = phi8;
const auto __temp_10_1390 = __temp_10_1343*__temp_10_1388*__temp_10_1389*__temp_10_1342;
const auto __temp_10_1391 = __temp_10_1386+__temp_10_1390;
const auto __temp_10_1392 = phi9;
const auto __temp_10_1393 = m9;
const auto __temp_10_1394 = __temp_10_1393*__temp_10_1393;
const auto __temp_10_1395 = __temp_10_1343*__temp_10_1392*__temp_10_1394*__temp_10_1342;
const auto __temp_10_1396 = __temp_10_1335*__temp_10_1339;
const auto __temp_10_1397 = __temp_10_1395+__temp_10_1396;
const auto __temp_10_1398 = phi10;
const auto __temp_10_1399 = m10;
const auto __temp_10_1401 = pow(__temp_10_1399,10);
const auto __temp_10_1402 = __temp_10_1343*__temp_10_1398*__temp_10_1401*__temp_10_1342;
const auto __temp_10_1403 = __temp_10_1336*__temp_10_1339;
const auto __temp_10_1404 = __temp_10_1402+__temp_10_1403;
        // -- END -- temporary pool (sequence=10)

        __dxdt[this->flatten(0)] = __temp_10_1327;
        __dxdt[this->flatten(1)] = __temp_10_1328;
        __dxdt[this->flatten(2)] = __temp_10_1329;
        __dxdt[this->flatten(3)] = __temp_10_1330;
        __dxdt[this->flatten(4)] = __temp_10_1331;
        __dxdt[this->flatten(5)] = __temp_10_1332;
        __dxdt[this->flatten(6)] = __temp_10_1333;
        __dxdt[this->flatten(7)] = __temp_10_1334;
        __dxdt[this->flatten(8)] = __temp_10_1335;
        __dxdt[this->flatten(9)] = __temp_10_1336;
        __dxdt[this->flatten(10)] = __temp_10_1349;
        __dxdt[this->flatten(11)] = __temp_10_1355;
        __dxdt[this->flatten(12)] = __temp_10_1361;
        __dxdt[this->flatten(13)] = __temp_10_1367;
        __dxdt[this->flatten(14)] = __temp_10_1373;
        __dxdt[this->flatten(15)] = __temp_10_1379;
        __dxdt[this->flatten(16)] = __temp_10_1385;
        __dxdt[this->flatten(17)] = __temp_10_1391;
        __dxdt[this->flatten(18)] = __temp_10_1397;
        __dxdt[this->flatten(19)] = __temp_10_1404;
	    }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


    template <typename number>
    void quadratic10_background_observer<number>::operator()(const backg_state<number>& x, double t)
	    {
        if(this->current_config != this->storage_list.end() && (*(this->current_config)).serial == this->serial)
	        {
            this->history.push_back(x);
            this->current_config++;
	        }
        this->serial++;
	    }


	}   // namespace transport


#endif  // __CPPTRANSPORT_QUADRATIC10_CORE_TWOPF_H_

