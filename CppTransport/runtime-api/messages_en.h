//
// Created by David Seery on 30/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_MESSAGES_EN_H_
#define __CPP_TRANSPORT_MESSAGES_EN_H_

#define __CPP_TRANSPORT_EFOLDS                "e-folds"
#define __CPP_TRANSPORT_TWOPF_MESSAGE         "Two-point function for"
#define __CPP_TRANSPORT_THREEPF_MESSAGE       "Three-point function for"

#define __CPP_TRANSPORT_WRONG_FIELD_NAMES_A   "Error: supplied number of field names [= "
#define __CPP_TRANSPORT_WRONG_FIELD_NAMES_B   " does not match expected number of fields [= "
#define __CPP_TRANSPORT_WRONG_F_LATEX_NAMES_A "Error: supplied number of LaTeX field names [= "
#define __CPP_TRANSPORT_WRONG_F_LATEX_NAMES_B " does not match expected number of fields [= "
#define __CPP_TRANSPORT_WRONG_PARAM_NAMES_A   "Error: supplied number of parameter names [= "
#define __CPP_TRANSPORT_WRONG_PARAM_NAMES_B   " does not match expected number of parameters [= "
#define __CPP_TRANSPORT_WRONG_P_LATEX_NAMES_A "Error: supplied number of LaTeX parameter names [= "
#define __CPP_TRANSPORT_WRONG_P_LATEX_NAMES_B " does not match expected number of parameters [= "
#define __CPP_TRANSPORT_WRONG_PARAMS_A        "Error: supplied number of parameters [= "
#define __CPP_TRANSPORT_WRONG_PARAMS_B        " does not match expected number [= "

#define __CPP_TRANSPORT_WRONG_ICS_A           "Unexpected number of initial conditions supplied (received "
#define __CPP_TRANSPORT_WRONG_ICS_B           ", expected "
#define __CPP_TRANSPORT_WRONG_ICS_C           " or "

#define __CPP_TRANSPORT_SOLVING_ICS_MESSAGE   "Solving for background evolution using initial conditions"
#define __CPP_TRANSPORT_STEPPER_MESSAGE       "  -- stepper "
#define __CPP_TRANSPORT_ABS_ERR               "abs err"
#define __CPP_TRANSPORT_REL_ERR               "rel err"
#define __CPP_TRANSPORT_STEP_SIZE             "initial step size"

#define __CPP_TRANSPORT_SOLVING_CONFIG        "Solving for configuration"
#define __CPP_TRANSPORT_OF                    "of"

#define __CPP_TRANSPORT_OBSERVER_TIME         "Integration in progress: time N"
#define __CPP_TRANSPORT_OBSERVER_ELAPSED      "| time elapsed since last report"

#define __CPP_TRANSPORT_NO_TIMES              "Fatal: no times specified for integration"
#define __CPP_TRANSPORT_NSTAR_TOO_EARLY       "Fatal: Nstar is earlier than the start point of the integration"
#define __CPP_TRANSPORT_NSTAR_TOO_LATE        "Fatal: Nstar is later than the end point of the integration"

#define __CPP_TRANSPORT_TWOPF_STORE           "Fatal: failed to find a configuration which would store the two-point function"
#define __CPP_TRANSPORT_BACKGROUND_STORE      "Fatal: failed to find a configuration which would store the background"

#define __CPP_TRANSPORT_ASCIITABLE_OUTPUT     "Error: could not open asciitable gadget output"

#define __CPP_TRANSPORT_LOGX_ERROR_A          "Warning: x-axis contains zero or negative values for plot"
#define __CPP_TRANSPORT_LOGX_ERROR_B          "no output produced"
#define __CPP_TRANSPORT_LOGY_ERROR_A          "Warning: y-value contains no nonzero values for line"
#define __CPP_TRANSPORT_LOGY_ERROR_B          "no output produced"

#endif // __CPP_TRANSPORT_MESSAGES_EN_H_