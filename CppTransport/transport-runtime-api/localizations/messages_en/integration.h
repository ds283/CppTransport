//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_INTERGRATION_H
#define CPPTRANSPORT_MESSAGES_EN_INTERGRATION_H


#define CPPTRANSPORT_OBSERVER_SLOW_INTEGRATION "Detected slow integration in progress"
#define CPPTRANSPORT_OBSERVER_UPDATE           "Progress update"
#define CPPTRANSPORT_OBSERVER_TIME             ": current stepper position at time N"
#define CPPTRANSPORT_OBSERVER_ELAPSED_FIRST    "| time elapsed since beginning of integration"
#define CPPTRANSPORT_OBSERVER_ELAPSED_LATER    "| time elapsed since last report"

#define CPPTRANSPORT_NO_TIMES                  "Fatal: no times specified for integration"

#define CPPTRANSPORT_SAMPLES_START_TOO_EARLY_A "fatal: sample times begin before the initial time"
#define CPPTRANSPORT_SAMPLES_START_TOO_EARLY_B "earliest sample time"
#define CPPTRANSPORT_SAMPLES_START_TOO_EARLY_C "initial time"

#define CPPTRANSPORT_NSTAR_TOO_LATE            "Fatal: Nstar is later than the end point of the integration"

#define CPPTRANSPORT_TWOPF_STORE               "Fatal: failed to find a configuration which would store the two-point function"
#define CPPTRANSPORT_BACKGROUND_STORE          "Fatal: failed to find a configuration which would store the background"

#define CPPTRANSPORT_NO_DEVICES                "Fatal: no devices specified in context"

#define CPPTRANSPORT_SINGLE_GPU_ONLY           "GPU error: only a single GPU device is currently supported per worker process"

#define CPPTRANSPORT_INTEGRATION_FAIL          "Fatal: integration failure"


#endif // CPPTRANSPORT_MESSAGES_EN_INTEGRATION_H
