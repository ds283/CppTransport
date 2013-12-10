//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __output_stack_H_
#define __output_stack_H_


#include <string>
#include <deque>


#include "filestack.h"
#include "buffer.h"


// forward reference to avoid circularity
class macro_package;
class package_group;


enum process_type { process_core, process_implementation };


class output_stack: public filestack_derivation_helper<output_stack>
  {
  public:

    struct inclusion
      {
        std::string       out;    // name of file being *written*
        std::string       in;     // name of file being read from
        unsigned int      line;
        buffer*           buf;
        macro_package*    ms;
        package_group*    package;
        enum process_type type;
      };

    ~output_stack();

    // explicitly delete inherited method, which requires only a name: we also want a buffer and a macro_package
    void              push              (const std::string name);
    void              push              (const std::string out, const std::string in, buffer* buf, macro_package* ms, package_group* pkg,
                                         enum process_type type);

    void              set_line          (unsigned int line);
    unsigned int      increment_line    ();
    unsigned int      get_line          () const;

    void              pop               ();

    std::string       write             (unsigned int level) const;
    std::string       write             () const;

    buffer*           top_buffer        () const;
    macro_package*    top_macro_package () const;
    enum process_type top_process_type  () const;

  protected:
    std::deque<struct inclusion> inclusions;
  };


#endif //__output_stack_H_
