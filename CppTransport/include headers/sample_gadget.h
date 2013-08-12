//
// Created by David Seery on 04/07/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __sample_gadget_H_
#define __sample_gadget_H_

#include <iostream>
#include <fstream>

#include <vector>
#include <string>

#include <math.h>

#define DEFAULT_SAMPLE_POINTS  (500)
#define DEFAULT_SAMPLE_SPACING (sample_gadget_linear)

namespace transport
  {

    enum sample_gadget_spacing
      {
        sample_gadget_linear, sample_gadget_logarithmic
      };

    template <typename number>
    class sample_gadget
      {
      public:
        sample_gadget(number a, number b, unsigned int N = DEFAULT_SAMPLE_POINTS,
                      enum sample_gadget_spacing spc = DEFAULT_SAMPLE_SPACING)
        : min(a), max(b), npoints(N), spacing(spc)
          {
            // set up axis; after creation, this cannot be changed - only resampled
            axis.resize(npoints);

            switch(spacing)
              {
                case sample_gadget_linear:
                  {
                    for(int i = 0; i < npoints; i++)
                      {
                        axis[i] = min + (double) i * (max - min) / npoints;
                      }
                  }
                break;

                case sample_gadget_logarithmic:
                  {
                    for(int i = 0; i < npoints; i++)
                      {
                        axis[i] = min * pow(max / min, (double) i / (double) npoints);
                      }
                  }
                break;

                default:
                  assert(false);
              }
          }

        void get_limits(number& a, number& b)
          {
            a = this->min;
            b = this->max;
          }

        enum sample_gadget_spacing get_spacing()
          {
            return (this->spacing);
          }

        const std::vector<number>& get_axis()
          {
            return (this->axis);
          }

      protected:
        number min;
        number max;

        unsigned int               npoints;
        enum sample_gadget_spacing spacing;

        std::vector<number> axis;
      };


// IMPLEMENTATION - SAMPLE_GADGET


  }   // namespace transport


#endif // __sample_gadget_H_
