//
// Created by David Seery on 12/06/2013.
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

#ifndef CPPTRANSPORT_MSG_EN_H
#define CPPTRANSPORT_MSG_EN_H

// Messages

constexpr auto ERROR_NO_ERROR_HANDLER                = "Internal error: no registered error handler";
constexpr auto ERROR_NO_WARNING_HANDLER              = "Internal error: no registered warning handler";
constexpr auto ERROR_NOT_FULL_CONTEXT                = "Internal error: attempt to unpack error context without full context data";
constexpr auto ERROR_NO_STACK_REGISTERED             = "Internal error: attempt to unpack error context with no filestack data";

constexpr auto ERROR_MESSAGE_AT_LINE                 = "at line";
constexpr auto ERROR_MESSAGE_WRAP_PAD                = "  ";
constexpr auto OUTPUT_STACK_OF                       = "of";
constexpr auto OUTPUT_STACK_WRAP_PAD                 = "  ";
constexpr auto OUTPUT_STACK_INCLUDED_FROM            = "included from line";
constexpr auto OUTPUT_STACK_OF_FILE                  = "of file";

constexpr auto WARNING_TOKEN                         = "warning: ";
constexpr auto ERROR_TOKEN                           = "error: ";
constexpr auto FATAL_TOKEN                           = "fatal: ";

constexpr auto NOTIFY_PATH_INCLUDES_TEMPLATES        = "Note: search path includes leaf 'templates'";

constexpr auto WARNING_PARSING_FAILED                = "Failed to parse file";
constexpr auto WARNING_VALIDATION_ERRORS             = "The following validation errors occurred:";

constexpr auto ERROR_MISSING_TEMPLATE                = "Could not find specified template";
constexpr auto ERROR_READING_TEMPLATE                = "Error reading from template file";
constexpr auto ERROR_EXPECTED_EQUALS                 = "Expected '=' symbol";
constexpr auto ERROR_EXPECTED_BACKEND_IDENTIFIER     = "Expected backend identifier";
constexpr auto ERROR_EXPECTED_CPPTRANSPORT_VERSION   = "Expected CppTransport version number in format 201801";
constexpr auto ERROR_EXPECTED_TEMPLATE_TYPE          = "Expected template type specifier";
constexpr auto ERROR_IMPROPER_TEMPLATE_HEADER        = "Improperly formed header line in template";
constexpr auto ERROR_TEMPLATE_TOO_RECENT_A           = "Template requires more recent version of CppTransport (>=";
constexpr auto ERROR_TEMPLATE_TOO_RECENT_B           = "current version";
constexpr auto ERROR_TEMPLATE_BACKEND_A              = "Template";
constexpr auto ERROR_TEMPLATE_BACKEND_B              = "requires unknown backend";
constexpr auto ERROR_TEMPLATE_LAGRANGIAN_A           = "Template is suitable for Lagrangian type";
constexpr auto ERROR_TEMPLATE_LAGRANGIAN_B           = "but model implements Lagrangian type";
constexpr auto WARNING_DUPLICATE_TEMPLATE_BACKEND    = "Duplicate backend identifier";
constexpr auto WARNING_DUPLICATE_TEMPLATE_MINVER     = "Duplicate minimum CppTransport version number";
constexpr auto WARNING_DUPLICATE_TEMPLATE_TYPE       = "Duplicate template type specifier";

constexpr auto ERROR_UNSET_BACKEND_DATA              = "Internal error: attempt to read from unset backend_data field";

constexpr auto WARNING_NO_REQUIRED_VERSION           = "No minimum version of CppTransport was specified, so some new features may be disabled";
constexpr auto ERROR_REQUIRED_VERSION_TOO_HIGH_A     = "Requires more recent version of CppTransport (requires";
constexpr auto ERROR_REQUIRED_VERSION_TOO_HIGH_B     = ", current version";
constexpr auto ERROR_REQUIRED_VERSION_TOO_HIGH_C     = ")";

constexpr auto ERROR_OPEN_TOPLEVEL                   = "Could not open top-level file";
constexpr auto ERROR_INCLUDE_FILE                    = "Could not open included file";
constexpr auto ERROR_INCLUDE_DIRECTIVE               = "Expected filename following #include directive; received";

constexpr auto ERROR_UNKNOWN_DIRECTIVE               = "Unknown directive";

constexpr auto ERROR_CPP_BUFFER_WRITE                = "Error opening output file";

constexpr auto ERROR_CSE_POWER_ARGUMENTS             = "Unexpected number of arguments to pow() during common subexpression elimination";

constexpr auto ERROR_NO_CORE_TEMPLATE                = "No core template specified";
constexpr auto ERROR_NO_IMPLEMENTATION_TEMPLATE      = "No implementation template specified";
constexpr auto ERROR_NO_BACKGROUND_STEPPER_BLOCK     = "No background stepper block specified";
constexpr auto ERROR_NO_PERTURBATIONS_STEPPER_BLOCK  = "No perturbations stepper block specified";
constexpr auto ERROR_NO_MODEL_BLOCK                  = "No model block specified";
constexpr auto ERROR_NO_POTENTIAL                    = "Model specification requires a potential";
constexpr auto ERROR_NO_METRIC                       = "Model specification requires a field-space metric";

constexpr auto WARNING_VEXCL_STEPPER_IGNORED_A       = "Using stepper type";
constexpr auto WARNING_VEXCL_STEPPER_IGNORED_B       = "; VexCL backend ignores stepper specification";

constexpr auto ERROR_UNKNOWN_LEXEME                  = "Unknown lexeme type";
constexpr auto ERROR_UNEXPECTED_END_OF_LEXEMES       = "fatal: Unexpected end of lexeme list";

constexpr auto ERROR_BUFFER_ACCESS_OUT_OF_RANGE      = "Out-of-range subscript in lexeme buffer";

constexpr auto ERROR_UNRECOGNIZED_NUMBER             = "Unrecognized numeric literal";
constexpr auto ERROR_UNRECOGNIZED_SYMBOL             = "Unrecognized character";
constexpr auto ERROR_EXPECTED_CLOSE_QUOTE            = "Unterminated string literal";
constexpr auto ERROR_EXPECTED_ELLIPSIS               = "Expected '...', not '..'";

constexpr auto ERROR_SYMBOL_EXISTS                   = "Redefinition of existing symbol";
constexpr auto NOTIFY_DUPLICATE_SYMBOL_DEFN_WAS      = "Note: location of original definition for symbol";
constexpr auto ERROR_IDENTIFIER_LOOKUP               = "Lookup of identifier name failed";
constexpr auto ERROR_STRING_LOOKUP                   = "Lookup of string literal value failed";
constexpr auto ERROR_INTEGER_LOOKUP                  = "Lookup of integer literal value failed";
constexpr auto ERROR_DECIMAL_LOOKUP                  = "Lookup of decimal literal value failed";
constexpr auto ERROR_UNKNOWN_IDENTIFIER              = "Undeclared identifier";

constexpr auto ERROR_AUTHOR_EXISTS                   = "Redeclaration of existing author";
constexpr auto NOTIFY_DUPLICATE_AUTHOR_WAS           = "Note: location of original declaration for author";

constexpr auto NOTIFY_DUPLICATION_DECLARATION_WAS    = "Note: location of original declaration was";
constexpr auto NOTIFY_DUPLICATION_DEFINITION_WAS     = "Note: location of original definition was";

constexpr auto ERROR_MODEL_REDECLARATION             = "Redeclaration of model block";
constexpr auto ERROR_NAME_REDECLARATION              = "Redeclaration of name metadata";
constexpr auto ERROR_LICENSE_REDECLARATION           = "Redeclaration of license metadata";
constexpr auto ERROR_REVISION_DECLARATION            = "Redeclaration of revision metadata";
constexpr auto ERROR_CITEGUIDE_REDECLARATION         = "Redeclaration of citation guidance metadata";
constexpr auto ERROR_URLS_REDECLARATION              = "Redeclaration of URL-list metadata";
constexpr auto ERROR_REFERENCES_REDECLARATION        = "Redeclaration of reference metadata";
constexpr auto ERROR_DESCRIPTION_REDECLARATION       = "Redeclaration of description metadata";

constexpr auto ERROR_IMPLEMENTATION_REDECLARATION    = "Redeclaration of implementation template";
constexpr auto ERROR_CORE_REDECLARATION              = "Redeclaration of core template";
constexpr auto ERROR_SAMPLING_REDECLARATION          = "Redeclaration of sampling bool";
constexpr auto ERROR_SAMPLING_TEMPLATE_REDECLARATION = "Redeclaration of sampling template file";
constexpr auto ERROR_AUTHORNAME_REDECLARATION        = "Redeclaration of author name";
constexpr auto ERROR_EMAIL_REDECLARATION             = "Redeclaration of email address (only one address per author is allowed)";
constexpr auto ERROR_INSTITUTE_REDECLARATION         = "Redeclaration of institutional affiliation (only one affiliation per author is allowed)";
constexpr auto ERROR_LATEX_REDECLARATION             = "Redeclaration of 'latex' attribute";
constexpr auto ERROR_VALUE_REDECLARATION             = "Redeclaration of 'value' attribute";
constexpr auto ERROR_PRIOR_REDECLARATION             = "Redeclaration of 'prior' attribute";
constexpr auto ERROR_DERIVVALUE_REDECLARATION         = "Redeclaration of 'deriv_value' attribute";
constexpr auto ERROR_DERIVPRIOR_REDECLARATION         = "Redeclaration of 'deriv_prior' attribute";
constexpr auto ERROR_STEPPER_REDECLARATION           = "Redeclaration of stepper 'name' attribute";
constexpr auto ERROR_ABSERR_REDECLARATION            = "Redeclaration of stepper 'abserr' attribute";
constexpr auto ERROR_RELERR_REDECLARATION            = "Redeclaration of stepper 'relerr' attribute";
constexpr auto ERROR_STEPSIZE_REDECLARATION          = "Redeclaration of stepper 'stepsize' attribute";
constexpr auto ERROR_BACKGROUND_REDECLARATION        = "Redeclaration of background stepper block";
constexpr auto ERROR_PERTURBATIONS_REDECLARATION     = "Redeclaration of perturbations stepper block";

constexpr auto ERROR_METRIC_REQUIRES_NONTRIVIAL      = "To assign a field-space metric, the Lagrangian type must be 'nontrivial_metric'";
constexpr auto ERROR_METRIC_SETTING_WAS              = "Lagrangian type was set here:";

constexpr auto ERROR_LAGRANGIAN_TYPE_REDECLARATION   = "Redeclaration of Lagrangian type";
constexpr auto ERROR_POTENTIAL_REDECLARATION         = "Redeclaration of potential";
constexpr auto ERROR_METRIC_REDECLARATION            = "Redeclaration of field-space metric";

constexpr auto ERROR_REQUIRED_VERSION_REDECLARATION  = "Redeclaration of minimum required version";

constexpr auto ERROR_MODEL_SPECIFICATION_STARTED     = "Model properties must be declared before the model is specified";
constexpr auto WARNING_SPECIFICATION_START_WAS       = "Model specification began here:";

constexpr auto ERROR_ASSIGNMENT_ITERATOR_NO_PARENT   = "Internal error: increment or decrement of unattached assignment set iterator";
constexpr auto ERROR_ASSIGNMENT_ITERATOR_DECREMENT   = "Internal error: attempt to decrement assignment iterator before first element";
constexpr auto ERROR_ASSIGNMENT_ITERATOR_INCREMENT   = "Internal error: attempt to increment assignment iterator after last element";

constexpr auto ERROR_TOKENIZE_NO_MACRO_MATCH         = "Internal error: unexpectedly failed to match a replacement rule";
constexpr auto ERROR_TOKENIZE_UNEXPECTED_LIST        = "Did not expect index list following simple replacement rule";
constexpr auto ERROR_TOKENIZE_SKIPPING               = "skipping";

constexpr auto ERROR_TOKENIZE_TOO_MANY_ERRORS        = "Too many errors from this token; further output will be suppressed";

constexpr auto ERROR_NO_PRE_MAP                      = "Internal error: Data to perform replacement rule expansion was not correctly pre-mapped";

constexpr auto ERROR_INCOMPATIBLE_UNROLL             = "Conflicting unroll/roll-up requirements";
constexpr auto NOTIFY_CONFLICT_RULE_FORCES_UNROLL    = "Note: replacement rule forces unroll";
constexpr auto NOTIFY_CONFLICT_RULE_PREVENTS_UNROLL  = "Note: replacement rule forces roll-up";
constexpr auto ERROR_LHS_RHS_INCOMPATIBLE_UNROLL     = "Left-hand side and right-hand side have incompatible unrolling requirements";
constexpr auto ERROR_PREVENT_INCOMPATIBLE_FAST       = "Macro preventing unroll is incompatible with --fast switch";
constexpr auto NOTIFY_LHS_RULE_FORCES_UNROLL         = "Note: left-hand side replacement rule prevents roll-up";
constexpr auto NOTIFY_RHS_RULE_FORCES_UNROLL         = "Note: right-hand side replacement rule prevents roll-up";
constexpr auto NOTIFY_RULE_FORCES_UNROLL             = "Note: replacement rule prevents roll-up";
constexpr auto NOTIFY_LHS_RULE_PREVENTS_UNROLL       = "Note: left-hand side replacement rule forces roll-up";
constexpr auto NOTIFY_RHS_RULE_PREVENTS_UNROLL       = "Note: right-hand side replacement rule forces roll-up";
constexpr auto NOTIFY_RULE_PREVENTS_UNROLL           = "Note: replacement rule forces roll-up";
constexpr auto WARN_POLICY_WOULD_UNROLL              = "Unroll/roll-up policy violation; policy would unroll, but roll-up forced";
constexpr auto WARN_POLICY_WOULD_ROLLUP              = "Unroll/roll-up policy violation; policy would roll-up, but unroll forced";
constexpr auto WARN_EXPLICIT_FORCE_IGNORED           = "Ignored explicit force unroll specifier because rule cannot be unrolled";
constexpr auto WARN_EXPLICIT_PREVENT_IGNORED         = "Ignored explicit force roll-up specifier because rule cannot be rolled up";
constexpr auto WARN_EXPLICIT_FORCE_FOLLOWS_PREVENT   = "Explicit force unroll specifier conflicts with earlier explicit force roll-up specifier";
constexpr auto WARN_EXPLICIT_FORCE_FOLLOWS_FORCE     = "Duplicate force unroll specifier is redundant";
constexpr auto WARN_EXPLICIT_PREVENT_FOLLOWS_PREVENT = "Duplicate force roll-up specifier is redundant";
constexpr auto WARN_EXPLICIT_PREVENT_FOLLOWS_FORCE   = "Explicit force roll-up specifier conflicts with earlier explicit force unroll specifier";
constexpr auto NOTIFY_ORIGINAL_EXPLICIT_WAS          = "Note: Location of earlier explicit specifier was";

constexpr auto ERROR_MISSING_INDEX_ASSIGNMENT        = "Internal error: missing index assignment";

constexpr auto ERROR_EXPECTED_OPEN_INDEX_LIST        = "Expected index list beginning with '[' for indexed rule";
constexpr auto ERROR_EXPECTED_INDEX_LABEL_A          = "Expected index label for indexed rule";
constexpr auto ERROR_EXPECTED_INDEX_LABEL_B          = "received";
constexpr auto ERROR_EXPECTED_CLOSE_INDEX_LIST       = "Expected close of index list ']' for indexed rule";
constexpr auto ERROR_EXPECTED_OPEN_ARGUMENT_LIST     = "Missing open of argument list '{' for rule";
constexpr auto ERROR_EXPECTED_CLOSE_ARGUMENT_LIST    = "Missing close of argument list '}' for rule";
constexpr auto ERROR_EXPECTED_CLOSE_ARGUMENT_QUOTE   = "Missing closing quotation '\"' for rule";
constexpr auto ERROR_UNEXPECTED_COMMA                = "Unexpected argument separator ',' for rule";
constexpr auto ERROR_EXPECTED_COMMA                  = "Expected argument separator ',' for rule";
constexpr auto ERROR_MACRO_WRONG_ARGUMENT_COUNT      = "Incorrect number of arguments for rule";
constexpr auto ERROR_DIRECTIVE_WRONG_ARGUMENT_COUNT  = "Incorrect number of arguments for directive";
constexpr auto ERROR_EXPECTED_ARGUMENT_COUNT         = "expected";
constexpr auto ERROR_RECEIVED_ARGUMENT_COUNT         = "received";
constexpr auto ERROR_MACRO_WRONG_INDEX_COUNT         = "Incorrect number of indices for replacement rule";
constexpr auto ERROR_DIRECTIVE_WRONG_INDEX_COUNT     = "Incorrect number of indices for directive";
constexpr auto ERROR_EXPECTED_INDEX_COUNT            = "expected";
constexpr auto ERROR_RECEIVED_INDEX_COUNT            = "received";

constexpr auto ERROR_WRONG_INDEX_CLASS               = "Index type mismatch for";
constexpr auto ERROR_WRONG_INDEX_LABEL               = "index";
constexpr auto ERROR_WRONG_INDEX_EXPECTED            = "expected type";

constexpr auto ERROR_RULE_AFTER_DIRECTIVE            = "Directives can not be mixed with other rules or index literals";
constexpr auto ERROR_DIRECTIVE_AFTER_RULE            = "Directives can not be mixed with other rules or index literals";
constexpr auto ERROR_DIRECTIVE_ON_RHS                = "A line containing a directive should not have a right-hand side";

constexpr auto ERROR_INDEX_LITERAL_NO_KERNEL         = "Line ended before kernel letter for index literal";
constexpr auto ERROR_INDEX_VALIDATION_FAIL           = "Mismatched index literal: expected";
constexpr auto NOTIFY_INDEX_DECLARATION_WAS          = "Note: location of original index declaration was";
constexpr auto ERROR_INDEX_NOT_VALID                 = "Use of undeclared index literal";
constexpr auto ERROR_NONTRIVIAL_REQUIRES_VARIANCE    = "Variance must be assigned when using a nontrival field-space metric: assuming index is contravariant";
constexpr auto NOTIFY_CANONICAL_IGNORES_VARIANCE     = "Note: variance specifiers are ignored in canonical models";
constexpr auto NOTIFY_PARAMETER_VARIANCE_IGNORED     = "Note: variance specifier ignored for parameter-type index";

constexpr auto ERROR_INDEX_LITERAL_REASSIGN          = "Internal error: missing database entry during index literal reassignment, for index";

constexpr auto ERROR_FIELD_INDICES_ARE_CONTRAVARIANT = "Field indices must be contravariant";
constexpr auto ERROR_COORD_INDICES_ARE_CONTRAVARIANT = "Coordinate indices must be contravariant";

constexpr auto ERROR_MISSING_LHS                     = "Missing left-hand side in temporary template ";
constexpr auto ERROR_MISSING_RHS                     = "Missing right-hand side in temporary template ";

constexpr auto ERROR_MACRO_LHS_RHS_MISMATCH          = "Mismatched index class on left- and right-hand sides";

constexpr auto ERROR_INDEX_DATABASE_EMPLACE_FAIL     = "Internal error: emplacement in index database failed";
constexpr auto ERROR_INDEX_DATABASE_OUT_OF_RANGE     = "Internal error: out of range access in index database";

constexpr auto ERROR_REPLACEMENT_RULE_INDEX_COUNT    = "Internal error: number of index classes provided to replacement rule constructor does not match declared number of indices";
constexpr auto ERROR_DIRECTIVE_RULE_INDEX_COUNT      = "Internal error: number of index classes provided to directive constructor does not match declared number of indices";
constexpr auto ERROR_REPLACEMENT_RULE_ZERO_INDICES   = "Internal error: number of indices for an indexed replacement rule cannot be zero";

constexpr auto ERROR_DN_DOES_NOT_ROLL                = "Internal error: attempt to roll-up a delta-N rule";
constexpr auto ERROR_INCONSISTENT_LAMBDA_MAP         = "Internal error: inconsistent lambda map size";

constexpr auto ERROR_FILESTACK_EMPTY                 = "Internal error: filestack empty when accessing top element";
constexpr auto ERROR_FILESTACK_TOO_SHORT             = "Internal error: filestack empty when attempting to pop";

constexpr auto ERROR_CURRENT_LINE_EMPTY              = "Internal error: current line empty in lexfile";

constexpr auto ERROR_OUT_OF_BOUNDS_CSE_MAP           = "Internal error: out of bounds subscript in CSE map";

constexpr auto WARNING_TEMPORARY_NO_TAG_SET          = "End of input reached, but no location set for temporary buffer. "
                                                       "This is most likely a template bug, and the translator output is unlikely to compile";

constexpr auto WARNING_HEX_CONVERSION_A              = "Leading '0x' in ";
constexpr auto WARNING_HEX_CONVERSION_B              = "indicates hex, but did not convert";
constexpr auto WARNING_OCTAL_CONVERSION_A            = "Leading '0' in ";
constexpr auto WARNING_OCTAL_CONVERSION_B            = "indicates octal, but did not convert";

constexpr auto WARNING_RECURSION_DEPTH               = "Maximum depth of recursive replacement rule expansion exceeded (max";

constexpr auto WARNING_LEXEME_KEYWORD                = "Internal error: attempt to get keyword type for non-keyword lexeme";
constexpr auto WARNING_LEXEME_SYMBOL                 = "Internal error: attempt to get symbol type for non-symbol lexeme";
constexpr auto WARNING_LEXEME_INTEGER                = "Internal error: attempt to get integer value for non-integer lexeme";
constexpr auto WARNING_LEXEME_DECIMAL                = "Internal error: attempt to get decimal value for non-decimal lexeme";
constexpr auto WARNING_LEXEME_STRING                 = "Internal error: attempt to get string value for non-string lexeme";
constexpr auto WARNING_LEXEME_IDENTIFIER             = "Internal error: attempt to get identifier value for non-identifier lexeme";

constexpr auto RESOURCE_SET_PARAMETERS               = "parameters resource set to";
constexpr auto RESOURCE_SET_COORDINATES              = "coordinates resource set to";
constexpr auto RESOURCE_SET_DV                       = "V,i resource set to";
constexpr auto RESOURCE_SET_DDV                      = "V,ij resource set to";
constexpr auto RESOURCE_SET_DDDV                     = "V,ijk resource set to";
constexpr auto RESOURCE_SET_CONNEXION                = "connexion resource set to";
constexpr auto RESOURCE_SET_METRIC                   = "metric resource set to";
constexpr auto RESOURCE_SET_METRIC_INVERSE           = "inverse metric resource set to";
constexpr auto RESOURCE_SET_RIEMANN_A2               = "Riemann_A2 resource set to";
constexpr auto RESOURCE_SET_RIEMANN_A3               = "Riemann_A3 resource set to";
constexpr auto RESOURCE_SET_RIEMANN_B3               = "Riemann_B3 resource set to";

constexpr auto RESOURCE_SET_PHASE_FLATTEN            = "phase-space flattener set to";
constexpr auto RESOURCE_SET_FIELD_FLATTEN            = "field-space flattener set to";

constexpr auto RESOURCE_RELEASE_FLATTENERS           = "release flatteners";
constexpr auto RESOURCE_RELEASE                      = "release resources";

constexpr auto RESOURCE_SET_WORKING_TYPE             = "working type set to";
constexpr auto RESOURCE_RELEASE_WORKING_TYPE         = "release working type";

constexpr auto NOTIFY_RESOURCE_REDECLARATION         = "Note: redeclaration of resource without prior release";
constexpr auto NOTIFY_RESOURCE_DECLARATION_WAS       = "Note: location of original declaration was";
constexpr auto ERROR_RESOURCE_LABEL_IN_USE           = "Resource label already in use for an index assignment with different variance";
constexpr auto NOTIFY_RESOURCE_DECLARED_NOT_USED     = "Note: resource label declared but not used; consider removal as a performance optimization";

constexpr auto DIRECTIVE_SET_MACRO_A                 = "set replacement rule";
constexpr auto DIRECTIVE_SET_MACRO_B                 = "to";

constexpr auto ERROR_DIRECTIVE_SET                   = "Error while evaluating set directive for";

constexpr auto NOTIFY_DIRECTIVE_NOT_EVALUATED        = "Note: output disabled, so did not evaluate directive";

constexpr auto ERROR_SET_INDEX_DUPLICATE             = "Index list declared to $SET contains duplicated index";

constexpr auto ERROR_SET_REDEFINITION                = "Redefinition of replacement rule";
constexpr auto WARN_PRIOR_REDEFINITION               = "Earlier definition of this rule was here";
constexpr auto ERROR_INDEX_SUBSTITUTION              = "Missing substitution for index";
constexpr auto ERROR_RESOURCE_FAILURE                = "Could not evaluate roll-up due to missing resource";

constexpr auto ERROR_LHS_INDEX_DUPLICATE             = "Left-hand side contains duplicated index";
constexpr auto NOTIFY_RHS_INDEX_SINGLE_OCCURRENCE    = "Note: index occurs only once on right-hand side";
constexpr auto NOTIFY_RHS_INDEX_SINGLE_VARIANCE      = "Note: right-hand side index does not occur in both covariant and contravariant positions";

constexpr auto ERROR_UNPAIRED_ELSE                   = "Unexpected $ELSE without opening $IF";
constexpr auto ERROR_UNPAIRED_ENDIF                  = "Unexpected $ENDIF without opening $IF";
constexpr auto ERROR_DUPLICATE_ELSE                  = "Duplicate $ELSE clause";

constexpr auto WARNING_UNKNOWN_SWITCH                = "Ignored unknown command-line switch";

constexpr auto MESSAGE_EXPRESSION_CACHE_HIT          = "total expression cache hit";
constexpr auto MESSAGE_EXPRESSION_CACHE_HITS         = "total expression cache hits";
constexpr auto MESSAGE_EXPRESSION_CACHE_MISSES       = "misses";
constexpr auto MESSAGE_EXPRESSION_CACHE_QUERY_TIME   = "time spent performing queries";
constexpr auto MESSAGE_EXPRESSION_CACHE_INSERT_TIME  = "inserts";

constexpr auto MESSAGE_LAMBDA_CACHE_HIT              = "lambda cache hit";
constexpr auto MESSAGE_LAMBDA_CACHE_HITS             = "lambda cache hits";
constexpr auto MESSAGE_LAMBDA_CACHE_MISSES           = "misses";
constexpr auto MESSAGE_LAMBDA_CACHE_QUERY_TIME       = "time spent performing queries";
constexpr auto MESSAGE_LAMBDA_CACHE_INSERT_TIME      = "inserts";

constexpr auto MESSAGE_SYMBOLIC_COMPUTE_TIME         = "symbolic computation";
constexpr auto MESSAGE_CSE_TIME                      = "common sub-expression elimination";
constexpr auto MESSAGE_MACRO_TIME                    = "replacement rule expansion took";
constexpr auto MESSAGE_TOKENIZATION_TIME             = "of which time spent tokenizing";

constexpr auto MESSAGE_PROCESSING_COMPLETE_A         = "processed";
constexpr auto MESSAGE_PROCESSING_SINGULAR           = "model";
constexpr auto MESSAGE_PROCESSING_PLURAL             = "models";
constexpr auto MESSAGE_PROCESSING_COMPLETE_B         = "in time";

constexpr auto MESSAGE_TRANSLATING                   = "translating";
constexpr auto MESSAGE_TRANSLATING_TO                = "into";
constexpr auto ANNOTATE_EXPANSION_OF_LINE            = "expansion of template line";

constexpr auto MESSAGE_TRANSLATION_RESULT            = "translation finished with";
constexpr auto MESSAGE_REPLACEMENT_RULE_EXPANSIONS   = "replacement rule expansions";

constexpr auto ERROR_UNKNOWN_STEPPER                 = "Unknown or unimplemented odeint-v2 stepper";
constexpr auto ERROR_UNDEFINED_STEPPER               = "Stepper block not declared";

constexpr auto ERROR_SYMBOL_DATABASE_EMPLACE_FAIL    = "Internal error: emplace to symbol database failed";

constexpr auto ERROR_COMPONENT_DATABASE_INCONSISTENT = "Internal error: metric component database is inconsistent";
constexpr auto ERROR_COMPONENT_REDEFINITION          = "Redefinition of metric component";
constexpr auto ERROR_BAD_INDEX_LABEL                 = "Unknown field-space metric index label";

constexpr auto ERROR_MISMATCHED_TENSOR_INDICES       = "Internal error: comparison of tensor indices with different variance";
constexpr auto ERROR_VARIDX_WITHOUT_VARIANCE         = "Internal error: construction of GiNaC::varidx without variance information";
constexpr auto ERROR_INDEX_MISSING_VARIANCE_DATA     = "Internal error: index missing variance data";

constexpr auto NOTIFY_REPOSITIONING_INDICES_A        = "Note: repositioning";
constexpr auto NOTIFY_REPOSITIONING_INDICES_B1       = "indices on resource with label";
constexpr auto NOTIFY_REPOSITIONING_INDICES_B2       = "index on resource with label";
constexpr auto WARNING_INDEX_POSITIONING_FAILURE     = "Index reposition failed; fallback to direct evaluation. Are suitable metric/inverse metric resources available?";

constexpr auto ERROR_METRIC_NOT_SQUARE               = "Internal error: field-space metric is not a square matrix";
constexpr auto ERROR_METRIC_DIMENSION                = "Internal error: field-space metric/inverse has inconsistent dimension";
constexpr auto ERROR_METRIC_INDICES_ARE_FIELDS       = "Internal error: indices used to construct a metric component should be fields";

constexpr auto ERROR_METRIC_RESOURCE_MIXED_INDICES   = "Metric resource should not have mixed indices";
constexpr auto ERROR_METRIC_RULE_MIXED_INDICES       = "$METRIC should not be used with mixed indices";
constexpr auto ERROR_CONNEXION_INDICES               = "$CONNECTION has incorrect index placement (should be first=up, second,third=down)";

constexpr auto NOTIFY_PARSE_TERMINATED               = "Translation terminated";
constexpr auto NOTIFY_TOO_MANY_ERRORS                = "encountered too many errors";

constexpr auto ERROR_VARIANCE_TENSOR_CACHE_EMPLACE   = "Internal error: failed to emplace tensor in variance_tensor_cache";

constexpr auto ERROR_SPECIES_MAPPING_OVERFLOW        = "Internal error: abstract index mapping overflow";
constexpr auto ERROR_SPECIES_TO_SPECIES_FAIL         = "Internal error: species-to-species map must be applied to a phase-space index";
constexpr auto ERROR_MOMENTUM_TO_SPECIES_FAIL        = "Internal error: momentum-to-species map must be applied to a phase-space index";

constexpr auto ERROR_UNIMPLEMENTED_MATHS_FUNCTION    = "Unimplemented mathematical function";
constexpr auto ERROR_USED_IN_EXPRESSION              = "used in expression";

constexpr auto ERROR_SYMBOL_PREVIOUSLY_COMPLEX       = "symbol was previously declared complex";
constexpr auto ERROR_SYMBOL_PREVIOUSLY_REAL          = "symbol was previously declared real";
constexpr auto ERROR_SYMBOL_RECORD_INSERTION         = "Internal error: emplace to symbol record database failed";

constexpr auto MESSAGE_HOUR_LABEL                    = "h";
constexpr auto MESSAGE_MINUTE_LABEL                  = "m";
constexpr auto MESSAGE_SECOND_LABEL                  = "s";

constexpr auto LABEL_INDEX_CLASS_PARAMETER           = "parameter";
constexpr auto LABEL_INDEX_CLASS_FIELD               = "field";
constexpr auto LABEL_INDEX_CLASS_FULL                = "full phase-space";

#endif //CPPTRANSPORT_MSG_EN_H
