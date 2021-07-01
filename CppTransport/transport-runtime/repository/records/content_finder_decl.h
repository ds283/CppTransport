//
// Created by David Seery on 25/01/2016.
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

#ifndef CPPTRANSPORT_CONTENT_FINDER_DECL_H
#define CPPTRANSPORT_CONTENT_FINDER_DECL_H


#include <string>
#include <functional>
#include <list>

// forward-declare repository and its records if necessary
#include "transport-runtime/repository/records/repository_records_forward_declare.h"
#include "transport-runtime/repository/repository_forward_declare.h"


namespace transport
  {


    //! ContentGroupFilterPredicate is used to filter content groups supplied by task_record<>
    //! for specific properties
    template <typename number>
    class ContentGroupFilterPredicate
      {
      public:

        //! constructor captures repository, content group specifiers and tag list
        ContentGroupFilterPredicate(repository<number>& r, const content_group_specifier& s, const tag_list& tg)
          : repo{r},
            specifier{s},
            tags{tg}
          {
          }

        // INTERFACE

      public:

        //! determine whether a named content group matches our criteria
        bool operator()(const std::string& name) const
          {
            switch(this->specifier.get_type())
              {
                case task_type::integration:
                  return this->match_integration_content_group(name);

                case task_type::postintegration:
                  return this->match_postintegration_content_group(name);

                case task_type::output:
                  throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_BAD_TASK_TYPE_CONTENT_GROUP_FILTER);
              }
            return true;
          }


        // INTERNAL METHODS

      protected:

        //! test integration content group
        bool match_integration_content_group(const std::string& name) const;

        //! test postintegration content group
        bool match_postintegration_content_group(const std::string& name) const;

        // INTERNAL DATA

      private:

        //! reference to repository
        repository<number>& repo;

        //!± content group specifier, or nullptr if not specified
        const content_group_specifier& specifier;

        //! refernce to supplied tag list (which may be empty)
        const tag_list& tags;

      };


    template <typename number>
    class integration_content_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        explicit integration_content_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~integration_content_finder() = default;


        // INTERFACE

      public:

        // TODO: consider whether we want to set the policy on construction. Do we always want to apply the
        //  same policy with the same instance of a content finder?

        //! Find content group. Returns a std::unique_ptr<> containing the repository record for the matched
        //! group. Where multiple groups match, a policy is applied to select a preferred group.
        //! If raise is set, an exception is raised if no content group can be found. Otherwise, an empty
        //! std::unique_ptr<> is returned if no content group can be matched.
        template <typename ContentSelectorPolicyType = repository_impl::MostRecentContentPolicy<integration_content_db> >
        std::unique_ptr<content_group_record < integration_payload> >
        operator()
          (const std::string& name, const content_group_specifier& specifier, const tag_list& tags,
           bool raise = true, ContentSelectorPolicyType policy = ContentSelectorPolicyType());


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


    template <typename number>
    class postintegration_content_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor captures repository object
        explicit postintegration_content_finder(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor is default
        ~postintegration_content_finder() = default;


        // INTERFACE

      public:

        // TODO: consider whether we want to set the policy on construction. Do we always want to apply the
        //  same policy with the same instance of a content finder?

        //! Find content group. Returns a std::unique_ptr<> containing the repository record for the matched
        //! group. Where multiple groups match, a policy is applied to select a preferred group.
        //! If raise is set, an exception is raised if no content group can be found. Otherwise, an empty
        //! std::unique_ptr<> is returned if no content group can be matched.
        template <typename ContentSelectorPolicyType = repository_impl::MostRecentContentPolicy<postintegration_content_db> >
        std::unique_ptr<content_group_record < postintegration_payload> >
        operator()
          (const std::string& name, const content_group_specifier& specifier, const tag_list& tags,
           bool raise = true, ContentSelectorPolicyType policy = ContentSelectorPolicyType());


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

      };


  }   // namespace transport


#endif //CPPTRANSPORT_CONTENT_FINDER_DECL_H
