cmake_minimum_required(VERSION 2.6)

project(CppTransport)

SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )
SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/splinter/cmake/" )

ADD_SUBDIRECTORY ( "splinter" "splinter-build" )

find_package(MPI REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem random timer date_time mpi log_setup log serialization thread)
find_package(GiNaC REQUIRED)
find_package(Eigen3 REQUIRED)
#find_package(CUDA REQUIRED)
#find_package(OpenCL REQUIRED)

SET ( CMAKE_CXX_FLAGS "-std=c++11 -Ofast -mavx" CACHE STRING "compile flags" FORCE )
#SET ( CMAKE_CXX_FLAGS "-std=c++11 -O1" CACHE STRING "compile flags" FORCE )
SET ( CMAKE_C_FLAGS "-Ofast -mavx" CACHE STRING "compile flags" FORCE )

include_directories(./ backends
					   backends/infrastructure
					   "backends/infrastructure/file handling"
					   "backends/infrastructure/language concepts"
					   "backends/infrastructure/macro substitution"
					   "backends/infrastructure/replacement rules"
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
					   "transport objects"
					   utilities
					   transport-runtime-api
					   jsoncpp
					   splinter/include
					   odeint/include
					/usr/include
					/usr/local/include
					${EIGEN3_INCLUDE_DIR}
					${Boost_INCLUDE_DIRS}
					${MPI_CXX_INCLUDE_PATH}
					${GINAC_INCLUDE_DIRS}
					${OPENCL_INCLUDE_DIR})


add_executable(CppTransport main.cpp
							"backends/infrastructure/file handling/buffer.cpp"
							"backends/infrastructure/file handling/output_stack.cpp"
							"backends/infrastructure/file handling/translation_unit.cpp"
							"backends/infrastructure/file handling/translator.cpp"
							"backends/infrastructure/language concepts/cse.cpp"
							"backends/infrastructure/macro substitution/index_assignment.cpp"
							"backends/infrastructure/macro substitution/macro.cpp"
							"backends/infrastructure/macro substitution/macro_tokenizer.cpp"
							"backends/infrastructure/replacement rules/package_group.cpp"
							"backends/infrastructure/replacement rules/package_group_factory.cpp"
							"backends/infrastructure/replacement rules/replacement_rule_package.cpp"
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
							concepts/filestack.cpp
							parser/bison/y_parser.tab.cc
							parser/input_stack.cpp
							parser/lexfile.cpp
							parser/parse_tree.cpp
							parser/quantity.cpp
							parser/y_driver.cpp
							parser/y_lexer.cpp
							"transport objects/canonical_u_tensor_factory.cpp"
							"transport objects/u_tensor_factory.cpp"
							utilities/basic_error.cpp
							utilities/error.cpp
							utilities/finder.cpp
							utilities/formatter.cpp
							utilities/to_printable.cpp)

target_link_libraries(CppTransport ${GINAC_LIBRARIES} ${Boost_LIBRARIES})


add_executable(dquad-mpi examples/dquad-mpi/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(dquad-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(dquad-makerepo examples/dquad-mpi/makerepo.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(dquad-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(dquad-makerepo2 examples/dquad-mpi/makerepo2.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(dquad-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(quadratic-mpi examples/quadratic/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(quadratic-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(quadratic-makerepo examples/quadratic/makerepo.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(quadratic-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(axion-mpi examples/axion-mpi/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(axion-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(axion-makerepo examples/axion-mpi/makerepo.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(axion-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(axion-makerepo2 examples/axion-mpi/makerepo2.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(axion-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(axion-makerepo3 examples/axion-mpi/makerepo3.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(axion-makerepo3 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(new-axion-mpi examples/new-axion/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(new-axion-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(new-axion-makerepo2 examples/new-axion/makerepo2.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(new-axion-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(mafalda40-makerepo examples/mafalda40/makerepo.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(mafalda40-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(mafalda40-mpi examples/mafalda40/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(mafalda40-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(quadratic10-makerepo examples/quadratic10/makerepo.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(quadratic10-makerepo sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(quadratic10-mpi examples/quadratic10/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(quadratic10-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2)


add_executable(powerlaw-makerepo2 examples/powerlaw/makerepo2.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(powerlaw-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARIES})


add_executable(powerlaw-mpi examples/powerlaw/main.cpp jsoncpp/jsoncpp.cpp)
target_link_libraries(powerlaw-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARIES})


# cuda_add_executable(dquad-cuda examples/dquad-vexcl/main.cpp jsoncpp/jsoncpp.cpp)
# target_link_libraries(dquad-cuda sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} splinter-static-1-2 /usr/local/CUDA/lib/libcuda.dylib)


# add_executable(dquad-opencl examples/dquad-vexcl/main.cpp jsoncpp/jsoncpp.cpp)
# target_link_libraries(dquad-opencl sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${OPENCL_LIBRARIES} splinter-static-1-2)
