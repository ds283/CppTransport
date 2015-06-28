cmake_minimum_required(VERSION 2.6)

project(CppTransport)

SET( CMAKE_CXX_FLAGS_COVERAGE "-prof-gen=srcpos -prof-dir /Users/ds283/Documents/Code/CppTransport/profiled" CACHE STRING
    "Flags used by the C++ compiler during code coverage builds."
    FORCE )
SET( CMAKE_C_FLAGS_COVERAGE "-prof-gen=srcpos -prof-dir /Users/ds283/Documents/Code/CppTransport/profiled" CACHE STRING
    "Flags used by the C compiler during code coverage builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "-prof-gen=srcpos -prof-dir /Users/ds283/Documents/Code/CppTransport/profiled" CACHE STRING
    "Flags used for linking binaries during code coverage builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "-prof-gen=srcpos -prof-dir /Users/ds283/Documents/Code/CppTransport/profiled" CACHE STRING
    "Flags used by the shared libraries linker during code coverage builds."
    FORCE )

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_C_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE )

SET( CMAKE_CXX_FLAGS_PROFILE "-g -Ofast" CACHE STRING
    "Flags used by the C++ compiler during profiling builds."
    FORCE )
SET( CMAKE_C_FLAGS_PROFILE "-g -Ofast" CACHE STRING
    "Flags used by the C compiler during profiling builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_PROFILE
    "-g -Ofast" CACHE STRING
    "Flags used for linking binaries during profiling builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_PROFILE
    "-g -Ofast" CACHE STRING
    "Flags used by the shared libraries linker during profiling builds."
    FORCE )

MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_PROFILE
    CMAKE_C_FLAGS_PROFILE
    CMAKE_EXE_LINKER_FLAGS_PROFILE
    CMAKE_SHARED_LINKER_FLAGS_PROFILE )

SET( CMAKE_CXX_FLAGS_DEBUG "-g" CACHE STRING
    "Flags used by the C++ compiler during debug builds."
    FORCE )
SET( CMAKE_C_FLAGS_DEBUG "-g" CACHE STRING
    "Flags used by the C compiler during debug builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_DEBUG
    "-g" CACHE STRING
    "Flags used for linking binaries during debug builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_DEBUG
    "-g" CACHE STRING
    "Flags used by the shared libraries linker during debug builds."
    FORCE )

SET( CMAKE_CXX_FLAGS_RELEASE "-Ofast" CACHE STRING
    "Flags used by the C++ compiler during release builds."
    FORCE )
SET( CMAKE_C_FLAGS_RELEASE "-Ofast" CACHE STRING
    "Flags used by the C compiler during release builds."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_RELEASE
    "-Ofast" CACHE STRING
    "Flags used for linking binaries during release builds."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_RELEASE
    "-Ofast" CACHE STRING
    "Flags used by the shared libraries linker during release builds."
    FORCE )

# Update the documentation string of CMAKE_BUILD_TYPE for GUIs
SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage Profile."
    FORCE )

SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )
SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/splinter/cmake/" )

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mavx" CACHE STRING "compile flags" FORCE )
SET ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -mavx"              CACHE STRING "compile flags" FORCE )

SET ( JSONCPP_AMALGAMATED "transport-runtime-api/thirdparty/jsoncpp/jsoncpp.cpp" )
SET ( SPLINTER_LIBRARY    "splinter-static-1-3" )
SET ( JSONCPP_LIBRARY     "jsoncpp_lib_static" )

# use version of Eigen3 bundled with SPLINTER
SET ( EIGEN3_INCLUDE_DIR  "transport-runtime-api/thirdparty/splinter/thirdparty/Eigen" )

set(DEFAULT_BUILD_TYPE "Release")

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE})
endif()
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

# pull in bundled SPLINTER sources; on most platforms will need to switch default build to 64-bit
set ( BITNESS "64" )
ADD_SUBDIRECTORY ( "transport-runtime-api/thirdparty/splinter" "splinter-build" )

# pull in bundles libjsoncpp sources
set ( BUILD_STATIC_LIBS )
ADD_SUBDIRECTORY ( "transport-runtime-api/thirdparty/jsoncpp"  "jsoncpp-build" )

find_package(MPI REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem random timer date_time mpi log_setup log serialization thread program_options)
find_package(GiNaC REQUIRED)
#find_package(CUDA REQUIRED)
#find_package(OpenCL REQUIRED)


include_directories(./ backends
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
					   backends/utilities
					   concepts
					   parser
					   parser/bison
					   transport-objects
					   utilities
					   transport-runtime-api
					   transport-runtime-api/thirdparty/jsoncpp/include
					   transport-runtime-api/thirdparty/splinter/include
					   transport-runtime-api/thirdparty/odeint/include
					/usr/include
					/usr/local/include
					${EIGEN3_INCLUDE_DIR}
					${Boost_INCLUDE_DIRS}
					${MPI_CXX_INCLUDE_PATH}
					${GINAC_INCLUDE_DIRS}
					${OPENCL_INCLUDE_DIR})


add_executable(CppTransport main.cpp
							backends/infrastructure/file_handling/buffer.cpp
							backends/infrastructure/file_handling/output_stack.cpp
							backends/infrastructure/file_handling/translation_unit.cpp
							backends/infrastructure/file_handling/translator.cpp
							backends/infrastructure/language_concepts/cse.cpp
							backends/infrastructure/macro_substitution/index_assignment.cpp
							backends/infrastructure/macro_substitution/macro.cpp
							backends/infrastructure/macro_substitution/macro_tokenizer.cpp
							backends/infrastructure/replacement_rules/package_group.cpp
							backends/infrastructure/replacement_rules/package_group_factory.cpp
							backends/infrastructure/replacement_rules/replacement_rule_package.cpp
							backends/languages/cpp/core/core_group.cpp
							backends/languages/cpp/core/core_macros.cpp
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
							backends/shared/flow_tensors.cpp
							backends/shared/fundamental.cpp
							backends/shared/gauge_xfm.cpp
							backends/shared/lagrangian_tensors.cpp
							backends/shared/summation.cpp
							backends/shared/temporary_pool.cpp
							backends/shared/utensors.cpp
							backends/utilities/symbol_factory.cpp
							parser/bison/y_parser.tab.cc
							parser/input_stack.cpp
							parser/lexfile.cpp
							parser/script.cpp
							parser/semantic_data.cpp
							parser/y_driver.cpp
							parser/y_lexer.cpp
							transport-objects/canonical_u_tensor_factory.cpp
							transport-objects/u_tensor_factory.cpp
							utilities/error.cpp
							utilities/finder.cpp
							utilities/formatter.cpp
							utilities/to_printable.cpp)

target_link_libraries(CppTransport ${GINAC_LIBRARIES} ${Boost_LIBRARIES})


add_executable(dquad-mpi examples/dquad-mpi/main.cpp )
target_link_libraries(dquad-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(dquad-makerepo examples/dquad-mpi/makerepo.cpp )
target_link_libraries(dquad-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(dquad-makerepo2 examples/dquad-mpi/makerepo2.cpp )
target_link_libraries(dquad-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(quadratic-mpi examples/quadratic/main.cpp )
target_link_libraries(quadratic-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(quadratic-makerepo examples/quadratic/makerepo.cpp )
target_link_libraries(quadratic-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(axion-mpi examples/axion-mpi/main.cpp )
target_link_libraries(axion-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(axion-makerepo examples/axion-mpi/makerepo.cpp )
target_link_libraries(axion-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(axion-makerepo2 examples/axion-mpi/makerepo2.cpp )
target_link_libraries(axion-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(axion-makerepo3 examples/axion-mpi/makerepo3.cpp )
target_link_libraries(axion-makerepo3 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(new-axion-mpi examples/new-axion/main.cpp )
target_link_libraries(new-axion-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(new-axion-makerepo2 examples/new-axion/makerepo2.cpp )
target_link_libraries(new-axion-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(qsfi-mpi examples/qsfi/main.cpp )
target_link_libraries(qsfi-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(qsfi-makerepo2 examples/qsfi/makerepo2.cpp )
target_link_libraries(qsfi-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(mafalda40-makerepo examples/mafalda40/makerepo.cpp )
target_link_libraries(mafalda40-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(mafalda40-mpi examples/mafalda40/main.cpp )
target_link_libraries(mafalda40-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(quadratic10-makerepo examples/quadratic10/makerepo.cpp )
target_link_libraries(quadratic10-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(quadratic10-mpi examples/quadratic10/main.cpp )
target_link_libraries(quadratic10-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(powerlaw-makerepo2 examples/powerlaw/makerepo2.cpp )
target_link_libraries(powerlaw-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(powerlaw-mpi examples/powerlaw/main.cpp )
target_link_libraries(powerlaw-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(heavy-hilltop-makerepo2 examples/heavy-hilltop/makerepo2.cpp )
target_link_libraries(heavy-hilltop-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(heavy-hilltop-mpi examples/heavy-hilltop/main.cpp )
target_link_libraries(heavy-hilltop-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(inflection-makerepo2 examples/inflection/makerepo2.cpp )
target_link_libraries(inflection-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(inflection-mpi examples/inflection/main.cpp )
target_link_libraries(inflection-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(chris-makerepo2 examples/chris-mpi/makerepo2.cpp )
target_link_libraries(chris-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(chris-makerepo3 examples/chris-mpi/makerepo3.cpp )
target_link_libraries(chris-makerepo3 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(chris-mpi examples/chris-mpi/main.cpp )
target_link_libraries(chris-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


# cuda_add_executable(dquad-cuda examples/dquad-vexcl/main.cpp )
# target_link_libraries(dquad-cuda sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY} /usr/local/CUDA/lib/libcuda.dylib)


# add_executable(dquad-opencl examples/dquad-vexcl/main.cpp )
# target_link_libraries(dquad-opencl sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${OPENCL_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})
