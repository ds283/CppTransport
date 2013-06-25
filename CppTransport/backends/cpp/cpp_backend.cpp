//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <time.h>

#include "cpp_backend.h"

#define MACRO_PREFIX "$$__"
#define MACRO_PREFIX_LENGTH (4)

static const std::string macros[] =
  {
    "TOOL", "VERSION", "GUARD", "DATE", "SOURCE",
    "NAME", "AUTHOR", "TAG", "MODEL", "HEADER", "CPP"
  };

static const unsigned int macro_lengths[] =
  {
    4, 7, 5, 4, 6,
    4, 6, 3, 5, 6, 3
  };

typedef std::string (*replacement_function)(script* source, std::string hname, std::string cname, std::string source_file);

static std::string replace_tool(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_version(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_guard(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_date(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_source(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_name(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_author(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_tag(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_model(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_header(script* source, std::string hname, std::string cname, std::string source_file);
static std::string replace_cpp(script* source, std::string hname, std::string cname, std::string source_file);


static replacement_function macro_replacements[] =
  {
    replace_tool, replace_version, replace_guard, replace_date, replace_source,
    replace_name, replace_author, replace_tag, replace_model, replace_header, replace_cpp
  };

#define NUMBER_MACROS (11)


// ******************************************************************


static bool process(std::string output, std::string input, script* source, std::string hname, std::string cname, std::string source_file);


// ******************************************************************


// generate C++ output
// returns 'false' if this fails
bool cpp_backend(struct input& data, finder* path)
  {
    assert(path != NULL);

    bool rval = true;

    script* source = data.driver->get_script();

    std::string class_name = source->get_class();
    std::string header_template;
    std::string class_template;

    if(source->get_class() == "")
      {
        error(ERROR_NO_CPP_TEMPLATE);
        rval = false;
      }
    else
      {
        rval = path->fqpn(class_name + ".h", header_template);
        if(rval)
          {
            if((rval = path->fqpn(class_name + ".cpp", class_template)) == false)
              {
                std::ostringstream msg;
                msg << ERROR_MISSING_CPP_CLASS << " '" << class_name << ".cpp'";
                error(msg.str());
              }
          }
        else
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_CPP_HEADER << " '" << class_name << ".h'";
            error(msg.str());
          }
      }

    if(rval)
      {
        rval = process(data.output + ".h", header_template, source, header_template, class_template, data.name);
      }
    if(rval)
      {
        rval = process(data.output + ".cpp", class_template, source, header_template, class_template, data.name);
      }

    return(rval);
  }


// ******************************************************************


static bool process(std::string output, std::string input, script* source, std::string hname, std::string cname, std::string source_file)
  {
    bool rval = true;

    std::ofstream out;
    out.open(output.c_str());

    std::ifstream in;
    in.open(input.c_str());

    if(in.is_open())
      {
        while(in.eof() == false && in.fail() == false)
          {
            std::string line;
            std::getline(in, line);

            while(line.find(MACRO_PREFIX) != std::string::npos)
              {
                bool replace = false;
                for(int i = 0; i < NUMBER_MACROS; i++)
                  {
                    size_t pos;
                    if((pos = line.find(MACRO_PREFIX + macros[i])) != std::string::npos)
                      {
                        line.replace(pos, macro_lengths[i] + MACRO_PREFIX_LENGTH, (*(macro_replacements[i]))(source, hname, cname, source_file));
                        replace = true;
                      }
                  }
                if(replace == false)          // failed to find a macro to match
                  {
                    break;
                  }
              }

            if(out.fail() == false)
              {
                out << line << "\n";
              }
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_CPP_TEMPLATE_READ << " '" << input << "'";
        error(msg.str());
        rval = false;
      }

    if(rval == true)
      {
        rval = !out.fail();
      }

    in.close();
    out.close();

    return(rval);
  }

static std::string replace_tool(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return CPPTRANSPORT_NAME;
  }

static std::string replace_version(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return CPPTRANSPORT_VERSION;
  }

static std::string replace_guard(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return "__" + source->get_model() + "_H_";
  }

static std::string replace_date(script* source, std::string hname, std::string cname, std::string source_file)
  {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
  }

static std::string replace_source(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(source_file);
  }

static std::string replace_name(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(source->get_name());
  }

static std::string replace_author(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(source->get_author());
  }

static std::string replace_tag(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(source->get_tag());
  }

static std::string replace_model(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(source->get_model());
  }

static std::string replace_header(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(hname);
  }

static std::string replace_cpp(script* source, std::string hname, std::string cname, std::string source_file)
  {
    return(cname);
  }
