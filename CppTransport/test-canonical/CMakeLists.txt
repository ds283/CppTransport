CMAKE_MINIMUM_REQUIRED(VERSION 3.0)


PROJECT(test-canonical)


# don't use FindCppTransport package; want to pull in headers from the source tree,
# not from any already installed elsewhere on the system
#FIND_PACKAGE(CppTransport REQUIRED)
#FIND_PACKAGE(MPI REQUIRED)
#FIND_PACKAGE(Boost 1.56 REQUIRED COMPONENTS system filesystem random timer date_time mpi log_setup log serialization thread program_options regex)
#find_package(CUDA REQUIRED)
#find_package(OpenCL REQUIRED)


SET(CPPTRANSPORT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)


# main SPLINTER includes have a different relative position in the build tree
# compared to the install tree, so they are pulled through 'SPLINTER' symlink in top-level directory.
# For that we only need CPPTRANSPORT_INCLUDE_DIR which here equals ..
# JsonCPP includes work fine, since they have the same relative position in the build
# tree and the source tree
# We also need to dig out the include path for Eigen from the bundled SPLINTER repository
SET(
  CPPTRANSPORT_INCLUDE_DIRS
  ${CPPTRANSPORT_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIRS}
  ${SPLINTER_INCLUDE_DIRS}
  ${SPLINTER_INCLUDE_DIRS}/SPLINTER
  ${ODEINT_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}
  )


SET(CPPTRANSPORT_LIBRARIES ${JSONCPP_LIBRARIES} ${SPLINTER_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/axion_core.h ${CMAKE_CURRENT_BINARY_DIR}/axion_mpi.h
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../CppTransport --verbose --profile --Wdevelop --Wunroll --no-search-env -I ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/axion.model
  DEPENDS axion.model
  DEPENDS defaults.model
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../templates/canonical_mpi.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../templates/canonical_core.h
  DEPENDS CppTransport
)


SET(
  AXION_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/axion_core.h
  ${CMAKE_CURRENT_BINARY_DIR}/axion_mpi.h
)


ADD_CUSTOM_TARGET(AxionGenerator DEPENDS ${AXION_HEADERS})


ADD_EXECUTABLE(test-canonical test-canonical.cpp)
ADD_DEPENDENCIES(test-canonical AxionGenerator)

TARGET_LINK_LIBRARIES(test-canonical sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${CPPTRANSPORT_LIBRARIES})
#SET_TARGET_PROPERTIES(test-canonical PROPERTIES
#  CXX_STANDARD 14
#  CXX_STANDARD_REQUIRED YES
#  CXX_EXTENSIONS NO
#  )
TARGET_COMPILE_OPTIONS(test-canonical PRIVATE -std=c++14 -ftemplate-backtrace-limit=0)


TARGET_INCLUDE_DIRECTORIES(
  test-canonical PRIVATE
  ${CPPTRANSPORT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Boost_INCLUDE_DIRS}
  ${MPI_CXX_INCLUDE_PATH}
  ${OPENCL_INCLUDE_DIR}
  )
