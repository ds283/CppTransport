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

#define DEFAULT_U2_NAME "__u2"
#define DEFAULT_U3_NAME "__u3"
#define DEFAULT_K_NAME  "__k"
#define DEFAULT_K1_NAME "__k1"
#define DEFAULT_K2_NAME "__k2"
#define DEFAULT_K3_NAME "__k3"
#define DEFAULT_A_NAME  "__a"

#define DEFAULT_HSQ_NAME "__Hsq"
#define DEFAULT_EPS_NAME "__eps"

static std::string replace_tool           (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_version        (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_guard          (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_date           (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_source         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_name           (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_author         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_tag            (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_model          (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_header         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_number_fields  (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_number_params  (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_field_list     (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_latex_list     (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_param_list     (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_platx_list     (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_state_list     (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_V              (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_Hsq            (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_eps            (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_abs_err      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_rel_err      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_step         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_stepper      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_abs_err      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_rel_err      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_step         (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_stepper      (struct replacement_data& data, const std::vector<std::string>& args);
static std::string replace_unique         (struct replacement_data& data, const std::vector<std::string>& args);

static std::string replace_parameter      (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_field          (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_coordinate     (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_sr_velocity    (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u1_tensor      (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u2_tensor      (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u3_tensor      (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u1_sexp_tensor (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u2_sexp_tensor (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u3_sexp_tensor (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u2_name        (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_u3_name        (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_zeta_xfm_1     (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);
static std::string replace_zeta_xfm_2     (struct replacement_data& data, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices);


static const std::string pre_macros[] =
  {
    "TOOL", "VERSION", "GUARD", "DATE", "SOURCE",
    "NAME", "AUTHOR", "TAG", "MODEL", "HEADER",
    "NUMBER_FIELDS", "NUMBER_PARAMS",
    "FIELD_NAME_LIST", "LATEX_NAME_LIST",
    "PARAM_NAME_LIST", "PLATX_NAME_LIST",
    "STATE_NAME_LIST", "V", "HUBBLE_SQ", "EPSILON",
    "BACKG_ABS_ERR", "BACKG_REL_ERR", "BACKG_STEP_SIZE", "BACKG_STEPPER",
    "PERT_ABS_ERR", "PERT_REL_ERR", "PERT_STEP_SIZE", "PERT_STEPPER"
  };

static const std::string post_macros[] =
  {
    "UNIQUE"
  };

static const replacement_function_simple pre_macro_replacements[] =
  {
    replace_tool, replace_version, replace_guard, replace_date, replace_source,
    replace_name, replace_author, replace_tag, replace_model, replace_header,
    replace_number_fields, replace_number_params,
    replace_field_list, replace_latex_list,
    replace_param_list, replace_platx_list,
    replace_state_list, replace_V, replace_Hsq, replace_eps,
    replace_b_abs_err, replace_b_rel_err, replace_b_step, replace_b_stepper,
    replace_p_abs_err, replace_p_rel_err, replace_p_step, replace_p_stepper,
  };

static const replacement_function_simple post_macro_replacements[] =
  {
    replace_unique
  };

static const unsigned int pre_macro_args[] =
  {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

static const unsigned int post_macro_args[] =
  {
    0
  };

static const std::string index_macros[] =
  {
    "PARAMETER", "FIELD", "COORDINATE",
    "SR_VELOCITY", "U1_TENSOR", "U2_TENSOR", "U3_TENSOR",
    "U1_PREDEF", "U2_PREDEF", "U3_PREDEF",
    "U2_NAME", "U3_NAME",
    "ZETA_XFM_1", "ZETA_XFM_2"
  };

static const unsigned int index_macro_indices[] =
  {
    1, 1, 1,
    1, 1, 2, 3,
    1, 2, 3,
    2, 3,
    1, 2
  };

static const unsigned int index_macro_ranges[] =
  {
    INDEX_RANGE_PARAMETER, 1, 2,
    1, 2, 2, 2,
    2, 2, 2,
    2, 2,
    2, 2
  };

static const replacement_function_index index_macro_replacements[] =
  {
    replace_parameter, replace_field, replace_coordinate,
    replace_sr_velocity, replace_u1_tensor, replace_u2_tensor, replace_u3_tensor,
    replace_u1_sexp_tensor, replace_u2_sexp_tensor, replace_u3_sexp_tensor,
    replace_u2_name, replace_u3_name,
    replace_zeta_xfm_1, replace_zeta_xfm_2
  };

static const unsigned int index_macro_args[] =
  {
    0, 0, 0,
    0, 0, 2, 4,
    2, 4, 6,
    1, 1,
    0, 0
  };


#define NUMBER_PRE_MACROS    (28)
#define NUMBER_POST_MACROS   (1)
#define NUMBER_INDEX_MACROS  (14)


// ******************************************************************


static bool                                 process        (struct replacement_data& d);

static unsigned int                         get_index_label(struct index_assignment& index);


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

        d.unique        = 0;

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

    struct macro_package ms(d.source->get_number_fields(), d.source->get_number_params(),
      MACRO_PREFIX, LINE_SPLIT, d,
      NUMBER_PRE_MACROS, pre_macros, pre_macro_args, pre_macro_replacements,
      NUMBER_POST_MACROS, post_macros, post_macro_args, post_macro_replacements,
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


static unsigned int get_index_label(struct index_assignment& index)
  {
    unsigned int label = 0;

    switch(index.trait)
      {
        case index_field:
          label = index.species;
        break;

        case index_momentum:
          label = index.species + index.num_fields;
        break;

        case index_parameter:
        default:
          assert(false);
      }

    return(label);
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

    strftime(buf, sizeof(buf), "%X on %d %m %Y", &tstruct);

    std::string rval(buf);

    return(rval);
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

static std::string replace_V(struct replacement_data& d, const std::vector<std::string>& args)
  {
    GiNaC::ex potential = d.source->get_potential();

    std::ostringstream out;
    out << GiNaC::csrc << potential;

    return(out.str());
  }

static std::string replace_Hsq(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::string rval;
    GiNaC::ex Hsq = d.u_factory->compute_Hsq();

    std::ostringstream out;
    out << GiNaC::csrc << Hsq;

    return(out.str());
  }

static std::string replace_eps(struct replacement_data& d, const std::vector<std::string>& args)
  {
    std::string rval;
    GiNaC::ex eps = d.u_factory->compute_eps();

    std::ostringstream out;
    out << GiNaC::csrc << eps;

    return(out.str());
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

static std::string replace_unique(struct replacement_data& data, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    out << data.unique++;

    return(out.str());
  }


// ******************************************************************


static std::string replace_parameter(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].trait == index_parameter);
    assert(indices[0].species < d.source->get_number_params());

    std::vector<GiNaC::symbol> parameters = d.source->get_param_symbols();
    out << GiNaC::csrc << parameters[indices[0].species];

    return(out.str());
  }

static std::string replace_field(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].trait == index_field);
    assert(indices[0].species < d.source->get_number_fields());

    std::vector<GiNaC::symbol> fields = d.source->get_field_symbols();
    out << GiNaC::csrc << fields[indices[0].species];

    return(out.str());
  }

static std::string replace_coordinate(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    std::vector<GiNaC::symbol> fields  = d.source->get_field_symbols();
    std::vector<GiNaC::symbol> momenta = d.source->get_deriv_symbols();

    switch(indices[0].trait)
      {
        case index_field:
          out << GiNaC::csrc << fields[indices[0].species];
          break;

        case index_momentum:
          out << GiNaC::csrc << momenta[indices[0].species];
          break;

        case index_parameter:
        default:
          assert(false);
      }

    return(out.str());
  }

static std::string replace_sr_velocity(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    std::vector<GiNaC::ex> sr_velocity = d.u_factory->compute_sr_u();
    out << GiNaC::csrc << sr_velocity[indices[0].species];

    return(out.str());
  }

static std::string replace_u1_tensor(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    std::vector<GiNaC::ex> u1 = d.u_factory->compute_u1();

    unsigned int i_label = get_index_label(indices[0]);

    out << GiNaC::csrc << u1[i_label];

    return(out.str());
  }

static std::string replace_u1_sexp_tensor(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    assert(args.size() == 2);

    GiNaC::symbol Hsq_symbol(args.size() >= 1 ? args[0] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 2 ? args[1] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex> u1 = d.u_factory->compute_u1(Hsq, eps);

    unsigned int i_label = get_index_label(indices[0]);

    out << GiNaC::csrc << u1[i_label];

    return(out.str());
  }

static std::string replace_u2_tensor(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    assert(args.size() == 2);

    GiNaC::symbol k(args.size() >= 1 ? args[0] : DEFAULT_K_NAME);
    GiNaC::symbol a(args.size() >= 2 ? args[1] : DEFAULT_A_NAME);

    std::vector< std::vector<GiNaC::ex> > u2 = d.u_factory->compute_u2(k, a);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    out << GiNaC::csrc << (u2[i_label])[j_label];

    return(out.str());
  }

static std::string replace_u2_sexp_tensor(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    assert(args.size() == 4);

    GiNaC::symbol k(args.size() >= 1 ? args[0] : DEFAULT_K_NAME);
    GiNaC::symbol a(args.size() >= 2 ? args[1] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 3 ? args[2] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 4 ? args[3] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector< std::vector<GiNaC::ex> > u2 = d.u_factory->compute_u2(k, a, Hsq, eps);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    out << GiNaC::csrc << (u2[i_label])[j_label];

    return(out.str());
  }

static std::string replace_u3_tensor(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());

    assert(args.size() == 4);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol  a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    std::vector< std::vector< std::vector<GiNaC::ex> > > u3 = d.u_factory->compute_u3(k1, k2, k3, a);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    out << GiNaC::csrc << u3[i_label][j_label][k_label];

    return(out.str());
  }

static std::string replace_u3_sexp_tensor(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());

    assert(args.size() == 6);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol  a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector< std::vector< std::vector<GiNaC::ex> > > u3 = d.u_factory->compute_u3(k1, k2, k3, a, Hsq, eps);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    out << GiNaC::csrc << u3[i_label][j_label][k_label];

    return(out.str());
  }

static std::string replace_u2_name(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    assert(args.size() == 1);

    out << (args.size() >= 1 ? args[0] : DEFAULT_U2_NAME) << "_";

    for(int i = 0; i < indices.size(); i++)
      {
        out << (i > 0 ? "_" : "") << index_numeric(indices[i]);
      }

    return(out.str());
  }

static std::string replace_u3_name(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());

    assert(args.size() == 1);

    out << (args.size() >= 1 ? args[0] : DEFAULT_U3_NAME) << "_";

    for(int i = 0; i < indices.size(); i++)
      {
        out << (i > 0 ? "_" : "") << index_numeric(indices[i]);
      }

    return(out.str());
  }

static std::string replace_zeta_xfm_1(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    std::vector<GiNaC::ex> dN = d.u_factory->compute_zeta_xfm_1();

    unsigned int i_label = get_index_label(indices[0]);

    out << GiNaC::csrc << dN[i_label];

    return(out.str());
  }

static std::string replace_zeta_xfm_2(struct replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices)
  {
    std::ostringstream out;

    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    std::vector< std::vector<GiNaC::ex> > ddN = d.u_factory->compute_zeta_xfm_2();

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    out << GiNaC::csrc << (ddN[i_label])[j_label];

    return(out.str());
  }
