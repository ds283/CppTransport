//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_MODELS_H
#define CPPTRANSPORT_MESSAGES_EN_MODELS_H


#define CPPTRANSPORT_WRONG_FIELD_NAMES_A   "Error: supplied number of field names [= "
#define CPPTRANSPORT_WRONG_FIELD_NAMES_B   "] does not match expected number of fields [= "
#define CPPTRANSPORT_WRONG_F_LATEX_NAMES_A "Error: supplied number of LaTeX field names [= "
#define CPPTRANSPORT_WRONG_F_LATEX_NAMES_B "] does not match expected number of fields [= "
#define CPPTRANSPORT_WRONG_PARAM_NAMES_A   "Error: supplied number of parameter names [= "
#define CPPTRANSPORT_WRONG_PARAM_NAMES_B   "] does not match expected number of parameters [= "
#define CPPTRANSPORT_WRONG_P_LATEX_NAMES_A "Error: supplied number of LaTeX parameter names [= "
#define CPPTRANSPORT_WRONG_P_LATEX_NAMES_B "] does not match expected number of parameters [= "
#define CPPTRANSPORT_WRONG_PARAMS_A        "Error: supplied number of parameters [= "
#define CPPTRANSPORT_WRONG_PARAMS_B        "] does not match expected number [= "

#define CPPTRANSPORT_WRONG_ICS_A           "Error: supplied number of initial conditions [= "
#define CPPTRANSPORT_WRONG_ICS_B           "] does not match expected number [= "
#define CPPTRANSPORT_WRONG_ICS_C           " or "

#define CPPTRANSPORT_SOLVING_ICS_MESSAGE   "Solving for background evolution using initial conditions"
#define CPPTRANSPORT_STEPPER_MESSAGE       "  -- stepper "
#define CPPTRANSPORT_ABS_ERR               "abs err"
#define CPPTRANSPORT_REL_ERR               "rel err"
#define CPPTRANSPORT_STEP_SIZE             "initial step size"

#define CPPTRANSPORT_SOLVING_CONFIG        "Solved for configuration"
#define CPPTRANSPORT_FAILED_CONFIG         "Failed to integrate configuration"
#define CPPTRANSPORT_FAILED_INTERNAL       "internal exception="
#define CPPTRANSPORT_RETRY_CONFIG          "Retrying configuration"
#define CPPTRANSPORT_OF                    "of"
#define CPPTRANSPORT_INTEGRATION_TIME      "integration time"
#define CPPTRANSPORT_BATCHING_TIME         "batching time"
#define CPPTRANSPORT_REFINEMENT_LEVEL      "mesh refinement level"
#define CPPTRANSPORT_REFINEMENT_INTERNAL   "internal exception="

#define CPPTRANSPORT_REFINEMENT_TOO_DEEP   "Integration error: too many levels of mesh refinement were required; decrease the underlying time step"

#define CPPTRANSPORT_FAIL_COMPUTE_T_EXIT   "Error: failed to compute time of horizon exit for all k-modes"


#endif // CPPTRANSPORT_MESSAGES_EN_MODELS_H
