//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013-15 University of Sussex. All rights reserved.
//

#ifndef __msg_H_
#define __msg_H_

// Messages

#define ERROR_NO_ERROR_HANDLER                "Internal error: no registered error handler"
#define ERROR_NO_WARNING_HANDLER              "Internal error: no registered warning handler"
#define ERROR_NO_MESSAGE_HANDLER              "Internal error: no registered message handler"
#define ERROR_NOT_FULL_CONTEXT                "Internal error: attempt to unpack error context without full context data"

#define ERROR_MESSAGE_AT_LINE                 "at line"
#define ERROR_MESSAGE_WRAP_PAD                "  "
#define OUTPUT_STACK_OF                       "of"
#define OUTPUT_STACK_WRAP_PAD                 ERROR_MESSAGE_WRAP_PAD
#define OUTPUT_STACK_INCLUDED_FROM            "included from line"
#define OUTPUT_STACK_OF_FILE                  "of file"

#define WARNING_TOKEN                         "warning: "
#define ERROR_TOKEN                           "error: "

#define ERROR_OPEN_TOPLEVEL                   "Could not open top-level file"
#define ERROR_INCLUDE_FILE                    "Could not open included file"
#define ERROR_INCLUDE_DIRECTIVE               "Expected filename following #include directive"

#define ERROR_CPP_BUFFER_WRITE                "Error opening output file"

#define ERROR_CSE_POWER_ARGUMENTS             "Unexpected number of arguments to pow() during common subexpression elimination"

#define ERROR_NO_CORE_TEMPLATE                "fatal: No core template specified"
#define ERROR_NO_IMPLEMENTATION_TEMPLATE      "fatal: No implementation template specified"
#define ERROR_MISSING_TEMPLATE                "Could not find specified template"
#define ERROR_READING_TEMPLATE                "Error reading from template file"
#define ERROR_IMPROPER_TEMPLATE_HEADER        "Improperly formed header line in template"
#define ERROR_EXPECTED_TEMPLATE_BACKEND       "Expected backend specifier following 'backend' keyword in template"
#define ERROR_EXPECTED_TEMPLATE_MINVER        "Expected minver specifier following 'minver' keyword in template"
#define ERROR_TEMPLATE_TOO_RECENT_A           "Template"
#define ERROR_TEMPLATE_TOO_RECENT_B           "requires more recent version of CppTransport (>"
#define ERROR_TEMPLATE_BACKEND_A              "Template"
#define ERROR_TEMPLATE_BACKEND_B              "requires unknown backend"
#define WARNING_DUPLICATE_TEMPLATE_BACKEND    "Header line contains duplicate backend specifiers in template"
#define WARNING_DUPLICATE_TEMPLATE_MINVER     "Header line contains duplicate minver specifiers in template"

#define ERROR_VEXCL_NO_STEPPER_STATE          "Fatal: stepper state type specification is mandatory with VexCL backend"
#define WARNING_VEXCL_STEPPER_IGNORED_A       "Using stepper type"
#define WARNING_VEXCL_STEPPER_IGNORED_B       "; VexCL backend ignores stepper specification"

#define ERROR_UNRECOGNIZED_NUMBER             "Unrecognized numeric literal"
#define ERROR_UNRECOGNIZED_SYMBOL             "Unrecognized character"
#define ERROR_EXPECTED_CLOSE_QUOTE            "Unterminated string literal"
#define ERROR_EXPECTED_ELLIPSIS               "Expected '...', not '..'"

#define ERROR_SYMBOL_EXISTS                   "Redefinition of existing symbol"
#define NOTIFY_DUPLICATION_DEFINITION_WAS     "Location of original definition for symbol"
#define ERROR_IDENTIFIER_LOOKUP               "Lookup of identifier name failed"
#define ERROR_STRING_LOOKUP                   "Lookup of string literal value failed"
#define ERROR_INTEGER_LOOKUP                  "Lookup of integer literal value failed"
#define ERROR_DECIMAL_LOOKUP                  "Lookup of decimal literal value failed"
#define ERROR_UNKNOWN_IDENTIFIER              "Undeclared identifier"

#define ERROR_EXPECTED_INDEX                  "Expected index label following prefix"
#define ERROR_TOO_FEW_INDICES                 "Too few indices supplied to summation-convention object"
#define ERROR_EXPECTED_OPEN_INDEX_LIST        "Expected index list beginning with '[' for indexed macro"
#define ERROR_EXPECTED_INDEX_LABEL_A          "Expected index label for indexed macro"
#define ERROR_EXPECTED_INDEX_LABEL_B          "received"
#define ERROR_EXPECTED_CLOSE_INDEX_LIST       "Expected close of index list ']' for indexed macro"
#define ERROR_DUPLICATE_INDEX                 "Duplicate declaration of lvalue summation convention index"
#define ERROR_EXPECTED_OPEN_ARGUMENT_LIST     "Missing open of argument list '{' for macro"
#define ERROR_EXPECTED_CLOSE_ARGUMENT_LIST    "Missing close of argument list '}' for macro"
#define ERROR_EXPECTED_CLOSE_ARGUMENT_QUOTE   "Missing closing quotation '\"' for macro"
#define ERROR_WRONG_ARGUMENT_COUNT            "Unexpected number of arguments to macro"
#define ERROR_EXPECTED_ARGUMENT_COUNT         "expected"
#define ERROR_WRONG_INDEX_COUNT               "Unexpected number of arguments to macro"
#define ERROR_EXPECTED_INDEX_COUNT            "expected"

#define ERROR_MISSING_LHS                     "Missing left-hand side in temporary template "
#define ERROR_MISSING_RHS                     "Missing right-hand side in temporary template "

#define ERROR_NO_BUFFER_REGISTERED            "Internal error: TemporaryPool macro package in use, but no buffer registered"
#define ERROR_NO_MACROS_REGISTERED            "Internal error: TemporaryPool macro package in use, but no macro set registered"

#define ERROR_FILESTACK_EMPTY                 "Internal error: filestack empty when accessing top element"
#define ERROR_FILESTACK_TOO_SHORT             "Internal error: filestack empty when attempting to pop"
#define ERROR_FILESTACK_OVERWRITE             "Internal error: attempt to overwrite data for top filestack element"

#define ERROR_CURRENT_LINE_EMPTY              "Internal error: current line empty in lexfile"

#define WARNING_TEMPORARY_NO_TAG_SET          "Temporary pool being written, but no location set: defaults to end-of-buffer"

#define WARNING_HEX_CONVERSION_A              "Leading '0x' in "
#define WARNING_HEX_CONVERSION_B              "indicates hex, but did not convert: treated as decmial"
#define WARNING_OCTAL_CONVERSION_A            "Leading '0' in "
#define WARNING_OCTAL_CONVERSION_B            "indicates octal, but did not convert: treated as decimal"

#define WARNING_RECURSION_DEPTH               "Maximum depth of recursive macro substitution exceeded (max"

#define WARNING_LEXEME_KEYWORD                "Internal error: attempt to get keyword type for non-keyword lexeme"
#define WARNING_LEXEME_SYMBOL                 "Internal error: attempt to get symbol type for non-symbol lexeme"
#define WARNING_LEXEME_INTEGER                "Internal error: attempt to get integer value for non-integer lexeme"
#define WARNING_LEXEME_DECIMAL                "Internal error: attempt to get decimal value for non-decimal lexeme"
#define WARNING_LEXEME_STRING                 "Internal error: attempt to get string value for non-string lexeme"
#define WARNING_LEXEME_IDENTIFIER             "Internal error: attempt to get identifier value for non-identifier lexeme"

#define WARNING_UNKNOWN_SWITCH                "Unknown command-line switch"

#define WARNING_PARSING_FAILED                "Failed to parse file"

#define MESSAGE_EXPRESSION_CACHE_HITS         "expression cache hits"
#define MESSAGE_EXPRESSION_CACHE_MISSES       "misses"
#define MESSAGE_EXPRESSION_CACHE_QUERY_TIME   "time spent performing queries"

#define MESSAGE_SYMBOLIC_COMPUTE_TIME         "symbolic computation"
#define MESSAGE_CSE_TIME                      "common sub-expression elimination"
#define MESSAGE_MACRO_TIME                    "macro replacement took"
#define MESSAGE_TOKENIZATION_TIME             "of which time spent tokenizing"

#define MESSAGE_PROCESSING_COMPLETE_A         "processed"
#define MESSAGE_PROCESSING_SINGULAR           "model"
#define MESSAGE_PROCESSING_PLURAL             "models"
#define MESSAGE_PROCESSING_COMPLETE_B         "in time"

#define MESSAGE_TRANSLATING                   "translating"
#define MESSAGE_TRANSLATING_TO                "into"
#define MESSAGE_EXPANSION_OF_LINE             "expansion of template line"

#define MESSAGE_OPENED_BUFFER                 "opened output file"

#define MESSAGE_TRANSLATION_RESULT            "translation succeeded with"
#define MESSAGE_MACRO_REPLACEMENTS            "macro replacements"

#define ERROR_UNKNOWN_STEPPER                 "Unknown or unimplemented odeint-v2 stepper"

#define MESSAGE_HOUR_LABEL                    "h"
#define MESSAGE_MINUTE_LABEL                  "m"
#define MESSAGE_SECOND_LABEL                  "s"

// output strings

#define BACKEND_MACRO_PREFIX                  "$$__"
#define BACKEND_LINE_SPLIT                    "$$//"

#define OUTPUT_CPPTRANSPORT_TAG               "__CPPTRANSPORT"
#define OUTPUT_TIME_FORMAT                    "%X on %d %m %Y"
#define OUTPUT_DEFAULT_POOL_TEMPLATE          "auto $1 = $2;"

#define OUTPUT_DEFAULT_CPP_CSE_TEMPORARY_NAME "__temp"

#define OUTPUT_DEFAULT_U2_NAME                "__u2"
#define OUTPUT_DEFAULT_U3_NAME                "__u3"
#define OUTPUT_DEFAULT_M_NAME                 "__M"
#define OUTPUT_DEFAULT_K_NAME                 "__k"
#define OUTPUT_DEFAULT_K1_NAME                "__k1"
#define OUTPUT_DEFAULT_K2_NAME                "__k2"
#define OUTPUT_DEFAULT_K3_NAME                "__k3"
#define OUTPUT_DEFAULT_A_NAME                 "__a"

#define OUTPUT_DEFAULT_HSQ_NAME               "__Hsq"
#define OUTPUT_DEFAULT_EPS_NAME               "__eps"

#define OUTPUT_DEFAULT_ONEINDEX_NAME          "__1index"
#define OUTPUT_DEFAULT_TWOINDEX_NAME          "__2index"
#define OUTPUT_DEFAULT_THREEINDEX_NAME        "__3index"

#define OUTPUT_DEFAULT_STEPPER_STATE_NAME     "double"

#define OUTPUT_TEMPORARY_POOL_START           "-- START -- temporary pool"
#define OUTPUT_TEMPORARY_POOL_END             "-- END -- temporary pool"
#define OUTPUT_TEMPORARY_POOL_SEQUENCE        "sequence"
#define OUTPUT_KERNEL_LOCATION                "-- VEXCL KERNEL INSERTION"

#define OUTPUT_VEXCL_KERNEL_PRE               ", \""
#define OUTPUT_VEXCL_KERNEL_POST              "\""
#define OUTPUT_VEXCL_UNKNOWN_KERNEL           (OUTPUT_VEXCL_KERNEL_PRE "<UNKNOWN_KERNEL_NAME>" OUTPUT_VEXCL_KERNEL_POST)

#define OUTPUT_VEXCL_KERNEL_LINE_PRE          "\""
#define OUTPUT_VEXCL_KERNEL_LINE_POST         "\\n\""

#define OUTPUT_OPENCL_DEFAULT_LABEL           "idx"

#endif //__msg_H_
