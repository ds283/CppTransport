cmake_minimum_required(VERSION 2.6)

project(CppTransport)

add_definitions(-std=c++11)
add_definitions(-fopenmp)

include_directories(./ backends backends/cpp parser parser/bison "transport objects" utilities /usr/include /usr/local/include /usr/local/include/odeint-v2)

add_executable(CppTransport main.cpp backends/cpp/cpp_backend.cpp parser/bison/y_parser.tab.cc parser/finder.cpp parser/lexfile.cpp parser/parse_tree.cpp parser/quantity.cpp parser/y_driver.cpp parser/y_lexer.cpp "transport objects/canonical_u_tensor_factory.cpp" "transport objects/u_tensor_factory.cpp" utilities/error.cpp utilities/index_assignment.cpp utilities/macro.cpp utilities/to_printable.cpp)
target_link_libraries(CppTransport ginac cln boost_system boost_filesystem boost_timer)

add_executable(dquad-openmp examples/dquad-openmp/main.cpp)
target_link_libraries(dquad-openmp boost_system boost_filesystem boost_timer)

add_executable(chris-openmp examples/chris-openmp/main.cpp)
target_link_libraries(chris-openmp boost_system boost_filesystem boost_timer)

add_executabke(axion-openmp examples/axion-openmp/main.cpp)
target_link_libraries(axion-openmp boost_system boost_filesystem boost_timer)