//
// Created by David Seery on 25/06/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#ifndef __CPP_TRANSPORT_TRANSPORT_H_
#define __CPP_TRANSPORT_TRANSPORT_H_

#define BOOST_ALL_DYN_LINK

#include "transport-runtime-api/messages.h"

#include "transport-runtime-api/models/observers.h"
#include "transport-runtime-api/models/canonical_model.h"

#include "transport-runtime-api/containers/index_selector.h"
#include "transport-runtime-api/plotgadgets/plot_gadget.h"
#include "transport-runtime-api/plotgadgets/python_plot_gadget.h"
#include "transport-runtime-api/plotgadgets/gnuplot_plot_gadget.h"
#include "transport-runtime-api/plotgadgets/asciitable_plot_gadget.h"

#include "transport-runtime-api/containers/data_products.h"

// current implementation uses unqlite as the repository database
#include "transport-runtime-api/unqlite/repository_unqlite.h"

// current implementation uses sqlite3 as the data container database
#include "transport-runtime-api/sqlite3/data_manager_sqlite3.h"

#include "transport-runtime-api/manager/task_manager.h"


#endif // __CPP_TRANSPORT_TRANSPORT_H_
