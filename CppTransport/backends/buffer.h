//
// Created by David Seery on 05/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//



#ifndef __buffer_H_
#define __buffer_H_


#include <list>
#include <string>


typedef std::function<void(void)> buffer_closure_handler;

class buffer
  {
  public:
    buffer()                                                         { tag = --buf.end(); }

    void write_to_end              (std::string line)                { this->buf.push_back(line); }
    void write_to_tag              (std::string line)                { this->buf.insert(this->tag, line); }

    void set_tag_to_end            ()                                { this->tag = --this->buf.end(); }

    void register_closure_handler  (buffer_closure_handler handler, void* tag)  { this->closure_handlers.push_back(handler); }
    void deregister_closure_handler(buffer_closure_handler handler, void* tag)  { this->buf.remove_if([&](std::pair<buffer_closure_handler,void*> item) -> bool { return item.first() == tag; } ); }

    void emit                      (std::string file);

  protected:
    std::list<std::string>                              buf;
    std::list<std::string>::iterator                    tag;

    std::list<std::pair<buffer_closure_handler, void*>> closure_handlers;
  };


#endif //__buffer_H_
