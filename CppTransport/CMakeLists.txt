CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(CppTransport)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)
INCLUDE(BuildCatch)
INCLUDE(BuildGiNaC)
INCLUDE(BuildJsonCPP)
INCLUDE(BuildOdeint)
INCLUDE(BuildSPLINTER)
INCLUDE(CompilerFlags)

# detect toolchain and set compiler flags appropriately
SET_COMPILER_FLAGS()

# RESOLVE DEPENDENCIES

# find Bison to produce parser
FIND_PACKAGE(BISON 3.0 REQUIRED)

# some libraries require system-level installation (MPI, Boost.Mpi) and are things that
# we shouldn't try to build ourseves. We insist that these are already available
# at configure time

# find MPI library
FIND_PACKAGE(MPI REQUIRED)

# find required Boost libraries
# minimum version is 1.56 to get support for Boost.Optional with move-only types
FIND_PACKAGE(Boost 1.56 REQUIRED COMPONENTS log system filesystem random timer date_time mpi thread serialization program_options regex)

# find OpenSSL libraries
FIND_PACKAGE(OpenSSL REQUIRED)

# GiNaC we allow to be pre-installed or not. If it isn't already available then we
# fall back to building a copy ourselves

# find GiNaC library unless explicitly prevented by the user
IF(NOT FORCE_BUILD_GINAC)
  FIND_PACKAGE(GiNaC REQUIRED)
ENDIF()

# handle non-standard external dependencies
ADD_CUSTOM_TARGET(DEPS ALL)
SET(EXTERNAL_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/thirdparty/dist")

BUILD_JSONCPP(${CMAKE_BINARY_DIR}/thirdparty/jsoncpp ${EXTERNAL_INSTALL_PREFIX})
BUILD_SPLINTER(${CMAKE_BINARY_DIR}/thirdparty/SPLINTER ${EXTERNAL_INSTALL_PREFIX})
BUILD_CATCH(${CMAKE_BINARY_DIR}/thirdparty/Catch ${EXTERNAL_INSTALL_PREFIX})
BUILD_ODEINT(${CMAKE_BINARY_DIR}/thirdparty/odeint-v2)

IF(NOT GINAC_FOUND)
  # GiNaC was not found, so attempt to build it from source
  BUILD_CLN(${CMAKE_BINARY_DIR}/thirdparty/CLN ${EXTERNAL_INSTALL_PREFIX})
  BUILD_GINAC(${CMAKE_BINARY_DIR}/thirdparty/GiNaC ${CLN_INCLUDE_DIRS} ${CLN_LIBRARY_DIRS} ${CLN_LIBRARIES} ${EXTERNAL_INSTALL_PREFIX})
ENDIF()


# configure build_data.h
STRING(TIMESTAMP CONFIG_TIMESTAMP)
CONFIGURE_FILE(configure_assets/build_data.h.in ${CMAKE_BINARY_DIR}/build_data.h)
# provision a second version of build_data.h for use in test-canonical/test-nontrivial-metric
CONFIGURE_FILE(configure_assets/build_data.h.in ${CMAKE_BINARY_DIR}/transport-runtime/build_data.h)

# build parser
BISON_TARGET(parser translator/parser/bison/y_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/y_parser.cpp)


ADD_EXECUTABLE(CppTransport
  translator/main.cpp
  translator/backends/infrastructure/file_handling/backend_data.cpp
  translator/backends/infrastructure/file_handling/buffer.cpp
  translator/backends/infrastructure/file_handling/output_stack.cpp
  translator/backends/infrastructure/file_handling/translation_unit.cpp
  translator/backends/infrastructure/file_handling/translator.cpp
  translator/backends/infrastructure/file_handling/translator_data.cpp
  translator/backends/infrastructure/file_handling/lexstream_data.cpp
  translator/backends/infrastructure/language_concepts/cse.cpp
  translator/backends/infrastructure/language_concepts/cse_map.cpp
  translator/backends/infrastructure/language_concepts/lambdas.cpp
  translator/backends/infrastructure/language_concepts/lambda_manager.cpp
  translator/backends/infrastructure/macro_substitution/index_assignment.cpp
  translator/backends/infrastructure/macro_substitution/macro.cpp
  translator/backends/infrastructure/macro_substitution/token_list.cpp
  translator/backends/infrastructure/macro_substitution/macro_tokens.cpp
  translator/backends/infrastructure/macro_substitution/abstract_index.cpp
  translator/backends/infrastructure/macro_substitution/index_literal.cpp
  translator/backends/infrastructure/macro_substitution/index_class.cpp
  translator/backends/infrastructure/replacement_rules/cse_map_field_indices.cpp
  translator/backends/infrastructure/replacement_rules/cse_map_phase_indices.cpp
  translator/backends/infrastructure/replacement_rules/package_group.cpp
  translator/backends/infrastructure/replacement_rules/package_group_factory.cpp
  translator/backends/infrastructure/replacement_rules/replacement_rule_definitions.cpp
  translator/backends/infrastructure/replacement_rules/replacement_rule_package.cpp
  translator/backends/infrastructure/replacement_rules/directive_definitions.cpp
  translator/backends/infrastructure/replacement_rules/directive_package.cpp
  translator/backends/languages/cpp/core/cpp_group.cpp
  translator/backends/languages/cpp/core/cpp_steppers.cpp
  translator/backends/languages/cpp/vexcl/vexcl_group.cpp
  translator/backends/languages/cpp/vexcl/vexcl_kernels.cpp
  translator/backends/languages/cpp/vexcl/vexcl_steppers.cpp
  translator/backends/languages/cpp/cpp_cse.cpp
  translator/backends/languages/cpp/cpp_printer.cpp
  translator/backends/languages/cuda/cuda_group.cpp
  translator/backends/languages/cuda/cuda_printer.cpp
  translator/backends/languages/opencl/opencl_group.cpp
  translator/backends/languages/opencl/opencl_printer.cpp
  translator/backends/languages/shared/C_style_printer.cpp
  translator/backends/languages/shared/kernel_argument_macros.cpp
  translator/backends/shared/curvature_tensors.cpp
  translator/backends/shared/directives.cpp
  translator/backends/shared/flow_tensors.cpp
  translator/backends/shared/fundamental.cpp
  translator/backends/shared/gauge_xfm.cpp
  translator/backends/shared/lagrangian_tensors.cpp
  translator/backends/shared/resources.cpp
  translator/backends/shared/temporary_pool.cpp
  translator/backends/shared/utensors.cpp
  ${BISON_parser_OUTPUTS}
  translator/parser/descriptor/blocks/lagrangian_block.cpp
  translator/parser/descriptor/blocks/metadata_block.cpp
  translator/parser/descriptor/blocks/templates_block.cpp
  translator/parser/descriptor/blocks/misc_block.cpp
  translator/parser/descriptor/symbols/declarations.cpp
  translator/parser/descriptor/model_descriptor.cpp
  translator/parser/driver/expression_tree_driver.cpp
  translator/parser/driver/lagrangian_driver.cpp
  translator/parser/driver/metadata_driver.cpp
  translator/parser/driver/templates_driver.cpp
  translator/parser/driver/misc_driver.cpp
  translator/parser/driver/y_driver.cpp
  translator/parser/semantic-values/attributes.cpp
  translator/parser/semantic-values/author.cpp
  translator/parser/semantic-values/metric.cpp
  translator/parser/semantic-values/stepper.cpp
  translator/parser/semantic-values/string_array.cpp
  translator/parser/semantic-values/subexpr.cpp
  translator/parser/input_stack.cpp
  translator/parser/lexfile.cpp
  translator/parser/y_lexer.cpp
  translator/transport-objects/canonical/resources_detail/potential_cache.cpp
  translator/transport-objects/canonical/tensors/A.cpp
  translator/transport-objects/canonical/tensors/Atilde.cpp
  translator/transport-objects/canonical/tensors/B.cpp
  translator/transport-objects/canonical/tensors/C.cpp
  translator/transport-objects/canonical/tensors/coordinates.cpp
  translator/transport-objects/canonical/tensors/dV.cpp
  translator/transport-objects/canonical/tensors/ddV.cpp
  translator/transport-objects/canonical/tensors/dddV.cpp
  translator/transport-objects/canonical/tensors/dN1.cpp
  translator/transport-objects/canonical/tensors/dN2.cpp
  translator/transport-objects/canonical/tensors/fields.cpp
  translator/transport-objects/canonical/tensors/Hubble.cpp
  translator/transport-objects/canonical/tensors/SR_velocity.cpp
  translator/transport-objects/canonical/tensors/M.cpp
  translator/transport-objects/canonical/tensors/momenta.cpp
  translator/transport-objects/canonical/tensors/u1.cpp
  translator/transport-objects/canonical/tensors/u2.cpp
  translator/transport-objects/canonical/tensors/u3.cpp
  translator/transport-objects/canonical/tensors/zeta1.cpp
  translator/transport-objects/canonical/tensors/zeta2.cpp
  translator/transport-objects/canonical/tensor_factory.cpp
  translator/transport-objects/canonical/resources.cpp
  translator/transport-objects/canonical/resource_manager.cpp
  translator/transport-objects/concepts/tensors/parameters.cpp
  translator/transport-objects/indices/indices.cpp
  translator/transport-objects/indices/index_flatten.cpp
  translator/transport-objects/indices/index_traits.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/connexion_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_dV_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_ddV_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_dddV_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannA2_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannA3_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannB3_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/derivative_symbols.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/potential_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/substitution_map_cache.cpp
  translator/transport-objects/nontrivial-metric/tensors/A.cpp
  translator/transport-objects/nontrivial-metric/tensors/Atilde.cpp
  translator/transport-objects/nontrivial-metric/tensors/B.cpp
  translator/transport-objects/nontrivial-metric/tensors/C.cpp
  translator/transport-objects/nontrivial-metric/tensors/connexion.cpp
  translator/transport-objects/nontrivial-metric/tensors/coordinates.cpp
  translator/transport-objects/nontrivial-metric/tensors/dV.cpp
  translator/transport-objects/nontrivial-metric/tensors/ddV.cpp
  translator/transport-objects/nontrivial-metric/tensors/dddV.cpp
  translator/transport-objects/nontrivial-metric/tensors/dN1.cpp
  translator/transport-objects/nontrivial-metric/tensors/dN2.cpp
  translator/transport-objects/nontrivial-metric/tensors/fields.cpp
  translator/transport-objects/nontrivial-metric/tensors/Hubble.cpp
  translator/transport-objects/nontrivial-metric/tensors/M.cpp
  translator/transport-objects/nontrivial-metric/tensors/metric.cpp
  translator/transport-objects/nontrivial-metric/tensors/metric_inverse.cpp
  translator/transport-objects/nontrivial-metric/tensors/momenta.cpp
  translator/transport-objects/nontrivial-metric/tensors/Riemann_A2.cpp
  translator/transport-objects/nontrivial-metric/tensors/Riemann_A3.cpp
  translator/transport-objects/nontrivial-metric/tensors/Riemann_B3.cpp
  translator/transport-objects/nontrivial-metric/tensors/SR_velocity.cpp
  translator/transport-objects/nontrivial-metric/tensors/u1.cpp
  translator/transport-objects/nontrivial-metric/tensors/u2.cpp
  translator/transport-objects/nontrivial-metric/tensors/u3.cpp
  translator/transport-objects/nontrivial-metric/tensors/zeta1.cpp
  translator/transport-objects/nontrivial-metric/tensors/zeta2.cpp
  translator/transport-objects/nontrivial-metric/metric_resource.cpp
  translator/transport-objects/nontrivial-metric/tensor_factory.cpp
  translator/transport-objects/nontrivial-metric/resources.cpp
  translator/transport-objects/nontrivial-metric/resource_manager.cpp
  translator/transport-objects/nontrivial-metric/curvature_classes.cpp
  translator/transport-objects/shared/cache_detail/cache_tags.cpp
  translator/transport-objects/shared/symbol_factory.cpp
  translator/transport-objects/shared/shared_resources.cpp
  translator/utilities/error.cpp
  translator/utilities/error_context.cpp
  translator/utilities/finder.cpp
  translator/utilities/formatter.cpp
  translator/utilities/ginac_print_indexed.cpp
  translator/utilities/to_printable.cpp
  translator/utilities/local_environment.cpp
  translator/utilities/argument_cache.cpp
  translator/version-policy/version_policy.cpp
  )

ADD_DEPENDENCIES(CppTransport DEPS)

TARGET_LINK_LIBRARIES(CppTransport ${GINAC_LIBRARIES} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
TARGET_COMPILE_OPTIONS(CppTransport PRIVATE -std=c++14)
#SET_TARGET_PROPERTIES(CppTransport PROPERTIES
#  CXX_STANDARD 14
#  CXX_STANDARD_REQUIRED YES
#  CXX_EXTENSIONS NO
#  )

TARGET_INCLUDE_DIRECTORIES(CppTransport PRIVATE
  translator
  translator/backends
  translator/backends/infrastructure
  translator/backends/infrastructure/file_handling
  translator/backends/infrastructure/language_concepts
  translator/backends/infrastructure/macro_substitution
  translator/backends/infrastructure/replacement_rules
  translator/backends/languages
  translator/backends/languages/cpp
  translator/backends/languages/cpp/core
  translator/backends/languages/cpp/vexcl
  translator/backends/languages/cuda
  translator/backends/languages/opencl
  translator/backends/languages/shared
  translator/backends/shared
  translator/instruments
  translator/parser
  translator/parser/descriptor/blocks
  translator/parser/descriptor/symbols
  translator/parser/descriptor
  translator/parser/driver
  translator/parser/semantic-values
  translator/transport-objects
  translator/transport-objects/indices
  translator/transport-objects/concepts/resources_detail
  translator/transport-objects/shared
  translator/utilities
  translator/version-policy
  ${Boost_INCLUDE_DIRS}
  ${GINAC_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )


# pull in test targets
ADD_SUBDIRECTORY(test-canonical EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(test-nontrivial-metric EXCLUDE_FROM_ALL)

ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)


# install targets for CppTransport translator, templates, and runtime system
# note that CppTransport-sendmail has more restrictive permissions since it may contain a password
INSTALL(TARGETS CppTransport DESTINATION bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)
INSTALL(DIRECTORY templates DESTINATION share/cpptransport)
INSTALL(DIRECTORY transport-runtime DESTINATION include/cpptransport)
INSTALL(FILES ${CMAKE_BINARY_DIR}/build_data.h DESTINATION include/cpptransport/transport-runtime)

IF(EMAIL_SMTP_SERVER AND EMAIL_FROM AND EMAIL_PASSWORD)
  CONFIGURE_FILE(configure_assets/CppTransport-sendmail.in ${CMAKE_BINARY_DIR}/CppTransport-sendmail)
  INSTALL(FILES ${CMAKE_BINARY_DIR}/CppTransport-sendmail DESTINATION bin PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
ELSE()
  MESSAGE(WARNING "CppTransport-sendmail script has not been configured. CppTransport will not issue email notifications unless a suitable script is installed manually. If you wish the script to be automatically configured, please re-run CMake and set the EMAIL_SMTP_SERVER, EMAIL_FROM and EMAIL_PASSWORD variables.")
ENDIF()

# install HTML, CSS and JavaScript assets
INSTALL(DIRECTORY thirdparty/bootstrap DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/bootstrap-tab-history DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/jQuery DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/prism DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/DataTables DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY HTML_assets DESTINATION share/cpptransport/HTML)

# emplace JsonCpp, SPLINTER and odeint-v2 assets into runtime system folders
INSTALL(DIRECTORY ${EXTERNAL_INSTALL_PREFIX}/include/json DESTINATION include/cpptransport)
INSTALL(DIRECTORY ${EXTERNAL_INSTALL_PREFIX}/include/SPLINTER DESTINATION include/cpptransport)
INSTALL(DIRECTORY ${ODEINT_INCLUDE_DIRS}/boost DESTINATION include/cpptransport)
INSTALL(DIRECTORY ${EXTERNAL_INSTALL_PREFIX}/lib DESTINATION lib/cpptransport)

# install CMake finder script for all these assets
CONFIGURE_FILE(cmake/FindCppTransport.cmake.in ${CMAKE_BINARY_DIR}/FindCppTransport.cmake @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/FindCppTransport.cmake DESTINATION share/cmake)
INSTALL(FILES cmake/CompilerFlags.cmake DESTINATION share/cmake)


## FILL IN DUMMY TARGETS FOR CLION

SET(TEMPLATES_FILES
  templates/canonical_core.h
  templates/canonical_mpi.h
  templates/nontrivial_metric_core.h
  templates/nontrivial_metric_mpi.h
  )

SET(TEMPLATES_VEXCL_CUDA_FILES
  templates/vexcl-cuda/canonical_vexcl-cuda.h
  templates/vexcl-cuda/threepf.cu
  templates/vexcl-cuda/twopf.cu
  )

SET(TEMPLATES_VEXCL_OPENCL_FILES
  templates/vexcl-opencl/canonical_vexcl-opencl.h
  templates/vexcl-opencl/threepf.cl
  templates/vexcl-opencl/twopf.cl
  )

SET(TOP_LEVEL_FILES
  )

SET(TRANSLATOR_BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES
  translator/backends/infrastructure/file_handling/backend_data.cpp
  translator/backends/infrastructure/file_handling/backend_data.h
  translator/backends/infrastructure/file_handling/buffer.cpp
  translator/backends/infrastructure/file_handling/buffer.h
  translator/backends/infrastructure/file_handling/contexted_value.h
  translator/backends/infrastructure/file_handling/filestack.h
  translator/backends/infrastructure/file_handling/lexstream_data.cpp
  translator/backends/infrastructure/file_handling/lexstream_data.h
  translator/backends/infrastructure/file_handling/output_stack.cpp
  translator/backends/infrastructure/file_handling/output_stack.h
  translator/backends/infrastructure/file_handling/translation_unit.cpp
  translator/backends/infrastructure/file_handling/translation_unit.h
  translator/backends/infrastructure/file_handling/translator.cpp
  translator/backends/infrastructure/file_handling/translator.h
  translator/backends/infrastructure/file_handling/translator_data.cpp
  translator/backends/infrastructure/file_handling/translator_data.h
  )

SET(TRANSLATOR_BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES
  translator/backends/infrastructure/language_concepts/cse.cpp
  translator/backends/infrastructure/language_concepts/cse.h
  translator/backends/infrastructure/language_concepts/cse_map.cpp
  translator/backends/infrastructure/language_concepts/cse_map.h
  translator/backends/infrastructure/language_concepts/lambda_manager.cpp
  translator/backends/infrastructure/language_concepts/lambda_manager.h
  translator/backends/infrastructure/language_concepts/lambdas.cpp
  translator/backends/infrastructure/language_concepts/lambdas.h
  translator/backends/infrastructure/language_concepts/language_printer.h
  )

SET(TRANSLATOR_BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES
  translator/backends/infrastructure/macro_substitution/abstract_index.cpp
  translator/backends/infrastructure/macro_substitution/abstract_index.h
  translator/backends/infrastructure/macro_substitution/index_assignment.cpp
  translator/backends/infrastructure/macro_substitution/index_assignment.h
  translator/backends/infrastructure/macro_substitution/index_class.cpp
  translator/backends/infrastructure/macro_substitution/index_class.h
  translator/backends/infrastructure/macro_substitution/index_database.h
  translator/backends/infrastructure/macro_substitution/macro.cpp
  translator/backends/infrastructure/macro_substitution/macro.h
  translator/backends/infrastructure/macro_substitution/token_list.cpp
  translator/backends/infrastructure/macro_substitution/token_list.h
  translator/backends/infrastructure/macro_substitution/macro_types.h
  translator/backends/infrastructure/macro_substitution/macro_tokens.cpp
  translator/backends/infrastructure/macro_substitution/macro_tokens.h
  translator/backends/infrastructure/macro_substitution/index_literal.cpp
  translator/backends/infrastructure/macro_substitution/index_literal.h
  )

SET(TRANSLATOR_BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES
  translator/backends/infrastructure/replacement_rules/cse_map_field_indices.cpp
  translator/backends/infrastructure/replacement_rules/cse_map_field_indices.h
  translator/backends/infrastructure/replacement_rules/cse_map_phase_indices.cpp
  translator/backends/infrastructure/replacement_rules/cse_map_phase_indices.h
  translator/backends/infrastructure/replacement_rules/package_group.cpp
  translator/backends/infrastructure/replacement_rules/package_group.h
  translator/backends/infrastructure/replacement_rules/package_group_factory.cpp
  translator/backends/infrastructure/replacement_rules/package_group_factory.h
  translator/backends/infrastructure/replacement_rules/replacement_rule_definitions.cpp
  translator/backends/infrastructure/replacement_rules/replacement_rule_definitions.h
  translator/backends/infrastructure/replacement_rules/replacement_rule_package.cpp
  translator/backends/infrastructure/replacement_rules/replacement_rule_package.h
  translator/backends/infrastructure/replacement_rules/directive_definitions.cpp
  translator/backends/infrastructure/replacement_rules/directive_definitions.h
  translator/backends/infrastructure/replacement_rules/rules_common.h
  translator/backends/infrastructure/replacement_rules/directive_package.cpp
  translator/backends/infrastructure/replacement_rules/directive_package.h
  )

SET(TRANSLATOR_BACKENDS_LANGUAGES_CPP_CORE_FILES
  translator/backends/languages/cpp/core/cpp_group.cpp
  translator/backends/languages/cpp/core/cpp_group.h
  translator/backends/languages/cpp/core/cpp_steppers.cpp
  translator/backends/languages/cpp/core/cpp_steppers.h
  )

SET(TRANSLATOR_BACKENDS_LANGUAGES_CPP_FILES
  translator/backends/languages/cpp/cpp_cse.cpp
  translator/backends/languages/cpp/cpp_cse.h
  translator/backends/languages/cpp/cpp_printer.cpp
  translator/backends/languages/cpp/cpp_printer.h
  )

SET(TRANSLATOR_BACKENDS_LANGUAGES_CPP_VEXCL_FILES
  translator/backends/languages/cpp/vexcl/vexcl_group.cpp
  translator/backends/languages/cpp/vexcl/vexcl_group.h
  translator/backends/languages/cpp/vexcl/vexcl_kernels.cpp
  translator/backends/languages/cpp/vexcl/vexcl_kernels.h
  translator/backends/languages/cpp/vexcl/vexcl_steppers.cpp
  translator/backends/languages/cpp/vexcl/vexcl_steppers.h
  )

SET(TRANSLATOR_BACKENDS_LANGUAGES_CUDA_FILES
  translator/backends/languages/cuda/cuda_group.cpp
  translator/backends/languages/cuda/cuda_group.h
  translator/backends/languages/cuda/cuda_printer.cpp
  translator/backends/languages/cuda/cuda_printer.h
  )

SET(TRANSLATOR_BACKENDS_LANGUAGES_OPENCL_FILES
  translator/backends/languages/opencl/opencl_group.cpp
  translator/backends/languages/opencl/opencl_group.h
  translator/backends/languages/opencl/opencl_printer.cpp
  translator/backends/languages/opencl/opencl_printer.h
  )

SET(TRANSLATOR_BACKENDS_LANGUAGES_SHARED_FILES
  translator/backends/languages/shared/C_style_printer.cpp
  translator/backends/languages/shared/C_style_printer.h
  translator/backends/languages/shared/kernel_argument_macros.cpp
  translator/backends/languages/shared/kernel_argument_macros.h
  )

SET(TRANSLATOR_BACKENDS_SHARED_FILES
  translator/backends/shared/curvature_tensors.cpp
  translator/backends/shared/curvature_tensors.h
  translator/backends/shared/directives.cpp
  translator/backends/shared/directives.h
  translator/backends/shared/flow_tensors.cpp
  translator/backends/shared/flow_tensors.h
  translator/backends/shared/fundamental.cpp
  translator/backends/shared/fundamental.h
  translator/backends/shared/gauge_xfm.cpp
  translator/backends/shared/gauge_xfm.h
  translator/backends/shared/lagrangian_tensors.cpp
  translator/backends/shared/lagrangian_tensors.h
  translator/backends/shared/resources.cpp
  translator/backends/shared/resources.h
  translator/backends/shared/temporary_pool.cpp
  translator/backends/shared/temporary_pool.h
  translator/backends/shared/utensors.cpp
  translator/backends/shared/utensors.h
  )

SET(TRANSLATOR_FILES
  translator/core.h
  translator/defaults.h
  translator/main.cpp
  translator/msg_en.h
  translator/switches.h
  translator/disable_warnings.h
  )

SET(TRANSLATOR_INSTRUMENTS_FILES
  translator/instruments/timing_instrument.h
  )

SET(TRANSLATOR_PARSER_BISON_FILES
  translator/parser/bison/y_parser.yy
  )

SET(TRANSLATOR_PARSER_DESCRIPTOR_BLOCKS_FILES
  translator/parser/descriptor/blocks/metadata_block.cpp
  translator/parser/descriptor/blocks/metadata_block.h
  translator/parser/descriptor/blocks/lagrangian_block.cpp
  translator/parser/descriptor/blocks/lagrangian_block.h
  translator/parser/descriptor/blocks/misc_block.cpp
  translator/parser/descriptor/blocks/misc_block.h
  translator/parser/descriptor/blocks/templates_block.cpp
  translator/parser/descriptor/blocks/templates_block.h
  )

SET(TRANSLATOR_PARSER_DESCRIPTOR_SYMBOLS_FILES
  translator/parser/descriptor/symbols/declarations.h
  translator/parser/descriptor/symbols/declarations.cpp
  translator/parser/descriptor/symbols/generic_iterator.h
  translator/parser/descriptor/symbols/symbol_database.h
  translator/parser/descriptor/symbols/symbol_tables.h
  )

SET(TRANSLATOR_PARSER_DESCRIPTOR_FILES
  translator/parser/descriptor/model_settings.h
  translator/parser/descriptor/model_descriptor.cpp
  translator/parser/descriptor/model_descriptor.h
  translator/parser/descriptor/generics_descriptor.h
  )

SET(TRANSLATOR_PARSER_DRIVER_FILES
  translator/parser/driver/generics_driver.h
  translator/parser/driver/expression_tree_driver.cpp
  translator/parser/driver/expression_tree_driver.h
  translator/parser/driver/lagrangian_driver.cpp
  translator/parser/driver/lagrangian_driver.h
  translator/parser/driver/metadata_driver.cpp
  translator/parser/driver/metadata_driver.h
  translator/parser/driver/templates_driver.cpp
  translator/parser/driver/templates_driver.h
  translator/parser/driver/misc_driver.cpp
  translator/parser/driver/misc_driver.h
  translator/parser/driver/y_driver.cpp
  translator/parser/driver/y_driver.h
  translator/parser/driver/parse_error.h
  )

SET(TRANSLATOR_PARSER_SEMANTIC_VALUES_FILES
  translator/parser/semantic-values/stepper.h
  translator/parser/semantic-values/stepper.cpp
  translator/parser/semantic-values/attributes.h
  translator/parser/semantic-values/attributes.cpp
  translator/parser/semantic-values/subexpr.h
  translator/parser/semantic-values/subexpr.cpp
  translator/parser/semantic-values/author.h
  translator/parser/semantic-values/author.cpp
  translator/parser/semantic-values/string_array.h
  translator/parser/semantic-values/string_array.cpp
  translator/parser/semantic-values/metric.h
  translator/parser/semantic-values/metric.cpp
  )

SET(TRANSLATOR_PARSER_FILES
  translator/parser/input_stack.cpp
  translator/parser/input_stack.h
  translator/parser/lexeme.h
  translator/parser/lexfile.cpp
  translator/parser/lexfile.h
  translator/parser/lexical.h
  translator/parser/lexstream.h
  translator/parser/semantic-values/semantic_values.h
  translator/parser/symbol_list.h
  translator/parser/y_common.h
  translator/parser/y_lexer.cpp
  translator/parser/y_lexer.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_RESOURCES_DETAIL_FILES
  translator/transport-objects/canonical/resources_detail/potential_cache.cpp
  translator/transport-objects/canonical/resources_detail/potential_cache.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_TENSORS_FILES
  translator/transport-objects/canonical/tensors/A.cpp
  translator/transport-objects/canonical/tensors/A.h
  translator/transport-objects/canonical/tensors/Atilde.cpp
  translator/transport-objects/canonical/tensors/Atilde.h
  translator/transport-objects/canonical/tensors/B.cpp
  translator/transport-objects/canonical/tensors/B.h
  translator/transport-objects/canonical/tensors/C.cpp
  translator/transport-objects/canonical/tensors/C.h
  translator/transport-objects/canonical/tensors/coordinates.cpp
  translator/transport-objects/canonical/tensors/coordinates.h
  translator/transport-objects/canonical/tensors/dddV.cpp
  translator/transport-objects/canonical/tensors/dddV.h
  translator/transport-objects/canonical/tensors/ddV.cpp
  translator/transport-objects/canonical/tensors/ddV.h
  translator/transport-objects/canonical/tensors/dN1.cpp
  translator/transport-objects/canonical/tensors/dN1.h
  translator/transport-objects/canonical/tensors/dN2.cpp
  translator/transport-objects/canonical/tensors/dN2.h
  translator/transport-objects/canonical/tensors/dV.cpp
  translator/transport-objects/canonical/tensors/dV.h
  translator/transport-objects/canonical/tensors/fields.cpp
  translator/transport-objects/canonical/tensors/fields.h
  translator/transport-objects/canonical/tensors/Hubble.cpp
  translator/transport-objects/canonical/tensors/Hubble.h
  translator/transport-objects/canonical/tensors/M.cpp
  translator/transport-objects/canonical/tensors/M.h
  translator/transport-objects/canonical/tensors/momenta.cpp
  translator/transport-objects/canonical/tensors/momenta.h
  translator/transport-objects/canonical/tensors/SR_velocity.cpp
  translator/transport-objects/canonical/tensors/SR_velocity.h
  translator/transport-objects/canonical/tensors/u1.cpp
  translator/transport-objects/canonical/tensors/u1.h
  translator/transport-objects/canonical/tensors/u2.cpp
  translator/transport-objects/canonical/tensors/u2.h
  translator/transport-objects/canonical/tensors/u3.cpp
  translator/transport-objects/canonical/tensors/u3.h
  translator/transport-objects/canonical/tensors/zeta1.cpp
  translator/transport-objects/canonical/tensors/zeta1.h
  translator/transport-objects/canonical/tensors/zeta2.cpp
  translator/transport-objects/canonical/tensors/zeta2.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_FILES
  translator/transport-objects/canonical/resource_manager.cpp
  translator/transport-objects/canonical/resource_manager.h
  translator/transport-objects/canonical/resources.cpp
  translator/transport-objects/canonical/resources.h
  translator/transport-objects/canonical/tensor_factory.cpp
  translator/transport-objects/canonical/tensor_factory.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_TENSORS_FILES
  translator/transport-objects/concepts/tensors/A.h
  translator/transport-objects/concepts/tensors/Atilde.h
  translator/transport-objects/concepts/tensors/B.h
  translator/transport-objects/concepts/tensors/C.h
  translator/transport-objects/concepts/tensors/connexion.h
  translator/transport-objects/concepts/tensors/coordinates.h
  translator/transport-objects/concepts/tensors/dddV.h
  translator/transport-objects/concepts/tensors/ddV.h
  translator/transport-objects/concepts/tensors/dN1.h
  translator/transport-objects/concepts/tensors/dN2.h
  translator/transport-objects/concepts/tensors/dV.h
  translator/transport-objects/concepts/tensors/fields.h
  translator/transport-objects/concepts/tensors/Hubble.h
  translator/transport-objects/concepts/tensors/M.h
  translator/transport-objects/concepts/tensors/metric.h
  translator/transport-objects/concepts/tensors/metric_inverse.h
  translator/transport-objects/concepts/tensors/momenta.h
  translator/transport-objects/concepts/tensors/parameters.h
  translator/transport-objects/concepts/tensors/parameters.cpp
  translator/transport-objects/concepts/tensors/Riemann_A2.h
  translator/transport-objects/concepts/tensors/Riemann_A3.h
  translator/transport-objects/concepts/tensors/Riemann_B3.h
  translator/transport-objects/concepts/tensors/SR_velocity.h
  translator/transport-objects/concepts/tensors/u1.h
  translator/transport-objects/concepts/tensors/u2.h
  translator/transport-objects/concepts/tensors/u3.h
  translator/transport-objects/concepts/tensors/zeta1.h
  translator/transport-objects/concepts/tensors/zeta2.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_FILES
  translator/transport-objects/concepts/flattened_tensor.h
  translator/transport-objects/concepts/resource_manager.h
  translator/transport-objects/concepts/tensor_factory.h
  translator/transport-objects/concepts/transport_tensor.h
  translator/transport-objects/concepts/tensor_exception.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_RESOURCES_DETAIL_FILES
  translator/transport-objects/concepts/resources_detail/indexed_resource.h
  translator/transport-objects/concepts/resources_detail/simple_resource.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_NONTRIVIAL_METRIC_RESOURCES_DETAIL_FILES
  translator/transport-objects/nontrivial-metric/resources_detail/connexion_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/connexion_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_dddV_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_dddV_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_ddV_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_ddV_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_dV_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_dV_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannA2_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannA2_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannA3_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannA3_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannB3_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/covariant_RiemannB3_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/derivative_symbols.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/derivative_symbols.h
  translator/transport-objects/nontrivial-metric/resources_detail/potential_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/potential_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/substitution_map_cache.cpp
  translator/transport-objects/nontrivial-metric/resources_detail/substitution_map_cache.h
  translator/transport-objects/nontrivial-metric/resources_detail/common.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_NONTRIVIAL_METRIC_TENSORS_FILES
  translator/transport-objects/nontrivial-metric/tensors/A.cpp
  translator/transport-objects/nontrivial-metric/tensors/A.h
  translator/transport-objects/nontrivial-metric/tensors/Atilde.cpp
  translator/transport-objects/nontrivial-metric/tensors/Atilde.h
  translator/transport-objects/nontrivial-metric/tensors/B.cpp
  translator/transport-objects/nontrivial-metric/tensors/B.h
  translator/transport-objects/nontrivial-metric/tensors/C.cpp
  translator/transport-objects/nontrivial-metric/tensors/C.h
  translator/transport-objects/nontrivial-metric/tensors/connexion.cpp
  translator/transport-objects/nontrivial-metric/tensors/connexion.h
  translator/transport-objects/nontrivial-metric/tensors/coordinates.cpp
  translator/transport-objects/nontrivial-metric/tensors/coordinates.h
  translator/transport-objects/nontrivial-metric/tensors/dddV.cpp
  translator/transport-objects/nontrivial-metric/tensors/dddV.h
  translator/transport-objects/nontrivial-metric/tensors/ddV.cpp
  translator/transport-objects/nontrivial-metric/tensors/ddV.h
  translator/transport-objects/nontrivial-metric/tensors/dN1.cpp
  translator/transport-objects/nontrivial-metric/tensors/dN1.h
  translator/transport-objects/nontrivial-metric/tensors/dN2.cpp
  translator/transport-objects/nontrivial-metric/tensors/dN2.h
  translator/transport-objects/nontrivial-metric/tensors/dV.cpp
  translator/transport-objects/nontrivial-metric/tensors/dV.h
  translator/transport-objects/nontrivial-metric/tensors/fields.cpp
  translator/transport-objects/nontrivial-metric/tensors/fields.h
  translator/transport-objects/nontrivial-metric/tensors/Hubble.cpp
  translator/transport-objects/nontrivial-metric/tensors/Hubble.h
  translator/transport-objects/nontrivial-metric/tensors/M.cpp
  translator/transport-objects/nontrivial-metric/tensors/M.h
  translator/transport-objects/nontrivial-metric/tensors/metric.cpp
  translator/transport-objects/nontrivial-metric/tensors/metric.h
  translator/transport-objects/nontrivial-metric/tensors/metric_inverse.cpp
  translator/transport-objects/nontrivial-metric/tensors/metric_inverse.h
  translator/transport-objects/nontrivial-metric/tensors/momenta.cpp
  translator/transport-objects/nontrivial-metric/tensors/momenta.h
  translator/transport-objects/nontrivial-metric/tensors/Riemann_A2.cpp
  translator/transport-objects/nontrivial-metric/tensors/Riemann_A2.h
  translator/transport-objects/nontrivial-metric/tensors/Riemann_A3.cpp
  translator/transport-objects/nontrivial-metric/tensors/Riemann_A3.h
  translator/transport-objects/nontrivial-metric/tensors/Riemann_B3.cpp
  translator/transport-objects/nontrivial-metric/tensors/Riemann_B3.h
  translator/transport-objects/nontrivial-metric/tensors/SR_velocity.cpp
  translator/transport-objects/nontrivial-metric/tensors/SR_velocity.h
  translator/transport-objects/nontrivial-metric/tensors/u1.cpp
  translator/transport-objects/nontrivial-metric/tensors/u1.h
  translator/transport-objects/nontrivial-metric/tensors/u2.cpp
  translator/transport-objects/nontrivial-metric/tensors/u2.h
  translator/transport-objects/nontrivial-metric/tensors/u3.cpp
  translator/transport-objects/nontrivial-metric/tensors/u3.h
  translator/transport-objects/nontrivial-metric/tensors/zeta1.cpp
  translator/transport-objects/nontrivial-metric/tensors/zeta1.h
  translator/transport-objects/nontrivial-metric/tensors/zeta2.cpp
  translator/transport-objects/nontrivial-metric/tensors/zeta2.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_NONTRIVIAL_METRIC_FILES
  translator/transport-objects/nontrivial-metric/resource_manager.cpp
  translator/transport-objects/nontrivial-metric/resource_manager.h
  translator/transport-objects/nontrivial-metric/resources.cpp
  translator/transport-objects/nontrivial-metric/resources.h
  translator/transport-objects/nontrivial-metric/tensor_factory.cpp
  translator/transport-objects/nontrivial-metric/tensor_factory.h
  translator/transport-objects/nontrivial-metric/curvature_classes.h
  translator/transport-objects/nontrivial-metric/curvature_classes.cpp
  translator/transport-objects/nontrivial-metric/metric_resource.cpp
  translator/transport-objects/nontrivial-metric/metric_resource.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_FILES
  translator/transport-objects/make_tensor_factory.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_INDICES_FILES
  translator/transport-objects/indices/index_flatten.cpp
  translator/transport-objects/indices/index_flatten.h
  translator/transport-objects/indices/index_traits.cpp
  translator/transport-objects/indices/index_traits.h
  translator/transport-objects/indices/indices.cpp
  translator/transport-objects/indices/indices.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_SHARED_CACHE_DETAIL_FILES
  translator/transport-objects/shared/cache_detail/cache_tags.cpp
  translator/transport-objects/shared/cache_detail/cache_tags.h
  translator/transport-objects/shared/cache_detail/cache_key.h
  )

SET(TRANSLATOR_TRANSPORT_OBJECTS_SHARED_FILES
  translator/transport-objects/shared/expression_cache.h
  translator/transport-objects/shared/ginac_cache.h
  translator/transport-objects/shared/shared_resources.cpp
  translator/transport-objects/shared/shared_resources.h
  translator/transport-objects/shared/symbol_factory.cpp
  translator/transport-objects/shared/symbol_factory.h
  translator/transport-objects/shared/resource_failure.h
  translator/transport-objects/shared/variance_tensor_cache.h
  )

SET(TRANSLATOR_VERSION_POLICY_FILES
  translator/version-policy/version_policy.cpp
  translator/version-policy/version_policy.h
  translator/version-policy/policies.h)

SET(TRANSLATOR_UTILITIES_FILES
  translator/utilities/argument_cache.cpp
  translator/utilities/argument_cache.h
  translator/utilities/error.cpp
  translator/utilities/error.h
  translator/utilities/error_context.cpp
  translator/utilities/error_context.h
  translator/utilities/finder.cpp
  translator/utilities/finder.h
  translator/utilities/formatter.cpp
  translator/utilities/formatter.h
  translator/utilities/ginac_print_indexed.cpp
  translator/utilities/ginac_print_indexed.h
  translator/utilities/local_environment.cpp
  translator/utilities/local_environment.h
  translator/utilities/to_printable.cpp
  translator/utilities/to_printable.h
  translator/utilities/hash_combine.h
  )

SET(TRANSPORT_RUNTIME_CONCEPTS_FILES
  transport-runtime/concepts/flattener.h
  transport-runtime/concepts/initial_conditions.h
  transport-runtime/concepts/initial_conditions_forward_declare.h
  transport-runtime/concepts/parameters.h
  transport-runtime/concepts/range.h
  )

SET(TRANSPORT_RUNTIME_CONCEPTS_RANGE_DETAIL_FILES
  transport-runtime/concepts/range_detail/abstract.h
  transport-runtime/concepts/range_detail/aggregate.h
  transport-runtime/concepts/range_detail/common.h
  transport-runtime/concepts/range_detail/helper.h
  transport-runtime/concepts/range_detail/basic.h
  )

SET(TRANSPORT_RUNTIME_DATA_BATCHERS_FILES
  transport-runtime/data/batchers/batchers.h
  transport-runtime/data/batchers/container_dispatch_function.h
  transport-runtime/data/batchers/container_replace_function.h
  transport-runtime/data/batchers/generic_batcher.h
  transport-runtime/data/batchers/integration_batcher.h
  transport-runtime/data/batchers/integration_items.h
  transport-runtime/data/batchers/postintegration_batcher.h
  transport-runtime/data/batchers/postintegration_items.h
  transport-runtime/data/batchers/postprocess_delegate.h
  )

SET(TRANSPORT_RUNTIME_DATA_DATAPIPE_FILES
  transport-runtime/data/datapipe/datapipe_decl.h
  transport-runtime/data/datapipe/datapipe_dispatch_function.h
  transport-runtime/data/datapipe/datapipe_forward_declare.h
  transport-runtime/data/datapipe/datapipe_impl.h
  transport-runtime/data/datapipe/linecache_specializations.h
  transport-runtime/data/datapipe/tags.h
  transport-runtime/data/datapipe/tags_forward_declare.h
  )

SET(TRANSPORT_RUNTIME_DATA_DATAPIPE_TAGS_FILES
  transport-runtime/data/datapipe/tags/config_tags.h
  transport-runtime/data/datapipe/tags/field_tags.h
  transport-runtime/data/datapipe/tags/fNL_tags.h
  transport-runtime/data/datapipe/tags/stats_tags.h
  transport-runtime/data/datapipe/tags/zeta_tags.h
  )

SET(TRANSPORT_RUNTIME_DATA_DETAIL_FILES
  transport-runtime/data/detail/data_manager_decl.h
  transport-runtime/data/detail/data_manager_impl.h
  )

SET(TRANSPORT_RUNTIME_DATA_FILES
  transport-runtime/data/data_manager.h
  transport-runtime/data/metadata.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_CONVENIENCE_FILES
  transport-runtime/derived-products/convenience/time_series_plot.h
  transport-runtime/derived-products/convenience/time_series_table.h
  transport-runtime/derived-products/convenience/wavenumber_series_plot.h
  transport-runtime/derived-products/convenience/wavenumber_series_table.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES
  transport-runtime/derived-products/derived-content/background-quantites/background_line.h
  transport-runtime/derived-products/derived-content/background-quantites/largest_u2_line.h
  transport-runtime/derived-products/derived-content/background-quantites/largest_u3_line.h
  transport-runtime/derived-products/derived-content/background-quantites/u2_line.h
  transport-runtime/derived-products/derived-content/background-quantites/u3_line.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES
  transport-runtime/derived-products/derived-content/concepts/derived_line.h
  transport-runtime/derived-products/derived-content/concepts/derived_line_helper.h
  transport-runtime/derived-products/derived-content/concepts/derived_line_type.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES
  transport-runtime/derived-products/derived-content/concepts/lines/fNL_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/r_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/tensor_twopf_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/threepf_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/twopf_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/zeta_reduced_bispectrum_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/zeta_threepf_line.h
  transport-runtime/derived-products/derived-content/concepts/lines/zeta_twopf_line.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES
  transport-runtime/derived-products/derived-content/concepts/series/time_series.h
  transport-runtime/derived-products/derived-content/concepts/series/wavenumber_series.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES
  transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/common.h
  transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/fNL_timeseries_compute.h
  transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/zeta_timeseries_compute.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES
  transport-runtime/derived-products/derived-content/correlation-functions/field_time_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/field_wavenumber_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/fNL_time_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/r_time_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/r_wavenumber_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/template_types.h
  transport-runtime/derived-products/derived-content/correlation-functions/tensor_time_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/tensor_wavenumber_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/zeta_time_series.h
  transport-runtime/derived-products/derived-content/correlation-functions/zeta_wavenumber_series.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES
  transport-runtime/derived-products/derived-content/deprecated/threepf_kconfig_shift.h
  transport-runtime/derived-products/derived-content/deprecated/threepf_time_shift.h
  transport-runtime/derived-products/derived-content/deprecated/zeta_kseries_compute.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES
  transport-runtime/derived-products/derived-content/integration-analysis/cost_metrics.h
  transport-runtime/derived-products/derived-content/integration-analysis/cost_wavenumber.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES
  transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_query.h
  transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_threepf_query.h
  transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_time_query.h
  transport-runtime/derived-products/derived-content/SQL_query/detail/SQL_twopf_query.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES
  transport-runtime/derived-products/derived-content/SQL_query/SQL_policy.h
  transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h
  transport-runtime/derived-products/derived-content/SQL_query/SQL_query_helper.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES
  transport-runtime/derived-products/derived-content/utilities/integration_task_gadget.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_FILES
  transport-runtime/derived-products/data_products.h
  transport-runtime/derived-products/derived_product.h
  transport-runtime/derived-products/derived_product_forward_declare.h
  transport-runtime/derived-products/derived_product_helper.h
  transport-runtime/derived-products/derived_product_type.h
  transport-runtime/derived-products/enumerations.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES
  transport-runtime/derived-products/line-collections/data_line.h
  transport-runtime/derived-products/line-collections/line_asciitable.h
  transport-runtime/derived-products/line-collections/line_collection.h
  transport-runtime/derived-products/line-collections/line_plot2d.h
  transport-runtime/derived-products/line-collections/line_values.h
  )

SET(TRANSPORT_RUNTIME_DERIVED_PRODUCTS_UTILITIES_FILES
  transport-runtime/derived-products/utilities/extractor.h
  transport-runtime/derived-products/utilities/index_selector.h
  transport-runtime/derived-products/utilities/wrapper.h
  )

SET(TRANSPORT_RUNTIME_FILES
  transport-runtime/ansi_colour_codes.h
  transport-runtime/default_symbols.h
  transport-runtime/defaults.h
  transport-runtime/enumerations.h
  transport-runtime/exceptions.h
  transport-runtime/messages.h
  transport-runtime/transport.h
  transport-runtime/version.h
  )

SET(TRANSPORT_RUNTIME_INSTRUMENTS_FILES
  transport-runtime/instruments/timing_instrument.h
  transport-runtime/instruments/busyidle_timer_set.h
  )

SET(TRANSPORT_RUNTIME_LOCALIZATIONS_FILES
  transport-runtime/localizations/messages_en.h
  )

SET(TRANSPORT_RUNTIME_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES
  transport-runtime/localizations/messages_en/derived-products/derived_line.h
  transport-runtime/localizations/messages_en/derived-products/derived_product.h
  transport-runtime/localizations/messages_en/derived-products/fNL_line.h
  transport-runtime/localizations/messages_en/derived-products/line_asciitable.h
  transport-runtime/localizations/messages_en/derived-products/line_collection.h
  transport-runtime/localizations/messages_en/derived-products/line_plot2d.h
  transport-runtime/localizations/messages_en/derived-products/line_values.h
  transport-runtime/localizations/messages_en/derived-products/SQL_query.h
  transport-runtime/localizations/messages_en/derived-products/time_series.h
  transport-runtime/localizations/messages_en/derived-products/time_series_convenience.h
  transport-runtime/localizations/messages_en/derived-products/wavenumber_series.h
  transport-runtime/localizations/messages_en/derived-products/wavenumber_series_convenience.h
  )

SET(TRANSPORT_RUNTIME_LOCALIZATIONS_MESSAGES_EN_FILES
  transport-runtime/localizations/messages_en/advisory_classes.h
  transport-runtime/localizations/messages_en/asciitable.h
  transport-runtime/localizations/messages_en/command_line.h
  transport-runtime/localizations/messages_en/data_manager.h
  transport-runtime/localizations/messages_en/initial_conditions.h
  transport-runtime/localizations/messages_en/integration.h
  transport-runtime/localizations/messages_en/json_repository_interface.h
  transport-runtime/localizations/messages_en/labels.h
  transport-runtime/localizations/messages_en/manager.h
  transport-runtime/localizations/messages_en/worker_scheduler.h
  transport-runtime/localizations/messages_en/worker_manager.h
  transport-runtime/localizations/messages_en/report_manager.h
  transport-runtime/localizations/messages_en/model_list.h
  transport-runtime/localizations/messages_en/model_manager.h
  transport-runtime/localizations/messages_en/models.h
  transport-runtime/localizations/messages_en/named_list.h
  transport-runtime/localizations/messages_en/parameters.h
  transport-runtime/localizations/messages_en/range.h
  transport-runtime/localizations/messages_en/report_cmdline.h
  transport-runtime/localizations/messages_en/report_HTML.h
  transport-runtime/localizations/messages_en/email.h
  transport-runtime/localizations/messages_en/repository.h
  transport-runtime/localizations/messages_en/repository_records.h
  transport-runtime/localizations/messages_en/repository_sqlite3.h
  transport-runtime/localizations/messages_en/repository_toolkit.h
  transport-runtime/localizations/messages_en/runtimeapi.h
  transport-runtime/localizations/messages_en/scheduler.h
  transport-runtime/localizations/messages_en/serialization.h
  transport-runtime/localizations/messages_en/sqlite3_utility.h
  transport-runtime/localizations/messages_en/task_manager.h
  transport-runtime/localizations/messages_en/transaction_manager.h
  transport-runtime/localizations/messages_en/work_journal.h
  transport-runtime/localizations/messages_en/busyidle_timer_set.h
  )

SET(TRANSPORT_RUNTIME_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES
  transport-runtime/localizations/messages_en/tasks/tasks.h
  transport-runtime/localizations/messages_en/tasks/threepf_config_database.h
  transport-runtime/localizations/messages_en/tasks/twopf_config_database.h
  )

SET(TRANSPORT_RUNTIME_MANAGER_DETAIL_FILES
  transport-runtime/manager/detail/aggregation.h
  transport-runtime/manager/detail/aggregation_forward_declare.h
  transport-runtime/manager/detail/job_descriptors.h
  transport-runtime/manager/detail/master_controller_decl.h
  transport-runtime/manager/detail/master_controller_core.h
  transport-runtime/manager/detail/master_controller_switches.h
  transport-runtime/manager/detail/master_controller_integration.h
  transport-runtime/manager/detail/master_controller_output.h
  transport-runtime/manager/detail/master_controller_postintegration.h
  transport-runtime/manager/detail/slave_container_dispatch_decl.h
  transport-runtime/manager/detail/slave_container_dispatch_impl.h
  transport-runtime/manager/detail/slave_controller_decl.h
  transport-runtime/manager/detail/slave_controller_impl.h
  transport-runtime/manager/detail/slave_datapipe_dispatch_decl.h
  transport-runtime/manager/detail/slave_datapipe_dispatch_impl.h
  transport-runtime/manager/detail/slave_message_buffer.h
  )

SET(TRANSPORT_RUNTIME_MANAGER_FILES
  transport-runtime/manager/argument_cache.h
  transport-runtime/manager/environment.h
  transport-runtime/manager/master_controller.h
  transport-runtime/manager/worker_scheduler.h
  transport-runtime/manager/worker_manager.h
  transport-runtime/manager/message_handlers.h
  transport-runtime/manager/model_manager.h
  transport-runtime/manager/mpi_operations.h
  transport-runtime/manager/slave_controller.h
  transport-runtime/manager/slave_work_handler.h
  transport-runtime/manager/task_gallery.h
  transport-runtime/manager/task_manager.h
  transport-runtime/manager/work_journal.h
  transport-runtime/manager/report_manager.h
  )

SET(TRANSPORT_RUNTIME_MODELS_FILES
  transport-runtime/models/advisory_classes.h
  transport-runtime/models/canonical_model.h
  transport-runtime/models/nontrivial_metric_model.h
  transport-runtime/models/model.h
  transport-runtime/models/model_forward_declare.h
  transport-runtime/models/observers.h
  transport-runtime/models/odeint_defaults.h
  )

SET(TRANSPORT_RUNTIME_REPORTING_FILES
  transport-runtime/reporting/command_line.h
  transport-runtime/reporting/content_group_data.h
  transport-runtime/reporting/HTML_report.h
  transport-runtime/reporting/HTML_report_bundle.h
  transport-runtime/reporting/HTML_writer.h
  transport-runtime/reporting/JavaScript_writer.h
  transport-runtime/reporting/repository_cache.h
  transport-runtime/reporting/key_value.h
  transport-runtime/reporting/unique_tags.h
  transport-runtime/reporting/email.h
  )

SET(TRANSPORT_RUNTIME_REPOSITORY_DETAIL_FILES
  transport-runtime/repository/detail/repository_decl.h
  transport-runtime/repository/detail/repository_impl.h
  transport-runtime/repository/detail/repository_mode.h
  transport-runtime/repository/detail/writer_repo_decl.h
  transport-runtime/repository/detail/writer_repo_impl.h
  )

SET(TRANSPORT_RUNTIME_REPOSITORY_FILES
  transport-runtime/repository/inflight.h
  transport-runtime/repository/json_repository.h
  transport-runtime/repository/repository.h
  transport-runtime/repository/repository_toolkit.h
  transport-runtime/repository/repository_graphkit.h
  transport-runtime/repository/repository_forward_declare.h
  )

SET(TRANSPORT_RUNTIME_REPOSITORY_RECORDS_DETAIL_FILES
  transport-runtime/repository/records/detail/content_group_decl.h
  transport-runtime/repository/records/detail/content_group_impl.h
  transport-runtime/repository/records/detail/content_group_metadata_decl.h
  transport-runtime/repository/records/detail/content_group_metadata_impl.h
  transport-runtime/repository/records/detail/derived_product_decl.h
  transport-runtime/repository/records/detail/derived_product_impl.h
  transport-runtime/repository/records/detail/metadata_decl.h
  transport-runtime/repository/records/detail/metadata_impl.h
  transport-runtime/repository/records/detail/notes.h
  transport-runtime/repository/records/detail/package_decl.h
  transport-runtime/repository/records/detail/package_impl.h
  transport-runtime/repository/records/detail/record_decl.h
  transport-runtime/repository/records/detail/record_impl.h
  transport-runtime/repository/records/detail/task_decl.h
  transport-runtime/repository/records/detail/task_impl.h
  )

SET(TRANSPORT_RUNTIME_REPOSITORY_RECORDS_FILES
  transport-runtime/repository/records/content_finder_decl.h
  transport-runtime/repository/records/content_finder_impl.h
  transport-runtime/repository/records/record_finder_decl.h
  transport-runtime/repository/records/record_finder_impl.h
  transport-runtime/repository/records/repository_records_decl.h
  transport-runtime/repository/records/repository_records_forward_declare.h
  transport-runtime/repository/records/repository_records_impl.h
  )

SET(TRANSPORT_RUNTIME_REPOSITORY_WRITERS_FILES
  transport-runtime/repository/writers/derived_content_writer.h
  transport-runtime/repository/writers/generic_writer.h
  transport-runtime/repository/writers/integration_writer.h
  transport-runtime/repository/writers/postintegration_writer.h
  transport-runtime/repository/writers/writers.h
  transport-runtime/repository/writers/aggregation_profiler.h
  )

SET(TRANSPORT_RUNTIME_SCHEDULER_FILES
  transport-runtime/scheduler/context.h
  transport-runtime/scheduler/scheduler.h
  transport-runtime/scheduler/work_queue.h
  )

SET(TRANSPORT_RUNTIME_SERIALIZATION_FILES
  transport-runtime/serialization/serializable.h
  )

SET(TRANSPORT_RUNTIME_SQLITE3_DETAIL_FILES
  transport-runtime/sqlite3/detail/container_replace_decl.h
  transport-runtime/sqlite3/detail/container_replace_impl.h
  transport-runtime/sqlite3/detail/data_manager_sqlite3_decl.h
  transport-runtime/sqlite3/detail/data_manager_sqlite3_impl.h
  transport-runtime/sqlite3/detail/data_manager_transaction_handler_decl.h
  transport-runtime/sqlite3/detail/data_manager_transaction_handler_impl.h
  transport-runtime/sqlite3/detail/repository_sqlite3_decl.h
  transport-runtime/sqlite3/detail/repository_sqlite3_impl.h
  transport-runtime/sqlite3/detail/repository_transaction_handler_decl.h
  transport-runtime/sqlite3/detail/repository_transaction_handler_impl.h
  transport-runtime/sqlite3/detail/writer_data_mgr_decl.h
  transport-runtime/sqlite3/detail/writer_data_mgr_impl.h
  )

SET(TRANSPORT_RUNTIME_SQLITE3_FILES
  transport-runtime/sqlite3/data_manager_sqlite3.h
  transport-runtime/sqlite3/repository_sqlite3.h
  )

SET(TRANSPORT_RUNTIME_SQLITE3_OPERATIONS_FILES
  transport-runtime/sqlite3/operations/data_manager.h
  transport-runtime/sqlite3/operations/data_manager_aggregate.h
  transport-runtime/sqlite3/operations/data_manager_common.h
  transport-runtime/sqlite3/operations/data_manager_create.h
  transport-runtime/sqlite3/operations/data_manager_integrity.h
  transport-runtime/sqlite3/operations/data_manager_pull.h
  transport-runtime/sqlite3/operations/data_manager_read.h
  transport-runtime/sqlite3/operations/data_manager_write.h
  transport-runtime/sqlite3/operations/data_manager_finalize.h
  transport-runtime/sqlite3/operations/data_traits.h
  transport-runtime/sqlite3/operations/repository.h
  transport-runtime/sqlite3/operations/repository_admin.h
  transport-runtime/sqlite3/operations/repository_common.h
  transport-runtime/sqlite3/operations/repository_count.h
  transport-runtime/sqlite3/operations/repository_create.h
  transport-runtime/sqlite3/operations/repository_delete.h
  transport-runtime/sqlite3/operations/repository_inflight.h
  transport-runtime/sqlite3/operations/repository_store.h
  transport-runtime/sqlite3/operations/sqlite3_utility.h
  )

SET(TRANSPORT_RUNTIME_TASKS_CONFIGURATION_DATABASE_FILES
  transport-runtime/tasks/configuration-database/generic_config_iterator.h
  transport-runtime/tasks/configuration-database/generic_record_iterator.h
  transport-runtime/tasks/configuration-database/generic_value_iterator.h
  transport-runtime/tasks/configuration-database/threepf_config_database.h
  transport-runtime/tasks/configuration-database/time_config_database.h
  transport-runtime/tasks/configuration-database/twopf_config_database.h
  )

SET(TRANSPORT_RUNTIME_TASKS_FILES
  transport-runtime/tasks/derivable_task.h
  transport-runtime/tasks/integration_tasks.h
  transport-runtime/tasks/output_tasks.h
  transport-runtime/tasks/postintegration_tasks.h
  transport-runtime/tasks/task.h
  transport-runtime/tasks/task_configurations.h
  transport-runtime/tasks/task_configurations_forward_declare.h
  transport-runtime/tasks/task_helper.h
  transport-runtime/tasks/task_types.h
  transport-runtime/tasks/tasks_forward_declare.h
  )

SET(TRANSPORT_RUNTIME_TASKS_INTEGRATION_DETAIL_FILES
  transport-runtime/tasks/integration_detail/abstract.h
  transport-runtime/tasks/integration_detail/background_task.h
  transport-runtime/tasks/integration_detail/common.h
  transport-runtime/tasks/integration_detail/default_policies.h
  transport-runtime/tasks/integration_detail/threepf_task.h
  transport-runtime/tasks/integration_detail/twopf_db_task.h
  transport-runtime/tasks/integration_detail/twopf_task.h
  )

SET(TRANSPORT_RUNTIME_TASKS_POSTINTEGRATION_DETAIL_FILES
  transport-runtime/tasks/postintegration_detail/abstract.h
  transport-runtime/tasks/postintegration_detail/common.h
  transport-runtime/tasks/postintegration_detail/fNL_task.h
  transport-runtime/tasks/postintegration_detail/zeta_threepf_task.h
  transport-runtime/tasks/postintegration_detail/zeta_twopf_db_task.h
  transport-runtime/tasks/postintegration_detail/zeta_twopf_task.h
  )

SET(TRANSPORT_RUNTIME_TRANSACTIONS_FILES
  transport-runtime/transactions/transaction_manager.h
  )

SET(TRANSPORT_RUNTIME_UTILITIES_FILES
  transport-runtime/utilities/asciitable.h
  transport-runtime/utilities/formatter.h
  transport-runtime/utilities/host_information.h
  transport-runtime/utilities/latex_output.h
  transport-runtime/utilities/linecache.h
  transport-runtime/utilities/match.h
  transport-runtime/utilities/named_list.h
  transport-runtime/utilities/plot_environment.h
  transport-runtime/utilities/random_string.h
  transport-runtime/utilities/spline1d.h
  transport-runtime/utilities/to_printable.h
  )

SET(TESTS_PYTRANSPORT_NONTRIVIAL_METRIC_FILES
  tests/PyTransport/nontrivial-metric/fieldmetric.h
  tests/PyTransport/nontrivial-metric/model.h
  tests/PyTransport/nontrivial-metric/potential.h
  tests/PyTransport/nontrivial-metric/nontrivial-metric.t.cpp
  )

SET(SOURCE_FILES
  ${TEMPLATES_FILES}
  ${TEMPLATES_VEXCL_CUDA_FILES}
  ${TEMPLATES_VEXCL_OPENCL_FILES}
  ${TOP_LEVEL_FILES}
  ${TRANSLATOR_BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES}
  ${TRANSLATOR_BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES}
  ${TRANSLATOR_BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES}
  ${TRANSLATOR_BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES}
  ${TRANSLATOR_BACKENDS_LANGUAGES_CPP_CORE_FILES}
  ${TRANSLATOR_BACKENDS_LANGUAGES_CPP_FILES}
  ${TRANSLATOR_BACKENDS_LANGUAGES_CPP_VEXCL_FILES}
  ${TRANSLATOR_BACKENDS_LANGUAGES_CUDA_FILES}
  ${TRANSLATOR_BACKENDS_LANGUAGES_OPENCL_FILES}
  ${TRANSLATOR_BACKENDS_LANGUAGES_SHARED_FILES}
  ${TRANSLATOR_BACKENDS_SHARED_FILES}
  ${TRANSLATOR_FILES}
  ${TRANSLATOR_INSTRUMENTS_FILES}
  ${TRANSLATOR_PARSER_BISON_FILES}
  ${TRANSLATOR_PARSER_DESCRIPTOR_BLOCKS_FILES}
  ${TRANSLATOR_PARSER_DESCRIPTOR_SYMBOLS_FILES}
  ${TRANSLATOR_PARSER_DESCRIPTOR_FILES}
  ${TRANSLATOR_PARSER_DRIVER_FILES}
  ${TRANSLATOR_PARSER_SEMANTIC_VALUES_FILES}
  ${TRANSLATOR_PARSER_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_RESOURCES_DETAIL_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_TENSORS_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_NONTRIVIAL_METRIC_RESOURCES_DETAIL_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_NONTRIVIAL_METRIC_TENSORS_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_NONTRIVIAL_METRIC_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_RESOURCES_DETAIL_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_TENSORS_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_INDICES_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_SHARED_CACHE_DETAIL_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_SHARED_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_FILES}
  ${TRANSLATOR_UTILITIES_FILES}
  ${TRANSLATOR_VERSION_POLICY_FILES}
  ${TRANSPORT_RUNTIME_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_CONCEPTS_RANGE_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_DATA_BATCHERS_FILES}
  ${TRANSPORT_RUNTIME_DATA_DATAPIPE_FILES}
  ${TRANSPORT_RUNTIME_DATA_DATAPIPE_TAGS_FILES}
  ${TRANSPORT_RUNTIME_DATA_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_DATA_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_CONVENIENCE_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES}
  ${TRANSPORT_RUNTIME_DERIVED_PRODUCTS_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_FILES}
  ${TRANSPORT_RUNTIME_INSTRUMENTS_FILES}
  ${TRANSPORT_RUNTIME_LOCALIZATIONS_FILES}
  ${TRANSPORT_RUNTIME_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES}
  ${TRANSPORT_RUNTIME_LOCALIZATIONS_MESSAGES_EN_FILES}
  ${TRANSPORT_RUNTIME_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES}
  ${TRANSPORT_RUNTIME_MANAGER_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_MANAGER_FILES}
  ${TRANSPORT_RUNTIME_MODELS_FILES}
  ${TRANSPORT_RUNTIME_REPORTING_FILES}
  ${TRANSPORT_RUNTIME_REPOSITORY_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_REPOSITORY_FILES}
  ${TRANSPORT_RUNTIME_REPOSITORY_RECORDS_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_REPOSITORY_RECORDS_FILES}
  ${TRANSPORT_RUNTIME_REPOSITORY_WRITERS_FILES}
  ${TRANSPORT_RUNTIME_SCHEDULER_FILES}
  ${TRANSPORT_RUNTIME_SERIALIZATION_FILES}
  ${TRANSPORT_RUNTIME_SQLITE3_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_SQLITE3_FILES}
  ${TRANSPORT_RUNTIME_SQLITE3_OPERATIONS_FILES}
  ${TRANSPORT_RUNTIME_TASKS_CONFIGURATION_DATABASE_FILES}
  ${TRANSPORT_RUNTIME_TASKS_FILES}
  ${TRANSPORT_RUNTIME_TASKS_INTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_TASKS_POSTINTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_TRANSACTIONS_FILES}
  ${TRANSPORT_RUNTIME_UTILITIES_FILES}
  ${TESTS_PYTRANSPORT_NONTRIVIAL_METRIC_FILES}
  )

ADD_EXECUTABLE(dummy_clion_target EXCLUDE_FROM_ALL ${SOURCE_FILES})
