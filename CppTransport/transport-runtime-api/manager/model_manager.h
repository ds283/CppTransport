//
// Created by David Seery on 25/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
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

#include "transport-runtime-api/version.h"
#include "transport-runtime-api/exceptions.h"
#include "transport-runtime-api/messages.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"


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
              }


            // INTERFACE

          public:

            //! Get pointer to stored model
            model<number>* get_model() const { return(this->m_ptr.get()); }

            //! Get uid of stored model
            const std::string& get_uid() const { return(this->uid); }


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
            ModelInstanceComparator(const std::string& u)
              : uid(u)
              {
              }

            //! destructor is default
            ~ModelInstanceComparator() = default;


            // INTERFACE

          public:

            bool operator()(const model_instance<number>& m)
              {
                return(m.get_uid() == this->uid);
              }


            // INTERNAL DATA

          private:

            //! UID for comparison
            const std::string& uid;

          };


      }   // namespace model_manager_impl


    // pull in model_manager_impl namespace for this block scope
    using namespace model_manager_impl;


    template <typename number>
    class model_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor is default
        model_manager() = default;

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

        //! Search for a model by uid
        model<number>* operator()(const std::string& uid) const;


        // INTERFACE -- WRITE DETAILS TO STREAM

      public:

        void write_models(std::ostream& stream) const;


        // INTERNAL DATA

      protected:

        typedef std::list< model_instance<number> > model_db;

        //! database of registered models
        model_db models;

      };


    template <typename number>
    model<number>* model_manager<number>::operator()(const std::string& uid) const
      {
        typename model_db::const_iterator t = std::find_if(this->models.begin(), this->models.end(), ModelInstanceComparator<number>(uid));

        if(t == this->models.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_INSTANCES_MISSING << " '" << uid << "'";
            throw runtime_exception(exception_type::MISSING_MODEL_INSTANCE, msg.str());
          }
        else
          {
            return(t->get_model());
          }
      }


    template <typename number>
    template <typename Model>
    std::shared_ptr<Model> model_manager<number>::create_model()
      {
        std::shared_ptr<Model> model = std::make_shared<Model>();
        this->register_model(model);

        return(model);
      }


    template <typename number>
    void model_manager<number>::register_model(std::shared_ptr< model<number> > m)
      {
        assert(m);

        const std::string& uid = m->get_identity_string();
        unsigned int version = m->get_translator_version();

        if(version > CPPTRANSPORT_RUNTIME_API_VERSION)
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_OLD_RUNTIMEAPI_A << " (" << CPPTRANSPORT_RUNTIME_API_VERSION << ") "
                << CPPTRANSPORT_OLD_RUNTIMEAPI_B << " " << uid << " " CPPTRANSPORT_OLD_RUNTIMEAPI_C << " " << version;
            throw runtime_exception(exception_type::RUNTIME_ERROR, msg.str());
          }

        typename model_db::const_iterator t = std::find_if(this->models.begin(), this->models.end(), ModelInstanceComparator<number>(uid));
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

            stream << c << ". " << mdl->get_name() << " [" << mdl->get_author() << "]" << '\n';
            stream << "   backend = " << mdl->get_backend() << " [bg=" << mdl->get_back_stepper() << ", pert=" << mdl->get_pert_stepper() << "]" << '\n';
            stream << "   UID = " << rec.get_uid() << " | built using CppTransport " << static_cast<double>(mdl->get_translator_version())/100.0 << '\n';
          }
      }


  }   // namespace transport


#endif //CPPTRANSPORT_MODEL_MANAGER_H
