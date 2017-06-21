//
// Created by David Seery on 19/12/2015.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
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
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_H
#define CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_H


#include <memory>
#include <vector>

#include "resource_manager.h"
#include "translator_data.h"
#include "expression_cache.h"
#include "indices.h"
#include "index_flatten.h"

#include "shared_resources.h"
#include "cse.h"
#include "language_printer.h"

#include "concepts/flattened_tensor.h"

#include "timing_instrument.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


namespace nontrivial_metric
  {

    constexpr unsigned int use_dV_argument = 1;
    constexpr unsigned int use_ddV_argument = 2;
    constexpr unsigned int use_dddV_argument = 4;

    //! implements resources for nontrivial_metric models, ie. trivial kinetic terms and just a potential
    class resources
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        resources(translator_data& p, resource_manager& m, expression_cache& c,
                  shared_resources& s, boost::timer::cpu_timer& t);

        //! destructor is default
        ~resources() = default;


        // RAW RESOURCES -- NO COMMON SUBEXPRESSION ELIMINATION

      public:

        //! generate V resource, including any necessary substitutions for parameters/coordinates
        //! returns raw expression, without applying any CSE
        GiNaC::ex raw_V_resource(const language_printer& printer);

        //! generate epsilon resource
        //! returns raw expression, without applying any CSE
        GiNaC::ex raw_eps_resource(const language_printer& printer);

        //! generate Hubble-squared resource
        //! returns raw expression, without applying any CSE
        GiNaC::ex raw_Hsq_resource(const language_printer& printer);


        // SCALAR RESOURCES

      public:


        //! generate V resource, including any necessary substitutions for parameters/coordinates
        GiNaC::ex V_resource(cse& cse_worker, const language_printer& printer);

        //! generate epsilon resource
        GiNaC::ex eps_resource(cse& cse_worker, const language_printer& printer);

        //! generate Hubble-squared resource
        GiNaC::ex Hsq_resource(cse& cse_worker, const language_printer& printer);


        // TENSOR RESOURCES -- COORDINATES

      public:

        //! generate concrete field-space coordinate label resource
        std::unique_ptr<flattened_tensor> generate_field_vector(const language_printer& printer) const;

        //! generate concrete field-space deriative label resource
        std::unique_ptr<flattened_tensor> generate_deriv_vector(const language_printer& printer) const;

        //! generate abstract field-space coordinate label resource
        GiNaC::ex generate_field_vector(const abstract_index& idx, const language_printer& printer) const;

        //! generate abstract fields-space derivative label resource
        GiNaC::ex generate_deriv_vector(const abstract_index& idx, const language_printer& printer) const;


        // TENSOR RESOURCES -- DERIVATIVES OF THE POTENTIAL

      public:

        //! generate concrete dV resource labels
        std::unique_ptr<flattened_tensor> dV_resource(const language_printer& printer);

        //! generate concrete ddV resource labels
        std::unique_ptr<flattened_tensor> ddV_resource(const language_printer& printer);

        //! generate concrete dddV resource labels
        std::unique_ptr<flattened_tensor> dddV_resource(const language_printer& printer);


        //! generate abstract dV resource label
        GiNaC::ex dV_resource(const index_literal& a, const language_printer& printer);

        //! generate abstract ddV resource label
        GiNaC::ex ddV_resource(const index_literal& a, const index_literal& b, const language_printer& printer);

        //! generate abstract dddV resource label
        GiNaC::ex dddV_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                const language_printer& printer);



        // BUILD ARGUMENT LISTS
        // used to interact with expression and lambda caches

      public:

        //! generate argument list
        std::unique_ptr<cache_tags> generate_cache_arguments(unsigned int flags, const language_printer& printer) const;


        // QUERY ROLL/UNROLL AVAILABILITY

      public:

        //! query whether dV can be rolled-up into loops
        bool can_roll_dV() const;

        //! query whether ddV can be rolled-up into loops
        bool can_roll_ddV() const;

        //! query whether dddV can be rolled-up into loops
        bool can_roll_dddV() const;
        
        //! query whether connexion can be rolled-up into loops
        bool can_roll_connexion() const;
        
        //! query whether metric can be rolled-up into loops
        bool can_roll_metric() const;
        
        //! query whether inverse metric can be rolled-up into loops
        bool can_roll_metric_inverse() const;
        
        //! query whether RiemannA2 can be rolled-up into loops
        bool can_roll_Riemann_A2() const;
    
        //! query whether RiemannA3 can be rolled-up into loops
        bool can_roll_Riemann_A3() const;
    
        //! query whether RiemannB3 can be rolled-up into loops
        bool can_roll_Riemann_B3() const;


        // INTERNAL API

      private:

        //! generate argument list for param/field combinations;
        //! used internally and as the first step in generating an external argument list
        std::unique_ptr<cache_tags> generate_cache_arguments(const language_printer& printer) const;


        // INTERNAL DATA

      private:

        // CACHES

        //! resource manager object
        resource_manager& mgr;

        //! expression cache
        expression_cache& cache;

        //! symbol factory
        symbol_factory& sym_factory;

        //! shared resource handler
        shared_resources& share;

        //! data payload provided by parent translation unit
        translator_data& payload;


        // SYMBOL LISTS

        //! list of symbols representing fields in the model
        const symbol_list field_list;

        //! list of symbols representing derivatives in the model
        const symbol_list deriv_list;

        //! list of symbols representing parameters in the model
        const symbol_list param_list;


        // LOCAL COPIES OF MODEL DATA

        //! number of parameters
        param_index num_params;

        //! number of field-space indices
        field_index num_fields;

        //! potential
        GiNaC::ex V;


        // AGENTS

        //! index flattener
        index_flatten fl;


        // TIMERS

        //! compute timer
        boost::timer::cpu_timer& compute_timer;

      };

  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_H
