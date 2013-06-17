//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __lexstream_H_
#define __lexstream_H_

#include <iostream>
#include <string>
#include <deque>

#include "lexeme.h"
#include "finder.h"


class lexstream
  {
    public:
      lexstream(const std::string filename, finder* search);
      ~lexstream();

      void    reset();
      lexeme* get();
      bool    eat();
      bool    state();

      void dump(std::ostream& stream);

    private:
      std::deque<struct lexeme>           lexeme_list;

      std::deque<struct lexeme>::iterator ptr;
      bool                                ptr_valid;
  };


#endif //__lexstream_H_
