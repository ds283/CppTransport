//
// Created by David Seery on 25/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __instance_manager_H_
#define __instance_manager_H_


#include <assert.h>
#include <iostream>
#include <list>

#include <functional>
#include <algorithm>
#include <stdexcept>

#include "transport/version.h"
#include "transport/exceptions.h"
#include "transport/messages.h"


namespace transport
  {

    // avoid circular references
    template <typename number> class model;

    // set up equality operator for model_instance record
    template <typename number> class model_instance;

    template <typename number>
    bool operator==(const model_instance<number>& lhs, const model_instance<number>& rhs);


    template <typename number>
    class model_instance
      {
        // CONSTRUCTOR

      public:
        model_instance(model<number>* m, const std::string& i, unsigned int v)
          : m_ptr(m), uid(i), tver(v)
          {
            assert(m != nullptr);
          }

        model_instance(const std::string& i)
          : m_ptr(nullptr), uid(i)
          {
          }

        //! Get pointer to stored model
        model<number>* get_model() const { return(this->m_ptr); }

        //! Delete stored model
        void delete_model() { delete(this->m_ptr); }

        // COMPARISON OPERATOR

      public:
        //! Equality comparison
        friend bool operator== <>(const model_instance<number>& lhs, const model_instance<number>& rhs);

        // INTERNAL DATA

      protected:
        model<number>*    m_ptr;
        const std::string uid;
        unsigned int      tver;
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
        //! Destroy an instance manager
        virtual ~instance_manager();

        // INTERFACE --MODEL MANAGEMENT API

        //! Register an instance of a model.

        //! There should be only one registered instance of each unique uid, otherwise an exception is thrown.
        //! This function checks the version of the translator used to produce this model.
        //! It should be no later than the version of the runtime api we are running, otherwise an exception is thrown
        void register_model(model<number>* m, const std::string& uid, unsigned int version);
        //! Deregister an instance of a model.
        void deregister_model(model<number>* m, const std::string& uid, unsigned int version);

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

        // INTERNAL DATA

      protected:
        std::list< model_instance<number> > models;
      };


    template <typename number>
    instance_manager<number>::~instance_manager()
      {
        // when deleted, models deregister themselves and therefore change iterators into the list.
        // to handle this, take a copy first
        std::list< model_instance<number> > models_copy = this->models;

        for(typename std::list< model_instance<number> >::iterator t = models_copy.begin(); t != models_copy.end(); t++)
          {
            (*t).delete_model();
          }
      }


    template <typename number>
    void instance_manager<number>::register_model(model<number>* m, const std::string& uid, unsigned int version)
      {
        assert(m != nullptr);

        if(version > __CPP_TRANSPORT_RUNTIME_API_VERSION)
          {
            std::ostringstream msg;
            msg << __CPP_TRANSPORT_OLD_RUNTIMEAPI_A << " (" << __CPP_TRANSPORT_RUNTIME_API_VERSION << ") "
              << __CPP_TRANSPORT_OLD_RUNTIMEAPI_B << " (" << m->get_name() << " " << version << ")";
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, msg.str());
          }

        model_instance<number> instance(m, uid, version);

        if(std::find(this->models.begin(), this->models.end(), instance) == this->models.end())
          {
            this->models.push_back(instance);
          }
        else
          {
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_INSTANCES_MULTIPLE);
          }
      }


    template <typename number>
    void instance_manager<number>::deregister_model(model<number>* m, const std::string& uid, unsigned int version)
      {
        assert(m != nullptr);

        model_instance<number> instance(m, uid, version);

        auto t = std::find(this->models.begin(), this->models.end(), instance);

        if(t != this->models.end())
          {
            this->models.erase(t);
          }
        else
          {
            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_INSTANCES_DELETE);
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
            msg << __CPP_TRANSPORT_INSTANCES_MISSING << " '" << i << "'";
            throw runtime_exception(runtime_exception::MISSING_MODEL_INSTANCE, msg.str());
          }
        else
          {
            return((*t).get_model());
          }
      }


  }   // namespace transport


#endif //__instance_manager_H_
