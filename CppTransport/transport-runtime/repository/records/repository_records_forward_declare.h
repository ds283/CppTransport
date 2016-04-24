//
// Created by David Seery on 26/03/15.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//


#if !defined(CPPTRANSPORT_REPOSITORY_RECORDS_FORWARD_DECLARE_H) && !defined(CPPTRANSPORT_REPOSITORY_RECORDS_H)
#define CPPTRANSPORT_REPOSITORY_RECORDS_FORWARD_DECLARE_H


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
    template <typename Payload> class content_group_record;

		class integration_metadata;
		class derived_content;
		class precomputed_products;
		class integration_payload;
		class postintegration_payload;
		class output_payload;

	}   // namespace transport


#endif //CPPTRANSPORT_REPOSITORY_RECORDS_FORWARD_DECLARE_H
