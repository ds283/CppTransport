CMAKE_MINIMUM_REQUIRED(VERSION 3.0)


PROJECT(test-nontrivial-metric)


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
  ${CMAKE_BINARY_DIR}
  )


SET(CPPTRANSPORT_LIBRARIES ${JSONCPP_LIBRARIES} ${SPLINTER_LIBRARIES})


ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gelaton_core.h ${CMAKE_CURRENT_BINARY_DIR}/gelaton_mpi.h
  COMMAND CppTransport --verbose --profile --Wdevelop --Wunroll --no-search-env -I ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/gelaton.model
  DEPENDS gelaton.model
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../templates/nontrivial_metric_mpi.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../templates/nontrivial_metric_core.h
  DEPENDS CppTransport
)


SET(
  GELATON_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/gelaton_core.h
  ${CMAKE_CURRENT_BINARY_DIR}/gelaton_mpi.h
)


ADD_CUSTOM_TARGET(GelatonGenerator DEPENDS ${GELATON_HEADERS})


ADD_EXECUTABLE(test-nontrivial-metric test-nontrivial-metric.cpp)
ADD_DEPENDENCIES(test-nontrivial-metric GelatonGenerator)

TARGET_LINK_LIBRARIES(test-nontrivial-metric sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${CPPTRANSPORT_LIBRARIES})
TARGET_COMPILE_OPTIONS(test-nontrivial-metric PRIVATE -std=c++14 -ftemplate-backtrace-limit=0)


TARGET_INCLUDE_DIRECTORIES(
  test-nontrivial-metric PRIVATE
  ${CPPTRANSPORT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Boost_INCLUDE_DIRS}
  ${MPI_CXX_INCLUDE_PATH}
  ${OPENCL_INCLUDE_DIR}
  )
