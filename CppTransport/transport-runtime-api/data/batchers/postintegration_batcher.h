//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015-2016 University of Sussex. All rights reserved.
//


#ifndef CPPTRANSPORT_POSTINTEGRATION_BATCHER_H
#define CPPTRANSPORT_POSTINTEGRATION_BATCHER_H


#include <vector>
#include <set>
#include <functional>

#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"

#include "transport-runtime-api/data/batchers/generic_batcher.h"
#include "transport-runtime-api/data/batchers/postintegration_items.h"


namespace transport
	{

		// forward declare
    template <typename number> class postintegration_batcher;


		// writer functions
		template <typename number>
		class postintegration_writers
			{

		  public:

		    //! Zeta 2pf writer function
		    typedef std::function<void(postintegration_batcher<number>*, const std::vector< typename postintegration_items<number>::zeta_twopf_item >&)> zeta_twopf_writer;

		    //! Zeta 3pf writer function
		    typedef std::function<void(postintegration_batcher<number>*, const std::vector< typename postintegration_items<number>::zeta_threepf_item >&)> zeta_threepf_writer;

		    //! fNL writer function
		    typedef std::function<void(postintegration_batcher<number>*, const std::set< typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator >&, derived_data::template_type)> fNL_writer;

        //! linear gauge xfm writer function
        typedef std::function<void(postintegration_batcher<number>*, const std::vector< typename postintegration_items<number>::gauge_xfm1_item >&)> gauge_xfm1_writer;

        //! quadratic gauge xfm writer function
        typedef std::function<void(postintegration_batcher<number>*, const std::vector< typename postintegration_items<number>::gauge_xfm2_123_item >&)> gauge_xfm2_123_writer;

        //! quadratic gauge xfm writer function
        typedef std::function<void(postintegration_batcher<number>*, const std::vector< typename postintegration_items<number>::gauge_xfm2_213_item >&)> gauge_xfm2_213_writer;

        //! quadratic gauge xfm writer function
        typedef std::function<void(postintegration_batcher<number>*, const std::vector< typename postintegration_items<number>::gauge_xfm2_312_item >&)> gauge_xfm2_312_writer;

			};


    template <typename number>
    class postintegration_batcher: public generic_batcher
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        postintegration_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                                const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                                handle_type h, unsigned int w);

        //! move constructor
        postintegration_batcher(postintegration_batcher<number>&&) = default;

        //! destructor is default
        virtual ~postintegration_batcher() = default;


        // MANAGEMENT

      public:

        //! Report finished block
        void report_finished_item(boost::timer::nanosecond_type time);

        //! Prepare for new work assignment
        void begin_assignment();

        //! Tidy up after a work assignment
        void end_assignment();


        // ADMIN

      public:

        //! Return number of fields
        unsigned int get_number_fields() const { return(this->Nfields); }


        // STATISTICS

      public:

        //! Get aggregate processing time
        boost::timer::nanosecond_type get_processing_time() const { return(this->total_time); }

        //! Get longest item processing time
        boost::timer::nanosecond_type get_max_processing_time() const { return(this->longest_time); }

        //! Get shortest item processing time
        boost::timer::nanosecond_type get_min_processing_time() const { return(this->shortest_time); }

        //! Get number of items processed
        unsigned int get_items_processed() const { return(this->items_processed); }


        // INTERNAL DATA

      protected:

        //! Number of work items processed
        unsigned int items_processed;

        //! number of fields in model
        unsigned int Nfields;

        //! Aggregate processing time
        boost::timer::nanosecond_type total_time;

        //! Longest individual processing time
        boost::timer::nanosecond_type longest_time;

        //! Shortest individual processing time
        boost::timer::nanosecond_type shortest_time;

	    };


		template <typename number>
    class zeta_twopf_batcher: public postintegration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:
            typename postintegration_writers<number>::zeta_twopf_writer twopf;
            typename postintegration_writers<number>::gauge_xfm1_writer gauge_xfm1;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        zeta_twopf_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                           const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                           const writer_group& w, std::unique_ptr<container_dispatch_function> d,
                           std::unique_ptr<container_replace_function> r, handle_type h, unsigned int wn);

        //! move constructor
        zeta_twopf_batcher(zeta_twopf_batcher<number>&&) = default;

        //! destructor is default
        virtual ~zeta_twopf_batcher() = default;


        // BATCH, UNBATCH

      public:

        //! batch twopf value
        void push_twopf(unsigned int time_serial, unsigned int k_serial, number zeta_twopf, unsigned int source_serial=0);

        void push_gauge_xfm1(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm1, unsigned int source_serial=0);

        //! unbatch a k-configuration serial number
        void unbatch(unsigned int source_serial);


        // INTERNAL API

      protected:

        virtual size_t storage() const override;

        virtual void flush(replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! zeta twopf cache
        std::vector< typename postintegration_items<number>::zeta_twopf_item > twopf_batch;

        //! linear gauge-xfm cache
        std::vector< typename postintegration_items<number>::gauge_xfm1_item > gauge_xfm1_batch;

	    };


		template <typename number>
    class zeta_threepf_batcher: public postintegration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:
            typename postintegration_writers<number>::zeta_twopf_writer     twopf;
            typename postintegration_writers<number>::zeta_threepf_writer   threepf;
            typename postintegration_writers<number>::gauge_xfm1_writer     gauge_xfm1;
            typename postintegration_writers<number>::gauge_xfm2_123_writer gauge_xfm2_123;
            typename postintegration_writers<number>::gauge_xfm2_213_writer gauge_xfm2_213;
            typename postintegration_writers<number>::gauge_xfm2_312_writer gauge_xfm2_312;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        zeta_threepf_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                             const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                             const writer_group& w, std::unique_ptr<container_dispatch_function> d,
                             std::unique_ptr<container_replace_function> r, handle_type h, unsigned int wn);

        //! move constructor
        zeta_threepf_batcher(zeta_threepf_batcher<number>&&) = default;

        //! destructor is default
        virtual ~zeta_threepf_batcher() = default;


        // BATCH, UNBATCH

      public:

        void push_twopf(unsigned int time_serial, unsigned int k_serial, number zeta_twopf, unsigned int source_serial=0);

        void push_threepf(unsigned int time_serial, unsigned int k_serial, number zeta_threepf, number redbsp, unsigned int source_serial=0);

        void push_gauge_xfm1(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm1, unsigned int source_serial=0);

        void push_gauge_xfm2_123(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm2, unsigned int source_serial=0);

        void push_gauge_xfm2_213(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm2, unsigned int source_serial=0);

        void push_gauge_xfm2_312(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm2, unsigned int source_serial=0);

        //! unbatch a k-configuration serial number
        void unbatch(unsigned int source_serial);


        // INTERNAL API

      protected:

        virtual size_t storage() const override;

        virtual void flush(replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! zeta twopf cache
        std::vector< typename postintegration_items<number>::zeta_twopf_item > twopf_batch;

        //! zeta threepf cache
        std::vector< typename postintegration_items<number>::zeta_threepf_item > threepf_batch;

        //! linear gauge-xfm cache
        std::vector< typename postintegration_items<number>::gauge_xfm1_item > gauge_xfm1_batch;

        //! quadratic gauge-xfm cache for the 123 permutation
        std::vector< typename postintegration_items<number>::gauge_xfm2_123_item > gauge_xfm2_123_batch;

        //! quadratic gauge-xfm cache for the 213 permutation
        std::vector< typename postintegration_items<number>::gauge_xfm2_213_item > gauge_xfm2_213_batch;

        //! quadratic gauge-xfm cache for the 312 permutation
        std::vector< typename postintegration_items<number>::gauge_xfm2_312_item > gauge_xfm2_312_batch;

	    };


		template <typename number>
    class fNL_batcher: public postintegration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:
            typename postintegration_writers<number>::fNL_writer fNL;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        fNL_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                    const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                    const writer_group& w, std::unique_ptr<container_dispatch_function> d,
                    std::unique_ptr<container_replace_function> r,
                    handle_type h, unsigned int wn, derived_data::template_type t);

        //! move constructor
        fNL_batcher(fNL_batcher<number>&&) = default;

        //! destructor is default
        virtual ~fNL_batcher() = default;


        // BATCH, UNBATCH

      public:

        //! push bispectrum.bispectrum, bispectrum.template, template.template values to the batcher
        void push_fNL(unsigned int time_serial, number BB, number BT, number TT);

        //! set number of k-configurations processed, so can be correctly reported to account for in-flight items
        void set_items_processed(unsigned int N) { this->items_processed = N; }


        // INTERNAL API

      protected:

        virtual size_t storage() const override;

        virtual void flush(replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! fNL cache
        std::set< typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator > fNL_batch;

        //! template being used by this batcher
        derived_data::template_type type;

	    };


    // POSTINTEGRATION BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    postintegration_batcher<number>::postintegration_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                                                             const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                             std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                                                             handle_type h, unsigned int w)
	    : generic_batcher(cap, ckp, cp, lp, std::move(d), std::move(r), h, w),
	      items_processed(0),
	      total_time(0),
	      longest_time(0),
	      shortest_time(0),
        Nfields(m->get_N_fields())
	    {
	    }


    template <typename number>
    void postintegration_batcher<number>::report_finished_item(boost::timer::nanosecond_type time)
	    {
        this->items_processed++;
        this->total_time += time;

        if(this->longest_time == 0 || time > this->longest_time) this->longest_time = time;
        if(this->shortest_time == 0 || time < this->shortest_time) this->shortest_time = time;

        if(this->flush_due)
          {
            this->flush_due = false;
            this->flush(replacement_action::action_replace);
          }
        else if(this->checkpoint_interval > 0 && this->checkpoint_timer.elapsed().wall > this->checkpoint_interval)
          {
            BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "** Lifetime of " << format_time(this->checkpoint_timer.elapsed().wall)
                << " exceeds checkpoint interval " << format_time(this->checkpoint_interval)
                << "; forcing flush";
            this->flush(replacement_action::action_replace);
          }
	    }


    template <typename number>
    void postintegration_batcher<number>::begin_assignment()
	    {
        this->items_processed = 0;
        this->total_time = 0;
        this->longest_time = 0;
        this->shortest_time = 0;
	    }


    template <typename number>
    void postintegration_batcher<number>::end_assignment()
	    {
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "";
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "-- Finished assignment: final statistics";
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   processed " << this->items_processed << " individual work items in " << format_time(this->total_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   mean processing time                = " << format_time(this->total_time/(this->items_processed > 0 ? this->items_processed : 1));
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   longest individual processing time  = " << format_time(this->longest_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   shortest individual processing time = " << format_time(this->shortest_time);

        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "";
	    }


    // ZETA TWOPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    zeta_twopf_batcher<number>::zeta_twopf_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                                                   const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                   const writer_group& w, std::unique_ptr<container_dispatch_function> d,
                                                   std::unique_ptr<container_replace_function> r, handle_type h, unsigned int wn)
	    : postintegration_batcher<number>(cap, ckp, m, cp, lp, std::move(d), std::move(r), h, wn),
	      writers(w)
	    {
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, number zeta_twopf, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = zeta_twopf;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::push_gauge_xfm1(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm1, unsigned int source_serial)
      {
        typename postintegration_items<number>::gauge_xfm1_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = gauge_xfm1;

        this->gauge_xfm1_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    size_t zeta_twopf_batcher<number>::storage() const
	    {
        return((3*sizeof(unsigned int) + 2*sizeof(number))*this->twopf_batch.size()
          + (3*sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->gauge_xfm1_batch.size());
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushing zeta twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.twopf(this, this->twopf_batch);
        this->writers.gauge_xfm1(this, this->gauge_xfm1_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->twopf_batch.clear();
        this->gauge_xfm1_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        (*this->dispatcher)(*this);

        // close current container, and replace with a new one if required
        (*this->replacer)(*this, action);

        // pass flush notification down to generic batcher (eg. resets checkpoint timer)
        this->generic_batcher::flush(action);
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::unbatch(unsigned int source_serial)
      {
        this->twopf_batch.erase(std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                                               UnbatchPredicate<typename postintegration_items<number>::zeta_twopf_item>(source_serial)),
                                this->twopf_batch.end());

        this->gauge_xfm1_batch.erase(std::remove_if(this->gauge_xfm1_batch.begin(), this->gauge_xfm1_batch.end(),
                                                    UnbatchPredicate<typename postintegration_items<number>::gauge_xfm1_item>(source_serial)),
                                     this->gauge_xfm1_batch.end());
      }


    // ZETA THREEPF BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    zeta_threepf_batcher<number>::zeta_threepf_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                                                       const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                       const writer_group& w, std::unique_ptr<container_dispatch_function> d,
                                                       std::unique_ptr<container_replace_function> r, handle_type h, unsigned int wn)
	    : postintegration_batcher<number>(cap, ckp, m, cp, lp, std::move(d), std::move(r), h, wn),
	      writers(w)
	    {
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, number zeta_twopf, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = zeta_twopf;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::push_threepf(unsigned int time_serial, unsigned int k_serial, number zeta_threepf, number redbsp, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_threepf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = zeta_threepf;
        item.redbsp         = redbsp;

        this->threepf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::push_gauge_xfm1(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm1, unsigned int source_serial)
      {
        typename postintegration_items<number>::gauge_xfm1_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = gauge_xfm1;

        this->gauge_xfm1_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void zeta_threepf_batcher<number>::push_gauge_xfm2_123(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm2, unsigned int source_serial)
      {
        typename postintegration_items<number>::gauge_xfm2_123_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = gauge_xfm2;

        this->gauge_xfm2_123_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void zeta_threepf_batcher<number>::push_gauge_xfm2_213(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm2, unsigned int source_serial)
      {
        typename postintegration_items<number>::gauge_xfm2_213_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = gauge_xfm2;

        this->gauge_xfm2_213_batch.push_back(item);
        this->check_for_flush();
      }


    template <typename number>
    void zeta_threepf_batcher<number>::push_gauge_xfm2_312(unsigned int time_serial, unsigned int k_serial, std::vector<number>& gauge_xfm2, unsigned int source_serial)
      {
        typename postintegration_items<number>::gauge_xfm2_312_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = gauge_xfm2;

        this->gauge_xfm2_312_batch.push_back(item);
        this->check_for_flush();
      }

    template <typename number>
    size_t zeta_threepf_batcher<number>::storage() const
	    {
        return((2*sizeof(unsigned int) + sizeof(number))*this->twopf_batch.size()
	        + (3*sizeof(unsigned int) + 3*sizeof(number))*this->threepf_batch.size()
          + (3*sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->gauge_xfm1_batch.size()
          + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->gauge_xfm2_123_batch.size()
          + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->gauge_xfm2_213_batch.size()
          + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->gauge_xfm2_312_batch.size());
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushing zeta threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.twopf(this, this->twopf_batch);
        this->writers.threepf(this, this->threepf_batch);
        this->writers.gauge_xfm1(this, this->gauge_xfm1_batch);
        this->writers.gauge_xfm2_123(this, this->gauge_xfm2_123_batch);
        this->writers.gauge_xfm2_213(this, this->gauge_xfm2_213_batch);
        this->writers.gauge_xfm2_312(this, this->gauge_xfm2_312_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->twopf_batch.clear();
        this->threepf_batch.clear();
        this->gauge_xfm1_batch.clear();
        this->gauge_xfm2_123_batch.clear();
        this->gauge_xfm2_213_batch.clear();
        this->gauge_xfm2_312_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        (*this->dispatcher)(*this);

        // close current container, and replace with a new one if required
        (*this->replacer)(*this, action);

        // pass flush notification down to generic batcher (eg. resets checkpoint timer)
        this->generic_batcher::flush(action);
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::unbatch(unsigned int source_serial)
      {
        this->twopf_batch.erase(std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                                               UnbatchPredicate<typename postintegration_items<number>::zeta_twopf_item>(source_serial)),
                                this->twopf_batch.end());

        this->threepf_batch.erase(std::remove_if(this->threepf_batch.begin(), this->threepf_batch.end(),
                                                 UnbatchPredicate<typename postintegration_items<number>::zeta_threepf_item>(source_serial)),
                                this->threepf_batch.end());

        this->gauge_xfm1_batch.erase(std::remove_if(this->gauge_xfm1_batch.begin(), this->gauge_xfm1_batch.end(),
                                                    UnbatchPredicate<typename postintegration_items<number>::gauge_xfm1_item>(source_serial)),
                                     this->gauge_xfm1_batch.end());

        this->gauge_xfm2_123_batch.erase(std::remove_if(this->gauge_xfm2_123_batch.begin(), this->gauge_xfm2_123_batch.end(),
                                                        UnbatchPredicate<typename postintegration_items<number>::gauge_xfm2_123_item>(source_serial)),
                                         this->gauge_xfm2_123_batch.end());

        this->gauge_xfm2_213_batch.erase(std::remove_if(this->gauge_xfm2_213_batch.begin(), this->gauge_xfm2_213_batch.end(),
                                                        UnbatchPredicate<typename postintegration_items<number>::gauge_xfm2_213_item>(source_serial)),
                                         this->gauge_xfm2_213_batch.end());

        this->gauge_xfm2_312_batch.erase(std::remove_if(this->gauge_xfm2_312_batch.begin(), this->gauge_xfm2_312_batch.end(),
                                                        UnbatchPredicate<typename postintegration_items<number>::gauge_xfm2_312_item>(source_serial)),
                                         this->gauge_xfm2_312_batch.end());
      }


    // FNL BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    fNL_batcher<number>::fNL_batcher(unsigned int cap, unsigned int ckp, model<number>* m,
                                     const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                     const writer_group& w, std::unique_ptr<container_dispatch_function> d,
                                     std::unique_ptr<container_replace_function> r,
                                     handle_type h, unsigned int wn, derived_data::template_type t)
	    : postintegration_batcher<number>(cap, ckp, m, cp, lp, std::move(d), std::move(r), h, wn),
	      writers(w),
	      type(t)
	    {
	    }


    template <typename number>
    void fNL_batcher<number>::push_fNL(unsigned int time_serial, number BB, number BT, number TT)
	    {
        typename postintegration_items<number>::fNL_item item;

        item.time_serial = time_serial;
        item.BB          = BB;
        item.BT          = BT;
        item.TT          = TT;

		    // if an existing item with this serial number exists, we want to accumulate the BT and TT values
		    // we then have to remove the existing record before inserting an updated one -- elements in a std::set
		    // are always const and therefore read-only
        typename std::set<  typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator >::iterator t;
		    t = this->fNL_batch.find(item); // searching in a set is fairly efficient, O(log N)

		    if(t != this->fNL_batch.end())
			    {
            item.BB += t->BB;
				    item.BT += t->BT;
				    item.TT += t->TT;
				    this->fNL_batch.erase(t);
			    }

        this->fNL_batch.insert(item);
        this->check_for_flush();
	    }


    template <typename number>
    size_t fNL_batcher<number>::storage() const
	    {
        return((2*sizeof(unsigned int) + sizeof(number))*this->fNL_batch.size());
	    }


    template <typename number>
    void fNL_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushing " <<
            template_type_to_string(this->type) << " batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.fNL(this, this->fNL_batch, this->type);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->fNL_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        (*this->dispatcher)(*this);

        // close current container, and replace with a new one if required
        (*this->replacer)(*this, action);

        // pass flush notification down to generic batcher (eg. resets checkpoint timer)
        this->generic_batcher::flush(action);
	    }

	}   // namespace transport


#endif //CPPTRANSPORT_POSTINTEGRATION_BATCHER_H
