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

#include "transport/exceptions.h"
#include "transport/messages_en.h"
#include "transport/models/model.h"


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
        model_instance(model<number>* m, const std::string& i)
          : m_ptr(m), uid(i)
          {
            assert(m != nullptr);
          }

        model_instance(const std::string& i)
          : m_ptr(nullptr), uid(i)
          {
          }

        // GET MODEL POINTER
        model<number>* get_model() const { return(this->m_ptr); }

        // COMPARISON OPERATOR

      public:
        friend bool operator== <>(const model_instance<number>& lhs, const model_instance<number>& rhs);

        // INTERNAL DATA

      protected:
        model<number>*    m_ptr;
        const std::string uid;
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
        virtual ~instance_manager();

        // INTERFACE --MODEL MANAGEMENT API

        // register and deregister models from our list
        void register_model(model<number>* m, const std::string& uid);
        void deregister_model(model<number>* m, const std::string& uid);

        // INTERFACE -- MODEL ACCESS

      protected:
        //! Search for a model by uid
        model<number>* find_model(const std::string& i);

      public:
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
      }


    template <typename number>
    void instance_manager<number>::register_model(model<number>* m, const std::string& uid)
      {
        assert(m != nullptr);

        model_instance<number> instance(m, uid);

        if(std::find(this->models.begin(), this->models.end(), instance) == this->models.end())
          {
            this->models.push_back(instance);
          }
        else
          {
            throw std::logic_error(__CPP_TRANSPORT_INSTANCES_MULTIPLE);
          }
      }


    template <typename number>
    void instance_manager<number>::deregister_model(model<number>* m, const std::string& uid)
      {
        assert(m != nullptr);

        model_instance<number> instance(m, uid);

        auto t = std::find(this->models.begin(), this->models.end(), instance);

        if(t != this->models.end())
          {
            this->models.erase(t);
          }
        else
          {
            throw std::logic_error(__CPP_TRANSPORT_INSTANCES_DELETE);
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
