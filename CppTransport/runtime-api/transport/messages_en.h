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

#define __CPP_TRANSPORT_WRONG_ICS_A           "Error: supplied number of initial conditions [= "
#define __CPP_TRANSPORT_WRONG_ICS_B           " does not match expected number [= "
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

#define __CPP_TRANSPORT_NO_DEVICES            "Fatal: no devices specified in context"

#define __CPP_TRANSPORT_ASCIITABLE_OUTPUT     "Error: could not open asciitable gadget output"

#define __CPP_TRANSPORT_WORK_QUEUE_OUTPUT_A   "Work queue over"
#define __CPP_TRANSPORT_WORK_QUEUE_OUTPUT_B   "devices:"
#define __CPP_TRANSPORT_WORK_QUEUE_OUTPUT_C   "device:"
#define __CPP_TRANSPORT_WORK_QUEUE_WEIGHT     "weight"
#define __CPP_TRANSPORT_WORK_QUEUE_MAXMEM     "max memory"
#define __CPP_TRANSPORT_WORK_QUEUE_UNBOUNDED  "no max memory size"
#define __CPP_TRANSPORT_WORK_QUEUE_QUEUE_NAME "Queue"
#define __CPP_TRANSPORT_WORK_QUEUE_QUEUE      "queue"
#define __CPP_TRANSPORT_WORK_QUEUE_QUEUES     "queues"

#define __CPP_TRANSPORT_KCONFIG_SERIAL        "Serial ="
#define __CPP_TRANSPORT_KCONFIG_KEQUALS       "k ="
#define __CPP_TRANSPORT_KCONFIG_KTEQUALS      "k_t ="
#define __CPP_TRANSPORT_KCONFIG_ALPHAEQUALS   "alpha ="
#define __CPP_TRANSPORT_KCONFIG_BETAEQUALS    "beta ="

#define __CPP_TRANSPORT_GIGABYTE              "Gb"
#define __CPP_TRANSPORT_MEGABYTE              "Mb"
#define __CPP_TRANSPORT_KILOBYTE              "kb"
#define __CPP_TRANSPORT_BYTE                  "b"

#define __CPP_TRANSPORT_WORK_LIST_RANGE       "Out of range when accessing 'device_work_list' object"
#define __CPP_TRANSPORT_DEVICE_QUEUE_RANGE    "Out of range when accessing 'device_queue' object"
#define __CPP_TRANSPORT_WORK_QUEUE_RANGE      "Out of range when accessing 'work_queue' object"
#define __CPP_TRANSPORT_TIMES_RANGE           "Out of range when accessing 'task' object"
#define __CPP_TRANSPORT_TWOPF_TASK_LIST_RANGE "Out of range when accessing 'twopf_list_task' object"
#define __CPP_TRANSPORT_RANGE_RANGE           "Out of range when accessing 'range' object"

#define __CPP_TRANSPORT_INTEGRATION_FAIL      "Fatal: integration failure"

#define __CPP_TRANSPORT_INSTANCES_MULTIPLE    "Attempt to add multiple instances of the same 'model' object"
#define __CPP_TRANSPORT_INSTANCES_DELETE      "Attempt to delete unregistered 'model' object"
#define __CPP_TRANSPORT_INSTANCES_MISSING     "No registered instance of model with uid"

#define __CPP_TRANSPORT_RANGE_INVALID_SPACING "Unknown spacing type in 'range' object"

#define __CPP_TRANSPORT_ICS_MISMATCH          "Mismatched number of values when constructing 'initial_conditions' object"
#define __CPP_TRANSPORT_PARAMS_MISMATCH       "Mismatched number of values when constructing 'parameters' object"
#define __CPP_TRANSPORT_MPLANCK_NEGATIVE      "Nonpositive value for M_Planck is not allowed"
#define __CPP_TRANSPORT_PARAM_DATA_MISMATCH   "Mismatched parameter and names in 'parameters' object"

#define __CPP_TRANSPORT_XML_MISMATCH          "Mismatched node tags when performing XML serialization"

#define __CPP_TRANSPORT_REPO_MISSING_ROOT     "Root directory does not exist when opening repository"
#define __CPP_TRANSPORT_REPO_MISSING_ENV      "Environment directory not present when opening repository"
#define __CPP_TRANSPORT_REPO_MISSING_CNTR     "Containers directory not present when opening repository"
#define __CPP_TRANSPORT_REPO_MISSING_PACKAGES "Container for model definitions not present or inaccessible when opening repository"
#define __CPP_TRANSPORT_REPO_MISSING_INTGRTNS "Container for integration definitions not present or inaccessible when opening repository"
#define __CPP_TRANSPORT_REPO_MODEL_EXISTS     "Attempt to insert duplicate model specification"
#define __CPP_TRANSPORT_REPO_INTGRTN_EXISTS   "Attempt to insert duplicate integration specification"
#define __CPP_TRANSPORT_REPO_INSERT_ERROR     "Error while inserting XML document into repository (DBXML code="
#define __CPP_TRANSPORT_REPO_QUERY_ERROR      "Error while querying XML document from repository (DBXML code="
#define __CPP_TRANSPORT_REPO_FAIL_ENV         "Failed to construct DBXML environment for repository"
#define __CPP_TRANSPORT_REPO_ROOT_EXISTS      "Root directory already exists when attempting to construct repository"
#define __CPP_TRANSPORT_REPO_WRITE_SLAVE      "Attempt to write to repository from MPI slave process"
#define __CPP_TRANSPORT_REPO_NOT_SET          "Attempt to write to repository when unset in 'task_manager'"
#define __CPP_TRANSPORT_REPO_NULL_MODEL       "Null model when attempting to write to repository"
#define __CPP_TRANSPORT_REPO_MISSING_TASK     "Repository error: could not find specified task"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL_A  "Repository error: could not find model"
#define __CPP_TRANSPORT_REPO_MISSING_MODEL_B  "corresponding to specified task"
#define __CPP_TRANSPORT_REPO_SKIPPING_TASK    "; skipping this task"
#define __CPP_TRANSPORT_REPO_NONE             "Nothing to do: no repository specified"
#define __CPP_TRANSPORT_BADLY_FORMED_INTGRTN  "Badly formed XML for 'integration' document"
#define __CPP_TRANSPORT_BADLY_FORMED_PACKAGE  "Badly formed XML for 'package' document"
#define __CPP_TRANSPORT_BADLY_FORMED_MODEL    "Badly formed XML for 'model' group"
#define __CPP_TRANSPORT_BADLY_FORMED_PARAMS   "Badly formed XML for 'parameters' group"
#define __CPP_TRANSPORT_RUN_REPAIR            "; consider checking database integrity"
#define __CPP_TRANSPORT_REPO_FOR_TASK         "for task"

#define __CPP_TRANSPORT_RANGE_A               "Range: "
#define __CPP_TRANSPORT_RANGE_B               " steps, spacing = "
#define __CPP_TRANSPORT_RANGE_C               ": min = "
#define __CPP_TRANSPORT_RANGE_D               "max = "
#define __CPP_TRANSPORT_RANGE_LINEAR          "linear"
#define __CPP_TRANSPORT_RANGE_LOGARITHMIC     "logarithmic"
#define __CPP_TRANSPORT_SERIALIZE_NULL_RANGE  "Error: attempt to serialize null 'range' object"

#define __CPP_TRANSPORT_TASK_TIMES            "Sampling times: "
#define __CPP_TRANSPORT_TASK_THREEPF_TYPE     "Unknown wavenumber grid type when attempting to write threepf task to repository"
#define __CPP_TRANSPORT_SERIALIZE_BASE_TASK   "Error: attempt to serialize base 'task' object"

#define __CPP_TRANSPORT_PARAMS_TAG            "Parameters:"
#define __CPP_TRANSPORT_MPLANCK_TAG           "M_Planck = "

#define __CPP_TRANSPORT_MULTIPLE_SET_REPO     "Multiple repository specifiers; the second and subsequent specifiers will be ignored"
#define __CPP_TRANSPORT_EXPECTED_REPO         "Too few arguments: expected pathname to repository"
#define __CPP_TRANSPORT_EXPECTED_TASK_ID      "Too few arguments: expected task identifier"
#define __CPP_TRANSPORT_UNKNOWN_SWITCH        "Unknown command-line switch"

#define __CPP_TRANSPORT_NAMED_LIST_FAIL       "Lookup failed in dbxml_delegate::named_list"

#define __CPP_TRANSPORT_EXEC_SLAVE            "task_manager::execute_tasks() called on slave process"
#define __CPP_TRANSPORT_WAIT_MASTER           "task_manager::wait_for_tasks() called on master process"

#define __CPP_TRANSPORT_LOGX_ERROR_A          "Warning: x-axis contains zero or negative values for plot"
#define __CPP_TRANSPORT_LOGX_ERROR_B          "no output produced"
#define __CPP_TRANSPORT_LOGY_ERROR_A          "Warning: y-value contains no nonzero values for line"
#define __CPP_TRANSPORT_LOGY_ERROR_B          "no output produced"

#endif // __CPP_TRANSPORT_MESSAGES_EN_H_