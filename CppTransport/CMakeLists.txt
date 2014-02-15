cmake_minimum_required(VERSION 2.6)

project(CppTransport)

find_package(MPI REQUIRED)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /home/ds283/Documents/Code/dbxml-2.5.16/install/lib)
add_definitions(-std=c++11 -g)

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
					   concepts
					   parser
					   parser/bison
					   "transport objects"
					   utilities
					/usr/include
					/usr/local/include
					/home/ds283/Documents/Code/dbxml-2.5.16/install/include)


add_executable(CppTransport main.cpp
							"backends/infrastructure/file handling/buffer.cpp"
							"backends/infrastructure/file handling/output_stack.cpp"
							"backends/infrastructure/file handling/translation_unit.cpp"
							"backends/infrastructure/file handling/translator.cpp"
							"backends/infrastructure/language concepts/cse.cpp"
							"backends/infrastructure/macro substitution/index_assignment.cpp"
							"backends/infrastructure/macro substitution/macro.cpp"
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
							utilities/to_printable.cpp)

target_link_libraries(CppTransport ginac cln boost_system boost_filesystem boost_timer boost_date_time)


add_executable(dquad-mpi examples/dquad-mpi/main.cpp)

target_link_libraries(dquad-mpi sqlite3 boost_system boost_filesystem boost_random boost_timer boost_date_time boost_mpi boost_log_setup boost_log boost_serialization boost_thread)
target_link_libraries(dquad-mpi /home/ds283/Documents/Code/dbxml-2.5.16/install/lib/libdbxml.so)
target_link_libraries(dquad-mpi /home/ds283/Documents/Code/dbxml-2.5.16/install/lib/libdb_cxx.so)

add_executable(dquad-makerepo examples/dquad-mpi/makerepo.cpp)

target_link_libraries(dquad-makerepo sqlite3 boost_system boost_filesystem boost_random boost_timer boost_date_time boost_mpi boost_log_setup boost_log boost_serialization boost_thread)
target_link_libraries(dquad-makerepo /home/ds283/Documents/Code/dbxml-2.5.16/install/lib/libdbxml.so)
target_link_libraries(dquad-makerepo /home/ds283/Documents/Code/dbxml-2.5.16/install/lib/libdb_cxx.so)
