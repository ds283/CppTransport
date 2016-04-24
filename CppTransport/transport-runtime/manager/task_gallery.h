//
// Created by David Seery on 07/03/2016.
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

#ifndef CPPTRANSPORT_TASK_GALLERY_H
#define CPPTRANSPORT_TASK_GALLERY_H


#include <list>
#include <algorithm>


namespace transport
  {

    namespace task_gallery_impl
      {

        template <typename number>
        class abstract_generator_record
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor is default
            abstract_generator_record() = default;

            //! destructor is default
            virtual ~abstract_generator_record() = default;


            // INTERFACE

          public:

            //! test whether this generator can produce a named task
            virtual void commit(repository<number>& repo) = 0;

          };

        template <typename number, typename GeneratorObject>
        class generator_record: public abstract_generator_record<number>
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            generator_record(GeneratorObject obj)
              : object(obj)
              {
              }

            //! destructor is default
            virtual ~generator_record() = default;


            // INTERFACE

          public:

            //! test whether this generator can produce a named task
            void commit(repository<number>& repo) override;


            // INTERNAL DATA

          private:

            //! store local copy of generator
            GeneratorObject object;

          };


        template <typename number, typename GeneratorObject>
        void generator_record<number, GeneratorObject>::commit(repository<number>& repo)
          {
            this->object(repo);
          }

      }

    // pull in task_gallery_impl for this translation unit
    using namespace task_gallery_impl;

    template <typename number>
    class task_gallery
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        task_gallery() = default;

        //! destructor is default
        ~task_gallery() = default;


        // INTERFACE

      public:

        //! add a gallery generator
        template <typename GeneratorObject>
        void add_generator(GeneratorObject obj);

        //! commit tasks in the gallery
        void commit(repository<number>& repo);


        // INTERNAL DATA

      private:

        //! list of declared generators
        typedef std::list< std::unique_ptr< abstract_generator_record<number> > > generator_db;
        generator_db db;

      };


    template <typename number>
    template <typename GeneratorObject>
    void task_gallery<number>::add_generator(GeneratorObject obj)
      {
        this->db.emplace_back(std::make_unique< generator_record<number, GeneratorObject> >(obj));
      }


    template <typename number>
    void task_gallery<number>::commit(repository<number>& repo)
      {
        for(std::unique_ptr< abstract_generator_record<number> >& rec : this->db)
          {
            rec->commit(repo);
          }
      }

  }


#endif //CPPTRANSPORT_TASK_GALLERY_H
