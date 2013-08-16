//
// Created by David Seery on 24/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_INDEX_SELECTOR_H_
#define __CPP_TRANSPORT_INDEX_SELECTOR_H_

#include <array>
#include <vector>

#include <assert.h>

namespace transport
  {

    template <unsigned int indices>
    class index_selector
      {
      public:
        index_selector(unsigned int N_f, unsigned int dimension=2);

        void none   ();
        void all    ();
        void set_on (std::array<unsigned int, indices>& which);
        void set_off(std::array<unsigned int, indices>& which);
        bool is_on  (std::array<unsigned int, indices>& which);

        unsigned int get_dimension() { return(this->dimension); }
        unsigned int get_fields()    { return(this->N_fields); }

      protected:
        const unsigned int        N_fields;
        const unsigned int        dimension;

        unsigned int              size;           // total number of components
        std::vector<bool>         enabled;
        std::vector<unsigned int> displacements;
      };


    template <unsigned int indices>
    index_selector<indices>::index_selector(unsigned int N_f, unsigned int dimension)
    : N_fields(N_f)
      {
        // work out how many components this object has
        size = 1;
        for(int i = 0; i < indices; i++)
          {
            size *= dimension * N_fields;
          }
        enabled.assign(size, true);   // by default, enable all components

        displacements.resize(indices);
        unsigned int count = 1;
        for(int i = 0; i < indices; i++)
          {
            displacements[indices-i-1] = count;
            count *= dimension * this->N_fields;
          }
      }


    template <unsigned int indices>
    void index_selector<indices>::none()
      {
        this->enabled.assign(size, false);
      }


    template <unsigned int indices>
    void index_selector<indices>::all()
      {
        this->enabled.assign(size, true);
      }


    template <unsigned int indices>
    void index_selector<indices>::set_on(std::array<unsigned int, indices>& which)
      {
        unsigned int index = 0;
        for(int      i     = 0; i < indices; i++)
          {
            assert(which[i] < this->dimension * this->N_fields);  // basic sanity check: TODO: add error handling

            index += this->displacements[i] * which[i];
          }

        assert(index < this->size);
        this->enabled[index] = true;
      }


    template <unsigned int indices>
    void index_selector<indices>::set_off(std::array<unsigned int, indices>& which)
      {
        unsigned int index = 0;
        for(int      i     = 0; i < indices; i++)
          {
            assert(which[i] < this->dimension * this->N_fields);  // basic sanity check: TODO: add error handling

            index += this->displacements[i] * which[i];
          }

        assert(index < this->size);
        this->enabled[index] = false;
      }


    template <unsigned int indices>
    bool index_selector<indices>::is_on(std::array<unsigned int, indices>& which)
      {
        unsigned int index = 0;
        for(int      i     = 0; i < indices; i++)
          {
            assert(which[i] < this->dimension * this->N_fields); // basic sanity check: TODO: add error handling

            index += this->displacements[i] * which[i];
          }

        assert(index < this->size);
        return (this->enabled[index]);
      }


  }  // namespace transport

#endif // __CPP_TRANSPORT_INDEX_SELECTOR_H_
