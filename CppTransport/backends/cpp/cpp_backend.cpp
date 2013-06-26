//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <time.h>

#include "cpp_backend.h"
#include "to_printable.h"

#define MACRO_PREFIX "$$__"
#define MACRO_PREFIX_LENGTH (4)

#define UNROLL_PREFIX "!!__"
#define UNROLL_PREFIX_LENGTH (4)

struct replacement_data
  {
    script*     source;
    std::string hname;
    std::string source_file;
  };


static const std::string macros[] =
  {
    "TOOL", "VERSION", "GUARD", "DATE", "SOURCE",
    "NAME", "AUTHOR", "TAG", "MODEL", "HEADER",
    "NUMBER_FIELDS", "NUMBER_PARAMS",
    "FIELD_NAME_LIST", "LATEX_NAME_LIST",
    "PARAM_NAME_LIST", "PLATX_NAME_LIST",
    "SR_VELOCITY"
  };

static const unsigned int macro_lengths[] =
  {
    4, 7, 5, 4, 6,
    4, 6, 3, 5, 6,
    13, 13,
    15, 15,
    15, 15,
    11
  };

static const unsigned int macro_iterations[] =
  {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0,
    0, 0,
    0, 0,
    1
  };


typedef std::string (*replacement_function)(struct replacement_data& data);

static std::string replace_tool         (struct replacement_data& data);
static std::string replace_version      (struct replacement_data& data);
static std::string replace_guard        (struct replacement_data& data);
static std::string replace_date         (struct replacement_data& data);
static std::string replace_source       (struct replacement_data& data);
static std::string replace_name         (struct replacement_data& data);
static std::string replace_author       (struct replacement_data& data);
static std::string replace_tag          (struct replacement_data& data);
static std::string replace_model        (struct replacement_data& data);
static std::string replace_header       (struct replacement_data& data);
static std::string replace_number_fields(struct replacement_data& data);
static std::string replace_number_params(struct replacement_data& data);
static std::string replace_field_list   (struct replacement_data& data);
static std::string replace_latex_list   (struct replacement_data& data);
static std::string replace_param_list   (struct replacement_data& data);
static std::string replace_platx_list   (struct replacement_data& data);
static std::string replace_sr_velocity  (struct replacement_data& data);

static replacement_function macro_replacements[] =
  {
    replace_tool, replace_version, replace_guard, replace_date, replace_source,
    replace_name, replace_author, replace_tag, replace_model, replace_header,
    replace_number_fields, replace_number_params,
    replace_field_list, replace_latex_list,
    replace_param_list, replace_platx_list,
    replace_sr_velocity
  };

#define NUMBER_MACROS (17)


// ******************************************************************


static bool process(std::string output, std::string input, struct replacement_data& d);


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
        struct replacement_data d;

        d.source      = source;
        d.hname       = data.output + ".h";
        d.source_file = data.name;
        rval = process(data.output + ".h", header_template, d);
      }

    return(rval);
  }


// ******************************************************************


static bool process(std::string output, std::string input, struct replacement_data& d)
  {
    bool rval = true;

    unsigned int N_f = d.source->get_number_fields();

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
                        line.replace(pos, macro_lengths[i] + MACRO_PREFIX_LENGTH, (*(macro_replacements[i]))(d));
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


// ********************************************************************************
// REPLACEMENT RULES


static std::string replace_tool(struct replacement_data& d)
  {
    return CPPTRANSPORT_NAME;
  }

static std::string replace_version(struct replacement_data& d)
  {
    return CPPTRANSPORT_VERSION;
  }

static std::string replace_guard(struct replacement_data& d)
  {
    return "__CPP_TRANSPORT_" + d.source->get_model() + "_H_";
  }

static std::string replace_date(struct replacement_data& d)
  {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%X on %d %m %Y", &tstruct);

    return buf;
  }

static std::string replace_source(struct replacement_data& d)
  {
    return(d.source_file);
  }

static std::string replace_name(struct replacement_data& d)
  {
    return(d.source->get_name());
  }

static std::string replace_author(struct replacement_data& d)
  {
    return(d.source->get_author());
  }

static std::string replace_tag(struct replacement_data& d)
  {
    return(d.source->get_tag());
  }

static std::string replace_model(struct replacement_data& d)
  {
    return(d.source->get_model());
  }

static std::string replace_header(struct replacement_data& d)
  {
    return(d.hname);
  }

static std::string replace_number_fields(struct replacement_data& d)
  {
    std::ostringstream out;

    out << d.source->get_number_fields();

    return(out.str());
  }

static std::string replace_number_params(struct replacement_data& d)
  {
    std::ostringstream out;

    out << d.source->get_number_params();

    return(out.str());
  }

static std::string replace_field_list(struct replacement_data& d)
  {
    std::vector<std::string> list = d.source->get_field_list();
    std::ostringstream out;

    out << "{ ";

    for(int i = 0; i < list.size(); i++)
      {
        if(i > 0)
          {
            out << ", ";
          }
        out << to_printable(list[i]);
      }
    out << " }";

    return(out.str());
  }

static std::string replace_latex_list(struct replacement_data& d)
  {
    std::vector<std::string> list = d.source->get_latex_list();
    std::ostringstream out;

    out << "{ ";

    for(int i = 0; i < list.size(); i++)
      {
        if(i > 0)
          {
            out << ", ";
          }
        out << to_printable(list[i]);
      }
    out << " }";

    return(out.str());
  }

static std::string replace_param_list(struct replacement_data& d)
  {
    std::vector<std::string> list = d.source->get_param_list();
    std::ostringstream out;

    out << "{ ";

    for(int i = 0; i < list.size(); i++)
      {
        if(i > 0)
          {
            out << ", ";
          }
        out << to_printable(list[i]);
      }
    out << " }";

    return(out.str());
  }

static std::string replace_platx_list(struct replacement_data& d)
  {
    std::vector<std::string> list = d.source->get_platx_list();
    std::ostringstream out;

    out << "{ ";

    for(int i = 0; i < list.size(); i++)
      {
        if(i > 0)
          {
            out << ", ";
          }
        out << to_printable(list[i]);
      }
    out << " }";

    return(out.str());
  }

static std::string replace_sr_velocity(struct replacement_data& d)
  {
    return("");
  }
