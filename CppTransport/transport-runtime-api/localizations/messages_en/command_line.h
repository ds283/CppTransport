//
// Created by David Seery on 19/06/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_COMMAND_LINE_H
#define CPPTRANSPORT_COMMAND_LINE_H


#define CPPTRANSPORT_SWITCH_HELP              "help"
#define CPPTRANSPORT_HELP_HELP                "output brief description of command-line options"

#define CPPTRANSPORT_SWITCH_VERSION           "version"
#define CPPTRANSPORT_HELP_VERSION             "output version information"

#define CPPTRANSPORT_SWITCH_REPO              "repo"
#define CPPTRANSPORT_HELP_REPO                "specify repository path"

#define CPPTRANSPORT_SWITCH_TAG               "tag"
#define CPPTRANSPORT_HELP_TAG                 "add tag"

#define CPPTRANSPORT_SWITCH_CAPACITY          "caches"
#define CPPTRANSPORT_HELP_CAPACITY            "set global cache (batcher, datapipe) capacity, measured in Mb"

#define CPPTRANSPORT_SWITCH_BATCHER_CAPACITY  "batch-cache"
#define CPPTRANSPORT_HELP_BATCHER_CAPACITY    "set batcher cache capacity, measured in Mb"

#define CPPTRANSPORT_SWITCH_CACHE_CAPACITY    "datapipe-cache"
#define CPPTRANSPORT_HELP_CACHE_CAPACITY      "set datapipe cache capacity, measured in Mb"

#define CPPTRANSPORT_SWITCH_VERBOSE           "verbose,v"
#define CPPTRANSPORT_SWITCH_VERBOSE_LONG      "verbose"
#define CPPTRANSPORT_HELP_VERBOSE             "enable verbose output"

#define CPPTRANSPORT_SWITCH_GANTT_CHART       "gantt-chart"
#define CPPTRANSPORT_HELP_GANTT_CHART         "ouput Gantt chart describing worker activity"

#define CPPTRANSPORT_SWITCH_SEED              "seed"
#define CPPTRANSPORT_HELP_SEED                "specify named output group to use as a seed"

#define CPPTRANSPORT_SWITCH_TASK              "task"
#define CPPTRANSPORT_HELP_TASK                "add named task to list of jobs"


#endif //CPPTRANSPORT_COMMAND_LINE_H
