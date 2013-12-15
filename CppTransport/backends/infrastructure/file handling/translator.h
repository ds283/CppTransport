//
// Created by David Seery on 09/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __translator_H_
#define __translator_H_


#include "buffer.h"
#include "output_stack.h"


// need forward reference to avoid circularity
class translation_unit;

typedef std::function<std::string(std::string&)> filter_function;

class translator
  {
  public:
    translator(translation_unit* tu);

    // translate, using the data in the supplied translation_unit, from
    // the template 'in' to the template 'out'
    // sometimes we want to supply a buffer, because we share buffers
    // across different input sources
    // otherwise, translate() creates a suitable buffer internally
    unsigned int translate(const std::string in, const std::string out, enum process_type type, filter_function* filter=nullptr);
    unsigned int translate(const std::string in, const std::string out, enum process_type type, buffer* buf, filter_function* filter=nullptr);

  protected:
    // internal API to process a file
    unsigned int process(const std::string in, const std::string out, enum process_type type, buffer* buf, filter_function* filter);

    // do the heavy lifting of actual processing
    unsigned int apply();

    // parse the header line from a template, tokenizing it into 'backend' and 'minimum version' data
    void parse_header_line(const std::string in, const std::string line, std::string& backend, double& minver);

    translation_unit* unit;
  };


#endif //__translator_H_
