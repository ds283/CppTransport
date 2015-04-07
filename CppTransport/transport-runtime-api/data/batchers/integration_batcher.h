//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __integration_batcher_H_
#define __integration_batcher_H_


#include <vector>
#include <functional>

#include "transport-runtime-api/data/batchers/generic_batcher.h"
#include "transport-runtime-api/data/batchers/integration_items.h"


namespace transport
	{

		// forward declare
		template <typename number> class integration_batcher;


		// writer functions
		template <typename number>
		class integration_writers
			{

		  public:

		    //! Background writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::backg_item>&)> backg_writer;

		    //! Two-point function writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::twopf_item>&)> twopf_writer;

		    //! Tensor two-point function writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::tensor_twopf_item>&)> tensor_twopf_writer;

		    //! Three-point function writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::threepf_item>&)> threepf_writer;

		    //! Per-configuration statistics writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::configuration_statistics>&)> stats_writer;

		    //! Host information writer function
		    typedef std::function<void(integration_batcher<number>*)> host_info_writer;
			};


		template <typename number>
    class integration_batcher: public generic_batcher
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        integration_batcher(unsigned int cap, unsigned int Nf, const std::string& b,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int w, unsigned int g=0, bool s=true);

        virtual ~integration_batcher() = default;


        // ADMINISTRATION

      public:

        //! Return number of fields
        unsigned int get_number_fields() const { return(this->Nfields); }

		    //! Return backend identifier
				const std::string& get_backend() const { return(this->backend); }

        //! Close this batcher -- called at the end of an integration
        virtual void close() override;


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details
        void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching);

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, unsigned int refinement);

        //! Report a failed integration
        void report_integration_failure();

        //! Report an integration which required mesh refinement
        void report_refinement();


        //! Query whether any integrations failed
        bool integrations_failed() const
	        {
            return (this->failures > 0);
	        }


        //! Query how many refinements occurred
        unsigned int number_refinements() const
	        {
            return (this->refinements);
	        }


        //! Prepare for new work assignment
        void begin_assignment();

        //! Tidy up after a work assignment
        void end_assignment();


        // INTEGRATION STATISTICS

      public:

        //! Get aggregate integration time
        boost::timer::nanosecond_type get_integration_time() const
	        {
            return (this->integration_time);
	        }


        //! Get longest integration time
        boost::timer::nanosecond_type get_max_integration_time() const
	        {
            return (this->max_integration_time);
	        }


        //! Get shortest integration time
        boost::timer::nanosecond_type get_min_integration_time() const
	        {
            return (this->min_integration_time);
	        }


        //! Get aggegrate batching time
        boost::timer::nanosecond_type get_batching_time() const
	        {
            return (this->batching_time);
	        }


        //! Get longest batching time
        boost::timer::nanosecond_type get_max_batching_time() const
	        {
            return (this->max_batching_time);
	        }


        //! Get shortest batching time
        boost::timer::nanosecond_type get_min_batching_time() const
	        {
            return (this->min_batching_time);
	        }


        //! Get total number of reported integrations
        unsigned int get_reported_integrations() const
	        {
            return (this->num_integrations);
	        }


        // PUSH BACKGROUND

      public:

        //! Push a background sample
        void push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values);


        // UNBATCH

      public:

        //! Unbatch a given configuration
        virtual void unbatch(unsigned int source_serial) = 0;


        // INTERNAL DATA

      protected:


        // CACHES

        //! Cache of background pushes
        std::vector< typename integration_items<number>::backg_item > backg_batch;

        //! Cache of per-configuration statistics
        std::vector< typename integration_items<number>::configuration_statistics > stats_batch;


        // OTHER INTERNAL DATA

        //! Number of fields associated with this integration
        const unsigned int Nfields;

		    //! Backend name associated with this integration
		    const std::string backend;


        // INTEGRATION EVENT TRACKING

        //! number of integrations which have failed
        unsigned int failures;

        //! number of integrations which required refinement
        unsigned int refinements;


        // INTEGRATION STATISTICS

        //! Are we collecting per-configuration statistics?
        bool collect_statistics;

        //! Number of integrations handled by this batcher
        unsigned int num_integrations;

        //! Aggregate integration time
        boost::timer::nanosecond_type integration_time;

        //! Aggregate batching time
        boost::timer::nanosecond_type batching_time;

        //! Longest integration time
        boost::timer::nanosecond_type max_integration_time;

        //! Shortest integration time
        boost::timer::nanosecond_type min_integration_time;

        //! Longest batching time
        boost::timer::nanosecond_type max_batching_time;

        //! Shortest batching time
        boost::timer::nanosecond_type min_batching_time;

	    };


		template <typename number>
    class twopf_batcher: public integration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:
            typename integration_writers<number>::backg_writer        backg;
            typename integration_writers<number>::twopf_writer        twopf;
            typename integration_writers<number>::tensor_twopf_writer tensor_twopf;
            typename integration_writers<number>::stats_writer        stats;
            typename integration_writers<number>::host_info_writer    host_info;
	        };


      public:

        template <typename handle_type>
        twopf_batcher(unsigned int cap, unsigned int Nf, const std::string& b,
                      const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                      const writer_group& w,
                      generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                      handle_type h, unsigned int wn, unsigned int wg, bool s);

        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

        virtual void unbatch(unsigned int source_serial) override;

      protected:

        virtual size_t storage() const override;

        virtual void flush(generic_batcher::replacement_action action) override;

      protected:

        const writer_group writers;

        std::vector< typename integration_items<number>::twopf_item >        twopf_batch;
        std::vector< typename integration_items<number>::tensor_twopf_item > tensor_twopf_batch;

	    };


		template <typename number>
    class threepf_batcher: public integration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:
            typename integration_writers<number>::backg_writer        backg;
            typename integration_writers<number>::twopf_writer        twopf_re;
            typename integration_writers<number>::twopf_writer        twopf_im;
            typename integration_writers<number>::tensor_twopf_writer tensor_twopf;
            typename integration_writers<number>::threepf_writer      threepf;
            typename integration_writers<number>::stats_writer        stats;
            typename integration_writers<number>::host_info_writer    host_info;
	        };


      public:

        typedef enum { real_twopf, imag_twopf } twopf_type;

        template <typename handle_type>
        threepf_batcher(unsigned int cap, unsigned int Nf, const std::string& b,
                        const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                        const writer_group& w,
                        generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                        handle_type h, unsigned int wn, unsigned int wg, bool s);

        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values, twopf_type t = real_twopf);

        void push_threepf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

        virtual void unbatch(unsigned int source_serial) override;

      protected:

        virtual size_t storage() const override;

        virtual void flush(generic_batcher::replacement_action action) override;

      protected:

        const writer_group writers;

        std::vector< typename integration_items<number>::twopf_item >        twopf_re_batch;
        std::vector< typename integration_items<number>::twopf_item >        twopf_im_batch;
        std::vector< typename integration_items<number>::tensor_twopf_item > tensor_twopf_batch;
        std::vector< typename integration_items<number>::threepf_item >      threepf_batch;

	    };


    // INTEGRATION BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    integration_batcher<number>::integration_batcher(unsigned int cap, unsigned int Nf, const std::string& b,
                                                     const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                     container_dispatch_function d, container_replacement_function r,
                                                     handle_type h, unsigned int w, unsigned int g, bool s)
	    : generic_batcher(cap, cp, lp, d, r, h, w, g),
	      Nfields(Nf),
	      backend(b),
	      num_integrations(0),
	      integration_time(0),
	      max_integration_time(0),
	      min_integration_time(0),
	      batching_time(0),
	      max_batching_time(0),
	      min_batching_time(0),
	      collect_statistics(s),
	      failures(0),
	      refinements(0)
	    {
	    }


    template <typename number>
    void integration_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching)
	    {
        this->integration_time += integration;
        this->batching_time += batching;

        this->num_integrations++;

        if(this->max_integration_time == 0 || integration > this->max_integration_time) this->max_integration_time = integration;
        if(this->min_integration_time == 0 || integration < this->min_integration_time) this->min_integration_time = integration;

        if(this->max_batching_time == 0 || batching > this->max_batching_time) this->max_batching_time = batching;
        if(this->min_batching_time == 0 || batching < this->min_batching_time) this->min_batching_time = batching;

        if(this->flush_due)
	        {
            this->flush_due = false;
            this->flush(action_replace);
	        }
	    }


    template <typename number>
    void integration_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                                 unsigned int kserial, unsigned int refinements)
	    {
        this->report_integration_success(integration, batching);

        typename integration_items<number>::configuration_statistics stats;

        stats.serial      = kserial;
        stats.integration = integration;
        stats.batching    = batching;
        stats.refinements = refinements;

        this->stats_batch.push_back(stats);

        if(this->flush_due)
	        {
            this->flush_due = false;
            this->flush(action_replace);
	        }
	    }


    template <typename number>
    void integration_batcher<number>::push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
	    {
        if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_BACKG);

        typename integration_items<number>::backg_item item;

        item.time_serial   = time_serial;
        item.source_serial = source_serial;
        item.coords        = values;

        this->backg_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void integration_batcher<number>::report_integration_failure()
	    {
        this->failures++;
        this->check_for_flush();

        if(this->flush_due)
	        {
            this->flush_due = false;
            this->flush(action_replace);
	        }
	    }


    template <typename number>
    void integration_batcher<number>::report_refinement()
	    {
        this->refinements++;
	    }


    template <typename number>
    void integration_batcher<number>::begin_assignment()
	    {
        this->num_integrations = 0;
        this->integration_time = 0;
        this->batching_time = 0;
        this->max_integration_time = 0;
        this->min_integration_time = 0;
        this->max_batching_time = 0;
        this->min_batching_time = 0;

        this->failures = 0;
        this->refinements = 0;
	    }


    template <typename number>
    void integration_batcher<number>::end_assignment()
	    {
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "";
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "-- Finished assignment: final integration statistics";
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   processed " << this->num_integrations << " individual integrations in " << format_time(this->integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   mean integration time           = " << format_time(this->integration_time/this->num_integrations);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   longest individual integration  = " << format_time(this->max_integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   shortest individual integration = " << format_time(this->min_integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   total batching time             = " << format_time(this->batching_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   mean batching time              = " << format_time(this->batching_time/this->num_integrations);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   longest individual batch        = " << format_time(this->max_batching_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   shortest individual batch       = " << format_time(this->min_batching_time);

        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "";

        if(this->refinements > 0)
	        {
            BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "-- " << this->refinements << " work items required mesh refinement";
	        }
        if(this->failures > 0)
	        {
            BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "-- " << this->failures << " work items failed to integrate";
	        }
	    }


    template <typename number>
    void integration_batcher<number>::close()
	    {
        this->generic_batcher::close();
	    }


    // TWOPF BATCHER METHODS

    template <typename number>
    template <typename handle_type>
    twopf_batcher<number>::twopf_batcher(unsigned int cap, unsigned int Nf, const std::string& b,
                                         const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                         const writer_group& w,
                                         generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                                         handle_type h, unsigned int wn, unsigned int wg, bool s)
	    : integration_batcher<number>(cap, Nf, b, cp, lp, d, r, h, wn, wg, s),
	      writers(w)
	    {
	    }


    template <typename number>
    void twopf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                           const std::vector<number>& values)
	    {
        if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);

        typename integration_items<number>::twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void twopf_batcher<number>::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                  const std::vector<number>& values)
	    {
        if(values.size() != 4) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TENSOR_TWOPF);

        typename integration_items<number>::tensor_twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->tensor_twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    size_t twopf_batcher<number>::storage() const
	    {
        return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->backg_batch.size()
	        + (3*sizeof(unsigned int) + 4*sizeof(number))*this->tensor_twopf_batch.size()
	        + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*this->twopf_batch.size()
	        + (2*sizeof(unsigned int) + 2*sizeof(boost::timer::nanosecond_type))*this->stats_batch.size());
	    }


    template <typename number>
    void twopf_batcher<number>::flush(generic_batcher::replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.host_info(this);
        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf(this, this->twopf_batch);
        this->writers.tensor_twopf(this, this->tensor_twopf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
        this->backg_batch.clear();
        this->twopf_batch.clear();
        this->tensor_twopf_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
	    }


    template <typename number>
    void twopf_batcher<number>::unbatch(unsigned int source_serial)
	    {
        this->backg_batch.erase(std::remove_if(this->backg_batch.begin(), this->backg_batch.end(),
                                               [ & ](const typename integration_items<number>::backg_item& item) -> bool
                                               {
                                                 return (item.source_serial == source_serial);
                                               }),
                                this->backg_batch.end());

        this->twopf_batch.erase(std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                                               [ & ](const typename integration_items<number>::twopf_item& item) -> bool
                                               {
                                                 return (item.source_serial == source_serial);
                                               }),
                                this->twopf_batch.end());

        this->tensor_twopf_batch.erase(std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                                                      [ & ](const typename integration_items<number>::tensor_twopf_item& item) -> bool
                                                      {
                                                        return (item.source_serial == source_serial);
                                                      }),
                                       this->tensor_twopf_batch.end());
	    }


    // THREEPF BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    threepf_batcher<number>::threepf_batcher(unsigned int cap, unsigned int Nf, const std::string& b,
                                             const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                             const writer_group& w,
                                             generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                                             handle_type h, unsigned int wn, unsigned int wg, bool s)
	    : integration_batcher<number>(cap, Nf, b, cp, lp, d, r, h, wn, wg, s),
	      writers(w)
	    {
	    }


    template <typename number>
    void threepf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                             const std::vector<number>& values, twopf_type t)
	    {
        if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TWOPF);

        typename integration_items<number>::twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        if(t == real_twopf) this->twopf_re_batch.push_back(item);
        else                this->twopf_im_batch.push_back(item);

        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_threepf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                               const std::vector<number>& values)
	    {
        if(values.size() != 2*this->Nfields*2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_THREEPF);

        typename integration_items<number>::threepf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->threepf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                    const std::vector<number>& values)
	    {
        if(values.size() != 4) throw runtime_exception(runtime_exception::STORAGE_ERROR, __CPP_TRANSPORT_NFIELDS_TENSOR_TWOPF);

        typename integration_items<number>::tensor_twopf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->tensor_twopf_batch.push_back(item);
        this->check_for_flush();
	    }


    template <typename number>
    size_t threepf_batcher<number>::storage() const
	    {
        return((sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->backg_batch.size()
	        + (3*sizeof(unsigned int) + 4*sizeof(number))*this->tensor_twopf_batch.size()
	        + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*sizeof(number))*(this->twopf_re_batch.size() + this->twopf_im_batch.size())
	        + (3*sizeof(unsigned int) + 2*this->Nfields*2*this->Nfields*2*this->Nfields*sizeof(number))*this->threepf_batch.size()
	        + (2*sizeof(unsigned int) + 2*sizeof(boost::timer::nanosecond_type))*this->stats_batch.size());
	    }


    template <typename number>
    void threepf_batcher<number>::flush(generic_batcher::replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.host_info(this);
        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf_re(this, this->twopf_re_batch);
        this->writers.twopf_im(this, this->twopf_im_batch);
        this->writers.tensor_twopf(this, this->tensor_twopf_batch);
        this->writers.threepf(this, this->threepf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
        this->backg_batch.clear();
        this->twopf_re_batch.clear();
        this->twopf_im_batch.clear();
        this->tensor_twopf_batch.clear();
        this->threepf_batch.clear();

        // push a message to the master node, indicating that new data is available
        // note that the order of calls to 'dispatcher' and 'replacer' is important
        // because 'dispatcher' needs the current path name, not the one created by
        // 'replacer'
        this->dispatcher(this);

        // close current container, and replace with a new one if required
        this->replacer(this, action);
	    }


    template <typename number>
    void threepf_batcher<number>::unbatch(unsigned int source_serial)
	    {
        this->backg_batch.erase(std::remove_if(this->backg_batch.begin(), this->backg_batch.end(),
                                               [ & ](const typename integration_items<number>::backg_item& item) -> bool
                                               {
                                                 return (item.source_serial == source_serial);
                                               }),
                                this->backg_batch.end());

        this->twopf_re_batch.erase(std::remove_if(this->twopf_re_batch.begin(), this->twopf_re_batch.end(),
                                                  [ & ](const typename integration_items<number>::twopf_item& item) -> bool
                                                  {
                                                    return (item.source_serial == source_serial);
                                                  }),
                                   this->twopf_re_batch.end());

        this->twopf_im_batch.erase(std::remove_if(this->twopf_im_batch.begin(), this->twopf_im_batch.end(),
                                                  [ & ](const typename integration_items<number>::twopf_item& item) -> bool
                                                  {
                                                    return (item.source_serial == source_serial);
                                                  }),
                                   this->twopf_im_batch.end());

        this->tensor_twopf_batch.erase(std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                                                      [ & ](const typename integration_items<number>::tensor_twopf_item& item) -> bool
                                                      {
                                                        return (item.source_serial == source_serial);
                                                      }),
                                       this->tensor_twopf_batch.end());

        this->threepf_batch.erase(std::remove_if(this->threepf_batch.begin(), this->threepf_batch.end(),
                                                 [ & ](const typename integration_items<number>::threepf_item& item) -> bool
                                                 {
                                                   return (item.source_serial == source_serial);
                                                 }),
                                  this->threepf_batch.end());
	    }


	}   // namespace transport


#endif //__integration_batcher_H_
