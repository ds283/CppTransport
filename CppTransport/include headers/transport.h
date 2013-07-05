//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_TRANSPORT_H_
#define __CPP_TRANSPORT_TRANSPORT_H_


inline std::string to_printable(const std::string& input)
  {
    std::string result = "";

    std::back_insert_iterator<std::string> out = std::back_inserter(result);

    for(std::string::const_iterator i = input.begin(); i != input.end(); i++)
      {
        unsigned char c = *i;
        if(' ' <= c and c <= '~' and c != '\\' and c != '"')
          {
            *out++ = c;
          }
        else
          {
            *out++ = '\\';
            switch(c)
              {
                case '"':
                  *out++ = '"';
                break;
                case '\\':
                  *out++ = '\\';
                break;
                case '\t':
                  *out++ = 't';
                break;
                case '\r':
                  *out++ = 'r';
                break;
                case '\n':
                  *out++ = 'n';
                break;
                default:
                  char const *const hexdig = "0123456789ABCDEF";
                *out++ = 'x';
                *out++ = hexdig[c >> 4];
                *out++ = hexdig[c & 0xF];
              }
          }
      }

    return (result);
  }

#include "messages_en.h"

#include "plot_maker.h"
#include "python_plot_maker.h"

#include "data_products.h"
#include "model_classes.h"


#endif // __CPP_TRANSPORT_TRANSPORT_H_