CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(higgs_goldstone)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/Users/SeanButchers/.cpptransport201701/share/cmake/")

SET(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "-Ofast -DNDEBUG")

FIND_PACKAGE(CppTransport REQUIRED)

INCLUDE_DIRECTORIES(${CPPTRANSPORT_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

ADD_CUSTOM_COMMAND(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/higgs_goldstone_core.h ${CMAKE_CURRENT_BINARY_DIR}/higgs_goldstone_mpi.h
  COMMAND CppTransport --verbose --fast ${CMAKE_CURRENT_SOURCE_DIR}/higgs_goldstone.model
  DEPENDS higgs_goldstone.model
)

SET(HEADERS ${CMAKE_CURRENT_BINARY_DIR}/higgs_goldstone_core.h ${CMAKE_CURRENT_BINARY_DIR}/higgs_goldstone_mpi.h)
ADD_CUSTOM_TARGET(Generator DEPENDS ${HEADERS})

ADD_EXECUTABLE(higgs_goldstone higgs_goldstone.cpp)
ADD_DEPENDENCIES(higgs_goldstone Generator)
TARGET_LINK_LIBRARIES(higgs_goldstone ${CPPTRANSPORT_LIBRARIES})
TARGET_COMPILE_OPTIONS(higgs_goldstone PRIVATE -std=c++14 -mavx)