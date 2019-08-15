//
// Created by David Seery on 20/12/2013.
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


#ifndef CPPTRANSPORT_CONTEXT_H
#define CPPTRANSPORT_CONTEXT_H


#include <cassert>
#include <stdexcept>

#include <string>
#include <sstream>
#include <vector>

#include "transport-runtime/localizations/messages_en.h"


namespace transport
  {

    namespace context_impl
      {

        //! a 'device' object records the capabilities of a single compute device;
        //! a collection of such devices forms a compute context
        template <typename ContextManager>
        class device
          {

            // TYPES

          public:

            enum class memory_type
              {
                bounded, unbounded
              };


            // CONSTRUCTOR, DESTRUCTOR

          public:

            //! constructor
            device(std::string n, unsigned int m, const ContextManager& p, enum memory_type t = memory_type::unbounded,
                   unsigned int w = 1)
              : name(n),
                mem_size(m),
                weight(w),
                type(t),
                parent(p)
              {
              }


            //! destructor is default
            ~device() = default;


            // INTERFACE

          public:

            //! get device name
            const std::string& get_name() const { return this->name; }


            //! get device memory capacity
            unsigned int get_mem_size() const { return this->mem_size; }


            //! get device memory type
            enum memory_type get_mem_type() const { return this->type; }


            //! get device compute weight
            unsigned int get_weight() const { return this->weight; }


            //! get device fractional weight (normalized by all devices in parent context)
            double get_fractional_weight() const
              { return static_cast<double>(this->weight) / static_cast<double>(parent.get_total_weight()); }


            // INTERNAL DATA

          protected:

            //! device name
            std::string name;

            //! device memory capacity
            unsigned int mem_size;

            //! device compute 'weight'
            //! this is used to specify the relative performance of different compute devices, so that
            //! work is allocated in proportionto their ability to execute it.
            //! Faster devices should have higher weights.
            unsigned int weight;

            //! device memory type: bounded, or unbounded?
            //! if unbounded, this can limit the number of work items that can be assigned,
            //! even if the device has uncommited notional compute capacity.
            enum memory_type type;

            //! capture reference to parent context object, here templated as a 'ContextManager'
            const ContextManager& parent;

          };

      }   // namespace context_impl


    //! A context is a collection of devices, each of which carries a weight.
    //! The context can be queried for individual devices, and can compute the fractional weight
    //! of any individual device.
    class context
      {

        // TYPES

      public:

        //! set up type for device record: this is a device<> record with context manager
        //! equal to ourselves
        using device = context_impl::device<context>;

      private:

        //! set up device_database type to express the collection of devices held internally
        using device_database = std::vector<device>;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        context()
          : total_weight(0)
          {
          }


        //! destructor is default
        ~context() = default;


        // INTERFACE

      public:

        //! get total size of this compute context, meaning the total number of participating devices
        size_t size() const { return this->devices.size(); }


        // CONTEXT MANAGEMENT

      public:

        //! add a device to the context
        void add_device(std::string name, unsigned int mem_size = 0,
                        enum device::memory_type type = device::memory_type::unbounded, unsigned int weight = 1);

        //! get device record
        const device& get_device(unsigned int d) const;


        //! get device record via [] operator overload
        const device& operator[](unsigned int d) const { return this->get_device(d); }


        //! return fractional weighting (ie., normalized to 1.0) for the dth device
        //! d must be within the current size of the context
        double fractional_weight(unsigned int d) const;


        //! get total weight of this compute context
        unsigned int get_total_weight() const { return this->total_weight; }


        // INTERNAL DATA

      protected:

        //! vector of device records describing this context
        device_database devices;

        //! total weight of this compute context
        unsigned total_weight;

      };


    void
    context::add_device(std::string name, unsigned int mem_size, enum device::memory_type type, unsigned int weight)
      {
        this->devices.emplace_back(name, mem_size, *this, type, weight);
        this->total_weight += weight;
      }


    const context::device& context::get_device(unsigned int d) const
      {
        const auto size = this->devices.size();
        
        assert(d < size);
        if(d < size) return (this->devices[d]);

        std::stringstream msg;
        msg << CPPTRANSPORT_CONTEXT_OUT_OF_RANGE << " (index=" << d << ", size=" << size << ")";
        throw std::out_of_range(msg.str());
      }


    double context::fractional_weight(unsigned int d) const
      {
        const auto size = this->devices.size();

        assert(d < size);
        if(d < size) return this->devices[d].get_fractional_weight();

        std::stringstream msg;
        msg << CPPTRANSPORT_CONTEXT_OUT_OF_RANGE << " (index=" << d << ", size=" << size << ")";
        throw std::out_of_range(msg.str());
      }

  } // namespace transport


#endif //CPPTRANSPORT_CONTEXT_H
