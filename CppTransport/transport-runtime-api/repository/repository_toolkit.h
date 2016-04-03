//
// Created by David Seery on 03/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_REPOSITORY_TOOLKIT_H
#define CPPTRANSPORT_REPOSITORY_TOOLKIT_H


#include "transport-runtime-api/repository/repository.h"

// need Boost regular expressions for pattern matching
#include "boost/regex.hpp"


namespace transport
  {

    namespace repository_toolkit_impl
      {

        //! template method to extract a content group
        template <typename number, typename Payload>
        std::unique_ptr< output_group_record<Payload> > get_rw_content_group(repository<number>& repo, const std::string& name, transaction_manager& mgr);


        //! specialize for integration payloads
        template <>
        std::unique_ptr< output_group_record< integration_payload> > get_rw_content_group(repository<double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_integration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< integration_payload> > get_rw_content_group(repository<float>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_integration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< integration_payload> > get_rw_content_group(repository<long double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_integration_content(name, mgr);
          }


        //! specialize for postintegration payloads
        template <>
        std::unique_ptr< output_group_record< postintegration_payload> > get_rw_content_group(repository<double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_postintegration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< postintegration_payload> > get_rw_content_group(repository<float>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_postintegration_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< postintegration_payload> > get_rw_content_group(repository<long double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_postintegration_content(name, mgr);
          }


        //! specialize for output payloads
        template <>
        std::unique_ptr< output_group_record< output_payload> > get_rw_content_group(repository<double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_output_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< output_payload> > get_rw_content_group(repository<float>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_output_content(name, mgr);
          }

        template <>
        std::unique_ptr< output_group_record< output_payload> > get_rw_content_group(repository<long double>& repo, const std::string& name, transaction_manager& mgr)
          {
            return repo.query_output_content(name, mgr);
          }

      }   // namespace repository_toolkit_impl


    // pull in repository_toolkit_impl for this scope
    using namespace repository_toolkit_impl;


    //! provides tools to manipulate repository records
    template <typename number>
    class repository_toolkit
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        repository_toolkit(repository<number>& r)
          : repo(r)
          {
          }

        //! destructor
        ~repository_toolkit() = default;


        // INTERFACE

      public:

        //! update tags and notes
        void update_tags_notes(const std::vector<std::string>& objects,
                               const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                               const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove);


        // INTERNAL API

      protected:

        //! iterate over a content database, updating tags and notes
        template <typename ContentDatabase>
        void update_tags_notes(ContentDatabase& db, const std::vector<std::string>& objects,
                               const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                               const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove);


        // INTERNAL DATA

      private:

        //! reference to repository objects
        repository<number>& repo;

      };


    template <typename number>
    void repository_toolkit<number>::update_tags_notes(const std::vector<std::string>& objects,
                                                       const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                                                       const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove)
      {
        // only output group records carry tags and notes
        integration_content_db integration_content = repo.enumerate_integration_task_content();
        postintegration_content_db postintegration_content = repo.enumerate_postintegration_task_content();
        output_content_db output_content = repo.enumerate_output_task_content();

        this->update_tags_notes(integration_content, objects, tags_add, tags_remove, notes_add, notes_remove);
        this->update_tags_notes(postintegration_content, objects, tags_add, tags_remove, notes_add, notes_remove);
        this->update_tags_notes(output_content, objects, tags_add, tags_remove, notes_add, notes_remove);
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::update_tags_notes(ContentDatabase& db, const std::vector<std::string>& objects,
                                                       const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                                                       const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(const typename ContentDatabase::value_type& item : db)
          {
            // ContentDatabase has mapped_type equal to std::unique_ptr< output_group_record<PayloadType> >
            // to get the output_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(const std::string& object_expr_string : objects)
              {
                boost::regex object_expr(object_expr_string);
                if(boost::regex_match(record.get_name(), object_expr))
                  {
                    // re-query the database to get a read/write version of this record
                    typename ContentDatabase::mapped_type rw_record = get_rw_content_group<number, typename ContentDatabase::mapped_type::element_type::payload_type>(this->repo, record.get_name(), mgr);

                    for(const std::string& tag : tags_remove)
                      {
                        rw_record->remove_tag(tag);
                      }

                    for(const std::string& tag : tags_add)
                      {
                        rw_record->add_tag(tag);
                      }

                    for(const std::string& note : notes_remove)
                      {
                        rw_record->remove_note(boost::lexical_cast<unsigned int>(note));
                      }

                    for(const std::string& note : notes_add)
                      {
                        rw_record->add_note(note);
                      }

                    // recommit record
                    // in the current implementation there is no need to re-read the value in the enumerated database, because
                    // each record in the enumeration is inspected only once
                    rw_record->commit();
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


  }


#endif //CPPTRANSPORT_REPOSITORY_TOOLKIT_H
