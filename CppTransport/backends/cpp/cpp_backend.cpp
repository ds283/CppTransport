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
#include "macro.h"

#define MACRO_PREFIX "$$__"
#define LINE_SPLIT   "$$//"

#define SR_U_NAME    "__sr_u"
#define U2_NAME      "__u2"
#define U3_NAME      "__u3"

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

static std::string replace_sr_velocity  (struct replacement_data& data, std::vector<struct index_assignment> args);
static std::string replace_u2_tensor    (struct replacement_data& data, std::vector<struct index_assignment> args);
static std::string replace_u3_tensor    (struct replacement_data& data, std::vector<struct index_assignment> args);


static const std::string macros[] =
  {
    "TOOL", "VERSION", "GUARD", "DATE", "SOURCE",
    "NAME", "AUTHOR", "TAG", "MODEL", "HEADER",
    "NUMBER_FIELDS", "NUMBER_PARAMS",
    "FIELD_NAME_LIST", "LATEX_NAME_LIST",
    "PARAM_NAME_LIST", "PLATX_NAME_LIST"
  };


static const replacement_function macro_replacements[] =
  {
    replace_tool, replace_version, replace_guard, replace_date, replace_source,
    replace_name, replace_author, replace_tag, replace_model, replace_header,
    replace_number_fields, replace_number_params,
    replace_field_list, replace_latex_list,
    replace_param_list, replace_platx_list
  };


static const std::string field_iters[] =
  {
    "SR_VELOCITY"
  };


static const unsigned int field_iter_args[] =
  {
    1
  };


static const replacement_function_iter field_iter_replacements[] =
  {
    replace_sr_velocity
  };


static const std::string all_iters[] =
  {
    "U2_TENSOR", "U3_TENSOR"
  };


static const unsigned int all_iter_args[] =
  {
    2, 3
  };


static const replacement_function_iter all_iter_replacements[] =
  {
    replace_u2_tensor, replace_u3_tensor
  };


#define NUMBER_MACROS     (16)
#define NUMBER_FIELD_ITER (1)
#define NUMBER_ALL_ITER   (2)


// ******************************************************************


static bool process     (std::string output, std::string input, struct replacement_data& d);
static void apply_macros(struct replacement_data& d, std::string& line);


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

    if(source->get_class() == "")
      {
        error(ERROR_NO_CPP_TEMPLATE);
        rval = false;
      }
    else
      {
        if((rval = path->fqpn(class_name + ".h", header_template)) == false)
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

    struct macro_package ms(d.source->get_number_fields(), MACRO_PREFIX, LINE_SPLIT, d,
      NUMBER_MACROS, macros, macro_replacements,
      NUMBER_FIELD_ITER, field_iters, field_iter_args, field_iter_replacements,
      NUMBER_ALL_ITER, all_iters, all_iter_args, all_iter_replacements);

    if(in.is_open())
      {
        while(in.eof() == false && in.fail() == false)
          {
            std::string line;
            std::getline(in, line);

            ms.apply(line);

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

static std::string replace_sr_velocity(struct replacement_data& d, std::vector<struct index_assignment> args)
  {
    std::string rval = SR_U_NAME;

    for(int i = 0; i < args.size(); i++)
      {
        rval = rval + "_" + index_stringize(args[i]);
      }

    return(rval);
  }

static std::string replace_u2_tensor(struct replacement_data& d, std::vector<struct index_assignment> args)
  {
    std::string rval = U2_NAME;

    for(int i = 0; i < args.size(); i++)
      {
        rval = rval + "_" + index_stringize(args[i]);
      }

    return(rval);
  }

static std::string replace_u3_tensor(struct replacement_data& d, std::vector<struct index_assignment> args)
  {
    std::string rval = U3_NAME;

    for(int i = 0; i < args.size(); i++)
      {
        rval = rval + "_" + index_stringize(args[i]);
      }

    return(rval);
  }