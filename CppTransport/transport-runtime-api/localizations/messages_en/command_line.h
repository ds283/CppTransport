//
// Created by David Seery on 19/06/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
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
#define CPPTRANSPORT_HELP_CAPACITY            "set capacity for all caches, measured in Mb"

#define CPPTRANSPORT_SWITCH_BATCHER_CAPACITY  "batch-cache"
#define CPPTRANSPORT_HELP_BATCHER_CAPACITY    "set batcher cache capacity, measured in Mb"

#define CPPTRANSPORT_SWITCH_CACHE_CAPACITY    "datapipe-cache"
#define CPPTRANSPORT_HELP_CACHE_CAPACITY      "set datapipe cache capacity, measured in Mb"

#define CPPTRANSPORT_SWITCH_VERBOSE           "verbose,v"
#define CPPTRANSPORT_SWITCH_VERBOSE_LONG      "verbose"
#define CPPTRANSPORT_HELP_VERBOSE             "enable verbose output"

#define CPPTRANSPORT_SWITCH_GANTT_CHART       "gantt-chart"
#define CPPTRANSPORT_HELP_GANTT_CHART         "output Gantt chart describing worker activity"

#define CPPTRANSPORT_SWITCH_JOURNAL           "journal"
#define CPPTRANSPORT_HELP_JOURNAL             "write JSON-format journal describing worker activity"

#define CPPTRANSPORT_SWITCH_CHECKPOINT        "checkpoint"
#define CPPTRANSPORT_HELP_CHECKPOINT          "flush data after given interval, measured in minutes"

#define CPPTRANSPORT_SWITCH_RECOVER           "recover"
#define CPPTRANSPORT_HELP_RECOVER             "attempt to recover crashed output groups"

#define CPPTRANSPORT_SWITCH_SEED              "seed"
#define CPPTRANSPORT_HELP_SEED                "use named output group as a seed"

#define CPPTRANSPORT_SWITCH_TASK              "task"
#define CPPTRANSPORT_HELP_TASK                "add named task to list of jobs"

#define CPPTRANSPORT_SWITCH_MODELS            "models"
#define CPPTRANSPORT_HELP_MODELS              "list models baked into this executable"

#define CPPTRANSPORT_SWITCH_NO_COLOUR         "no-colour"
#define CPPTRANSPORT_HELP_NO_COLOUR           "disable colourized output"

#define CPPTRANSPORT_SWITCH_NO_COLOR          "no-color"
#define CPPTRANSPORT_HELP_NO_COLOR            "disable colourized output"


#endif //CPPTRANSPORT_COMMAND_LINE_H
