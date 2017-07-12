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
    constexpr unsigned int use_Riemann_A2 = 1 << 4;
    constexpr unsigned int use_Riemann_A3 = 1 << 5;
    constexpr unsigned int use_Riemann_B3 = 1 << 6;


    class PotentialResourceCache;
    class SubstitutionMapCache;
    class ConnexionCache;
    class DerivativeSymbolsCache;

    class CovariantdVCache;
    class CovariantddVCache;
    class CovariantdddVCache;

    class CovariantRiemannA2Cache;
    class CovariantRiemannA3Cache;
    class CovariantRiemannB3Cache;


    //! implements resources for nontrivial_metric models, ie. arbitrary kinetic matrix and any potential
    class resources
      {

        // FRIENDSHIP

        friend class PotentialResourceCache;
        friend class SubstitutionMapCache;
        friend class ConnexionCache;
        friend class DerivativeSymbolsCache;

        friend class CovariantdVCache;
        friend class CovariantddVCache;
        friend class CovariantdddVCache;

        friend class CovariantRiemannA2Cache;
        friend class CovariantRiemannA3Cache;
        friend class CovariantRiemannB3Cache;


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
        GiNaC::ex raw_V_resource(const language_printer& printer) const;

        //! generate epsilon resource
        //! returns raw expression, without applying any CSE
        GiNaC::ex raw_eps_resource(const language_printer& printer) const;

        //! generate Hubble-squared resource
        //! returns raw expression, without applying any CSE
        GiNaC::ex raw_Hsq_resource(const language_printer& printer) const;

      protected:

        //! generate metric resource, including any necessary substitutions for parameters/coordinates
        //! returns raw expression, without applying any CSE
        std::unique_ptr<flattened_tensor> raw_G_resource(const language_printer& printer) const;

        //! generate inverse metric resource, including any necessary substitutions for parameters/coordinates
        //! returns raw expression, without applying any CSE
        std::unique_ptr<flattened_tensor> raw_Ginv_resource(const language_printer& printer) const;
    
        
        // SCALAR RESOURCES

      public:


        //! generate V resource, including any necessary substitutions for parameters/coordinates
        GiNaC::ex V_resource(cse& cse_worker, const language_printer& printer) const;

        //! generate epsilon resource
        GiNaC::ex eps_resource(cse& cse_worker, const language_printer& printer) const;

        //! generate Hubble-squared resource
        GiNaC::ex Hsq_resource(cse& cse_worker, const language_printer& printer) const;


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

        //! generate concrete dV resource
        std::unique_ptr<flattened_tensor>
        dV_resource(variance v, const language_printer& printer) const;

        //! generate concrete ddV resource
        std::unique_ptr<flattened_tensor>
        ddV_resource(variance vi, variance vj, const language_printer& printer) const;

        //! generate concrete dddV resource
        std::unique_ptr<flattened_tensor>
        dddV_resource(variance vi, variance vj, variance vk, const language_printer& printer) const;


        //! generate abstract dV resource label
        GiNaC::ex
        dV_resource(const index_literal& a, const language_printer& printer) const;

        //! generate abstract ddV resource label
        GiNaC::ex
        ddV_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const;

        //! generate abstract dddV resource label
        GiNaC::ex
        dddV_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                      const language_printer& printer) const;


        // TENSOR RESOURCES -- METRIC AND CONNEXION

      public:

        //! generate concrete metric resource
        std::unique_ptr<flattened_tensor> metric_resource(const language_printer& printer) const;

        //! generate concrete inverse metric resource
        std::unique_ptr<flattened_tensor> metric_inverse_resource(const language_printer& printer) const;

        //! generate concrete connexion resource
        std::unique_ptr<flattened_tensor> connexion_resource(const language_printer& printer) const;


        //! generate abstract metric resource label
        GiNaC::ex metric_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const;

        //! generate abstract inverse metric resource label
        GiNaC::ex metric_inverse_resource(const index_literal& a, const index_literal& b,
                                          const language_printer& printer) const;

        //! generate abstract connexion resource labels
        GiNaC::ex connexion_resource(const index_literal& a, const index_literal& b, const index_literal& c,
                                     const language_printer& printer) const;


        // TENSOR RESOURCES -- CURVATURE QUANTITIES

      public:

        //! generate concrete Riemann A2 resource
        std::unique_ptr<flattened_tensor> Riemann_A2_resource(variance vi, variance vj, const language_printer& printer) const;

        //! generate concrete Riemann A3 resource
        std::unique_ptr<flattened_tensor> Riemann_A3_resource(variance vi, variance vj, variance vk, const language_printer& printer) const;

        //! generate concrete Riemann B3 resource
        std::unique_ptr<flattened_tensor> Riemann_B3_resource(variance vi, variance vj, variance vk, const language_printer& printer) const;


        //! generate abstract Riemann A2 resource label
        GiNaC::ex Riemann_A2_resource(const index_literal& a, const index_literal& b, const language_printer& printer) const;

        //! generate abstract Riemann A3 resource label
        GiNaC::ex Riemann_A3_resource(const index_literal& a, const index_literal& b, const index_literal& c, const language_printer& printer) const;

        //! generate abstract Riemann B3 resource label
        GiNaC::ex Riemann_B3_resource(const index_literal& a, const index_literal& b, const index_literal& c, const language_printer& printer) const;


        // BUILD CACHE TAG LISTS
        // used to interact with expression and lambda caches

      public:
    
        //! generate argument list -- 1 index
        template <typename IndexType>
        cache_tags generate_cache_arguments(unsigned int flags, std::array<IndexType, 1> idxs, const language_printer& printer) const;

        //! generate argument list -- 2 index
        template <typename IndexType>
        cache_tags generate_cache_arguments(unsigned int flags, std::array<IndexType, 2> idxs, const language_printer& printer) const;
    
        //! generate argument list -- 3 index
        template <typename IndexType>
        cache_tags generate_cache_arguments(unsigned int flags, std::array<IndexType, 3> idxs, const language_printer& printer) const;
        
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


        // INTERNAL API -- UTILITY FUNCTIONS

      private:

        //! generate argument list for param/field combinations;
        //! used internally and as the first step in generating an external argument list
        cache_tags generate_cache_arguments(const language_printer& printer) const;

        //! generate substitution map for parameter and coordinate labels
        GiNaC::exmap make_substitution_map(const language_printer& printer) const;


        // INTERNAL API -- RAISE AND LOWER INDICES

      private:

        //! dress an abstract resource label with given variance assignment 'avail' to give the required variance
        //! assignment 'reqd'.
        //! Returns a GiNaC expression representing the abstract resource with correct variance assignment
        //! If supplied, the array 'offsets' gives a list of offsets that are used to displace the index values
        //! subscripted into the label (but not occurrences of the metric)
        template <size_t Indices, typename IndexType>
        GiNaC::ex
        position_indices(const std::array<variance, Indices> avail,
                         const std::array<std::reference_wrapper<const IndexType>, Indices> reqd,
                         const contexted_value<std::string>& label, const std::string& flatten,
                         const language_printer& printer,
                         const std::array<int, Indices> offsets =  std::array<int, Indices>{}) const;
    
        void
        warn_resource_index_reposition(const contexted_value<std::string>& label, unsigned int size,
                                       unsigned int repositioned) const;

        //! dress a concrete resource with given variance asignment 'avail' to produce the required variance
        //! assignment 'reqd'
        template <size_t Indices>
        GiNaC::ex
        position_indices(const std::array<variance, Indices> avail, const std::array<variance, Indices> reqd,
                         const std::array<field_index, Indices> indices, const flattened_tensor& tensor,
                         const language_printer& printer) const;


        // INTERNAL API -- GENERATE FLATTENED VECTOR OF RESOURCE LABELS

      private:

        //! generate 1-index labels
        void tensor_resource_label(flattened_tensor& list, const std::array<variance, 1>& avail,
                                   const std::array<variance, 1> reqd, const contexted_value<std::string>& resource,
                                   const contexted_value<std::string>& flatten, const language_printer& printer) const;

        //! generate 2-index labels
        void tensor_resource_label(flattened_tensor& list, const std::array<variance, 2>& avail,
                                   const std::array<variance, 2> reqd, const contexted_value<std::string>& resource,
                                   const contexted_value<std::string>& flatten, const language_printer& printer) const;

        //! generate 3-index labels
        void tensor_resource_label(flattened_tensor& list, const std::array<variance, 3>& avail,
                                   const std::array<variance, 3> reqd, const contexted_value<std::string>& resource,
                                   const contexted_value<std::string>& flatten, const language_printer& printer) const;

        //! generate 2-index labels, no index repositioning
        void tensor_resource_label(flattened_tensor& list, const std::array<variance, 2> reqd,
                                   const contexted_value<std::string>& resource,
                                   const contexted_value<std::string>& flatten, const language_printer& printer) const;

        //! generate 3-index labels, no index repositioning
        void tensor_resource_label(flattened_tensor& list, const std::array<variance, 3> reqd,
                                   const contexted_value<std::string>& resource,
                                   const contexted_value<std::string>& flatten, const language_printer& printer) const;


        // INTERNAL API -- GENERATE FLATTENED VECTOR OF GINAC EXPRESSIONS FROM COMPUTE CACHE

      private:

        //! generate 1-index
        template <typename CacheObject>
        void tensor_resource_expr(flattened_tensor& list, const std::array<variance, 1> reqd,
                                  expression_item_types type, const language_printer& printer, CacheObject& cache) const;

        //! generate 2-index
        template <typename CacheObject>
        void tensor_resource_expr(flattened_tensor& list, const std::array<variance, 2> reqd,
                                  expression_item_types type, const language_printer& printer, CacheObject& cache) const;

        //! generate 3-index
        template <typename CacheObject>
        void tensor_resource_expr(flattened_tensor& list, const std::array<variance, 3> reqd,
                                  expression_item_types type, const language_printer& printer, CacheObject& cache) const;

        
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
        
        //! covariant derivative of Riemann tensor
        std::unique_ptr<DRiemann_T> DRie_T;

        // AGENTS

        //! index flattener
        index_flatten fl;


        // TIMERS

        //! compute timer
        boost::timer::cpu_timer& compute_timer;

      };
    
    
    // template selected when ResourceType is returned from a simple resource
    template <>
    inline void resources::push_resource_tag(cache_tags& args, const boost::optional< contexted_value<std::string> >& resource) const
      {
        if(resource)   // no need to push arguments if no resource available
          {
            GiNaC::symbol sym = sym_factory.get_symbol(resource.get());
            args += sym;
          }
      }
    
    
    // template selected when ResourceType is returned from an indexed resource
    template <typename ResourceType>
    void resources::push_resource_tag(cache_tags& args, const ResourceType& resource) const
      {
        if(resource)   // no need to push arguments if no resource available
          {
            GiNaC::symbol sym = sym_factory.get_symbol(resource.get().second);
            args += sym;
          }
      }
    
    
    template <typename IndexType>
    cache_tags
    resources::generate_cache_arguments(unsigned int flags, std::array<IndexType, 1> idxs, const language_printer& printer) const
      {
        // first, generate arguments from param/coordinates if they exist
        auto args = this->generate_cache_arguments(printer);
        const auto& flatten = this->mgr.field_flatten();
        
        // push G and G inverse tags, since it's difficult to keep track of where they are used
        // this might lead to some inefficiencies, since we might push them when they're not needed
        // but we'd rather do a small amount of extra calculation than risk a difficult-to-diagnose
        // cache aliasing issue
        if(flatten)
          {
            this->push_resource_tag(args, this->mgr.metric());
            this->push_resource_tag(args, this->mgr.metric_inverse());
            
          }
        
        std::array<variance, 1> v{ idxs[0].get_variance() };

        if(flatten && ((flags & use_dV) != 0)) this->push_resource_tag(args, this->mgr.dV(v, false));
        
        return args;
      }
    
    
    template <typename IndexType>
    cache_tags
    resources::generate_cache_arguments(unsigned int flags, std::array<IndexType, 2> idxs, const language_printer& printer) const
      {
        // first, generate arguments from param/coordinates if they exist
        auto args = this->generate_cache_arguments(printer);
        const auto& flatten = this->mgr.field_flatten();
        
        // push G and G inverse tags, since it's difficult to keep track of where they are used
        // this might lead to some inefficiencies, since we might push them when they're not needed
        // but we'd rather do a small amount of extra calculation than risk a difficult-to-diagnose
        // cache aliasing issue
        if(flatten)
          {
            this->push_resource_tag(args, this->mgr.metric());
            this->push_resource_tag(args, this->mgr.metric_inverse());
            
          }
    
        std::array<variance, 1> v1{ idxs[0].get_variance() };
        std::array<variance, 1> v2{ idxs[1].get_variance() };
        std::array<variance, 2> v12{ idxs[0].get_variance(), idxs[1].get_variance() };
    
        // assume we don't need to be too careful with index perumutations since we'll get the same
        // match from eg. 12 and 21
    
        if(flatten && ((flags & use_dV) != 0)) this->push_resource_tag(args, this->mgr.dV(v1, false));
        if(flatten && ((flags & use_dV) != 0)) this->push_resource_tag(args, this->mgr.dV(v2, false));
        if(flatten && ((flags & use_ddV) != 0)) this->push_resource_tag(args, this->mgr.ddV(v12, false));
        
        // Riemann tensor resources are more ambiguous, since whether they are used or not depends on the index assignments that
        // are available, and the availability of the metric tensor and its inverse.
        // As above, assume we don't need to be careful with index permutations since v12 and v21 would generate the
        // same symbol
        if(flatten && ((flags & use_Riemann_A2) != 0)) this->push_resource_tag(args, this->mgr.Riemann_A2(v12, false));
        
        return args;
      }
    
    
    template <typename IndexType>
    cache_tags
    resources::generate_cache_arguments(unsigned int flags, std::array<IndexType, 3> idxs, const language_printer& printer) const
      {
        // first, generate arguments from param/coordinates if they exist
        auto args = this->generate_cache_arguments(printer);
        const auto& flatten = this->mgr.field_flatten();
        
        // push G and G inverse tags, since it's difficult to keep track of where they are used
        // this might lead to some inefficiencies, since we might push them when they're not needed
        // but we'd rather do a small amount of extra calculation than risk a difficult-to-diagnose
        // cache aliasing issue
        if(flatten)
          {
            this->push_resource_tag(args, this->mgr.metric());
            this->push_resource_tag(args, this->mgr.metric_inverse());
            
          }
    
        std::array<variance, 1> v1{ idxs[0].get_variance() };
        std::array<variance, 1> v2{ idxs[1].get_variance() };
        std::array<variance, 1> v3{ idxs[2].get_variance() };
        std::array<variance, 2> v12{ idxs[0].get_variance(), idxs[1].get_variance() };
        std::array<variance, 2> v13{ idxs[0].get_variance(), idxs[1].get_variance() };
        std::array<variance, 2> v23{ idxs[0].get_variance(), idxs[1].get_variance() };
        std::array<variance, 3> v123{ idxs[0].get_variance(), idxs[1].get_variance(), idxs[2].get_variance() };
    
        // assume we don't need to be too careful with index permutations since we'll get the same
        // match from eg. 123 and 132 or 321
        
        if(flatten && ((flags & use_dV) != 0)) this->push_resource_tag(args, this->mgr.dV(v1, false));
        if(flatten && ((flags & use_dV) != 0)) this->push_resource_tag(args, this->mgr.dV(v2, false));
        if(flatten && ((flags & use_dV) != 0)) this->push_resource_tag(args, this->mgr.dV(v3, false));
        if(flatten && ((flags & use_ddV) != 0)) this->push_resource_tag(args, this->mgr.ddV(v12, false));
        if(flatten && ((flags & use_ddV) != 0)) this->push_resource_tag(args, this->mgr.ddV(v13, false));
        if(flatten && ((flags & use_ddV) != 0)) this->push_resource_tag(args, this->mgr.ddV(v23, false));
        if(flatten && ((flags & use_dddV) != 0)) this->push_resource_tag(args, this->mgr.dddV(v123, false));
        if(flatten && ((flags & use_Gamma) != 0)) this->push_resource_tag(args, this->mgr.connexion());
        
        // Riemann tensor resources are more ambiguous, since whether they are used or not depends on the index assignments that
        // are available, and the availability of the metric tensor and its inverse.
        // As above, assume we don't need to be careful with index permutations since v123 and v132 etc. would generate the
        // same symbol
        if(flatten && ((flags & use_Riemann_A3) != 0)) this->push_resource_tag(args, this->mgr.Riemann_A3(v123, false));
        if(flatten && ((flags & use_Riemann_B3) != 0)) this->push_resource_tag(args, this->mgr.Riemann_B3(v123, false));
        
        return args;
      }

  }   // namespace nontrivial_metric


#endif //CPPTRANSPORT_NONTRIVIAL_METRIC_RESOURCES_H
