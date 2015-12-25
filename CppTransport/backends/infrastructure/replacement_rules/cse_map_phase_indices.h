//
// Created by David Seery on 22/12/2015.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_CSE_MAP_PHASE_INDICES_H
#define CPPTRANSPORT_CSE_MAP_PHASE_INDICES_H


#include "replacement_rule_definitions.h"
#include "cse.h"
#include "indices.h"
#include "index_flatten.h"


namespace macro_packages
  {

    class cse_map_phase1 : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cse_map_phase1(std::string nm, unsigned int a, param_index num_params, field_index num_fields)
          : replacement_rule_index(std::move(nm), a, 1, index_class::full),
            fl(static_cast<unsigned int>(num_params), static_cast<unsigned int>(num_fields))
          {
          }

        //! destructor
        virtual ~cse_map_phase1() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! default post-hook will release CSE map
        virtual void post_hook(const macro_argument_list&) override { this->map.release(); }


        // INTERNAL DATA

      protected:

        //! CSE map object; should be filled in by implementation-defined 'pre' method
        std::unique_ptr<cse_map> map;

        //! index flattener
        index_flatten fl;

      };


    class cse_map_phase2 : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cse_map_phase2(std::string nm, unsigned int a, param_index num_params, field_index num_fields)
          : replacement_rule_index(std::move(nm), a, 2, index_class::full),
            fl(static_cast<unsigned int>(num_params), static_cast<unsigned int>(num_fields))
          {
          }

        //! destructor
        virtual ~cse_map_phase2() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! default post-hook will release CSE map
        virtual void post_hook(const macro_argument_list&) override { this->map.release(); }


        // INTERNAL DATA

      protected:

        //! CSE map object; should be filled in by implementation-defined 'pre' method
        std::unique_ptr<cse_map> map;

        //! index flattener
        index_flatten fl;

      };


    class cse_map_phase3 : public replacement_rule_index
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        cse_map_phase3(std::string nm, unsigned int a, param_index num_params, field_index num_fields)
          : replacement_rule_index(std::move(nm), a, 3, index_class::full),
            fl(static_cast<unsigned int>(num_params), static_cast<unsigned int>(num_fields))
          {
          }

        //! destructor
        virtual ~cse_map_phase3() = default;


        // INTERNAL API

      protected:

        //! evaluate
        virtual std::string unroll(const macro_argument_list& args, const assignment_list& indices) override;

        //! default post-hook will release CSE map
        virtual void post_hook(const macro_argument_list&) override { this->map.release(); }


        // INTERNAL DATA

      protected:

        //! CSE map object; should be filled in by implementation-defined 'pre' method
        std::unique_ptr<cse_map> map;

        //! index flattener
        index_flatten fl;

      };

  }   // namespace macro_packages


#endif //CPPTRANSPORT_CSE_MAP_PHASE_INDICES_H
