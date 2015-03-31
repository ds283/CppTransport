//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H
#define __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H


#define __CPP_TRANSPORT_REPO_MISSING_ROOT                "Repository error: Root directory does not exist"
#define __CPP_TRANSPORT_REPO_MISSING_DATABASE            "Repository error: Repository database not present or inaccessible:"
#define __CPP_TRANSPORT_REPO_MISSING_PACKAGE_STORE       "Repository error: Package store not present or inaccessible"
#define __CPP_TRANSPORT_REPO_MISSING_TASK_STORE          "Repository error: Task store not present or inaccessible"
#define __CPP_TRANSPORT_REPO_MISSING_PRODUCT_STORE       "Repository error: Product store not present or inaccessible"
#define __CPP_TRANSPORT_REPO_MISSING_OUTPUT_STORE        "Repository error: Output store not present or inaccessible"

#define __CPP_TRANSPORT_REPO_PACKAGE_EXISTS              "Repository error: attempt to insert duplicate package record"
#define __CPP_TRANSPORT_REPO_TASK_EXISTS                 "Repository error: attempt to insert duplicate task record"
#define __CPP_TRANSPORT_REPO_PRODUCT_EXISTS              "Repository error: attempt to insert duplicate derived product record"
#define __CPP_TRANSPORT_REPO_OUTPUT_EXISTS               "Repository error: attempt to insert duplicate output record"

#define __CPP_TRANSPORT_REPO_PACKAGE_MISSING             "Repository error: missing record for package"
#define __CPP_TRANSPORT_REPO_TASK_MISSING                "Repository error: missing record for task"
#define __CPP_TRANSPORT_REPO_PRODUCT_MISSING             "Repository error: missing record for derived product"
#define __CPP_TRANSPORT_REPO_OUTPUT_MISSING              "Repository error: missing record for output"

#define __CPP_TRANSPORT_REPO_TASK_DUPLICATE              "Repository error: duplicate task"
#define __CPP_TRANSPORT_REPO_PACKAGE_DUPLICATE           "Repository error: duplicate package"
#define __CPP_TRANSPORT_REPO_PRODUCT_DUPLICATE           "Repository error: duplicate derived product"
#define __CPP_TRANSPORT_REPO_CONTENT_DUPLICATE           "Repository error: duplicate content group"

#define __CPP_TRANSPORT_REPO_COMMIT_FAILURE              "Repository error: could not write database record for"
#define __CPP_TRANSPORT_REPO_DESERIALIZE_FAILURE         "Repository error: could not read database record for"

#define __CPP_TRANSPORT_REPO_RECORD_EXISTS_A             "Repository error: Attempt to insert duplicate"
#define __CPP_TRANSPORT_REPO_RECORD_EXISTS_B             "record"

#define __CPP_TRANSPORT_REPO_RECORD_MISSING_A            "Repository error:"
#define __CPP_TRANSPORT_REPO_RECORD_MISSING_B            "record for"
#define __CPP_TRANSPORT_REPO_RECORD_MISSING_C            "not found when committing update to repository"

#define __CPP_TRANSPORT_REPO_RECORD_DUPLICATE_A          "Repository error: Duplicate"
#define __CPP_TRANSPORT_REPO_RECORD_DUPLICATE_B          "record for"

#define __CPP_TRANSPORT_REPO_INSERT_ERROR                "Repository error: Error while inserting document into repository (backend code="
#define __CPP_TRANSPORT_REPO_QUERY_ERROR                 "Repository error: Error while querying document from repository (backend code="
#define __CPP_TRANSPORT_REPO_DELETE_ERROR                "Repository error: Error while deleting repository document (backend id="
#define __CPP_TRANSPORT_REPO_COMMIT_ERROR                "Repository error: Error while committing changes to repository (backend id="
#define __CPP_TRANSPORT_REPO_PRECOMMIT_ERROR             "Repository error: Error while preparing to commit changes to repository (backend id="

#define __CPP_TRANSPORT_REPO_CANT_WRITE_FAILURE_PATH     "Repository error: Error while attempting to move output group to failure cache"

#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_A    "Autocommit: integration task"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_INTEGRATION_B    "requires package"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A        "Autocommit: derived product"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B        "requires integration task"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_OUTPUT_A         "Autocommit: output task"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_OUTPUT_B         "requires derived product"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_A     "Autocommit: postintegration task"
#define __CPP_TRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_B     "requires integration task"

#define __CPP_TRANSPORT_REPO_NOTE_NO_STATISTICS          "Collection of per-configuration timing statistics disabled because not supported by backend"
#define __CPP_TRANSPORT_REPO_NOTE_NO_INTEGRATION         "Cannot be used for integrations over the spectrum or bispectrum because task does not linearly step in k-configurations or the storage policy disables some configurations"

#define __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_A   "Committed output group"
#define __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_B   "for task"
#define __CPP_TRANSPORT_REPO_COMMITTING_OUTPUT_GROUP_C   "tags"

#define __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_A   "Committed write-back of post-integration products to output group"
#define __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_B   "for task"
#define __CPP_TRANSPORT_REPO_WRITEBACK_POSTINT_GROUP_C   "parent task"

#define __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_A       "Task"
#define __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_B       "Some momentum configurations failed to integrate, or other errors were encountered. Failing output group"
#define __CPP_TRANSPORT_REPO_FAILED_OUTPUT_GROUP_C       "moved to fail cache in repository"

#define __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_A      "Task"
#define __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_B      "Some derived content could not be generated, or other errors were encountered. Failing output group"
#define __CPP_TRANSPORT_REPO_FAILED_CONTENT_GROUP_C      "moved to fail cache in repository"

#define __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_A      "Task"
#define __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_B      "Some postprocessing could not be completed, or other errors were encountered. Failing output group"
#define __CPP_TRANSPORT_REPO_FAILED_POSTINT_GROUP_C      "moved to fail cache in repository"

#define __CPP_TRANSPORT_REPO_ROOT_EXISTS                 "Repository error: Root directory already exists"
#define __CPP_TRANSPORT_REPO_WRITE_SLAVE                 "Internal error: Attempt to write to repository from MPI slave process"
#define __CPP_TRANSPORT_REPO_NOT_SET                     "Internal error: Attempt to access repository when unset in 'task_manager'"

#define __CPP_TRANSPORT_REPO_NULL_MODEL                  "Internal error: Null model in repository"
#define __CPP_TRANSPORT_REPO_NULL_TASK                   "Internal error: Null task in repository"
#define __CPP_TRANSPORT_REPO_NULL_SERIALIZATION_READER   "Internal error: Null serialization reader in repository"
#define __CPP_TRANSPORT_REPO_NULL_RECORD                 "Internal error: Null record in repository"

#define __CPP_TRANSPORT_REPO_MISSING_RECORD              "Repository error: Could not find database record"
#define __CPP_TRANSPORT_REPO_MISSING_PACKAGE             "Repository error: Could not find specified package"
#define __CPP_TRANSPORT_REPO_MISSING_PRODUCT             "Repository error: Could not find specified derived product"
#define __CPP_TRANSPORT_REPO_MISSING_CONTENT             "Repostiory error: Could not find specified content group"

#define __CPP_TRANSPORT_REPO_SKIPPING_TASK               "; skipping this task"
#define __CPP_TRANSPORT_REPO_NONE                        "Nothing to do: no repository specified"
#define __CPP_TRANSPORT_RUN_REPAIR                       "; consider checking database integrity"
#define __CPP_TRANSPORT_REPO_FOR_TASK                    "while processing task"
#define __CPP_TRANSPORT_UNKNOWN_JOB_TYPE                 "Internal error: Unexpected job type in 'task_manager'"
#define __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE   "Repository error: Attempt to read an unset 'data_manager' data-container handle in 'integration_writer'"
#define __CPP_TRANSPORT_REPO_OUTPUT_WRITER_UNSETTASK     "Repository error: Attempt to read an unset 'data_manager' taskfile handle in 'integration_writer'"
#define __CPP_TRANSPORT_REPO_DERIVED_WRITER_UNSETTASK    "Repository error: Attempt to read an unset 'data_manager' taskfile handle in 'derived_content_writer'"
#define __CPP_TRANSPORT_REPO_WRITER_AGGREGATOR_UNSET     "Repository error: Aggregator unset in writer object"
#define __CPP_TRANSPORT_REPO_WRITER_TWOPF_MERGER_UNSET   "Repository error: zeta twopf merger unset in writer object"
#define __CPP_TRANSPORT_REPO_WRITER_THREEPF_MERGER_UNSET "Repository error: zeta threepf merger unset in writer object"
#define __CPP_TRANSPORT_REPO_WRITER_REDBSP_MERGER_UNSET  "Repository error: zeta reduced bispectrum merger unset in writer object"
#define __CPP_TRANSPORT_REPO_WRITER_FNL_MERGER_UNSET     "Repository error: fNL merger unset in writer object"

#define __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN "Repository error: Attempt to enumerate derived contents for non-integration task"
#define __CPP_TRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT  "Repository error: Attempt to enumerate derived contents for non-output task"

#define __CPP_TRANSPORT_REPO_OUTPUT_TASK_NOT_INTGRTN     "Repository error: task named in output task is not of integration-type"
#define __CPP_TRANSPORT_REPO_ZETA_TASK_NOT_INTGRTN       "Repository error: task named in zeta task is not of integration-type"
#define __CPP_TRANSPORT_REPO_NO_MATCHING_OUTPUT_GROUPS   "Repository error: no matching output groups for task"

#define __CPP_TRANSPORT_REPO_TASK_DESERIALIZE_FAIL       "Repository error: failed to deserialize task"
#define __CPP_TRANSPORT_REPO_PRODUCT_DESERIALIZE_FAIL    "Repository error: failed to deserialize derived product"

#define __CPP_TRANSPORT_REPO_PACKAGE_RECORD              "package"
#define __CPP_TRANSPORT_REPO_TASK_RECORD                 "task"
#define __CPP_TRANSPORT_REPO_INTEGRATION_TASK_RECORD     "integration task"
#define __CPP_TRANSPORT_REPO_POSTINTEGRATION_TASK_RECORD "postintegration task"
#define __CPP_TRANSPORT_REPO_OUTPUT_TASK_RECORD          "output task"
#define __CPP_TRANSPORT_REPO_DERIVED_PRODUCT_RECORD      "derived product"
#define __CPP_TRANSPORT_REPO_CONTENT_RECORD              "content"

#define __CPP_TRANSPORT_REPO_UNKNOWN_RECORD_TYPE         "Repository error: unknown type for record"
#define __CPP_TRANSPORT_REPO_RECORD_CAST_FAILED          "Internal error: dynamic cast of repository record failed"

#define __CPP_TRANSPORT_REPO_TASK_IS_OUTPUT              "Internal error: received unexpected output task"
#define __CPP_TRANSPORT_REPO_TASK_IS_INTEGRATION         "Internal error: received unexpected integration task"
#define __CPP_TRANSPORT_REPO_TASK_IS_POSTINTEGRATION     "Internal error: received unexpected postintegration task"

#define __CPP_TRANSPORT_PROCESSED_TASKS_A                "Task manager: processed"
#define __CPP_TRANSPORT_PROCESSED_TASKS_B_SINGULAR       "database task"
#define __CPP_TRANSPORT_PROCESSED_TASKS_B_PLURAL         "database tasks"
#define __CPP_TRANSPORT_PROCESSED_TASKS_C                "in wallclock time"


#endif // __CPP_TRANSPORT_MESSAGES_EN_REPOSITORY_H
