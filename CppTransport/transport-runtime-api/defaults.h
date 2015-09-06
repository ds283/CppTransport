//
// Created by David Seery on 14/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __defaults_H_
#define __defaults_H_


#include "boost/timer/timer.hpp"


#define CPPTRANSPORT_DEFAULT_WRAP_WIDTH                 (135)
#define CPPTRANSPORT_DEFAULT_PLOT_PRECISION             (3)
#define CPPTRANSPORT_DEFAULT_TABLE_PRECISION            (17)

#define CPPTRANSPORT_DEFAULT_ICS_GAP_TOLERANCE          (1E-8)
#define CPPTRANSPORT_DEFAULT_ICS_TIME_STEPS             (5)

// default number of e-folds over which to search for end of inflation
#define CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH    (1000.0)

// default normalization of scale factor ln a(t) at horizon exit of k=1 mode
#define CPPTRANSPORT_DEFAULT_ASTAR_NORMALIZATION        (4.0)

#define CPPTRANSPORT_DEFAULT_MESH_REFINEMENTS           (10)
#define CPPTRANSPORT_DEFAULT_FAST_FORWARD_EFOLDS        (5.0)
#define CPPTRANSPORT_DEFAULT_RECOMMENDED_EFOLDS         (3.0)

#define CPPTRANSPORT_DEFAULT_COLLECT_INITIAL_CONDITIONS (false)

// store wavenumbers to a relative precision of 1 part in 1E5
#define CPPTRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION   (1E-5)

#define CPPTRANSPORT_DEFAULT_PYTHON_PATH                "/usr/bin/python"

// default storage limit on nodes - 500 Mb
// on a machine with 8 workers, that would give 4000 Mb or 4 Gb
// this can be increased (either here, or when creating a
// task_manager object) on machines with more memory
#define CPPTRANSPORT_DEFAULT_BATCHER_STORAGE            (500*1024*1024)
#define CPPTRANSPORT_DEFAULT_PIPE_STORAGE               (500*1024*1024)

// default size of the k-configuration caches - 1 Mb
#define CPPTRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE   (1*1024*1024)

// default checkpointing interval measured in seconds. 0 indicates that checkpointing is disabled
#define CPPTRANSPORT_DEFAULT_CHECKPOINT_INTERVAL        (0)

// tolerance when merging axis points; points closer than this are considered equivalent
#define CPPTRANSPORT_AXIS_MERGE_TOLERANCE               (1E-8)

// smallest allowed squeezing in Fergusson--Shellard--Liguori coordinates
// current 1E-8, which allows about |ln(1E-8)| ~ 18 e-folds of squeezing
#define CPPTRANSPORT_DEFAULT_SMALLEST_SQUEEZING         (1E-8)

// tolerance to use when testing whether kc-configurations match our constrints
#define CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE          (1E-10)

// log file name
#define CPPTRANSPORT_LOG_FILENAME_A                     "worker_"
#define CPPTRANSPORT_LOG_FILENAME_B                     "_%3N.log"

// notification delay for slow integrations; default is 10 minutes
#define CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY    (boost::timer::nanosecond_type(10)*60*1000*1000*1000)

// maximum number of iterations and search tolerance to use when root-finding to compute horizon exit times
constexpr boost::uintmax_t CPPTRANSPORT_MAX_ITERATIONS      = 500;
constexpr double           CPPTRANSPORT_ROOT_FIND_TOLERANCE = 1E-5;
constexpr double           CPPTRANSPORT_ROOT_FIND_ACCURACY  = 1E-3;


#endif //__defaults_H_
