//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//



#ifndef __buffer_H_
#define __buffer_H_


#include <list>
#include <deque>
#include <string>
#include <functional>


typedef std::function<void(void)> buffer_flush_handler;

class buffer
  {
  public:

    struct delimiter
      {
        std::string left;
        std::string right;
      };

    buffer();

    void write_to_end              (std::string line);
    void write_to_tag              (std::string line);

    void set_tag_to_end            ();

    void register_closure_handler  (buffer_flush_handler handler, void* tag);
    void deregister_closure_handler(buffer_flush_handler handler, void* tag);

    void emit                      (std::string file);

    void flush                     ();

    void push_delimiter            (std::string left, std::string right);
    void pop_delimiter             ();

    void push_skip_blank           (bool skip);
    void pop_skip_blank            ();

  protected:

    void delimit_line              (std::string& line);
    void write                     (std::string& line, std::list<std::string>::iterator insertion_point);

    std::list<std::string>                             buf;
    std::list<std::string>::iterator                   tag;       // position of tagged location within the output stream

    std::list<std::pair<buffer_flush_handler, void*> > flush_handlers;

    std::deque<struct delimiter>                       delimiters;
    std::deque<bool>                                   skips;
  };


#endif //__buffer_H_
