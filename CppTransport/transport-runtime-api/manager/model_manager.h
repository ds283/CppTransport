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

            model_instance(const std::string& i)
              : m_ptr(nullptr), uid(i)
              {
              }

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

            std::shared_ptr< model<number> > m_ptr;
            const std::string                uid;
            unsigned int                     tver;

          };


        template <typename number>
        bool operator==(const model_instance<number>& lhs, const model_instance<number>& rhs)
          {
            // only check uids; there would be no way to tell the difference between two different instances
            // with the same uid
            return(lhs.uid == rhs.uid);
          }

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

        //! Create a model instance and register it
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
        model<number>* operator()(const std::string& i) const;


        // INTERFACE -- WRITE DETAILS TO STREAM

      public:

        void write_models(std::ostream& stream) const;


        // INTERNAL DATA

      protected:

        //! list of model_instance records
        std::list< model_instance<number> > models;

      };


    template <typename number>
    class model_finder
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        model_finder(model_manager<number>& m)
          : mgr(m)
          {
          }

        //! destructor is default
        ~model_finder() = default;


        // OPERATOR() TO FIND MODEL
        model<number>* operator()(const std::string& name) const;


        // INTERNAL DATA

      private:

        //! reference to instance manager object
        model_manager<number>& mgr;

      };


    template <typename number>
    model<number>* model_manager<number>::operator()(const std::string& name) const
      {
        model_instance<number> instance(name);

        typename std::list< model_instance<number> >::const_iterator t;
        if((t = std::find(this->models.begin(), this->models.end(), instance)) == this->models.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_INSTANCES_MISSING << " '" << name << "'";
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
    void model_manager<number>::register_model(std::shared_ptr<model < number> > m)
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

        model_instance<number> instance(m, uid, version);

        if(std::find(this->models.begin(), this->models.end(), instance) == this->models.end())
          {
            this->models.push_back(instance);
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
