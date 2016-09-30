CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(CppTransport)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)
INCLUDE(BuildDeps)

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

  # suppress missing override warnings which are automatically enabled in recent
  # versions of Clang. These produce a slew of warnings from GiNaC >= 1.7
  # also suppress unknown attribute warnings which produce errors for GiNaC_deprecated
  SET(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG -Wno-inconsistent-missing-override -Wno-unknown-attributes")
  SET(CMAKE_C_FLAGS_RELEASE "-Ofast -Wno-inconsistent-missing-override -Wno-unknown-attributes")

ELSEIF(GCC)

  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  SET(CMAKE_C_FLAGS_RELEASE "-O3")

ELSEIF(INTEL)

  SET(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
  SET(CMAKE_C_FLAGS_RELEASE "-Ofast")

ENDIF()


# find MPI library
FIND_PACKAGE(MPI REQUIRED)

# find required Boost libraries
FIND_PACKAGE(Boost REQUIRED COMPONENTS log system filesystem random timer date_time mpi thread serialization program_options regex)

# find GiNaC library
FIND_PACKAGE(GiNaC REQUIRED)

# find OpenSSL libraries
FIND_PACKAGE(OpenSSL REQUIRED)


# handle non-standard external dependencies
ADD_CUSTOM_TARGET(DEPS ALL)
SET(EXTERNAL_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/thirdparty/dist")

BUILD_JSONCPP(${CMAKE_BINARY_DIR}/thirdparty/jsoncpp ${EXTERNAL_INSTALL_PREFIX})
BUILD_SPLINTER(${CMAKE_BINARY_DIR}/thirdparty/SPLINTER ${EXTERNAL_INSTALL_PREFIX})


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
  translator/parser/y_parser.cpp
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
  translator/transport-objects/canonical/Atilde.cpp
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

ADD_DEPENDENCIES(CppTransport DEPS)

TARGET_LINK_LIBRARIES(CppTransport ${GINAC_LIBRARIES} ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
TARGET_COMPILE_OPTIONS(CppTransport PRIVATE -std=c++14)

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

# emplace JsonCpp and SPLINTER assets into runtime system folders
INSTALL(DIRECTORY ${EXTERNAL_INSTALL_PREFIX}/include/json DESTINATION include/cpptransport)
INSTALL(DIRECTORY ${EXTERNAL_INSTALL_PREFIX}/include/SPLINTER DESTINATION include/cpptransport)
INSTALL(DIRECTORY ${EXTERNAL_INSTALL_PREFIX}/lib DESTINATION lib/cpptransport)

# install CMake finder script for all these assets
CONFIGURE_FILE(cmake/FindCppTransport.cmake.in ${CMAKE_BINARY_DIR}/FindCppTransport.cmake @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/FindCppTransport.cmake DESTINATION share/cmake)
