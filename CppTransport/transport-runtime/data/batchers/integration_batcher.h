//
// Created by David Seery on 26/03/15.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_INTEGRATION_BATCHER_H
#define CPPTRANSPORT_INTEGRATION_BATCHER_H


#include <vector>
#include <set>
#include <functional>

#include "transport-runtime/enumerations.h"

#include "transport-runtime/data/batchers/generic_batcher.h"
#include "transport-runtime/data/batchers/postintegration_batcher.h"
#include "transport-runtime/data/batchers/integration_items.h"
#include "transport-runtime/data/batchers/postprocess_delegate.h"

#include "transport-runtime/models/model_forward_declare.h"
#include "transport-runtime/tasks/tasks_forward_declare.h"
#include "transport-runtime/tasks/task_configurations.h"


namespace transport
	{

		// forward declare
		template <typename number> class integration_batcher;


		// writer functions
		template <typename number>
		class integration_writers
			{

		  public:

        //! Transaction factory
        using transaction_factory = std::function<transaction_manager(integration_batcher<number>*)>;

        // Write functions don't take const references for each cache because the cache is sorted in-place
        // This sort step is important -- it dramatically improves SQLite performance

		    //! Background writer function
		    using backg_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::backg_item> >&)>;

		    //! 2pf writer function - real part
		    using twopf_re_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::twopf_re_item> >&)>;

		    //! 2pf writer function - imaginary part
		    using twopf_im_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::twopf_im_item> >&)>;

		    //! 2pf 'spectral index' writer function (currently only the real part is stored)
		    using twopf_si_re_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::twopf_si_re_item> >&)>;

		    //! Tensor two-point function writer function (currently only the real part is stored)
		    using tensor_twopf_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::tensor_twopf_item> >&)>;

        //! Tensor two-point function writer function (currently only the real part is stored)
        using tensor_twopf_si_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::tensor_twopf_si_item> >&)>;

		    //! Three-point function writer function for momentum insertions
		    using threepf_momentum_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::threepf_momentum_item> >&)>;

        //! Three-point function writer function for derivative insertions
        using threepf_Nderiv_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::threepf_Nderiv_item> >&)>;

		    //! Per-configuration statistics writer function
		    using stats_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::configuration_statistics> >&)>;

				//! Per-configuration initial conditions writer function
				using ics_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::ics_item> >&)>;

		    //! Per-configuration initial conditions writer function - kt variant
		    using ics_kt_writer = std::function<void(transaction_manager&, integration_batcher<number>*, std::vector<std::unique_ptr< typename integration_items<number>::ics_kt_item> >&)>;

		    //! Host information writer function
		    using host_info_writer =  std::function<void(transaction_manager&, integration_batcher<number>*)>;
			};


		template <typename number>
    class integration_batcher: public generic_batcher
	    {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        integration_batcher(size_t cap, unsigned int ckp, model<number>* m, integration_task<number>* tk,
                            const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                            std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                            handle_type h, unsigned int w, unsigned int g=0, bool ics=false);

        //! move constructor
        integration_batcher(integration_batcher<number>&&) noexcept = default;

        //! destructor is default
        ~integration_batcher() override = default;


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

        //! Return stepper tolerance - background evolution
        std::pair< double, double > get_back_tol() const { return(this->mdl->get_back_tol()); }

        //! Return stepper tolerance - perturbations evolution
        std::pair< double, double > get_pert_tol() const { return(this->mdl->get_pert_tol()); }

        //! Close this batcher -- called at the end of an integration
        void close() override;


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        virtual void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, size_t steps, unsigned int refinement);

        //! Report a failed integration for a specific serial number
        virtual void report_integration_failure(unsigned int kserial);

        //! Report an integration which required mesh refinement
        void report_refinement();


        //! Query whether any integrations failed
        bool is_failed() const { return (this->failures > 0); }

        //! Query how many integrations report a refinement
        unsigned int get_reported_refinements() const { return(this->refinements); }

        //! Query number of failed integration reports
        unsigned int get_reported_failures() const { return(this->failures); }

        //! Query a list of failed serial numbers (not all backends may support this)
        const std::set< unsigned int>& get_failed_serials() const { return(this->failed_serials); }


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


        // UNBATCH

      public:

        //! Unbatch a given configuration
        virtual void unbatch(unsigned int source_serial) = 0;


        // INTERNAL DATA

      protected:


        // CACHES

        //! Cache of background pushes
        std::vector< std::unique_ptr< typename integration_items<number>::backg_item > > backg_batch;

        //! Cache of per-configuration statistics
        std::vector< std::unique_ptr< typename integration_items<number>::configuration_statistics > > stats_batch;


        // OTHER INTERNAL DATA

		    //! pointer to parent model
        model<number>* mdl;

        //! pointer to parent task
        integration_task<number>* parent_task;

        //! cache number of fields associated with this integration
        const unsigned int Nfields;

        //! cache number of time configurations
        const size_t time_db_size;


        // INTEGRATION EVENT TRACKING

        //! number of integrations which have failed
        unsigned int failures;

        //! list of failed k-configuration serial numbers (not all backends may support tracking this)
        std::set< unsigned int > failed_serials;

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

	          using writer_type = integration_writers<number>;

            //! constructor captures all writers and ensures they are correctly initialized;
            //! once constructed, the writers cannot be changed
            writer_group(typename writer_type::transaction_factory f, typename writer_type::backg_writer bg,
                         typename writer_type::twopf_re_writer tpf, typename writer_type::twopf_si_re_writer twsi,
                         typename writer_type::tensor_twopf_writer ten, typename writer_type::tensor_twopf_si_writer tensi,
                         typename writer_type::stats_writer s, typename writer_type::ics_writer i,
                         typename writer_type::host_info_writer h)
              : factory{std::move(f)},
                backg{std::move(bg)},
                twopf{std::move(tpf)},
                twopf_si{std::move(twsi)},
                tensor_twopf{std::move(ten)},
                tensor_twopf_si{std::move(tensi)},
                stats{std::move(s)},
                ics{std::move(i)},
                host_info{std::move(h)}
              {
              }

            const typename writer_type::transaction_factory factory;
            const typename writer_type::backg_writer backg;
            const typename writer_type::twopf_re_writer twopf;
            const typename writer_type::twopf_si_re_writer twopf_si;
            const typename writer_type::tensor_twopf_writer tensor_twopf;
            const typename writer_type::tensor_twopf_si_writer tensor_twopf_si;
            const typename writer_type::stats_writer stats;
            const typename writer_type::ics_writer ics;
            const typename writer_type::host_info_writer host_info;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        twopf_batcher(size_t cap, unsigned int ckp, model<number>* m, twopf_task<number>* tk,
                      const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                      const writer_group& w,
                      std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                      handle_type h, unsigned int wn, unsigned int wg);

        //! move constructor
        twopf_batcher(twopf_batcher<number>&&)  noexcept = default;

        //! destructor is default
        virtual ~twopf_batcher() = default;


        // ADMINISTRATION

      public:

        //! Override integration_batcher close() to push notification to a paired batcher, if one is present
        void close() override
          {
            this->integration_batcher<number>::close();
            if(this->paired_batcher != nullptr) this->paired_batcher->close();
            this->paired_batcher = nullptr;
          }


        // BATCH, UNBATCH

      public:

	      //! push a sampled twopf value to the batcher with assigned time and k-config serial numbers;
	      //! the background field configuration is needed in case paired batching is being used, in which case
	      //! we need to evaluate the gauge transformation to zeta.
	      //! \param time_serial    time sample serial number
	      //! \param k_serial       k-configuration sample serial number
	      //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
	      //!                       twopf task this is just the same as k_serial, but in a threepf task
	      //!                       it will be the 3pf k-config serial number, which might generate up to three
	      //!                       twopf k-configurations
	      //! \param value          vector representing flattened 2-point correlation function
	      //! \param backg          vector representing background field configuration; not used by the integration
	      //!                       batcher itself, but can be passed to a paired zeta batcher for the purpose
	      //!                       of computing gauge transformation coefficients
        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                        const std::vector<number>& value, const std::vector<number>& backg);

        //! push a sampled twopf 'spectral index' value to the batcher with assigned time and k-config serial numbers;
        //! the background field configuration is needed in case paired batching is being used, in which case
        //! we need to evaluate the gauge transformation to zeta.
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration sample serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened 2-point 'spectral index' correlation function
        //! \param backg          vector representing background field configuration; not used by the integration
        //!                       batcher itself, but can be passed to a paired zeta batcher for the purpose
        //!                       of computing gauge transformation coefficients
        void push_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                           const std::vector<number>& value, const std::vector<number>& backg);

        //! push a sampled tensor twopf value to the batcher with assigned time and k-config serial numbers
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened tensor two-point function
        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                               const std::vector<number>& value);

        //! push a sampled tensor twopf 'spectral index' value to the batcher with assigned time and k-config
        //! serial numbers
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened tensor two-point function
        void push_tensor_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                  const std::vector<number>& value);

        //! push a set of initial conditions for a specified k-config serial number
        //! \param k_serial   k-configuration serial numbers
        //! \param t_exit     time of horizon-exit for this k-configuration
        //! \param values     snapshot of field configuration at horizon-exit time
        void push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values);

        //! unbatch entries corresponding to a specific source serial numbers
        //! \param source_serial  specify which configurations to unbatch
        void unbatch(unsigned int source_serial) override;

	    protected:

        //! used internally by push_twopf() to push a zeta 2pf value into the paired batcher
        void push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                               const std::vector<number>& twopf, const std::vector<number>& backg);


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                        unsigned int kserial, size_t steps, unsigned int refinement) override;

        //! Report a failed integration for a specific serial number
        void report_integration_failure(unsigned int kserial) override;


        // FLUSH INTERFACE

      public:

        //! Override generic batcher set_flush_mode() to push flush settings to a paired batcher, if one is present; then unpair
        void set_flush_mode(generic_batcher::flush_mode f) override
          {
            this->generic_batcher::set_flush_mode(f);
            if(this->paired_batcher != nullptr) this->paired_batcher->set_flush_mode(f);
          }


        // PAIR

      public:

        //! Pair with a zeta batcher. Remember to push our flush setting to it.
        void pair(zeta_twopf_batcher<number>* batcher)
          {
            assert(batcher != nullptr);
            this->paired_batcher = batcher;
            this->paired_batcher->set_flush_mode(this->get_flush_mode());
          }


        // INTERNAL API

      protected:

        size_t storage() const override;

        void flush(replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! twopf cache
        std::vector< std::unique_ptr< typename integration_items<number>::twopf_re_item > > twopf_batch;
        
        //! twopf 'spectral index' cache
        std::vector< std::unique_ptr< typename integration_items<number>::twopf_si_re_item > > twopf_si_batch;

        //! tensor twopf cache
        std::vector< std::unique_ptr< typename integration_items<number>::tensor_twopf_item > > tensor_twopf_batch;

        //! tensor twopf 'spectral index' cache
        std::vector< std::unique_ptr< typename integration_items<number>::tensor_twopf_si_item > > tensor_twopf_si_batch;

        //! initial conditions cache
        std::vector< std::unique_ptr< typename integration_items<number>::ics_item > > ics_batch;

        //! cache for linear part of gauge transformation
        std::vector<number> gauge_xfm1;


        // PAIRING

        //! task associated with this batcher (for computing gauge transforms, etc.)
        twopf_task<number>* parent_task;

        //! Paired zeta batcher, if present
        zeta_twopf_batcher<number>* paired_batcher;

        //! cache number of k-configurations in database
        const size_t kconfig_db_size;


        // COMPUTATION AGENT

        //! compute delegate
        postprocess_delegate<number> compute_agent;

      };


		template <typename number>
    class threepf_batcher: public integration_batcher<number>
	    {

      public:

        class writer_group
	        {
          public:

	          using writer_type = integration_writers<number>;

	          //! constructor captures all writers and ensures they are correctly initialized;
	          //! once constructed, the writers cannot be changed
            writer_group(typename writer_type::transaction_factory f, typename writer_type::backg_writer bg,
                         typename writer_type::twopf_re_writer twre, typename writer_type::twopf_im_writer twim, typename writer_type::twopf_si_re_writer twsi,
                         typename writer_type::tensor_twopf_writer ten, typename writer_type::tensor_twopf_si_writer tensi,
                         typename writer_type::threepf_momentum_writer thmom, typename writer_type::threepf_Nderiv_writer thdv,
                         typename writer_type::stats_writer s, typename writer_type::ics_writer i, typename writer_type::ics_kt_writer ikt,
                         typename writer_type::host_info_writer h)
              : factory{std::move(f)},
                backg{std::move(bg)},
                twopf_re{std::move(twre)},
                twopf_im{std::move(twim)},
                twopf_si_re{std::move(twsi)},
                tensor_twopf{std::move(ten)},
                tensor_twopf_si{std::move(tensi)},
                threepf_momentum{std::move(thmom)},
                threepf_Nderiv{std::move(thdv)},
                stats{std::move(s)},
                ics{std::move(i)},
                kt_ics{std::move(ikt)},
                host_info{std::move(h)}
              {
              }

            const typename writer_type::transaction_factory factory;
            const typename writer_type::backg_writer backg;
            const typename writer_type::twopf_re_writer twopf_re;
            const typename writer_type::twopf_im_writer twopf_im;
            const typename writer_type::twopf_si_re_writer twopf_si_re;
            const typename writer_type::tensor_twopf_writer tensor_twopf;
            const typename writer_type::tensor_twopf_si_writer tensor_twopf_si;
            const typename writer_type::threepf_momentum_writer threepf_momentum;
            const typename writer_type::threepf_Nderiv_writer threepf_Nderiv;
            const typename writer_type::stats_writer stats;
            const typename writer_type::ics_writer ics;
            const typename writer_type::ics_kt_writer kt_ics;
            const typename writer_type::host_info_writer host_info;
	        };


        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        template <typename handle_type>
        threepf_batcher(size_t cap, unsigned int ckp, model<number>* m, threepf_task<number>* tk,
                        const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                        const writer_group& w,
                        std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                        handle_type h, unsigned int wn, unsigned int wg);

        //! move constructor
        threepf_batcher(threepf_batcher<number>&&)  noexcept = default;

        //! destructor is default
        virtual ~threepf_batcher() = default;


        // INTEGRATION MANAGEMENT

      public:

        //! Add integration details, plus report a k-configuration serial number and mesh refinement level for storing per-configuration statistics
        void report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching, unsigned int kserial, size_t steps, unsigned int refinement) override;

        //! Report a failed integration for a specific serial number
        void report_integration_failure(unsigned int kserial) override;



        // ADMINISTRATION

      public:

        //! Override integration_batcher close() to push notification to a paired batcher, if one is present; then unpair
        void close() override
          {
            this->integration_batcher<number>::close();
            if(this->paired_batcher != nullptr) this->paired_batcher->close();
            this->paired_batcher = nullptr;
          }


        // BATCH, UNBATCH

      public:

        //! push a sampled twopf value to the batcher with assigned time and k-config serial numbers;
        //! the background field configuration is needed in case paired batching is being used, in which case
        //! we need to evaluate the gauge transformation to zeta.
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration sample serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value           vector representing flattened 2-point correlation function
        //! \param backg          vector representing background field configuration; not used by the integration
        //!                       batcher itself, but can be passed to a paired zeta batcher for the purpose
        //!                       of computing gauge transformation coefficients
        //! \param t              value of type enum twopf_type specifying whether theis is a real or
        //!                       imaginary part
        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                        const std::vector<number>& value, const std::vector<number>& backg,
                        twopf_type t = twopf_type::real);

        //! push a sampled twopf 'spectral index' value to the batcher with assigned time and k-config serial numbers;
        //! the background field configuration is needed in case paired batching is being used, in which case
        //! we need to evaluate the gauge transformation to zeta.
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration sample serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened 2-point 'spectral index' correlation function
        //! \param backg          vector representing background field configuration; not used by the integration
        //!                       batcher itself, but can be passed to a paired zeta batcher for the purpose
        //!                       of computing gauge transformation coefficients
        void push_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                           const std::vector<number>& value, const std::vector<number>& backg);

        //! push a sampled threepf value to the batcher with assigned time and k-config serial numbers;
        //! the background field configuration is needed in case paired batching is being used, in which case
        //! we need to evaluate the gauge transformation to zeta.
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration sample serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened 2-point correlation function
        //! \param tpf_k1_re      real part of 2pf for mode k1, passed to paired zeta batcher (if present) for
        //!                       computations of the zeta 3pf
        //! \param tpf_k2_re      real part of 2pf for mode k2, passed to paired zeta batcher (if present) for
        //!                       computations of the zeta 3pf
        //! \param tpf_k3_re      real part of 2pf for mode k3, passed to paired zeta batcher (if present) for
        //!                       computations of the zeta 3pf
        //! \param tpf_k1_im      imaginary part of 2pf for mode k1, passed to paired zeta batcher (if present) for
        //!                       computations of the zeta 3pf
        //! \param tpf_k2_im      imaginary part of 2pf for mode k2, passed to paired zeta batcher (if present) for
        //!                       computations of the zeta 3pf
        //! \param tpf_k3_im      imaginary part of 2pf for mode k3, passed to paired zeta batcher (if present) for
        //!                       computations of the zeta 3pf
        //! \param bg             vector representing background field configuration; not used by the integration
        //!                       batcher itself, but can be passed to a paired zeta batcher for the purpose
        //!                       of computing gauge transformation coefficients
        //! \param t              value of type enum twopf_type specifying whether theis is a real or
        //!                       imaginary part
        void push_threepf(unsigned int time_serial, double t,
                          const threepf_kconfig& kconfig, unsigned int source_serial,
                          const std::vector<number>& value,
                          const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                          const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                          const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im,
                          const std::vector<number>& bg);

        //! push a sampled tensor twopf value to the batcher with assigned time and k-config serial numbers
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened tensor two-point function
        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                               const std::vector<number>& value);

        //! push a sampled tensor twopf 'spectral index' value to the batcher with assigned time and k-config
        //! serial numbers
        //! \param time_serial    time sample serial number
        //! \param k_serial       k-configuration serial number
        //! \param source_serial  k-configuration serial number from where this sample "came from"; in a
        //!                       twopf task this is just the same as k_serial, but in a threepf task
        //!                       it will be the 3pf k-config serial number, which might generate up to three
        //!                       twopf k-configurations
        //! \param value          vector representing flattened tensor two-point function
        void push_tensor_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                  const std::vector<number>& value);

        //! push a set of initial conditions for a specified k-config serial number;
        //! for a 3pf the k-mode exiting the horizon is taken to be the smallest k-modein the configuration
        //! \param k_serial   k-configuration serial numbers
        //! \param t_exit     time of horizon-exit for this k-configuration
        //! \param values     snapshot of field configuration at horizon-exit time
        void push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values);


        //! push a set of initial conditions for a specified k-config serial number, but measured using
        //! k_t as the mode exiting the horizon rather than the smallest k-mode in the configuration
        //! \param k_serial   k-configuration serial numbers
        //! \param t_exit     time of horizon-exit for this k-configuration
        //! \param values     snapshot of field configuration at horizon-exit time
		    void push_kt_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values);

        //! unbatch entries corresponding to a specific source serial numbers
        //! \param source_serial  specify which configurations to unbatch
        void unbatch(unsigned int source_serial) override;

	    protected:

        //! used internally by push_twopf() to push a zeta 2pf value into the paired batcher
        void push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                               const std::vector<number>& twopf, const std::vector<number>& backg);

        //! used internally by push_threepff() to push a zeta 3pf value into the paired batcher
        void push_paired_threepf(unsigned int time_serial, double t,
                                 const threepf_kconfig& kconfig, unsigned int source_serial, const std::vector<number>& value,
                                 const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                                 const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                                 const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im,
                                 const std::vector<number>& bg);


        // FLUSH INTERFACE

      public:

        //! Override generic batcher set_flush_mode() to push flush settings to a paired batcher, if one is present
        void set_flush_mode(generic_batcher::flush_mode f) override
          {
            this->generic_batcher::set_flush_mode(f);
            if(this->paired_batcher != nullptr) this->paired_batcher->set_flush_mode(f);
          }


        // PAIR

      public:

        //! Pair with a zeta batcher. Remember to push our flush setting to it.
        void pair(zeta_threepf_batcher<number>* batcher)
          {
            assert(batcher != nullptr);
            this->paired_batcher = batcher;
            this->paired_batcher->set_flush_mode(this->get_flush_mode());
          }


        // INTERNAL API

      protected:

        size_t storage() const override;

        void flush(replacement_action action) override;


        // INTERNAL DATA

      protected:

        //! writer group
        const writer_group writers;

        //! real twopf cache
        std::vector< std::unique_ptr< typename integration_items<number>::twopf_re_item > > twopf_re_batch;

        //! imaginary twopf cache
        std::vector< std::unique_ptr< typename integration_items<number>::twopf_im_item > > twopf_im_batch;

        //! twopf 'spectral index' cache
        std::vector< std::unique_ptr< typename integration_items<number>::twopf_si_re_item > > twopf_si_re_batch;

        //! tensor twopf cache
        std::vector< std::unique_ptr< typename integration_items<number>::tensor_twopf_item > > tensor_twopf_batch;

        //! tensor twopf 'spectral index' cache
        std::vector< std::unique_ptr< typename integration_items<number>::tensor_twopf_si_item > > tensor_twopf_si_batch;

        //! threepf momentum-insertions cache
        std::vector< std::unique_ptr< typename integration_items<number>::threepf_momentum_item > > threepf_momentum_batch;

        //! threepf Nderiv-insertions cache
        std::vector< std::unique_ptr< typename integration_items<number>::threepf_Nderiv_item > > threepf_Nderiv_batch;

        //! initial conditions cache
        std::vector< std::unique_ptr< typename integration_items<number>::ics_item > > ics_batch;

        //! k_t initial conditions cache
        std::vector< std::unique_ptr< typename integration_items<number>::ics_kt_item > > kt_ics_batch;

        //! cache for linear part of gauge transformation
        std::vector<number> gauge_xfm1;

        //! cache for quadratic part of gauge transformation, 123 permutation
        std::vector<number> gauge_xfm2_123;

        //! cache for quadratic part of gauge transformation, 213 permutation
        std::vector<number> gauge_xfm2_213;

        //! cache for quadratic part of gauge transformation, 312 permutation
        std::vector<number> gauge_xfm2_312;


        // PAIRING

        //! threepf-task associated with this batcher (for computing gauge transforms etc.)
        threepf_task<number>* parent_task;

        //! Paired zeta batcher, if present
        zeta_threepf_batcher<number>* paired_batcher;

        //! cache number of k-configurations in database
        const size_t kconfig_db_size;


        // COMPUTATION AGENT

        //! compute delegate
        postprocess_delegate<number> compute_agent;

	    };


    // INTEGRATION BATCHER METHODS


    template <typename number>
    template <typename handle_type>
    integration_batcher<number>::integration_batcher(size_t cap, unsigned int ckp, model<number>* m, integration_task<number>* tk,
                                                     const boost::filesystem::path& cp, const boost::filesystem::path& lp,
                                                     std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                                                     handle_type h, unsigned int w, unsigned int g, bool ics)
	    : generic_batcher(cap, ckp, cp, lp, std::move(d), std::move(r), h, w, g),
        Nfields(m->get_N_fields()),
        mdl(m),
        parent_task(tk),
        time_db_size(tk->get_stored_time_config_database().size()),
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
    void integration_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                                 unsigned int kserial, size_t steps, unsigned int refinements)
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
            this->flush(replacement_action::action_replace);
          }
        else if(this->checkpoint_interval > 0 && this->checkpoint_timer.elapsed().wall > this->checkpoint_interval)
          {
            BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal)
              << "** Lifetime of " << format_time(this->checkpoint_timer.elapsed().wall)
              << " exceeds checkpoint interval " << format_time(this->checkpoint_interval)
              << "; forcing flush";
            this->flush(replacement_action::action_replace);
          }

		    if(this->collect_statistics)
			    {
		        this->stats_batch.emplace_back(std::make_unique<typename integration_items<number>::configuration_statistics>(kserial, integration, batching, refinements, steps));
			    }

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
    void integration_batcher<number>::push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
	    {
        this->backg_batch.emplace_back(std::make_unique<typename integration_items<number>::backg_item>(time_serial, source_serial, values, this->time_db_size, this->Nfields));
        this->check_for_flush();
	    }


    template <typename number>
    void integration_batcher<number>::report_integration_failure(unsigned int kserial)
      {
        this->failed_serials.insert(kserial);
        this->failures++;
        this->check_for_flush();
    
        if(this->flush_due)
          {
            this->flush_due = false;
            this->flush(replacement_action::action_replace);
          }
        else if(this->checkpoint_interval > 0 && this->checkpoint_timer.elapsed().wall > this->checkpoint_interval)
          {
            BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal)
              << "** Lifetime of " << format_time(this->checkpoint_timer.elapsed().wall)
              << " exceeds checkpoint interval " << format_time(this->checkpoint_interval) << "; forcing flush";
            this->flush(replacement_action::action_replace);
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
        this->failed_serials.clear();
	    }


    template <typename number>
    void integration_batcher<number>::end_assignment()
	    {
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "";
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "-- Finished assignment: final integration statistics";
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   processed " << this->num_integrations << " individual integrations in " << format_time(this->integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   mean integration time           = " << format_time(this->integration_time/(this->num_integrations > 0 ? this->num_integrations : 1));
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   longest individual integration  = " << format_time(this->max_integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   shortest individual integration = " << format_time(this->min_integration_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   total batching time             = " << format_time(this->batching_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   mean batching time              = " << format_time(this->batching_time/(this->num_integrations > 0 ? this->num_integrations : 1));
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   longest individual batch        = " << format_time(this->max_batching_time);
        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "--   shortest individual batch       = " << format_time(this->min_batching_time);

        BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "";

        if(this->refinements > 0)
	        {
            BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "!! " << this->refinements << " work items required mesh refinement";
	        }
        if(this->failures > 0)
	        {
            BOOST_LOG_SEV(this->log_source, generic_batcher::log_severity_level::normal) << "!! " << this->failures << " work items failed to integrate";
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
    twopf_batcher<number>::twopf_batcher(size_t cap, unsigned int ckp, model<number>* m, twopf_task<number>* tk,
                                         const boost::filesystem::path& cp, const boost::filesystem::path& lp, const writer_group& w,
                                         std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                                         handle_type h, unsigned int wn, unsigned int wg)
	    : integration_batcher<number>(cap, ckp, m, tk, cp, lp, std::move(d), std::move(r), h, wn, wg, tk->get_collect_initial_conditions()),
	      writers(w),
        paired_batcher(nullptr),
        parent_task(tk),
        kconfig_db_size(tk->get_twopf_database().size()),
        compute_agent(m, tk)
	    {
        assert(this->parent_task != nullptr);

        gauge_xfm1.resize(2*this->Nfields);
	    }


    template <typename number>
    void twopf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                           const std::vector<number>& value, const std::vector<number>& backg)
	    {
        this->twopf_batch.emplace_back(
          std::make_unique<typename integration_items<number>::twopf_re_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size, this->Nfields));
        if(this->paired_batcher != nullptr) this->push_paired_twopf(time_serial, k_serial, source_serial, value, backg);

        this->check_for_flush();
	    }


    template <typename number>
    void twopf_batcher<number>::push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                  const std::vector<number>& twopf, const std::vector<number>& backg)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        number zeta_twopf = 0.0;

        this->compute_agent.zeta_twopf(twopf, backg, zeta_twopf, this->gauge_xfm1);
        this->paired_batcher->push_twopf(time_serial, k_serial, zeta_twopf, source_serial);
        this->paired_batcher->push_gauge_xfm1(time_serial, k_serial, this->gauge_xfm1, source_serial);
      }


    template <typename number>
    void
    twopf_batcher<number>::push_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                         const std::vector<number>& value, const std::vector<number>& backg)
      {
        this->twopf_si_batch.emplace_back(
          std::make_unique<typename integration_items<number>::twopf_si_re_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size, this->Nfields));
        // TODO: push spectral index to paired zeta batcher, if one is present
        
        this->check_for_flush();
      }


    template <typename number>
    void twopf_batcher<number>::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial,
                                                  unsigned int source_serial,
                                                  const std::vector<number>& value)
	    {
        this->tensor_twopf_batch.emplace_back(
          std::make_unique<typename integration_items<number>::tensor_twopf_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size));
        this->check_for_flush();
	    }


    template <typename number>
    void twopf_batcher<number>::push_tensor_twopf_si(unsigned int time_serial, unsigned int k_serial,
                                                     unsigned int source_serial, const std::vector<number>& value)
      {
        this->tensor_twopf_si_batch.emplace_back(
          std::make_unique<typename integration_items<number>::tensor_twopf_si_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size));
        this->check_for_flush();
      }


    template <typename number>
    void twopf_batcher<number>::push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
      {
        if(this->collect_initial_conditions)
          {
            this->ics_batch.emplace_back(
              std::make_unique<typename integration_items<number>::ics_item>(k_serial, t_exit, values,
                                                                             this->kconfig_db_size, this->Nfields));
            this->check_for_flush();
          }
      }


    template <typename number>
    size_t twopf_batcher<number>::storage() const
	    {
        const auto backg_size = 2 * this->Nfields * sizeof(number);
        constexpr auto tensor_size = 4 * sizeof(number);
        const auto twopf_size = 2 * this->Nfields * 2 * this->Nfields * sizeof(number);
        constexpr auto timing_data_size = sizeof(boost::timer::nanosecond_type);

        return(
          (4 * sizeof(unsigned int) + backg_size) * this->backg_batch.size()
          + (5 * sizeof(unsigned int) + tensor_size) * this->tensor_twopf_batch.size()
          + (5 * sizeof(unsigned int) + tensor_size) * this->tensor_twopf_si_batch.size()
          + (6 * sizeof(unsigned int) + twopf_size) * this->twopf_batch.size()
          + (6 * sizeof(unsigned int) + twopf_size) * this->twopf_si_batch.size()
          + (2 * sizeof(unsigned int) + sizeof(size_t) + 2 * timing_data_size) * this->stats_batch.size()
          + (sizeof(unsigned int) + backg_size) * this->ics_batch.size()
        );
	    }


    template <typename number>
    void twopf_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushing twopf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        transaction_manager mgr = this->writers.factory(this);

        this->writers.host_info(mgr, this);
        if(this->collect_statistics) this->writers.stats(mgr, this, this->stats_batch);
		    if(this->collect_initial_conditions) this->writers.ics(mgr, this, this->ics_batch);
        this->writers.backg(mgr, this, this->backg_batch);
        this->writers.twopf(mgr, this, this->twopf_batch);
        this->writers.twopf_si(mgr, this, this->twopf_si_batch);
        this->writers.tensor_twopf(mgr, this, this->tensor_twopf_batch);
        this->writers.tensor_twopf_si(mgr, this, this->tensor_twopf_si_batch);

        mgr.commit();

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
		    this->ics_batch.clear();
        this->backg_batch.clear();
        this->twopf_batch.clear();
        this->twopf_si_batch.clear();
        this->tensor_twopf_batch.clear();
        this->tensor_twopf_si_batch.clear();

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
    void twopf_batcher<number>::unbatch(unsigned int source_serial)
	    {
        this->backg_batch.erase(
          std::remove_if(this->backg_batch.begin(), this->backg_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::backg_item>(source_serial)),
          this->backg_batch.end());

        this->twopf_batch.erase(
          std::remove_if(this->twopf_batch.begin(), this->twopf_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::twopf_re_item>(source_serial)),
          this->twopf_batch.end());

        this->twopf_si_batch.erase(
          std::remove_if(this->twopf_si_batch.begin(), this->twopf_si_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::twopf_si_re_item>(source_serial)),
          this->twopf_si_batch.end());

        this->tensor_twopf_batch.erase(
          std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::tensor_twopf_item>(source_serial)),
          this->tensor_twopf_batch.end());

        this->tensor_twopf_si_batch.erase(
          std::remove_if(this->tensor_twopf_si_batch.begin(), this->tensor_twopf_si_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::tensor_twopf_si_item>(source_serial)),
          this->tensor_twopf_si_batch.end());

        this->ics_batch.erase(
          std::remove_if(this->ics_batch.begin(), this->ics_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::ics_item>(source_serial)),
          this->ics_batch.end());

        if(this->paired_batcher != nullptr) this->paired_batcher->unbatch(source_serial);
	    }


    template <typename number>
    void twopf_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                           unsigned int kserial, size_t steps, unsigned int refinement)
      {
        this->integration_batcher<number>::report_integration_success(integration, batching, kserial, steps, refinement);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(integration);
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
    threepf_batcher<number>::threepf_batcher(size_t cap, unsigned int ckp, model<number>* m, threepf_task<number>* tk,
                                             const boost::filesystem::path& cp, const boost::filesystem::path& lp, const writer_group& w,
                                             std::unique_ptr<container_dispatch_function> d, std::unique_ptr<container_replace_function> r,
                                             handle_type h, unsigned int wn, unsigned int wg)
	    : integration_batcher<number>(cap, ckp, m, tk, cp, lp, std::move(d), std::move(r), h, wn, wg, tk->get_collect_initial_conditions()),
	      writers(w),
        paired_batcher(nullptr),
        parent_task(tk),
        kconfig_db_size(tk->get_threepf_database().size()),
        compute_agent(m, tk)
	    {
        assert(this->parent_task != nullptr);

        gauge_xfm1.resize(2*this->Nfields);

        gauge_xfm2_123.resize(2*this->Nfields * 2*this->Nfields);
        gauge_xfm2_213.resize(2*this->Nfields * 2*this->Nfields);
        gauge_xfm2_312.resize(2*this->Nfields * 2*this->Nfields);
	    }


    template <typename number>
    void threepf_batcher<number>::push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                             const std::vector<number>& value, const std::vector<number>& backg, twopf_type t)
	    {
        switch(t)
          {
            case twopf_type::real:
              {
                this->twopf_re_batch.emplace_back(
                  std::make_unique<typename integration_items<number>::twopf_re_item>(
                    time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size, this->Nfields));
                break;
              }

            case twopf_type::imag:
              {
                this->twopf_im_batch.emplace_back(
                  std::make_unique<typename integration_items<number>::twopf_im_item>(
                    time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size, this->Nfields));
                break;
              }
          }

        if(t == twopf_type::real && this->paired_batcher != nullptr)
          {
            this->push_paired_twopf(time_serial, k_serial, source_serial, value, backg);
          }

        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_paired_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                    const std::vector<number>& twopf, const std::vector<number>& backg)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        number zeta_twopf = 0.0;

        this->compute_agent.zeta_twopf(twopf, backg, zeta_twopf, this->gauge_xfm1);
        this->paired_batcher->push_twopf(time_serial, k_serial, zeta_twopf, source_serial);
        this->paired_batcher->push_gauge_xfm1(time_serial, k_serial, this->gauge_xfm1, source_serial);
      }


    template <typename number>
    void
    threepf_batcher<number>::push_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                           const std::vector<number>& value, const std::vector<number>& backg)
      {
        this->twopf_si_re_batch.emplace_back(
          std::make_unique<typename integration_items<number>::twopf_si_re_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size, this->Nfields));
        // TODO: push spectral index to paired zeta batcher, if one is present

        this->check_for_flush();
      }


    template <typename number>
    void threepf_batcher<number>::push_threepf(unsigned int time_serial, double t, const threepf_kconfig& kconfig,
                                               unsigned int source_serial, const std::vector<number>& value,
                                               const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                                               const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                                               const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im,
                                               const std::vector<number>& bg)
	    {
        // momentum three-point function can be copied across directly
        this->threepf_momentum_batch.emplace_back(
          std::make_unique<typename integration_items<number>::threepf_momentum_item>(
            time_serial, kconfig.serial, source_serial, value, this->time_db_size, this->kconfig_db_size, this->Nfields));

        // derivative three-point function needs extra shifts in order to convert any momentum insertions
        // into time-derivative insertions
        std::vector<number> Nderiv_values(value.size());
        for(unsigned int i = 0; i < 2*this->Nfields; ++i)
          {
            for(unsigned int j = 0; j < 2*this->Nfields; ++j)
              {
                for(unsigned int k = 0; k < 2*this->Nfields; ++k)
                  {
                    number tpf = value[this->mdl->flatten(i, j, k)];

                    this->compute_agent.shift(i, j, k, kconfig, time_serial, tpf_k1_re, tpf_k1_im, tpf_k2_re,
                                              tpf_k2_im, tpf_k3_re, tpf_k3_im, bg, tpf);

                    Nderiv_values[this->mdl->flatten(i,j,k)] = tpf;
                  }
              }
          }

        this->threepf_Nderiv_batch.emplace_back(
          std::make_unique<typename integration_items<number>::threepf_Nderiv_item>(
            time_serial, kconfig.serial, source_serial, Nderiv_values, this->time_db_size, this->kconfig_db_size, this->Nfields));

        if(this->paired_batcher != nullptr)
          this->push_paired_threepf(time_serial, t, kconfig, source_serial, value,
                                    tpf_k1_re, tpf_k1_im, tpf_k2_re, tpf_k2_im, tpf_k3_re, tpf_k3_im, bg);

        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_paired_threepf(unsigned int time_serial, double t,
                                                      const threepf_kconfig& kconfig, unsigned int source_serial,
                                                      const std::vector<number>& value,
                                                      const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                                                      const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                                                      const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im, const std::vector<number>& bg)
      {
        assert(this->mdl != nullptr);
        assert(this->parent_task != nullptr);

        number zeta_threepf = 0.0;
        number redbsp = 0.0;

        this->compute_agent.zeta_threepf(kconfig, t, value, tpf_k1_re, tpf_k1_im, tpf_k2_re, tpf_k2_im, tpf_k3_re, tpf_k3_im, bg, zeta_threepf, redbsp,
                                         this->gauge_xfm1, this->gauge_xfm2_123, this->gauge_xfm2_213, this->gauge_xfm2_312);

        this->paired_batcher->push_threepf(time_serial, kconfig.serial, zeta_threepf, redbsp, source_serial);
        this->paired_batcher->push_gauge_xfm2_123(time_serial, kconfig.serial, this->gauge_xfm2_123, source_serial);
        this->paired_batcher->push_gauge_xfm2_213(time_serial, kconfig.serial, this->gauge_xfm2_213, source_serial);
        this->paired_batcher->push_gauge_xfm2_312(time_serial, kconfig.serial, this->gauge_xfm2_312, source_serial);
      }


    template <typename number>
    void threepf_batcher<number>::push_tensor_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                                    const std::vector<number>& value)
	    {
        this->tensor_twopf_batch.emplace_back(
          std::make_unique<typename integration_items<number>::tensor_twopf_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size));
        this->check_for_flush();
	    }


    template <typename number>
    void threepf_batcher<number>::push_tensor_twopf_si(unsigned int time_serial, unsigned int k_serial,
                                                       unsigned int source_serial, const std::vector<number>& value)
      {
        this->tensor_twopf_si_batch.emplace_back(
          std::make_unique<typename integration_items<number>::tensor_twopf_si_item>(
            time_serial, k_serial, source_serial, value, this->time_db_size, this->kconfig_db_size));
        this->check_for_flush();
      }


    template <typename number>
    size_t threepf_batcher<number>::storage() const
	    {
        const auto backg_size = 2 * this->Nfields * sizeof(number);
        constexpr auto tensor_size = 4 * sizeof(number);
        const auto twopf_size = 2 * this->Nfields * 2 * this->Nfields * sizeof(number);
        const auto threepf_size = 2 * this->Nfields * 2 * this->Nfields * 2 * this->Nfields * sizeof(number);
        constexpr auto timing_data_size = sizeof(boost::timer::nanosecond_type);

        return (
          (4 * sizeof(unsigned int) + backg_size) * this->backg_batch.size()
          + (5 * sizeof(unsigned int) + tensor_size) * this->tensor_twopf_batch.size()
          + (5 * sizeof(unsigned int) + tensor_size) * this->tensor_twopf_si_batch.size()
          + (6 * sizeof(unsigned int) + twopf_size) * this->twopf_re_batch.size()
          + (6 * sizeof(unsigned int) + twopf_size) * this->twopf_im_batch.size()
          + (6 * sizeof(unsigned int) + twopf_size) * this->twopf_si_re_batch.size()
          + (6 * sizeof(unsigned int) + threepf_size) * this->threepf_momentum_batch.size()
          + (6 * sizeof(unsigned int) + threepf_size) * this->threepf_Nderiv_batch.size()
          + (2 * sizeof(unsigned int) + sizeof(size_t) + 2 * timing_data_size) * this->stats_batch.size()
          + (sizeof(unsigned int) + backg_size) * this->ics_batch.size()
          + (sizeof(unsigned int) + backg_size) * this->kt_ics_batch.size());
	    }


    template <typename number>
    void threepf_batcher<number>::push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
      {
        if(this->collect_initial_conditions)
          {
            this->ics_batch.emplace_back(std::make_unique<typename integration_items<number>::ics_item>(k_serial, t_exit, values, this->kconfig_db_size, this->Nfields));
            this->check_for_flush();
          }
      }


    template <typename number>
    void threepf_batcher<number>::push_kt_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
	    {
        if(this->collect_initial_conditions)
	        {
            this->kt_ics_batch.emplace_back(std::make_unique<typename integration_items<number>::ics_kt_item>(k_serial, t_exit, values, this->kconfig_db_size, this->Nfields));
            this->check_for_flush();
	        }
	    }


    template <typename number>
    void threepf_batcher<number>::flush(replacement_action action)
	    {
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushing threepf batcher (capacity=" << format_memory(this->capacity) << ") of size " << format_memory(this->storage());

        // set up a timer to measure how long it takes to flush
        boost::timer::cpu_timer flush_timer;

        transaction_manager mgr = this->writers.factory(this);

        this->writers.host_info(mgr, this);
        if(this->collect_statistics) this->writers.stats(mgr, this, this->stats_batch);
		    if(this->collect_initial_conditions) this->writers.ics(mgr, this, this->ics_batch);
		    if(this->collect_initial_conditions) this->writers.kt_ics(mgr, this, this->kt_ics_batch);
        this->writers.backg(mgr, this, this->backg_batch);
        this->writers.twopf_re(mgr, this, this->twopf_re_batch);
        this->writers.twopf_im(mgr, this, this->twopf_im_batch);
        this->writers.twopf_si_re(mgr, this, this->twopf_si_re_batch);
        this->writers.tensor_twopf(mgr, this, this->tensor_twopf_batch);
        this->writers.tensor_twopf_si(mgr, this, this->tensor_twopf_si_batch);
        this->writers.threepf_momentum(mgr, this, this->threepf_momentum_batch);
        this->writers.threepf_Nderiv(mgr, this, this->threepf_Nderiv_batch);

        mgr.commit();

        flush_timer.stop();
        BOOST_LOG_SEV(this->get_log(), generic_batcher::log_severity_level::normal) << "** Flushed in time " << format_time(flush_timer.elapsed().wall) << "; pushing to master process";

        this->stats_batch.clear();
		    this->ics_batch.clear();
		    this->kt_ics_batch.clear();
        this->backg_batch.clear();
        this->twopf_re_batch.clear();
        this->twopf_im_batch.clear();
        this->twopf_si_re_batch.clear();
        this->tensor_twopf_batch.clear();
        this->tensor_twopf_si_batch.clear();
        this->threepf_momentum_batch.clear();
        this->threepf_Nderiv_batch.clear();

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
    void threepf_batcher<number>::unbatch(unsigned int source_serial)
      {
        this->backg_batch.erase(
          std::remove_if(this->backg_batch.begin(), this->backg_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::backg_item>(source_serial)),
          this->backg_batch.end());

        this->twopf_re_batch.erase(
          std::remove_if(this->twopf_re_batch.begin(), this->twopf_re_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::twopf_re_item>(source_serial)),
          this->twopf_re_batch.end());

        this->twopf_im_batch.erase(
          std::remove_if(this->twopf_im_batch.begin(), this->twopf_im_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::twopf_im_item>(source_serial)),
          this->twopf_im_batch.end());

        this->twopf_si_re_batch.erase(
          std::remove_if(this->twopf_si_re_batch.begin(), this->twopf_si_re_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::twopf_si_re_item>(source_serial)),
          this->twopf_si_re_batch.end());

        this->tensor_twopf_batch.erase(
          std::remove_if(this->tensor_twopf_batch.begin(), this->tensor_twopf_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::tensor_twopf_item>(source_serial)),
          this->tensor_twopf_batch.end());

        this->tensor_twopf_si_batch.erase(
          std::remove_if(this->tensor_twopf_si_batch.begin(), this->tensor_twopf_si_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::tensor_twopf_si_item>(source_serial)),
          this->tensor_twopf_si_batch.end());

        this->threepf_momentum_batch.erase(
          std::remove_if(this->threepf_momentum_batch.begin(), this->threepf_momentum_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::threepf_momentum_item>(source_serial)),
          this->threepf_momentum_batch.end());

        this->threepf_Nderiv_batch.erase(
          std::remove_if(this->threepf_Nderiv_batch.begin(), this->threepf_Nderiv_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::threepf_Nderiv_item>(source_serial)),
          this->threepf_Nderiv_batch.end());

        this->ics_batch.erase(
          std::remove_if(this->ics_batch.begin(), this->ics_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::ics_item>(source_serial)),
          this->ics_batch.end());

        this->kt_ics_batch.erase(
          std::remove_if(this->kt_ics_batch.begin(), this->kt_ics_batch.end(),
                         UnbatchPredicate<typename integration_items<number>::ics_kt_item>(source_serial)),
          this->kt_ics_batch.end());

        if(this->paired_batcher != nullptr) this->paired_batcher->unbatch(source_serial);
      }


    template <typename number>
    void threepf_batcher<number>::report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                                             unsigned int kserial, size_t steps, unsigned int refinement)
      {
        this->integration_batcher<number>::report_integration_success(integration, batching, kserial, steps, refinement);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(integration);
      }


    template <typename number>
    void threepf_batcher<number>::report_integration_failure(unsigned int kserial)
      {
        this->integration_batcher<number>::report_integration_failure(kserial);
        if(this->paired_batcher != nullptr) this->paired_batcher->report_finished_item(0);
      }


  }   // namespace transport


#endif //CPPTRANSPORT_INTEGRATION_BATCHER_H
