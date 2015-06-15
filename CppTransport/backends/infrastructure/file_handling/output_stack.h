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

    class inclusion
      {

      public:

		    inclusion(const std::string& i, unsigned int l, buffer& b, macro_agent& a, enum process_type t)
			    : in(i),
			      line(l),
			      buf(b),
			      agent(a),
			      type(t)
			    {
			    }

      public:

        std::string       in;
        unsigned int      line;
        buffer&           buf;
		    macro_agent&      agent;
        enum process_type type;

      };


		// CONSTRUCTOR, DESTRUCOTR

  public:

    ~output_stack();


		// INTERFACE - implements a 'filestack' interface

  public:

		// PUSH AND POP

    // we are forced to have the basic push method inherited from filestack, even though we don't want it,
    // because otherwise it makes output_stack abstract
    void              push              (const std::string name);

    // push an object to the top of the stack
    void              push              (const std::string& in, buffer& buf, macro_agent& agent, enum process_type type);

    void              pop               ();

		// HANDLE LINE NUMBERS

    void              set_line          (unsigned int line);
    unsigned int      increment_line    ();
    unsigned int      get_line          () const;

		// STRINGIZE

    std::string       write             (size_t level) const;
    std::string       write             () const;


		// INTERFACE - specific to output_stack

  public:

    buffer&           top_buffer        ();
    macro_agent&      top_macro_package ();
    enum process_type top_process_type  () const;


		// INTERNAL DATA

  protected:

    std::deque<struct inclusion> inclusions;

  };


#endif //__output_stack_H_
