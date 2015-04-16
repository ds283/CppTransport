//
// Created by David Seery on 16/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __time_config_database_H_
#define __time_config_database_H_


#include <functional>
#include <assert.h>


namespace transport
  {

    // TIME CONFIGURATION STORAGE POLICIES

    //! defines a 'time-configuration storage policy' data object, passed to a policy specification
    //! for the purpose of deciding whether a time configuration will be kept
    class time_config_storage_policy_data
      {
      public:
        time_config_storage_policy_data(double t, unsigned int s)
          : serial(s), time(t)
          {
          }

      public:
        unsigned int serial;
        double       time;
      };

    //! defines a 'time-configuration storage policy' object which determines which time steps are retained in the database
    typedef std::function<bool(time_config_storage_policy_data&)> time_config_storage_policy;

    class time_storage_record
      {
      public:
        time_storage_record(bool s, unsigned int n, double t)
          : store(s),
            tserial(n),
            time(t)
          {
            assert(s == true || (s== false && n == 0));
          }

        bool store;
        unsigned int tserial;
        double time;
      };

    //! default time configuration storage policy - store everything
    class default_time_config_storage_policy
      {
      public:
        bool operator() (time_config_storage_policy_data&) { return(true); }
      };

  }   // namespace transport


#endif //__time_config_database_H_
