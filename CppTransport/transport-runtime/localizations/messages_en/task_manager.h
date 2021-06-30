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


#ifndef CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H


namespace transport
  {

    constexpr auto CPPTRANSPORT_EXEC_SLAVE                         = "Task_manager::execute_tasks() called on slave process";
    constexpr auto CPPTRANSPORT_WAIT_MASTER                        = "Task_manager::wait_for_tasks() called on master process";
    constexpr auto CPPTRANSPORT_UNKNOWN_DERIVED_TASK               = "Unknown derived 'task<number>' class for task";
    constexpr auto CPPTRANSPORT_TOO_FEW_WORKERS                    = "Too few workers: require at least two worker processes to process a task";
    constexpr auto CPPTRANSPORT_UNEXPECTED_MPI                     = "Unexpected MPI message received";

    constexpr auto CPPTRANSPORT_UNEXPECTED_UNHANDLED               = "Unexpected unhandled exception";

    constexpr auto CPPTRANSPORT_TASK_MISSING                       = "Could not find repository record for task";
    constexpr auto CPPTRANSPORT_TASK_CANT_BE_READ                  = "Can not read repository record for task";

    constexpr auto CPPTRANSPORT_FILTER_TAG                         = "Filter for work items:";

    constexpr auto CPPTRANSPORT_EXPECTED_TWOPF_TASK                = "Expected postintegration parent task to be of type twopf_task, but instead found task";
    constexpr auto CPPTRANSPORT_EXPECTED_THREEPF_TASK              = "Expected postintegration parent task to be of type threepf_task, but instead found task";
    constexpr auto CPPTRANSPORT_EXPECTED_ZETA_THREEPF_TASK         = "Expected postintegration parent task to be of type zeta_threepf_task, but instead found task";

    constexpr auto CPPTRANSPORT_NO_CANDIDATE_INTEGRATION_GROUP     = "Could not find a candidate integration content group for task";
    constexpr auto CPPTRANSPORT_NO_CANDIDATE_POSTINTEGRATION_GROUP = "Could not find a candidate postintegrationc ontent group for task";
    constexpr auto CPPTRANSPORT_POSTINTEGRATION_HAS_OUTPUT_SOURCE  = "Postintegration task has parent output task";

    constexpr auto CPPTRANSPORT_SEED_GROUP_NOT_FOUND_A             = "Could not find a matching content group";
    constexpr auto CPPTRANSPORT_SEED_GROUP_NOT_FOUND_B             = "to seed task";

    constexpr auto CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_A    = "Paired groups";
    constexpr auto CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_B    = "and";
    constexpr auto CPPTRANSPORT_SEED_GROUP_MISMATCHED_SERIALS_C    = "do not have the same missing k-configurations and cannot be used to seed a paired integration";

    constexpr auto CPPTRANSPORT_PROCESSING_GANTT_CHART             = "generating process Gantt chart";
    constexpr auto CPPTRANSPORT_PROCESSING_ACTIVITY_JOURNAL        = "generating activity journal";

    constexpr auto CPPTRANSPORT_TASK_MANAGER_LABEL                 = "Task manager:";
    constexpr auto CPPTRANSPORT_TASK_MANAGER_WARNING_LABEL         = "Warning:";

    constexpr auto CPPTRANSPORT_UNKNOWN_SWITCH                     = "Ignored unknown command-line switch";
    constexpr auto CPPTRANSPORT_UNKNOWN_PLOT_STYLE                 = "Ignored unknown plot style";
    constexpr auto CPPTRANSPORT_UNKNOWN_MPL_BACKEND                = "Ignored unknown Matplotlib backend";
    constexpr auto CPPTRANSPORT_UNKNOWN_REPORT_INTERVAL            = "Ignored unrecognized report interval";
    constexpr auto CPPTRANSPORT_UNKNOWN_REPORT_DELAY               = "Ignored unrecognized report time delay";
    constexpr auto CPPTRANSPORT_UNKNOWN_CHECKPOINT_INTERVAL        = "Ignored unrecognized checkpoint interval";
    constexpr auto CPPTRANSPORT_UNKNOWN_REPORT_FLAGS               = "Ignored unrecognized email reporting flags";

    constexpr auto CPPTRANSPORT_MASTER_REPORTED_BY_WORKER          = "reported by worker";

    constexpr auto CPPTRANSPORT_SLAVE_NOT_INTEGRATION_CONTENT      = "Expected datapipe to be attached to an integration payload";
    constexpr auto CPPTRANSPORT_SLAVE_NOT_POSTINTEGRATION_CONTENT  = "Expected datapipe to be attached to a postintegration payload";

  }   // namespace transport


#endif // CPPTRANSPORT_MESSAGES_EN_TASK_MANAGER_H
