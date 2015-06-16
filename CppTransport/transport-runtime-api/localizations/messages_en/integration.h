//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_INTERGRATION_H
#define __CPP_TRANSPORT_MESSAGES_EN_INTERGRATION_H


#define __CPP_TRANSPORT_OBSERVER_SLOW_INTEGRATION "Detected slow integration in progress"
#define __CPP_TRANSPORT_OBSERVER_UPDATE           "Progress update"
#define __CPP_TRANSPORT_OBSERVER_TIME             ": current stepper position at time N"
#define __CPP_TRANSPORT_OBSERVER_ELAPSED_FIRST    "| time elapsed since beginning of integration"
#define __CPP_TRANSPORT_OBSERVER_ELAPSED_LATER    "| time elapsed since last report"

#define __CPP_TRANSPORT_NO_TIMES                  "Fatal: no times specified for integration"

#define __CPP_TRANSPORT_SAMPLES_START_TOO_EARLY_A "fatal: sample times begin before the initial time"
#define __CPP_TRANSPORT_SAMPLES_START_TOO_EARLY_B "earliest sample time"
#define __CPP_TRANSPORT_SAMPLES_START_TOO_EARLY_C "initial time"

#define __CPP_TRANSPORT_NSTAR_TOO_LATE            "Fatal: Nstar is later than the end point of the integration"

#define __CPP_TRANSPORT_TWOPF_STORE               "Fatal: failed to find a configuration which would store the two-point function"
#define __CPP_TRANSPORT_BACKGROUND_STORE          "Fatal: failed to find a configuration which would store the background"

#define __CPP_TRANSPORT_NO_DEVICES                "Fatal: no devices specified in context"

#define __CPP_TRANSPORT_SINGLE_GPU_ONLY           "GPU error: only a single GPU device is currently supported per worker process"

#define __CPP_TRANSPORT_INTEGRATION_FAIL          "Fatal: integration failure"


#endif // __CPP_TRANSPORT_MESSAGES_EN_INTEGRATION_H
