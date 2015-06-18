//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_MESSAGES_EN_DATA_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_DATA_MANAGER_H


#define CPPTRANSPORT_NFIELDS_BACKG                               "Storage error: Unexpected number of elements when batching output of background"
#define CPPTRANSPORT_NFIELDS_TWOPF                               "Storage error: Unexpected number of elements when batching output of twopf"
#define CPPTRANSPORT_NFIELDS_THREEPF                             "Storage error: Unexpected number of elements when batching output of threepf"
#define CPPTRANSPORT_NFIELDS_TENSOR_TWOPF                        "Storage error: Unexpected number of elements when batching output of tensor twopf"

#define CPPTRANSPORT_DATAMGR_NOT_SET                             "Internal error: Attempt to access data manager when unset in 'task_manager'"

#define CPPTRANSPORT_DATACTR_AGGREGATION_HANDLER_NOT_SET         "Internal error: Could not set aggregation handler"

#define CPPTRANSPORT_DATACTR_NOT_CLOSED                          "Data container error: Some containers not closed when deleting 'data_manager' object"
#define CPPTRANSPORT_DATACTR_CLOSE                               "Data container error: Unexpected error when closing containers on destruction of 'data_manager' object (backend code="
#define CPPTRANSPORT_DATACTR_CREATE_A                            "Data container error: Could not create data container"
#define CPPTRANSPORT_DATACTR_CREATE_B                            "(backend code="
#define CPPTRANSPORT_DATACTR_OPEN_A                              "Data container error: Could not open data container"
#define CPPTRANSPORT_DATACTR_OPEN_B                              "(backend code="

#define CPPTRANSPORT_DATACTR_TASKLIST_FAIL                       "Data container error: Failed to create tasklist table in data container (backend code="
#define CPPTRANSPORT_DATACTR_TASKLIST_OPEN_A                     "Data container error: Failed to open tasklist"
#define CPPTRANSPORT_DATACTR_TASKLIST_OPEN_B                     "(backend code="
#define CPPTRANSPORT_DATACTR_TASKLIST_READ_A                     "Data container error: Failed to read from tasklist"
#define CPPTRANSPORT_DATACTR_TASKLIST_READ_B                     "(backend code="

#define CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_A                      "Data container error: Failed to create temporary container"
#define CPPTRANSPORT_DATACTR_TEMPCTR_FAIL_B                      "(backend code="

#define CPPTRANSPORT_DATACTR_TIMETAB_FAIL                        "Data container error: Failed to create time-sample table in data container (backend code="
#define CPPTRANSPORT_DATACTR_TWOPFTAB_FAIL                       "Data container error: Failed to create twopf-sample table in data container (backend code="
#define CPPTRANSPORT_DATACTR_THREEPFTAB_FAIL                     "Data container error: Failed to create threepf-sample table in data container (backend code="

#define CPPTRANSPORT_DATACTR_STATS_INSERT_FAIL                   "Data container error: Failed to create per-configuration statistics table in data container (backend code="
#define CPPTRANSPORT_DATACTR_ICS_INSERT_FAIL                     "Data container error: Failed to create initial conditions table in data container (backend code="
#define CPPTRANSPORT_DATACTR_WORKER_INSERT_FAIL                  "Data container error: Failed to create worker information table in data container (backend code="
#define CPPTRANSPORT_DATACTR_BACKG_DATATAB_FAIL                  "Data container error: Failed to create background-value table in data container (backend code="
#define CPPTRANSPORT_DATACTR_TWOPF_DATATAB_FAIL                  "Data container error: Failed to create twopf-value table in data container (backend code="
#define CPPTRANSPORT_DATACTR_TENSOR_TWOPF_DATATAB_FAIL           "Data container error: Failed to creata tensor twopf-value table in data container (backend code="
#define CPPTRANSPORT_DATACTR_THREEPF_DATATAB_FAIL                "Data container error: Failed to create threepf-value table in data container (backend code="

#define CPPTRANSPORT_DATACTR_ZETA_TWOPF_DATATAB_FAIL             "Data container error: Failed to create zeta twopf-value table in data container (backend code="
#define CPPTRANSPORT_DATACTR_ZETA_THREEPF_DATATAB_FAIL           "Data container error: Failed to create zeta threepf-value table in data container (backend code="
#define CPPTRANSPORT_DATACTR_ZETA_REDBSP_DATATAB_FAIL            "Data container error: Failed to create zeta reduced-bispectrum-value table in data container (backend code="
#define CPPTRANSPORT_DATACTR_FNL_DATATAB_FAIL                    "Data container error: Failed to create fNL-value table in data container (backend code="
#define CPPTRANSPORT_DATAMGR_DROP_DEST_FAIL                      "Data container error: Failed to drop destination table prior to copying post-integration table (backend code="

#define CPPTRANSPORT_DATACTR_BACKGROUND_COPY                     "Data container error: Failed to copy background values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_TWOPF_COPY                          "Data container error: Failed to copy twopf values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_TENSOR_TWOPF_COPY                   "Data container error: Failed to copy tensor twopf values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_STATISTICS_COPY                     "Data container error: Failed to copy per-configuration statistics from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_ICS_COPY                            "Data container error: Failed to copy initial conditions from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_WORKERS_COPY                        "Data container error: Failed to copy worker information from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_THREEPF_COPY                        "Data container error: Failed to copy threepf values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_ZETA_TWOPF_COPY                     "Data container error: Failed to copy zeta twopf values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_ZETA_THREEPF_COPY                   "Data container error: Failed to copy zeta threepf values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_ZETA_REDUCED_BISPECTRUM_COPY        "Data container error: Failed to copy zeta reduced bispectrum values from temporary container (backend code="
#define CPPTRANSPORT_DATACTR_FNL_COPY                            "Data container error: Failed to copy fNL values from temporary bispectrum container (backend code="
#define CPPTRANSPORT_DATACTR_POST_INTEGRATION_COPY               "Data container error: Failed to copy post-integration table (backend code="
#define CPPTRANSPORT_DATACTR_REMOVE_TEMP                         "Data container error: Could not remove temporary container"

#define CPPTRANSPORT_DATAMGR_NULL_DATAPIPE                       "Data manager error: Null datapipe specifier"
#define CPPTRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED            "Data manager error: Attempt to detach datapipe, but no output group is attached"
#define CPPTRANSPORT_DATAMGR_PIPE_WRONG_CONTENT                  "Data manager error: Datapipe attached to wrong type of content group"
#define CPPTRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED        "Data manager error: Attempt to attach datapipe, but output group already attached"
#define CPPTRANSPORT_DATAMGR_UNKNOWN_DERIVABLE_TASK              "Data manager error: Attempt to attach datapipe to derivable task of unknown type: task name"
#define CPPTRANSPORT_DATAMGR_PIPE_NOT_ATTACHED                   "Data manager error: Attempt to use datapipe when no output group is attached"
#define CPPTRANSPORT_DATAMGR_PIPE_NULL_TASK                      "Data manager error: Null task supplied when attaching datapipe"
#define CPPTRANSPORT_DATAMGR_UNKNOWN_CF_TYPE                     "Data manager error: Datapipe pull request for unknown correlation function type"
#define CPPTRANSPORT_DATAMGR_NO_OUTPUT_GROUP                     "Data manager error: Could not find a matching output group for task"
#define CPPTRANSPORT_DATAMGR_ZETA_IM_NOT_IMPLEMENTED             "Data manager error: Pull for imaginary zeta two-point function is not implemented"

#define CPPTRANSPORT_DATAMGR_NULL_DERIVED_PRODUCT                "Data manager error: Null derived product"
#define CPPTRANSPORT_DATAMGR_NULL_BATCHER                        "Data manager error: Null batcher"

#define CPPTRANSPORT_DATAMGR_DERIVED_PRODUCT_MISSING             "Data manager error: Can not find expected derived product in temporary location"

#define CPPTRANSPORT_DATAMGR_TEMP_SERIAL_CREATE_FAIL             "Data manager error: Failed to create temporary table of serial numbers (backend code="
#define CPPTRANSPORT_DATAMGR_TEMP_SERIAL_INSERT_FAIL             "Data manager error: Failed to insert value in temporary table of serial numbers (backend code="
#define CPPTRANSPORT_DATAMGR_TEMP_SERIAL_DROP_FAIL               "Data manager error: Failed to drop temporary table of serial numbers (backend code="
#define CPPTRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL               "Data manager error: Failed to select time sample (backend code="
#define CPPTRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW_A               "Data manager error: Read too few time samples from the container: expected"
#define CPPTRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW_B               "received"
#define CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL            "Data manager error: Failed to select k-configuration sample (backend code="
#define CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW_A            "Data manager error: Read too few k-configuration samples from the container: expected"
#define CPPTRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW_B            "received"
#define CPPTRANSPORT_DATAMGR_TEMP_THREEPF_CREATE_FAIL            "Data manager error: Failed to create temporary table of threepf configurations (backend code="
#define CPPTRANSPORT_DATAMGR_TEMP_THREEPF_DROP_FAIL              "Data manager error: Failed to drop temporary table of threepf configurations (backend code="

#define CPPTRANSPORT_DATAMGR_UNKNOWN_FNL_TEMPLATE                "Data manager error: Unknown fNL template type"

#define CPPTRANSPORT_DATAMGR_INTEGRITY_READ_FAIL                 "Data manager error: Failure while performing integrity check (backend code="


#endif // CPPTRANSPORT_MESSAGES_EN_DATA_MANAGER_H
