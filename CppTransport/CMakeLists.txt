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
FIND_PACKAGE(Boost 1.56 REQUIRED COMPONENTS system filesystem random timer date_time mpi log_setup log serialization thread program_options)
FIND_PACKAGE(GiNaC 1.6 REQUIRED)


# build parser
BISON_TARGET(parser parser/bison/y_parser.yy ${CMAKE_CURRENT_BINARY_DIR}/y_parser.cpp)


ADD_EXECUTABLE(CppTransport
  main.cpp
  backends/infrastructure/file_handling/backend_data.cpp
  backends/infrastructure/file_handling/buffer.cpp
  backends/infrastructure/file_handling/output_stack.cpp
  backends/infrastructure/file_handling/translation_unit.cpp
  backends/infrastructure/file_handling/translator.cpp
  backends/infrastructure/file_handling/translator_data.cpp
  backends/infrastructure/file_handling/lexstream_data.cpp
  backends/infrastructure/language_concepts/cse.cpp
  backends/infrastructure/language_concepts/cse_map.cpp
  backends/infrastructure/language_concepts/lambdas.cpp
  backends/infrastructure/language_concepts/lambda_manager.cpp
  backends/infrastructure/macro_substitution/index_assignment.cpp
  backends/infrastructure/macro_substitution/macro.cpp
  backends/infrastructure/macro_substitution/macro_tokenizer.cpp
  backends/infrastructure/macro_substitution/abstract_index.cpp
  backends/infrastructure/macro_substitution/index_class.cpp
  backends/infrastructure/replacement_rules/cse_map_field_indices.cpp
  backends/infrastructure/replacement_rules/cse_map_phase_indices.cpp
  backends/infrastructure/replacement_rules/package_group.cpp
  backends/infrastructure/replacement_rules/package_group_factory.cpp
  backends/infrastructure/replacement_rules/replacement_rule_package.cpp
  backends/infrastructure/replacement_rules/replacement_rule_definitions.cpp
  backends/languages/cpp/core/cpp_group.cpp
  backends/languages/cpp/core/cpp_steppers.cpp
  backends/languages/cpp/vexcl/vexcl_group.cpp
  backends/languages/cpp/vexcl/vexcl_kernels.cpp
  backends/languages/cpp/vexcl/vexcl_steppers.cpp
  backends/languages/cpp/cpp_cse.cpp
  backends/languages/cpp/cpp_printer.cpp
  backends/languages/cuda/cuda_group.cpp
  backends/languages/cuda/cuda_printer.cpp
  backends/languages/opencl/opencl_group.cpp
  backends/languages/opencl/opencl_printer.cpp
  backends/languages/shared/C_style_printer.cpp
  backends/languages/shared/kernel_argument_macros.cpp
  backends/shared/directives.cpp
  backends/shared/flow_tensors.cpp
  backends/shared/fundamental.cpp
  backends/shared/gauge_xfm.cpp
  backends/shared/lagrangian_tensors.cpp
  backends/shared/resources.cpp
  backends/shared/temporary_pool.cpp
  backends/shared/utensors.cpp
  ${BISON_parser_OUTPUTS}
  parser/input_stack.cpp
  parser/lexfile.cpp
  parser/script.cpp
  parser/semantic_data.cpp
  parser/y_driver.cpp
  parser/y_lexer.cpp
  transport-objects/canonical/resources.cpp
  transport-objects/canonical/Hubble.cpp
  transport-objects/canonical/dV.cpp
  transport-objects/canonical/ddV.cpp
  transport-objects/canonical/dddV.cpp
  transport-objects/canonical/SR_velocity.cpp
  transport-objects/canonical/A.cpp
  transport-objects/canonical/B.cpp
  transport-objects/canonical/C.cpp
  transport-objects/canonical/M.cpp
  transport-objects/canonical/u1.cpp
  transport-objects/canonical/u2.cpp
  transport-objects/canonical/u3.cpp
  transport-objects/canonical/zeta1.cpp
  transport-objects/canonical/zeta2.cpp
  transport-objects/canonical/dN1.cpp
  transport-objects/canonical/dN2.cpp
  transport-objects/canonical/tensor_factory.cpp
  transport-objects/resource_manager.cpp
  transport-objects/concepts/transport_tensor.cpp
  transport-objects/indices/indices.cpp
  transport-objects/indices/index_flatten.cpp
  transport-objects/indices/index_traits.cpp
  transport-objects/utilities/symbol_factory.cpp
  transport-objects/utilities/shared_resources.cpp
  utilities/error.cpp
  utilities/error_context.cpp
  utilities/finder.cpp
  utilities/formatter.cpp
  utilities/ginac_print_indexed.cpp
  utilities/to_printable.cpp
  utilities/local_environment.cpp
  utilities/argument_cache.cpp
  )

TARGET_LINK_LIBRARIES(CppTransport ${GINAC_LIBRARIES} ${Boost_LIBRARIES})
TARGET_COMPILE_OPTIONS(CppTransport PRIVATE -std=c++14)

TARGET_INCLUDE_DIRECTORIES(CppTransport PRIVATE
  ./
  backends
  backends/infrastructure
  backends/infrastructure/file_handling
  backends/infrastructure/language_concepts
  backends/infrastructure/macro_substitution
  backends/infrastructure/replacement_rules
  backends/languages
  backends/languages/cpp
  backends/languages/cpp/core
  backends/languages/cpp/vexcl
  backends/languages/cuda
  backends/languages/opencl
  backends/languages/shared
  backends/shared
  concepts
  instruments
  parser
  transport-objects
  transport-objects/indices
  transport-objects/utilities
  utilities
  ${Boost_INCLUDE_DIRS}
  ${GINAC_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  )


# pull in test target
ADD_SUBDIRECTORY(test-target EXCLUDE_FROM_ALL)


# install targets for CppTransport translator, cpptransport-templates, and header-only library/runtime API
INSTALL(TARGETS CppTransport DESTINATION bin)
INSTALL(DIRECTORY cpptransport-templates DESTINATION share)
INSTALL(DIRECTORY transport-runtime-api DESTINATION include)
INSTALL(FILES cmake/FindCppTransport.cmake DESTINATION share/cmake)


## FILL IN DUMMY TARGETS FOR CLION

SET(BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES
  backends/infrastructure/file_handling/backend_data.cpp
  backends/infrastructure/file_handling/backend_data.h
  backends/infrastructure/file_handling/buffer.cpp
  backends/infrastructure/file_handling/buffer.h
  backends/infrastructure/file_handling/contexted_value.h
  backends/infrastructure/file_handling/filestack.h
  backends/infrastructure/file_handling/lexstream_data.cpp
  backends/infrastructure/file_handling/lexstream_data.h
  backends/infrastructure/file_handling/output_stack.cpp
  backends/infrastructure/file_handling/output_stack.h
  backends/infrastructure/file_handling/translation_unit.cpp
  backends/infrastructure/file_handling/translation_unit.h
  backends/infrastructure/file_handling/translator.cpp
  backends/infrastructure/file_handling/translator.h
  backends/infrastructure/file_handling/translator_data.cpp
  backends/infrastructure/file_handling/translator_data.h
  )

SET(BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES
  backends/infrastructure/language_concepts/cse.cpp
  backends/infrastructure/language_concepts/cse.h
  backends/infrastructure/language_concepts/cse_map.cpp
  backends/infrastructure/language_concepts/cse_map.h
  backends/infrastructure/language_concepts/language_printer.h
  backends/infrastructure/language_concepts/lambdas.cpp
  backends/infrastructure/language_concepts/lambdas.h
  backends/infrastructure/language_concepts/lambda_manager.h
  backends/infrastructure/language_concepts/lambda_manager.cpp
  )

SET(BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES
  backends/infrastructure/macro_substitution/abstract_index.cpp
  backends/infrastructure/macro_substitution/abstract_index.h
  backends/infrastructure/macro_substitution/index_assignment.cpp
  backends/infrastructure/macro_substitution/index_assignment.h
  backends/infrastructure/macro_substitution/index_database.h
  backends/infrastructure/macro_substitution/index_class.cpp
  backends/infrastructure/macro_substitution/index_class.h
  backends/infrastructure/macro_substitution/macro.cpp
  backends/infrastructure/macro_substitution/macro.h
  backends/infrastructure/macro_substitution/macro_tokenizer.cpp
  backends/infrastructure/macro_substitution/macro_tokenizer.h
  backends/infrastructure/macro_substitution/macro_types.h
  )

SET(BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES
  backends/infrastructure/replacement_rules/package_group.cpp
  backends/infrastructure/replacement_rules/package_group.h
  backends/infrastructure/replacement_rules/package_group_factory.cpp
  backends/infrastructure/replacement_rules/package_group_factory.h
  backends/infrastructure/replacement_rules/replacement_rule_definitions.cpp
  backends/infrastructure/replacement_rules/replacement_rule_definitions.h
  backends/infrastructure/replacement_rules/replacement_rule_package.cpp
  backends/infrastructure/replacement_rules/replacement_rule_package.h
  backends/infrastructure/replacement_rules/cse_map_phase_indices.cpp
  backends/infrastructure/replacement_rules/cse_map_phase_indices.h
  backends/infrastructure/replacement_rules/cse_map_field_indices.cpp
  backends/infrastructure/replacement_rules/cse_map_field_indices.h
  )

SET(BACKENDS_LANGUAGES_CPP_CORE_FILES
  backends/languages/cpp/core/cpp_group.cpp
  backends/languages/cpp/core/cpp_group.h
  backends/languages/cpp/core/cpp_steppers.cpp
  backends/languages/cpp/core/cpp_steppers.h
  )

SET(BACKENDS_LANGUAGES_CPP_FILES
  backends/languages/cpp/cpp_cse.cpp
  backends/languages/cpp/cpp_cse.h
  backends/languages/cpp/cpp_printer.cpp
  backends/languages/cpp/cpp_printer.h
  )

SET(BACKENDS_LANGUAGES_CPP_VEXCL_FILES
  backends/languages/cpp/vexcl/vexcl_group.cpp
  backends/languages/cpp/vexcl/vexcl_group.h
  backends/languages/cpp/vexcl/vexcl_kernels.cpp
  backends/languages/cpp/vexcl/vexcl_kernels.h
  backends/languages/cpp/vexcl/vexcl_steppers.cpp
  backends/languages/cpp/vexcl/vexcl_steppers.h
  )

SET(BACKENDS_LANGUAGES_CUDA_FILES
  backends/languages/cuda/cuda_group.cpp
  backends/languages/cuda/cuda_group.h
  backends/languages/cuda/cuda_printer.cpp
  backends/languages/cuda/cuda_printer.h
  )

SET(BACKENDS_LANGUAGES_OPENCL_FILES
  backends/languages/opencl/opencl_group.cpp
  backends/languages/opencl/opencl_group.h
  backends/languages/opencl/opencl_printer.cpp
  backends/languages/opencl/opencl_printer.h
  )

SET(BACKENDS_LANGUAGES_SHARED_FILES
  backends/languages/shared/C_style_printer.cpp
  backends/languages/shared/C_style_printer.h
  backends/languages/shared/kernel_argument_macros.cpp
  backends/languages/shared/kernel_argument_macros.h
  )

SET(BACKENDS_SHARED_FILES
  backends/shared/flow_tensors.cpp
  backends/shared/flow_tensors.h
  backends/shared/fundamental.cpp
  backends/shared/fundamental.h
  backends/shared/gauge_xfm.cpp
  backends/shared/gauge_xfm.h
  backends/shared/lagrangian_tensors.cpp
  backends/shared/lagrangian_tensors.h
  backends/shared/resources.cpp
  backends/shared/resources.h
  backends/shared/temporary_pool.cpp
  backends/shared/temporary_pool.h
  backends/shared/utensors.cpp
  backends/shared/utensors.h
  backends/shared/directives.cpp
  backends/shared/directives.h
  )

SET(CONCEPTS_FILES
  concepts/model_settings.h
  concepts/stepper.h
  concepts/symbol_list.h
  )

SET(INSTRUMENTS_FILES
  instruments/timing_instrument.h
  )

SET(PARSER_BISON_FILES
  parser/bison/y_parser.yy
  )

SET(PARSER_FILES
  parser/input_stack.cpp
  parser/input_stack.h
  parser/lexeme.h
  parser/lexfile.cpp
  parser/lexfile.h
  parser/lexical.h
  parser/lexstream.h
  parser/script.cpp
  parser/script.h
  parser/semantic_data.cpp
  parser/semantic_data.h
  parser/y_common.h
  parser/y_driver.cpp
  parser/y_driver.h
  parser/y_lexer.cpp
  parser/y_lexer.h
  )

SET(TEMPLATES_FILES
  cpptransport-templates/canonical_mpi.h
  cpptransport-templates/canonical_core.h
  )

SET(TEMPLATES_VEXCL_CUDA_FILES
  cpptransport-templates/vexcl-cuda/threepf.cu
  cpptransport-templates/vexcl-cuda/twopf.cu
  )

SET(TEMPLATES_VEXCL_OPENCL_FILES
  cpptransport-templates/vexcl-opencl/threepf.cl
  cpptransport-templates/vexcl-opencl/twopf.cl
  )

SET(TEST_TARGET_FILES
  test-target/CMakeLists.txt
  test-target/test_target.cpp
  )

SET(TOP_LEVEL_FILES
  CMakeLists.txt
  core.h
  CppTransport.1
  main.cpp
  msg_en.h
  switches.h
  )

SET(TRANSPORT_OBJECTS_CANONICAL_FILES
  transport-objects/canonical/resources.cpp
  transport-objects/canonical/resources.h
  transport-objects/canonical/Hubble.cpp
  transport-objects/canonical/Hubble.h
  transport-objects/canonical/dV.cpp
  transport-objects/canonical/dV.h
  transport-objects/canonical/ddV.cpp
  transport-objects/canonical/ddV.h
  transport-objects/canonical/dddV.cpp
  transport-objects/canonical/dddV.h
  transport-objects/canonical/SR_velocity.cpp
  transport-objects/canonical/SR_velocity.h
  transport-objects/canonical/A.cpp
  transport-objects/canonical/A.h
  transport-objects/canonical/B.cpp
  transport-objects/canonical/B.h
  transport-objects/canonical/C.cpp
  transport-objects/canonical/C.h
  transport-objects/canonical/M.cpp
  transport-objects/canonical/M.h
  transport-objects/canonical/u1.cpp
  transport-objects/canonical/u1.h
  transport-objects/canonical/u2.cpp
  transport-objects/canonical/u2.h
  transport-objects/canonical/u3.cpp
  transport-objects/canonical/u3.h
  transport-objects/canonical/zeta1.cpp
  transport-objects/canonical/zeta1.h
  transport-objects/canonical/zeta2.cpp
  transport-objects/canonical/zeta2.h
  transport-objects/canonical/dN1.cpp
  transport-objects/canonical/dN1.h
  transport-objects/canonical/dN2.cpp
  transport-objects/canonical/dN2.h
  transport-objects/canonical/tensor_factory.cpp
  transport-objects/canonical/tensor_factory.h
  )

SET(TRANSPORT_OBJECTS_CONCEPTS_FILES
  transport-objects/concepts/transport_tensor.cpp
  transport-objects/concepts/transport_tensor.h
  transport-objects/concepts/Hubble.h
  transport-objects/concepts/dV.h
  transport-objects/concepts/ddV.h
  transport-objects/concepts/dddV.h
  transport-objects/concepts/SR_velocity.h
  transport-objects/concepts/A.h
  transport-objects/concepts/B.h
  transport-objects/concepts/C.h
  transport-objects/concepts/M.h
  transport-objects/concepts/u1.h
  transport-objects/concepts/u2.h
  transport-objects/concepts/u3.h
  transport-objects/concepts/zeta1.h
  transport-objects/concepts/zeta2.h
  transport-objects/concepts/dN1.h
  transport-objects/concepts/dN2.h
  transport-objects/concepts/flattened_tensor.h
  transport-objects/concepts/tensor_factory.h
  )

SET(TRANSPORT_OBJECTS_FILES
  transport-objects/resource_manager.cpp
  transport-objects/resource_manager.h
  transport-objects/make_tensor_factory.h)

SET(TRANSPORT_OBJECTS_INDICES_FILES
  transport-objects/indices/indices.cpp
  transport-objects/indices/indices.h
  transport-objects/indices/index_flatten.h
  transport-objects/indices/index_flatten.cpp
  transport-objects/indices/index_traits.cpp
  transport-objects/indices/index_traits.h
  )

SET(TRANSPORT_OBJECTS_UTILITIES_FILES
  transport-objects/utilities/expression_cache.h
  transport-objects/utilities/ginac_cache.h
  transport-objects/utilities/shared_resources.cpp
  transport-objects/utilities/shared_resources.h
  transport-objects/utilities/symbol_factory.cpp
  transport-objects/utilities/symbol_factory.h
  )

SET(TRANSPORT_RUNTIME_API_CONCEPTS_FILES
  transport-runtime-api/concepts/flattener.h
  transport-runtime-api/concepts/initial_conditions.h
  transport-runtime-api/concepts/initial_conditions_forward_declare.h
  transport-runtime-api/concepts/parameters.h
  transport-runtime-api/concepts/range.h
  )

SET(TRANSPORT_RUNTIME_API_CONCEPTS_RANGE_DETAIL_FILES
  transport-runtime-api/concepts/range_detail/abstract.h
  transport-runtime-api/concepts/range_detail/aggregation.h
  transport-runtime-api/concepts/range_detail/common.h
  transport-runtime-api/concepts/range_detail/helper.h
  transport-runtime-api/concepts/range_detail/stepping.h
  )

SET(TRANSPORT_RUNTIME_API_DATA_BATCHERS_FILES
  transport-runtime-api/data/batchers/batchers.h
  transport-runtime-api/data/batchers/generic_batcher.h
  transport-runtime-api/data/batchers/integration_batcher.h
  transport-runtime-api/data/batchers/integration_items.h
  transport-runtime-api/data/batchers/postintegration_batcher.h
  transport-runtime-api/data/batchers/postintegration_items.h
  transport-runtime-api/data/batchers/postprocess_delegate.h
  )

SET(TRANSPORT_RUNTIME_API_DATA_DATAPIPE_FILES
  transport-runtime-api/data/datapipe/datapipe.h
  transport-runtime-api/data/datapipe/datapipe_detail.h
  transport-runtime-api/data/datapipe/datapipe_forward_declare.h
  transport-runtime-api/data/datapipe/linecache_specializations.h
  transport-runtime-api/data/datapipe/tags.h
  transport-runtime-api/data/datapipe/tags_forward_declare.h
  )

SET(TRANSPORT_RUNTIME_API_DATA_DATAPIPE_TAGS_FILES
  transport-runtime-api/data/datapipe/tags/config_tags.h
  transport-runtime-api/data/datapipe/tags/field_tags.h
  transport-runtime-api/data/datapipe/tags/fNL_tags.h
  transport-runtime-api/data/datapipe/tags/stats_tags.h
  transport-runtime-api/data/datapipe/tags/zeta_tags.h
  )

SET(TRANSPORT_RUNTIME_API_DATA_FILES
  transport-runtime-api/data/data_manager.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_CONVENIENCE_FILES
  transport-runtime-api/derived-products/convenience/time_series_plot.h
  transport-runtime-api/derived-products/convenience/time_series_table.h
  transport-runtime-api/derived-products/convenience/wavenumber_series_plot.h
  transport-runtime-api/derived-products/convenience/wavenumber_series_table.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES
  transport-runtime-api/derived-products/derived-content/background-quantites/background_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/largest_u2_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/largest_u3_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/u2_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/u3_line.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES
  transport-runtime-api/derived-products/derived-content/concepts/derived_line.h
  transport-runtime-api/derived-products/derived-content/concepts/derived_line_helper.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES
  transport-runtime-api/derived-products/derived-content/concepts/lines/fNL_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/r_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/tensor_twopf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/threepf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/twopf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_reduced_bispectrum_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_threepf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_twopf_line.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES
  transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h
  transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/common.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/fNL_timeseries_compute.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/zeta_timeseries_compute.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES
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

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES
  transport-runtime-api/derived-products/derived-content/deprecated/threepf_kconfig_shift.h
  transport-runtime-api/derived-products/derived-content/deprecated/threepf_time_shift.h
  transport-runtime-api/derived-products/derived-content/deprecated/zeta_kseries_compute.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES
  transport-runtime-api/derived-products/derived-content/integration-analysis/cost_metrics.h
  transport-runtime-api/derived-products/derived-content/integration-analysis/cost_wavenumber.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_threepf_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_time_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_twopf_query.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_policy.h
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query_helper.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES
  transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_FILES
  transport-runtime-api/derived-products/data_products.h
  transport-runtime-api/derived-products/derived_product.h
  transport-runtime-api/derived-products/derived_product_forward_declare.h
  transport-runtime-api/derived-products/derived_product_helper.h
  transport-runtime-api/derived-products/enumerations.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES
  transport-runtime-api/derived-products/line-collections/data_line.h
  transport-runtime-api/derived-products/line-collections/line_asciitable.h
  transport-runtime-api/derived-products/line-collections/line_collection.h
  transport-runtime-api/derived-products/line-collections/line_plot2d.h
  transport-runtime-api/derived-products/line-collections/line_values.h
  )

SET(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_UTILITIES_FILES
  transport-runtime-api/derived-products/utilities/extractor.h
  transport-runtime-api/derived-products/utilities/index_selector.h
  transport-runtime-api/derived-products/utilities/wrapper.h
  )

SET(TRANSPORT_RUNTIME_API_INSTRUMENTS_FILES
  transport-runtime-api/instruments/timing_instrument.h
  )

SET(TRANSPORT_RUNTIME_API_FILES
  transport-runtime-api/ansi_colour_codes.h
  transport-runtime-api/default_symbols.h
  transport-runtime-api/defaults.h
  transport-runtime-api/enumerations.h
  transport-runtime-api/exceptions.h
  transport-runtime-api/messages.h
  transport-runtime-api/transport.h
  transport-runtime-api/version.h
  )

SET(TRANSPORT_RUNTIME_API_LOCALIZATIONS_FILES
  transport-runtime-api/localizations/messages_en.h
  )

SET(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES
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

SET(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_FILES
  transport-runtime-api/localizations/messages_en/advisory_classes.h
  transport-runtime-api/localizations/messages_en/command_line.h
  transport-runtime-api/localizations/messages_en/data_manager.h
  transport-runtime-api/localizations/messages_en/initial_conditions.h
  transport-runtime-api/localizations/messages_en/instance_manager.h
  transport-runtime-api/localizations/messages_en/integration.h
  transport-runtime-api/localizations/messages_en/json_repository_interface.h
  transport-runtime-api/localizations/messages_en/labels.h
  transport-runtime-api/localizations/messages_en/manager.h
  transport-runtime-api/localizations/messages_en/master_scheduler.h
  transport-runtime-api/localizations/messages_en/model_list.h
  transport-runtime-api/localizations/messages_en/models.h
  transport-runtime-api/localizations/messages_en/named_list.h
  transport-runtime-api/localizations/messages_en/parameters.h
  transport-runtime-api/localizations/messages_en/range.h
  transport-runtime-api/localizations/messages_en/repository.h
  transport-runtime-api/localizations/messages_en/repository_sqlite3.h
  transport-runtime-api/localizations/messages_en/runtimeapi.h
  transport-runtime-api/localizations/messages_en/scheduler.h
  transport-runtime-api/localizations/messages_en/serialization.h
  transport-runtime-api/localizations/messages_en/sqlite3_utility.h
  transport-runtime-api/localizations/messages_en/task_manager.h
  transport-runtime-api/localizations/messages_en/work_journal.h
  )

SET(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES
  transport-runtime-api/localizations/messages_en/tasks/tasks.h
  transport-runtime-api/localizations/messages_en/tasks/threepf_config_database.h
  transport-runtime-api/localizations/messages_en/tasks/twopf_config_database.h
  )

SET(TRANSPORT_RUNTIME_API_MANAGER_FILES
  transport-runtime-api/manager/argument_cache.h
  transport-runtime-api/manager/environment.h
  transport-runtime-api/manager/instance_manager.h
  transport-runtime-api/manager/master_controller.h
  transport-runtime-api/manager/master_scheduler.h
  transport-runtime-api/manager/mpi_operations.h
  transport-runtime-api/manager/slave_controller.h
  transport-runtime-api/manager/slave_work_handler.h
  transport-runtime-api/manager/task_manager.h
  transport-runtime-api/manager/work_journal.h
  )

SET(TRANSPORT_RUNTIME_API_MODELS_FILES
  transport-runtime-api/models/advisory_classes.h
  transport-runtime-api/models/canonical_model.h
  transport-runtime-api/models/model.h
  transport-runtime-api/models/model_forward_declare.h
  transport-runtime-api/models/observers.h
  )

SET(TRANSPORT_RUNTIME_API_REPOSITORY_FILES
  transport-runtime-api/repository/json_repository.h
  transport-runtime-api/repository/repository.h
  transport-runtime-api/repository/transaction_manager.h
  )

SET(TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_FILES
  transport-runtime-api/repository/records/record_finder.h
  transport-runtime-api/repository/records/repository_records.h
  transport-runtime-api/repository/records/repository_records_forward_declare.h
  )

SET(TRANSPORT_RUNTIME_API_REPOSITORY_WRITERS_FILES
  transport-runtime-api/repository/writers/derived_content_writer.h
  transport-runtime-api/repository/writers/generic_writer.h
  transport-runtime-api/repository/writers/integration_writer.h
  transport-runtime-api/repository/writers/postintegration_writer.h
  transport-runtime-api/repository/writers/writers.h
  )

SET(TRANSPORT_RUNTIME_API_SCHEDULER_FILES
  transport-runtime-api/scheduler/context.h
  transport-runtime-api/scheduler/scheduler.h
  transport-runtime-api/scheduler/work_queue.h
  )

SET(TRANSPORT_RUNTIME_API_SERIALIZATION_FILES
  transport-runtime-api/serialization/serializable.h
  )

SET(TRANSPORT_RUNTIME_API_SQLITE3_FILES
  transport-runtime-api/sqlite3/data_manager_sqlite3.h
  transport-runtime-api/sqlite3/repository_sqlite3.h
  )

SET(TRANSPORT_RUNTIME_API_SQLITE3_OPERATIONS_FILES
  transport-runtime-api/sqlite3/operations/data_manager.h
  transport-runtime-api/sqlite3/operations/data_manager_aggregate.h
  transport-runtime-api/sqlite3/operations/data_manager_common.h
  transport-runtime-api/sqlite3/operations/data_manager_create.h
  transport-runtime-api/sqlite3/operations/data_manager_integrity.h
  transport-runtime-api/sqlite3/operations/data_manager_pull.h
  transport-runtime-api/sqlite3/operations/data_manager_temporaries.h
  transport-runtime-api/sqlite3/operations/data_manager_write.h
  transport-runtime-api/sqlite3/operations/data_traits.h
  transport-runtime-api/sqlite3/operations/repository.h
  transport-runtime-api/sqlite3/operations/repository_admin.h
  transport-runtime-api/sqlite3/operations/repository_common.h
  transport-runtime-api/sqlite3/operations/repository_count.h
  transport-runtime-api/sqlite3/operations/repository_create.h
  transport-runtime-api/sqlite3/operations/repository_inflight.h
  transport-runtime-api/sqlite3/operations/repository_store.h
  transport-runtime-api/sqlite3/operations/sqlite3_utility.h
  )

SET(TRANSPORT_RUNTIME_API_TASKS_CONFIGURATION_DATABASE_FILES
  transport-runtime-api/tasks/configuration-database/generic_config_iterator.h
  transport-runtime-api/tasks/configuration-database/generic_record_iterator.h
  transport-runtime-api/tasks/configuration-database/generic_value_iterator.h
  transport-runtime-api/tasks/configuration-database/threepf_config_database.h
  transport-runtime-api/tasks/configuration-database/time_config_database.h
  transport-runtime-api/tasks/configuration-database/twopf_config_database.h
  )

SET(TRANSPORT_RUNTIME_API_TASKS_FILES
  transport-runtime-api/tasks/derivable_task.h
  transport-runtime-api/tasks/integration_tasks.h
  transport-runtime-api/tasks/output_tasks.h
  transport-runtime-api/tasks/postintegration_tasks.h
  transport-runtime-api/tasks/task.h
  transport-runtime-api/tasks/task_configurations.h
  transport-runtime-api/tasks/task_configurations_forward_declare.h
  transport-runtime-api/tasks/task_helper.h
  transport-runtime-api/tasks/tasks_forward_declare.h
  )

SET(TRANSPORT_RUNTIME_API_TASKS_INTEGRATION_DETAIL_FILES
  transport-runtime-api/tasks/integration_detail/abstract.h
  transport-runtime-api/tasks/integration_detail/background_task.h
  transport-runtime-api/tasks/integration_detail/common.h
  transport-runtime-api/tasks/integration_detail/threepf_task.h
  transport-runtime-api/tasks/integration_detail/twopf_list_task.h
  transport-runtime-api/tasks/integration_detail/twopf_task.h
  transport-runtime-api/tasks/integration_detail/default_policies.h
  )

SET(TRANSPORT_RUNTIME_API_TASKS_POSTINTEGRATION_DETAIL_FILES
  transport-runtime-api/tasks/postintegration_detail/abstract.h
  transport-runtime-api/tasks/postintegration_detail/common.h
  transport-runtime-api/tasks/postintegration_detail/fNL_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_threepf_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_twopf_list_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_twopf_task.h
  )

SET(TRANSPORT_RUNTIME_API_UTILITIES_FILES
  transport-runtime-api/utilities/asciitable.h
  transport-runtime-api/utilities/formatter.h
  transport-runtime-api/utilities/host_information.h
  transport-runtime-api/utilities/latex_output.h
  transport-runtime-api/utilities/linecache.h
  transport-runtime-api/utilities/named_list.h
  transport-runtime-api/utilities/python_finder.h
  transport-runtime-api/utilities/random_string.h
  transport-runtime-api/utilities/spline1d.h
  )

SET(UTILITIES_FILES
  utilities/argument_cache.cpp
  utilities/argument_cache.h
  utilities/error.cpp
  utilities/error.h
  utilities/error_context.cpp
  utilities/error_context.h
  utilities/finder.cpp
  utilities/finder.h
  utilities/formatter.cpp
  utilities/formatter.h
  utilities/local_environment.cpp
  utilities/local_environment.h
  utilities/to_printable.cpp
  utilities/to_printable.h
  utilities/ginac_print_indexed.cpp
  utilities/ginac_print_indexed.h
  )

SET(SOURCE_FILES
  ${BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES}
  ${BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES}
  ${BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES}
  ${BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES}
  ${BACKENDS_LANGUAGES_CPP_CORE_FILES}
  ${BACKENDS_LANGUAGES_CPP_FILES}
  ${BACKENDS_LANGUAGES_CPP_VEXCL_FILES}
  ${BACKENDS_LANGUAGES_CUDA_FILES}
  ${BACKENDS_LANGUAGES_OPENCL_FILES}
  ${BACKENDS_LANGUAGES_SHARED_FILES}
  ${BACKENDS_SHARED_FILES}
  ${CONCEPTS_FILES}
  ${INSTRUMENTS_FILES}
  ${PARSER_BISON_FILES}
  ${PARSER_FILES}
  ${TEMPLATES_FILES}
  ${TEMPLATES_VEXCL_CUDA_FILES}
  ${TEMPLATES_VEXCL_OPENCL_FILES}
  ${TEST_TARGET_FILES}
  ${TOP_LEVEL_FILES}
  ${TRANSPORT_OBJECTS_CANONICAL_FILES}
  ${TRANSPORT_OBJECTS_CONCEPTS_FILES}
  ${TRANSPORT_OBJECTS_FILES}
  ${TRANSPORT_OBJECTS_INDICES_FILES}
  ${TRANSPORT_OBJECTS_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_API_CONCEPTS_RANGE_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_BATCHERS_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DATAPIPE_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DATAPIPE_TAGS_FILES}
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
  ${TRANSPORT_RUNTIME_API_MANAGER_FILES}
  ${TRANSPORT_RUNTIME_API_MODELS_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_WRITERS_FILES}
  ${TRANSPORT_RUNTIME_API_SCHEDULER_FILES}
  ${TRANSPORT_RUNTIME_API_SERIALIZATION_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_OPERATIONS_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_CONFIGURATION_DATABASE_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_INTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_POSTINTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_UTILITIES_FILES}
  ${UTILITIES_FILES}
  )

ADD_EXECUTABLE(dummy_clion_target EXCLUDE_FROM_ALL ${SOURCE_FILES})
