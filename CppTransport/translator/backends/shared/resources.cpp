//
// Created by David Seery on 18/12/2015.
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


#include <assert.h>
#include <functional>

#include "resources.h"
#include "concepts/resource_manager.h"
#include "nontrivial-metric/resource_manager.h"

#include "flow_tensors.h"


#define BIND(X, N) std::move(std::make_unique<X>(N, m, p))
#define EMPLACE(pkg, obj) try { emplace_directive(pkg, obj); } catch(std::exception& xe) { }


namespace macro_packages
  {

    resources::resources(translator_data& p, resource_manager& m)
      : directive_package(p)
      {
        EMPLACE(simple_package, BIND(set_params, "RESOURCE_PARAMETERS"));
        EMPLACE(simple_package, BIND(set_coordinates, "RESOURCE_COORDINATES"));
        EMPLACE(simple_package, BIND(set_phase_flatten, "PHASE_FLATTEN"));
        EMPLACE(simple_package, BIND(set_field_flatten, "FIELD_FLATTEN"));
        EMPLACE(simple_package, BIND(release_flatteners, "RELEASE_FLATTENERS"));
        EMPLACE(simple_package, BIND(release, "RESOURCE_RELEASE"));
        EMPLACE(simple_package, BIND(set_working_type, "WORKING_TYPE"));
        EMPLACE(simple_package, BIND(release_working_type, "RELEASE_WORKING_TYPE"));

        EMPLACE(index_package, BIND(set_metric, "RESOURCE_G"));
        EMPLACE(index_package, BIND(set_dV, "RESOURCE_DV"));
        EMPLACE(index_package, BIND(set_ddV, "RESOURCE_DDV"));
        EMPLACE(index_package, BIND(set_dddV, "RESOURCE_DDDV"));
        EMPLACE(index_package, BIND(set_connexion, "RESOURCE_CONNECTION"));
        EMPLACE(index_package, BIND(set_Riemann_A2, "RESOURCE_RIEMANN_A2"));
        EMPLACE(index_package, BIND(set_Riemann_A3, "RESOURCE_RIEMANN_A3"));
        EMPLACE(index_package, BIND(set_Riemann_B3, "RESOURCE_RIEMANN_B3"));
      }


    std::string set_params::apply(const macro_argument_list& args)
      {
        this->mgr.assign_parameters(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PARAMETERS << " '" << static_cast<std::string>(args[RESOURCES::PARAMETERS_KERNEL_ARGUMENT]) << "'";

        return msg.str();
      }


    std::string set_coordinates::apply(const macro_argument_list& args)
      {
        this->mgr.assign_coordinates(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_COORDINATES << " '" << static_cast<std::string>(args[RESOURCES::COORDINATES_KERNEL_ARGUMENT]) << "'";

        return msg.str();
      }


    std::string set_phase_flatten::apply(const macro_argument_list& args)
      {
        this->mgr.assign_phase_flatten(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_PHASE_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::PHASE_FLATTEN_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string set_field_flatten::apply(const macro_argument_list& args)
      {
        this->mgr.assign_field_flatten(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_FIELD_FLATTEN << " '" << static_cast<std::string>(args[RESOURCES::FIELD_FLATTEN_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string release_flatteners::apply(const macro_argument_list& args)
      {
        this->mgr.release_flatteners();

        return RESOURCE_RELEASE_FLATTENERS;
      }


    std::string set_dV::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        // build list of index variances
        std::array<variance, RESOURCE_INDICES::DV_INDICES> v = { variance::none };
        for(unsigned int i = 0; i < v.size() && i < indices.size(); ++i)
          {
            v[i] = indices[i].get()->get_variance();
          }

        this->mgr.assign_dV(args[RESOURCES::DV_KERNEL_ARGUMENT], v);

        std::ostringstream msg;
        msg << RESOURCE_SET_DV << " '" << static_cast<std::string>(args[RESOURCES::DV_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    boost::optional<unsigned int> set_dV::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCES::DV_TOTAL_INDICES;
          }
      }


    boost::optional<std::vector<index_class> > set_dV::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric: return std::vector<index_class>{ index_class::field_only };
          }
      }


    std::string set_ddV::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        // build list of index variances
        std::array<variance, RESOURCE_INDICES::DDV_INDICES> v = { variance::none, variance::none };
        for(unsigned int i = 0; i < v.size() && i < indices.size(); ++i)
          {
            v[i] = indices[i].get()->get_variance();
          }

        this->mgr.assign_ddV(args[RESOURCES::DDV_KERNEL_ARGUMENT], v);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDV << " '" << static_cast<std::string>(args[RESOURCES::DDV_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    boost::optional<unsigned int> set_ddV::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCES::DDV_TOTAL_INDICES;
          }
      }


    boost::optional<std::vector<index_class> > set_ddV::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric:
              return std::vector<index_class>{ index_class::field_only, index_class::field_only };
          }
      }


    std::string set_dddV::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        // build list of index variances
        std::array<variance, RESOURCE_INDICES::DDDV_INDICES> v = { variance::none, variance::none, variance::none };
        for(unsigned int i = 0; i < v.size() && i < indices.size(); ++i)
          {
            v[i] = indices[i].get()->get_variance();
          }

        this->mgr.assign_dddV(args[RESOURCES::DDDV_KERNEL_ARGUMENT], v);

        std::ostringstream msg;
        msg << RESOURCE_SET_DDDV << " '" << static_cast<std::string>(args[RESOURCES::DDDV_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    boost::optional<unsigned int> set_dddV::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCES::DDDV_TOTAL_INDICES;
          }
      }


    boost::optional<std::vector<index_class> > set_dddV::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric:
              return std::vector<index_class>{ index_class::field_only, index_class::field_only, index_class::field_only };
          }
      }


    std::string set_connexion::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        this->mgr.assign_connexion(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_CONNEXION << " '" << static_cast<std::string>(args[RESOURCES::CONNEXION_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    boost::optional<unsigned int> set_connexion::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return 0;
          }
      }


    boost::optional<std::vector<index_class> > set_connexion::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric: return boost::none;
          }
      }
    
    
    std::string set_metric::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        if(indices[0]->get_variance() != indices[1]->get_variance())
          {
            throw rule_apply_fail(ERROR_METRIC_RESOURCE_MIXED_INDICES);
          }
    
        std::ostringstream msg;

        if(indices[0]->get_variance() == variance::contravariant)
          {
            this->mgr.assign_metric(args[RESOURCES::METRIC_KERNEL_ARGUMENT]);
            msg << RESOURCE_SET_METRIC << " '" << static_cast<std::string>(args[RESOURCES::METRIC_KERNEL_ARGUMENT]) << "'";
          }
        else
          {
            this->mgr.assign_metric_inverse(args[RESOURCES::METRIC_KERNEL_ARGUMENT]);
            msg << RESOURCE_SET_METRIC_INVERSE << " '" << static_cast<std::string>(args[RESOURCES::METRIC_KERNEL_ARGUMENT]) << "'";
          }
        
        return msg.str();
      }
    
    
    boost::optional<unsigned int> set_metric::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCE_INDICES::METRIC_INDICES;
          }
      }
    
    
    boost::optional<std::vector<index_class> > set_metric::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric:
              return std::vector<index_class>{ index_class::field_only, index_class::field_only};
          }
      }


    std::string set_Riemann_A2::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        // build list of index variances
        std::array<variance, RESOURCE_INDICES::RIEMANN_A2_INDICES> v = { variance::none, variance::none };
        for(unsigned int i = 0; i < v.size() && i < indices.size(); ++i)
          {
            v[i] = indices[i].get()->get_variance();
          }

        this->mgr.assign_Riemann_A2(args[RESOURCES::RIEMANN_A2_KERNEL_ARGUMENT], v);

        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RESOURCES::RIEMANN_A2_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    boost::optional<unsigned int> set_Riemann_A2::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCES::RIEMANN_A2_TOTAL_INDICES;
          }
      }


    boost::optional< std::vector<index_class> > set_Riemann_A2::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric:
              return std::vector<index_class>{ index_class::field_only, index_class::field_only };
          }
      }
    
    
    std::string set_Riemann_A3::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        // build list of index variances
        std::array<variance, RESOURCE_INDICES::RIEMANN_A3_INDICES> v = { variance::none, variance::none };
        for(unsigned int i = 0; i < v.size() && i < indices.size(); ++i)
          {
            v[i] = indices[i].get()->get_variance();
          }

        this->mgr.assign_Riemann_A3(args[RESOURCES::RIEMANN_A3_KERNEL_ARGUMENT], v);
        
        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RESOURCES::RIEMANN_A3_KERNEL_ARGUMENT]) << "'";
        
        return msg.str();
      }
    
    
    boost::optional<unsigned int> set_Riemann_A3::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCES::RIEMANN_A3_TOTAL_INDICES;
          }
      }
    
    
    boost::optional<std::vector<index_class> > set_Riemann_A3::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric:
              return std::vector<index_class>{ index_class::field_only, index_class::field_only, index_class::field_only };
          }
      }
    
    
    std::string set_Riemann_B3::apply(const macro_argument_list& args, const index_literal_list& indices)
      {
        // build list of index variances
        std::array<variance, RESOURCE_INDICES::RIEMANN_B3_INDICES> v = { variance::none, variance::none };
        for(unsigned int i = 0; i < v.size() && i < indices.size(); ++i)
          {
            v[i] = indices[i].get()->get_variance();
          }

        this->mgr.assign_Riemann_B3(args[RESOURCES::RIEMANN_B3_KERNEL_ARGUMENT], v);
        
        std::ostringstream msg;
        msg << RESOURCE_SET_RIEMANN << " '" << static_cast<std::string>(args[RESOURCES::RIEMANN_B3_KERNEL_ARGUMENT]) << "'";
        
        return msg.str();
      }
    
    
    boost::optional<unsigned int> set_Riemann_B3::define_indices(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return 0;
            case model_type::nontrivial_metric: return RESOURCES::RIEMANN_B3_TOTAL_INDICES;
          }
      }
    
    
    boost::optional<std::vector<index_class> > set_Riemann_B3::define_classes(model_type t)
      {
        switch(t)
          {
            case model_type::canonical: return boost::none;
            case model_type::nontrivial_metric:
              return std::vector<index_class>{ index_class::field_only, index_class::field_only, index_class::field_only };
          }
      }


    std::string release::apply(const macro_argument_list& args)
      {
        this->mgr.release();

        return RESOURCE_RELEASE;
      }


    std::string set_working_type::apply(const macro_argument_list& args)
      {
        this->mgr.assign_working_type(args[RESOURCES::WORKING_TYPE_KERNEL_ARGUMENT]);

        std::ostringstream msg;
        msg << RESOURCE_SET_WORKING_TYPE << " '" << static_cast<std::string>(args[RESOURCES::WORKING_TYPE_KERNEL_ARGUMENT]) << "'";
    
        return msg.str();
      }


    std::string release_working_type::apply(const macro_argument_list& args)
      {
        this->mgr.release_working_type();

        return RESOURCE_RELEASE_WORKING_TYPE;
      }

  }
