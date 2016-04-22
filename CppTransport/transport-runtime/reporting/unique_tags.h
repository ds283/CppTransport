//
// Created by David Seery on 24/03/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_UNIQUE_TAGS_H
#define CPPTRANSPORT_UNIQUE_TAGS_H


#include <map>

#include "transport-runtime/repository/repository.h"

#include "transport-runtime/defaults.h"


namespace transport
  {

    namespace reporting
      {

        template <typename number>
        class unique_tags
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            unique_tags(std::string t=CPPTRANSPORT_DEFAULT_REPORT_TAG_STEM)
              : counter(0),
                tag_stem(t)
              {
              }

            // disable copying to prevent aliasing
            unique_tags(const unique_tags<number>& obj) = delete;

            //! destructor is default
            ~unique_tags() = default;


            // INTERFACE

          public:

            //! reset all caches
            void reset();

            //! get unique ID for package
            std::string get_id(const package_record<number>& rec);

            //! get unique ID for task
            std::string get_id(const task_record<number>& rec);

            //! get unique ID for derived product
            std::string get_id(const derived_product_record<number>& rec);

            //! get unique ID for content group
            template <typename Payload>
            std::string get_id(const content_group_record<Payload>& rec);


            // INTERNAL DATA

          private:

            typedef std::map< std::string, std::string > package_id_database;
            typedef std::map< std::string, std::string > task_id_database;
            typedef std::map< std::string, std::string > derived_product_id_database;
            typedef std::map< std::string, std::string > content_group_id_database;

            //! map of packages to ids
            package_id_database package_db;

            //! map of tasks to ids
            task_id_database task_db;

            //! map of derived_products to ids
            derived_product_id_database product_db;

            //! map of content groups to ids
            content_group_id_database output_db;


            // UNIQUE COUNTER

            //! counter
            unsigned int counter;

            //! prefix used to construct tags
            const std::string tag_stem;

          };


        template <typename number>
        void unique_tags<number>::reset()
          {
            this->package_db.clear();
            this->task_db.clear();
            this->product_db.clear();
            this->output_db.clear();

            counter = 0;
          }


        template <typename number>
        std::string unique_tags<number>::get_id(const package_record<number>& rec)
          {
            package_id_database::const_iterator t = this->package_db.find(rec.get_name());

            if(t != this->package_db.end()) return t->second;

            std::ostringstream tag;
            tag << this->tag_stem << this->counter++;

            this->package_db.insert(std::make_pair(rec.get_name(), tag.str()));
            return tag.str();
          }


        template <typename number>
        std::string unique_tags<number>::get_id(const task_record<number>& rec)
          {
            task_id_database::const_iterator t = this->task_db.find(rec.get_name());

            if(t != this->task_db.end()) return t->second;

            std::ostringstream tag;
            tag << this->tag_stem << this->counter++;

            this->task_db.insert(std::make_pair(rec.get_name(), tag.str()));
            return tag.str();
          }


        template <typename number>
        std::string unique_tags<number>::get_id(const derived_product_record<number>& rec)
          {
            derived_product_id_database::const_iterator t = this->product_db.find(rec.get_name());

            if(t != this->product_db.end()) return t->second;

            std::ostringstream tag;
            tag << this->tag_stem << this->counter++;

            this->product_db.insert(std::make_pair(rec.get_name(), tag.str()));
            return tag.str();
          }


        template <typename number>
        template <typename Payload>
        std::string unique_tags<number>::get_id(const content_group_record<Payload>& rec)
          {
            content_group_id_database::const_iterator t = this->output_db.find(rec.get_name());

            if(t != this->output_db.end()) return t->second;

            std::ostringstream tag;
            tag << this->tag_stem << this->counter++;

            this->output_db.insert(std::make_pair(rec.get_name(), tag.str()));
            return tag.str();
          }


      }   // namespace reporting

  }   // namespace transport


#endif //CPPTRANSPORT_UNIQUE_TAGS_H
