//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <vector>
#include <list>
#include <functional>
#include <memory>

// forward-declare model if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// need repository definitions
#include "transport-runtime-api/repository/records/repository_records.h"
#include "transport-runtime-api/repository/writers/writers.h"

// need batcher definitions
#include "transport-runtime-api/data/batchers/batchers.h"

// need datapipe defintions
#include "transport-runtime-api/data/datapipe/datapipe.h"

#include "boost/filesystem/operations.hpp"


namespace transport
  {

    template <typename number>
    class data_manager
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! Create a data_manager instance with a nominated capacity per batcher
        data_manager(unsigned int bcap, unsigned int dcap, unsigned int zcap)
          : batcher_capacity(bcap),
            pipe_data_capacity(dcap),
            pipe_zeta_capacity(zcap)
          {
          }

        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager() = default;


        // CAPACITY ADMIN

      public:

        //! Return the maximum memory available for batchers on this worker
        size_t get_batcher_capacity() const { return(this->batcher_capacity); }

        //! Set the maximum memory avilable for batchers on this worker
        void set_batcher_capacity(size_t size) { this->batcher_capacity = size; }

        //! Return the maximum memory available for data cache on this worker
        size_t get_data_capacity() const { return(this->pipe_data_capacity); }

        //! Set capacity available for data cache on this worker
        void set_data_capacity(size_t size) { this->pipe_data_capacity = size; }

        //! Return the maximum memory available for zeta cache on this worker
        size_t get_zeta_capacity() const { return(this->pipe_zeta_capacity); }

        //! Set capacity available for zeta cache on this worker
        void set_zeta_capacity(size_t size) { this->pipe_zeta_capacity = size; }


        // WRITER HANDLING

      public:

        //! Initialize an integration_writer object.
        //! Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void initialize_writer(std::shared_ptr< integration_writer<number> >& writer) = 0;

        //! Close an integration_writer object.
        virtual void close_writer(std::shared_ptr< integration_writer<number> >& writer) = 0;

        //! Initialize a derived_content_writer object.
        virtual void initialize_writer(std::shared_ptr< derived_content_writer<number> >& writer) = 0;

        //! Close an open derived_content_writer object.
        virtual void close_writer(std::shared_ptr< derived_content_writer<number> >& writer) = 0;

        //! Initialize a postintegration_writer object.
        virtual void initialize_writer(std::shared_ptr< postintegration_writer<number> >& writer) = 0;

        //! Close an open postintegration_writer object.
        virtual void close_writer(std::shared_ptr< postintegration_writer<number> >& writer) = 0;


        // WRITE TABLES FOR A DATA CONTAINER

      public:

        //! Create tables needed for a twopf container
        virtual void create_tables(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk) = 0;

        //! Create tables needed for a zeta twopf container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk) = 0;

        //! Create tables needed for a zeta threepf container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk) = 0;

        //! Create tables needed for an fNL container
        virtual void create_tables(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk) = 0;


        // SEEDING

      public:

        //! Seed a writer for a twopf task
        virtual void seed_writer(std::shared_ptr< integration_writer<number> >& writer, twopf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<integration_payload> >& seed) = 0;

        //! Seed a writer for a threepf task
        virtual void seed_writer(std::shared_ptr< integration_writer<number> >& writer, threepf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<integration_payload> >& seed) = 0;

        //! Seed a writer for a zeta twopf task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_twopf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) = 0;

        //! Seed a writer for a zeta threepf task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, zeta_threepf_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) = 0;

        //! Seed a writer for an fNL task
        virtual void seed_writer(std::shared_ptr< postintegration_writer<number> >& writer, fNL_task<number>* tk,
                                 const std::shared_ptr< output_group_record<postintegration_payload> >& seed) = 0;


        // CONSTRUCT BATCHERS

      public:

        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher<number> create_temp_twopf_container(twopf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                  const boost::filesystem::path& logdir,
                                                                  unsigned int worker, unsigned int group, model<number>* m,
                                                                  generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher<number> create_temp_threepf_container(threepf_task<number>* tk, const boost::filesystem::path& tempdir,
                                                                      const boost::filesystem::path& logdir,
                                                                      unsigned int worker, unsigned int group, model<number>* m,
                                                                      generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for zeta twopf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_twopf_batcher<number> create_temp_zeta_twopf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                            unsigned int worker, generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for zeta threepf data. Returns a batcher which can be used for writing to the container.
        virtual zeta_threepf_batcher<number> create_temp_zeta_threepf_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                                                unsigned int worker, generic_batcher::container_dispatch_function dispatcher) = 0;

        //! Create a temporary container for fNL data. Returns a batcher which can be used for writing to the container.
        virtual fNL_batcher<number> create_temp_fNL_container(const boost::filesystem::path& tempdir, const boost::filesystem::path& logdir,
                                                              unsigned int worker, generic_batcher::container_dispatch_function dispatcher, derived_data::template_type type) = 0;


		    // DATA PIPES AND DATA ACCESS

      public:

		    //! Create a datapipe
		    virtual datapipe<number> create_datapipe(const boost::filesystem::path& logdir, const boost::filesystem::path& tempdir,
		                                             typename datapipe<number>::integration_content_finder integration_finder,
		                                             typename datapipe<number>::postintegration_content_finder postintegration_finder,
		                                             typename datapipe<number>::dispatch_function dispatcher,
		                                             unsigned int worker, bool no_log = false) = 0;

        //! Pull a set of time sample-points from a datapipe
        virtual void pull_time_config(datapipe<number>* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<double>& sample) = 0;

        //! Pull a set of 2pf k-configuration serial numbers from a datapipe
        virtual void pull_kconfig_twopf(datapipe<number>* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<twopf_kconfig>& sample) = 0;

        //! Pull a set of 3pd k-configuration serial numbesr from a datapipe
        //! Simultaneously, populates three lists (k1, k2, k3) with serial numbers for the 2pf k-configurations
        //! corresponding to k1, k2, k3
        virtual void pull_kconfig_threepf(datapipe<number>* pipe, const std::vector<unsigned int>& serial_numbers, std::vector<threepf_kconfig>& sample) = 0;

        //! Pull a time sample of a background field from a datapipe
        virtual void pull_background_time_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& t_serials, std::vector<number>& sample) = 0;

        //! Pull a time sample of a twopf component at fixed k-configuration from a datapipe
        virtual void pull_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                            unsigned int k_serial, std::vector<number>& sample, typename datapipe<number>::twopf_type type) = 0;

        //! Pull a sample of a threepf at fixed k-configuration from a datapipe
        virtual void pull_threepf_time_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                              unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of a tensor twopf at fixed k-configuration from a datapipe
        virtual void pull_tensor_twopf_time_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& t_serials,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta twopf at fixed k-configuration from a datapipe
        virtual void pull_zeta_twopf_time_sample(datapipe<number>*, const std::vector<unsigned int>& t_serials,
                                                 unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta threepf at fixed k-configuration from a datapipe
        virtual void pull_zeta_threepf_time_sample(datapipe<number>*, const std::vector<unsigned int>& t_serials,
                                                   unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of the zeta reduced bispectrum at fixed k-configuration from a datapipe
        virtual void pull_zeta_redbsp_time_sample(datapipe<number>*, const std::vector<unsigned int>& t_serials,
                                                  unsigned int k_serial, std::vector<number>& sample) = 0;

        //! Pull a sample of fNL from a datapipe
        virtual void pull_fNL_time_sample(datapipe<number>*, const std::vector<unsigned int>& t_serials,
                                          std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a sample of bispectrum.template from a datapipe
        virtual void pull_BT_time_sample(datapipe<number>*, const std::vector<unsigned int>& t_serials,
                                         std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a sample of template.template from a datapipe
        virtual void pull_TT_time_sample(datapipe<number>*, const std::vector<unsigned int>& t_serials,
                                         std::vector<number>& sample, derived_data::template_type type) = 0;

        //! Pull a kconfig sample of a twopf component at fixed time from a datapipe
        virtual void pull_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                               unsigned int t_serial, std::vector<number>& sample, typename datapipe<number>::twopf_type type) = 0;

        //! Pull a kconfig sample of a threepf at fixed time from a datapipe
        virtual void pull_threepf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                                 unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of a tensor twopf component at fixed time from a datapipe
        virtual void pull_tensor_twopf_kconfig_sample(datapipe<number>* pipe, unsigned int id, const std::vector<unsigned int>& k_serials,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta twopf at fixed time from a datapipe
        virtual void pull_zeta_twopf_kconfig_sample(datapipe<number>*, const std::vector<unsigned int>& k_serials,
                                                    unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta threepf at fixed time from a datapipe
        virtual void pull_zeta_threepf_kconfig_sample(datapipe<number>*, const std::vector<unsigned int>& k_serials,
                                                      unsigned int t_serial, std::vector<number>& sample) = 0;

        //! Pull a kconfig sample of the zeta reduced bispectrum at fixed time from a datapipe
        virtual void pull_zeta_redbsp_kconfig_sample(datapipe<number>*, const std::vector<unsigned int>& k_serials,
                                                     unsigned int t_serial, std::vector<number>& sample) = 0;

        // INTERNAL DATA

      protected:

        //! Capacity available for batchers
        unsigned int                                             batcher_capacity;

        //! Capacity available for data cache
        unsigned int                                             pipe_data_capacity;

        //! Capacity available for zeta cache
        unsigned int                                             pipe_zeta_capacity;

      };


  }   // namespace transport



#endif //__data_manager_H_
