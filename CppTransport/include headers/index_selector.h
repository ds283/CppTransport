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
      // DATA PRODUCTS -- objects wrapping the various data products produced by each model

      // handle weirdness with friend template functions
      // see http://www.cplusplus.com/forum/general/45776/
      template <typename number>
      class background;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, background<number>& obj);

      template <typename number>
      class twopf;

      template <typename number>
      std::ostream& operator<<(std::ostream& out, twopf<number>& obj);

      template <unsigned int indices>
      class index_selector
        {
          public:
            index_selector(unsigned int N_f);

            void none     ();
            void all      ();
            void set_on   (std::array<unsigned int, indices>& which);
            void set_off  (std::array<unsigned int, indices>& which);

            bool is_on    (std::array<unsigned int, indices>& which);

          protected:
            const unsigned int        N_fields;
            unsigned int              size;
            std::vector<bool>         enabled;
            std::vector<unsigned int> displacements;
        };

      template <unsigned int indices>
      index_selector<indices>::index_selector(unsigned int N_f)
        : N_fields(N_f)
        {
          size = 1;
          for(int i = 0; i < indices; i++)
            {
              size *= 2*N_fields;
            }
          enabled.assign(size, true);   // by default, plot all components

          displacements.resize(indices);
          unsigned int count = 1;
          for(int i = 0; i < indices; i++)
            {
              displacements[indices-i-1] = count;
              count *= 2*this->N_fields;
            }
        };

      template <unsigned int indices>
      void index_selector<indices>::none()
        {
          enabled.assign(size, false);
        }

      template <unsigned int indices>
      void index_selector<indices>::all()
        {
          enabled.assign(size, true);
        }

      template <unsigned int indices>
      void index_selector<indices>::set_on(std::array<unsigned int, indices>& which)
        {
          unsigned int index = 0;
          for(int i = 0; i < indices; i++)
            {
              index += displacements[i] * which[i];
            }

          assert(index < this->size);
          this->enabled[index] = true;
        }

      template <unsigned int indices>
      void index_selector<indices>::set_off(std::array<unsigned int, indices>& which)
        {
          unsigned int index = 0;
          for(int i = 0; i < indices; i++)
            {
              index += displacements[i] * which[i];
            }

          assert(index < this->size);
          this->enabled[index] = false;
        }

      template <unsigned int indices>
      bool index_selector<indices>::is_on(std::array<unsigned int, indices>& which)
        {
          unsigned int index = 0;
          for(int i = 0; i < indices; i++)
            {
              index += displacements[i] * which[i];
            }

          assert(index < this->size);
          return(this->enabled[index]);
        }


  }	// namespace transport

#endif // __CPP_TRANSPORT_INDEX_SELECTOR_H_
