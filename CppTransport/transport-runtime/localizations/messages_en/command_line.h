//
// Created by David Seery on 19/06/15.
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

#ifndef CPPTRANSPORT_COMMAND_LINE_H
#define CPPTRANSPORT_COMMAND_LINE_H


#define CPPTRANSPORT_SWITCH_HELP              "help"
#define CPPTRANSPORT_HELP_HELP                "output brief description of command-line options"

#define CPPTRANSPORT_SWITCH_VERSION           "version"
#define CPPTRANSPORT_HELP_VERSION             "output version information"

#define CPPTRANSPORT_SWITCH_LICENSE           "license"
#define CPPTRANSPORT_HELP_LICENSE             "show licensing information for the CppTransport platform"

#define CPPTRANSPORT_SWITCH_BUILDDATA         "build-data"
#define CPPTRANSPORT_HELP_BUILDDATA           "show build configuration information"

#define CPPTRANSPORT_SWITCH_PROF_AGGREGATE    "profile-aggregation"
#define CPPTRANSPORT_HELP_PROF_AGGREGATE      "profile database aggregations"

#define CPPTRANSPORT_SWITCH_REPO              "repo,r"
#define CPPTRANSPORT_SWITCH_REPO_LONG         "repo"
#define CPPTRANSPORT_HELP_REPO                "specify repository path"

#define CPPTRANSPORT_SWITCH_INCLUDE           "include,I"
#define CPPTRANSPORT_SWITCH_INCLUDE_LONG      "include"
#define CPPTRANSPORT_HELP_INCLUDE             "add specified path to search list"

#define CPPTRANSPORT_SWITCH_NETWORK_MODE      "network-mode"
#define CPPTRANSPORT_HELP_NETWORK_MODE        "must be set if repository is on a network filing system (NFS, Lustre)"

#define CPPTRANSPORT_SWITCH_REJECT_FAILED     "reject-failed"
#define CPPTRANSPORT_HELP_REJECT_FAILED       "don't commit failed integrations"

#define CPPTRANSPORT_SWITCH_TAG               "tag"
#define CPPTRANSPORT_HELP_TAG                 "add tag to output generated by task"

#define CPPTRANSPORT_SWITCH_CAPACITY          "caches"
#define CPPTRANSPORT_HELP_CAPACITY            "set capacity for all caches, measured in Mb (default 500Mb)"

#define CPPTRANSPORT_SWITCH_BATCHER_CAPACITY  "batch-cache"
#define CPPTRANSPORT_HELP_BATCHER_CAPACITY    "set batcher cache capacity, measured in Mb (default 500Mb)"

#define CPPTRANSPORT_SWITCH_CACHE_CAPACITY    "datapipe-cache"
#define CPPTRANSPORT_HELP_CACHE_CAPACITY      "set datapipe cache capacity, measured in Mb (default 500Mb)"

#define CPPTRANSPORT_SWITCH_VERBOSE           "verbose,v"
#define CPPTRANSPORT_SWITCH_VERBOSE_LONG      "verbose"
#define CPPTRANSPORT_HELP_VERBOSE             "enable verbose output"

#define CPPTRANSPORT_SWITCH_GANTT_CHART       "gantt"
#define CPPTRANSPORT_HELP_GANTT_CHART         "output Gantt chart describing activity"

#define CPPTRANSPORT_SWITCH_JOURNAL           "journal"
#define CPPTRANSPORT_HELP_JOURNAL             "write JSON journal describing activity"

#define CPPTRANSPORT_SWITCH_CHECKPOINT        "checkpoint"
#define CPPTRANSPORT_HELP_CHECKPOINT          "commit data after given interval, measured in minutes (default off)"

#define CPPTRANSPORT_SWITCH_RECOVER           "recover"
#define CPPTRANSPORT_HELP_RECOVER             "attempt to recover crashed tasks or jobs"

#define CPPTRANSPORT_SWITCH_UPGRADE           "upgrade"
#define CPPTRANSPORT_HELP_UPGRADE             "upgrade repository data containers if required"

#define CPPTRANSPORT_SWITCH_SEED              "seed"
#define CPPTRANSPORT_HELP_SEED                "use named content group as a seed"

#define CPPTRANSPORT_SWITCH_CREATE            "create"
#define CPPTRANSPORT_HELP_CREATE              "write repository content"

#define CPPTRANSPORT_SWITCH_TASK              "task"
#define CPPTRANSPORT_HELP_TASK                "add named task to list of jobs to be processed"

#define CPPTRANSPORT_SWITCH_MODELS            "models"
#define CPPTRANSPORT_HELP_MODELS              "list models baked into this executable"

#define CPPTRANSPORT_SWITCH_NO_COLOUR         "no-colour"
#define CPPTRANSPORT_HELP_NO_COLOUR           "disable colourized output"

#define CPPTRANSPORT_SWITCH_NO_COLOR          "no-color"
#define CPPTRANSPORT_HELP_NO_COLOR            "disable colourized output"

#define CPPTRANSPORT_SWITCH_WIDTH             "width"
#define CPPTRANSPORT_HELP_WIDTH               "specify default terminal width"

#define CPPTRANSPORT_SWITCH_PLOT_STYLE        "plot-style,p"
#define CPPTRANSPORT_SWITCH_PLOT_STYLE_LONG   "plot-style"
#define CPPTRANSPORT_HELP_PLOT_STYLE          "set plotting style"

#define CPPTRANSPORT_SWITCH_MPL_BACKEND       "mpl-backend"
#define CPPTRANSPORT_HELP_MPL_BACKEND         "set Matplotlib backend"

#define CPPTRANSPORT_SWITCH_STATUS            "status"
#define CPPTRANSPORT_HELP_STATUS              "summarize current status of repository"

#define CPPTRANSPORT_SWITCH_INFLIGHT          "inflight"
#define CPPTRANSPORT_HELP_INFLIGHT            "report on currently active jobs"

#define CPPTRANSPORT_SWITCH_INFO              "info"
#define CPPTRANSPORT_HELP_INFO                "show information on the specified object"

#define CPPTRANSPORT_SWITCH_PROVENANCE        "provenance"
#define CPPTRANSPORT_HELP_PROVENANCE          "report detailed provenance data on content group"

#define CPPTRANSPORT_SWITCH_HTML              "html"
#define CPPTRANSPORT_HELP_HTML                "produce a HTML-format repository report"

#define CPPTRANSPORT_SWITCH_OBJECT            "object"
#define CPPTRANSPORT_HELP_OBJECT              "select repository objects"

#define CPPTRANSPORT_SWITCH_LOCK              "lock"
#define CPPTRANSPORT_HELP_LOCK                "lock specified objects to prevent modification"

#define CPPTRANSPORT_SWITCH_UNLOCK            "unlock"
#define CPPTRANSPORT_HELP_UNLOCK              "unlock specified objects"

#define CPPTRANSPORT_SWITCH_ADD_TAG           "add-tag"
#define CPPTRANSPORT_HELP_ADD_TAG             "add tag to specified objects"

#define CPPTRANSPORT_SWITCH_DELETE_TAG        "delete-tag"
#define CPPTRANSPORT_HELP_DELETE_TAG          "remove tag from specified objects"

#define CPPTRANSPORT_SWITCH_ADD_NOTE          "add-note"
#define CPPTRANSPORT_HELP_ADD_NOTE            "add note to specified objects"

#define CPPTRANSPORT_SWITCH_DELETE_NOTE       "delete-note"
#define CPPTRANSPORT_HELP_DELETE_NOTE         "remove note from specified objects"

#define CPPTRANSPORT_SWITCH_DELETE            "delete"
#define CPPTRANSPORT_HELP_DELETE              "remove specified content groups"
#define CPPTRANSPORT_SWITCH_DELETE_SINGLET    "--delete cannot be used in conjunction with other repository actions"

#define CPPTRANSPORT_SWITCH_REPORT_PERCENT    "report-percent"
#define CPPTRANSPORT_HELP_REPORT_PERCENT      "report on task progress at given percentage intervals (default 10%)"

#define CPPTRANSPORT_SWITCH_REPORT_INTERVAL   "report-interval"
#define CPPTRANSPORT_HELP_REPORT_INTERVAL     "report on task progress at given time intervals (default off)"

#define CPPTRANSPORT_SWITCH_REPORT_DELAY      "report-delay"
#define CPPTRANSPORT_HELP_REPORT_DELAY        "set delay before issuing first report (default 5 min)"

#define CPPTRANSPORT_SWITCH_REPORT_EMAIL      "report-email,M"
#define CPPTRANSPORT_SWITCH_REPORT_EMAIL_LONG "report-email"
#define CPPTRANSPORT_HELP_REPORT_EMAIL        "forward progress reports to email address (default none)"

#define CPPTRANSPORT_SWITCH_REPORT_WHEN       "report-when,m"
#define CPPTRANSPORT_SWITCH_REPORT_WHEN_LONG  "report-when"
#define CPPTRANSPORT_HELP_REPORT_WHEN         "specify at which times reports should be issued to email (default bpe)"


#endif //CPPTRANSPORT_COMMAND_LINE_H
