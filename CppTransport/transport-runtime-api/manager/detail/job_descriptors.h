//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MANAGER_DETAIL_JOB_DESCRIPTORS_H
#define CPPTRANSPORT_MANAGER_DETAIL_JOB_DESCRIPTORS_H


#include <string>

#include "transport-runtime-api/manager/mpi_operations.h"


namespace transport
  {

    namespace master_controller_impl
      {

        //! Labels for types of job
        enum class job_type { job_task };


        //! Job descriptor class
        class job_descriptor
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            job_descriptor(job_type t, const std::string& n, const std::list<std::string>& tg, const std::string& o)
              : type(t),
                name(n),
                tags(tg),
                output(o),
                seeded(false)
              {
              }

            job_descriptor(job_type t, const std::string& n, const std::list<std::string>& tg)
              : type(t),
                name(n),
                tags(tg),
                seeded(false)
              {
              }


            // INTERFACE

          public:

            //! return job type
            job_type get_type() const
              {
                return (this->type);
              }


            //! return job name
            const std::string& get_name() const
              {
                return (this->name);
              }


            //! return job tags
            const std::list<std::string>& get_tags() const
              {
                return (this->tags);
              }


            //! return job output name
            const std::string& get_output() const
              {
                return (this->output);
              }


            //! set job seeding group
            void set_seed(const std::string& s)
              {
                this->seeded = true;
                this->seed_group = s;
              }


            //! determine job seeding status
            bool is_seeded() const
              {
                return (this->seeded);
              }


            //! get job seeding group
            const std::string& get_seed_group() const
              {
                return (this->seed_group);
              };


            // INTERNAL DATA

          private:

            //! job type
            job_type type;

            //! job name
            std::string name;

            //! tags associated with job
            std::list<std::string> tags;

            //! output destination, if needed
            std::string output;

            //! is this job seeded
            bool seeded;

            //! seed group, if used
            std::string seed_group;

          };

      }   // namespace master_controller_impl

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_JOB_DESCRIPTORS_H
