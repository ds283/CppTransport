//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#if !defined(__repository_records_forward_declare_H_) && !defined(__repository_records_H_)
#define __repository_records_forward_declare_H_


namespace transport
	{

		class record_metadata;
		class repository_record;
		template <typename number> class package_record;
		template <typename number> class task_record;
		template <typename number> class integration_task_record;
		template <typename number> class postintegration_task_record;
		template <typename number> class derived_product_record;
		template <typename number> class output_task_record;
    template <typename Payload> class output_group_record;

		class integration_metadata;
		class derived_content;
		class precomputed_products;
		class integration_payload;
		class postintegration_payload;
		class output_payload;

	}   // namespace transport


#endif //__repository_records_forward_declare_H_
