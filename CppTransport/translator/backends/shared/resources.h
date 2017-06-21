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

#ifndef CPPTRANSPORT_MACROS_RESOURCES_H
#define CPPTRANSPORT_MACROS_RESOURCES_H


#include "directive_package.h"
#include "concepts/resource_manager.h"
#include "nontrivial-metric/resource_manager.h"


namespace macro_packages
  {

    namespace RESOURCES
      {

        constexpr unsigned int PARAMETERS_KERNEL_ARGUMENT = 0;
        constexpr unsigned int PARAMETERS_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int COORDINATES_KERNEL_ARGUMENT = 0;
        constexpr unsigned int COORDINATES_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int COORDINATES_TOTAL_INDICES = 1;

        constexpr unsigned int PHASE_FLATTEN_KERNEL_ARGUMENT = 0;
        constexpr unsigned int PHASE_FLATTEN_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int FIELD_FLATTEN_KERNEL_ARGUMENT = 0;
        constexpr unsigned int FIELD_FLATTEN_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int RELEASE_FLATTENERS_TOTAL_ARGUMENTS = 0;

        constexpr unsigned int DV_KERNEL_ARGUMENT = 0;
        constexpr unsigned int DV_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int DV_TOTAL_INDICES = 1;

        constexpr unsigned int DDV_KERNEL_ARGUMENT = 0;
        constexpr unsigned int DDV_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int DDV_TOTAL_INDICES = 2;

        constexpr unsigned int DDDV_KERNEL_ARGUMENT = 0;
        constexpr unsigned int DDDV_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int DDDV_TOTAL_INDICES = 3;

        constexpr unsigned int CONNEXION_KERNEL_ARGUMENT = 0;
        constexpr unsigned int CONNEXION_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int CONNEXION_TOTAL_INDICES = 3;
        
        constexpr unsigned int METRIC_KERNEL_ARGUMENT = 0;
        constexpr unsigned int METRIC_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int METRIC_TOTAL_INDICES = 2;
    
        constexpr unsigned int INVERSE_METRIC_KERNEL_ARGUMENT = 0;
        constexpr unsigned int INVERSE_METRIC_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int INVERSE_METRIC_TOTAL_INDICES = 2;

        constexpr unsigned int RIEMANN_A2_KERNEL_ARGUMENT = 0;
        constexpr unsigned int RIEMANN_A2_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int RIEMANN_A2_TOTAL_INDICES = 2;
    
        constexpr unsigned int RIEMANN_A3_KERNEL_ARGUMENT = 0;
        constexpr unsigned int RIEMANN_A3_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int RIEMANN_A3_TOTAL_INDICES = 3;
    
        constexpr unsigned int RIEMANN_B3_KERNEL_ARGUMENT = 0;
        constexpr unsigned int RIEMANN_B3_TOTAL_ARGUMENTS = 1;
        constexpr unsigned int RIEMANN_B3_TOTAL_INDICES = 3;

        constexpr unsigned int RELEASE_TOTAL_ARGUMENTS = 0;

        constexpr unsigned int WORKING_TYPE_KERNEL_ARGUMENT = 0;
        constexpr unsigned int WORKING_TYPE_TOTAL_ARGUMENTS = 1;

        constexpr unsigned int RELEASE_WORKING_TYPE_TOTAL_ARGUMENTS = 0;

      }   // namespace RESOURCES


    class set_params : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_params(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::PARAMETERS_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_params() = default;


        // INTERNAL API

      protected:

        //! apply
        std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_coordinates : public directive_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_coordinates(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::COORDINATES_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_coordinates() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;

        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);

        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_phase_flatten : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_phase_flatten(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::PHASE_FLATTEN_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_phase_flatten() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_field_flatten : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_field_flatten(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::FIELD_FLATTEN_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_field_flatten() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_dV : public directive_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_dV(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::DV_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_dV() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;

        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);

        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_ddV : public directive_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_ddV(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::DDV_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_ddV() = default;


        // INTERNAL  API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;

        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);

        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_dddV : public directive_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_dddV(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::DDDV_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_dddV() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;

        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);

        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };
    
    
    //! field-space connexion
    //! dynamic cast to nontrivial_metric::nontrivial_metric_resource_manager& will fail if the model is not
    //! appropriate for this directive, disabling it
    class set_connexion : public directive_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_connexion(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::CONNEXION_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(dynamic_cast<nontrivial_metric::nontrivial_metric_resource_manager&>(m))
          {
          }

        //! destructor
        virtual ~set_connexion() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;

        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);

        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);


        // INTERNAL DATA

      private:
        
        //! resource manager
        nontrivial_metric::nontrivial_metric_resource_manager& mgr;

      };
    
    
    //! field-space metric
    //! dynamic cast to nontrivial_metric::nontrivial_metric_resource_manager& will fail if the model is not
    //! appropriate for this directive, disabling it
    class set_metric : public directive_index
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        set_metric(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::CONNEXION_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(dynamic_cast<nontrivial_metric::nontrivial_metric_resource_manager&>(m))
          {
          }
        
        //! destructor
        virtual ~set_metric() = default;
        
        
        // INTERNAL API
      
      protected:
        
        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);
        
        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        nontrivial_metric::nontrivial_metric_resource_manager& mgr;
        
      };
    
    
    //! field-space inverse metric
    //! dynamic cast to nontrivial_metric::nontrivial_metric_resource_manager& will fail if the model is not
    //! appropriate for this directive, disabling it
    class set_inverse_metric : public directive_index
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        set_inverse_metric(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::CONNEXION_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(dynamic_cast<nontrivial_metric::nontrivial_metric_resource_manager&>(m))
          {
          }
        
        //! destructor
        virtual ~set_inverse_metric() = default;
        
        
        // INTERNAL API
      
      protected:
        
        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);
        
        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        nontrivial_metric::nontrivial_metric_resource_manager& mgr;
        
      };


    //! Riemann 'A2' is the 2-index combination of the Riemann tensor appearing in the A-tensor
    //! (it also appears in the M-tensor)
    //! dynamic cast to nontrivial_metric::nontrivial_metric_resource_manager& will fail if the model is not
    //! appropriate for this directive, disabling it
    class set_Riemann_A2 : public directive_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_Riemann_A2(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::RIEMANN_A2_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(dynamic_cast<nontrivial_metric::nontrivial_metric_resource_manager&>(m))
          {
          }

        //! destructor
        virtual ~set_Riemann_A2() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;

        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);

        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);


        // INTERNAL DATA

      private:

        //! resource manager
        nontrivial_metric::nontrivial_metric_resource_manager& mgr;

      };
    
    
    //! Riemann 'A3' is the 3-index combination of the Riemann tensor appearing in the A-tensor
    //! dynamic cast to nontrivial_metric::nontrivial_metric_resource_manager& will fail if the model is not
    //! appropriate for this directive, disabling it
    class set_Riemann_A3 : public directive_index
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        set_Riemann_A3(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::RIEMANN_A3_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(dynamic_cast<nontrivial_metric::nontrivial_metric_resource_manager&>(m))
          {
          }
        
        //! destructor
        virtual ~set_Riemann_A3() = default;
        
        
        // INTERNAL API
      
      protected:
        
        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);
        
        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        nontrivial_metric::nontrivial_metric_resource_manager& mgr;
        
      };
    
    
    //! Riemann 'B3' is the 2-index combination of the Riemann tensor appearing in the B-tensor
    //! dynamic cast to nontrivial_metric::nontrivial_metric_resource_manager& will fail if the model is not
    //! appropriate for this directive, disabling it
    class set_Riemann_B3 : public directive_index
      {
        
        // CONSTRUCTOR, DESTRUCTOR
      
      public:
        
        //! constructor
        set_Riemann_B3(std::string n, resource_manager& m, translator_data& p)
          : directive_index(std::move(n), RESOURCES::RIEMANN_B3_TOTAL_ARGUMENTS, p,
                            define_indices(p.model.get_lagrangian_type()), define_classes(p.model.get_lagrangian_type())),
            mgr(dynamic_cast<nontrivial_metric::nontrivial_metric_resource_manager&>(m))
          {
          }
        
        //! destructor
        virtual ~set_Riemann_B3() = default;
        
        
        // INTERNAL API
      
      protected:
        
        //! apply
        virtual std::string apply(const macro_argument_list& args, const index_literal_list& indices) override;
        
        //! determine how many indices should be expected
        boost::optional<unsigned int> define_indices(model_type t);
        
        //! determine which index classes should be expected
        boost::optional< std::vector<index_class> > define_classes(model_type t);
        
        
        // INTERNAL DATA
      
      private:
        
        //! resource manager
        nontrivial_metric::nontrivial_metric_resource_manager& mgr;
        
      };


    //! directive to release all resources, *except* flattener
    //! this is the most common use case; we wish to release resources for eg. coordinates or
    //! dV, ddV, dddV that have been used in a function body, but we don't want to
    //! change the flattener function
    class release : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RELEASE_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~release() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    //! directive to release names of flattener functions
    //! this is a much less common use-case than just release of resources
    class release_flatteners : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release_flatteners(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RELEASE_FLATTENERS_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~release_flatteners() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class set_working_type : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        set_working_type(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::WORKING_TYPE_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~set_working_type() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class release_working_type : public directive_simple
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        release_working_type(std::string n, resource_manager& m, translator_data& p)
          : directive_simple(std::move(n), RESOURCES::RELEASE_WORKING_TYPE_TOTAL_ARGUMENTS, p),
            mgr(m)
          {
          }

        //! destructor
        virtual ~release_working_type() = default;


        // INTERNAL API

      protected:

        //! apply
        virtual std::string apply(const macro_argument_list& args) override;


        // INTERNAL DATA

      private:

        //! resource manager
        resource_manager& mgr;

      };


    class resources : public directive_package
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        resources(translator_data& p, resource_manager& m);

        //! destructor is default
        virtual ~resources() = default;

      };

  }


#endif //CPPTRANSPORT_MACROS_RESOURCES_H
