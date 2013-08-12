//
// Created by David Seery on 24/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_INDEX_SELECTOR_H_
#define __CPP_TRANSPORT_INDEX_SELECTOR_H_

#include <array>

#include <assert.h>

namespace transport
  {
      // in this class, the functions are all defined inline in the class declaration
      // ordinarily I would not do this, but template arguments could not be given
      // default arguments until C++11, and neither of the default compilers shipped by
      // Apple have been updated to handle this case. Changes have been made downstream,
      // so when Apple release updated tools it will be possible to revert this.

      template <unsigned int indices, unsigned int dimension=2>
      class index_selector
        {
          public:
            index_selector(unsigned int N_f)
              : N_fields(N_f)
              {
                // work out how many components this object has
                size = 1;
                for(int i = 0; i < indices; i++)
                  {
                    size *= dimension*N_fields;
                  }
                enabled.assign(size, true);   // by default, enable all components

                displacements.resize(indices);
                unsigned int count = 1;
                for(int i = 0; i < indices; i++)
                  {
                    displacements[indices-i-1] = count;
                    count *= dimension*this->N_fields;
                  }
              }

            void none() { this->enabled.assign(size, false); }
            void all()  { this->enabled.assign(size, true); }

            void set_on(std::array<unsigned int, indices>& which)
              {
                unsigned int index = 0;
                for(int i = 0; i < indices; i++)
                  {
                    assert(which[i] < dimension*this->N_fields);  // basic sanity check: TODO: add error handling

                    index += displacements[i] * which[i];
                  }

                assert(index < this->size);
                this->enabled[index] = true;
              }

            void set_off(std::array<unsigned int, indices>& which)
              {
                unsigned int index = 0;
                for(int i = 0; i < indices; i++)
                  {
                    assert(which[i] < dimension*this->N_fields);  // basic sanity check: TODO: add error handling

                    index += displacements[i] * which[i];
                  }

                assert(index < this->size);
                this->enabled[index] = false;
              }

            bool is_on(std::array<unsigned int, indices>& which)
              {
                unsigned int index = 0;
                for(int i = 0; i < indices; i++)
                  {
                    assert(which[i] < dimension*this->N_fields); // basic sanity check: TODO: add error handling

                    index += displacements[i] * which[i];
                  }

                assert(index < this->size);
                return(this->enabled[index]);
              }

          protected:
            const unsigned int        N_fields;
            unsigned int              size;
            std::vector<bool>         enabled;
            std::vector<unsigned int> displacements;
        };


  }	// namespace transport

#endif // __CPP_TRANSPORT_INDEX_SELECTOR_H_
