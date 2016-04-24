//
// Created by David Seery on 20/12/2013.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#ifndef __context_H_
#define __context_H_


#include <assert.h>
#include <string>
#include <vector>


namespace transport
  {

    class context
      {

      public:

        class device
          {
          public:
            enum memory_type { bounded, unbounded };

            device(std::string n, unsigned int m, const context* p, enum memory_type t=unbounded, unsigned int w=1)
              : name(n), mem_size(m), weight(w), type(t), parent(p)
              {
              }

            const std::string& get_name() const              { return(this->name); }
            unsigned int       get_mem_size() const          { return(this->mem_size); }
            enum memory_type   get_mem_type() const          { return(this->type); }
            unsigned int       get_weight() const            { return(this->weight); }
            double             get_fractional_weight() const { return(static_cast<double>(this->weight) / parent->total_weight); }

          protected:
            std::string      name;
            unsigned int     mem_size;
            unsigned int     weight;
            enum memory_type type;

            const context*         parent;
          };

        context()
          : total_weight(0)
          {
          }

        size_t size() const { return(this->devices.size()); }

        // add a device to the context
        void add_device(std::string name, unsigned int mem_size=0, enum device::memory_type type=device::unbounded, unsigned int weight=1);

        // return fractional weighting (ie., normalized to 1.0) for the dth device
        // d must be within the current size of the context
        double fractional_weight(unsigned int d) const;

        const device& get_device(unsigned int d) const;
        const device& operator[](unsigned int d) const { return(this->get_device(d)); }

        friend class device;

      protected:

        std::vector<device> devices;
        unsigned            total_weight;

      };


    void context::add_device(std::string name, unsigned int mem_size, enum device::memory_type type, unsigned int weight)
      {
        device dev(name, mem_size, this, type, weight);

        this->devices.push_back(dev);
        this->total_weight += weight;
      }


    const context::device& context::get_device(unsigned int d) const
      {
        assert(d < this->devices.size());

        if(d < this->devices.size())
          {
            return(this->devices[d]);
          }
        else
          {
            return(this->devices[this->devices.size()-1]);
          }
      }


    double context::fractional_weight(unsigned int d) const
      {
        assert(d < this->devices.size());

        double rval = 0;

        if(d < this->devices.size())
          {
            rval = static_cast<double>(this->devices[d].get_weight()) / static_cast<double>(this->total_weight);
          }

        return(rval);
      }

  } // namespace transport


#endif //__context_H_
