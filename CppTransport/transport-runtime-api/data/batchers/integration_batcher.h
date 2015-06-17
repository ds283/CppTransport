//
// Created by David Seery on 26/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __integration_batcher_H_
#define __integration_batcher_H_


#include <vector>
#include <functional>

#include "transport-runtime-api/data/batchers/generic_batcher.h"
#include "transport-runtime-api/data/batchers/postintegration_batcher.h"
#include "transport-runtime-api/data/batchers/integration_items.h"
#include "transport-runtime-api/data/batchers/zeta_compute.h"

#include "transport-runtime-api/models/model_forward_declare.h"
#include "transport-runtime-api/tasks/tasks_forward_declare.h"
#include "transport-runtime-api/tasks/task_configurations.h"


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
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::twopf_re_item>&)> twopf_re_writer;

		    //! Two-point function writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::twopf_im_item>&)> twopf_im_writer;

		    //! Tensor two-point function writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::tensor_twopf_item>&)> tensor_twopf_writer;

		    //! Three-point function writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::threepf_item>&)> threepf_writer;

		    //! Per-configuration statistics writer function
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::configuration_statistics>&)> stats_writer;

				//! Per-configuration initial conditions writer function
				typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::ics_item>&)> ics_writer;

		    //! Per-configuration initial conditions writer function - kt variant
		    typedef std::function<void(integration_batcher<number>*, const std::vector<typename integration_items<number>::ics_kt_item>&)> ics_kt_writer;

		    //! Host information writer function
		    typedef std::function<void(integration_batcher<number>*)> host_info_writer;
			};


		template <typename number>
    class integration_batcher: public generic_batcher
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        integration_batcher(unsigned int cap, model<number>* m,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            container_dispatch_function d, container_replacement_function r,
                            handle_type h, unsigned int w, unsigned int g=0, bool ics=false);

        virtual ~integration_batcher() = default;


        // ADMINISTRATION

      public:

        //! Return number of fields
        unsigned int get_number_fields() const { return(this->Nfields); }

		    //! Return backend identifier
				const std::string& get_backend() const { return(this->mdl->get_backend()); }

        //! Return stepper identifier - background evolution
        const std::string& get_back_stepper() const { return(this->mdl->get_back_stepper()); }

        //! Return stepper identifier - perturbations evolution
        const std::string& get_pert_stepper() const { return(this->mdl->get_pert_stepper()); }

        //! Close this batcher -- called at the end of an integration
        virtual void close() override;


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching);

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, size_t steps, unsigned int refinement);

        //! Report a failed integration
        virtual void report_integration_failure();

        //! Report a failed integration for a specific serial number
        virtual void report_integration_failure(unsigned int kserial);

        //! Report an integration which required mesh refinement
        void report_refinement();


        //! Query whether any integrations failed
        bool is_failed() const { return (this->failures > 0); }

        //! Query how many refinements occurred
        unsigned int get_reported_refinements() const { return(this->refinements); }

        //! Query number of failed integration reports
        unsigned int get_reported_failures() const { return(this->failures); }

        //! Query a list of failed serial numbers (not all backends may support this)
        const std::list< unsigned int>& get_failed_serials() const { return(this->failed_serials); }


        //! Prepare for new work assignment
        void begin_assignment();

        //! Tidy up after a work assignment
        void end_assignment();


		    // PER-CONFIGURATION STATISTICS AND AUXILIARY INFORMATION

      public:

		    //! are we collecting per-configuration statistics?
		    bool is_collecting_statistics() const { return(this->collect_statistics); }

		    //! are we collecting initial conditions?
		    bool is_collecting_initial_conditions() const { return(this->collect_initial_conditions); }


        // GLOBAL BATCHER STATISTICS

      public:

        //! Get aggregate integration time
        boost::timer::nanosecond_type get_integration_time() const { return (this->integration_time); }

        //! Get longest integration time
        boost::timer::nanosecond_type get_max_integration_time() const { return (this->max_integration_time); }
        
        //! Get shortest integration time
        boost::timer::nanosecond_type get_min_integration_time() const  { return (this->min_integration_time); }

        //! Get aggegrate batching time
        boost::timer::nanosecond_type get_batching_time() const { return (this->batching_time); }

        //! Get longest batching time
        boost::timer::nanosecond_type get_max_batching_time() const { return (this->max_batching_time); }

        //! Get shortest batching time
        boost::timer::nanosecond_type get_min_batching_time() const { return (this->min_batching_time); }

        //! Get total number of reported integrations
        unsigned int get_reported_integrations() const { return (this->num_integrations); }


        // PUSH BACKGROUND

      public:

        //! Push a background sample
        void push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values);

		    //! Push a set of initial conditions
        void push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values);


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

        //! initial conditions cache
        std::vector< typename integration_items<number>::ics_item > ics_batch;


        // OTHER INTERNAL DATA

		    //! pointer to parent model
        model<number>* mdl;

        //! Cache number of fields associated with this integration
        const unsigned int Nfields;


        // INTEGRATION EVENT TRACKING

        //! number of integrations which have failed
        unsigned int failures;

        //! list of failed k-configuration serial numbers (not all backends may support tracking this)
        std::list< unsigned int > failed_serials;

        //! number of integrations which required refinement
        unsigned int refinements;


        // INTEGRATION STATISTICS

        //! Are we collecting per-configuration statistics?
        bool collect_statistics;

		    //! Are we collecting initial conditions data?
		    bool collect_initial_conditions;

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
            typename integration_writers<number>::twopf_re_writer     twopf;
            typename integration_writers<number>::tensor_twopf_writer tensor_twopf;
            typename integration_writers<number>::stats_writer        stats;
            typename integration_writers<number>::ics_writer          ics;
            typename integration_writers<number>::host_info_writer    host_info;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        twopf_batcher(unsigned int cap, model<number>* m, twopf_task<number>* tk,
                      const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                      const writer_group& w,
                      generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                      handle_type h, unsigned int wn, unsigned int wg);


        // ADMINISTRATION

      public:

        //! Override integration_batcher close() to push notification to a paired batcher, if one is present
        virtual void close() override { this->integration_batcher<number>::close(); if(this->paired_batcher != nullptr) this->paired_batcher->close(); this->paired_batcher = nullptr; }


        // BATCH, UNBATCH

      public:

        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values, const std::vector<number>& backg);

        void push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& twopf, const std::vector<number>& backg);

        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

        virtual void unbatch(unsigned int source_serial) override;


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching) override;

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, size_t steps, unsigned int refinement) override;

        //! Report a failed integration
        virtual void report_integration_failure() override;

        //! Report a failed integration for a specific serial number
        virtual void report_integration_failure(unsigned int kserial) override;


        // FLUSH INTERFACE

      public:

        //! Override generic batcher set_flush_mode() to push flush settings to a paired batcher, if one is present; then unpair
        virtual void set_flush_mode(generic_batcher::flush_mode f) override { this->generic_batcher::set_flush_mode(f); if(this->paired_batcher != nullptr) this->paired_batcher->set_flush_mode(f); }


        // PAIR

      public:

        //! Pair with a zeta batcher. Remember to push our flush setting to it.
        void pair(zeta_twopf_batcher<number>* batcher) { assert(batcher != nullptr); this->paired_batcher = batcher; this->paired_batcher->set_flush_mode(this->get_flush_mode()); }


        // INTERNAL API

      protected:

        virtual size_t storage() const override;

        virtual void flush(generic_batcher::replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! twopf cache
        std::vector< typename integration_items<number>::twopf_re_item >     twopf_batch;

        //! tensor twopf cache
        std::vector< typename integration_items<number>::tensor_twopf_item > tensor_twopf_batch;


        // PAIRING

        //! task associated with this batcher (for computing gauge transforms, etc.)
        twopf_task<number>* parent_task;

        //! Paired zeta batcher, if present
        zeta_twopf_batcher<number>* paired_batcher;


        // ZETA COMPUTATION AGENT

        //! compute delegate
        zeta_compute<number> zeta_agent;

      };


		template <typename number>
    class threepf_batcher: public integration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:
            typename integration_writers<number>::backg_writer        backg;
            typename integration_writers<number>::twopf_re_writer     twopf_re;
            typename integration_writers<number>::twopf_im_writer     twopf_im;
            typename integration_writers<number>::tensor_twopf_writer tensor_twopf;
            typename integration_writers<number>::threepf_writer      threepf;
            typename integration_writers<number>::stats_writer        stats;
		        typename integration_writers<number>::ics_writer          ics;
		        typename integration_writers<number>::ics_kt_writer       kt_ics;
            typename integration_writers<number>::host_info_writer    host_info;
	        };

        typedef enum { real_twopf, imag_twopf } twopf_type;


        // CONSTRUCTOR, DESTRUCTOR

      public:

        template <typename handle_type>
        threepf_batcher(unsigned int cap, model<number>* m, threepf_task<number>* tk,
                        const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                        const writer_group& w,
                        generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                        handle_type h, unsigned int wn, unsigned int wg);


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching) override;

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, size_t steps, unsigned int refinement) override;

        //! Report a failed integration
        virtual void report_integration_failure() override;

        //! Report a failed integration for a specific serial number
        virtual void report_integration_failure(unsigned int kserial) override;



        // ADMINISTRATION

      public:

        //! Override integration_batcher close() to push notification to a paired batcher, if one is present; then unpair
        virtual void close() override { this->integration_batcher<number>::close(); if(this->paired_batcher != nullptr) this->paired_batcher->close(); this->paired_batcher = nullptr; }


        // BATCH, UNBATCH

      public:

        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values, const std::vector<number>& backg, twopf_type t = real_twopf);

        void push_threepf(unsigned int time_serial, double t,
                          const threepf_kconfig& kconfig, unsigned int source_serial, const std::vector<number>& values,
                          const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                          const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                          const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im, const std::vector<number>& bg);

        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& values);

        void push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial, const std::vector<number>& twopf, const std::vector<number>& backg);

        void push_paired_threepf(unsigned int time_serial, double t,
                                 const threepf_kconfig& kconfig, unsigned int source_serial, const std::vector<number>& values,
                                 const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                                 const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                                 const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im, const std::vector<number>& bg);

		    void push_kt_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values);

        virtual void unbatch(unsigned int source_serial) override;


        // FLUSH INTERFACE

      public:

        //! Override generic batcher set_flush_mode() to push flush settings to a paired batcher, if one is present
        virtual void set_flush_mode(generic_batcher::flush_mode f) override { this->generic_batcher::set_flush_mode(f); if(this->paired_batcher != nullptr) this->paired_batcher->set_flush_mode(f); }


        // PAIR

      public:

        //! Pair with a zeta batcher. Remember to push our flush setting to it.
        void pair(zeta_threepf_batcher<number>* batcher) { assert(batcher != nullptr); this->paired_batcher = batcher; this->paired_batcher->set_flush_mode(this->get_flush_mode()); }


        // INTERNAL API

      protected:

        virtual size_t storage() const override;

        virtual void flush(generic_batcher::replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! real twopf cache
        std::vector< typename integration_items<number>::twopf_re_item >     twopf_re_batch;

        //! imaginary twopf cache
        std::vector< typename integration_items<number>::twopf_im_item >     twopf_im_batch;

        //! tensor twopf cache
        std::vector< typename integration_items<number>::tensor_twopf_item > tensor_twopf_batch;

        //! threeof cache
        std::vector< typename integration_items<number>::threepf_item >      threepf_batch;

		    //! k_t initial conditions cache
		    std::vector< typename integration_items<number>::ics_kt_item >       kt_ics_batch;


        // PAIRING

        //! threepf-task associated with this batcher (for computing gauge transfroms etc.)
        threepf_task<number>* parent_task;

        //! Paired zeta batcher, if present
        zeta_threepf_batcher<number>* paired_batcher;


        // ZETA COMPUTATION AGENT

        //! compute delegate
        zeta_compute<number> zeta_agent;

	    };


    // INTEGRATION BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    integration_batcher<number>::integration_batcher(unsigned int cap, model<number>* m,
                                                     const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                     container_dispatch_function d, container_replacement_function r,
                                                     handle_type h, unsigned int w, unsigned int g, bool ics)
	    : generic_batcher(cap, cp, lp, d, r, h, w, g),
        Nfields(m->get_N_fields()),
        mdl(m),
	      num_integrations(0),
	      integration_time(0),
	      max_integration_time(0),
	      min_integration_time(0),
	      batching_time(0),
	      max_batching_time(0),
	      min_batching_time(0),
	      collect_statistics(m->supports_per_configuration_statistics()),
	      collect_initial_conditions(ics),
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
                                                                 unsigned int kserial, size_t steps, unsigned int refinements)
	    {
        this->report_integration_success(integration, batching);

		    if(this->collect_statistics)
			    {
		        typename integration_items<number>::configuration_statistics stats;

		        stats.serial      = kserial;
		        stats.integration = integration;
		        stats.batching    = batching;
		        stats.refinements = refinements;
		        stats.steps       = steps;

		        this->stats_batch.push_back(stats);
			    }

        if(this->flush_due)
	        {
            this->flush_due = false;
            this->flush(action_replace);
	        }
	    }


    template <typename number>
    void integration_batcher<number>::push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
	    {
        if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_BACKG);

        typename integration_items<number>::backg_item item;

        item.time_serial   = time_serial;
        item.source_serial = source_serial;
        item.coords        = values;

        this->backg_batch.push_back(item);
        this->check_for_flush();
	    }


		template <typename number>
		void integration_batcher<number>::push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
			{
		    if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_BACKG);

				if(this->collect_initial_conditions)
					{
						typename integration_items<number>::ics_item ics;

				    ics.source_serial = k_serial;
						ics.texit         = t_exit;
				    ics.coords        = values;

						this->ics_batch.push_back(ics);
						this->check_for_flush();
					}
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
    void integration_batcher<number>::report_integration_failure(unsigned int kserial)
      {
        this->failed_serials.push_back(kserial);
        this->report_integration_failure();
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
        this->failed_serials.clear();
	    }


    template <typename number>
    void integration_batcher<number>::end_assignment()
	    {
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "";
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "-- Finished assignment: final integration statistics";
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   processed " << this->num_integrations << " individual integrations in " << format_time(this->integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   mean integration time           = " << format_time(this->integration_time/(this->num_integrations > 0 ? this->num_integrations : 1));
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   longest individual integration  = " << format_time(this->max_integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   shortest individual integration = " << format_time(this->min_integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   total batching time             = " << format_time(this->batching_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   mean batching time              = " << format_time(this->batching_time/(this->num_integrations > 0 ? this->num_integrations : 1));
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   longest individual batch        = " << format_time(this->max_batching_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "--   shortest individual batch       = " << format_time(this->min_batching_time);

        BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "";

        if(this->refinements > 0)
	        {
            BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "!! " << this->refinements << " work items required mesh refinement";
	        }
        if(this->failures > 0)
	        {
            BOOST_LOG_SEV(this->log_source, generic_batcher::normal) << "!! " << this->failures << " work items failed to integrate";
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
    twopf_batcher<number>::twopf_batcher(unsigned int cap, model<number>* m, twopf_task<number>* tk,
                                         const boost::filesystem::path& cp, const boost::filesystem::path& lp, const writer_group& w,
                                         generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                                         handle_type h, unsigned int wn, unsigned int wg)
	    : integration_batcher<number>(cap, m, cp, lp, d, r, h, wn, wg, tk->get_collect_initial_conditions()),
	      writers(w),
        paired_batcher(nullptr),
        parent_task(tk),
        zeta_agent(m, tk)
	    {
        assert(this->parent_task != nullptr);
	    }


    template <typename number>
    void twopf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                           const std::vector<number>& values, const std::vector<number>& backg)
	    {
        if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_TWOPF);

        typename integration_items<number>::twopf_re_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = k_serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->twopf_batch.push_back(item);

        if(this->paired_batcher != nullptr) this->push_paired_twopf(time_serial, k_serial, source_serial, values, backg);

        this->check_for_flush();
	    }


    template <typename number>
    void twopf_batcher<number>::push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                  const std::vector<number>& twopf, const std::vector<number>& backg)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        number zeta_twopf = this->zeta_agent.zeta_twopf(twopf, backg);
        this->paired_batcher->push_twopf(time_serial, k_serial, zeta_twopf, source_serial);
      }


    template <typename number>
    void twopf_batcher<number>::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                  const std::vector<number>& values)
	    {
        if(values.size() != 4) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_TENSOR_TWOPF);

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
	        + (2*sizeof(unsigned int) + sizeof(size_t) + 2*sizeof(boost::timer::nanosecond_type))*this->stats_batch.size()
		      + (sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->ics_batch.size());
	    }


    template <typename number>
    void twopf_batcher<number>::flush(generic_batcher::replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.host_info(this);
        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
		    if(this->collect_initial_conditions) this->writers.ics(this, this->ics_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf(this, this->twopf_batch);
        this->writers.tensor_twopf(this, this->tensor_twopf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
		    this->ics_batch.clear();
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
                                               UnbatchPredicate<typename integration_items<number>::backg_item>(source_serial)),
                                this->backg_batch.end());

        this->twopf_batch.erase(std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                                               UnbatchPredicate<typename integration_items<number>::twopf_re_item>(source_serial)),
                                this->twopf_batch.end());

        this->tensor_twopf_batch.erase(std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                                                      UnbatchPredicate<typename integration_items<number>::tensor_twopf_item>(source_serial)),
                                       this->tensor_twopf_batch.end());

        this->ics_batch.erase(std::remove_if(this->ics_batch.begin(), this->ics_batch.end(),
                                             UnbatchPredicate<typename integration_items<number>::ics_item>(source_serial)),
                              this->ics_batch.end());

        if(this->paired_batcher != nullptr) this->paired_batcher->unbatch(source_serial);
	    }


    template <typename number>
    void twopf_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching)
      {
        this->integration_batcher<number>::report_integration_success(integration, batching);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(integration);
      }


    template <typename number>
    void twopf_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                           unsigned int kserial, size_t steps, unsigned int refinement)
      {
        this->integration_batcher<number>::report_integration_success(integration, batching, kserial, steps, refinement);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(integration);
      }


    template <typename number>
    void twopf_batcher<number>::report_integration_failure()
      {
        this->integration_batcher<number>::report_integration_failure();
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(0);
      }


    template <typename number>
    void twopf_batcher<number>::report_integration_failure(unsigned int kserial)
      {
        this->integration_batcher<number>::report_integration_failure(kserial);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(0);
      }


    // THREEPF BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    threepf_batcher<number>::threepf_batcher(unsigned int cap, model<number>* m, threepf_task<number>* tk,
                                             const boost::filesystem::path& cp, const boost::filesystem::path& lp, const writer_group& w,
                                             generic_batcher::container_dispatch_function d, generic_batcher::container_replacement_function r,
                                             handle_type h, unsigned int wn, unsigned int wg)
	    : integration_batcher<number>(cap, m, cp, lp, d, r, h, wn, wg, tk->get_collect_initial_conditions()),
	      writers(w),
        paired_batcher(nullptr),
        parent_task(tk),
        zeta_agent(m, tk)
	    {
        assert(this->parent_task != nullptr);
	    }


    template <typename number>
    void threepf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                             const std::vector<number>& values, const std::vector<number>& backg, twopf_type t)
	    {
        if(values.size() != 2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_TWOPF);

		    if(t == real_twopf)
			    {
		        typename integration_items<number>::twopf_re_item item;

		        item.time_serial    = time_serial;
		        item.kconfig_serial = k_serial;
		        item.source_serial  = source_serial;
		        item.elements       = values;

				    this->twopf_re_batch.push_back(item);
			    }
		    else
			    {
		        typename integration_items<number>::twopf_im_item item;

		        item.time_serial    = time_serial;
		        item.kconfig_serial = k_serial;
		        item.source_serial  = source_serial;
		        item.elements       = values;

		        this->twopf_im_batch.push_back(item);
			    }

        if(t == real_twopf && this->paired_batcher != nullptr) this->push_paired_twopf(time_serial, k_serial, source_serial, values, backg);

        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                    const std::vector<number>& twopf, const std::vector<number>& backg)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        number zeta_twopf = this->zeta_agent.zeta_twopf(twopf, backg);
        this->paired_batcher->push_twopf(time_serial, k_serial, zeta_twopf, source_serial);
      }


    template <typename number>
    void threepf_batcher<number>::push_threepf(unsigned int time_serial, double t, const threepf_kconfig& kconfig, unsigned int source_serial,
                                               const std::vector<number>& values,
                                               const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                                               const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                                               const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im, const std::vector<number>& bg)
	    {
        if(values.size() != 2*this->Nfields*2*this->Nfields*2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_THREEPF);

        typename integration_items<number>::threepf_item item;

        item.time_serial    = time_serial;
        item.kconfig_serial = kconfig.serial;
        item.source_serial  = source_serial;
        item.elements       = values;

        this->threepf_batch.push_back(item);

        if(this->paired_batcher != nullptr)
          this->push_paired_threepf(time_serial, t, kconfig, source_serial, values,
                                    tpf_k1_re, tpf_k1_im, tpf_k2_re, tpf_k2_im, tpf_k3_re, tpf_k3_im, bg);

        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_paired_threepf(unsigned int time_serial, double t,
                                                      const threepf_kconfig& kconfig, unsigned int source_serial,
                                                      const std::vector<number>& threepf,
                                                      const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                                                      const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                                                      const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im, const std::vector<number>& bg)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        number zeta_threepf = 0.0;
        number redbsp = 0.0;

        this->zeta_agent.zeta_threepf(kconfig, t, threepf, tpf_k1_re, tpf_k1_im, tpf_k2_re, tpf_k2_im, tpf_k3_re, tpf_k3_im, bg, zeta_threepf, redbsp);

        this->paired_batcher->push_threepf(time_serial, kconfig.serial, zeta_threepf, source_serial);
        this->paired_batcher->push_reduced_bispectrum(time_serial, kconfig.serial, redbsp, source_serial);
      }


    template <typename number>
    void threepf_batcher<number>::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                    const std::vector<number>& values)
	    {
        if(values.size() != 4) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_TENSOR_TWOPF);

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
	        + (2*sizeof(unsigned int) + sizeof(size_t) + 2*sizeof(boost::timer::nanosecond_type))*this->stats_batch.size()
	        + (sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->ics_batch.size()
          + (sizeof(unsigned int) + 2*this->Nfields*sizeof(number))*this->kt_ics_batch.size());
	    }


    template <typename number>
    void threepf_batcher<number>::push_kt_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
	    {
        if(values.size() != 2*this->Nfields) throw runtime_exception(runtime_exception::STORAGE_ERROR, CPPTRANSPORT_NFIELDS_BACKG);

        if(this->collect_initial_conditions)
	        {
            typename integration_items<number>::ics_kt_item ics;

            ics.source_serial = k_serial;
		        ics.texit         = t_exit;
            ics.coords        = values;

            this->kt_ics_batch.push_back(ics);
            this->check_for_flush();
	        }
	    }


    template <typename number>
    void threepf_batcher<number>::flush(generic_batcher::replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        this->writers.host_info(this);
        if(this->collect_statistics) this->writers.stats(this, this->stats_batch);
		    if(this->collect_initial_conditions) this->writers.ics(this, this->ics_batch);
		    if(this->collect_initial_conditions) this->writers.kt_ics(this, this->kt_ics_batch);
        this->writers.backg(this, this->backg_batch);
        this->writers.twopf_re(this, this->twopf_re_batch);
        this->writers.twopf_im(this, this->twopf_im_batch);
        this->writers.tensor_twopf(this, this->tensor_twopf_batch);
        this->writers.threepf(this, this->threepf_batch);

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
		    this->ics_batch.clear();
		    this->kt_ics_batch.clear();
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
                                               UnbatchPredicate<typename integration_items<number>::backg_item>(source_serial)),
                                this->backg_batch.end());

        this->twopf_re_batch.erase(std::remove_if(this->twopf_re_batch.begin(), this->twopf_re_batch.end(),
                                                  UnbatchPredicate<typename integration_items<number>::twopf_re_item>(source_serial)),
                                   this->twopf_re_batch.end());

        this->twopf_im_batch.erase(std::remove_if(this->twopf_im_batch.begin(), this->twopf_im_batch.end(),
                                                  UnbatchPredicate<typename integration_items<number>::twopf_im_item>(source_serial)),
                                   this->twopf_im_batch.end());

        this->tensor_twopf_batch.erase(std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                                                      UnbatchPredicate<typename integration_items<number>::tensor_twopf_item>(source_serial)),
                                       this->tensor_twopf_batch.end());

        this->threepf_batch.erase(std::remove_if(this->threepf_batch.begin(), this->threepf_batch.end(),
                                                 UnbatchPredicate<typename integration_items<number>::threepf_item>(source_serial)),
                                  this->threepf_batch.end());

        this->ics_batch.erase(std::remove_if(this->ics_batch.begin(), this->ics_batch.end(),
                                             UnbatchPredicate<typename integration_items<number>::ics_item>(source_serial)),
                              this->ics_batch.end());

        this->kt_ics_batch.erase(std::remove_if(this->kt_ics_batch.begin(), this->kt_ics_batch.end(),
                                                UnbatchPredicate<typename integration_items<number>::ics_kt_item>(source_serial)),
                              this->kt_ics_batch.end());

        if(this->paired_batcher != nullptr) this->paired_batcher->unbatch(source_serial);
	    }


    template <typename number>
    void threepf_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching)
      {
        this->integration_batcher<number>::report_integration_success(integration, batching);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(integration);
      }


    template <typename number>
    void threepf_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                             unsigned int kserial, size_t steps, unsigned int refinement)
      {
        this->integration_batcher<number>::report_integration_success(integration, batching, kserial, steps, refinement);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(integration);
      }


    template <typename number>
    void threepf_batcher<number>::report_integration_failure()
      {
        this->integration_batcher<number>::report_integration_failure();
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(0);
      }


    template <typename number>
    void threepf_batcher<number>::report_integration_failure(unsigned int kserial)
      {
        this->integration_batcher<number>::report_integration_failure(kserial);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(0);
      }


  }   // namespace transport


#endif //__integration_batcher_H_
