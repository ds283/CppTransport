//
// Created by David Seery on 03/04/2016.
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

#ifndef CPPTRANSPORT_REPOSITORY_TOOLKIT_H
#define CPPTRANSPORT_REPOSITORY_TOOLKIT_H


#include "transport-runtime/repository/repository.h"
#include "transport-runtime/repository/repository_graphkit.h"

#include "transport-runtime/manager/message_handlers.h"

#include "transport-runtime/utilities/match.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"
#include "transport-runtime/exceptions.h"


namespace transport
  {

    namespace repository_toolkit_impl
      {
    
        //! template method to get content group record from repository
        template <typename number, typename Payload>
        struct get_rw_content_group
          {
            
            std::unique_ptr< content_group_record<Payload> >
            get(repository<number>& repo, const std::string& name, transaction_manager& mgr);
            
          };


        //! specialize for integration payloads
        template <typename number>
        struct get_rw_content_group<number, integration_payload>
          {

            std::unique_ptr< content_group_record< integration_payload > >
            get(repository<number>& repo, const std::string& name, transaction_manager& mgr)
              {
                return repo.query_integration_content(name, mgr);
              }
            
          };

        //! specialize for postintegration payloads
        template <typename number>
        struct get_rw_content_group<number, postintegration_payload>
          {

            std::unique_ptr< content_group_record< postintegration_payload > >
            get(repository<number>& repo, const std::string& name, transaction_manager& mgr)
              {
                return repo.query_postintegration_content(name, mgr);
              }
            
          };

        //! specialize for output payloads
        template <typename number>
        struct get_rw_content_group<number, output_payload>
          {

            std::unique_ptr< content_group_record< output_payload > >
            get(repository<number>& repo, const std::string& name, transaction_manager& mgr)
              {
                return repo.query_output_content(name, mgr);
              }
            
          };


        //! template method to delete a content group
        template <typename number, typename Payload>
        struct erase_repository_record
          {
    
            void erase(repository<number>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr);
            
          };


        // specialize for integration payloads
        template <typename number>
        struct erase_repository_record<number, integration_payload>
          {

            void erase(repository<number>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
              {
                repo.delete_integration_content(name, task_name, mgr);
              }
            
          };

        // specialize for postintegration payloads
        template <typename number>
        struct erase_repository_record<number, postintegration_payload>
          {

            void erase(repository<number>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
              {
                repo.delete_postintegration_content(name, task_name, mgr);
              }
            
          };

        // specialize for output payloads
        template <typename number>
        struct erase_repository_record<number, output_payload>
          {

            void erase(repository<number>& repo, const std::string& name, const std::string& task_name, transaction_manager& mgr)
              {
                repo.delete_output_content(name, task_name, mgr);
              }
            
          };

      }   // namespace repository_toolkit_impl


    // pull in repository_toolkit_impl for this scope
    using namespace repository_toolkit_impl;




    //! repository_toolkit provides tools to manipulate repository records,
    //! or otherwise perform useful services which are not part of the core
    //! repository API
    template <typename number>
    class repository_toolkit
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        repository_toolkit(repository<number>& r, error_handler eh, warning_handler wh, message_handler mh)
          : repo(r),
            graphkit(r, eh, wh, mh),
            err(eh),
            warn(wh),
            msg(mh)
          {
          }

        //! destructor is default
        ~repository_toolkit() = default;


        // REPOSITORY RECORDS -- INTERFACE

      public:

        //! update tags and notes
        void update_tags_notes(const std::vector<std::string>& objects,
                               const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                               const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove);

        //! remove content groups
        void delete_content(const std::vector<std::string>& objects);

        //! lock records
        void lock_content(const std::vector<std::string>& objects);

        //! unlock records
        void unlock_content(const std::vector<std::string>& objects);


        // REPOSITORY RECORDS -- INTERNAL API

      protected:

        //! iterate over a content database, updating tags and notes
        template <typename ContentDatabase>
        void update_tags_notes(ContentDatabase& db, std::map< std::string, bool >& items,
                               const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                               const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove);

        //! iterate over a content database, deleting content groups
        template <typename ContentDatabase>
        void delete_content(ContentDatabase& db, std::map< std::string, bool >& items, std::unique_ptr<repository_distance_matrix>& dmat);

        //! iterate over a content database, locking content groups
        template <typename ContentDatabase>
        void lock_content(ContentDatabase& db, std::map< std::string, bool >& items);

        //! iterate over a content database, unlocking content groups
        template <typename ContentDatabase>
        void unlock_content(ContentDatabase& db, std::map< std::string, bool >& items);


        // INTERNAL DATA

      private:

        //! reference to repository object
        repository<number>& repo;

        //! graph toolkit
        repository_graphkit<number> graphkit;


        // MESSAGE HANDLERS

        //! error handler
        error_handler err;

        //! warning handler
        warning_handler warn;

        //! message handler
        message_handler msg;

      };


    template <typename number>
    void repository_toolkit<number>::update_tags_notes(const std::vector<std::string>& objects,
                                                       const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                                                       const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove)
      {
        // make local copy of objects to process; we will tag items in this list as 'processed' as we go
        std::map<std::string, bool> items;
        for(const std::string& item : objects)
          {
            items[item] = false;
          }

        // only content group records carry tags and notes
        integration_content_db integration_content = this->repo.enumerate_integration_task_content();
        postintegration_content_db postintegration_content = this->repo.enumerate_postintegration_task_content();
        output_content_db output_content = this->repo.enumerate_output_task_content();

        this->update_tags_notes(integration_content, items, tags_add, tags_remove, notes_add, notes_remove);
        this->update_tags_notes(postintegration_content, items, tags_add, tags_remove, notes_add, notes_remove);
        this->update_tags_notes(output_content, items, tags_add, tags_remove, notes_add, notes_remove);

        for(const std::pair< const std::string, bool >& item : items)
          {
            if(!item.second)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT << " '" << item.first << "'";
                this->msg(msg.str());
              }
          }
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::update_tags_notes(ContentDatabase& db, std::map< std::string, bool >& items,
                                                       const std::vector<std::string>& tags_add, const std::vector<std::string>& tags_remove,
                                                       const std::vector<std::string>& notes_add, const std::vector<std::string>& notes_remove)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(const typename ContentDatabase::value_type& item : db)
          {
            // ContentDatabase has mapped_type equal to std::unique_ptr< content_group_record<PayloadType> >
            // to get the content_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(std::pair< const std::string, bool >& t : items)
              {
                const std::string& match_expr = t.first;
                if(check_match(record.get_name(), match_expr, true))    // true = insist on exact match
                  {
                    t.second = true;

                    // re-query the database to get a read/write version of this record
                    typename ContentDatabase::mapped_type rw_record =
                      get_rw_content_group<number, typename ContentDatabase::mapped_type::element_type::payload_type>{}.get(
                        this->repo, record.get_name(), mgr);

                    if(rw_record->get_lock_status())
                      {
                        std::ostringstream msg;
                        msg << CPPTRANSPORT_REPO_TOOLKIT_IGNORING_LOCKED << " '" << record.get_name() << "'";
                        this->msg(msg.str());
                      }
                    else
                      {
                        for(const std::string& tag : tags_remove)   rw_record->remove_tag(tag);
                        for(const std::string& tag : tags_add)      rw_record->add_tag(tag);
                        for(const std::string& note : notes_remove) rw_record->remove_note(boost::lexical_cast<unsigned int>(note));
                        for(const std::string& note : notes_add)    rw_record->add_note(note);

                        // recommit record
                        // in the current implementation there is no need to re-read the value in the enumerated database
                        // (the record it contains is now out of date) because each record in the enumeration is inspected only once

                        // However, if multiple passes were ever implemented, the record in the enumeration would have to
                        // be modified
                        rw_record->commit();
                      }
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


    template <typename number>
    void repository_toolkit<number>::delete_content(const std::vector<std::string>& objects)
      {
        // make local copy of objects to process; we will tag items in this list as 'processed' as we go
        std::map<std::string, bool> items;
        for(const std::string& item : objects)
          {
            items[item] = false;
          }

        // cache distance matrix for the content group graph
        std::unique_ptr<repository_distance_matrix> dmat = this->graphkit.content_group_distance_matrix();

        // cache content databases
        integration_content_db integration_content = this->repo.enumerate_integration_task_content();
        postintegration_content_db postintegration_content = this->repo.enumerate_postintegration_task_content();
        output_content_db output_content = this->repo.enumerate_output_task_content();

        // attempt deletion in order output, postintegration, integration so that wildcard matches
        // will delete descendent content groups first;
        // that way we minimize 'can't delete because has descendents' errors
        this->delete_content(output_content, items, dmat);
        this->delete_content(postintegration_content, items, dmat);
        this->delete_content(integration_content, items, dmat);

        for(const std::pair< const std::string, bool >& item : items)
          {
            if(!item.second)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT << " '" << item.first << "'";
                this->msg(msg.str());
              }
          }
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::delete_content(ContentDatabase& db, std::map< std::string, bool >& items,
                                                    std::unique_ptr<repository_distance_matrix>& dmat)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(typename ContentDatabase::const_iterator t = db.begin(); t != db.end(); /* intentionally empty increment field */)
          {
            const typename ContentDatabase::value_type& item = *t;
            // ContentDatabase has mapped_type equal to std::unique_ptr< content_group_record<PayloadType> >
            // to get the content_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(std::pair< const std::string, bool >& item : items)
              {
                if(check_match(record.get_name(), item.first, true))    // true = insist on exact match
                  {
                    item.second = true;   // mark as a match for this item
                    std::unique_ptr< std::set<std::string> > dependent_groups = dmat->find_dependent_objects(record.get_name());

                    if(dependent_groups)
                      {
                        if(dependent_groups->size() == 0)
                          {
                            if(record.get_lock_status())
                              {
                                std::ostringstream msg;
                                msg << CPPTRANSPORT_REPO_TOOLKIT_IGNORING_LOCKED << " '" << record.get_name() << "'";
                                this->msg(msg.str());
                                ++t;
                              }
                            else
                              {
                                std::ostringstream msg;
                                msg << CPPTRANSPORT_REPO_TOOLKIT_DELETING_OBJECT << " '" << record.get_name() << "'";
                                this->msg(msg.str());

                                erase_repository_record<number, typename ContentDatabase::mapped_type::element_type::payload_type>{}.erase(
                                  this->repo, record.get_name(), record.get_task_name(),mgr);

                                // erase record and reset iterator to point to following element
                                t = db.erase(t);

                                // delete this record from the repository enumeration, and then recompute the distance matrix
                                std::unique_ptr<repository_distance_matrix> new_dmat = this->graphkit.content_group_distance_matrix();
                                dmat.swap(new_dmat);
                              }
                          }
                        else
                          {
                            std::ostringstream msg;
                            msg << CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_A << " '" << record.get_name() << "', "
                                << CPPTRANSPORT_REPO_TOOLKIT_CANNOT_DELETE_DEPENDENT_B << " ";

                            unsigned int count = 0;
                            for(const std::string& group : *dependent_groups)
                              {
                                if(count > 0) msg << ", ";
                                msg << group;
                                ++count;
                              }

                            this->err(msg.str());
                            ++t;
                          }
                      }
                  }
                else // no match, just increment iterator and move on
                  {
                    ++t;
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


    template <typename number>
    void repository_toolkit<number>::lock_content(const std::vector<std::string>& objects)
      {
        // make local copy of objects to process; we will tag items in this list as 'processed' as we go
        std::map<std::string, bool> items;
        for(const std::string& item : objects)
          {
            items[item] = false;
          }

        // cache content databases
        integration_content_db integration_content = this->repo.enumerate_integration_task_content();
        postintegration_content_db postintegration_content = this->repo.enumerate_postintegration_task_content();
        output_content_db output_content = this->repo.enumerate_output_task_content();

        this->lock_content(integration_content, items);
        this->lock_content(postintegration_content, items);
        this->lock_content(output_content, items);

        for(const std::pair< const std::string, bool >& item : items)
          {
            if(!item.second)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT << " '" << item.first << "'";
                this->msg(msg.str());
              }
          }
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::lock_content(ContentDatabase& db, std::map< std::string, bool >& items)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(const typename ContentDatabase::value_type& item : db)
          {
            // ContentDatabase has mapped_type equal to std::unique_ptr< content_group_record<PayloadType> >
            // to get the content_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(std::pair< const std::string, bool >& t : items)
              {
                const std::string& match_expr = t.first;
                if(check_match(record.get_name(), match_expr, true))    // true = insist on exact match
                  {
                    t.second = true;

                    // re-query the database to get a read/write version of this record
                    typename ContentDatabase::mapped_type rw_record =
                      get_rw_content_group<number, typename ContentDatabase::mapped_type::element_type::payload_type>{}.get(
                        this->repo, record.get_name(), mgr);

                    rw_record->set_lock_status(true);

                    // recommit record
                    rw_record->commit();
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


    template <typename number>
    void repository_toolkit<number>::unlock_content(const std::vector<std::string>& objects)
      {
        // make local copy of objects to process; we will tag items in this list as 'processed' as we go
        std::map<std::string, bool> items;
        for(const std::string& item : objects)
          {
            items[item] = false;
          }

        // cache content databases
        integration_content_db integration_content = this->repo.enumerate_integration_task_content();
        postintegration_content_db postintegration_content = this->repo.enumerate_postintegration_task_content();
        output_content_db output_content = this->repo.enumerate_output_task_content();

        this->unlock_content(integration_content, items);
        this->unlock_content(postintegration_content, items);
        this->unlock_content(output_content, items);

        for(const std::pair< const std::string, bool >& item : items)
          {
            if(!item.second)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_REPO_TOOLKIT_MISSING_OBJECT << " '" << item.first << "'";
                this->msg(msg.str());
              }
          }
      }


    template <typename number>
    template <typename ContentDatabase>
    void repository_toolkit<number>::unlock_content(ContentDatabase& db, std::map< std::string, bool >& items)
      {
        // lock the database by setting up a transaction
        transaction_manager mgr = this->repo.transaction_factory();

        // step through records in content database
        for(const typename ContentDatabase::value_type& item : db)
          {
            // ContentDatabase has mapped_type equal to std::unique_ptr< content_group_record<PayloadType> >
            // to get the content_group_record<PayloadType> we need the ::element_type member of std::unique_ptr<>
            const typename ContentDatabase::mapped_type::element_type& record = *item.second;

            // step through objects in match list
            for(std::pair< const std::string, bool >& t : items)
              {
                const std::string& match_expr = t.first;
                if(check_match(record.get_name(), match_expr, true))    // true = insist on exact match
                  {
                    t.second = true;

                    // re-query the database to get a read/write version of this record
                    typename ContentDatabase::mapped_type rw_record =
                      get_rw_content_group<number, typename ContentDatabase::mapped_type::element_type::payload_type>{}.get(
                        this->repo, record.get_name(), mgr);

                    rw_record->set_lock_status(false);

                    // recommit record
                    rw_record->commit();
                  }
              }
          }

        // commit all changes
        mgr.commit();
      }


  }   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_TOOLKIT_H

