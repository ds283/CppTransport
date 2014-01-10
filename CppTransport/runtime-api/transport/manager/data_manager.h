//
// Created by David Seery on 08/01/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __data_manager_H_
#define __data_manager_H_


#include <set>

#include "transport/tasks/task.h"
#include "transport/manager/repository.h"


namespace transport
  {


    template <typename number>
    class data_manager
      {

      public:
        // data structures for storing individual sample points from each integration
        class backg_item
          {
            unsigned int        time_serial;
            std::vector<number> coords;
          };

        class twopf_item
          {
            unsigned int        time_serial;
            unsigned int        kconfig_serial;
            std::vector<number> elements;
          };

        class threepf_item
          {
            unsigned int        time_serial;
            unsigned int        kconfig_serial;
            std::vector<number> elements;
          };

        // writer functions, used by the compute backends to store the output of each integration
        // in a temporary container

        // background writer
        typedef std::function<void(const std::vector<backg_item>&)> backg_writer;

        // twopf writer
        typedef std::function<void(const std::vector<twopf_item>&)> twopf_writer;

        // threepf writer requires: threepf writer
        typedef std::function<void(const std::vector<threepf_item>&)> threepf_writer;

        // group writers together in batches for twopf and threepf integrations
        class twopf_writer_group
          {
          public:
            backg_writer backg;
            twopf_writer twopf;
          };

        class threepf_writer_group
          {
          public:
            backg_writer   backg;
            twopf_writer   twopf_re;
            twopf_writer   twopf_im;
            threepf_writer threepf;
          };

        class generic_batcher
          {
          public:
            template <typename handle_type>
            generic_batcher(unsigned int cp, unsigned int Nf, const boost::filesystem::path& cn, handle_type h)
              : capacity(cp), Nfields(Nf), container_path(cn), num_backg(0), manager_handle(static_cast<void*>(h))
              {
              }

            size_t get_capacity() const { return(this->capacity); }

            const boost::filesystem::path& get_container_path() const { return(this->container_path); }

            template <typename handle_type>
            void get_manager_handle(handle_type* h) { *h = static_cast<handle_type>(this->manager_handle); }

            void push_backg(unsigned int time_serial, const std::vector<number>& values)
              {
                if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_BACKG);
                backg_item item;
                item.time_serial = time_serial;
                item.coords      = values;

                this->backg_batch.push_back(item);
                if(this->storage() > this->capacity) this->flush();
              }

          protected:
            virtual size_t storage() const = 0;

            virtual void flush() = 0;

          protected:
            const unsigned int            capacity;
            const boost::filesystem::path container_path;

            const unsigned int            Nfields;

            unsigned int                  num_backg;

            std::vector<backg_item>       backg_batch;

            const void*                   manager_handle;
          };

        class twopf_batcher: public generic_batcher
          {
          public:
            template <typename handle_type>
            twopf_batcher(unsigned int cp, unsigned int Nf, const boost::filesystem::path& cn, const twopf_writer_group& w, handle_type h)
              : generic_batcher(cp, Nf, cn, h), writers(w), num_twopf(0)
              {
              }

            void push_twopf(unsigned int time_serial, unsigned int k_serial, const std::vector<number>& values)
              {
                if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);
                twopf_item item;
                item.time_serial    = time_serial;
                item.kconfig_serial = k_serial;
                item.elements       = values;

                this->twopf_batch.push_back(item);
                if(this->storage() > this->capacity) this->flush();
              }

          protected:
            size_t storage() const { return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*num_backg
                                            + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*num_twopf); }

            void flush()
              {
                this->writers.backg(this->backg_batch);
                this->writers.twopf(this->twopf_batch);

                this->backg_batch.clear();
                this->twopf_batch.clear();
                num_backg = num_twopf = 0;
              }

          private:
            const twopf_writer_group writers;

            unsigned int             num_twopf;

            std::vector<twopf_item>  twopf_batch;
          };


        class threepf_batcher: public generic_batcher;
          {
          public:

            typedef enum { real_twopf, imag_twopf } twopf_type;

            template <handle_type>
            threepf_batcher(unsigned int cp, unsigned int Nf, const boost::filesystem::path& cn, const threepf_writer_group& w, handle_type h)
              : generic_batcher(cp, Nf, cn, h), writers(w), num_twopf_re(0), num_twopf_im(0), num_threepf(0)
              {
              }

            void push_twopf(unsigned int time_serial, unsigned int k_serial, const std::vector<number>& values, twopf_type type=real_twopf)
              {
                if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);
                twopf_item item;
                item.time_serial    = time_serial;
                item.kconfig_serial = k_serial;
                item.elements       = values;

                if(type == real_twopf) this->twopf_re_batch.push_back(item);
                else                   this->twopf_im_batch.push_back(item);

                if(this->storage() > this->capacity) this->flush();
              }

            void push_threepf(unsigned int time_serial, unsigned int k_serial, const std::vector<number>& values)
              {
                if(values.size() != 2*this->Nfields*2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_THREEPF);
                threepf_item item;
                item.time_serial    = time_serial;
                item.kconfig_serial = k_serial;
                item.elements       = values;

                this->threepf_batch.push_back(item);
                if(this->storage() > this->capacity) this->flush();
              }

          protected:
            size_t storage() const { return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*num_backg
                                            + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*(num_twopf_re + num_twopf_im)
                                            + (2*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*2*this->Nfields*sizeof(number))*num_threepf); }

            void flush()
              {
                this->writers.backg(this->backg_batch);
                this->writers.twopf_re(this->twopf_re_batch);
                this->writers.twopf_im(this->twopf_im_batch);
                this->writers.threepf(this->threepf_batch);

                this->backg_batch.clear();
                this->twopf_re_batch.clear();
                this->twopf_im_batch.clear();
                this->threepf_batch.clear();
                num_backg = num_twopf_re = num_twopf_im = num_threepf = 0;
              }

          private:
            const threepf_writer_group writers;

            unsigned int               num_twopf_re;
            unsigned int               num_twopf_im;
            unsigned int               num_threepf;

            std::vector<twopf_item>    twopf_re_batch;
            std::vector<twopf_item>    twopf_im_batch;
            std::vector<threepf_item>  threepf_batch;
          };


        // CONSTRUCTOR, DESTRUCTOR

      public:
        //! Create a data_manager instance
        data_manager(unsigned int cp)
          : capacity(cp)
          {
          }

        //! Destroy the data_manager instance. In practice this would always be delegated to an implementation class
        virtual ~data_manager()
          {
          }


        // INTERFACE -- CONTAINER HANDLING

      public:
        //! Create a new container. Never overwrites existing data; if the container already exists, an exception is thrown
        virtual void create_container(repository<number>* repo, typename repository<number>::integration_container& ctr) = 0;

        //! Open an existing container
        virtual void open_container(repository<number>* repo, typename repository<number>::integration_container& ctr) = 0;

        //! Close an open container
        virtual void close_container(typename repository<number>::integration_container& ctr) = 0;


        // INTERFACE -- WRITE INDEX TABLES

      public:
        //! Create tables needed for a twopf container
        virtual void create_tables(typename repository<number>::integration_container& ctr, twopf_task<number>* tk,
                                   unsigned int Nfields) = 0;

        //! Create tables needed for a threepf container
        virtual void create_tables(typename repository<number>::integration_container& ctr, threepf_task<number>* tk,
                                   unsigned int Nfields) = 0;


        // INTERFACE -- TASK FILES

      public:
        //! Create a list of task assignments, over a number of devices, from a work queue of twopf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<twopf_kconfig>& queue) = 0;

        //! Create a list of task assignments, over a number of devices, from a work queue of threepf_kconfig-s
        virtual void create_taskfile(typename repository<number>::integration_container& ctr, const work_queue<threepf_kconfig>& queue) = 0;

        //! Read a list of task assignments for a particular worker
        virtual std::set<unsigned int> read_taskfile(const boost::filesystem::path& taskfile, unsigned int worker) = 0;


        // INTERFACE -- TEMPORARY CONTAINERS

      public:
        //! Create a temporary container for twopf data. Returns a batcher which can be used for writing to the container.
        virtual twopf_batcher create_temp_twopf_container(const boost::filesystem::path& tempdir, unsigned int worker, unsigned int Nfields) = 0;

        //! Create a temporary container for threepf data. Returns a batcher which can be used for writing to the container.
        virtual threepf_batcher create_temp_threepf_container(const boost::filesystem::path& tempdir, unsigned int worker, unsigned int Nfields) = 0;

        //! Close a temporary container. Returns path to the container.
        virtual void close_temporary_container(const generic_batcher& handle) = 0;


        // INTERNAL DATA

      protected:
        //! Maximum memory available to each worker process
        unsigned int capacity;
      };

  }   // namespace transport



#endif //__data_manager_H_
