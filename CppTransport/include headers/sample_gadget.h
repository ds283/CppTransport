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

#define DEFAULT_SAMPLE_POINTS (500)


template <typename number>
class sample_gadget
  {
    public:
    sample_gadget(number a, number b) : min(a), max(b), npoints(DEFAULT_SAMPLE_POINTS) {}
    sample_gadget(number a, number b, unsigned int N) : min(a), max(b), npoints(N)       {}

    void set_limits(number a, number b)   { assert(a <= b); this->min = (a <= b ? a : b); this->max = (b >= a ? b : a); }
    void get_limits(number& a, number& b) { a = this->min; b = this->max; }

    std::vector<number> linear_axis();
    std::vector<number> logarithmic_axis();

    std::vector< std::vector<number> > resample(const std::vector< std::vector<number> >& sample);

    protected:
    number       min;
    number       max;
    unsigned int npoints;
  };


// IMPLEMENTATION - SAMPLE_GADGET


template <typename number>
std::vector<number> sample_gadget<number>::linear_axis()
  {
    std::vector<number> axis(this->npoints);   // the axis should contain npoints points

    number step = (this->max - this->min) / this->npoints;

    for(int i = 0; i < this->npoints; i++)
      {
        axis[i] = this->min + i*step;
      }

    return(axis);
  }


template <typename number>
std::vector<number> sample_gadget<number>::logarithmic_axis()
  {
    std::vector<number> axis(this->npoints);

    for(int i = 0; i < this->npoints; i++)
      {
        axis[i] = this->min * pow(this->max/this->min, (double)i/(double)this->npoints);
      }

    return(axis);
  }


#endif // __sample_gadget_H_
