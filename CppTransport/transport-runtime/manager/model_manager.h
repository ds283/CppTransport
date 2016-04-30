//
// Created by David Seery on 25/12/2013.
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


#ifndef CPPTRANSPORT_MODEL_MANAGER_H
#define CPPTRANSPORT_MODEL_MANAGER_H


#include <assert.h>
#include <iostream>
#include <list>

#include <functional>
#include <algorithm>
#include <stdexcept>

#include <memory>

#include "transport-runtime/manager/argument_cache.h"
#include "transport-runtime/manager/environment.h"

#include "transport-runtime/manager/message_handlers.h"

#include "transport-runtime/utilities/formatter.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

#include "transport-runtime/version.h"
#include "transport-runtime/exceptions.h"
#include "transport-runtime/messages.h"


namespace transport
  {

    namespace model_manager_impl
      {

        // set up equality operator for model_instance record
        template <typename number> class model_instance;

        template <typename number>
        bool operator==(const model_instance<number>& lhs, const model_instance<number>& rhs);


        template <typename number>
        class model_instance
          {

            // CONSTRUCTOR

          public:

            model_instance(std::shared_ptr< model<number> > m, const std::string& i, unsigned int v)
            : m_ptr(m),
              uid(i),
              tver(v)
              {
                assert(m != nullptr);
                if(m != nullptr) revision = m->get_revision();
              }


            // INTERFACE

          public:

            //! Get pointer to stored model
            model<number>* get_model() const { return(this->m_ptr.get()); }

            //! Get uid of stored model
            const std::string& get_uid() const { return(this->uid); }

            //! Get revision number of stored model
            unsigned int get_revision() const { return(this->revision); }


            // COMPARISON OPERATOR

          public:

            //! Equality comparison
            friend bool operator== <>(const model_instance<number>& lhs, const model_instance<number>& rhs);


            // INTERNAL DATA

          protected:

            //! shared pointer to model object
            std::shared_ptr< model<number> > m_ptr;

            //! UID for this model
            const std::string uid;

            //! runtime API version expected by this model
            unsigned int tver;

            //! revision number for this model definition
            unsigned int revision;

          };


        template <typename number>
        bool operator==(const model_instance<number>& lhs, const model_instance<number>& rhs)
          {
            // only check uids; there would be no way to tell the difference between two different instances
            // with the same uid
            return(lhs.uid == rhs.uid);
          }


        template <typename number>
        class ModelInstanceComparator
          {

            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor captures UID for comparison
            ModelInstanceComparator(const std::string& u, unsigned int r)
              : uid(u),
                min_revision(r)
              {
              }

            //! destructor is default
            ~ModelInstanceComparator() = default;


            // INTERFACE

          public:

            bool operator()(const model_instance<number>& m)
              {
                return(m.get_uid() == this->uid && m.get_revision() >= this->min_revision);
              }


            // INTERNAL DATA

          private:

            //! UID for comparison
            const std::string& uid;

            //! minimum revision number requested
            unsigned int min_revision;

          };


      }   // namespace model_manager_impl


    // pull in model_manager_impl namespace for this block scope
    using namespace model_manager_impl;


    template <typename number>
    class model_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor caches local environment policies
        model_manager(local_environment& e, argument_cache& c);

        //! destructor is default; cached list of shared_ptr<>s will be destroyed automatically, releasing
        //! our hold on those pointers
        ~model_manager() = default;


        // INTERFACE -- MODEL MANAGEMENT API

      public:

        //! Create a model instance and register it; we return a std::shared_ptr<>
        //! because ownership is shared between ourselves (we keep ownership of this instance in the
        //! models list) and the client, who is entitled to keep their own copy of
        //! the model object available
        template <typename Model>
        std::shared_ptr<Model> create_model();

      protected:

        //! Register an instance of a model.

        //! There should be only one registered instance of each unique uid, otherwise an exception is thrown.
        //! This function checks the version of the translator used to produce this model.
        //! It should be no later than our own runtime api, otherwise an exception is thrown
        void register_model(std::shared_ptr< model<number> > m);


        // INTERFACE -- MODEL ACCESS

      public:

        //! Search for a model by uid and minimum revision number
        model<number>* operator()(const std::string& uid, unsigned int min_revision) const;


        // INTERFACE -- WRITE DETAILS TO STREAM

      public:

        void write_models(std::ostream& stream) const;


        // INTERNAL DATA

      protected:

        typedef std::list< model_instance<number> > model_db;

        //! database of registered models
        model_db models;


        // ENVIRONMENTAL POLICIES

        //! reference to local environment
        local_environment& env;

        //! reference to argument cache
        argument_cache& arg_cache;

      };


    template <typename number>
    model<number>* model_manager<number>::operator()(const std::string& uid, unsigned int min_revision) const
      {
        typename model_db::const_iterator t = std::find_if(this->models.begin(), this->models.end(), ModelInstanceComparator<number>(uid, min_revision));

        if(t == this->models.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_INSTANCES_MISSING_A << " '" << uid << "' " << CPPTRANSPORT_INSTANCES_MISSING_B << " " << min_revision;
            throw runtime_exception(exception_type::MISSING_MODEL_INSTANCE, msg.str());
          }
        else
          {
            return(t->get_model());
          }
      }


    template <typename number>
    model_manager<number>::model_manager(local_environment& e, argument_cache& c)
      : env(e),
        arg_cache(c)
      {
      }


    template <typename number>
    template <typename Model>
    std::shared_ptr<Model> model_manager<number>::create_model()
      {
        std::shared_ptr<Model> model = std::make_shared<Model>(error_handler(this->env, this->arg_cache),
                                                               warning_handler(this->env, this->arg_cache),
                                                               message_handler(this->env, this->arg_cache));

        // register the model in our database
        this->register_model(model);

        return(model);
      }


    template <typename number>
    void model_manager<number>::register_model(std::shared_ptr< model<number> > m)
      {
        assert(m);

        const std::string& uid = m->get_identity_string();
        unsigned int version = m->get_translator_version();
        unsigned int revision = m->get_revision();

        if(version > CPPTRANSPORT_RUNTIME_API_VERSION)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_OLD_RUNTIMEAPI_A << " (" << format_version(CPPTRANSPORT_RUNTIME_API_VERSION) << ") "
                << CPPTRANSPORT_OLD_RUNTIMEAPI_B << " " << uid << " " CPPTRANSPORT_OLD_RUNTIMEAPI_C << " " << version;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        typename model_db::const_iterator t = std::find_if(this->models.begin(), this->models.end(), ModelInstanceComparator<number>(uid, revision));
        if(t == this->models.end())
          {
            this->models.emplace_back(m, uid, version);
          }
        else
          {
            throw runtime_exception(exception_type::RUNTIME_ERROR, CPPTRANSPORT_INSTANCES_MULTIPLE);
          }
      }


    template <typename number>
    void model_manager<number>::write_models(std::ostream& stream) const
      {
        unsigned int c = 0;

        for(const model_instance<number>& rec : this->models)
          {
            model<number>* mdl = rec.get_model();
            c++;

            stream << c << ". " << mdl->get_name() << " [license=" << mdl->get_license() << ", revision=" << mdl->get_revision() << "]" << '\n';
            stream << "   backend = " << mdl->get_backend() << " [bg=" << mdl->get_back_stepper() << ", pert=" << mdl->get_pert_stepper() << "]" << '\n';
            stream << "   UID = " << rec.get_uid() << " | built using CppTransport " << format_version(mdl->get_translator_version()) << '\n';
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_MODEL_MANAGER_H
