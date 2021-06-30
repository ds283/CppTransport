//
// Created by David Seery on 22/01/2016.
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

#ifndef CPPTRANSPORT_MANAGER_DETAIL_JOB_DESCRIPTORS_H
#define CPPTRANSPORT_MANAGER_DETAIL_JOB_DESCRIPTORS_H


#include <string>
#include <utility>
#include <unordered_map>

#include "transport-runtime/manager/mpi_operations.h"


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

            job_descriptor(job_type t, std::string n, tag_list tg, std::string o)
              : type(t),
                name(std::move(n)),
                tags(std::move(tg)),
                output(std::move(o)),
                seeded(false)
              {
              }

            job_descriptor(job_type t, std::string n, tag_list tg)
              : type(t),
                name(std::move(n)),
                tags(std::move(tg)),
                seeded(false)
              {
              }


            // INTERFACE

          public:

            //! return job type
            job_type get_type() const
              {
                return this->type;
              }


            //! return job name
            const std::string& get_name() const
              {
                return this->name;
              }


            //! return job tags
            const std::list<std::string>& get_tags() const
              {
                return this->tags;
              }


            //! return job output name
            const std::string& get_output() const
              {
                return this->output;
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
                return this->seeded;
              }


            //! get job seeding group
            const std::string& get_seed_group() const
              {
                return this->seed_group;
              };


            // INTERNAL DATA

          private:

            //! job type
            job_type type;

            //! job name
            std::string name;

            //! tags associated with job
            tag_list tags;

            //! output destination, if needed
            std::string output;

            //! is this job seeded
            bool seeded;

            //! seed group, if used
            std::string seed_group;

          };


        class FindJobDescriptorByName
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            FindJobDescriptorByName(std::string n)
              : name(n)
              {
              }

            //! destructor
            ~FindJobDescriptorByName() = default;


            // INTERFACE

          public:

            //! test for name
            bool operator()(const job_descriptor& j) const
              {
                return j.get_name() == this->name;
              }


            // INTERNAL DATA

          private:

            //! name to search for
            const std::string name;

          };


        class CompareJobDescriptorByList
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            CompareJobDescriptorByList(const ordered_record_name_set& order);

            //! destructor
            ~CompareJobDescriptorByList() = default;


            // INTERFACE

          public:

            //! comparison operator; note can't be marked const since
            //! order_map may be modified if job descriptors are not present
            bool operator()(const job_descriptor& A, const job_descriptor& B);


            // INTERNAL DATA

          private:

            //! map from names to ordering
            std::unordered_map< ordered_record_name_set::value_type, unsigned int > order_map;

          };


        CompareJobDescriptorByList::CompareJobDescriptorByList(const ordered_record_name_set& order)
          {
            unsigned int number = 0;
            for(const std::string& object : order)
              {
                this->order_map[object] = number++;
              }
          }


        bool CompareJobDescriptorByList::operator()(const job_descriptor& A, const job_descriptor& B)
          {
            return this->order_map[A.get_name()] < this->order_map[B.get_name()];
          }


      }   // namespace master_controller_impl

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_JOB_DESCRIPTORS_H
