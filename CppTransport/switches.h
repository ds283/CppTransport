//
// Created by David Seery on 30/11/2015.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_SWITCHES_H
#define CPPTRANSPORT_SWITCHES_H


// Command line options

#define VERSION_SWITCH                "version"
#define VERSION_HELP                  "output version information"

#define HELP_SWITCH                   "help"
#define HELP_HELP                     "obtain brief description of command-line options"

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

#define MISC_OPTIONS                  "Miscellaneous options"
#define CONFIG_OPTIONS                "Configuration options"
#define HIDDEN_OPTIONS                "Hidden options"


#endif //CPPTRANSPORT_SWITCHES_H