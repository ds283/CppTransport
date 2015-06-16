//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __postintegration_batcher_H_
#define __postintegration_batcher_H_


#include <vector>
#include <set>
#include <functional>

#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"

#include "transport-runtime-api/data/batchers/generic_batcher.h"
#include "transport-runtime-api/data/batchers/postintegration_items.h"


namespace transport
	{

		// forward declare
    class postintegration_batcher;


		// writer functions
		template <typename number>
		class postintegration_writers
			{

		  public:

		    //! Zeta 2pf writer function
		    typedef std::function<void(postintegration_batcher*, const std::vector< typename postintegration_items<number>::zeta_twopf_item >&)> zeta_twopf_writer;

		    //! Zeta 3pf writer function
		    typedef std::function<void(postintegration_batcher*, const std::vector< typename postintegration_items<number>::zeta_threepf_item >&)> zeta_threepf_writer;

		    //! Zeta reduced bispectrum writer function
		    typedef std::function<void(postintegration_batcher*, const std::vector< typename postintegration_items<number>::zeta_redbsp_item >&)> zeta_redbsp_writer;

		    //! fNL writer function
		    typedef std::function<void(postintegration_batcher*, const std::set< typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator >&, derived_data::template_type)> fNL_writer;

			};


    class postintegration_batcher: public generic_batcher
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        postintegration_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                container_dispatch_function d, container_replacement_function r,
                                handle_type h, unsigned int w);

        virtual ~postintegration_batcher() = default;


        // MANAGEMENT

      public:

        //! Report finished block
        void report_finished_item(boost::timer::nanosecond_type time);

        //! Prepare for new work assignment
        void begin_assignment();

        //! Tidy up after a work assignment
        void end_assignment();


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

      private:

        //! Number of work items processed
        unsigned int items_processed;

        //! Aggregate processing time
        boost::timer::nanosecond_type total_time;

        //! Longest individual processing time
        boost::timer::nanosecond_type longest_time;

        //! Shortest individual processing time
        boost::timer::nanosecond_type shortest_time;

	    };


		template <typename number>
    class zeta_twopf_batcher: public postintegration_batcher
	    {

      public:

        class writer_group
	        {
          public:
            typename postintegration_writers<number>::zeta_twopf_writer twopf;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        zeta_twopf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                           const writer_group& w, generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                           handle_type h, unsigned int wn);


        // BATCH, UNBATCH

      public:

        //! batch twopf value
        void push_twopf(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial=0);

        //! unbatch a k-configuration serial number
        void unbatch(unsigned int source_serial);


        // INTERNAL API

      protected:

        virtual size_t storage() const override;

        virtual void flush(generic_batcher::replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! zeta twopf cache
        std::vector< typename postintegration_items<number>::zeta_twopf_item > twopf_batch;

	    };


		template <typename number>
    class zeta_threepf_batcher: public postintegration_batcher
	    {

      public:

        class writer_group
	        {
          public:
            typename postintegration_writers<number>::zeta_twopf_writer   twopf;
            typename postintegration_writers<number>::zeta_threepf_writer threepf;
            typename postintegration_writers<number>::zeta_redbsp_writer  redbsp;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        zeta_threepf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                             const writer_group& w, container_dispatch_function d, container_replacement_function r,
                             handle_type h, unsigned int wn);


        // BATCH, UNBATCH

      public:

        void push_twopf(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial=0);

        void push_threepf(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial=0);

        void push_reduced_bispectrum(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial=0);

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

        //! zeta reduced bispectrum cache
        std::vector< typename postintegration_items<number>::zeta_redbsp_item > redbsp_batch;

	    };


		template <typename number>
    class fNL_batcher: public postintegration_batcher
	    {

      public:

        class writer_group
	        {
          public:
            typename postintegration_writers<number>::fNL_writer fNL;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        fNL_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                    const writer_group& w, container_dispatch_function d, container_replacement_function r,
                    handle_type h, unsigned int wn, derived_data::template_type t);


        // BATCH, UNBATCH

      public:

        void push_fNL(unsigned int time_serial, number BT, number TT);


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


    template <typename handle_type>
    postintegration_batcher::postintegration_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                     container_dispatch_function d, container_replacement_function r,
                                                     handle_type h, unsigned int w)
	    : generic_batcher(cap, cp, lp, d, r, h, w),
	      items_processed(0),
	      total_time(0),
	      longest_time(0),
	      shortest_time(0)
	    {
	    }


    void postintegration_batcher::report_finished_item(boost::timer::nanosecond_type time)
	    {
        this->items_processed++;
        this->total_time += time;

        if(this->longest_time == 0 || time > this->longest_time) this->longest_time = time;
        if(this->shortest_time == 0 || time < this->shortest_time) this->shortest_time = time;

        if(this->flush_due)
          {
            this->flush_due = false;
            this->flush(action_replace);
          }
	    }


    void postintegration_batcher::begin_assignment()
	    {
        this->items_processed = 0;
        this->total_time = 0;
        this->longest_time = 0;
        this->shortest_time = 0;
	    }


    void postintegration_batcher::end_assignment()
	    {
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "";
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "-- Finished assignment: final statistics";
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   processed " << this->items_processed << " individual work items in " << format_time(this->total_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   mean processing time                = " << format_time(this->total_time/(this->items_processed > 0 ? this->items_processed : 1));
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   longest individual processing time  = " << format_time(this->longest_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   shortest individual processing time = " << format_time(this->shortest_time);

        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "";
	    }


    // ZETA TWOPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    zeta_twopf_batcher<number>::zeta_twopf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                   const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                                   handle_type h, unsigned int wn)
	    : postintegration_batcher(cap, cp, lp, d, r, h, wn),
	      writers(w)
	    {
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = value;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    size_t zeta_twopf_batcher<number>::storage() const
	    {
        return((3*sizeof(unsigned int) + sizeof(number))*this->twopf_batch.size());
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing zeta twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.twopf(this, this->twopf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->twopf_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
	    }


    template <typename number>
    void zeta_twopf_batcher<number>::unbatch(unsigned int source_serial)
      {
        this->twopf_batch.erase(std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                                               UnbatchPredicate<typename postintegration_items<number>::zeta_twopf_item>(source_serial)),
                                this->twopf_batch.end());
      }


    // ZETA THREEPF BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    zeta_threepf_batcher<number>::zeta_threepf_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                       const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                                       handle_type h, unsigned int wn)
	    : postintegration_batcher(cap, cp, lp, d, r, h, wn),
	      writers(w)
	    {
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = value;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::push_threepf(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_threepf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = value;

        this->threepf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::push_reduced_bispectrum(unsigned int time_serial, unsigned int k_serial, number value, unsigned int source_serial)
	    {
        typename postintegration_items<number>::zeta_redbsp_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.value          = value;

        this->redbsp_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    size_t zeta_threepf_batcher<number>::storage() const
	    {
        return((2*sizeof(unsigned int) + sizeof(number))*this->twopf_batch.size()
	        + (3*sizeof(unsigned int) + sizeof(number))*this->threepf_batch.size()
	        + (3*sizeof(unsigned int) + sizeof(number))*this->redbsp_batch.size());
	    }


    template <typename number>
    void zeta_threepf_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing zeta threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.twopf(this, this->twopf_batch);
        this->writers.threepf(this, this->threepf_batch);
        this->writers.redbsp(this, this->redbsp_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->twopf_batch.clear();
        this->threepf_batch.clear();
        this->redbsp_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
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

        this->redbsp_batch.erase(std::remove_if(this->redbsp_batch.begin(), this->redbsp_batch.end(),
                                                UnbatchPredicate<typename postintegration_items<number>::zeta_redbsp_item>(source_serial)),
                                this->redbsp_batch.end());
      }


    // FNL BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    fNL_batcher<number>::fNL_batcher(unsigned int cap, const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                     const writer_group& w, container_dispatch_function d, container_replacement_function r,
                                     handle_type h, unsigned int wn, derived_data::template_type t)
	    : postintegration_batcher(cap, cp, lp, d, r, h, wn),
	      writers(w),
	      type(t)
	    {
	    }


    template <typename number>
    void fNL_batcher<number>::push_fNL(unsigned int time_serial, number BT, number TT)
	    {
        typename postintegration_items<number>::fNL_item item;

        item.time_serial = time_serial;
        item.BT          = BT;
        item.TT          = TT;

		    // if an existing item with this serial number exists, we want to accumulate the BT and TT values
		    // we then have to remove the existing record before inserting an updated one -- elements in a std::set
		    // are always const and therefore read-only
        typename std::set<  typename postintegration_items<number>::fNL_item, typename postintegration_items<number>::fNL_item_comparator >::iterator t;
		    t = this->fNL_batch.find(item); // searching in a set is fairly efficient, O(log N)

		    if(t != this->fNL_batch.end())
			    {
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
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushing " << template_name(this->type) << " batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.fNL(this, this->fNL_batch, this->type);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->fNL_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
	    }

	}   // namespace transport


#endif //__postintegration_batcher_H_
