//
// Created by David Seery on 20/08/2015.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_COMPUTE_GADGETS_COMMON_H
#define CPPTRANSPORT_COMPUTE_GADGETS_COMMON_H


namespace transport
  {

    namespace derived_data
      {


        namespace derived_data_impl
          {

            enum class fixed_index { first_index, second_index };


            enum class operator_position { left_pos, middle_pos, right_pos };

          }   // namespace derived_data_impl


      }   // namespace derived_data

  }   // namespace transport


#endif //CPPTRANSPORT_COMPUTE_GADGETS_COMMON_H
