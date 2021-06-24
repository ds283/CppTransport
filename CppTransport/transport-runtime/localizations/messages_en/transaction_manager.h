//
// Created by David Seery on 18/04/2016.
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

#ifndef CPPTRANSPORT_MESSAGES_EN_TRANSACTION_MANAGER_H
#define CPPTRANSPORT_MESSAGES_EN_TRANSACTION_MANAGER_H



#define CPPTRANSPORT_TRANSACTION_NO_LOCK         "Repository error: transaction manager cannot gain exclusive repository lock"
#define CPPTRANSPORT_TRANSACTION_LOST_LOCK       "Repository error: transaction manager lost exclusive lock"

#define CPPTRANSPORT_RECOVER_WHILE_TRANSACTIONS  "Repository error: attempt to perform recovery while transactions are in progress"

#define CPPTRANSPORT_TRANSACTION_UNDERWAY        "Repository error: attempt to begin a new transaction while one is already underway"
#define CPPTRANSPORT_TRANSACTION_COMMITTED       "Repository error: attempt to add to a transaction which has already been committed"
#define CPPTRANSPORT_TRANSACTION_DEAD            "Repository error: attempt to add to a transaction which has been rolled back"

#define CPPTRANSPORT_TRANSACTION_OVER_RELEASE    "Over-release of transaction"



#endif //CPPTRANSPORT_MESSAGES_EN_TRANSACTION_MANAGER_H
