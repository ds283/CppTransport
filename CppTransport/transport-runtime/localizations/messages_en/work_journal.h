//
// Created by David Seery on 05/04/15.
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


#ifndef CPPTRANSPORT_WORK_JOURNAL_MESSAGES_H
#define CPPTRANSPORT_WORK_JOURNAL_MESSAGES_H


#define CPPTRANSPORT_JOURNAL_OPEN_FAIL                "Failed to create journal analysis file"
#define CPPTRANSPORT_JOURNAL_AGGREGATE_TOO_FEW        "Too few items in journal: Missing end-aggregation event"
#define CPPTRANSPORT_JOURNAL_MPI_TOO_FEW              "Too few items in journal: Missing end-mpi event"
#define CPPTRANSPORT_JOURNAL_DATABASE_TOO_FEW         "Too few items in journal: Missing end-database event"
#define CPPTRANSPORT_JOURNAL_TWOPF_TOO_FEW            "Too few items in journal: Missing end-twopf event"
#define CPPTRANSPORT_JOURNAL_THREEPF_TOO_FEW          "Too few items in journal: Missing end-threepf event"
#define CPPTRANSPORT_JOURNAL_ZETA_TWOPF_TOO_FEW       "Too few items in journal: Missing end-zeta-twopf event"
#define CPPTRANSPORT_JOURNAL_ZETA_THREEPF_TOO_FEW     "Too few items in journal: Missing end-zeta-threepf event"
#define CPPTRANSPORT_JOURNAL_FNL_TOO_FEW              "Too few items in journal: Missing end-fNL event"
#define CPPTRANSPORT_JOURNAL_OUTPUT_TOO_FEW           "Too few items in journal: Missing end-output event"
#define CPPTRANSPORT_JOURNAL_AGGREGATE_END_MISSING    "Expected end-aggregation event following begin-aggregation"
#define CPPTRANSPORT_JOURNAL_MPI_END_MISSING          "Expected end-mpi event following begin-mpi"
#define CPPTRANSPORT_JOURNAL_DATABASE_END_MISSING     "Expected end-database event following begin-database"
#define CPPTRANSPORT_JOURNAL_TWOPF_END_MISSING        "Expected end-twopf event following begin-twopf"
#define CPPTRANSPORT_JOURNAL_THREEPF_END_MISSING      "Expected end-threepf event following begin-threepf"
#define CPPTRANSPORT_JOURNAL_ZETA_TWOPF_END_MISSING   "Expected end-zeta-twopf event following begin-zeta-twopf"
#define CPPTRANSPORT_JOURNAL_ZETA_THREEPF_END_MISSING "Expected end-zeta-threepf event following begin-zeta-threepf"
#define CPPTRANSPORT_JOURNAL_FNL_END_MISSING          "Expected end-fBL event following begin-fNL"
#define CPPTRANSPORT_JOURNAL_OUTPUT_END_MISSING       "Expected end-output event following begin-output"

#define CPPTRANSPORT_JOURNAL_UNEXPECTED_EVENT         "Unexpected event in work journal"


#endif //CPPTRANSPORT_WORK_JOURNAL_MESSAGES_H
