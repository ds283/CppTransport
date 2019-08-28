//
// Created by David Seery on 14/05/2014.
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


#ifndef CPPTRANSPORT_DEFAULTS_H
#define CPPTRANSPORT_DEFAULTS_H


#include "boost/timer/timer.hpp"


namespace transport
  {

    // default type used for 'number' template type (ie. used during integration)
    // if not otherwise specified
    typedef double default_number_type;

    constexpr unsigned int CPPTRANSPORT_DEFAULT_WRAP_WIDTH                 = (135);
    constexpr unsigned int CPPTRANSPORT_DEFAULT_PLOT_PRECISION             = (3);
    constexpr unsigned int CPPTRANSPORT_DEFAULT_TABLE_PRECISION            = (17);

    constexpr auto         CPPTRANSPORT_DEFAULT_REPORT_TAG_STEM            = "tag";

    constexpr auto         CPPTRANSPORT_DEFAULT_HTML_BUTTON_TAG_PREFIX     = "button";
    constexpr unsigned int CPPTRANSPORT_DEFAULT_HTML_DATABASE_MAX_SIZE     = 5000;
    constexpr unsigned int CPPTRANSPORT_DEFAULT_HTML_MISC_PRECISION        = 2;
    constexpr unsigned int CPPTRANSPORT_DEFAULT_HTML_EFOLDS_PRECISION      = 3;
    constexpr unsigned int CPPTRANSPORT_DEFAULT_HTML_DATABASE_PRECISION    = 6;
    constexpr unsigned int CPPTRANSPORT_DEFAULT_HTML_PAGEABLE_TABLE_SIZE   = 10;

    constexpr unsigned int CPPTRANSPORT_DEFAULT_TERMINAL_WIDTH             = (135);

    constexpr double       CPPTRANSPORT_DEFAULT_ICS_GAP_TOLERANCE          = (1E-8);
    constexpr unsigned int CPPTRANSPORT_DEFAULT_ICS_TIME_STEPS             = (5);

    // default number of e-folds over which to search for end of inflation
    constexpr double       CPPTRANSPORT_DEFAULT_END_OF_INFLATION_SEARCH    = (1000.0);

    // default normalization of scale factor ln a(t) at horizon exit of k=1 mode
    constexpr double       CPPTRANSPORT_DEFAULT_ASTAR_NORMALIZATION        = (0.0);

    constexpr unsigned int CPPTRANSPORT_DEFAULT_MESH_REFINEMENTS           = (10);
    constexpr double       CPPTRANSPORT_DEFAULT_ADAPTIVE_ICS_EFOLDS        = (4.0);
    constexpr double       CPPTRANSPORT_DEFAULT_RECOMMENDED_EFOLDS         = (4.0);
    constexpr double       CPPTRANSPORT_DEFAULT_RECOMMENDED_SETTLE_EFOLDS  = (2.0);

    constexpr bool         CPPTRANSPORT_DEFAULT_COLLECT_INITIAL_CONDITIONS = (false);

    // store wavenumbers to a relative precision of 1 part in 1E5
    constexpr double       CPPTRANSPORT_DEFAULT_KCONFIG_SEARCH_PRECISION   = (1E-5);

    constexpr auto         CPPTRANSPORT_DEFAULT_PYTHON_EXECUTABLE          = "python";
    constexpr auto         CPPTRANSPORT_DEFAULT_DOT_EXECUTABLE             = "dot";
    constexpr auto         CPPTRANSPORT_DEFAULT_SENDMAIL_EXECUTABLE        = "CppTransport-sendmail";
    
    constexpr auto         CPPTRANSPORT_DEFAULT_PYTHON_LOCATION            = "/usr/bin";
    constexpr auto         CPPTRANSPORT_DEFAULT_DOT_LOCATION               = "/usr/bin";
    constexpr auto         CPPTRANSPORT_DEFAULT_SENDMAIL_LOCATION          = "/usr/bin";

    constexpr auto         CPPTRANSPORT_HOME_ENV                           = "HOME";
    constexpr auto         CPPTRANSPORT_PATH_ENV                           = "CPPTRANSPORT_PATH";
    constexpr auto         CPPTRANSPORT_SHELL_PATH_ENV                     = "PATH";
    constexpr auto         CPPTRANSPORT_TERM_ENV                           = "TERM";
    constexpr auto         CPPTRANSPORT_COLUMNS_ENV                        = "COLUMNS";
    constexpr auto         CPPTRANSPORT_RUNTIME_CONFIG_FILE                = ".cpptransport_runtime";
    constexpr auto         CPPTRANSPORT_PROFILE_CONFIG_FILE                = ".profile";
    constexpr auto         CPPTRANSPORT_HTML_RESOURCE_DIRECTORY            = "HTML";

    constexpr auto         CPPTRANSPORT_DEFAULT_COMPLETION_UNSET           = "unset";
    
    // name of global timer used in master and slave controllers
    constexpr auto         CPPTRANSPORT_DEFAULT_TIMER                      = "global";

    // default storage limit on nodes - 500 Mb
    // on a machine with 8 workers, that would give 4000 Mb or 4 Gb
    // this can be increased (either here, or when creating a
    // task_manager object) on machines with more memory
    constexpr unsigned int CPPTRANSPORT_DEFAULT_BATCHER_STORAGE            = (500*1024*1024);
    constexpr unsigned int CPPTRANSPORT_DEFAULT_PIPE_STORAGE               = (500*1024*1024);

    // default size of the k-configuration caches - 1 Mb
    constexpr unsigned int CPPTRANSPORT_DEFAULT_CONFIGURATION_CACHE_SIZE   = (1*1024*1024);

    // default checkpointing interval measured in seconds. 0 indicates that checkpointing is disabled
    constexpr unsigned int CPPTRANSPORT_DEFAULT_CHECKPOINT_INTERVAL        = (0);
    
    // default intervals at which to issue progress reports during tasks
    constexpr unsigned int CPPTRANSPORT_DEFAULT_REPORT_PERCENT_INTERVAL    = (10);
    constexpr unsigned int CPPTRANSPORT_DEFAULT_REPORT_TIME_INTERVAL       = (0);
    constexpr unsigned int CPPTRANSPORT_DEFAULT_REPORT_TIME_DELAY          = (60*5);

    // tolerance when merging axis points; points closer than this are considered equivalent
    constexpr double       CPPTRANSPORT_AXIS_MERGE_TOLERANCE               = (1E-8);

    // smallest allowed squeezing in Fergusson--Shellard coordinates
    // currently 1E-8, which allows about |ln(1E-8)| ~ 18 e-folds of squeezing
    constexpr double       CPPTRANSPORT_DEFAULT_SMALLEST_SQUEEZING         = (1E-8);

    // tolerance to use when testing whether kc-configurations match our constraints
    constexpr double       CPPTRANSPORT_DEFAULT_KCONFIG_TOLERANCE          = (1E-10);

    // log file name
    constexpr auto         CPPTRANSPORT_LOG_FILENAME_A                     = "worker_";
    constexpr auto         CPPTRANSPORT_LOG_FILENAME_B                     = "_%3N.log";
    
    constexpr auto         CPPTRANSPORT_REPORT_FILENAME_A                  = "reports.txt";
    
    // maximum number of attempts to gain a database lick
    constexpr unsigned int CPPTRANSPORT_DEFAULT_LOCKFILE_ATTEMPTS          = (50);

    // maximum number of iterations and search tolerance to use when root-finding to compute horizon exit times
    constexpr boost::uintmax_t CPPTRANSPORT_MAX_ITERATIONS                 = 500;
    constexpr double           CPPTRANSPORT_ROOT_FIND_TOLERANCE            = 1E-4;
    constexpr double           CPPTRANSPORT_ROOT_FIND_ACCURACY             = 1E-3;

    // default maximum size which can be automatically VACUUMed
    constexpr boost::uintmax_t CPPTRANSPORT_MAX_VACUUMABLE_SIZE            = 2147483648;   // = 2Gb

    // notification delay for slow integrations; default is 10 minutes
    constexpr boost::timer::nanosecond_type CPPTRANSPORT_DEFAULT_SLOW_INTEGRATION_NOTIFY = boost::timer::nanosecond_type(10)*60*1000*1000*1000;

  }   // namespace transport


#endif //CPPTRANSPORT_DEFAULTS_H
