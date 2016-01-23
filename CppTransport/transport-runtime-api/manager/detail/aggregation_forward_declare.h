//
// Created by David Seery on 22/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#if !defined(CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_FORWARD_DECLARE_H) && !defined(CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION)
#define CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_FORWARD_DECLARE_H


namespace transport
  {

    namespace master_controller_impl
      {

        template <typename number> class integration_aggregator;
        template <typename number> class postintegration_aggregator;
        template <typename number> class derived_content_aggregator;

        class aggregation_record;
        template <typename number> class integration_aggregation_record;
        template <typename number> class postintegration_aggregation_record;
        template <typename number> class derived_content_aggregation_record;

      }   // namespace master_controller_impl

  }   // namespace transport


#endif //CPPTRANSPORT_MANAGER_DETAIL_AGGREGATION_FORWARD_DECLARE_H
