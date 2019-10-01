// backend = cpp, minver = 201901, lagrangian = canonical
//
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This template file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// As a special exception, you may create a larger work that contains
// part or all of this template file and distribute that work
// under terms of your choice.  Alternatively, if you modify or redistribute
// this template file itself, you may (at your option) remove this
// special exception, which will cause the template and the resulting
// CppTransport output files to be licensed under the GNU General Public
// License without this special exception.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
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
#include "boost/optional.hpp"

#include "Eigen/Core"

#include "transport-runtime/transport.h"
#include "transport-runtime/models/canonical_model.h"
#include "transport-runtime/models/odeint_defaults.h"


// #define CPPTRANSPORT_INSTRUMENT
// #define CPPTRANSPORT_NO_STRICT_FP_TEST


namespace transport
  {

    // Literal data pool
    namespace $MODEL_pool
      {
        static std::vector<std::string> field_names = $FIELD_NAME_LIST;
        static std::vector<std::string> latex_names = $LATEX_NAME_LIST;
        static std::vector<std::string> param_names = $PARAM_NAME_LIST;
        static std::vector<std::string> platx_names = $PLATX_NAME_LIST;
        static std::vector<std::string> state_names = $STATE_NAME_LIST;

        static std::string              name        = $NAME;
        static std::string              citeguide   = $CITEGUIDE;
        static std::string              description = $DESCRIPTION;
        static std::string              license     = $LICENSE;
        static unsigned int             revision    = $REVISION;

        static std::vector<std::string> references  = $REFERENCES;
        static std::vector<std::string> urls        = $URLS;

        static author_db::value_type    auth_arr[]  = $AUTHORS{"{ $NAME, std::make_unique<author_record>($NAME, $EMAIL, $INSTITUTE) }"};
        static author_db                author(std::make_move_iterator(std::begin(auth_arr)), std::make_move_iterator(std::end(auth_arr)));

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
    
    
        // FLATTENING FUNCTIONS
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
    
      }


#undef DEFINE_INDEX_TOOLS
#define DEFINE_INDEX_TOOLS \
    using $MODEL_pool::SPECIES; \
    using $MODEL_pool::MOMENTUM; \
    using $MODEL_pool::IS_FIELD; \
    using $MODEL_pool::IS_MOMENTUM; \
    using $MODEL_pool::FLATTEN; \
    using $MODEL_pool::FIELDS_FLATTEN; \
    using $MODEL_pool::TENSOR_FLATTEN; \

    
    $PHASE_FLATTEN{FLATTEN}
    $FIELD_FLATTEN{FIELDS_FLATTEN}

    $WORKING_TYPE{number}


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
        $MODEL(local_environment& e, argument_cache& a);

        //! destructor
		    virtual ~$MODEL();


        // EXTRACT MODEL INFORMATION -- implements a 'model' interface

      public:

        const std::string& get_name() const override { return($MODEL_pool::name); }

        const author_db& get_authors() const override { return($MODEL_pool::author); }

        const std::string& get_citeguide() const override { return($MODEL_pool::citeguide); }

        const std::string& get_description() const override { return($MODEL_pool::description); }

        const std::string& get_license() const override { return($MODEL_pool::license); }

        unsigned int get_revision() const override { return($MODEL_pool::revision); }

        const std::vector<std::string>& get_references() const override { return($MODEL_pool::references); }

        const std::vector<std::string>& get_urls() const override { return($MODEL_pool::urls); }

        unsigned int get_N_fields() const override { return($NUMBER_FIELDS); }

        unsigned int get_N_params() const override { return($NUMBER_PARAMS); }

        const std::vector< std::string >& get_field_names() const override { return($MODEL_pool::field_names); }

        const std::vector< std::string >& get_f_latex_names() const override { return($MODEL_pool::latex_names); }

        const std::vector< std::string >& get_param_names() const override { return($MODEL_pool::param_names); }

        const std::vector< std::string >& get_p_latex_names() const override { return($MODEL_pool::platx_names); }

        const std::vector< std::string >& get_state_names() const override { return($MODEL_pool::state_names); }


        // INDEX FLATTENING FUNCTIONS -- implements an 'abstract_flattener' interface

      public:

        unsigned int flatten(unsigned int a)                                         const override { return(a); };
        unsigned int flatten(unsigned int a, unsigned int b)                         const override { return(2*$NUMBER_FIELDS*a + b); };
        unsigned int flatten(unsigned int a, unsigned int b, unsigned int c)         const override { return(2*$NUMBER_FIELDS*2*$NUMBER_FIELDS*a + 2*$NUMBER_FIELDS*b + c); };

        unsigned int fields_flatten(unsigned int a)                                  const override { return(a); };
        unsigned int fields_flatten(unsigned int a, unsigned int b)                  const override { return($NUMBER_FIELDS*a + b); };
        unsigned int fields_flatten(unsigned int a, unsigned int b, unsigned int c)  const override { return($NUMBER_FIELDS*$NUMBER_FIELDS*a + $NUMBER_FIELDS*b + c); };

        unsigned int tensor_flatten(unsigned int a, unsigned int b)                  const override { return(2*a + b); }


        // INDEX TRAITS -- implements an 'abstract_flattener' interface

      public:

        unsigned int species(unsigned int a)                                         const override { return((a >= $NUMBER_FIELDS) ? a-$NUMBER_FIELDS : a); };
        unsigned int momentum(unsigned int a)                                        const override { return((a >= $NUMBER_FIELDS) ? a : a+$NUMBER_FIELDS); };
        unsigned int is_field(unsigned int a)                                        const override { return(a < $NUMBER_FIELDS); }
        unsigned int is_momentum(unsigned int a)                                     const override { return(a >= $NUMBER_FIELDS && a <= 2*$NUMBER_FIELDS); }


        // COMPUTE BASIC PHYSICAL QUANTITIES -- implements a 'model'/'canonical_model' interface

      public:

        // Over-ride functions inherited from 'model'
        number H(const parameters<number>& __params, const flattened_tensor<number>& __coords) const override;
        number epsilon(const parameters<number>& __params, const flattened_tensor<number>& __coords) const override;
        number eta(const parameters<number>& __params, const flattened_tensor<number>& __coords) const override;

        // Over-ride functions inherited from 'canonical_model'
        number V(const parameters<number>& __params, const flattened_tensor<number>& __coords) const override;


        // INITIAL CONDITIONS HANDLING -- implements a 'model' interface

      public:

        void validate_ics(const parameters<number>& p, const flattened_tensor<number>& input, flattened_tensor<number>& output) override;


        // PARAMETER HANDLING -- implements a 'model' interface

      public:

        void validate_params(const flattened_tensor<number>& input, flattened_tensor<number>& output) override;


        // CALCULATE MODEL-SPECIFIC QUANTITIES -- implements a 'model' interface

      public:

        // calculate gauge transformations to zeta
        void compute_gauge_xfm_1(const integration_task<number>* __task, const flattened_tensor<number>& __state, flattened_tensor<number>& __dN) override;
        void compute_gauge_xfm_2(const integration_task<number>* __task, const flattened_tensor<number>& __state, double __k, double __k1, double __k2, double __N, flattened_tensor<number>& __ddN) override;

        // calculate tensor quantities, including the 'flow' tensors u2, u3 and the basic tensors A, B, C from which u3 is built
        void u2(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __k, double __N, flattened_tensor<number>& __u2) override;
        void u3(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn, double __kr, double __N, flattened_tensor<number>& __u3) override;

        void A(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn, double __kr, double __N, flattened_tensor<number>& __A) override;
        void B(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn, double __kr, double __N, flattened_tensor<number>& __B) override;
        void C(const integration_task<number>* __task, const flattened_tensor<number>& __fields, double __km, double __kn, double __kr, double __N, flattened_tensor<number>& __C) override;

        // calculate mass matrix
        void M(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
               flattened_tensor<number>& __M, massmatrix_type __type = massmatrix_type::include_mixing) override;

        // calculate raw mass spectrum
        void mass_spectrum(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
                           bool _norm, flattened_tensor<number>& __M, flattened_tensor<number>& __E,
                           massmatrix_type __type = massmatrix_type::include_mixing) override;

        // calculate the sorted mass spectrum, normalized to H^2 if desired
        void sorted_mass_spectrum(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
                                  bool __norm, flattened_tensor<number>& __M, flattened_tensor<number>& __E,
                                  massmatrix_type __type = massmatrix_type::include_mixing) override;

        // BACKEND INTERFACE (PARTIAL IMPLEMENTATION -- WE PROVIDE A COMMON BACKGROUND INTEGRATOR)

      public:

        void backend_process_backg(const background_task<number>* tk, backg_history<number>& solution, bool silent=false) override;

        double compute_end_of_inflation(const integration_task<number>* tk, double search_time=CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH) override;

		    void compute_aH(const integration_task<number>* tk, std::vector<double>& N,
		                    flattened_tensor<number>& log_aH, flattened_tensor<number>& log_a2H2M,
		                    boost::optional<double> largest_k = boost::none) override;

        void compute_H(const integration_task<number>* tk, std::vector<double>& N,
                       flattened_tensor<number>& log_H, boost::optional<double> largest_k = boost::none) override;


        // CALCULATE INITIAL CONDITIONS FOR N-POINT FUNCTIONS

      protected:

        number make_twopf_re_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields, double __k_norm);

        number make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields, double __k_norm);

        number make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                    const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields, double __k_norm);

        number make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                               double kmode_1, double kmode_2, double kmode_3, double __Ninit,
                               const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields, double __k_norm);


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

        $IF{!fast}
          number* __dV;
          number* __ddV;
          number* __dddV;
        $ENDIF

        number* __raw_params;

        //! workspace: Eigen matrix representing mass matrix
        Eigen::Matrix<number, $NUMBER_FIELDS, $NUMBER_FIELDS> __mass_matrix;

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
            $IF{!fast}
              $RESOURCE_RELEASE
              __dV = new number[$NUMBER_FIELDS];
            $ENDIF

            __raw_params = new number[$NUMBER_PARAMS];
    
            const auto& __pvector = __params.get_vector();
            __raw_params[$1] = __pvector[$1];
          }

        void close_down_workspace()
          {
            $IF{!fast}
              delete[] __dV;
            $ENDIF

            delete[] __raw_params;
          }

        void operator ()(const backg_state<number>& __x, backg_state<number>& __dxdt, number __t);

      protected:

        const parameters<number>& __params;

        $IF{!fast}
          number* __dV;
        $ENDIF

        number* __raw_params;

        number __Mp;

      };


    // integration - observer object for background only
    template <typename number>
    class $MODEL_background_observer
      {

      public:

        $MODEL_background_observer(backg_history<number>& h, const time_config_database& t)
          : history(h),
            time_db(t)
          {
            current_step = time_db.record_begin();
          }

        void operator ()(const backg_state<number>& x, number t);

      private:

        backg_history<number>& history;

        const time_config_database& time_db;

        time_config_database::const_record_iterator current_step;

      };


    // CLASS $MODEL -- CONSTRUCTORS, DESTRUCTORS


    template <typename number>
    $MODEL<number>::$MODEL(local_environment& e, argument_cache& a)
      : canonical_model<number>("$UNIQUE_ID", $NUMERIC_VERSION, e, a)
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

        $IF{!fast}
          __dV = new number[$NUMBER_FIELDS];
          __ddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS];
          __dddV = new number[$NUMBER_FIELDS * $NUMBER_FIELDS * $NUMBER_FIELDS];
        $ENDIF

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

        $IF{!fast}
          delete[] __dV;
          delete[] __ddV;
          delete[] __dddV;
        $ENDIF

        delete[] __raw_params;
      }


    // INTERFACE: COMPUTE BASIC PHYSICAL QUANTITIES


    template <typename number>
    number $MODEL<number>::H(const parameters<number>& __params, const flattened_tensor<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);
        if(__coords.size() != 2*$NUMBER_FIELDS)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_COORDS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << "]";
            throw std::out_of_range(msg.str());
          }
    
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto __Mp = __params.get_Mp();
        const auto& __param_vector = __params.get_vector();

        $RESOURCE_PARAMETERS{__param_vector}
        $RESOURCE_COORDINATES{__coords}

        $TEMP_POOL{"const auto $1 = $2;"}

        return std::sqrt($HUBBLE_SQ);
      }


    template <typename number>
    number $MODEL<number>::epsilon(const parameters<number>& __params, const flattened_tensor<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);
        if(__coords.size() != 2*$NUMBER_FIELDS)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_COORDS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << "]";
            throw std::out_of_range(msg.str());
          }
    
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto __Mp = __params.get_Mp();
        // note canonical epsilon doesn't depend on any Lagrangian parameters, only the field derivatives

        $RESOURCE_COORDINATES{__coords}

        $TEMP_POOL{"const auto $1 = $2;"}

        return $EPSILON;
      }


    template <typename number>
    number $MODEL<number>::eta(const parameters<number>& __params, const flattened_tensor<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);
        if(__coords.size() != 2*$NUMBER_FIELDS)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_COORDS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << "]";
            throw std::out_of_range(msg.str());
          }

        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto __Mp = __params.get_Mp();
        const auto& __param_vector = __params.get_vector();

        $RESOURCE_PARAMETERS{__param_vector}
        $RESOURCE_COORDINATES{__coords}

        $TEMP_POOL{"const auto $1 = $2;"}

        return $ETA;
      }


    template <typename number>
    number $MODEL<number>::V(const parameters<number>& __params, const flattened_tensor<number>& __coords) const
      {
        assert(__coords.size() == 2*$NUMBER_FIELDS);
        if(__coords.size() != 2*$NUMBER_FIELDS)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_WRONG_COORDS_A << __coords.size() << CPPTRANSPORT_WRONG_ICS_B << 2*$NUMBER_FIELDS << "]";
            throw std::out_of_range(msg.str());
          }
    
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto __Mp = __params.get_Mp();
        const auto& __param_vector = __params.get_vector();

        $RESOURCE_PARAMETERS{__param_vector}
        $RESOURCE_COORDINATES{__coords}

        $TEMP_POOL{"const auto $1 = $2;"}

        return $POTENTIAL;
      }


    $IF{!fast}
      template <typename number, typename StateType>
      void $MODEL_compute_dV(const number* __raw_params, const StateType& __x, number __Mp, number* __dV)
        {
          DEFINE_INDEX_TOOLS
          $RESOURCE_RELEASE

          $RESOURCE_PARAMETERS{__raw_params}
          $RESOURCE_COORDINATES{__x}

          $TEMP_POOL{"const auto $1 = $2;"}

          // force unroll to make explicit that we wish to populate array elements
          __dV[FIELDS_FLATTEN($a)] = $DV[a]|;
        }


      template <typename number, typename StateType>
      void $MODEL_compute_ddV(const number* __raw_params, const StateType& __x, number __Mp, number* __ddV)
        {
          DEFINE_INDEX_TOOLS
          $RESOURCE_RELEASE

          $RESOURCE_PARAMETERS{__raw_params}
          $RESOURCE_COORDINATES{__x}

          $TEMP_POOL{"const auto $1 = $2;"}

          // force unroll to make explicit that we wish to populate array elements
          __ddV[FIELDS_FLATTEN($a,$b)] = $DDV[ab]|;
        }


      template <typename number, typename StateType>
      void $MODEL_compute_dddV(const number* __raw_params, const StateType& __x, number __Mp, number* __dddV)
        {
          DEFINE_INDEX_TOOLS
          $RESOURCE_RELEASE

          $RESOURCE_PARAMETERS{__raw_params}
          $RESOURCE_COORDINATES{__x}

          $TEMP_POOL{"const auto $1 = $2;"}

          // force unroll to make explicit that we wish to populate array elements
          __dddV[FIELDS_FLATTEN($a,$b,$c)] = $DDDV[abc]|;
        }
    $ENDIF


    // Handle initial conditions


    template <typename number>
    void $MODEL<number>::validate_ics(const parameters<number>& __params, const flattened_tensor<number>& __input, flattened_tensor<number>& __output)
      {
        __output.clear();
        __output.reserve(2*$NUMBER_FIELDS);
        __output.insert(__output.end(), __input.begin(), __input.end());

        if(__input.size() == $NUMBER_FIELDS)  // initial conditions for momenta *were not* supplied -- need to compute them
          {
            DEFINE_INDEX_TOOLS
            $RESOURCE_RELEASE

            // supply the missing initial conditions using a slow-roll approximation
            const auto __Mp = __params.get_Mp();

            const auto& __pvector = __params.get_vector();
            $RESOURCE_PARAMETERS{__pvector}
            $RESOURCE_COORDINATES{__input}

            $TEMP_POOL{"const auto $1 = $2;"}

            // force unroll to make explicit that we wish to populate array elements
            __output.push_back($SR_VELOCITY[a]|);
          }
        else if(__input.size() == 2*$NUMBER_FIELDS)  // initial conditions for momenta *were* supplied
          {
            // need do nothing
          }
        else
          {
            std::ostringstream msg;

            msg << CPPTRANSPORT_WRONG_ICS_A << __input.size()
                << CPPTRANSPORT_WRONG_ICS_B << $NUMBER_FIELDS
                << CPPTRANSPORT_WRONG_ICS_C << 2*$NUMBER_FIELDS << "]";

            throw std::out_of_range(msg.str());
          }
      }


    // Handle parameters


    template <typename number>
    void $MODEL<number>::validate_params(const flattened_tensor<number>& input, flattened_tensor<number>& output)
      {
        output.clear();

        if(input.size() == $NUMBER_PARAMS)
          {
            output.assign(input.begin(), input.end());
          }
        else
          {
            std::ostringstream msg;

            msg << CPPTRANSPORT_WRONG_PARAMS_A << input.size() << CPPTRANSPORT_WRONG_PARAMS_B << $NUMBER_PARAMS << "]";

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
                                            const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields,
                                            double __k_norm)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}

        $TEMP_POOL{"const auto $1 = $2;"}

        const auto __Hsq = $HUBBLE_SQ;
        const auto __N = std::log(__k / (__a * std::sqrt(__Hsq)));

        number __tpf = 0.0;

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

        // return value, rescaled to give dimensionless correlation function
        return(__tpf * __k_norm*__k_norm*__k_norm);
      }


  // set up initial conditions for the imaginary part of the equal-time two-point function
  template <typename number>
  number $MODEL<number>::make_twopf_im_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                          const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields,
                                          double __k_norm)
    {
      DEFINE_INDEX_TOOLS
      $RESOURCE_RELEASE
      const auto& __pvector = __task->get_params().get_vector();
      __raw_params[$1] = __pvector[$1];

      const auto __Mp = __task->get_params().get_Mp();
      const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

      $RESOURCE_PARAMETERS{__raw_params}
      $RESOURCE_COORDINATES{__fields}

      $TEMP_POOL{"const auto $1 = $2;"}

      const auto __Hsq = $HUBBLE_SQ;
      const auto __N = std::log(__k / (__a * std::sqrt(__Hsq)));

      number __tpf = 0.0;

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

      // return value, rescaled to give dimensionless correlation function
      return(__tpf * __k_norm*__k_norm*__k_norm);
    }


    // set up initial conditions for the real part of the equal-time tensor two-point function
    template <typename number>
    number $MODEL<number>::make_twopf_tensor_ic(unsigned int __i, unsigned int __j, double __k, double __Ninit,
                                                const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields,
                                                double __k_norm)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

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

        // return value, rescaled to give dimensionless correlation function
        return(__tpf * __k_norm*__k_norm*__k_norm);
      }


    // set up initial conditions for the real part of the equal-time three-point function
    template <typename number>
    number $MODEL<number>::make_threepf_ic(unsigned int __i, unsigned int __j, unsigned int __k,
                                           double __k1, double __k2, double __k3, double __Ninit,
                                           const twopf_db_task<number>* __task, const flattened_tensor<number>& __fields,
                                           double __k_norm)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__Ninit - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __fields, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __fields, __Mp, __ddV);
          $MODEL_compute_dddV(__raw_params, __fields, __Mp, __dddV);
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $IF{!fast}
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
          $RESOURCE_DDDV{__dddV}
        $ENDIF

        const auto __Hsq = $HUBBLE_SQ;

        const auto __kt      = __k1 + __k2 + __k3;
        const auto __Ksq     = __k1*__k2 + __k1*__k3 + __k2*__k3;
        const auto __kprod3  = 4.0 * __k1*__k1*__k1 * __k2*__k2*__k2 * __k3*__k3*__k3;

        const auto __k2dotk3 = (__k1*__k1 - __k2*__k2 - __k3*__k3)/2.0;
        const auto __k1dotk3 = (__k2*__k2 - __k1*__k1 - __k3*__k3)/2.0;
        const auto __k1dotk2 = (__k3*__k3 - __k1*__k1 - __k2*__k2)/2.0;

        number __tpf = 0.0;

        $SET[abc]{B_k1k2k3, "__B_k1k2k3[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{B_k1k3k2, "__B_k1k3k2[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{C_k1k2k3, "__C_k1k2k3[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{C_k1k3k2, "__C_k1k3k2[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{Atilde_k1k2k3, "__A_k1k2k3[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{Atilde_k1k3k2, "__A_k1k3k2[FIELDS_FLATTEN($a,$b,$c)]"}

        $SET[abc]{B_k2k3k1, "__B_k2k3k1[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{B_k2k1k3, "__B_k2k1k3[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{C_k2k3k1, "__C_k2k3k1[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{C_k2k1k3, "__C_k2k1k3[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{Atilde_k2k3k1, "__A_k2k3k1[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{Atilde_k2k1k3, "__A_k2k1k3[FIELDS_FLATTEN($a,$b,$c)]"}

        $SET[abc]{B_k3k1k2, "__B_k3k1k2[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{B_k3k2k1, "__B_k3k2k1[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{C_k3k1k2, "__C_k3k1k2[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{C_k3k2k1, "__C_k3k2k1[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{Atilde_k3k1k2, "__A_k3k1k2[FIELDS_FLATTEN($a,$b,$c)]"}
        $SET[abc]{Atilde_k3k2k1, "__A_k3k2k1[FIELDS_FLATTEN($a,$b,$c)]"}

        $B_k1k2k3[abc] = $B_TENSOR[abc]{__k1, __k2, __k3, __a};
        $B_k1k3k2[abc] = $B_TENSOR[abc]{__k1, __k3, __k2, __a};
        $C_k1k2k3[abc] = $C_TENSOR[abc]{__k1, __k2, __k3, __a};
        $C_k1k3k2[abc] = $C_TENSOR[abc]{__k1, __k3, __k2, __a};
        $Atilde_k1k2k3[abc] = $ATILDE_TENSOR[abc]{__k1, __k2, __k3, __a};
        $Atilde_k1k3k2[abc] = $ATILDE_TENSOR[abc]{__k1, __k3, __k2, __a};

        $B_k2k3k1[abc] = $B_TENSOR[abc]{__k2, __k3, __k1, __a};
        $B_k2k1k3[abc] = $B_TENSOR[abc]{__k2, __k1, __k3, __a};
        $C_k2k3k1[abc] = $C_TENSOR[abc]{__k2, __k3, __k1, __a};
        $C_k2k1k3[abc] = $C_TENSOR[abc]{__k2, __k1, __k3, __a};
        $Atilde_k2k3k1[abc] = $ATILDE_TENSOR[abc]{__k2, __k3, __k1, __a};
        $Atilde_k2k1k3[abc] = $ATILDE_TENSOR[abc]{__k2, __k1, __k3, __a};

        $B_k3k1k2[abc] = $B_TENSOR[abc]{__k3, __k1, __k2, __a};
        $B_k3k2k1[abc] = $B_TENSOR[abc]{__k3, __k2, __k1, __a};
        $C_k3k1k2[abc] = $C_TENSOR[abc]{__k3, __k1, __k2, __a};
        $C_k3k2k1[abc] = $C_TENSOR[abc]{__k3, __k2, __k1, __a};
        $Atilde_k3k1k2[abc] = $ATILDE_TENSOR[abc]{__k3, __k1, __k2, __a};
        $Atilde_k3k2k1[abc] = $ATILDE_TENSOR[abc]{__k3, __k2, __k1, __a};

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

                // these components are dimension 1
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 1
                __tpf += - (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k2/2.0;
                __tpf += - (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k3/2.0;
                __tpf += - (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k3/2.0;

                // these components are dimension 1
                __tpf += __a*__a * __Hsq * (__A_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __A_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))]) / 2.0;
                __tpf += __a*__a * __Hsq * (__A_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))] + __A_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))]) / 2.0;
                __tpf += __a*__a * __Hsq * (__A_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))] + __A_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) / 2.0;

                // these components are dimension 1
                __tpf += __a*__a * __Hsq * (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))]) * ( (__k1+__k2)*__k3 / (__k1*__k2) - __Ksq / (__k1*__k2)) / 2.0;
                __tpf += __a*__a * __Hsq * (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))]) * ( (__k1+__k3)*__k2 / (__k1*__k3) - __Ksq / (__k1*__k3)) / 2.0;
                __tpf += __a*__a * __Hsq * (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * ( (__k2+__k3)*__k1 / (__k2*__k3) - __Ksq / (__k2*__k3)) / 2.0;

                __tpf *= __prefactor / __kprod3;

                break;
              }

            case 2:   // field-field-momentum correlation function
              {
                assert(total_fields == 2);

                auto __momentum_k = (IS_MOMENTUM(__i) ? __k1 : 0.0) + (IS_MOMENTUM(__j) ? __k2 : 0.0) + (IS_MOMENTUM(__k) ? __k3 : 0.0);

                // prefactor has dimension 3
                auto __prefactor = __k1*__k2*__k3 / (__kt * __a*__a*__a*__a);

                // component of prefactor that should not be symmetrized; has dimension 2
                auto __mom_factor_1 = __momentum_k*__momentum_k*(__kt-__momentum_k);

                // these components are dimension 1
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_1  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 1
                     __tpf_1 += - (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k2 / 2.0;
                     __tpf_1 += - (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k3 / 2.0;
                     __tpf_1 += - (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k3 / 2.0;

                // these components are dimension 1
                     __tpf_1 += __a*__a * __Hsq * (__A_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __A_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))]) / 2.0;
                     __tpf_1 += __a*__a * __Hsq * (__A_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))] + __A_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))]) / 2.0;
                     __tpf_1 += __a*__a * __Hsq * (__A_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))] + __A_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) / 2.0;

                __tpf = __prefactor * __mom_factor_1 * __tpf_1 / __kprod3;

                // this prefactor has dimension 3
                auto __mom_factor_2 = __momentum_k;

                // these components are dimension 3
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_2  = - (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * (__Ksq + __k1*__k2*__k3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_2 += - (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * (__Ksq + __k1*__k2*__k3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_2 += - (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * (__Ksq + __k1*__k2*__k3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 3
                     __tpf_2 += (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k1*__k2*__k2*(1.0+__k3/__kt) / 2.0;
                     __tpf_2 += (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k1*__k3*__k3*(1.0+__k2/__kt) / 2.0;
                     __tpf_2 += (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k2*__k3*__k3*(1.0+__k1/__kt) / 2.0;

                // these components are dimension 3
                     __tpf_2 += (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k1*__k1*__k2*__k3 / 2.0;
                     __tpf_2 += (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k2*__k2*__k1*__k3 / 2.0;
                     __tpf_2 += (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k3*__k3*__k1*__k2 / 2.0;

                // these components are dimension 3
                     __tpf_2 += - __a*__a * __Hsq * (__A_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __A_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))]) * (__Ksq - __k1*__k2*__k3/__kt) / 2.0;
                     __tpf_2 += - __a*__a * __Hsq * (__A_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))] + __A_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))]) * (__Ksq - __k1*__k2*__k3/__kt) / 2.0;
                     __tpf_2 += - __a*__a * __Hsq * (__A_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))] + __A_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * (__Ksq - __k1*__k2*__k3/__kt) / 2.0;

                __tpf += __prefactor * __mom_factor_2 * __tpf_2 / __kprod3;

                break;
              }

            case 1:   // field-momentum-momentum correlation function
              {
                assert(total_fields == 1);

                // this prefactor has dimension 3
                auto __prefactor = (__k1*__k2*__k3) / (__kt * __Hsq * __a*__a*__a*__a*__a*__a);

                // component of prefactor that should not be symmetrized
                auto __mom_factor1 = (IS_FIELD(__i) ? __k2*__k2*__k3*__k3*__k1 : 0.0) + (IS_FIELD(__j) ? __k1*__k1*__k3*__k3*__k2 : 0.0) + (IS_FIELD(__k) ? __k1*__k1*__k2*__k2*__k3 : 0.0);

                // these components have dimension 1
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                auto __tpf_1  = - (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * __k2dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += - (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * __k1dotk3 / (2.0*__Mp*__Mp);
                     __tpf_1 += - (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components have dimension 1
                     __tpf_1 += (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))]) * __k1*__k2 / 2.0;
                     __tpf_1 += (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))]) * __k1*__k3 / 2.0;
                     __tpf_1 += (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * __k2*__k3 / 2.0;

                // these components have dimension 1
                     __tpf_1 += - __a*__a * __Hsq * (__A_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __A_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))]) / 2.0;
                     __tpf_1 += - __a*__a * __Hsq * (__A_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))] + __A_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))]) / 2.0;
                     __tpf_1 += - __a*__a * __Hsq * (__A_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))] + __A_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) / 2.0;

                // these components have dimension 1
                     __tpf_1 += - __a*__a * __Hsq * (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))]) * (__k1+__k2)*__k3 / (__k1*__k2) / 2.0;
                     __tpf_1 += - __a*__a * __Hsq * (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))]) * (__k1+__k3)*__k2 / (__k1*__k3) / 2.0;
                     __tpf_1 += - __a*__a * __Hsq * (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * (__k2+__k3)*__k1 / (__k2*__k3) / 2.0;

                __tpf = __prefactor * __mom_factor1 * __tpf_1 / __kprod3;

                // prefactor has dimension 4
                auto __mom_factor2 = (IS_FIELD(__i) ? __k2*__k2*__k3*__k3 : 0.0) + (IS_FIELD(__j) ? __k1*__k1*__k3*__k3 : 0.0) + (IS_FIELD(__k) ? __k1*__k1*__k2*__k2 : 0.0);

                // these components have dimension 2
                auto __tpf_2  = __a*__a * __Hsq * (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))]) * __k3 / 2.0;
                     __tpf_2 += __a*__a * __Hsq * (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))]) * __k2 / 2.0;
                     __tpf_2 += __a*__a * __Hsq * (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * __k1 / 2.0;

                __tpf += __prefactor * __mom_factor2 * __tpf_2 / __kprod3;

                break;
              }

            case 0:   // momentum-momentum-momentum correlation function
              {
                assert(total_fields == 0);

                // prefactor is dimension 3
                auto __prefactor = (__k1*__k1) * (__k2*__k2) * (__k3*__k3) / (__kt * __Hsq * __a*__a*__a*__a*__a*__a);

                // these components are dimension 3
                // note factor of 2 compared to analytic calculation, from symmetrization over beta, gamma
                __tpf  = (SPECIES(__j) == SPECIES(__k) ? __fields[MOMENTUM(__i)] : 0.0) * (__Ksq + __k1*__k2*__k3/__kt) * __k2dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__k) ? __fields[MOMENTUM(__j)] : 0.0) * (__Ksq + __k1*__k2*__k3/__kt) * __k1dotk3 / (2.0*__Mp*__Mp);
                __tpf += (SPECIES(__i) == SPECIES(__j) ? __fields[MOMENTUM(__k)] : 0.0) * (__Ksq + __k1*__k2*__k3/__kt) * __k1dotk2 / (2.0*__Mp*__Mp);

                // these components are dimension 3
                __tpf += - (__C_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __C_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k1*__k1*__k2*__k2*(1.0+__k3/__kt) / 2.0;
                __tpf += - (__C_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __C_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k1*__k1*__k3*__k3*(1.0+__k2/__kt) / 2.0;
                __tpf += - (__C_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __C_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k2*__k2*__k3*__k3*(1.0+__k1/__kt) / 2.0;

                // these components are dimension 3
                __tpf += - (__B_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))] + __B_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))])*__k1*__k1*__k2*__k3/2.0;
                __tpf += - (__B_k1k3k2[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))] + __B_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))])*__k2*__k2*__k1*__k3/2.0;
                __tpf += - (__B_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __B_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))])*__k3*__k3*__k1*__k2/2.0;

                // these components are dimension 3
                __tpf += __a*__a * __Hsq * (__A_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__j), SPECIES(__k))] + __A_k1k2k3[FIELDS_FLATTEN(SPECIES(__i), SPECIES(__k), SPECIES(__j))]) * (__Ksq - __k1*__k2*__k3/__kt) / 2.0;
                __tpf += __a*__a * __Hsq * (__A_k2k1k3[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__i), SPECIES(__k))] + __A_k2k3k1[FIELDS_FLATTEN(SPECIES(__j), SPECIES(__k), SPECIES(__i))]) * (__Ksq - __k1*__k2*__k3/__kt) / 2.0;
                __tpf += __a*__a * __Hsq * (__A_k3k1k2[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__i), SPECIES(__j))] + __A_k3k2k1[FIELDS_FLATTEN(SPECIES(__k), SPECIES(__j), SPECIES(__i))]) * (__Ksq - __k1*__k2*__k3/__kt) / 2.0;

                __tpf *= __prefactor / __kprod3;

                break;
              }

            default:
              assert(false);
          }

        // return value, rescaled to give dimensionless correlation function
        return(__tpf * __k_norm*__k_norm*__k_norm*__k_norm*__k_norm*__k_norm);
      }


    // CALCULATE GAUGE TRANSFORMATIONS


    template <typename number>
    void $MODEL<number>::compute_gauge_xfm_1(const integration_task<number>* __task,
                                             const flattened_tensor<number>& __state,
                                             flattened_tensor<number>& __dN)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}

        $TEMP_POOL{"const auto $1 = $2;"}

        __dN[FLATTEN($A)] = $ZETA_XFM_1[A];
      }


    template <typename number>
    void $MODEL<number>::compute_gauge_xfm_2(const integration_task<number>* __task,
                                             const flattened_tensor<number>& __state,
                                             double __k, double __k1, double __k2, double __N,
                                             flattened_tensor<number>& __ddN)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__state}
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __state, __Mp, __dV);
          $RESOURCE_DV{__dV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        __ddN[FLATTEN($A,$B)] = $ZETA_XFM_2[AB]{__k, __k1, __k2, __a};
      }


    // CALCULATE TENSOR QUANTITIES


    template <typename number>
    void $MODEL<number>::u2(const integration_task<number>* __task,
                            const flattened_tensor<number>& __fields, double __k, double __N,
                            flattened_tensor<number>& __u2)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __fields, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __fields, __Mp, __ddV);
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        __u2[FLATTEN($A,$B)] = $U2_TENSOR[AB]{__k, __a};
      }


    template <typename number>
    void $MODEL<number>::u3(const integration_task<number>* __task,
                            const flattened_tensor<number>& __fields, double __k1, double __k2, double __k3, double __N,
                            flattened_tensor<number>& __u3)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __fields, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __fields, __Mp, __ddV);
          $MODEL_compute_dddV(__raw_params, __fields, __Mp, __dddV);
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
          $RESOURCE_DDDV{__dddV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        __u3[FLATTEN($A,$B,$C)] = $U3_TENSOR[ABC]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::A(const integration_task<number>* __task,
                           const flattened_tensor<number>& __fields, double __k1, double __k2, double __k3, double __N,
                           flattened_tensor<number>& __A)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __fields, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __fields, __Mp, __ddV);
          $MODEL_compute_dddV(__raw_params, __fields, __Mp, __dddV);
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
          $RESOURCE_DDDV{__dddV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        __A[FIELDS_FLATTEN($a,$b,$c)] = $A_TENSOR[abc]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::B(const integration_task<number>* __task,
                           const flattened_tensor<number>& __fields, double __k1, double __k2, double __k3, double __N,
                           flattened_tensor<number>& __B)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __fields, __Mp, __dV);
          $RESOURCE_DV{__dV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        __B[FIELDS_FLATTEN($a,$b,$c)] = $B_TENSOR[abc]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::C(const integration_task<number>* __task,
                           const flattened_tensor<number>& __fields, double __k1, double __k2, double __k3, double __N,
                           flattened_tensor<number>& __C)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();
        const auto __a = std::exp(__N - __task->get_N_horizon_crossing() + __task->get_astar_normalization());

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}

        $TEMP_POOL{"const auto $1 = $2;"}

        __C[FIELDS_FLATTEN($a,$b,$c)] = $C_TENSOR[abc]{__k1, __k2, __k3, __a};
      }


    template <typename number>
    void $MODEL<number>::M(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
                           flattened_tensor<number>& __M, massmatrix_type __type)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE
        const auto& __pvector = __task->get_params().get_vector();
        __raw_params[$1] = __pvector[$1];

        const auto __Mp = __task->get_params().get_Mp();

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__fields}
        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __fields, __Mp, __dV);
          $MODEL_compute_ddV(__raw_params, __fields, __Mp, __ddV);
          $RESOURCE_DV{__dV}
          $RESOURCE_DDV{__ddV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        switch(__type)
          {
            case massmatrix_type::include_mixing:
              {
                __M[FIELDS_FLATTEN($a,$b)] = $M_TENSOR[ab];
                break;
              }

            case massmatrix_type::hessian_approx:
              {
                __M[FIELDS_FLATTEN($a,$b)] = $DDV[ab];
                break;
              }

            default:
              assert(false);
          }
      }


    template <typename number>
    void $MODEL<number>::sorted_mass_spectrum(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
                                              bool __norm, flattened_tensor<number>& __M, flattened_tensor<number>& __E,
                                              massmatrix_type __type)
      {
        // get raw, unsorted mass spectrum in __E
        this->mass_spectrum(__task, __fields, __norm, __M, __E, __type);

        // sort mass spectrum into order
        std::sort(__E.begin(), __E.end());
      }


    template <typename number>
    void $MODEL<number>::mass_spectrum(const integration_task<number>* __task, const flattened_tensor<number>& __fields,
                                       bool __norm, flattened_tensor<number>& __M, flattened_tensor<number>& __E,
                                       massmatrix_type __type)
      {
        DEFINE_INDEX_TOOLS

        // write mass matrix (in canonical format) into __M
        this->M(__task, __fields, __M, __type);

        // copy elements of the mass matrix into an Eigen matrix
        __mass_matrix($a,$b) = __M[FIELDS_FLATTEN($a,$b)];

        // extract eigenvalues from this matrix
        // In general Eigen would give us complex results, which we'd like to avoid. That can be done by
        // forcing Eigen to use a self-adjoint matrix, which has guaranteed real eigenvalues.
        // Also, note that Eigen gives us back the results as an Eigen vector, not a flattened_tensor,
        // so we have to copy between the two
        auto __evalues = __mass_matrix.template selfadjointView<Eigen::Upper>().eigenvalues();

        // if normalized values requested, divide through by H^2
        if(__norm)
          {
            DEFINE_INDEX_TOOLS

            $RESOURCE_RELEASE
            const auto& __pvector = __task->get_params().get_vector();
            __raw_params[$1] = __pvector[$1];

            const auto __Mp = __task->get_params().get_Mp();

            $TEMP_POOL{"const auto $1 = $2;"}

            $RESOURCE_PARAMETERS{__raw_params};
            $RESOURCE_COORDINATES{__fields};

            const auto __Hsq = $HUBBLE_SQ;

            // copy eigenvalues into output matrix
            __E[$a] = __evalues($a) / __Hsq;
          }
        else
          {
            // copy eigenvalues into output matrix
            __E[FIELDS_FLATTEN($a)] = __evalues($a);
          }
      }


    template <typename number>
    void $MODEL<number>::backend_process_backg(const background_task<number>* tk, backg_history<number>& solution, bool silent)
      {
        DEFINE_INDEX_TOOLS

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
        
        using boost::numeric::odeint::integrate_times;
    
        auto stepper = $MAKE_BACKG_STEPPER{backg_state<number>, number, number, CPPTRANSPORT_ALGEBRA_NAME(backg_state<number>), CPPTRANSPORT_OPERATIONS_NAME(backg_state<number>)};
        integrate_times(stepper, system, x, time_db.value_begin(), time_db.value_end(),
                        static_cast<number>($BACKG_STEP_SIZE), obs);
        
        system.close_down_workspace();
      }


    namespace $MODEL_impl
      {

        template <typename number>
        class EpsilonUnityPredicate
          {
          public:
            EpsilonUnityPredicate(const parameters<number>& p)
              : __Mp(p.get_Mp())
              {
              }

            bool operator()(const std::pair< backg_state<number>, number >& __x)
              {
                DEFINE_INDEX_TOOLS
                $RESOURCE_RELEASE
                $TEMP_POOL{"const auto $1 = $2;"}

                $RESOURCE_COORDINATES{__x.first}

                const auto __eps = $EPSILON;

                return (__eps >= 1.0 || __eps < 0.0);
              }

          private:

            //! cache Planck mass
            const number __Mp;

          };

      }   // namespace $MODEL_impl


    template <typename number>
    double $MODEL<number>::compute_end_of_inflation(const integration_task<number>* tk, double search_time)
      {
        DEFINE_INDEX_TOOLS

        assert(tk != nullptr);

        // set up a functor to evolve this system
        $MODEL_background_functor<number> system(tk->get_params());
        system.set_up_workspace();

        auto ics = tk->get_ics_vector();

        backg_state<number> x($MODEL_pool::backg_state_size);
        x[FLATTEN($A)] = ics[$A];

		    // find point where epsilon = 1
        using boost::numeric::odeint::make_adaptive_time_range;
        
        auto stepper = $MAKE_BACKG_STEPPER{backg_state<number>, number, number, CPPTRANSPORT_ALGEBRA_NAME(backg_state<number>), CPPTRANSPORT_OPERATIONS_NAME(backg_state<number>)};
        auto range = make_adaptive_time_range(stepper, system, x, tk->get_N_initial(), tk->get_N_initial()+search_time, $BACKG_STEP_SIZE);
    
        double Nend = 0.0;
        try
          {
            // returns the first iterator in 'range' for which the predicate EpsilonUnityPredicate() is satisfied
            auto iter = boost::find_if(range, $MODEL_impl::EpsilonUnityPredicate<number>(tk->get_params()));
            if(iter == boost::end(range)) throw end_of_inflation_not_found{};
        
            // may require explicit narrowing cast to double if working type is wider
            Nend = static_cast<double>(iter->second);
          }
        catch(integration_produced_nan& xe)
          {
            throw end_of_inflation_not_found{};
          }
        catch(Hsq_is_negative& xe)
          {
            throw end_of_inflation_not_found{};
          }
    
        system.close_down_workspace();
    
        return Nend;
      };


    namespace $MODEL_impl
      {

        template <typename number>
        class aHAggregatorPredicate
          {
          public:
            aHAggregatorPredicate(const integration_task<number>* tk, model<number>* m, std::vector<double>& N,
                                  flattened_tensor<number>& log_aH, flattened_tensor<number>& log_a2H2M,
                                  boost::optional<double>& lk)
              : params(tk->get_params()),
                task(tk),
                mdl(m),
                N_vector(N),
                log_aH_vector(log_aH),
                log_a2H2M_vector(log_a2H2M),
                largest_k(lk),
                flat_M($NUMBER_FIELDS*$NUMBER_FIELDS),
                flat_E($NUMBER_FIELDS),
                N_horizon_crossing(tk->get_N_horizon_crossing()),
                astar_normalization(tk->get_astar_normalization()),
                __Mp(params.get_Mp()),
                __params(params.get_vector())
              {
              }

            number largest_evalue(const backg_state<number>& fields)
              {
                this->mdl->mass_spectrum(this->task, fields, false, this->flat_M, this->flat_E);

                // step through eigenvalue vector, extracting largest absolute value
                number largest_eigenvalue = -std::numeric_limits<number>().max();

                if(std::abs(this->flat_E[$a]) > largest_eigenvalue) { largest_eigenvalue = std::abs(this->flat_E[$a]); }

                return largest_eigenvalue;
              }

            bool operator()(const std::pair< backg_state<number>, number >& __x)
              {
                DEFINE_INDEX_TOOLS
                $RESOURCE_RELEASE
                $TEMP_POOL{"const auto $1 = $2;"}

                $RESOURCE_PARAMETERS{__params}
                $RESOURCE_COORDINATES{__x.first}

                const auto __Hsq = $HUBBLE_SQ;
                const auto __H   = std::sqrt(__Hsq);

                const auto __N   = __x.second - this->N_horizon_crossing + this->astar_normalization;

                this->N_vector.push_back(static_cast<double>(__x.second));
                this->log_aH_vector.push_back(__N + std::log(__H)); // = log(aH)
                this->log_a2H2M_vector.push_back(2.0*__N + 2.0*std::log(__H)
                                                 + std::log(this->largest_evalue(__x.first))); // = log(a^2 H^2 * largest eigenvalue)

                // if a largest k-mode was provided,
                // are we now at a point where we have comfortably covered the horizon crossing time for it?
                if(!this->largest_k) return false;

                if(std::log(*largest_k) - __N - std::log(__H) < -0.5) return true;
                return false;
              }

          private:

            //! pointer to model object
            model<number>* mdl;

            //! point to task object
            const integration_task<number>* task;

            //! parameters for the model in use
            const parameters<number>& params;

            //! cache parameters vectors
            const flattened_tensor<number>& __params;

            //! cache Planck mass
            const number __Mp;

            //! output vector for times N
            std::vector<double>& N_vector;

            //! output vector for values log(aH)
            flattened_tensor<number>& log_aH_vector;

            //! output vector for field values
            flattened_tensor<number>& log_a2H2M_vector;

            //! working space for calculation of mass matrix
            flattened_tensor<number> flat_M;

            //! working space for calculation of mass eigenvalues
            flattened_tensor<number> flat_E;

            //! largest k-mode for which we are trying to find a horizon-exit time
            const boost::optional<double>& largest_k;

            //! time of horizon crossing
            const double N_horizon_crossing;

            //! normalization of ln(a) at horizon crossing time
            const double astar_normalization;

          };

        template <typename number>
        class HAggregatorPredicate
        {
        public:
          HAggregatorPredicate(const integration_task<number>* tk, model<number>* m, std::vector<double>& N,
                               flattened_tensor<number>& log_H, boost::optional<double>& lk)
            : params(tk->get_params()),
              task(tk),
              mdl(m),
              N_vector(N),
              log_H_vector(log_H),
              largest_k(lk),
              __Mp(params.get_Mp()),
              __params(params.get_vector())
          {
          }

          bool operator()(const std::pair< backg_state<number>, number >& __x)
          {
            DEFINE_INDEX_TOOLS
            $RESOURCE_RELEASE
            $TEMP_POOL{"const auto $1 = $2;"}

            $RESOURCE_PARAMETERS{__params}
            $RESOURCE_COORDINATES{__x.first}

            const auto __Hsq = $HUBBLE_SQ;
            const auto __H   = std::sqrt(__Hsq);

            this->N_vector.push_back(static_cast<double>(__x.second));
            this->log_H_vector.push_back(std::log(__H)); // = log(H)

            // if a largest k-mode was provided,
            // are we now at a point where we have comfortably covered the horizon crossing time for it?
            if(!this->largest_k) return false;

            // if(std::log(*largest_k) - __N - std::log(__H) < -0.5) return true;
            // return false;
          }

        private:
          //! pointer to model object
          model<number>* mdl;

          //! point to task object
          const integration_task<number>* task;

          //! parameters for the model in use
          const parameters<number>& params;

          //! cache parameters vectors
          const flattened_tensor<number>& __params;

          //! cache Planck mass
          const number __Mp;

          //! output vector for times N
          std::vector<double>& N_vector;

          //! output vector for values log(aH)
          flattened_tensor<number>& log_H_vector;

          //! largest k-mode for which we are trying to find a horizon-exit time
          const boost::optional<double>& largest_k;
        };

      }   // namespace $MODEL_impl


		template <typename number>
		void $MODEL<number>::compute_aH(const integration_task<number>* tk, std::vector<double>& N,
		                                flattened_tensor<number>& log_aH, flattened_tensor<number>& log_a2H2M,
		                                boost::optional<double> largest_k)
			{
        DEFINE_INDEX_TOOLS

				N.clear();
				log_aH.clear();
        log_a2H2M.clear();

				// set up a functor to evolve the system
				$MODEL_background_functor<number> system(tk->get_params());
        system.set_up_workspace();

				auto ics = tk->get_ics_vector();

				backg_state<number> x($MODEL_pool::backg_state_size);
				x[FLATTEN($A)] = ics[$A];

        double N_range = 0.0;
        bool found_end = false;
        try
          {
            N_range   = tk->get_N_end_of_inflation();
            found_end = true;
          }
        catch(end_of_inflation_not_found& xe)
          {
            // try to fall back on a sensible default
            N_range = tk->get_N_initial() + CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH;
          }

        using boost::numeric::odeint::make_adaptive_time_range;
        
        auto stepper = $MAKE_BACKG_STEPPER{backg_state<number>, number, number, CPPTRANSPORT_ALGEBRA_NAME(backg_state<number>), CPPTRANSPORT_OPERATIONS_NAME(backg_state<number>)};
        auto range = make_adaptive_time_range(stepper, system, x, tk->get_N_initial(), N_range, $BACKG_STEP_SIZE);

        $MODEL_impl::aHAggregatorPredicate<number> aggregator(tk, this, N, log_aH, log_a2H2M, largest_k);

				// step through iterators, finding first point which is comfortably after time when largest_k has left
				// the horizon
				// aggregator writes N, log_aH and the field values into the output vectors at each iteration
				auto iter = boost::find_if(range, aggregator);

				// if we got to the end of the range, then we didn't cover all exit times up to largest_k
				// so something has gone wrong
				if(iter == boost::end(range) && largest_k)
					{
            throw failed_to_compute_horizon_exit(tk->get_N_initial(), N_range, found_end, log_aH.size(),
                                                 (N.size() > 0 ? N.back() : 0.0),
                                                 (log_aH.size() > 0 ? static_cast<double>(log_aH.back()) : 0.0),
                                                 largest_k.get());
					}

        system.close_down_workspace();
			}

    template <typename number>
    void $MODEL<number>::compute_H(const integration_task<number>* tk, std::vector<double>& N,
                                   flattened_tensor<number>& log_H, boost::optional<double> largest_k)
    {
      DEFINE_INDEX_TOOLS

      N.clear();
      log_H.clear();

      // set up a functor to evolve the system
      $MODEL_background_functor<number> system(tk->get_params());
      system.set_up_workspace();

      auto ics = tk->get_ics_vector();

      backg_state<number> x($MODEL_pool::backg_state_size);
      x[FLATTEN($A)] = ics[$A];

      double N_range = 0.0;
      bool found_end = false;
      try
      {
        N_range   = tk->get_N_end_of_inflation();
        found_end = true;
      }
      catch(end_of_inflation_not_found& xe)
      {
        // try to fall back on a sensible default
        N_range = tk->get_N_initial() + CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH;
      }

      using boost::numeric::odeint::make_adaptive_time_range;

      auto stepper = $MAKE_BACKG_STEPPER{backg_state<number>, number, number, CPPTRANSPORT_ALGEBRA_NAME(backg_state<number>), CPPTRANSPORT_OPERATIONS_NAME(backg_state<number>)};
      auto range = make_adaptive_time_range(stepper, system, x, tk->get_N_initial(), N_range, $BACKG_STEP_SIZE);

      $MODEL_impl::HAggregatorPredicate<number> aggregator(tk, this, N, log_H, largest_k);

      // step through iterators, finding first point which is comfortably after time when largest_k has left
      // the horizon
      // aggregator writes N, log_aH and the field values into the output vectors at each iteration
      auto iter = boost::find_if(range, aggregator);

      // if we got to the end of the range, then we didn't cover all exit times up to largest_k
      // so something has gone wrong
      if(iter == boost::end(range) && largest_k)
      {
        throw failed_to_compute_horizon_exit(tk->get_N_initial(), N_range, found_end, log_H.size(),
                                             (N.size() > 0 ? N.back() : 0.0),
                                             (log_H.size() > 0 ? static_cast<double>(log_H.back()) : 0.0),
                                             largest_k.get());
      }

      system.close_down_workspace();
    }

    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND INTEGRATION


    template <typename number>
    void $MODEL_background_functor<number>::operator()(const backg_state<number>& __x, backg_state<number>& __dxdt, number __t)
      {
        DEFINE_INDEX_TOOLS
        $RESOURCE_RELEASE

        $RESOURCE_PARAMETERS{__raw_params}
        $RESOURCE_COORDINATES{__x}

        $IF{!fast}
          $MODEL_compute_dV(__raw_params, __x, __Mp, __dV);
          $RESOURCE_DV{__dV}
        $ENDIF

        $TEMP_POOL{"const auto $1 = $2;"}

        const auto __Hsq = $HUBBLE_SQ;
        const auto __eps = $EPSILON;
        const auto __V = $POTENTIAL;

        // check whether 0 < epsilon < 3
        if(__eps < 0.0) throw eps_is_negative(static_cast<double>(__t), static_cast<double>(__Hsq), static_cast<double>(__eps), static_cast<double>(__V), __x, $MODEL_pool::state_names);
        if(__eps > 3.0) throw eps_too_large(static_cast<double>(__t), static_cast<double>(__Hsq), static_cast<double>(__eps), static_cast<double>(__V), __x, $MODEL_pool::state_names);

        // check whether potential is +ve definite
        if(__V < 0.0) throw V_is_negative(static_cast<double>(__t), static_cast<double>(__Hsq), static_cast<double>(__eps), static_cast<double>(__V), __x, $MODEL_pool::state_names);

        // check whether Hsq is positive
        if(__Hsq < 0.0) throw Hsq_is_negative(static_cast<double>(__t), static_cast<double>(__Hsq), static_cast<double>(__eps), static_cast<double>(__V), __x, $MODEL_pool::state_names);

        // check for nan being produced
        if(std::isnan(__x[$A])) throw integration_produced_nan(static_cast<double>(__t), static_cast<double>(__Hsq), static_cast<double>(__eps), static_cast<double>(__V), __x, $MODEL_pool::state_names);

        __dxdt[FLATTEN($A)] = $U1_TENSOR[A];
      }


    // IMPLEMENTATION - FUNCTOR FOR BACKGROUND OBSERVATION


    template <typename number>
    void $MODEL_background_observer<number>::operator()(const backg_state<number>& x, number t)
      {
        if(this->current_step != this->time_db.record_end() && this->current_step->is_stored())
          {
            this->history.push_back(x);
          }
        this->current_step++;
      }


  };   // namespace transport


#endif  // $GUARD
