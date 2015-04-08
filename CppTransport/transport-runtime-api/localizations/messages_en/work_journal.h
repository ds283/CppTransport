//
// Created by David Seery on 05/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __work_journal_messages_H_
#define __work_journal_messages_H_


#define __CPP_TRANSPORT_JOURNAL_OPEN_FAIL                "Failed to create journal analysis file"
#define __CPP_TRANSPORT_JOURNAL_AGGREGATE_TOO_FEW        "Too few items in journal: Missing end-aggregation event"
#define __CPP_TRANSPORT_JOURNAL_MPI_TOO_FEW              "Too few items in journal: Missing end-mpi event"
#define __CPP_TRANSPORT_JOURNAL_TWOPF_TOO_FEW            "Too few items in journal: Missing end-twopf event"
#define __CPP_TRANSPORT_JOURNAL_THREEPF_TOO_FEW          "Too few items in journal: Missing end-threepf event"
#define __CPP_TRANSPORT_JOURNAL_ZETA_TWOPF_TOO_FEW       "Too few items in journal: Missing end-zeta-twopf event"
#define __CPP_TRANSPORT_JOURNAL_ZETA_THREEPF_TOO_FEW     "Too few items in journal: Missing end-zeta-threepf event"
#define __CPP_TRANSPORT_JOURNAL_FNL_TOO_FEW              "Too few items in journal: Missing end-fNL event"
#define __CPP_TRANSPORT_JOURNAL_OUTPUT_TOO_FEW           "Too few items in journal: Missing end-output event"
#define __CPP_TRANSPORT_JOURNAL_AGGREGATE_END_MISSING    "Expected end-aggregation event following begin-aggregation"
#define __CPP_TRANSPORT_JOURNAL_MPI_END_MISSING          "Expected end-mpi event following begin-mpi"
#define __CPP_TRANSPORT_JOURNAL_TWOPF_END_MISSING        "Expected end-twopf event following begin-twopf"
#define __CPP_TRANSPORT_JOURNAL_THREEPF_END_MISSING      "Expected end-threepf event following begin-threepf"
#define __CPP_TRANSPORT_JOURNAL_ZETA_TWOPF_END_MISSING   "Expected end-zeta-twopf event following begin-zeta-twopf"
#define __CPP_TRANSPORT_JOURNAL_ZETA_THREEPF_END_MISSING "Expected end-zeta-threepf event following begin-zeta-threepf"
#define __CPP_TRANSPORT_JOURNAL_FNL_END_MISSING          "Expected end-fBL event following begin-fNL"
#define __CPP_TRANSPORT_JOURNAL_OUTPUT_END_MISSING       "Expected end-output event following begin-output"

#define __CPP_TRANSPORT_JOURNAL_UNEXPECTED_EVENT         "Unexpected event in work journal"


#endif //__work_journal_messages_H_
