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


#include <assert.h>
#include <string>
#include <vector>


namespace transport
  {
    
    // forward-declare context class
    class context;
    
    namespace context_impl
      {

        //! device record
        template <typename ContextManager>
        class device
          {

            // TYPES
            
          public:

            enum class memory_type { bounded, unbounded };
            
            
            // CONSTRUCTOR, DESTRUCTOR
            
          public:
        
            //! constructor
            device(std::string n, unsigned int m, const ContextManager& p, enum memory_type t=memory_type::unbounded, unsigned int w=1)
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
    
            //! device compute weight
            unsigned int weight;
    
            //! device memory type
            enum memory_type type;
    
            //! capture reference to parent context object
            const ContextManager& parent;

          };
        
      }   // namespace context_impl

    class context
      {
        
        // TYPES
        
      public:
        
        //! set up type for device record
        using device = context_impl::device<context>;
        
        
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

        //! get total size of this compute context
        size_t size() const { return this->devices.size(); }

        
        // CONTEXT MANAGEMENT
        
      public:
        
        // add a device to the context
        void add_device(std::string name, unsigned int mem_size=0,
                        enum device::memory_type type=device::memory_type::unbounded, unsigned int weight=1);
    
        //! get device record
        const device& get_device(unsigned int d) const;
        
        //! get device record via [] overliad
        const device& operator[](unsigned int d) const { return this->get_device(d); }
        
        //! return fractional weighting (ie., normalized to 1.0) for the dth device
        //! d must be within the current size of the context
        double fractional_weight(unsigned int d) const;
        
        //! get total weight of this compute context
        unsigned int get_total_weight() const { return this->total_weight; }

        
        // INTERNAL DATA
        
      protected:

        //! vector of device records describing this context
        std::vector<device> devices;
        
        //! total weight of this compute context
        unsigned total_weight;

      };


    void context::add_device(std::string name, unsigned int mem_size, enum device::memory_type type, unsigned int weight)
      {
        this->devices.emplace_back(name, mem_size, *this, type, weight);
        this->total_weight += weight;
      }


    const context::device& context::get_device(unsigned int d) const
      {
        assert(d < this->devices.size());

        if(d < this->devices.size()) return(this->devices[d]);

        return(this->devices[this->devices.size()-1]);
      }


    double context::fractional_weight(unsigned int d) const
      {
        assert(d < this->devices.size());

        if(d < this->devices.size())
          return this->devices[d].get_fractional_weight();

        return(0.0);
      }

  } // namespace transport


#endif //CPPTRANSPORT_CONTEXT_H
