//
// Created by David Seery on 12/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __msg_H_
#define __msg_H_

#define ERROR_MESSAGE_AT_LINE                 "at line"
#define ERROR_MESSAGE_WRAP_PAD                "  "
#define OUTPUT_STACK_OF                       "of"
#define OUTPUT_STACK_WRAP_PAD                 ERROR_MESSAGE_WRAP_PAD
#define OUTPUT_STACK_INCLUDED_FROM            "included from line"
#define OUTPUT_STACK_OF_FILE                  "of file"

#define ERROR_OPEN_TOPLEVEL                   "Could not open top-level file"
#define ERROR_INCLUDE_FILE                    "Could not open included file"
#define ERROR_INCLUDE_DIRECTIVE               "Expected filename following #include directive"

#define ERROR_MISSING_PATHNAME                "Too few arguments: expected pathname following"

#define ERROR_CPP_BUFFER_WRITE                "Error writing to output file"

#define ERROR_NO_CORE_TEMPLATE                "fatal: No core template specified"
#define ERROR_NO_IMPLEMENTATION_TEMPLATE      "fatal: No implementation template specified"
#define ERROR_MISSING_TEMPLATE                "Could not find specified template"
#define ERROR_READING_TEMPLATE                "Error reading from template file"
#define ERROR_TOKENIZE_TEMPLATE_HEADER        "Could not find properly-formed header in template file"
#define ERROR_TEMPLATE_TOO_RECENT_A           "Template"
#define ERROR_TEMPLATE_TOO_RECENT_B           "requires more recent version of CppTransport (>"
#define ERROR_TEMPLATE_BACKEND_A              "Template"
#define ERROR_TEMPLATE_BACKEND_B              "requires unknown backend"

#define ERROR_VEXCL_NO_STEPPER_STATE          "Fatal: stepper state type specification is mandatory with VexCL backend"
#define WARNING_VEXCL_STEPPER_IGNORED         "Using stepper type 'runge_kutta_dopri5'; VexCL backend ignores stepper specification"

#define ERROR_UNRECOGNIZED_NUMBER             "Unrecognized numeric literal"
#define ERROR_UNRECOGNIZED_SYMBOL             "Unrecognized character"
#define ERROR_EXPECTED_CLOSE_QUOTE            "Unterminated string literal"
#define ERROR_EXPECTED_ELLIPSIS               "Expected '...', not '..'"

#define ERROR_SYMBOL_EXISTS                   "Redefinition of existing symbol"
#define ERROR_IDENTIFIER_LOOKUP               "Lookup of identifier name failed"
#define ERROR_STRING_LOOKUP                   "Lookup of string literal value failed"
#define ERROR_INTEGER_LOOKUP                  "Lookup of integer literal value failed"
#define ERROR_DECIMAL_LOOKUP                  "Lookup of decimal literal value failed"
#define ERROR_UNKNOWN_IDENTIFIER              "Undeclared identifier"

#define ERROR_EXPECTED_INDEX                  "Expected index label following prefix"
#define ERROR_TOO_FEW_INDICES                 "Too few indices supplied to summation-convention object"
#define ERROR_EXPECTED_OPEN_IDXL              "Expected index list beginning with '[' for indexed macro"
#define ERROR_EXPECTED_CLOSE_IDXL             "Expected close of index list ']' for indexed macro"
#define ERROR_DUPLICATE_INDEX                 "Duplicate declaration of lvalue summation convention index"
#define ERROR_EXPECTED_CLOSE_ARGL             "Expected close of argument list '}'"
#define ERROR_WRONG_ARG_NUM                   "Unexpected number of arguments to macro"
#define ERROR_WRONG_ARG_NUM_EXPECT            "expected"

#define ERROR_MISSING_LHS                     "Missing left-hand side in temporary template "
#define ERROR_MISSING_RHS                     "Missing right-hand side in temporary template "

#define ERROR_NO_BUFFER_REGISTERED            "Internal error: TemporaryPool macro package in use, but no buffer registered"
#define ERROR_NO_MACROS_REGISTERED            "Internal error: TemporaryPool macro package in use, but no macro set registered"

#define ERROR_FILESTACK_EMPTY                 "Internal error: filestack empty when setting line number"
#define ERROR_FILESTACK_TOO_SHORT             "Internal error: filestack empty when attempting to pop"
#define ERROR_FILESTACK_LOCKED                "Internal error: attempt to modify locked filestack"

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

#define MESSAGE_PROCESSING_COMPLETE_A         "Processed"
#define MESSAGE_PROCESSING_SINGULAR           "file"
#define MESSAGE_PROCESSING_PLURAL             "files"
#define MESSAGE_PROCESSING_COMPLETE_B         "in time"

#define ERROR_UNKNOWN_STEPPER                 "Unknown or unimplemented odeint-v2 stepper"

// output strings

#define BACKEND_MACRO_PREFIX                  "$$__"
#define BACKEND_LINE_SPLIT                    "$$//"

#define OUTPUT_CPPTRANSPORT_TAG               "__CPPTRANSPORT"
#define OUTPUT_TIME_FORMAT                    "%X on %d %m %Y"
#define OUTPUT_DEFAULT_POOL_TEMPLATE          "auto $1 = $2;"

#define OUTPUT_DEFAULT_CPP_KERNEL_NAME        "__temp"

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

#define OUTPUT_DEFAULT_STEPPER_STATE_NAME     "double"

#endif //__msg_H_
