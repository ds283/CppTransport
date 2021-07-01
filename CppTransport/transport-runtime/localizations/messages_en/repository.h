//
// Created by David Seery on 05/05/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_MESSAGES_EN_REPOSITORY_H
#define CPPTRANSPORT_MESSAGES_EN_REPOSITORY_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_REPO_MISSING_ROOT                         = "Root directory does not exist";
    constexpr auto CPPTRANSPORT_REPO_MISSING_DATABASE                     = "Repository database not present or inaccessible:";
    constexpr auto CPPTRANSPORT_REPO_MISSING_PACKAGE_STORE                = "Package store not present or inaccessible";
    constexpr auto CPPTRANSPORT_REPO_MISSING_TASK_STORE                   = "Task store not present or inaccessible";
    constexpr auto CPPTRANSPORT_REPO_MISSING_PRODUCT_STORE                = "Product store not present or inaccessible";
    constexpr auto CPPTRANSPORT_REPO_MISSING_OUTPUT_STORE                 = "Output store not present or inaccessible";
    constexpr auto CPPTRANSPORT_REPO_PACKAGE_EXISTS                       = "Attempt to insert duplicate package record";
    constexpr auto CPPTRANSPORT_REPO_TASK_EXISTS                          = "Attempt to insert duplicate task record";
    constexpr auto CPPTRANSPORT_REPO_PRODUCT_EXISTS                       = "Attempt to insert duplicate derived product record";
    constexpr auto CPPTRANSPORT_REPO_OUTPUT_EXISTS                        = "Attempt to insert duplicate output record";

    constexpr auto CPPTRANSPORT_REPO_PACKAGE_MISSING                      = "Missing record for package";
    constexpr auto CPPTRANSPORT_REPO_TASK_MISSING                         = "Missing record for task";
    constexpr auto CPPTRANSPORT_REPO_PRODUCT_MISSING                      = "Missing record for derived product";
    constexpr auto CPPTRANSPORT_REPO_OUTPUT_MISSING                       = "Missing record for output";

    constexpr auto CPPTRANSPORT_REPO_TASK_DUPLICATE                       = "Attempt to add record for already existing task";
    constexpr auto CPPTRANSPORT_REPO_PACKAGE_DUPLICATE                    = "Attempt to add record for already existing package";
    constexpr auto CPPTRANSPORT_REPO_PRODUCT_DUPLICATE                    = "Attempt to add record for already existing derived product";
    constexpr auto CPPTRANSPORT_REPO_CONTENT_DUPLICATE                    = "Attempt to add record for already existing content group";

    constexpr auto CPPTRANSPORT_REPO_GROUP_IS_LOCKED                      = "Attempt to delete locked content group";

    constexpr auto CPPTRANSPORT_REPO_COMMIT_FAILURE                       = "Could not write database record for";
    constexpr auto CPPTRANSPORT_REPO_DESERIALIZE_CANT_OPEN                = "Could not locate database record for";
    constexpr auto CPPTRANSPORT_REPO_DESERIALIZE_FAILURE                  = "Could not read database record for";

    constexpr auto CPPTRANSPORT_REPO_CANT_WRITE_FAILURE_PATH              = "Repository error: Error while attempting to move content group to failure cache";

    constexpr auto CPPTRANSPORT_TASK_NOT_SERIALIZABLE                     = "Serializable is not allowed for task";

    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_A                 = "Autocommit: derived product";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_B                 = "requires integration task";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_C                 = "Autocommit: derived product";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_PRODUCT_D                 = "requires postintegration task";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_A              = "Autocommit: postintegration task";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_B              = "requires integration task";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_C              = "Autocommit: postintegration task";
    constexpr auto CPPTRANSPORT_REPO_AUTOCOMMIT_POSTINTEGR_D              = "requires postintegration task";

    constexpr auto CPPTRANSPORT_REPO_NOTE_NO_STATISTICS                   = "Collection of per-configuration timing statistics disabled because not supported by backend";
    constexpr auto CPPTRANSPORT_REPO_NOTE_NO_INTEGRATION                  = "Cannot be used for integrations over the spectrum or bispectrum because task does not linearly step in k-configurations or the storage policy disables some configurations";

    constexpr auto CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_A           = "Committed content group";
    constexpr auto CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_B           = "for task";
    constexpr auto CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_C           = "tags";
    constexpr auto CPPTRANSPORT_REPO_COMMITTING_CONTENT_GROUP_D           = "at";

    constexpr auto CPPTRANSPORT_REPO_WARN_CONTENT_GROUP_A                 = "Content group";
    constexpr auto CPPTRANSPORT_REPO_WARN_CONTENT_GROUP_B                 = "has missing content";

    constexpr auto CPPTRANSPORT_REPO_CONTENT_FAILED_A                     = "Content group";
    constexpr auto CPPTRANSPORT_REPO_CONTENT_FAILED_B                     = "moved to fail cache in repository";

    constexpr auto CPPTRANSPORT_REPO_FAILED_TASK_CONTENT_GROUP_A          = "Task";
    constexpr auto CPPTRANSPORT_REPO_FAILED_TASK_CONTENT_GROUP_B          = "Errors were encountered during integration";

    constexpr auto CPPTRANSPORT_REPO_FAILED_OUTPUT_CONTENT_GROUP_A        = "Task";
    constexpr auto CPPTRANSPORT_REPO_FAILED_OUTPUT_CONTENT_GROUP_B        = "Some derived content could not be generated";

    constexpr auto CPPTRANSPORT_REPO_FAILED_POSTINT_GROUP_A               = "Task";
    constexpr auto CPPTRANSPORT_REPO_FAILED_POSTINT_GROUP_B               = "Some postprocessing could not be completed";

    constexpr auto CPPTRANSPORT_REPO_NOT_SET                              = "Attempt to access repository, but it has not yet been set";
    constexpr auto CPPTRANSPORT_INTEGRATION_CONTENT_FINDER_NOT_SET        = "Attempt to use integration content finder, but it has not yet been set";
    constexpr auto CPPTRANSPORT_POSTINTEGRATION_CONTENT_FINDER_NOT_SET    = "Attempt to use integration content finder, but it has not yet been set";

    constexpr auto CPPTRANSPORT_REPO_MISSING_RECORD                       = "Could not find database record";

    constexpr auto CPPTRANSPORT_REPO_SKIPPING_TASK                        = "; skipping this task";
    constexpr auto CPPTRANSPORT_REPO_NONE                                 = "Nothing to do: no repository specified";
    constexpr auto CPPTRANSPORT_REPO_FOR_TASK                             = "while processing task";
    constexpr auto CPPTRANSPORT_REPO_OUTPUT_WRITER_UNSETHANDLE            = "Attempt to read an unset 'data_manager' data-container handle in 'integration_writer'";
    constexpr auto CPPTRANSPORT_REPO_WRITER_AGGREGATOR_UNSET              = "Aggregator unset in writer object";
    constexpr auto CPPTRANSPORT_REPO_WRITER_FAILURE_UNSUPPORTED           = "Writer doesn't support failure lists";

    constexpr auto CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_INTGRTN          = "Attempt to enumerate derived contents for non-integration task";
    constexpr auto CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_POSTINT          = "Attempt to enumerate derived contents for non-postintegration task";
    constexpr auto CPPTRANSPORT_REPO_EXTRACT_DERIVED_NOT_OUTPUT           = "Attempt to enumerate derived contents for non-output task";

    constexpr auto CPPTRANSPORT_REPO_TASK_NOT_INTEGRATION                 = "Derived product requires an integration task";
    constexpr auto CPPTRANSPORT_REPO_ZETA_TASK_NOT_DERIVABLE              = "Parent task specified in zeta task is not of derivable-type";
    constexpr auto CPPTRANSPORT_REPO_NO_MATCHING_CONTENT_GROUPS           = "No matching content groups for task";

    constexpr auto CPPTRANSPORT_REPO_TASK_DESERIALIZE_FAIL                = "Failed to deserialize task";
    constexpr auto CPPTRANSPORT_REPO_PRODUCT_DESERIALIZE_FAIL             = "Failed to deserialize derived product";

    constexpr auto CPPTRANSPORT_REPO_RECORD_CAST_FAILED                   = "Dynamic cast of repository record failed";

    constexpr auto CPPTRANSPORT_REPO_COMMIT_READONLY                      = "Attempt to commit read-only record";

    constexpr auto CPPTRANSPORT_REPOSITORY_MODE_READONLY                  = "read-only";
    constexpr auto CPPTRANSPORT_REPOSITORY_MODE_READWRITE                 = "read-write";

  }   // namespace transport


#endif // CPPTRANSPORT_MESSAGES_EN_REPOSITORY_H
