//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_TASKS_FORWARD_DECLARE_H
#define CPPTRANSPORT_TASKS_FORWARD_DECLARE_H

#include <iostream>

#include "sqlite3.h"

#include "transport-runtime/tasks/task_types.h"


namespace transport
	{

#ifndef CPPTRANSPORT_TASK_H
    template <typename number> class task;
#endif

#ifndef CPPTRANSPORT_DERIVABLE_TASK_H
		template <typename number> class derivable_task;
#endif

#ifndef CPPTRANSPORT_INTEGRATION_ABSTRACT_TASK_H
    template <typename number> class integration_task;
#endif

#ifndef CPPTRANSPORT_TWOPF_LIST_TASK_H
    template <typename number> class twopf_db_task;
#endif

#ifndef CPPTRANSPORT_TWOPF_TASK_H
    template <typename number> class twopf_task;
#endif

#ifndef CPPTRANSPORT_THREEPF_TASK_H
    template <typename number> class threepf_task;
#endif

#ifndef CPPTRANSPORT_TASK_HELPER_H
    namespace integration_task_helper
	    {
        template <typename number>
        std::unique_ptr< integration_task<number> > deserialize(const std::string& nm, Json::Value& reader, sqlite3* handle, package_finder<number>& f);
	    }
#endif

#ifndef CPPTRANSPORT_OUTPUT_TASKS_H
    template <typename number> class output_task;
#endif

#ifndef CPPTRANSPORT_TASK_HELPER_H
    namespace output_task_helper
	    {
        template <typename number>
        std::unique_ptr< output_task<number> > deserialize(const std::string& nm, Json::Value& reader, derived_product_finder<number>& pfinder);
	    }
#endif

#ifndef CPPTRANSPORT_POSTINTEGRATION_ABSTRACT_TASK_H
    template <typename number> class postintegration_task;
#endif

#ifndef CPPTRANSPORT_ZETA_TWOPF_LIST_TASK_H
		template <typename number> class zeta_twopf_list_task;
#endif

#ifndef CPPTRANSPORT_ZETA_TWOPF_TASK_H
    template <typename number> class zeta_twopf_task;
#endif

#ifndef CPPTRANSPORT_ZETA_THREEPF_TASK_H
    template <typename number> class zeta_threepf_task;
#endif

#ifndef CPPTRANSPORT_FNL_TASK_H
    template <typename number> class fNL_task;
#endif

#ifndef CPPTRANSPORT_TASK_HELPER_H
    namespace postintegration_task_helper
	    {
        template <typename number>
        std::unique_ptr< postintegration_task<number> > deserialize(const std::string& nm, Json::Value& reader, task_finder<number>& f);
	    }
#endif

	}   // namespace transport


#endif //CPPTRANSPORT_TASKS_FORWARD_DECLARE_H
