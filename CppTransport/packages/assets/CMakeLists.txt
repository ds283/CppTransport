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
BUILD_ODEINT(${CMAKE_BINARY_DIR}/thirdparty/odeint-v2)

IF(NOT GINAC_FOUND)
  # GiNaC was not found, so attempt to build it from source
  BUILD_CLN(${CMAKE_BINARY_DIR}/thirdparty/CLN ${EXTERNAL_INSTALL_PREFIX})
  BUILD_GINAC(${CMAKE_BINARY_DIR}/thirdparty/GiNaC ${CLN_INCLUDE_DIRS} ${CLN_LIBRARY_DIRS} ${CLN_LIBRARIES} ${EXTERNAL_INSTALL_PREFIX})
ENDIF()


# configure build_data.h
STRING(TIMESTAMP CONFIG_TIMESTAMP)
CONFIGURE_FILE(configure_assets/build_data.h.in ${CMAKE_BINARY_DIR}/build_data.h)


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
  translator/parser/y_parser.cpp
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
SET_TARGET_PROPERTIES(CppTransport PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  )

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
