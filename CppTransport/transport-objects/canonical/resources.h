//
// Created by David Seery on 19/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CANONICAL_RESOURCES_H
#define CPPTRANSPORT_CANONICAL_RESOURCES_H


#include <memory>
#include <vector>

#include "resource_manager.h"
#include "translator_data.h"
#include "expression_cache.h"
#include "indices.h"
#include "index_flatten.h"

#include "shared_resources.h"
#include "language_printer.h"

#include "concepts/flattened_tensor.h"

#include "timing_instrument.h"

#include "ginac/ginac.h"


namespace canonical
  {

    constexpr unsigned int use_dV_argument = 1;
    constexpr unsigned int use_ddV_argument = 2;
    constexpr unsigned int use_dddV_argument = 4;

    //! implements resources for canonical models, ie. trivial kinetic terms and just a potential
    class resources
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        resources(translator_data& p, resource_manager& m, expression_cache& c, shared_resources& s, boost::timer::cpu_timer& t);

        //! destructor is default
        ~resources() = default;


        // INTERFACE -- BUILD RESOURCES

      public:

        //! generate V resource
        GiNaC::ex V_resource(language_printer& printer);

        //! generate epsilon resource
        GiNaC::ex eps_resource(language_printer& printer);

        //! generate Hubble-squared resource
        GiNaC::ex Hsq_resource(language_printer& printer);

        //! generate dV resources
        std::unique_ptr<flattened_tensor> dV_resource(language_printer& printer);

        //! generate ddV resources
        std::unique_ptr<flattened_tensor> ddV_resource(language_printer& printer);

        //! generate dddV resources
        std::unique_ptr<flattened_tensor> dddV_resource(language_printer& printer);


        // INTERFACE -- BUILD ARGUMENT LISTS
        // used to interact with expression cache

      public:

        //! generate argument list
        std::unique_ptr<ginac_cache_args> generate_arguments(unsigned int flags, language_printer& printer) const;


        // INTERFACE -- QUERY ROLL/UNROLL AVAILABILITY

      public:

        //! query whether dV can be rolled-up into loops
        bool roll_dV() const;

        //! query whether ddV can be rolled-up into loops
        bool roll_ddV() const;

        //! query whether dddV can be rolled-up into loops
        bool roll_dddV() const;


        // INTERNAL API

      private:

        //! generate argument list for param/field combinations;
        //! used internally and as the first step in generating an external argument list
        std::unique_ptr<ginac_cache_args> generate_arguments(language_printer& printer) const;


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

  }   // namespace canonical


#endif //CPPTRANSPORT_CANONICAL_RESOURCES_H
