//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <time.h>

#include "core.h"
#include "cpp_backend.h"
#include "to_printable.h"
#include "macro.h"

#define MACRO_PREFIX "$$__"
#define LINE_SPLIT   "$$//"

#define SR_U_NAME    "__sr_u"
#define U1_NAME      "__u1"
#define U2_NAME      "__u2"
#define U3_NAME      "__u3"

#define MAX_INDEX    (26)

static std::string replace_tool         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_version      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_guard        (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_date         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_source       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_name         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_author       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_tag          (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_model        (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_header       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_number_fields(struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_number_params(struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_field_list   (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_latex_list   (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_param_list   (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_platx_list   (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_state_list   (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_parameters   (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_fields       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_all          (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_V            (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_set_sr_u     (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_set_u1       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_set_u2       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_set_u3       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_abs_err    (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_rel_err    (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_step       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_stepper    (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_abs_err    (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_rel_err    (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_step       (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_stepper    (struct replacement_data& data, const std::vector<std::string>& args);


static std::string replace_sr_velocity  (struct replacement_data& data, const std::vector<std::string>& args, std::vector<struct index_assignment> indices);
static std::string replace_u1_tensor    (struct replacement_data& data, const std::vector<std::string>& args, std::vector<struct index_assignment> indices);
static std::string replace_u2_tensor    (struct replacement_data& data, const std::vector<std::string>& args, std::vector<struct index_assignment> indices);
static std::string replace_u3_tensor    (struct replacement_data& data, const std::vector<std::string>& args, std::vector<struct index_assignment> indices);


static const std::string simple_macros[] =
  {
    "TOOL", "VERSION", "GUARD", "DATE", "SOURCE",
    "NAME", "AUTHOR", "TAG", "MODEL", "HEADER",
    "NUMBER_FIELDS", "NUMBER_PARAMS",
    "FIELD_NAME_LIST", "LATEX_NAME_LIST",
    "PARAM_NAME_LIST", "PLATX_NAME_LIST",
    "STATE_NAME_LIST",
    "SET_PARAMETERS", "SET_FIELDS", "SET_ALL", "V",
    "SET_SR_VELOCITY",
    "SET_U1_TENSOR", "SET_U2_TENSOR", "SET_U3_TENSOR",
    "BACKG_ABS_ERR", "BACKG_REL_ERR", "BACKG_STEP_SIZE", "BACKG_STEPPER",
    "PERT_ABS_ERR", "PERT_REL_ERR", "PERT_STEP_SIZE", "PERT_STEPPER"
  };

static const replacement_function_simple simple_macro_replacements[] =
  {
    replace_tool, replace_version, replace_guard, replace_date, replace_source,
    replace_name, replace_author, replace_tag, replace_model, replace_header,
    replace_number_fields, replace_number_params,
    replace_field_list, replace_latex_list,
    replace_param_list, replace_platx_list,
    replace_state_list,
    replace_parameters, replace_fields, replace_all, replace_V,
    replace_set_sr_u,
    replace_set_u1, replace_set_u2, replace_set_u3,
    replace_b_abs_err, replace_b_rel_err, replace_b_step, replace_b_stepper,
    replace_p_abs_err, replace_p_rel_err, replace_p_step, replace_p_stepper
  };

static const unsigned int simple_macro_args[] =
  {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0,
    0, 0,
    0, 0,
    0,
    2, 2, 2, 0,
    1,
    1, 1, 1,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

static const std::string index_macros[] =
  {
    "SR_VELOCITY", "U1_TENSOR", "U2_TENSOR", "U3_TENSOR"
  };

static const unsigned int index_macro_indices[] =
  {
    1, 1, 2, 3
  };

static const unsigned int index_macro_ranges[] =
  {
    1, 2, 2, 2
  };

static const replacement_function_index index_macro_replacements[] =
  {
    replace_sr_velocity, replace_u1_tensor, replace_u2_tensor, replace_u3_tensor
  };

static const unsigned int index_macro_args[] =
  {
    0, 0, 0, 0
  };


#define NUMBER_SIMPLE_MACROS (33)
#define NUMBER_INDEX_MACROS  (4)


// ******************************************************************


static bool                                 process           (struct replacement_data& d);

static std::vector<index_abstract>          make_field_indices(struct replacement_data& data);
static std::vector<index_abstract>          make_u_indices    (struct replacement_data& data, unsigned int num);

// ******************************************************************


// generate C++ output
// returns 'false' if this fails
bool cpp_backend(struct input& data, finder* path)
  {
    assert(path != NULL);

    bool rval = true;

    script* source = data.driver->get_script();

    std::string class_name = source->get_class();
    std::string h_template;

    if(source->get_class() == "")
      {
        error(ERROR_NO_CPP_TEMPLATE);
        rval = false;
      }
    else
      {
        if((rval = path->fqpn(class_name + ".h", h_template)) == false)
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_CPP_HEADER << " '" << class_name << ".h'";
            error(msg.str());
          }
      }

    if(rval)
      {
        struct replacement_data d;

        // set up replacement data, including manufacture of a u_tensor_factory
        d.source        = source;
        d.source_file   = data.name;
        d.u_factory     = make_u_tensor_factory(source);

        d.output_file   = data.output + ".h";
        d.template_file = h_template;

        rval = process(d);

        delete d.u_factory;
      }

    return(rval);
  }


// ******************************************************************


static bool process(struct replacement_data& d)
  {
    bool rval = true;

    unsigned int current_line = 1;
    std::deque<struct inclusion> path;

    std::ofstream out;
    out.open(d.output_file.c_str());
    if(out.fail())
      {
        std::ostringstream msg;
        msg << ERROR_BACKEND_OUTPUT << " '" << d.output_file << "'";
        error(msg.str());
      }

    std::ifstream in;
    in.open(d.template_file.c_str());

    // set up a deque of inclusions for error reporting purposes
    struct inclusion inc;
    inc.line = 0;               // line number is irrelevant; we just set it to zero
    inc.name = d.template_file;
    path.push_back(inc);

    struct macro_package ms(d.source->get_number_fields(), MACRO_PREFIX, LINE_SPLIT, d,
      NUMBER_SIMPLE_MACROS, simple_macros, simple_macro_args, simple_macro_replacements,
      NUMBER_INDEX_MACROS, index_macros, index_macro_indices, index_macro_ranges, index_macro_args, index_macro_replacements);

    if(in.is_open())
      {
        while(in.eof() == false && in.fail() == false)
          {
            std::string line;
            std::getline(in, line);

            ms.apply(line, current_line, path);

            if(out.fail() == false)
              {
                out << line << std::endl;
              }

            current_line++;
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_CPP_TEMPLATE_READ << " '" << d.template_file << "'";
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


// ******************************************************************


static std::vector<index_abstract> make_field_indices(struct replacement_data& d)
  {
    assert(d.source != NULL);

    std::vector<struct index_abstract> rval;

    struct index_abstract index;

    index.label   = 'a';
    index.assign  = true;
    index.range   = 1;      // ranges only over fields, not fields + momenta

    rval.push_back(index);

    return(rval);
  }


static std::vector<index_abstract> make_u_indices(struct replacement_data& data, unsigned int num)
  {
    assert(data.source != NULL);

    std::vector<struct index_abstract> rval;

    if(num > MAX_INDEX)
      {
        error(ERROR_INDEX_OUT_OF_RANGE);
        num = MAX_INDEX;
      }

    for(int i = 0; i < num; i++)
      {
        struct index_abstract index;

        index.label   = 'A' + i;
        index.assign  = true;
        index.range   = 2;

        rval.push_back(index);
      }

    return(rval);
  }


// ********************************************************************************
// REPLACEMENT RULES


static std::string replace_tool(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return CPPTRANSPORT_NAME;
  }

static std::string replace_version(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return CPPTRANSPORT_VERSION;
  }

static std::string replace_guard(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return "__CPP_TRANSPORT_" + d.source->get_model() + "_H_";
  }

static std::string replace_date(struct replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_source(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source_file);
  }

static std::string replace_name(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_name());
  }

static std::string replace_author(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_author());
  }

static std::string replace_tag(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_tag());
  }

static std::string replace_model(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_model());
  }

static std::string replace_header(struct replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.output_file);
  }

static std::string replace_number_fields(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    out << d.source->get_number_fields();

    return(out.str());
  }

static std::string replace_number_params(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    out << d.source->get_number_params();

    return(out.str());
  }

static std::string replace_field_list(struct replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_latex_list(struct replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_param_list(struct replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_platx_list(struct replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_state_list(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::symbol> f_list = d.source->get_field_symbols();
    std::vector<GiNaC::symbol> d_list = d.source->get_deriv_symbols();

    std::ostringstream out;

    out << "{ ";

    for(int i = 0; i < f_list.size(); i++)
      {
        if(i > 0)
          {
            out << ", ";
          }
        out << to_printable(f_list[i].get_name());
      }
    for(int i = 0; i < d_list.size(); i++)
      {
        out << ", " << to_printable(d_list[i].get_name());
      }

    out << " }";

    return(out.str());
  }

static std::string replace_parameters(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<std::string> list = d.source->get_param_list();
    std::ostringstream out;

    std::string type_name  = "number";
    std::string param_name = "parameters";
    if(args.size() >= 1)
      {
        type_name = args[0];
      }
    if(args.size() >= 2)
      {
        param_name = args[1];
      }
    for(int i = 0; i < list.size(); i++)
      {
        if(i > 0)
          {
            out << std::endl;
          }
        out << type_name << " " << list[i] << " = " << param_name << "[" << i << "];";
      }

    out << std::endl << type_name << " __Mp = this->M_Planck;";

    return(out.str());
  }

static std::string replace_fields(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::symbol> list = d.source->get_field_symbols();
    std::ostringstream out;

    std::string type_name      = "number";
    std::string container_name = "fields";
    if(args.size() >= 1)
      {
        type_name = args[0];
      }
    if(args.size() >= 2)
      {
        container_name = args[1];
      }

    for(int i = 0; i < list.size(); i++)
      {
        if(i > 0)
          {
            out << std::endl;
          }
        out << GiNaC::csrc << type_name << " " << list[i] << " = " << container_name << "[" << i << "];";
      }

    return(out.str());
  }

static std::string replace_all(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::symbol> f_list = d.source->get_field_symbols();
    std::vector<GiNaC::symbol> d_list = d.source->get_deriv_symbols();
    std::ostringstream out;

    std::string type_name      = "number";
    std::string container_name = "fields";

    if(args.size() >= 1)
      {
        type_name = args[0];
      }
    if(args.size() >= 2)
      {
        container_name = args[1];
      }

    for(int i = 0; i < f_list.size(); i++)
      {
        if(i > 0)
          {
            out << std::endl;
          }
        out << GiNaC::csrc << type_name << " " << f_list[i] << " = " << container_name << "[" << i << "];";
      }

    for(int i = 0; i < d_list.size(); i++)
      {
        out << std::endl;
        out << GiNaC::csrc << type_name << " " << d_list[i] << " = " << container_name << "[" << f_list.size() + i << "];";
      }

    return(out.str());
  }

static std::string replace_V(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::string rval;
    GiNaC::ex potential = d.source->get_potential();

    std::ostringstream out;
    out << GiNaC::csrc << potential;
    rval = out.str();

    return(rval);
  }

static std::string replace_set_sr_u(struct replacement_data& data, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    std::string type_name = "number";

    if(args.size() >= 1)
      {
        type_name = args[0];
      }

    // set up an assignment over a field index
    std::vector<struct index_abstract> field_indices = make_field_indices(data);

    assignment_package assigner(data.source->get_number_fields());
    std::vector< std::vector<struct index_assignment> > field_assignment = assigner.assign(field_indices);

    std::vector<GiNaC::ex> sr_velocity = data.u_factory->compute_sr_u();

    // now, loop over this assignment, generating the appropriate SR expressions as we go
    for(int i = 0; i < field_assignment.size(); i++)
      {
        if(i > 0)
          {
            out << std::endl;
          }

        out << type_name << " " << SR_U_NAME;
        for(int j = 0; j < field_assignment[i].size(); j++)
          {
            out << "_" << index_stringize((field_assignment[i])[j]);
          }

        out << " = " << GiNaC::csrc << sr_velocity[i] << " ;";
      }

    return(out.str());
  }

static std::string replace_set_u1(struct replacement_data& data, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    std::string type_name = "number";

    if(args.size() >= 1)
      {
        type_name = args[0];
      }

    // set up an assignment over a single phase space index
    std::vector<struct index_abstract> indices                           = make_u_indices(data, 1);

    assignment_package assigner(data.source->get_number_fields());
    std::vector< std::vector<struct index_assignment> > index_assignment = assigner.assign(indices);

    std::vector<GiNaC::ex> u1 = data.u_factory->compute_u1();

    // now, loop over this assignment
    for(int i = 0; i < index_assignment.size(); i++)
      {
        if(i > 0)
          {
            out << std::endl;
          }

        out << type_name << " " << U1_NAME;
        for(int j = 0; j < index_assignment[i].size(); j++)
          {
            out << "_" << index_stringize((index_assignment[i])[j]);
          }

        out << " = " << GiNaC::csrc << u1[i] << " ;";
      }

    return(out.str());
  }

static std::string replace_set_u2(struct replacement_data& data, const std::vector<std::string>& args)
  {
    std::string rval = "";

    return(rval);
  }

static std::string replace_set_u3(struct replacement_data& data, const std::vector<std::string>& args)
  {
    std::string rval = "";

    return(rval);
  }

static std::string replace_b_abs_err(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    std::ostringstream out;
    out << s.abserr;

    return(out.str());
  }

static std::string replace_b_rel_err(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    std::ostringstream out;
    out << s.relerr;

    return(out.str());
  }

static std::string replace_b_step(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    std::ostringstream out;
    out << s.stepsize;

    return(out.str());
  }

static std::string replace_b_stepper(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    return(s.name);
  }

static std::string replace_p_abs_err(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    std::ostringstream out;
    out << s.abserr;

    return(out.str());
  }

static std::string replace_p_rel_err(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    std::ostringstream out;
    out << s.relerr;

    return(out.str());
  }

static std::string replace_p_step(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    std::ostringstream out;
    out << s.stepsize;

    return(out.str());
  }

static std::string replace_p_stepper(struct replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    return(s.name);
  }
// ******************************************************************


static std::string replace_sr_velocity(struct replacement_data& d, const std::vector<std::string>& args, std::vector<struct index_assignment> indices)
  {
    std::string rval = SR_U_NAME;

    for(int i = 0; i < indices.size(); i++)
      {
        rval = rval + "_" + index_stringize(indices[i]);
      }

    return(rval);
  }

static std::string replace_u1_tensor(struct replacement_data& d, const std::vector<std::string>& args, std::vector<struct index_assignment> indices)
  {
    std::string rval = U1_NAME;

    assert(indices.size() == 1);

    for(int i = 0; i < indices.size(); i++)
      {
        rval = rval + "_" + index_stringize(indices[i]);
      }

    return(rval);
  }

static std::string replace_u2_tensor(struct replacement_data& d, const std::vector<std::string>& args, std::vector<struct index_assignment> indices)
  {
    std::string rval = U2_NAME;

    assert(indices.size() == 2);

    for(int i = 0; i < indices.size(); i++)
      {
        rval = rval + "_" + index_stringize(indices[i]);
      }

    return(rval);
  }

static std::string replace_u3_tensor(struct replacement_data& d, const std::vector<std::string>& args, std::vector<struct index_assignment> indices)
  {
    std::string rval = U3_NAME;

    assert(indices.size() == 3);

    for(int i = 0; i < indices.size(); i++)
      {
        rval = rval + "_" + index_stringize(indices[i]);
      }

    return(rval);
  }
