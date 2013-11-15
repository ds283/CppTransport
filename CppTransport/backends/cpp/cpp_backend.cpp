//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <assert.h>
#include <time.h>

#include <vector>
#include <list>

#include "boost/algorithm/string.hpp"

#include "core.h"
#include "cpp_backend.h"
#include "to_printable.h"
#include "macro.h"
#include "cse.h"
#include "flatten.h"
#include "cse_map.h"

#define MACRO_PREFIX "$$__"
#define LINE_SPLIT   "$$//"

#define DEFAULT_U2_NAME "__u2"
#define DEFAULT_U3_NAME "__u3"
#define DEFAULT_M_NAME  "__M"
#define DEFAULT_K_NAME  "__k"
#define DEFAULT_K1_NAME "__k1"
#define DEFAULT_K2_NAME "__k2"
#define DEFAULT_K3_NAME "__k3"
#define DEFAULT_A_NAME  "__a"

#define DEFAULT_HSQ_NAME "__Hsq"
#define DEFAULT_EPS_NAME "__eps"

#define DEFAULT_STEPPER_STATE_NAME "<UNKNOWN_STATE_TYPE>"

#define DEFAULT_POOL_TEMPLATE      "auto $1 = $2;"

// PRE macros
static std::string replace_tool           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_version        (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_guard          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_date           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_source         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_name           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_author         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_tag            (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_model          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_header         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_core           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_number_fields  (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_number_params  (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_field_list     (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_latex_list     (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_param_list     (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_platx_list     (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_state_list     (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_V              (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_Hsq            (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_eps            (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_abs_err      (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_rel_err      (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_step         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_b_stepper      (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_abs_err      (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_rel_err      (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_step         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_p_stepper      (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_backg_stepper  (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_pert_stepper   (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_temp_pool      (replacement_data& data, const std::vector<std::string>& args);

// POST macros
static std::string replace_unique         (replacement_data& data, const std::vector<std::string>& args);

// INDEX macros
static std::string replace_parameter      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);

static std::string replace_field          (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);

static std::string replace_coordinate     (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);

static void*       pre_sr_velocity        (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_sr_velocity    (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_sr_velocity       (void* state);

static void*       pre_u1_tensor          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u1_tensor      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u1_tensor         (void* state);

static void*       pre_u2_tensor          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u2_tensor      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u2_tensor         (void* state);

static void*       pre_u3_tensor          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u3_tensor      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u3_tensor         (void* state);

static void*       pre_u1_predef          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u1_predef      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u1_predef         (void* state);

static void*       pre_u2_predef          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u2_predef      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u2_predef         (void* state);

static void*       pre_u3_predef          (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u3_predef      (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u3_predef         (void* state);

static void*       pre_u2_name            (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u2_name        (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u2_name           (void* state);

static void*       pre_u3_name            (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_u3_name        (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_u3_name           (void* state);

static void*       pre_zeta_xfm_1         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_zeta_xfm_1     (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_zeta_xfm_1        (void* state);

static void*       pre_zeta_xfm_2         (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_zeta_xfm_2     (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_zeta_xfm_2        (void* state);

static void*       pre_A_tensor           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_A_tensor       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_A_tensor          (void* state);

static void*       pre_B_tensor           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_B_tensor       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_B_tensor          (void* state);

static void*       pre_C_tensor           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_C_tensor       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_C_tensor          (void* state);

static void*       pre_A_predef           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_A_predef       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_A_predef          (void* state);

static void*       pre_B_predef           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_B_predef       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_B_predef          (void* state);

static void*       pre_C_predef           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_C_predef       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_C_predef          (void* state);

static void*       pre_M_tensor           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_M_tensor       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_M_tensor          (void* state);

static void*       pre_M_predef           (replacement_data& data, const std::vector<std::string>& args);
static std::string replace_M_predef       (replacement_data& data, const std::vector<std::string>& args,
                                           std::vector<struct index_assignment> indices, void* state);
static void        post_M_predef          (void* state);


static const std::string pre_macros[] =
  {
    "TOOL", "VERSION", "GUARD", "DATE", "SOURCE",
    "NAME", "AUTHOR", "TAG", "MODEL", "HEADER", "CORE",
    "NUMBER_FIELDS", "NUMBER_PARAMS",
    "FIELD_NAME_LIST", "LATEX_NAME_LIST",
    "PARAM_NAME_LIST", "PLATX_NAME_LIST",
    "STATE_NAME_LIST", "V", "HUBBLE_SQ", "EPSILON",
    "BACKG_ABS_ERR", "BACKG_REL_ERR", "BACKG_STEP_SIZE", "BACKG_STEPPER",
    "PERT_ABS_ERR", "PERT_REL_ERR", "PERT_STEP_SIZE", "PERT_STEPPER",
    "MAKE_BACKG_STEPPER", "MAKE_PERT_STEPPER",
    "TEMP_POOL"
  };

static const std::string post_macros[] =
  {
    "UNIQUE"
  };

static const replacement_function_simple pre_macro_replacements[] =
  {
    replace_tool, replace_version, replace_guard, replace_date, replace_source,
    replace_name, replace_author, replace_tag, replace_model, replace_header, replace_core,
    replace_number_fields, replace_number_params,
    replace_field_list, replace_latex_list,
    replace_param_list, replace_platx_list,
    replace_state_list, replace_V, replace_Hsq, replace_eps,
    replace_b_abs_err, replace_b_rel_err, replace_b_step, replace_b_stepper,
    replace_p_abs_err, replace_p_rel_err, replace_p_step, replace_p_stepper,
    replace_backg_stepper, replace_pert_stepper,
    replace_temp_pool
  };

static const replacement_function_simple post_macro_replacements[] =
  {
    replace_unique
  };

static const unsigned int pre_macro_args[] =
  {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    1, 1,
    1
  };

static const unsigned int post_macro_args[] =
  {
    0
  };

static const std::string index_macros[] =
  {
    "PARAMETER", "FIELD", "COORDINATE",
    "SR_VELOCITY",
    "U1_TENSOR", "U2_TENSOR", "U3_TENSOR",
    "U1_PREDEF", "U2_PREDEF", "U3_PREDEF",
    "U2_NAME", "U3_NAME",
    "ZETA_XFM_1", "ZETA_XFM_2",
    "A_TENSOR", "B_TENSOR", "C_TENSOR",
    "A_PREDEF", "B_PREDEF", "C_PREDEF",
    "M_TENSOR", "M_PREDEF"
  };

static const unsigned int index_macro_indices[] =
  {
    1, 1, 1,
    1,
    1, 2, 3,
    1, 2, 3,
    2, 3,
    1, 2,
    3, 3, 3,
    3, 3, 3,
    2, 2
  };

static const unsigned int index_macro_ranges[] =
  {
    INDEX_RANGE_PARAMETER, 1, 2,
    1, 2, 2, 2,
    2, 2, 2,
    2, 2,
    2, 2,
    1, 1, 1,
    1, 1, 1,
    1, 1
  };

static const replacement_function_pre index_macro_pre[] =
  {
    nullptr, nullptr, nullptr,
    pre_sr_velocity,
    pre_u1_tensor, pre_u2_tensor, pre_u3_tensor,
    pre_u1_predef, pre_u2_predef, pre_u3_predef,
    nullptr, nullptr,
    pre_zeta_xfm_1, pre_zeta_xfm_2,
    pre_A_tensor, pre_B_tensor, pre_C_tensor,
    pre_A_predef, pre_B_predef, pre_C_predef,
    pre_M_tensor, pre_M_predef
  };

static const replacement_function_post index_macro_post[] =
  {
    nullptr, nullptr, nullptr,
    post_sr_velocity,
    post_u1_tensor, post_u2_tensor, post_u3_tensor,
    post_u1_predef, post_u2_predef, post_u3_predef,
    nullptr, nullptr,
    post_zeta_xfm_1, post_zeta_xfm_2,
    post_A_tensor, post_B_tensor, post_C_tensor,
    post_A_predef, post_B_predef, post_C_predef,
    post_M_tensor, post_M_predef
  };

static const replacement_function_index index_macro_replacements[] =
  {
    replace_parameter, replace_field, replace_coordinate,
    replace_sr_velocity,
    replace_u1_tensor, replace_u2_tensor, replace_u3_tensor,
    replace_u1_predef, replace_u2_predef, replace_u3_predef,
    replace_u2_name, replace_u3_name,
    replace_zeta_xfm_1, replace_zeta_xfm_2,
    replace_A_tensor, replace_B_tensor, replace_C_tensor,
    replace_A_predef, replace_B_predef, replace_C_predef,
    replace_M_tensor, replace_M_predef
  };

static const unsigned int index_macro_args[] =
  {
    0, 0, 0,
    0, 0, 2, 4,
    2, 4, 6,
    1, 1,
    0, 0,
    4, 4, 4,
    6, 6, 6,
    0, 2
  };


#define NUMBER_PRE_MACROS    (32)
#define NUMBER_POST_MACROS   (1)
#define NUMBER_INDEX_MACROS  (22)


// ******************************************************************

static bool         apply_replacement(const std::string& input, const std::string& output,
                                      u_tensor_factory* u_factory, cse& temp_factory, flattener& fl,
                                      const struct input& data, script* source, finder& path, bool cse);
static std::string  strip_dot_h      (const std::string& pathname);
static std::string  leafname         (const std::string& pathname);

static bool         process          (replacement_data& d);

static unsigned int get_index_label  (struct index_assignment& index);

static std::string  replace_stepper  (const struct stepper& s, std::string state_name);


// ******************************************************************


// generate C++ output
// returns 'false' if this fails
bool cpp_backend(const struct input& data, finder& path, bool do_cse)
  {
    bool rval = true;

    script* source = data.driver->get_script();

    u_tensor_factory* u_factory = make_u_tensor_factory(source);
    cse               temp_factory(0);
    flattener         fl(1);  // dimension of flattener will be set later

    rval = apply_replacement(source->get_core(), data.core_output, u_factory, temp_factory, fl, data, source, path, do_cse);
    if(rval) rval = apply_replacement(source->get_implementation(), data.implementation_output, u_factory, temp_factory, fl, data, source, path, do_cse);

    delete u_factory;

    return(rval);
  }


static bool apply_replacement(const std::string& input, const std::string& output,
                              u_tensor_factory* u_factory, cse& temp_factory, flattener& fl,
                              const struct input& data, script* source, finder& path, bool do_cse)
  {
    bool rval = true;
    std::string h_template;

    if(input == "")
      {
        error(ERROR_NO_CPP_TEMPLATE);
        rval = false;
      }
    else
      {
        if((rval = path.fqpn(input + ".h", h_template)) == false)
          {
            std::ostringstream msg;
            msg << ERROR_MISSING_CPP_HEADER << " '" << input << ".h'";
            error(msg.str());
          }
      }

    if(rval)
      {
        // set up a buffer to allow insertion of temporaries into the output stream
        std::list<std::string> buffer;

        // set up replacement data
        replacement_data d(buffer, temp_factory, fl);

        d.source        = source;
        d.source_file   = data.name;

        d.u_factory     = u_factory;

        d.do_cse        = do_cse;
        d.pool          = buffer.begin();
        d.pool_template = DEFAULT_POOL_TEMPLATE;

        d.output_file   = strip_dot_h(output) + ".h";
        d.core_file     = strip_dot_h(data.core_output) + ".h";
        d.guard         = boost::to_upper_copy(leafname(output));
        d.template_file = h_template;

        d.unique        = 0;

        d.num_fields    = source->get_number_fields();

        // set up a deque of inclusions for error reporting purposes
        struct inclusion inc;
        inc.line = 0;               // line number is irrelevant; we just set it to zero
        inc.name = d.template_file;
        d.path.push_back(inc);
        d.current_line = 1;

        class macro_package ms(source->get_number_fields(), source->get_number_params(), source->get_indexorder(),
          MACRO_PREFIX, LINE_SPLIT, d,
          NUMBER_PRE_MACROS, pre_macros, pre_macro_args, pre_macro_replacements,
          NUMBER_POST_MACROS, post_macros, post_macro_args, post_macro_replacements,
          NUMBER_INDEX_MACROS, index_macros, index_macro_indices, index_macro_ranges, index_macro_args,
                               index_macro_replacements, index_macro_pre, index_macro_post);

        d.ms = &ms;

        rval = process(d);
      }

    return(rval);
  }


static std::string strip_dot_h(const std::string& pathname)
  {
    std::string rval;

    if(pathname.substr(pathname.length() - 3) == ".h")
      {
        rval = pathname.substr(0, pathname.length() - 3);
      }
    else if(pathname.substr(pathname.length() - 5) == ".hpp")
      {
        rval = pathname.substr(0, pathname.length() - 5);
      }
    else
      {
        rval = pathname;
      }

    return(rval);
  }


static std::string leafname(const std::string& pathname)
  {
    std::string rval = pathname;
    size_t      pos;

    while((pos = rval.find("/")) != std::string::npos)
      {
        rval.erase(0, pos+1);
      }

    while((pos = rval.find(".")) != std::string::npos)
      {
        rval.erase(pos+1, rval.length()-pos-1);
      }

    return(rval);
  }


// ******************************************************************


static bool process(replacement_data& d)
  {
    bool rval = true;

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

    if(in.is_open())
      {
        while(in.eof() == false && in.fail() == false)
          {
            std::string line;
            std::getline(in, line);

            d.ms->apply(line);
            d.buffer.push_back(line);

            d.current_line++;
          }

        for(std::list<std::string>::iterator it = d.buffer.begin(); it != d.buffer.end() && out.fail() == false; it++)
          {
            out << *it << std::endl;
          }
        if(out.fail())
          {
            std::ostringstream msg;
            msg << ERROR_CPP_BACKEND_WRITE << "'" << d.output_file << "'";
            error(msg.str());
          }
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_CPP_TEMPLATE_READ << " '" << d.template_file << "'";
        error(msg.str());
        rval = false;
      }

    if(rval == true) rval = !out.fail();

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


static std::string replace_stepper(const struct stepper& s, std::string state_name)
  {
    std::ostringstream out;

    // note that we need a generic stepper which works with an arbitrary state type; see
    // http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/concepts/system.html
    // we can't use things like rosenbrock4 or the implicit euler methods which work only with boost matrices

    // exactly when the steppers call the observer functor depends which stepper is in use; see
    // http://headmyshoulder.github.io/odeint-v2/doc/boost_numeric_odeint/odeint_in_detail/integrate_functions.html

    // to summarize the discussion there:
    //  ** If stepper is a Stepper or Error Stepper dt is the step size used for integration.
    //     However, whenever a time point from the sequence is approached the step size dt will
    //     be reduced to obtain the state x(t) exactly at the time point.
    //  ** If stepper is a Controlled Stepper then dt is the initial step size. The actual step
    //     size is adjusted during integration according to error control.
    //     However, if a time point from the sequence is approached the step size is
    //     reduced to obtain the state x(t) exactly at the time point. [runge_kutta_fehlberg78]
    //  ** If stepper is a Dense Output Stepper then dt is the initial step size. The actual step
    //     size is adjusted during integration according to error control. Dense output is used
    //     to obtain the states x(t) at the time points from the sequence. [runge_kutta_dopri5, bulirsch_stoer]

    if(s.name == "runge_kutta_dopri5")
      {
        out << "make_dense_output< runge_kutta_dopri5< " << state_name << " > >(" << s.abserr << ", " << s.relerr << ")";
      }
    else if(s.name == "bulirsch_stoer_dense_out")
      {
        out << "bulirsch_stoer_dense_out< " << state_name << " >(" << s.abserr << ", " << s.relerr << ")";
      }
    else if(s.name == "runge_kutta_fehlberg78")
      {
        out << "make_controlled< runge_kutta_fehlberg78< " << state_name << " > >(" << s.abserr << ", " << s.relerr << ")";
      }
    else
      {
        std::ostringstream msg;
        msg << ERROR_UNKNOWN_STEPPER << " '" << s.name << "'";
        error(msg.str());
        out << "<UNKNOWN_STEPPER>";
      }

    return(out.str());
  }


// ********************************************************************************


// REPLACEMENT RULES

// PRE macros


static std::string replace_tool(replacement_data& d, const std::vector<std::string>& args)
  {
    return CPPTRANSPORT_NAME;
  }

static std::string replace_version(replacement_data& d, const std::vector<std::string>& args)
  {
    return CPPTRANSPORT_VERSION;
  }

static std::string replace_guard(replacement_data& d, const std::vector<std::string>& args)
  {
    return "__CPP_TRANSPORT_" + d.guard + "_H_";
  }

static std::string replace_date(replacement_data& d, const std::vector<std::string>& args)
  {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%X on %d %m %Y", &tstruct);

    std::string rval(buf);

    return(rval);
  }

static std::string replace_source(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source_file);
  }

static std::string replace_name(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_name());
  }

static std::string replace_author(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_author());
  }

static std::string replace_tag(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_tag());
  }

static std::string replace_model(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.source->get_model());
  }

static std::string replace_header(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.output_file);
  }

static std::string replace_core(replacement_data& d, const std::vector<std::string>& args)
  {
    return(d.core_file);
  }

static std::string replace_number_fields(replacement_data& d, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    out << d.source->get_number_fields();

    return(out.str());
  }

static std::string replace_number_params(replacement_data& d, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    out << d.source->get_number_params();

    return(out.str());
  }

static std::string replace_field_list(replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_latex_list(replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_param_list(replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_platx_list(replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_state_list(replacement_data& d, const std::vector<std::string>& args)
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

static std::string replace_V(replacement_data& d, const std::vector<std::string>& args)
  {
    GiNaC::ex potential = d.source->get_potential();

    std::ostringstream out;
    out << GiNaC::csrc << potential;

    return(out.str());
  }

static std::string replace_Hsq(replacement_data& d, const std::vector<std::string>& args)
  {
    std::string rval;
    GiNaC::ex Hsq = d.u_factory->compute_Hsq();

    std::ostringstream out;
    out << GiNaC::csrc << Hsq;

    return(out.str());
  }

static std::string replace_eps(replacement_data& d, const std::vector<std::string>& args)
  {
    std::string rval;
    GiNaC::ex eps = d.u_factory->compute_eps();

    std::ostringstream out;
    out << GiNaC::csrc << eps;

    return(out.str());
  }

static std::string replace_b_abs_err(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    std::ostringstream out;
    out << s.abserr;

    return(out.str());
  }

static std::string replace_b_rel_err(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    std::ostringstream out;
    out << s.relerr;

    return(out.str());
  }

static std::string replace_b_step(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    std::ostringstream out;
    out << s.stepsize;

    return(out.str());
  }

static std::string replace_b_stepper(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    return(s.name);
  }

static std::string replace_p_abs_err(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    std::ostringstream out;
    out << s.abserr;

    return(out.str());
  }

static std::string replace_p_rel_err(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    std::ostringstream out;
    out << s.relerr;

    return(out.str());
  }

static std::string replace_p_step(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    std::ostringstream out;
    out << s.stepsize;

    return(out.str());
  }

static std::string replace_p_stepper(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    return(s.name);
  }

static std::string replace_backg_stepper(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_background_stepper();

    assert(args.size() == 1);
    std::string state_name = (args.size() >= 1 ? args[0] : DEFAULT_STEPPER_STATE_NAME);

    return(replace_stepper(s, state_name));
  }

static std::string replace_pert_stepper(replacement_data& data, const std::vector<std::string>& args)
  {
    const struct stepper s = data.source->get_perturbations_stepper();

    assert(args.size() == 1);
    std::string state_name = (args.size() >= 1 ? args[0] : DEFAULT_STEPPER_STATE_NAME);

    return(replace_stepper(s, state_name));
  }

static std::string replace_temp_pool(replacement_data& data, const std::vector<std::string>& args)
  {
    assert(args.size() == 1);
    std::string t = (args.size() >= 1 ? args[0] : DEFAULT_POOL_TEMPLATE);

    // mark current endpoint in the buffer as the insertion point

    data.pool_template = t;
    data.pool          = --data.buffer.end();

    return(""); // replace with a blank
  }

// POST macros


static std::string replace_unique(replacement_data& data, const std::vector<std::string>& args)
  {
    std::ostringstream out;

    out << data.unique++;

    return(out.str());
  }


// ******************************************************************


// INDEX macros


// ******************************************************************

static std::string replace_parameter(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].trait == index_parameter);
    assert(indices[0].species < d.source->get_number_params());

    std::vector<GiNaC::symbol> parameters = d.source->get_param_symbols();
    out << GiNaC::csrc << parameters[indices[0].species];

    return(out.str());
  }

// ******************************************************************

static std::string replace_field(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    std::ostringstream out;

    assert(indices.size() == 1);
    assert(indices[0].trait == index_field);
    assert(indices[0].species < d.source->get_number_fields());

    std::vector<GiNaC::symbol> fields = d.source->get_field_symbols();
    out << GiNaC::csrc << fields[indices[0].species];

    return(out.str());
  }

// ******************************************************************

static std::string replace_coordinate(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
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

// ******************************************************************

static void* pre_sr_velocity(replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_sr_u(*container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_sr_velocity(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    return((*map)[d.fl.flatten(indices[0].species)]);
  }

static void post_sr_velocity(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_u1_tensor(replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_u1(*container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_u1_tensor(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    return((*map)[d.fl.flatten(i_label)]);
  }

static void post_u1_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_u1_predef(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 2);

    GiNaC::symbol Hsq_symbol(args.size() >= 1 ? args[0] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 2 ? args[1] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_u1(Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_u1_predef(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    return((*map)[d.fl.flatten(i_label)]);
  }

static void post_u1_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_u2_tensor(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 2);

    GiNaC::symbol k(args.size() >= 1 ? args[0] : DEFAULT_K_NAME);
    GiNaC::symbol a(args.size() >= 2 ? args[1] : DEFAULT_A_NAME);

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_u2(k, a, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_u2_tensor(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    return((*map)[d.fl.flatten(i_label,j_label)]);
  }

static void post_u2_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_u2_predef(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 4);

    GiNaC::symbol k(args.size() >= 1 ? args[0] : DEFAULT_K_NAME);
    GiNaC::symbol a(args.size() >= 2 ? args[1] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 3 ? args[2] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 4 ? args[3] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_u2(k, a, Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_u2_predef(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    return((*map)[d.fl.flatten(i_label,j_label)]);
  }

static void post_u2_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_u3_tensor(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 4);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol  a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_u3(k1, k2, k3, a, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_u3_tensor(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_u3_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_u3_predef(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 6);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol  a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_u3(k1, k2, k3, a, Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_u3_predef(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_u3_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static std::string replace_u2_name(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
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

// ******************************************************************

static std::string replace_u3_name(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
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

// ******************************************************************

static void* pre_zeta_xfm_1(replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_zeta_xfm_1(*container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_zeta_xfm_1(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 1);
    assert(indices[0].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);

    return((*map)[d.fl.flatten(i_label)]);
  }

static void post_zeta_xfm_1(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_zeta_xfm_2(replacement_data& d, const std::vector<std::string>& args)
  {
    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_zeta_xfm_2(*container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_zeta_xfm_2(replacement_data& d, const std::vector<std::string>& args,
  std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(2*d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    return((*map)[d.fl.flatten(i_label,j_label)]);
  }

static void post_zeta_xfm_2(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_A_tensor(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 4);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol  a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_A(k1, k2, k3, a, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_A_tensor(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);
    assert(indices[2].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_A_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_A_predef(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 6);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol  a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_A(k1, k2, k3, a, Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return(map);
  }

static std::string replace_A_predef(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);
    assert(indices[2].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_A_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*)state;
  }

// ******************************************************************

static void* pre_B_tensor(replacement_data &d, const std::vector<std::string> &args)
  {
    assert(args.size() == 4);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_B(k1, k2, k3, a, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return (map);
  }

static std::string replace_B_tensor(replacement_data &d, const std::vector<std::string> &args,
                                    std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);
    assert(indices[2].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*) state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return ((*map)[d.fl.flatten(i_label, j_label, k_label)]);
  }

static void post_B_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*) state;
  }

// ******************************************************************

static void* pre_B_predef(replacement_data& d, const std::vector<std::string>& args)
  {
    assert(args.size() == 6);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_B(k1, k2, k3, a, Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return (map);
  }

static std::string replace_B_predef(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void *state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);
    assert(indices[2].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);
 
    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_B_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*) state;
  }

// ******************************************************************

static void* pre_C_tensor(replacement_data &d, const std::vector<std::string> &args)
  {
    assert(args.size() == 4);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_C(k1, k2, k3, a, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return (map);
  }

static std::string replace_C_tensor(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void *state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);
    assert(indices[2].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_C_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*) state;
  }

// ******************************************************************

static void* pre_C_predef(replacement_data &d, const std::vector<std::string> &args)
  {
    assert(args.size() == 6);

    GiNaC::symbol k1(args.size() >= 1 ? args[0] : DEFAULT_K1_NAME);
    GiNaC::symbol k2(args.size() >= 2 ? args[1] : DEFAULT_K2_NAME);
    GiNaC::symbol k3(args.size() >= 3 ? args[2] : DEFAULT_K3_NAME);
    GiNaC::symbol a(args.size() >= 4 ? args[3] : DEFAULT_A_NAME);

    GiNaC::symbol Hsq_symbol(args.size() >= 5 ? args[4] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 6 ? args[5] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_C(k1, k2, k3, a, Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return (map);
  }

static std::string replace_C_predef(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 3);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[2].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);
    assert(indices[2].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);
    unsigned int k_label = get_index_label(indices[2]);

    return((*map)[d.fl.flatten(i_label,j_label,k_label)]);
  }

static void post_C_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*) state;
  }

// ******************************************************************

static void* pre_M_tensor(replacement_data &d, const std::vector<std::string> &args)
  {
    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_M(*container, d.fl);

    cse_map* map = new cse_map(container, d);

    return (map);
  }

static std::string replace_M_tensor(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    return((*map)[d.fl.flatten(i_label,j_label)]);
  }

static void post_M_tensor(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*) state;
  }

// ******************************************************************

static void* pre_M_predef(replacement_data &d, const std::vector<std::string> &args)
  {
    assert(args.size() == 2);

    GiNaC::symbol Hsq_symbol(args.size() >= 1 ? args[0] : DEFAULT_HSQ_NAME);
    GiNaC::symbol eps_symbol(args.size() >= 2 ? args[1] : DEFAULT_EPS_NAME);
    GiNaC::ex     Hsq = Hsq_symbol;
    GiNaC::ex     eps = eps_symbol;

    std::vector<GiNaC::ex>* container = new std::vector<GiNaC::ex>;
    d.u_factory->compute_M(Hsq, eps, *container, d.fl);

    cse_map* map = new cse_map(container, d);

    return (map);
  }

static std::string replace_M_predef(replacement_data& d, const std::vector<std::string>& args,
                                    std::vector<struct index_assignment> indices, void* state)
  {
    assert(indices.size() == 2);
    assert(indices[0].species < d.source->get_number_fields());
    assert(indices[1].species < d.source->get_number_fields());
    assert(indices[0].trait == index_field);
    assert(indices[1].trait == index_field);

    assert(state != nullptr);
    cse_map* map = (cse_map*)state;

    d.fl.set_size(d.num_fields);

    unsigned int i_label = get_index_label(indices[0]);
    unsigned int j_label = get_index_label(indices[1]);

    return((*map)[d.fl.flatten(i_label,j_label)]);
  }

static void post_M_predef(void* state)
  {
    assert(state != nullptr);

    delete (cse_map*) state;
  }

// ******************************************************************
