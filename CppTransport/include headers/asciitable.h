//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_ASCIITABLE_H_
#define __CPP_TRANSPORT_ASCIITABLE_H_

#include <iostream>
#include <iomanip>
#include <assert.h>

#include <vector>


#define DEFAULT_PRECISION (12)

namespace transport
  {

      template <typename number>
      class asciitable
        {
          public:
            asciitable(std::ostream& s) : stream(s), precision(DEFAULT_PRECISION) {}

          void write(std::string x_name, const std::vector<std::string>& columns,
            const std::vector<number>& xs, const std::vector< std::vector<number> >& ys);

          void set_precision(unsigned int p);

          protected:
            std::ostream& stream;
            unsigned int  precision;
        };


      // IMPLEMENTATION -- CLASS asciitable


      template <typename number>
      void asciitable<number>::write(std::string x_name,
        const std::vector<std::string>& columns, const std::vector<number>& xs, const std::vector< std::vector<number> >& ys)
        {
          size_t max = this->precision+1;

          assert(xs.size() == ys.size());

          for(int i = 0; i < columns.size(); i++)
            {
              size_t len;
              if((len = columns[i].size()) > max)
                {
                  max = len;
                }
            }
          max++;

          this->stream << std::right << std::setw(max) << x_name;
          for(int i = 0; i < columns.size(); i++)
            {
              this->stream << std::right << std::setw(max) << columns[i];
            }
          this->stream << std::endl;

          for(int i = 0; i < ys.size(); i++)
            {
              assert(columns.size() == ys[i].size());

              this->stream << " " << std::right << std::setw(max-1) << xs[i];

              for(int j = 0; j < ys[i].size(); j++)
                {
                  this->stream << " " << std::right << std::setw(max-1) << (ys[i])[j];
                }
              this->stream << std::endl;
            }
        }


      template <typename number>
      void asciitable<number>::set_precision(unsigned int p)
        {
          this->precision = p;
        }


  }   // namespace transport

#endif //__CPP_TRANSPORT_ASCIITABLE_H_
