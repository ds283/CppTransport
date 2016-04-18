//
// Created by David Seery on 18/04/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_MESSAGES_EN_TRANSACTION_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_TRANSACTION_MANAGER_H



#define CPPTRANSPORT_TRANSACTION_NO_LOCK         "Repository error: transaction manager cannot gain exclusive repository lock"
#define CPPTRANSPORT_TRANSACTION_LOST_LOCK       "Repository error: transaction manager lost exclusive lock"

#define CPPTRANSPORT_RECOVER_WHILE_TRANSACTIONS  "Repository error: attempt to perform recovery while transactions are in progress"

#define CPPTRANSPORT_TRANSACTION_UNDERWAY        "Repository error: attempt to begin a new transaction while one is already underway"
#define CPPTRANSPORT_TRANSACTION_COMMITTED       "Repository error: attempt to add to a transaction which has already been committed"
#define CPPTRANSPORT_TRANSACTION_DEAD            "Repository error: attempt to add to a transaction which has been rolled back"

#define CPPTRANSPORT_TRANSACTION_OVER_RELEASE    "Internal error: over-release of transaction"



#endif //CPPTRANSPORT_MESSAGES_EN_TRANSACTION_MANAGER_H
