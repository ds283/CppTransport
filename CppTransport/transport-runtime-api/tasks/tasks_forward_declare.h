//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __tasks_forward_declare_H_
#define __tasks_forward_declare_H_


namespace transport
	{

#ifndef __task_H_
    template <typename number> class task;
#endif

#ifndef __derivable_task_H_
		template <typename number> class derivable_task;
#endif

#ifndef __integration_abstract_tasks_H_
    template <typename number> class integration_task;
#endif

#ifndef __twopf_list_task_H_
    template <typename number> class twopf_list_task;
#endif

#ifndef __twopf_task_H_
    template <typename number> class twopf_task;
#endif

#ifndef __threepf_task_H_
    template <typename number> class threepf_task;
#endif

#ifndef __task_helper_H_
    namespace integration_task_helper
	    {
        template <typename number>
        integration_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::package_finder& f);
	    }
#endif

#ifndef __output_tasks_H_
    template <typename number> class output_task;
#endif

#ifndef __task_helper_H_
    namespace output_task_helper
	    {
        template <typename number>
        output_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::derived_product_finder& pfinder);
	    }
#endif

#ifndef __postintegration_abstract_task_H_
    template <typename number> class postintegration_task;
#endif

#ifndef __zeta_twopf_list_task_H_
		template <typename number> class zeta_twopf_list_task;
#endif

#ifndef __zeta_twopf_task_H_
    template <typename number> class zeta_twopf_task;
#endif

#ifndef __zeta_threepf_task_H_
    template <typename number> class zeta_threepf_task;
#endif

#ifndef __fNL_task_H_
    template <typename number> class fNL_task;
#endif

#ifndef __task_helper_H_
    namespace postintegration_task_helper
	    {
        template <typename number>
        postintegration_task<number>* deserialize(const std::string& nm, Json::Value& reader, typename repository_finder<number>::task_finder& f);
	    }
#endif

	}   // namespace transport


#endif //__tasks_forward_declare_H_
