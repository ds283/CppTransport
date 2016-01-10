CMAKE_MINIMUM_REQUIRED(VERSION 3.0)


PROJECT(test-target)


# don't use FindCppTransport package; want to pull in headers from the source tree,
# not from any already installed elsewhere on the system
#FIND_PACKAGE(CppTransport REQUIRED)
FIND_PACKAGE(MPI REQUIRED)
FIND_PACKAGE(Boost 1.56 REQUIRED COMPONENTS system filesystem random timer date_time mpi log_setup log serialization thread program_options)
#find_package(CUDA REQUIRED)
#find_package(OpenCL REQUIRED)


SET(CPPTRANSPORT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
SET(CPPTRANSPORT_JSONCPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/jsoncpp/include)
SET(CPPTRANSPORT_SPLINTER_EIGEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/splinter/thirdparty/Eigen)


# main SPLINTER includes have a different relative position in the build tree
# compared to the install tree, so they are pulled through 'SPLINTER' symlink in top-level directory.
# For that we only need CPPTRANSPORT_INCLUDE_DIR which here equals ..
# JsonCPP includes work fine, since they have the same relative position in the build
# tree and the source tree
# We also need to dig the include path for Eigen out of the bundled SPLINTER repository
SET(
  CPPTRANSPORT_INCLUDE_DIRS
  ${CPPTRANSPORT_INCLUDE_DIR}
  ${CPPTRANSPORT_JSONCPP_INCLUDE_DIR}
  ${CPPTRANSPORT_SPLINTER_EIGEN_DIR}
  )


SET(CPPTRANSPORT_LIBRARIES jsoncpp_lib_static splinter-static-2-0)


ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/axion_core.h ${CMAKE_CURRENT_BINARY_DIR}/axion_mpi.h
  COMMAND CppTransport --verbose --no-search-env -I ${CMAKE_CURRENT_SOURCE_DIR}/../cpptransport-templates ${CMAKE_CURRENT_SOURCE_DIR}/axion.model
  DEPENDS axion.model
  DEPENDS defaults.model
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../cpptransport-templates/canonical_mpi.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../cpptransport-templates/canonical_core.h
)


SET(
  AXION_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/axion_core.h
  ${CMAKE_CURRENT_BINARY_DIR}/axion_mpi.h
)


ADD_CUSTOM_TARGET(AxionGenerator DEPENDS ${AXION_HEADERS})


ADD_EXECUTABLE(test-target test_target.cpp)
ADD_DEPENDENCIES(test-target AxionGenerator)

TARGET_LINK_LIBRARIES(test-target sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${CPPTRANSPORT_LIBRARIES})
TARGET_COMPILE_OPTIONS(test-target PRIVATE -std=c++14)


TARGET_INCLUDE_DIRECTORIES(
  test-target PRIVATE
  ${CPPTRANSPORT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Boost_INCLUDE_DIRS}
  ${MPI_CXX_INCLUDE_PATH}
  ${OPENCL_INCLUDE_DIR}
  )
