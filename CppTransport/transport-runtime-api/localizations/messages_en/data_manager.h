//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_DATA_MANAGER_H
#define __CPP_TRANSPORT_MESSAGES_EN_DATA_MANAGER_H


#define __CPP_TRANSPORT_NFIELDS_BACKG                               "Storage error: Unexpected number of elements when batching output of background"
#define __CPP_TRANSPORT_NFIELDS_TWOPF                               "Storage error: Unexpected number of elements when batching output of twopf"
#define __CPP_TRANSPORT_NFIELDS_THREEPF                             "Storage error: Unexpected number of elements when batching output of threepf"

#define __CPP_TRANSPORT_DATAMGR_NOT_SET                             "Internal error: Attempt to access data manager when unset in 'task_manager'"

#define __CPP_TRANSPORT_DATACTR_NOT_CLOSED                          "Data container error: Some containers not closed when deleting 'data_manager' object"
#define __CPP_TRANSPORT_DATACTR_CLOSE                               "Data container error: Unexpected error when closing containers on destruction of 'data_manager' object (backend code="
#define __CPP_TRANSPORT_DATACTR_CREATE_A                            "Data container error: Could not create data container"
#define __CPP_TRANSPORT_DATACTR_CREATE_B                            "(backend code="
#define __CPP_TRANSPORT_DATACTR_OPEN_A                              "Data container error: Could not open data container"
#define __CPP_TRANSPORT_DATACTR_OPEN_B                              "(backend code="
#define __CPP_TRANSPORT_DATACTR_TIMETAB_FAIL                        "Data container error: Failed to create time-sample table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_TWOPFTAB_FAIL                       "Data container error: Failed to create twopf-sample table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_THREEPFTAB_FAIL                     "Data container error: Failed to create threepf-sample table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_TASKLIST_FAIL                       "Data container error: Failed to create tasklist table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_TASKLIST_OPEN_A                     "Data container error: Failed to open tasklist"
#define __CPP_TRANSPORT_DATACTR_TASKLIST_OPEN_B                     "(backend code="
#define __CPP_TRANSPORT_DATACTR_TASKLIST_READ_A                     "Data container error: Failed to read from tasklist"
#define __CPP_TRANSPORT_DATACTR_TASKLIST_READ_B                     "(backend code="
#define __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_A                      "Data container error: Failed to create temporary container"
#define __CPP_TRANSPORT_DATACTR_TEMPCTR_FAIL_B                      "(backend code="
#define __CPP_TRANSPORT_DATACTR_AGGREGATE_A                         "Data container error: Failed to open temporary container"
#define __CPP_TRANSPORT_DATACTR_AGGREGATE_B                         "for batching into principal database (backend code="
#define __CPP_TRANSPORT_DATACTR_BACKG_DATATAB_FAIL                  "Data container error: Failed to create background-value table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_TWOPF_DATATAB_FAIL                  "Data container error: Failed to create twopf-value table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_THREEPF_DATATAB_FAIL                "Data container error: Failed to create threepf-value table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_DN_DATATAB_FAIL                     "Data container error: Failed to create gauge-xfm1-value table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_DDN_DATATAB_FAIL                    "Data container error: Failed to create gauge-xfm2-value table in data container (backend code="
#define __CPP_TRANSPORT_DATACTR_BACKGATTACH                         "Data container error: Failed to attach temporary container for backg transfer (backend code="
#define __CPP_TRANSPORT_DATACTR_BACKGREAD                           "Data container error: Failed to read backg values from temporary container (backend code="
#define __CPP_TRANSPORT_DATACTR_BACKGWRITE                          "Data container error: Failed to aggregate backg values into principal container (backend code="
#define __CPP_TRANSPORT_DATACTR_BACKGXFM                            "Data container error: Failed to aggregate gauge-xfm values into principal container (backend code="
#define __CPP_TRANSPORT_DATACTR_BACKGDETACH                         "Data container error: Failed to detach temporary container following backg trnsfer (backend code="
#define __CPP_TRANSPORT_DATACTR_TWOPFATTACH                         "Data container error: Failed to attach temporary container for twopf transfer (backend code="
#define __CPP_TRANSPORT_DATACTR_TWOPFCOPY                           "Data container error: Failed to copy twopf values from temporary container (backend code="
#define __CPP_TRANSPORT_DATACTR_TWOPFDETACH                         "Data container error: Failed to detach temporary container following twopf transfer (backend code="
#define __CPP_TRANSPORT_DATACTR_THREEPFATTACH                       "Data container error: Failed to attach temporary container for threepf transfer (backend code="
#define __CPP_TRANSPORT_DATACTR_THREEPFCOPY                         "Data container error: Failed to copy threepf values from temporary container (backend code="
#define __CPP_TRANSPORT_DATACTR_THREEPFDETACH                       "Data container error: Failed to detach temporary container following threepf transfer (backend code="
#define __CPP_TRANSPORT_DATACTR_REMOVE_TEMP                         "Data container error: Could not remove temporary container"

#define __CPP_TRANSPORT_DATAMGR_NULL_DATAPIPE                       "Data manager error: Null datapipe specifier"
#define __CPP_TRANSPORT_DATAMGR_DETACH_PIPE_NOT_ATTACHED            "Data manager error: Attempt to detach datapipe, but no output group is attached"
#define __CPP_TRANSPORT_DATAMGR_ATTACH_PIPE_ALREADY_ATTACHED        "Data manager error: Attempt to attach datapipe, but output group already attached"
#define __CPP_TRANSPORT_DATAMGR_PIPE_NOT_ATTACHED                   "Data manager error: Attempt to use datapipe when no output group is attached"
#define __CPP_TRANSPORT_DATAMGR_PIPE_NULL_TASK                      "Data manager error: Null task supplied when attaching datapipe"
#define __CPP_TRANSPORT_DATAMGR_NO_OUTPUT_GROUP                     "Data manager error: Could not a matching output group for task"

#define __CPP_TRANSPORT_DATAMGR_TEMP_SERIAL_CREATE_FAIL             "Data manager error: Failed to create temporary table of serial numbers (backend code="
#define __CPP_TRANSPORT_DATAMGR_TEMP_SERIAL_INSERT_FAIL             "Data manager error: Failed to insert value in temporary table of serial numbers (backend code="
#define __CPP_TRANSPORT_DATAMGR_TEMP_SERIAL_DROP_FAIL               "Data manager error: Failed to drop temporary table of serial numbers (backend code="
#define __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_READ_FAIL               "Data manager error: Failed to select time sample (backend code="
#define __CPP_TRANSPORT_DATAMGR_TIME_SERIAL_TOO_FEW                 "Data manager error: Read fewer time samples from the container than expected"
#define __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_READ_FAIL            "Data manager error: Failed to select k-configuration sample (backend code="
#define __CPP_TRANSPORT_DATAMGR_KCONFIG_SERIAL_TOO_FEW              "Data manager error: Read fewer k-configuration samples from the container than expected"
#define __CPP_TRANSPORT_DATAMGR_TEMP_THREEPF_CREATE_FAIL            "Data manager error: Failed to create temporary table of threepf configurations (backend code="
#define __CPP_TRANSPORT_DATAMGR_TEMP_THREEPF_DROP_FAIL              "Data manager error: Failed to drop temporary table of threepf configurations (backend code="

#define __CPP_TRANSPORT_DATAMGR_NEGATIVE_SERIAL_NUMBERS             "Data manager error: Received negative serial numbers from database backend"

#endif // __CPP_TRANSPORT_MESSAGES_EN_DATA_MANAGER_H
