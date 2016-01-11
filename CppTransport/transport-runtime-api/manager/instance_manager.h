//
// Created by David Seery on 25/12/2013.
// Copyright (c) 2013-2016 University of Sussex. All rights reserved.
//


#ifndef __instance_manager_H_
#define __instance_manager_H_


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


    template <typename number>
    class instance_manager
      {

      public:

        typedef std::function< model<number>*(const std::string&) > model_finder;


        // CONSTRUCTOR, DESTRUCTOR
        // ensure destructor is declared virtual so that derived types are destroyed

      public:

        //! destructor is default; cached list of shared_ptr<>s will be destroyed automatically, releasing
        //! our hold on those pointers
        virtual ~instance_manager() = default;


        // INTERFACE --MODEL MANAGEMENT API

      public:

        //! Create a model instance and register it
        template <typename Model>
        std::shared_ptr<Model> create_model();

      protected:

        //! Register an instance of a model.

        //! There should be only one registered instance of each unique uid, otherwise an exception is thrown.
        //! This function checks the version of the translator used to produce this model.
        //! It should be no later than the version of the runtime api we are running, otherwise an exception is thrown
        void register_model(std::shared_ptr< model<number> > m);


        // INTERFACE -- MODEL ACCESS

      protected:

        //! Search for a model by uid
        model<number>* find_model(const std::string& i);

      public:

        //! Construct a model_finder function for this instance manager
        model_finder model_finder_factory()
          {
            return(std::bind(&instance_manager<number>::find_model, this, std::placeholders::_1));
          }


        // INTERFACE -- WRITE DETAILS TO STREAM

      public:

        void write_models(std::ostream& stream);


        // INTERNAL DATA

      protected:

        std::list< model_instance<number> > models;

      };


    template <typename number>
    template <typename Model>
    std::shared_ptr<Model> instance_manager<number>::create_model()
      {
        std::shared_ptr<Model> model = std::make_shared<Model>();
        this->register_model(model);

        return(model);
      }


    template <typename number>
    void instance_manager<number>::register_model(std::shared_ptr< model<number> > m)
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
    model<number>* instance_manager<number>::find_model(const std::string& i)
      {
        model_instance<number> instance(i);

        typename std::list< model_instance<number> >::const_iterator t;
        if((t = std::find(this->models.begin(), this->models.end(), instance)) == this->models.end())
          {
            std::ostringstream msg;
            msg << CPPTRANSPORT_INSTANCES_MISSING << " '" << i << "'";
            throw runtime_exception(exception_type::MISSING_MODEL_INSTANCE, msg.str());
          }
        else
          {
            return((*t).get_model());
          }
      }


    template <typename number>
    void instance_manager<number>::write_models(std::ostream& stream)
      {
        unsigned int c = 0;

        for(typename std::list< model_instance<number> >::const_iterator t = this->models.begin(); t != this->models.end(); ++t)
          {
            model<number>* mdl = t->get_model();
            c++;

            stream << c << ". " << mdl->get_name() << " [" << mdl->get_author() << "]" << '\n';
            stream << "   backend = " << mdl->get_backend() << " [bg=" << mdl->get_back_stepper() << ", pert=" << mdl->get_pert_stepper() << "]" << '\n';
            stream << "   UID = " << t->get_uid() << " | built using CppTransport " << static_cast<double>(mdl->get_translator_version())/100.0 << '\n';
          }
      }


  }   // namespace transport


#endif //__instance_manager_H_
