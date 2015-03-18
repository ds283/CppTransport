//
// Created by David Seery on 10/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//


#ifndef __output_stack_H_
#define __output_stack_H_


#include <string>
#include <deque>


#include "filestack.h"
#include "buffer.h"


// forward reference to avoid circularity
class macro_agent;
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
        macro_agent*    ms;
        package_group*    package;
        enum process_type type;
      };

    ~output_stack();

    // we are forced to have the basic push method inherited from filestack, even though we don't want it,
    // because otherwise it makes output_stack abstract
    void              push              (const std::string name);

    // push an object to the top of the stack
    void              push              (const std::string out, const std::string in, buffer* buf, enum process_type type);

    // really, we want the macro_agent* and package_group* information as part of the push() method above,
    // but constructors for the replacement_package_data objects may depend on there being objects
    // on the stack - eg. temporary_pool, which uses this mechanism to claim buffer emit notifications.
    // so, allow this data to be updated afterwards
    void              push_top_data     (macro_agent* ms, package_group* pkg);

    void              set_line          (unsigned int line);
    unsigned int      increment_line    ();
    unsigned int      get_line          () const;

    void              pop               ();

    std::string       write             (size_t level) const;
    std::string       write             () const;

    buffer*           top_buffer        () const;
    macro_agent       *    top_macro_package () const;
    enum process_type top_process_type  () const;

  protected:
    std::deque<struct inclusion> inclusions;
  };


#endif //__output_stack_H_
