CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(CppTransport)

# silence CMake warning about @rpath
# this is relevant only for shared libraries which we're not using, so has
# no consequences for us
SET(CMAKE_MACOSX_RPATH TRUE)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/splinter/cmake/")


# Attempt some platform/compiler introspection
STRING(COMPARE EQUAL "Linux" ${CMAKE_SYSTEM_NAME} LINUX)
STRING(COMPARE EQUAL "Windows" ${CMAKE_SYSTEM_NAME} WINDOWS)
STRING(COMPARE EQUAL "Darwin" ${CMAKE_SYSTEM_NAME} OS_X)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET(CLANG TRUE)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  SET(CLANG TRUE)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  SET(GCC TRUE)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  SET(INTEL TRUE)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  SET(MSVC TRUE)
ENDIF()


IF(CLANG)

  SET(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
  SET(CMAKE_C_FLAGS_RELEASE "-Ofast")

ELSEIF(GCC)

  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  SET(CMAKE_C_FLAGS_RELEASE "-O3")

ELSEIF(INTEL)

  SET(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
  SET(CMAKE_C_FLAGS_RELEASE "-Ofast")

ENDIF()


# find Bison to produce parser
FIND_PACKAGE(BISON 3.0 REQUIRED)


# pull in bundled JsonCpp and SPLINTER libraries
SET(ARCH x86-64)
ADD_SUBDIRECTORY(thirdparty/splinter)
ADD_SUBDIRECTORY(thirdparty/jsoncpp)


# find required libraries
FIND_PACKAGE(MPI REQUIRED)
FIND_PACKAGE(Boost 1.56 REQUIRED COMPONENTS system filesystem random timer date_time mpi log_setup log serialization thread program_options regex)
FIND_PACKAGE(GiNaC 1.6 REQUIRED)


# build parser
BISON_TARGET(translator/parser translator/parser/bison/y_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/y_parser.cpp)


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
  translator/backends/infrastructure/macro_substitution/macro_tokenizer.cpp
  translator/backends/infrastructure/macro_substitution/abstract_index.cpp
  translator/backends/infrastructure/macro_substitution/index_class.cpp
  translator/backends/infrastructure/replacement_rules/cse_map_field_indices.cpp
  translator/backends/infrastructure/replacement_rules/cse_map_phase_indices.cpp
  translator/backends/infrastructure/replacement_rules/package_group.cpp
  translator/backends/infrastructure/replacement_rules/package_group_factory.cpp
  translator/backends/infrastructure/replacement_rules/replacement_rule_package.cpp
  translator/backends/infrastructure/replacement_rules/replacement_rule_definitions.cpp
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
  translator/backends/shared/directives.cpp
  translator/backends/shared/flow_tensors.cpp
  translator/backends/shared/fundamental.cpp
  translator/backends/shared/gauge_xfm.cpp
  translator/backends/shared/lagrangian_tensors.cpp
  translator/backends/shared/resources.cpp
  translator/backends/shared/temporary_pool.cpp
  translator/backends/shared/utensors.cpp
  ${BISON_parser_OUTPUTS}
  translator/parser/input_stack.cpp
  translator/parser/lexfile.cpp
  translator/parser/script.cpp
  translator/parser/y_driver.cpp
  translator/parser/y_lexer.cpp
  translator/transport-objects/canonical/resources.cpp
  translator/transport-objects/canonical/Hubble.cpp
  translator/transport-objects/canonical/dV.cpp
  translator/transport-objects/canonical/ddV.cpp
  translator/transport-objects/canonical/dddV.cpp
  translator/transport-objects/canonical/SR_velocity.cpp
  translator/transport-objects/canonical/A.cpp
  translator/transport-objects/canonical/B.cpp
  translator/transport-objects/canonical/C.cpp
  translator/transport-objects/canonical/M.cpp
  translator/transport-objects/canonical/u1.cpp
  translator/transport-objects/canonical/u2.cpp
  translator/transport-objects/canonical/u3.cpp
  translator/transport-objects/canonical/zeta1.cpp
  translator/transport-objects/canonical/zeta2.cpp
  translator/transport-objects/canonical/dN1.cpp
  translator/transport-objects/canonical/dN2.cpp
  translator/transport-objects/canonical/tensor_factory.cpp
  translator/transport-objects/resource_manager.cpp
  translator/transport-objects/concepts/transport_tensor.cpp
  translator/transport-objects/indices/indices.cpp
  translator/transport-objects/indices/index_flatten.cpp
  translator/transport-objects/indices/index_traits.cpp
  translator/transport-objects/utilities/symbol_factory.cpp
  translator/transport-objects/utilities/shared_resources.cpp
  translator/utilities/error.cpp
  translator/utilities/error_context.cpp
  translator/utilities/finder.cpp
  translator/utilities/formatter.cpp
  translator/utilities/ginac_print_indexed.cpp
  translator/utilities/to_printable.cpp
  translator/utilities/local_environment.cpp
  translator/utilities/argument_cache.cpp
  )

TARGET_LINK_LIBRARIES(CppTransport ${GINAC_LIBRARIES} ${Boost_LIBRARIES})
TARGET_COMPILE_OPTIONS(CppTransport PRIVATE -std=c++14)

TARGET_INCLUDE_DIRECTORIES(CppTransport PRIVATE
  ./
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
  translator/concepts
  translator/instruments
  translator/parser
  translator/transport-objects
  translator/transport-objects/indices
  translator/transport-objects/utilities
  translator/utilities
  ${Boost_INCLUDE_DIRS}
  ${GINAC_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  )


# pull in test target
ADD_SUBDIRECTORY(test-target EXCLUDE_FROM_ALL)


# install targets for CppTransport translator, cpptransport-templates, and header-only library/runtime API
INSTALL(TARGETS CppTransport DESTINATION bin)
INSTALL(DIRECTORY cpptransport-templates DESTINATION share/cpptransport)
INSTALL(DIRECTORY thirdparty/bootstrap DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/bootstrap-tab-history DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/jQuery DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/prism DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY thirdparty/DataTables DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY HTML_assets DESTINATION share/cpptransport/HTML)
INSTALL(DIRECTORY transport-runtime-api DESTINATION include)
INSTALL(FILES cmake/FindCppTransport.cmake DESTINATION share/cmake)


## FILL IN DUMMY TARGETS FOR CLION

set(CPPTRANSPORT_TEMPLATES_FILES
  cpptransport-templates/canonical_core.h
  cpptransport-templates/canonical_mpi.h
  )

set(CPPTRANSPORT_TEMPLATES_VEXCL_CUDA_FILES
  cpptransport-templates/vexcl-cuda/canonical_vexcl-cuda.h
  cpptransport-templates/vexcl-cuda/threepf.cu
  cpptransport-templates/vexcl-cuda/twopf.cu
  )

set(CPPTRANSPORT_TEMPLATES_VEXCL_OPENCL_FILES
  cpptransport-templates/vexcl-opencl/canonical_vexcl-opencl.h
  cpptransport-templates/vexcl-opencl/threepf.cl
  cpptransport-templates/vexcl-opencl/twopf.cl
  )

set(TOP_LEVEL_FILES
  )

set(TRANSLATOR_BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES
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

set(TRANSLATOR_BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES
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

set(TRANSLATOR_BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES
  translator/backends/infrastructure/macro_substitution/abstract_index.cpp
  translator/backends/infrastructure/macro_substitution/abstract_index.h
  translator/backends/infrastructure/macro_substitution/index_assignment.cpp
  translator/backends/infrastructure/macro_substitution/index_assignment.h
  translator/backends/infrastructure/macro_substitution/index_class.cpp
  translator/backends/infrastructure/macro_substitution/index_class.h
  translator/backends/infrastructure/macro_substitution/index_database.h
  translator/backends/infrastructure/macro_substitution/macro.cpp
  translator/backends/infrastructure/macro_substitution/macro.h
  translator/backends/infrastructure/macro_substitution/macro_tokenizer.cpp
  translator/backends/infrastructure/macro_substitution/macro_tokenizer.h
  translator/backends/infrastructure/macro_substitution/macro_types.h
  )

set(TRANSLATOR_BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES
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
  )

set(TRANSLATOR_BACKENDS_LANGUAGES_CPP_CORE_FILES
  translator/backends/languages/cpp/core/cpp_group.cpp
  translator/backends/languages/cpp/core/cpp_group.h
  translator/backends/languages/cpp/core/cpp_steppers.cpp
  translator/backends/languages/cpp/core/cpp_steppers.h
  )

set(TRANSLATOR_BACKENDS_LANGUAGES_CPP_FILES
  translator/backends/languages/cpp/cpp_cse.cpp
  translator/backends/languages/cpp/cpp_cse.h
  translator/backends/languages/cpp/cpp_printer.cpp
  translator/backends/languages/cpp/cpp_printer.h
  )

set(TRANSLATOR_BACKENDS_LANGUAGES_CPP_VEXCL_FILES
  translator/backends/languages/cpp/vexcl/vexcl_group.cpp
  translator/backends/languages/cpp/vexcl/vexcl_group.h
  translator/backends/languages/cpp/vexcl/vexcl_kernels.cpp
  translator/backends/languages/cpp/vexcl/vexcl_kernels.h
  translator/backends/languages/cpp/vexcl/vexcl_steppers.cpp
  translator/backends/languages/cpp/vexcl/vexcl_steppers.h
  )

set(TRANSLATOR_BACKENDS_LANGUAGES_CUDA_FILES
  translator/backends/languages/cuda/cuda_group.cpp
  translator/backends/languages/cuda/cuda_group.h
  translator/backends/languages/cuda/cuda_printer.cpp
  translator/backends/languages/cuda/cuda_printer.h
  )

set(TRANSLATOR_BACKENDS_LANGUAGES_OPENCL_FILES
  translator/backends/languages/opencl/opencl_group.cpp
  translator/backends/languages/opencl/opencl_group.h
  translator/backends/languages/opencl/opencl_printer.cpp
  translator/backends/languages/opencl/opencl_printer.h
  )

set(TRANSLATOR_BACKENDS_LANGUAGES_SHARED_FILES
  translator/backends/languages/shared/C_style_printer.cpp
  translator/backends/languages/shared/C_style_printer.h
  translator/backends/languages/shared/kernel_argument_macros.cpp
  translator/backends/languages/shared/kernel_argument_macros.h
  )

set(TRANSLATOR_BACKENDS_SHARED_FILES
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

set(TRANSLATOR_CONCEPTS_FILES
  translator/concepts/model_settings.h
  translator/concepts/stepper.h
  translator/concepts/symbol_list.h
  )

set(TRANSLATOR_FILES
  translator/core.h
  translator/defaults.h
  translator/main.cpp
  translator/msg_en.h
  translator/switches.h
  )

set(TRANSLATOR_INSTRUMENTS_FILES
  translator/instruments/timing_instrument.h
  )

set(TRANSLATOR_PARSER_BISON_FILES
  translator/parser/bison/y_parser.yy
  )

set(TRANSLATOR_PARSER_FILES
  translator/parser/input_stack.cpp
  translator/parser/input_stack.h
  translator/parser/lexeme.h
  translator/parser/lexfile.cpp
  translator/parser/lexfile.h
  translator/parser/lexical.h
  translator/parser/lexstream.h
  translator/parser/script.cpp
  translator/parser/script.h
  translator/parser/semantic_data.h
  translator/parser/y_common.h
  translator/parser/y_driver.cpp
  translator/parser/y_driver.h
  translator/parser/y_lexer.cpp
  translator/parser/y_lexer.h
  )

set(TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_FILES
  translator/transport-objects/canonical/A.cpp
  translator/transport-objects/canonical/A.h
  translator/transport-objects/canonical/B.cpp
  translator/transport-objects/canonical/B.h
  translator/transport-objects/canonical/C.cpp
  translator/transport-objects/canonical/C.h
  translator/transport-objects/canonical/dddV.cpp
  translator/transport-objects/canonical/dddV.h
  translator/transport-objects/canonical/ddV.cpp
  translator/transport-objects/canonical/ddV.h
  translator/transport-objects/canonical/dN1.cpp
  translator/transport-objects/canonical/dN1.h
  translator/transport-objects/canonical/dN2.cpp
  translator/transport-objects/canonical/dN2.h
  translator/transport-objects/canonical/dV.cpp
  translator/transport-objects/canonical/dV.h
  translator/transport-objects/canonical/Hubble.cpp
  translator/transport-objects/canonical/Hubble.h
  translator/transport-objects/canonical/M.cpp
  translator/transport-objects/canonical/M.h
  translator/transport-objects/canonical/resources.cpp
  translator/transport-objects/canonical/resources.h
  translator/transport-objects/canonical/SR_velocity.cpp
  translator/transport-objects/canonical/SR_velocity.h
  translator/transport-objects/canonical/tensor_factory.cpp
  translator/transport-objects/canonical/tensor_factory.h
  translator/transport-objects/canonical/u1.cpp
  translator/transport-objects/canonical/u1.h
  translator/transport-objects/canonical/u2.cpp
  translator/transport-objects/canonical/u2.h
  translator/transport-objects/canonical/u3.cpp
  translator/transport-objects/canonical/u3.h
  translator/transport-objects/canonical/zeta1.cpp
  translator/transport-objects/canonical/zeta1.h
  translator/transport-objects/canonical/zeta2.cpp
  translator/transport-objects/canonical/zeta2.h
  )

set(TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_FILES
  translator/transport-objects/concepts/A.h
  translator/transport-objects/concepts/B.h
  translator/transport-objects/concepts/C.h
  translator/transport-objects/concepts/dddV.h
  translator/transport-objects/concepts/ddV.h
  translator/transport-objects/concepts/dN1.h
  translator/transport-objects/concepts/dN2.h
  translator/transport-objects/concepts/dV.h
  translator/transport-objects/concepts/flattened_tensor.h
  translator/transport-objects/concepts/Hubble.h
  translator/transport-objects/concepts/M.h
  translator/transport-objects/concepts/SR_velocity.h
  translator/transport-objects/concepts/tensor_factory.h
  translator/transport-objects/concepts/transport_tensor.cpp
  translator/transport-objects/concepts/transport_tensor.h
  translator/transport-objects/concepts/u1.h
  translator/transport-objects/concepts/u2.h
  translator/transport-objects/concepts/u3.h
  translator/transport-objects/concepts/zeta1.h
  translator/transport-objects/concepts/zeta2.h
  )

set(TRANSLATOR_TRANSPORT_OBJECTS_DEPRECATED_FILES
  translator/transport-objects/deprecated/canonical_u_tensor_factory.cpp
  translator/transport-objects/deprecated/canonical_u_tensor_factory.h
  translator/transport-objects/deprecated/flatten.h
  translator/transport-objects/deprecated/make_u_tensor_factory.h
  translator/transport-objects/deprecated/u_tensor_factory.cpp
  translator/transport-objects/deprecated/u_tensor_factory.h
  )

set(TRANSLATOR_TRANSPORT_OBJECTS_FILES
  translator/transport-objects/make_tensor_factory.h
  translator/transport-objects/resource_manager.cpp
  translator/transport-objects/resource_manager.h
  )

set(TRANSLATOR_TRANSPORT_OBJECTS_INDICES_FILES
  translator/transport-objects/indices/index_flatten.cpp
  translator/transport-objects/indices/index_flatten.h
  translator/transport-objects/indices/index_traits.cpp
  translator/transport-objects/indices/index_traits.h
  translator/transport-objects/indices/indices.cpp
  translator/transport-objects/indices/indices.h
  )

set(TRANSLATOR_TRANSPORT_OBJECTS_UTILITIES_FILES
  translator/transport-objects/utilities/expression_cache.h
  translator/transport-objects/utilities/ginac_cache.h
  translator/transport-objects/utilities/shared_resources.cpp
  translator/transport-objects/utilities/shared_resources.h
  translator/transport-objects/utilities/symbol_factory.cpp
  translator/transport-objects/utilities/symbol_factory.h
  )

set(TRANSLATOR_UTILITIES_FILES
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
  )

set(TRANSPORT_RUNTIME_API_CONCEPTS_FILES
  transport-runtime-api/concepts/flattener.h
  transport-runtime-api/concepts/initial_conditions.h
  transport-runtime-api/concepts/initial_conditions_forward_declare.h
  transport-runtime-api/concepts/parameters.h
  transport-runtime-api/concepts/range.h
  )

set(TRANSPORT_RUNTIME_API_CONCEPTS_RANGE_DETAIL_FILES
  transport-runtime-api/concepts/range_detail/abstract.h
  transport-runtime-api/concepts/range_detail/aggregation.h
  transport-runtime-api/concepts/range_detail/common.h
  transport-runtime-api/concepts/range_detail/helper.h
  transport-runtime-api/concepts/range_detail/stepping.h
  )

set(TRANSPORT_RUNTIME_API_DATA_BATCHERS_FILES
  transport-runtime-api/data/batchers/batchers.h
  transport-runtime-api/data/batchers/container_dispatch_function.h
  transport-runtime-api/data/batchers/container_replace_function.h
  transport-runtime-api/data/batchers/generic_batcher.h
  transport-runtime-api/data/batchers/integration_batcher.h
  transport-runtime-api/data/batchers/integration_items.h
  transport-runtime-api/data/batchers/postintegration_batcher.h
  transport-runtime-api/data/batchers/postintegration_items.h
  transport-runtime-api/data/batchers/postprocess_delegate.h
  )

set(TRANSPORT_RUNTIME_API_DATA_DATAPIPE_FILES
  transport-runtime-api/data/datapipe/datapipe_decl.h
  transport-runtime-api/data/datapipe/datapipe_dispatch_function.h
  transport-runtime-api/data/datapipe/datapipe_forward_declare.h
  transport-runtime-api/data/datapipe/datapipe_impl.h
  transport-runtime-api/data/datapipe/linecache_specializations.h
  transport-runtime-api/data/datapipe/tags.h
  transport-runtime-api/data/datapipe/tags_forward_declare.h
  )

set(TRANSPORT_RUNTIME_API_DATA_DATAPIPE_TAGS_FILES
  transport-runtime-api/data/datapipe/tags/config_tags.h
  transport-runtime-api/data/datapipe/tags/field_tags.h
  transport-runtime-api/data/datapipe/tags/fNL_tags.h
  transport-runtime-api/data/datapipe/tags/stats_tags.h
  transport-runtime-api/data/datapipe/tags/zeta_tags.h
  )

set(TRANSPORT_RUNTIME_API_DATA_DETAIL_FILES
  transport-runtime-api/data/detail/data_manager_decl.h
  transport-runtime-api/data/detail/data_manager_impl.h
  )

set(TRANSPORT_RUNTIME_API_DATA_FILES
  transport-runtime-api/data/data_manager.h
  transport-runtime-api/data/metadata.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_CONVENIENCE_FILES
  transport-runtime-api/derived-products/convenience/time_series_plot.h
  transport-runtime-api/derived-products/convenience/time_series_table.h
  transport-runtime-api/derived-products/convenience/wavenumber_series_plot.h
  transport-runtime-api/derived-products/convenience/wavenumber_series_table.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES
  transport-runtime-api/derived-products/derived-content/background-quantites/background_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/largest_u2_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/largest_u3_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/u2_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/u3_line.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES
  transport-runtime-api/derived-products/derived-content/concepts/derived_line.h
  transport-runtime-api/derived-products/derived-content/concepts/derived_line_helper.h
  transport-runtime-api/derived-products/derived-content/concepts/derived_line_type.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES
  transport-runtime-api/derived-products/derived-content/concepts/lines/fNL_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/r_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/tensor_twopf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/threepf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/twopf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_reduced_bispectrum_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_threepf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_twopf_line.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES
  transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h
  transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/common.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/fNL_timeseries_compute.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/zeta_timeseries_compute.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES
  transport-runtime-api/derived-products/derived-content/correlation-functions/field_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/field_wavenumber_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/fNL_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/r_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/r_wavenumber_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/tensor_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/tensor_wavenumber_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/zeta_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/zeta_wavenumber_series.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES
  transport-runtime-api/derived-products/derived-content/deprecated/threepf_kconfig_shift.h
  transport-runtime-api/derived-products/derived-content/deprecated/threepf_time_shift.h
  transport-runtime-api/derived-products/derived-content/deprecated/zeta_kseries_compute.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES
  transport-runtime-api/derived-products/derived-content/integration-analysis/cost_metrics.h
  transport-runtime-api/derived-products/derived-content/integration-analysis/cost_wavenumber.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_threepf_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_time_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_twopf_query.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_policy.h
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query_helper.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES
  transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_FILES
  transport-runtime-api/derived-products/data_products.h
  transport-runtime-api/derived-products/derived_product.h
  transport-runtime-api/derived-products/derived_product_forward_declare.h
  transport-runtime-api/derived-products/derived_product_helper.h
  transport-runtime-api/derived-products/derived_product_type.h
  transport-runtime-api/derived-products/enumerations.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES
  transport-runtime-api/derived-products/line-collections/data_line.h
  transport-runtime-api/derived-products/line-collections/line_asciitable.h
  transport-runtime-api/derived-products/line-collections/line_collection.h
  transport-runtime-api/derived-products/line-collections/line_plot2d.h
  transport-runtime-api/derived-products/line-collections/line_values.h
  )

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_UTILITIES_FILES
  transport-runtime-api/derived-products/utilities/extractor.h
  transport-runtime-api/derived-products/utilities/index_selector.h
  transport-runtime-api/derived-products/utilities/wrapper.h
  )

set(TRANSPORT_RUNTIME_API_FILES
  transport-runtime-api/ansi_colour_codes.h
  transport-runtime-api/default_symbols.h
  transport-runtime-api/defaults.h
  transport-runtime-api/enumerations.h
  transport-runtime-api/exceptions.h
  transport-runtime-api/messages.h
  transport-runtime-api/transport.h
  transport-runtime-api/version.h
  )

set(TRANSPORT_RUNTIME_API_INSTRUMENTS_FILES
  transport-runtime-api/instruments/timing_instrument.h
  )

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_FILES
  transport-runtime-api/localizations/messages_en.h
  )

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES
  transport-runtime-api/localizations/messages_en/derived-products/derived_line.h
  transport-runtime-api/localizations/messages_en/derived-products/derived_product.h
  transport-runtime-api/localizations/messages_en/derived-products/fNL_line.h
  transport-runtime-api/localizations/messages_en/derived-products/line_asciitable.h
  transport-runtime-api/localizations/messages_en/derived-products/line_collection.h
  transport-runtime-api/localizations/messages_en/derived-products/line_plot2d.h
  transport-runtime-api/localizations/messages_en/derived-products/line_values.h
  transport-runtime-api/localizations/messages_en/derived-products/SQL_query.h
  transport-runtime-api/localizations/messages_en/derived-products/time_series.h
  transport-runtime-api/localizations/messages_en/derived-products/time_series_convenience.h
  transport-runtime-api/localizations/messages_en/derived-products/wavenumber_series.h
  transport-runtime-api/localizations/messages_en/derived-products/wavenumber_series_convenience.h
  )

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_FILES
  transport-runtime-api/localizations/messages_en/advisory_classes.h
  transport-runtime-api/localizations/messages_en/asciitable.h
  transport-runtime-api/localizations/messages_en/command_line.h
  transport-runtime-api/localizations/messages_en/data_manager.h
  transport-runtime-api/localizations/messages_en/initial_conditions.h
  transport-runtime-api/localizations/messages_en/integration.h
  transport-runtime-api/localizations/messages_en/json_repository_interface.h
  transport-runtime-api/localizations/messages_en/labels.h
  transport-runtime-api/localizations/messages_en/manager.h
  transport-runtime-api/localizations/messages_en/master_scheduler.h
  transport-runtime-api/localizations/messages_en/model_list.h
  transport-runtime-api/localizations/messages_en/model_manager.h
  transport-runtime-api/localizations/messages_en/models.h
  transport-runtime-api/localizations/messages_en/named_list.h
  transport-runtime-api/localizations/messages_en/parameters.h
  transport-runtime-api/localizations/messages_en/range.h
  transport-runtime-api/localizations/messages_en/report_cmdline.h
  transport-runtime-api/localizations/messages_en/report_HTML.h
  transport-runtime-api/localizations/messages_en/repository.h
  transport-runtime-api/localizations/messages_en/repository_records.h
  transport-runtime-api/localizations/messages_en/repository_sqlite3.h
  transport-runtime-api/localizations/messages_en/repository_toolkit.h
  transport-runtime-api/localizations/messages_en/runtimeapi.h
  transport-runtime-api/localizations/messages_en/scheduler.h
  transport-runtime-api/localizations/messages_en/serialization.h
  transport-runtime-api/localizations/messages_en/sqlite3_utility.h
  transport-runtime-api/localizations/messages_en/task_manager.h
  transport-runtime-api/localizations/messages_en/transaction_manager.h
  transport-runtime-api/localizations/messages_en/work_journal.h
  )

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES
  transport-runtime-api/localizations/messages_en/tasks/tasks.h
  transport-runtime-api/localizations/messages_en/tasks/threepf_config_database.h
  transport-runtime-api/localizations/messages_en/tasks/twopf_config_database.h
  )

set(TRANSPORT_RUNTIME_API_MANAGER_DETAIL_FILES
  transport-runtime-api/manager/detail/aggregation.h
  transport-runtime-api/manager/detail/aggregation_forward_declare.h
  transport-runtime-api/manager/detail/job_descriptors.h
  transport-runtime-api/manager/detail/master_controller_core.h
  transport-runtime-api/manager/detail/master_controller_decl.h
  transport-runtime-api/manager/detail/master_controller_integration.h
  transport-runtime-api/manager/detail/master_controller_output.h
  transport-runtime-api/manager/detail/master_controller_postintegration.h
  transport-runtime-api/manager/detail/slave_container_dispatch_decl.h
  transport-runtime-api/manager/detail/slave_container_dispatch_impl.h
  transport-runtime-api/manager/detail/slave_controller_decl.h
  transport-runtime-api/manager/detail/slave_controller_impl.h
  transport-runtime-api/manager/detail/slave_datapipe_dispatch_decl.h
  transport-runtime-api/manager/detail/slave_datapipe_dispatch_impl.h
  transport-runtime-api/manager/detail/slave_message_buffer.h
  )

set(TRANSPORT_RUNTIME_API_MANAGER_FILES
  transport-runtime-api/manager/argument_cache.h
  transport-runtime-api/manager/environment.h
  transport-runtime-api/manager/master_controller.h
  transport-runtime-api/manager/master_scheduler.h
  transport-runtime-api/manager/message_handlers.h
  transport-runtime-api/manager/model_manager.h
  transport-runtime-api/manager/mpi_operations.h
  transport-runtime-api/manager/slave_controller.h
  transport-runtime-api/manager/slave_work_handler.h
  transport-runtime-api/manager/task_gallery.h
  transport-runtime-api/manager/task_manager.h
  transport-runtime-api/manager/work_journal.h
  )

set(TRANSPORT_RUNTIME_API_MODELS_FILES
  transport-runtime-api/models/advisory_classes.h
  transport-runtime-api/models/canonical_model.h
  transport-runtime-api/models/model.h
  transport-runtime-api/models/model_forward_declare.h
  transport-runtime-api/models/observers.h
  )

set(TRANSPORT_RUNTIME_API_REPORTING_FILES
  transport-runtime-api/reporting/command_line.h
  transport-runtime-api/reporting/content_group_data.h
  transport-runtime-api/reporting/CSS_writer.h
  transport-runtime-api/reporting/HTML_report.h
  transport-runtime-api/reporting/HTML_report_bundle.h
  transport-runtime-api/reporting/HTML_writer.h
  transport-runtime-api/reporting/JavaScript_writer.h
  transport-runtime-api/reporting/key_value.h
  transport-runtime-api/reporting/unique_tags.h
  )

set(TRANSPORT_RUNTIME_API_REPOSITORY_DETAIL_FILES
  transport-runtime-api/repository/detail/repository_decl.h
  transport-runtime-api/repository/detail/repository_impl.h
  transport-runtime-api/repository/detail/repository_mode.h
  transport-runtime-api/repository/detail/writer_repo_decl.h
  transport-runtime-api/repository/detail/writer_repo_impl.h
  )

set(TRANSPORT_RUNTIME_API_REPOSITORY_FILES
  transport-runtime-api/repository/inflight.h
  transport-runtime-api/repository/json_repository.h
  transport-runtime-api/repository/repository.h
  transport-runtime-api/repository/repository_cache.h
  transport-runtime-api/repository/repository_toolkit.h
  )

set(TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_DETAIL_FILES
  transport-runtime-api/repository/records/detail/content_group_decl.h
  transport-runtime-api/repository/records/detail/content_group_impl.h
  transport-runtime-api/repository/records/detail/content_group_metadata_decl.h
  transport-runtime-api/repository/records/detail/content_group_metadata_impl.h
  transport-runtime-api/repository/records/detail/derived_product_decl.h
  transport-runtime-api/repository/records/detail/derived_product_impl.h
  transport-runtime-api/repository/records/detail/metadata_decl.h
  transport-runtime-api/repository/records/detail/metadata_impl.h
  transport-runtime-api/repository/records/detail/notes.h
  transport-runtime-api/repository/records/detail/package_decl.h
  transport-runtime-api/repository/records/detail/package_impl.h
  transport-runtime-api/repository/records/detail/record_decl.h
  transport-runtime-api/repository/records/detail/record_impl.h
  transport-runtime-api/repository/records/detail/task_decl.h
  transport-runtime-api/repository/records/detail/task_impl.h
  )

set(TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_FILES
  transport-runtime-api/repository/records/content_finder_decl.h
  transport-runtime-api/repository/records/content_finder_impl.h
  transport-runtime-api/repository/records/record_finder_decl.h
  transport-runtime-api/repository/records/record_finder_impl.h
  transport-runtime-api/repository/records/repository_records_decl.h
  transport-runtime-api/repository/records/repository_records_forward_declare.h
  transport-runtime-api/repository/records/repository_records_impl.h
  )

set(TRANSPORT_RUNTIME_API_REPOSITORY_WRITERS_FILES
  transport-runtime-api/repository/writers/derived_content_writer.h
  transport-runtime-api/repository/writers/generic_writer.h
  transport-runtime-api/repository/writers/integration_writer.h
  transport-runtime-api/repository/writers/postintegration_writer.h
  transport-runtime-api/repository/writers/writers.h
  )

set(TRANSPORT_RUNTIME_API_SCHEDULER_FILES
  transport-runtime-api/scheduler/context.h
  transport-runtime-api/scheduler/scheduler.h
  transport-runtime-api/scheduler/work_queue.h
  )

set(TRANSPORT_RUNTIME_API_SERIALIZATION_FILES
  transport-runtime-api/serialization/serializable.h
  )

set(TRANSPORT_RUNTIME_API_SQLITE3_DETAIL_FILES
  transport-runtime-api/sqlite3/detail/container_replace_decl.h
  transport-runtime-api/sqlite3/detail/container_replace_impl.h
  transport-runtime-api/sqlite3/detail/data_manager_sqlite3_decl.h
  transport-runtime-api/sqlite3/detail/data_manager_sqlite3_impl.h
  transport-runtime-api/sqlite3/detail/data_manager_transaction_handler_decl.h
  transport-runtime-api/sqlite3/detail/data_manager_transaction_handler_impl.h
  transport-runtime-api/sqlite3/detail/repository_sqlite3_decl.h
  transport-runtime-api/sqlite3/detail/repository_sqlite3_impl.h
  transport-runtime-api/sqlite3/detail/repository_transaction_handler_decl.h
  transport-runtime-api/sqlite3/detail/repository_transaction_handler_impl.h
  transport-runtime-api/sqlite3/detail/writer_data_mgr_decl.h
  transport-runtime-api/sqlite3/detail/writer_data_mgr_impl.h
  )

set(TRANSPORT_RUNTIME_API_SQLITE3_FILES
  transport-runtime-api/sqlite3/data_manager_sqlite3.h
  transport-runtime-api/sqlite3/repository_sqlite3.h
  )

set(TRANSPORT_RUNTIME_API_SQLITE3_OPERATIONS_FILES
  transport-runtime-api/sqlite3/operations/data_manager.h
  transport-runtime-api/sqlite3/operations/data_manager_aggregate.h
  transport-runtime-api/sqlite3/operations/data_manager_common.h
  transport-runtime-api/sqlite3/operations/data_manager_create.h
  transport-runtime-api/sqlite3/operations/data_manager_integrity.h
  transport-runtime-api/sqlite3/operations/data_manager_pull.h
  transport-runtime-api/sqlite3/operations/data_manager_read.h
  transport-runtime-api/sqlite3/operations/data_manager_write.h
  transport-runtime-api/sqlite3/operations/data_traits.h
  transport-runtime-api/sqlite3/operations/repository.h
  transport-runtime-api/sqlite3/operations/repository_admin.h
  transport-runtime-api/sqlite3/operations/repository_common.h
  transport-runtime-api/sqlite3/operations/repository_count.h
  transport-runtime-api/sqlite3/operations/repository_create.h
  transport-runtime-api/sqlite3/operations/repository_delete.h
  transport-runtime-api/sqlite3/operations/repository_inflight.h
  transport-runtime-api/sqlite3/operations/repository_store.h
  transport-runtime-api/sqlite3/operations/sqlite3_utility.h
  )

set(TRANSPORT_RUNTIME_API_TASKS_CONFIGURATION_DATABASE_FILES
  transport-runtime-api/tasks/configuration-database/generic_config_iterator.h
  transport-runtime-api/tasks/configuration-database/generic_record_iterator.h
  transport-runtime-api/tasks/configuration-database/generic_value_iterator.h
  transport-runtime-api/tasks/configuration-database/threepf_config_database.h
  transport-runtime-api/tasks/configuration-database/time_config_database.h
  transport-runtime-api/tasks/configuration-database/twopf_config_database.h
  )

set(TRANSPORT_RUNTIME_API_TASKS_FILES
  transport-runtime-api/tasks/derivable_task.h
  transport-runtime-api/tasks/integration_tasks.h
  transport-runtime-api/tasks/output_tasks.h
  transport-runtime-api/tasks/postintegration_tasks.h
  transport-runtime-api/tasks/task.h
  transport-runtime-api/tasks/task_configurations.h
  transport-runtime-api/tasks/task_configurations_forward_declare.h
  transport-runtime-api/tasks/task_helper.h
  transport-runtime-api/tasks/task_types.h
  transport-runtime-api/tasks/tasks_forward_declare.h
  )

set(TRANSPORT_RUNTIME_API_TASKS_INTEGRATION_DETAIL_FILES
  transport-runtime-api/tasks/integration_detail/abstract.h
  transport-runtime-api/tasks/integration_detail/background_task.h
  transport-runtime-api/tasks/integration_detail/common.h
  transport-runtime-api/tasks/integration_detail/default_policies.h
  transport-runtime-api/tasks/integration_detail/threepf_task.h
  transport-runtime-api/tasks/integration_detail/twopf_db_task.h
  transport-runtime-api/tasks/integration_detail/twopf_task.h
  )

set(TRANSPORT_RUNTIME_API_TASKS_POSTINTEGRATION_DETAIL_FILES
  transport-runtime-api/tasks/postintegration_detail/abstract.h
  transport-runtime-api/tasks/postintegration_detail/common.h
  transport-runtime-api/tasks/postintegration_detail/fNL_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_threepf_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_twopf_list_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_twopf_task.h
  )

set(TRANSPORT_RUNTIME_API_TRANSACTIONS_FILES
  transport-runtime-api/transactions/transaction_manager.h
  )

set(TRANSPORT_RUNTIME_API_UTILITIES_FILES
  transport-runtime-api/utilities/asciitable.h
  transport-runtime-api/utilities/formatter.h
  transport-runtime-api/utilities/host_information.h
  transport-runtime-api/utilities/latex_output.h
  transport-runtime-api/utilities/linecache.h
  transport-runtime-api/utilities/match.h
  transport-runtime-api/utilities/named_list.h
  transport-runtime-api/utilities/plot_environment.h
  transport-runtime-api/utilities/random_string.h
  transport-runtime-api/utilities/spline1d.h
  )

set(SOURCE_FILES
  ${CPPTRANSPORT_TEMPLATES_FILES}
  ${CPPTRANSPORT_TEMPLATES_VEXCL_CUDA_FILES}
  ${CPPTRANSPORT_TEMPLATES_VEXCL_OPENCL_FILES}
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
  ${TRANSLATOR_CONCEPTS_FILES}
  ${TRANSLATOR_FILES}
  ${TRANSLATOR_INSTRUMENTS_FILES}
  ${TRANSLATOR_PARSER_BISON_FILES}
  ${TRANSLATOR_PARSER_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CANONICAL_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_CONCEPTS_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_DEPRECATED_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_INDICES_FILES}
  ${TRANSLATOR_TRANSPORT_OBJECTS_UTILITIES_FILES}
  ${TRANSLATOR_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_API_CONCEPTS_RANGE_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_BATCHERS_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DATAPIPE_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DATAPIPE_TAGS_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_CONVENIENCE_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_FILES}
  ${TRANSPORT_RUNTIME_API_INSTRUMENTS_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES}
  ${TRANSPORT_RUNTIME_API_MANAGER_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_MANAGER_FILES}
  ${TRANSPORT_RUNTIME_API_MODELS_FILES}
  ${TRANSPORT_RUNTIME_API_REPORTING_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_WRITERS_FILES}
  ${TRANSPORT_RUNTIME_API_SCHEDULER_FILES}
  ${TRANSPORT_RUNTIME_API_SERIALIZATION_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_OPERATIONS_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_CONFIGURATION_DATABASE_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_INTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_POSTINTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_TRANSACTIONS_FILES}
  ${TRANSPORT_RUNTIME_API_UTILITIES_FILES}
  )

ADD_EXECUTABLE(dummy_clion_target EXCLUDE_FROM_ALL ${SOURCE_FILES})
