//
// Created by David Seery on 30/11/2015.
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

#ifndef CPPTRANSPORT_SWITCHES_H
#define CPPTRANSPORT_SWITCHES_H


// Command line options

#define VERSION_SWITCH                "version"
#define VERSION_HELP                  "output version information"

#define HELP_SWITCH                   "help"
#define HELP_HELP                     "obtain brief description of command-line options"

#define LICENSE_SWITCH                "license"
#define LICENSE_HELP                  "show licensing information for the CppTransport translator"

#define BUILDDATA_SWITCH              "build-data"
#define BUILDDATA_HELP                "show build configuration information"

#define VERBOSE_SWITCH                "verbose,v"
#define VERBOSE_SWITCH_LONG           "verbose"
#define VERBOSE_HELP                  "enable verbose output"

#define INCLUDE_SWITCH                "include,I"
#define INCLUDE_SWITCH_LONG           "include"
#define INCLUDE_HELP                  "add specified path to search list"

#define CORE_OUTPUT_SWITCH            "core-output"
#define CORE_OUTPUT_HELP              "specify name of core header"

#define IMPLEMENTATION_OUTPUT_SWITCH  "implementation-output"
#define IMPLEMENTATION_OUTPUT_HELP    "specify name of implementation header"

#define NO_CSE_SWITCH                 "no-cse"
#define NO_CSE_HELP                   "disable common sub-expression elimination"

#define NO_COLOUR_SWITCH              "no-colour"
#define NO_COLOUR_HELP                "disable colourized output"

#define NO_COLOR_SWITCH               "no-color"
#define NO_COLOR_HELP                 "disable colourized output"

#define INPUT_FILE_SWITCH             "input-file"
#define INPUT_FILE_HELP               "do not use"

#define NO_ENV_SEARCH_SWITCH          "no-search-env"
#define NO_ENV_SEARCH_HELP            "do not search CPPTRANSPORT_PATH"

#define ANNOTATE_SWITCH               "annotate"
#define ANNOTATE_HELP                 "annotate generated header files"

#define UNROLL_POLICY_SWITCH          "unroll-policy"
#define UNROLL_POLICY_HELP            "set maximum size of unrolled index set"

#define FAST_SWITCH                   "fast"
#define FAST_HELP                     "unroll all loops and optimize for speed"

#define PROFILING_SWITCH              "profile"
#define PROFILING_HELP                "display profiling information"

// WARNINGS

#define DEVELOP_WARNINGS              "Wdevelop"
#define DEVELOP_WARN_HELP             "display developer warnings"

#define UNROLL_WARNINGS               "Wunroll"
#define UNROLL_WARN_HELP              "report violations of the unroll policy"

// HEADERS

#define MISC_OPTIONS                  "Miscellaneous options"
#define CONFIG_OPTIONS                "Configuration options"
#define HIDDEN_OPTIONS                "Hidden options"
#define GENERATION_OPTIONS            "Code generation options"
#define WARNING_OPTIONS               "Warnings"


#endif //CPPTRANSPORT_SWITCHES_H
