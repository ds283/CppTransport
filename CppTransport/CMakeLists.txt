cmake_minimum_required(VERSION 2.6)

project(CppTransport)


SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )
SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/splinter/cmake/" )

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -mavx" CACHE STRING "compile flags" FORCE )
SET ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -mavx"              CACHE STRING "compile flags" FORCE )

SET ( JSONCPP_AMALGAMATED "transport-runtime-api/thirdparty/jsoncpp/jsoncpp.cpp" )
SET ( SPLINTER_LIBRARY    "splinter-static-1-3" )
SET ( JSONCPP_LIBRARY     "jsoncpp_lib_static" )

# use version of Eigen3 bundled with SPLINTER
SET ( EIGEN3_INCLUDE_DIR  "transport-runtime-api/thirdparty/splinter/thirdparty/Eigen" )



# pull in bundled SPLINTER sources; on most platforms will need to switch default build to 64-bit
set ( BITNESS "64" )
ADD_SUBDIRECTORY ( "transport-runtime-api/thirdparty/splinter" "splinter-build" )


# pull in bundles libjsoncpp sources
set ( BUILD_STATIC_LIBS )
ADD_SUBDIRECTORY ( "transport-runtime-api/thirdparty/jsoncpp"  "jsoncpp-build" )


# find required packages
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


add_executable(oscillatory-mpi examples/oscillatory/main.cpp )
target_link_libraries(oscillatory-mpi sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


add_executable(oscillatory-makerepo2 examples/oscillatory/makerepo2.cpp )
target_link_libraries(oscillatory-makerepo2 sqlite3 ${MPI_LIBRARIES} ${Boost_LIBRARIES} ${SPLINTER_LIBRARY} ${JSONCPP_LIBRARY})


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




## FILL IN DUMMY TARGETS FOR CLION

set(EXAMPLES_DQUAD_MPI_FILES
  examples/dquad-mpi/defaults.model
  examples/dquad-mpi/dq.model
  examples/dquad-mpi/main.cpp
  examples/dquad-mpi/makerepo.cpp
  examples/dquad-mpi/makerepo2.cpp
)

set(PARSER_FILES
  parser/input_stack.cpp
  parser/input_stack.h
  parser/lexeme.h
  parser/lexfile.cpp
  parser/lexfile.h
  parser/lexical.h
  parser/lexstream.h
  parser/script.cpp
  parser/script.h
  parser/semantic_data.cpp
  parser/semantic_data.h
  parser/y_common.h
  parser/y_driver.cpp
  parser/y_driver.h
  parser/y_lexer.cpp
  parser/y_lexer.h
)

set(EXAMPLES_NEW_AXION_FILES
  examples/new-axion/defaults.model
  examples/new-axion/main.cpp
  examples/new-axion/makerepo2.cpp
  examples/new-axion/new-axion.model
)

set(TOP_LEVEL_FILES
  core.h
  main.cpp
  msg_en.h
)

set(TRANSPORT_RUNTIME_API_UTILITIES_FILES
  transport-runtime-api/utilities/asciitable.h
  transport-runtime-api/utilities/formatter.h
  transport-runtime-api/utilities/host_information.h
  transport-runtime-api/utilities/latex_output.h
  transport-runtime-api/utilities/linecache.h
  transport-runtime-api/utilities/named_list.h
  transport-runtime-api/utilities/python_finder.h
  transport-runtime-api/utilities/random_string.h
  transport-runtime-api/utilities/spline1d.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_FILES
  transport-runtime-api/derived-products/data_products.h
  transport-runtime-api/derived-products/derived_product.h
  transport-runtime-api/derived-products/derived_product_forward_declare.h
  transport-runtime-api/derived-products/derived_product_helper.h
  transport-runtime-api/derived-products/enumerations.h)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_policy.h
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query_helper.h
)

set(EXAMPLES_RCHI_FILES
  examples/rchi/main.cpp
  examples/rchi/rchi.model
)

set(BACKENDS_SHARED_FILES
  backends/shared/flow_tensors.cpp
  backends/shared/flow_tensors.h
  backends/shared/fundamental.cpp
  backends/shared/fundamental.h
  backends/shared/gauge_xfm.cpp
  backends/shared/gauge_xfm.h
  backends/shared/lagrangian_tensors.cpp
  backends/shared/lagrangian_tensors.h
  backends/shared/summation.cpp
  backends/shared/summation.h
  backends/shared/temporary_pool.cpp
  backends/shared/temporary_pool.h
  backends/shared/utensors.cpp
  backends/shared/utensors.h
)

set(TEMPLATES_VEXCL_CUDA_FILES
  templates/vexcl-cuda/canonical_vexcl-cuda.h
  templates/vexcl-cuda/threepf.cu
  templates/vexcl-cuda/twopf.cu
)

set(EXAMPLES_OSCILLATORY_FILES
)

set(BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES
  backends/infrastructure/macro_substitution/index_assignment.cpp
  backends/infrastructure/macro_substitution/index_assignment.h
  backends/infrastructure/macro_substitution/macro.cpp
  backends/infrastructure/macro_substitution/macro.h
  backends/infrastructure/macro_substitution/macro_tokenizer.cpp
  backends/infrastructure/macro_substitution/macro_tokenizer.h
)

set(PARSER_BISON_FILES
  parser/bison/location.hh
  parser/bison/position.hh
  parser/bison/stack.hh
  parser/bison/y_parser.tab.cc
  parser/bison/y_parser.tab.hh
  parser/bison/y_parser.yy
)

set(EXAMPLES_CHRIS_MPI_FILES
  examples/chris-mpi/chris.model
  examples/chris-mpi/defaults.model
  examples/chris-mpi/main.cpp
  examples/chris-mpi/makerepo2.cpp
  examples/chris-mpi/makerepo3.cpp
)

set(TRANSPORT_RUNTIME_API_DATA_FILES
  transport-runtime-api/data/data_manager.h
)

set(EXAMPLES_MAFALDA40_FILES
  examples/mafalda40/defaults.model
  examples/mafalda40/mafalda40.model
  examples/mafalda40/main.cpp
  examples/mafalda40/makerepo.cpp
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES
  transport-runtime-api/derived-products/derived-content/integration-analysis/cost_metrics.h
  transport-runtime-api/derived-products/derived-content/integration-analysis/cost_wavenumber.h
)

set(UTILITIES_FILES
  utilities/error.cpp
  utilities/error.h
  utilities/finder.cpp
  utilities/finder.h
  utilities/flatten.h
  utilities/formatter.cpp
  utilities/formatter.h
  utilities/to_printable.cpp
  utilities/to_printable.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_CONVENIENCE_FILES
  transport-runtime-api/derived-products/convenience/time_series_plot.h
  transport-runtime-api/derived-products/convenience/time_series_table.h
  transport-runtime-api/derived-products/convenience/wavenumber_series_plot.h
  transport-runtime-api/derived-products/convenience/wavenumber_series_table.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_threepf_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_time_query.h
  transport-runtime-api/derived-products/derived-content/SQL_query/detail/SQL_twopf_query.h
)

set(TRANSPORT_RUNTIME_API_MODELS_FILES
  transport-runtime-api/models/advisory_classes.h
  transport-runtime-api/models/canonical_model.h
  transport-runtime-api/models/model.h
  transport-runtime-api/models/model_forward_declare.h
  transport-runtime-api/models/observers.h
)

set(TRANSPORT_RUNTIME_API_SERIALIZATION_FILES
  transport-runtime-api/serialization/serializable.h
)

set(TRANSPORT_RUNTIME_API_DATA_DATAPIPE_FILES
  transport-runtime-api/data/datapipe/datapipe.h
  transport-runtime-api/data/datapipe/datapipe_detail.h
  transport-runtime-api/data/datapipe/datapipe_forward_declare.h
  transport-runtime-api/data/datapipe/linecache_specializations.h
  transport-runtime-api/data/datapipe/tags.h
  transport-runtime-api/data/datapipe/tags_forward_declare.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES
  transport-runtime-api/derived-products/derived-content/concepts/series/time_series.h
  transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h
)

set(BACKENDS_LANGUAGES_SHARED_FILES
  backends/languages/shared/C_style_printer.cpp
  backends/languages/shared/C_style_printer.h
  backends/languages/shared/kernel_argument_macros.cpp
  backends/languages/shared/kernel_argument_macros.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES
  transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h
)

set(EXAMPLES_HEAVY_HILLTOP_FILES
  examples/heavy-hilltop/defaults.model
  examples/heavy-hilltop/heavy_hilltop.model
  examples/heavy-hilltop/main.cpp
  examples/heavy-hilltop/makerepo2.cpp
)

set(EXAMPLES_QUADRATIC_FILES
  examples/quadratic/defaults.model
  examples/quadratic/main.cpp
  examples/quadratic/makerepo.cpp
  examples/quadratic/quad.model
)

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_FILES
  transport-runtime-api/localizations/messages_en.h
)

set(TRANSPORT_RUNTIME_API_TASKS_POSTINTEGRATION_DETAIL_FILES
  transport-runtime-api/tasks/postintegration_detail/abstract.h
  transport-runtime-api/tasks/postintegration_detail/common.h
  transport-runtime-api/tasks/postintegration_detail/fNL_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_threepf_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_twopf_list_task.h
  transport-runtime-api/tasks/postintegration_detail/zeta_twopf_task.h
)

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_FILES
  transport-runtime-api/localizations/messages_en/advisory_classes.h
  transport-runtime-api/localizations/messages_en/command_line.h
  transport-runtime-api/localizations/messages_en/data_manager.h
  transport-runtime-api/localizations/messages_en/initial_conditions.h
  transport-runtime-api/localizations/messages_en/instance_manager.h
  transport-runtime-api/localizations/messages_en/integration.h
  transport-runtime-api/localizations/messages_en/json_repository_interface.h
  transport-runtime-api/localizations/messages_en/labels.h
  transport-runtime-api/localizations/messages_en/manager.h
  transport-runtime-api/localizations/messages_en/master_scheduler.h
  transport-runtime-api/localizations/messages_en/model_list.h
  transport-runtime-api/localizations/messages_en/models.h
  transport-runtime-api/localizations/messages_en/named_list.h
  transport-runtime-api/localizations/messages_en/parameters.h
  transport-runtime-api/localizations/messages_en/range.h
  transport-runtime-api/localizations/messages_en/repository.h
  transport-runtime-api/localizations/messages_en/repository_sqlite3.h
  transport-runtime-api/localizations/messages_en/runtimeapi.h
  transport-runtime-api/localizations/messages_en/scheduler.h
  transport-runtime-api/localizations/messages_en/serialization.h
  transport-runtime-api/localizations/messages_en/sqlite3_utility.h
  transport-runtime-api/localizations/messages_en/task_manager.h
  transport-runtime-api/localizations/messages_en/work_journal.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES
  transport-runtime-api/derived-products/line-collections/data_line.h
  transport-runtime-api/derived-products/line-collections/line_asciitable.h
  transport-runtime-api/derived-products/line-collections/line_collection.h
  transport-runtime-api/derived-products/line-collections/line_plot2d.h
  transport-runtime-api/derived-products/line-collections/line_values.h
)

set(TRANSPORT_RUNTIME_API_REPOSITORY_FILES
  transport-runtime-api/repository/json_repository.h
  transport-runtime-api/repository/repository.h
  transport-runtime-api/repository/repository_creation_key.h
  transport-runtime-api/repository/transaction_manager.h
)

set(CONCEPTS_FILES
  concepts/filestack.h
  concepts/indexorder.h
  concepts/stepper.h
)

set(TEMPLATES_VEXCL_OPENCL_FILES
  templates/vexcl-opencl/canonical_vexcl-opencl.h
  templates/vexcl-opencl/threepf.cl
  templates/vexcl-opencl/twopf.cl
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES
  transport-runtime-api/derived-products/derived-content/concepts/lines/fNL_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/r_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/tensor_twopf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/threepf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/twopf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_reduced_bispectrum_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_threepf_line.h
  transport-runtime-api/derived-products/derived-content/concepts/lines/zeta_twopf_line.h
)

set(TRANSPORT_RUNTIME_API_CONCEPTS_RANGE_DETAIL_FILES
  transport-runtime-api/concepts/range_detail/abstract.h
  transport-runtime-api/concepts/range_detail/aggregation.h
  transport-runtime-api/concepts/range_detail/common.h
  transport-runtime-api/concepts/range_detail/helper.h
  transport-runtime-api/concepts/range_detail/stepping.h
)

set(BACKENDS_LANGUAGES_CUDA_FILES
  backends/languages/cuda/cuda_group.cpp
  backends/languages/cuda/cuda_group.h
  backends/languages/cuda/cuda_printer.cpp
  backends/languages/cuda/cuda_printer.h
)

set(TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_FILES
  transport-runtime-api/repository/records/record_finder.h
  transport-runtime-api/repository/records/repository_records.h
  transport-runtime-api/repository/records/repository_records_forward_declare.h
)

set(TRANSPORT_RUNTIME_API_CONCEPTS_FILES
  transport-runtime-api/concepts/flattener.h
  transport-runtime-api/concepts/initial_conditions.h
  transport-runtime-api/concepts/initial_conditions_forward_declare.h
  transport-runtime-api/concepts/parameters.h
  transport-runtime-api/concepts/range.h
)

set(EXAMPLES_DQUAD_VEXCL_FILES
  examples/dquad-vexcl/cuda.model
  examples/dquad-vexcl/defaults.model
  examples/dquad-vexcl/dq.model
  examples/dquad-vexcl/dq_core.h
  examples/dquad-vexcl/dq_vexcl-cuda.h
  examples/dquad-vexcl/dq_vexcl-opencl.h
  examples/dquad-vexcl/main.cpp
  examples/dquad-vexcl/makerepo.cpp
  examples/dquad-vexcl/opencl.model
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES
  transport-runtime-api/derived-products/derived-content/background-quantites/background_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/largest_u2_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/largest_u3_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/u2_line.h
  transport-runtime-api/derived-products/derived-content/background-quantites/u3_line.h
)

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES
  transport-runtime-api/localizations/messages_en/tasks/tasks.h
  transport-runtime-api/localizations/messages_en/tasks/threepf_config_database.h
  transport-runtime-api/localizations/messages_en/tasks/twopf_config_database.h
)

set(BACKENDS_LANGUAGES_CPP_FILES
  backends/languages/cpp/cpp_cse.cpp
  backends/languages/cpp/cpp_cse.h
  backends/languages/cpp/cpp_printer.cpp
  backends/languages/cpp/cpp_printer.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES
  transport-runtime-api/derived-products/derived-content/concepts/derived_line.h
  transport-runtime-api/derived-products/derived-content/concepts/derived_line_helper.h
)

set(TRANSPORT_RUNTIME_API_SCHEDULER_FILES
  transport-runtime-api/scheduler/context.h
  transport-runtime-api/scheduler/scheduler.h
  transport-runtime-api/scheduler/work_queue.h
)

set(EXAMPLES_QSFI_FILES
  examples/qsfi/defaults.model
  examples/qsfi/main.cpp
  examples/qsfi/makerepo2.cpp
  examples/qsfi/qsfi.model
)

set(TRANSPORT_RUNTIME_API_TASKS_INTEGRATION_DETAIL_FILES
  transport-runtime-api/tasks/integration_detail/abstract.h
  transport-runtime-api/tasks/integration_detail/background_task.h
  transport-runtime-api/tasks/integration_detail/common.h
  transport-runtime-api/tasks/integration_detail/threepf_task.h
  transport-runtime-api/tasks/integration_detail/twopf_list_task.h
  transport-runtime-api/tasks/integration_detail/twopf_task.h
)

set(TRANSPORT_RUNTIME_API_DATA_BATCHERS_FILES
  transport-runtime-api/data/batchers/batchers.h
  transport-runtime-api/data/batchers/generic_batcher.h
  transport-runtime-api/data/batchers/integration_batcher.h
  transport-runtime-api/data/batchers/integration_items.h
  transport-runtime-api/data/batchers/postintegration_batcher.h
  transport-runtime-api/data/batchers/postintegration_items.h
  transport-runtime-api/data/batchers/zeta_compute.h
)

set(EXAMPLES_AXION_MPI_FILES
  examples/axion-mpi/axion.model
  examples/axion-mpi/defaults.model
  examples/axion-mpi/main.cpp
  examples/axion-mpi/makerepo.cpp
  examples/axion-mpi/makerepo2.cpp
  examples/axion-mpi/makerepo3.cpp
)

set(BACKENDS_LANGUAGES_OPENCL_FILES
  backends/languages/opencl/opencl_group.cpp
  backends/languages/opencl/opencl_group.h
  backends/languages/opencl/opencl_printer.cpp
  backends/languages/opencl/opencl_printer.h
)

set(TRANSPORT_RUNTIME_API_SQLITE3_OPERATIONS_FILES
  transport-runtime-api/sqlite3/operations/data_manager.h
  transport-runtime-api/sqlite3/operations/data_manager_aggregate.h
  transport-runtime-api/sqlite3/operations/data_manager_common.h
  transport-runtime-api/sqlite3/operations/data_manager_create.h
  transport-runtime-api/sqlite3/operations/data_manager_integrity.h
  transport-runtime-api/sqlite3/operations/data_manager_pull.h
  transport-runtime-api/sqlite3/operations/data_manager_temporaries.h
  transport-runtime-api/sqlite3/operations/data_manager_write.h
  transport-runtime-api/sqlite3/operations/data_traits.h
  transport-runtime-api/sqlite3/operations/repository.h
  transport-runtime-api/sqlite3/operations/repository_admin.h
  transport-runtime-api/sqlite3/operations/repository_common.h
  transport-runtime-api/sqlite3/operations/repository_count.h
  transport-runtime-api/sqlite3/operations/repository_create.h
  transport-runtime-api/sqlite3/operations/repository_inflight.h
  transport-runtime-api/sqlite3/operations/repository_store.h
  transport-runtime-api/sqlite3/operations/sqlite3_utility.h
)

set(TRANSPORT_RUNTIME_API_TASKS_FILES
  transport-runtime-api/tasks/derivable_task.h
  transport-runtime-api/tasks/integration_tasks.h
  transport-runtime-api/tasks/output_tasks.h
  transport-runtime-api/tasks/postintegration_tasks.h
  transport-runtime-api/tasks/task.h
  transport-runtime-api/tasks/task_configurations.h
  transport-runtime-api/tasks/task_configurations_forward_declare.h
  transport-runtime-api/tasks/task_helper.h
  transport-runtime-api/tasks/tasks_forward_declare.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES
  transport-runtime-api/derived-products/derived-content/correlation-functions/field_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/field_wavenumber_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/fNL_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/r_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/r_wavenumber_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/tensor_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/tensor_wavenumber_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/zeta_time_series.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/zeta_wavenumber_series.h
)

set(BACKENDS_LANGUAGES_CPP_CORE_FILES
  backends/languages/cpp/core/core_group.cpp
  backends/languages/cpp/core/core_group.h
  backends/languages/cpp/core/core_macros.cpp
  backends/languages/cpp/core/core_macros.h
)

set(EXAMPLES_POWERLAW_FILES
  examples/powerlaw/defaults.model
  examples/powerlaw/main.cpp
  examples/powerlaw/makerepo2.cpp
  examples/powerlaw/powerlaw.model
)

set(TEMPLATES_FILES
  templates/canonical_basic.h
  templates/canonical_basic_loop.h
  templates/canonical_basic_twopf.h
  templates/canonical_basic_unrolled.h
  templates/canonical_core.h
  templates/canonical_core_twopf.h
)

set(BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES
  backends/infrastructure/language_concepts/cse.cpp
  backends/infrastructure/language_concepts/cse.h
  backends/infrastructure/language_concepts/language_printer.h
)

set(TRANSPORT_RUNTIME_API_DATA_DATAPIPE_TAGS_FILES
  transport-runtime-api/data/datapipe/tags/config_tags.h
  transport-runtime-api/data/datapipe/tags/field_tags.h
  transport-runtime-api/data/datapipe/tags/fNL_tags.h
  transport-runtime-api/data/datapipe/tags/stats_tags.h
  transport-runtime-api/data/datapipe/tags/zeta_tags.h
)

set(BACKENDS_UTILITIES_FILES
  backends/utilities/ginac_cache.h
  backends/utilities/symbol_factory.cpp
  backends/utilities/symbol_factory.h
)

set(BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES
  backends/infrastructure/file_handling/buffer.cpp
  backends/infrastructure/file_handling/buffer.h
  backends/infrastructure/file_handling/output_stack.cpp
  backends/infrastructure/file_handling/output_stack.h
  backends/infrastructure/file_handling/translation_unit.cpp
  backends/infrastructure/file_handling/translation_unit.h
  backends/infrastructure/file_handling/translator.cpp
  backends/infrastructure/file_handling/translator.h
)

set(EXAMPLES_INFLECTION_FILES
  examples/inflection/defaults.model
  examples/inflection/inflection.model
  examples/inflection/main.cpp
  examples/inflection/makerepo2.cpp
)

set(TRANSPORT_RUNTIME_API_TASKS_CONFIGURATION_DATABASE_FILES
  transport-runtime-api/tasks/configuration-database/generic_config_iterator.h
  transport-runtime-api/tasks/configuration-database/generic_record_iterator.h
  transport-runtime-api/tasks/configuration-database/generic_value_iterator.h
  transport-runtime-api/tasks/configuration-database/threepf_config_database.h
  transport-runtime-api/tasks/configuration-database/time_config_database.h
  transport-runtime-api/tasks/configuration-database/twopf_config_database.h
)

set(TRANSPORT_RUNTIME_API_SQLITE3_FILES
  transport-runtime-api/sqlite3/data_manager_sqlite3.h
  transport-runtime-api/sqlite3/repository_sqlite3.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES
  transport-runtime-api/derived-products/derived-content/deprecated/zeta_kseries_compute.h
)

set(TRANSPORT_RUNTIME_API_MANAGER_FILES
  transport-runtime-api/manager/argument_cache.h
  transport-runtime-api/manager/environment.h
  transport-runtime-api/manager/instance_manager.h
  transport-runtime-api/manager/master_controller.h
  transport-runtime-api/manager/master_scheduler.h
  transport-runtime-api/manager/mpi_operations.h
  transport-runtime-api/manager/slave_controller.h
  transport-runtime-api/manager/slave_work_handler.h
  transport-runtime-api/manager/task_manager.h
  transport-runtime-api/manager/work_journal.h
)

set(TRANSPORT_OBJECTS_FILES
  transport-objects/canonical_u_tensor_factory.cpp
  transport-objects/u_tensor_factory.cpp
  transport-objects/u_tensor_factory.h
)

set(BACKENDS_LANGUAGES_CPP_VEXCL_FILES
  backends/languages/cpp/vexcl/vexcl_group.cpp
  backends/languages/cpp/vexcl/vexcl_group.h
  backends/languages/cpp/vexcl/vexcl_kernels.cpp
  backends/languages/cpp/vexcl/vexcl_kernels.h
  backends/languages/cpp/vexcl/vexcl_steppers.cpp
  backends/languages/cpp/vexcl/vexcl_steppers.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/fNL_timeseries_compute.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/threepf_kconfig_shift.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/threepf_time_shift.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/zeta_timeseries_compute.h
  transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/common.h)

set(TRANSPORT_RUNTIME_API_REPOSITORY_WRITERS_FILES
  transport-runtime-api/repository/writers/derived_content_writer.h
  transport-runtime-api/repository/writers/generic_writer.h
  transport-runtime-api/repository/writers/integration_writer.h
  transport-runtime-api/repository/writers/postintegration_writer.h
  transport-runtime-api/repository/writers/writers.h
)

set(BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES
  backends/infrastructure/replacement_rules/package_group.cpp
  backends/infrastructure/replacement_rules/package_group.h
  backends/infrastructure/replacement_rules/package_group_factory.cpp
  backends/infrastructure/replacement_rules/package_group_factory.h
  backends/infrastructure/replacement_rules/replacement_rule_definitions.h
  backends/infrastructure/replacement_rules/replacement_rule_package.cpp
  backends/infrastructure/replacement_rules/replacement_rule_package.h
)

set(TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES
  transport-runtime-api/localizations/messages_en/derived-products/derived_line.h
  transport-runtime-api/localizations/messages_en/derived-products/derived_product.h
  transport-runtime-api/localizations/messages_en/derived-products/fNL_line.h
  transport-runtime-api/localizations/messages_en/derived-products/line_asciitable.h
  transport-runtime-api/localizations/messages_en/derived-products/line_collection.h
  transport-runtime-api/localizations/messages_en/derived-products/line_plot2d.h
  transport-runtime-api/localizations/messages_en/derived-products/line_values.h
  transport-runtime-api/localizations/messages_en/derived-products/SQL_query.h
  transport-runtime-api/localizations/messages_en/derived-products/time_series.h
  transport-runtime-api/localizations/messages_en/derived-products/time_series_convenience.h
  transport-runtime-api/localizations/messages_en/derived-products/wavenumber_series.h
  transport-runtime-api/localizations/messages_en/derived-products/wavenumber_series_convenience.h
)

set(TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_UTILITIES_FILES
  transport-runtime-api/derived-products/utilities/extractor.h
  transport-runtime-api/derived-products/utilities/index_selector.h
  transport-runtime-api/derived-products/utilities/wrapper.h
)

set(TRANSPORT_RUNTIME_API_FILES
  transport-runtime-api/ansi_colour_codes.h
  transport-runtime-api/default_symbols.h
  transport-runtime-api/defaults.h
  transport-runtime-api/exceptions.h
  transport-runtime-api/messages.h
  transport-runtime-api/transport.h
  transport-runtime-api/version.h
  transport-runtime-api/enumerations.h)

set(EXAMPLES_QUADRATIC10_FILES
  examples/quadratic10/defaults.model
  examples/quadratic10/main.cpp
  examples/quadratic10/makerepo.cpp
  examples/quadratic10/quadratic10.model
)

set(SOURCE_FILES
  ${EXAMPLES_DQUAD_MPI_FILES}
  ${PARSER_FILES}
  ${EXAMPLES_NEW_AXION_FILES}
  ${TOP_LEVEL_FILES}
  ${TRANSPORT_RUNTIME_API_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_FILES}
  ${EXAMPLES_RCHI_FILES}
  ${BACKENDS_SHARED_FILES}
  ${TEMPLATES_VEXCL_CUDA_FILES}
  ${EXAMPLES_OSCILLATORY_FILES}
  ${BACKENDS_INFRASTRUCTURE_MACRO_SUBSTITUTION_FILES}
  ${PARSER_BISON_FILES}
  ${EXAMPLES_CHRIS_MPI_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_FILES}
  ${EXAMPLES_MAFALDA40_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_INTEGRATION_ANALYSIS_FILES}
  ${UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_CONVENIENCE_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_SQL_QUERY_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_MODELS_FILES}
  ${TRANSPORT_RUNTIME_API_SERIALIZATION_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DATAPIPE_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_SERIES_FILES}
  ${BACKENDS_LANGUAGES_SHARED_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_UTILITIES_FILES}
  ${EXAMPLES_HEAVY_HILLTOP_FILES}
  ${EXAMPLES_QUADRATIC_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_POSTINTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_LINE_COLLECTIONS_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_FILES}
  ${CONCEPTS_FILES}
  ${TEMPLATES_VEXCL_OPENCL_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_LINES_FILES}
  ${TRANSPORT_RUNTIME_API_CONCEPTS_RANGE_DETAIL_FILES}
  ${BACKENDS_LANGUAGES_CUDA_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_RECORDS_FILES}
  ${TRANSPORT_RUNTIME_API_CONCEPTS_FILES}
  ${EXAMPLES_DQUAD_VEXCL_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_BACKGROUND_QUANTITES_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_TASKS_FILES}
  ${BACKENDS_LANGUAGES_CPP_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_API_SCHEDULER_FILES}
  ${EXAMPLES_QSFI_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_INTEGRATION_DETAIL_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_BATCHERS_FILES}
  ${EXAMPLES_AXION_MPI_FILES}
  ${BACKENDS_LANGUAGES_OPENCL_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_OPERATIONS_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_FILES}
  ${BACKENDS_LANGUAGES_CPP_CORE_FILES}
  ${EXAMPLES_POWERLAW_FILES}
  ${TEMPLATES_FILES}
  ${BACKENDS_INFRASTRUCTURE_LANGUAGE_CONCEPTS_FILES}
  ${TRANSPORT_RUNTIME_API_DATA_DATAPIPE_TAGS_FILES}
  ${BACKENDS_UTILITIES_FILES}
  ${BACKENDS_INFRASTRUCTURE_FILE_HANDLING_FILES}
  ${EXAMPLES_INFLECTION_FILES}
  ${TRANSPORT_RUNTIME_API_TASKS_CONFIGURATION_DATABASE_FILES}
  ${TRANSPORT_RUNTIME_API_SQLITE3_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_DEPRECATED_FILES}
  ${TRANSPORT_RUNTIME_API_MANAGER_FILES}
  ${TRANSPORT_OBJECTS_FILES}
  ${BACKENDS_LANGUAGES_CPP_VEXCL_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_DERIVED_CONTENT_CORRELATION_FUNCTIONS_COMPUTE_GADGETS_FILES}
  ${TRANSPORT_RUNTIME_API_REPOSITORY_WRITERS_FILES}
  ${BACKENDS_INFRASTRUCTURE_REPLACEMENT_RULES_FILES}
  ${TRANSPORT_RUNTIME_API_LOCALIZATIONS_MESSAGES_EN_DERIVED_PRODUCTS_FILES}
  ${TRANSPORT_RUNTIME_API_DERIVED_PRODUCTS_UTILITIES_FILES}
  ${TRANSPORT_RUNTIME_API_FILES}
  ${EXAMPLES_QUADRATIC10_FILES}
)

add_executable(dummy_clion_target EXCLUDE_FROM_ALL ${SOURCE_FILES})
