//
// Created by David Seery on 25/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __instance_manager_H_
#define __instance_manager_H_


#include <assert.h>
#include <iostream>
#include <list>

#include <algorithm>
#include <stdexcept>

#include "transport/messages_en.h"


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
          }

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
        return(lhs.m_ptr == rhs.m_ptr && lhs.uid == rhs.uid);
      }


    template <typename number>
    class instance_manager
      {
      public:
        virtual ~instance_manager();

        // MODEL MANAGEMENT API

        // register and deregister models from our list
        void register_model(model<number>* m, const std::string& uid);
        void deregister_model(model<number>* m, const std::string& uid);

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

  }   // namespace transport


#endif //__instance_manager_H_
