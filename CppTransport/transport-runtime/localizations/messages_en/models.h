//
// Created by David Seery on 05/05/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_MESSAGES_EN_MODELS_H
#define CPPTRANSPORT_MESSAGES_EN_MODELS_H


#define CPPTRANSPORT_WRONG_FIELD_NAMES_A         "Error: supplied number of field names [= "
#define CPPTRANSPORT_WRONG_FIELD_NAMES_B         "] does not match expected number of fields [= "
#define CPPTRANSPORT_WRONG_F_LATEX_NAMES_A       "Error: supplied number of LaTeX field names [= "
#define CPPTRANSPORT_WRONG_F_LATEX_NAMES_B       "] does not match expected number of fields [= "
#define CPPTRANSPORT_WRONG_PARAM_NAMES_A         "Error: supplied number of parameter names [= "
#define CPPTRANSPORT_WRONG_PARAM_NAMES_B         "] does not match expected number of parameters [= "
#define CPPTRANSPORT_WRONG_P_LATEX_NAMES_A       "Error: supplied number of LaTeX parameter names [= "
#define CPPTRANSPORT_WRONG_P_LATEX_NAMES_B       "] does not match expected number of parameters [= "
#define CPPTRANSPORT_WRONG_PARAMS_A              "Error: supplied number of parameters [= "
#define CPPTRANSPORT_WRONG_PARAMS_B              "] does not match expected number [= "

#define CPPTRANSPORT_WRONG_ICS_A                 "Error: supplied number of initial conditions [= "
#define CPPTRANSPORT_WRONG_COORDS_A              "Error: supplied number of phase-space coordinates [= "
#define CPPTRANSPORT_WRONG_ICS_B                 "] does not match expected number [= "
#define CPPTRANSPORT_WRONG_ICS_C                 " or "

#define CPPTRANSPORT_SOLVING_ICS_MESSAGE         "Solving for background evolution using initial conditions"
#define CPPTRANSPORT_STEPPER_MESSAGE             "  -- stepper "
#define CPPTRANSPORT_ABS_ERR                     "abs err"
#define CPPTRANSPORT_REL_ERR                     "rel err"
#define CPPTRANSPORT_STEP_SIZE                   "initial step size"

#define CPPTRANSPORT_SOLVING_CONFIG              "Solved for configuration"
#define CPPTRANSPORT_FAILED_CONFIG               "Failed to integrate configuration"
#define CPPTRANSPORT_FAILED_INTERNAL             "internal exception="
#define CPPTRANSPORT_RETRY_CONFIG                "Retrying configuration"
#define CPPTRANSPORT_OF                          "of"
#define CPPTRANSPORT_INTEGRATION_TIME            "integration time"
#define CPPTRANSPORT_BATCHING_TIME               "batching time"
#define CPPTRANSPORT_REFINEMENT_LEVEL            "mesh refinement level"
#define CPPTRANSPORT_REFINEMENT_INTERNAL         "internal exception="

#define CPPTRANSPORT_EXIT_TIME                   "t_exit"
#define CPPTRANSPORT_EXIT_TIME_KT                "(k_t)"
#define CPPTRANSPORT_MASSLESS_TIME               "t_massless"
#define CPPTRANSPORT_INITIAL_TIME                "t_init"
#define CPPTRANSPORT_MASSLESS_EFOLDS             "massless e-folds"
#define CPPTRANSPORT_SUBHORIZON_EFOLDS           "subhorizon e-folds"

#define CPPTRANSPORT_REFINEMENT_TOO_DEEP         "Integration error: too many levels of mesh refinement were required; decrease the underlying time step"

#define CPPTRANSPORT_INTEGRATOR_NAN_OR_INF       "Integration error: encountered NaN or infinity"

#define CPPTRANSPORT_TENSOR_INDICES_OUT_OF_RANGE "Compute backend error: tensor indices out of range"
#define CPPTRANSPORT_INDICES_OUT_OF_RANGE        "Compute backend error: indies out of range"

#endif // CPPTRANSPORT_MESSAGES_EN_MODELS_H
