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
#include "curvature_classes.h"
#include "cse.h"
#include "language_printer.h"

#include "concepts/flattened_tensor.h"

#include "timing_instrument.h"

#include "disable_warnings.h"
#include "ginac/ginac.h"


namespace nontrivial_metric
  {

    constexpr unsigned int use_dV = 1 << 0;
    constexpr unsigned int use_ddV = 1 << 1;
    constexpr unsigned int use_dddV = 1 << 2;
    constexpr unsigned int use_Gamma = 1 << 3;


    class PotentialResourceCache;
    class SubstitutionMapCache;
    class ConnexionCache;

    class CovariantdVCache;
    class CovariantddVCache;
    class CovariantdddVCache;


    //! implements resources for nontrivial_metric models, ie. trivial kinetic terms and just a potential
    class resources
      {

        // FRIENDSHIP

        friend class PotentialResourceCache;
        friend class SubstitutionMapCache;
        friend class ConnexionCache;

        friend class CovariantdVCache;
        friend class CovariantddVCache;
        friend class CovariantdddVCache;


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

        //! generate metric resource, including any necessary substitutions for parameters/coordinates
        //! returns raw expression, without applying any CSE
        std::unique_ptr<flattened_tensor> raw_G_resource(const language_printer& printer);

        //! generate inverse metric resource, including any necessary substitutions for parameters/coordinates
        //! returns raw expression, without applying any CSE
        std::unique_ptr<flattened_tensor> raw_Ginv_resource(const language_printer& printer);

        //! generate connexion resource, including any necessary substitutions for parameters/coordinates
        //! returns raw expression, without applying any CSE
        std::unique_ptr<flattened_tensor> raw_connexion_resource(const language_printer& printer);


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
        std::unique_ptr<flattened_tensor> generate_deriv_vector(variance var, const language_printer& printer) const;

        //! generate abstract field-space coordinate label resource
        GiNaC::ex generate_field_vector(const index_literal& idx, const language_printer& printer) const;

        //! generate abstract fields-space derivative label resource
        GiNaC::ex generate_deriv_vector(const index_literal& idx, const language_printer& printer) const;


        // TENSOR RESOURCES -- DERIVATIVES OF THE POTENTIAL

      public:

        //! generate concrete dV resource labels
        std::unique_ptr<flattened_tensor>
        dV_resource(variance v, const language_printer& printer);

        //! generate concrete ddV resource labels
        std::unique_ptr<flattened_tensor>
        ddV_resource(variance vi, variance vj, const language_printer& printer);

        //! generate concrete dddV resource labels
        std::unique_ptr<flattened_tensor>
        dddV_resource(variance vi, variance vj, variance vk, const language_printer& printer);


        //! generate abstract dV resource label
        GiNaC::ex
        dV_resource(const index_literal& a, const language_printer& printer);

        //! generate abstract ddV resource label
        GiNaC::ex
        ddV_resource(const index_literal& a, const index_literal& b, const language_printer& printer);

        //! generate abstract dddV resource label
        GiNaC::ex
        dddV_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                      const language_printer& printer);


        // TENSOR RESOURCES -- CURVATURE QUANTITIES

      public:

        //! generate concrete metric resource labels
        std::unique_ptr<flattened_tensor> metric_resource(const language_printer& printer);

        //! generate concrete inverse metric resource labels
        std::unique_ptr<flattened_tensor> metric_inverse_resource(const language_printer& printer);

        //! generate concrete connexion resource labels
        std::unique_ptr<flattened_tensor> connexion_resource(const language_printer& printer);


        //! generate abstract metric resource label
        GiNaC::ex metric_resource(const index_literal& a, const index_literal& b, const language_printer& printer);

        //! generate abstract inverse metric resource label
        GiNaC::ex metric_inverse_resource(const index_literal& a, const index_literal& b,
                                          const language_printer& printer);

        //! generate abstract connexion resource labels
        GiNaC::ex connexion_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                     const language_printer& printer);


        // BUILD ARGUMENT LISTS
        // used to interact with expression and lambda caches

      public:

        //! generate argument list
        cache_tags generate_cache_arguments(unsigned int flags, const language_printer& printer) const;
        
      protected:
    
        template <typename ResourceType>
        void push_resource_tag(cache_tags& args, const ResourceType& resource) const;


        // QUERY ROLL/UNROLL AVAILABILITY

      public:

        //! query whether dV can be rolled-up into loops
        bool can_roll_dV(const std::array< variance, RESOURCE_INDICES::DV_INDICES >& vars) const;

        //! query whether ddV can be rolled-up into loops
        bool can_roll_ddV(const std::array< variance, RESOURCE_INDICES::DDV_INDICES >& vars) const;

        //! query whether dddV can be rolled-up into loops
        bool can_roll_dddV(const std::array< variance, RESOURCE_INDICES::DDDV_INDICES >& vars) const;
        
        //! query whether connexion can be rolled-up into loops
        bool can_roll_connexion() const;
        
        //! query whether metric can be rolled-up into loops
        bool can_roll_metric() const;
        
        //! query whether inverse metric can be rolled-up into loops
        bool can_roll_metric_inverse() const;
        
        //! query whether RiemannA2 can be rolled-up into loops
        bool can_roll_Riemann_A2(const std::array< variance, RESOURCE_INDICES::RIEMANN_A2_INDICES >& vars) const;
    
        //! query whether RiemannA3 can be rolled-up into loops
        bool can_roll_Riemann_A3(const std::array< variance, RESOURCE_INDICES::RIEMANN_A3_INDICES >& vars) const;
    
        //! query whether RiemannB3 can be rolled-up into loops
        bool can_roll_Riemann_B3(const std::array< variance, RESOURCE_INDICES::RIEMANN_B3_INDICES >& vars) const;
        
        
        //! determine whether metric resources are available to perform index raising and lowering as needed
        template <unsigned long Indices>
        bool get_roll_metric_requirements(const std::array< variance, Indices >& avail, const std::array< variance, Indices >& reqd) const;


        // INTERNAL API

      private:

        //! generate argument list for param/field combinations;
        //! used internally and as the first step in generating an external argument list
        cache_tags generate_cache_arguments(const language_printer& printer) const;

        //! generate substitution map for parameter and coordinate labels
        GiNaC::exmap make_substitution_map(const language_printer& printer) const;



        // INTERNAL API -- TENSOR RESOURCES, DERIVATIVES OF THE POTENTIAL

      private:

        //! generate concrete dV resource using labels
        void dV_resource_label(flattened_tensor& list, const std::array<variance, RESOURCE_INDICES::DV_INDICES>& avail,
                               const std::array<variance, RESOURCE_INDICES::DV_INDICES> reqd,
                               const contexted_value<std::string>& resource,
                               const contexted_value<std::string>& flatten,
                               const language_printer& printer);

        //! generate concrete dV resource using literal expressions
        void dV_resource_expr(flattened_tensor& list, const std::array<variance, RESOURCE_INDICES::DV_INDICES> reqd,
                              const language_printer& printer);


        //! generate concrete ddV resource using labels
        void ddV_resource_label(flattened_tensor& list, const std::array<variance, RESOURCE_INDICES::DDV_INDICES>& avail,
                                const std::array<variance, RESOURCE_INDICES::DDV_INDICES> reqd,
                                const contexted_value<std::string>& resource, const contexted_value<std::string>& flatten,
                                const language_printer& printer);

        //! generate concrete ddV resource using literal expressions
        void ddV_resource_expr(flattened_tensor& list, const std::array<variance, RESOURCE_INDICES::DDV_INDICES> reqd,
                               const language_printer& printer);


        //! generate concrete dddV resource using labels
        void dddV_resource_label(const language_printer& printer, flattened_tensor& list,
                                 const contexted_value<std::string>& resource,
                                 const contexted_value<std::string>& flatten);

        //! generate concrete dddV resource using literal expressions
        void dddV_resource_expr(const language_printer& printer, flattened_tensor& list);


        // INTERNAL API -- TENSOR RESOURCES, CURVATURE QUANTITIES

      private:

        //! generate concrete metric resource using labels
        void metric_resource_label(const language_printer& printer, flattened_tensor& list,
                                   const contexted_value<std::string>& resource,
                                   const contexted_value<std::string>& flatten);

        //! generate concrete inverse metric resource using labels
        void metric_inverse_resource_label(const language_printer& printer, flattened_tensor& list,
                                           const contexted_value<std::string>& resource,
                                           const contexted_value<std::string>& flatten);

        //! generate concrete connexion resource using labels
        void connexion_resource_label(const language_printer& printer, flattened_tensor& list,
                                      const contexted_value<std::string>& resource,
                                      const contexted_value<std::string>& flatten);


        //! dress an abstract resource label with given variance assignment 'avail' to give the required variance
        //! assignment 'reqd'.
        //! Returns a GiNaC expression representing the abstract resource with correct variance assignment
        template <size_t Indices, typename IndexType>
        GiNaC::ex
        position_indices(const std::array<variance, Indices> avail,
                         const std::array<std::reference_wrapper<const IndexType>, Indices> reqd,
                         const contexted_value<std::string>& label, const std::string& flatten,
                         const language_printer& printer) const;
    
        void
        warn_resource_index_reposition(const contexted_value<std::string>& label, unsigned int size,
                                       unsigned int repositioned) const;

        //! dress a concrete resource with given variance asignment 'avail' to produce the required variance
        //! assignment 'reqd'
        template <size_t Indices>
        GiNaC::ex
        position_indices(const std::array<variance, Indices> avail, const std::array<variance, Indices> reqd,
                         const std::array<field_index, Indices> indices, const flattened_tensor& tensor,
                         const language_printer& printer);

        
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
    
        //! potential
        GiNaC::ex V;

        //! field metric
        std::unique_ptr<GiNaC::matrix> G;

        //! inverse field metric
        std::unique_ptr<GiNaC::matrix> Ginv;

        //! Christoffel symbols
        std::unique_ptr<Christoffel> Crstfl;

        //! Riemann tensor components
        std::unique_ptr<Riemann_T> Rie_T;

        // AGENTS

        //! index flattener
        index_flatten fl;


        // TIMERS

        //! compute timer
        boost::timer::cpu_timer& compute_timer;

      };

  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_H
