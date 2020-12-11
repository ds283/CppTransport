// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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
// @contributor: Sean Butchers <smlb20@sussex.ac.uk>
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// @contributor: Alessandro Maraio <am963@sussex.ac.uk>
// --@@


#include <utility>
#include <vector>
#include <list>
#include <functional>
#include <memory>

#include "boost/filesystem/operations.hpp"
#include "boost/timer/timer.hpp"
#include "boost/log/core.hpp"
#include "boost/log/trivial.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/sources/severity_feature.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/attributes.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

#include "transport-runtime/enumerations.h"
#include "transport-runtime/data/batchers/integration_items.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/data/batchers/postprocess_delegate.h"
#include "transport-runtime/tasks/task_configurations.h"

// START OF BASIC BATCHER METHODS

namespace transport
  {

    class basic_batcher
      {
        // Enum definitions needed for logging
      public:
        //! Logging severity level
        enum class log_severity_level
          {
            datapipe_pull,
            normal,
            warning,
            error,
            critical
          };

        //! logging sink
        typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> sink_t;

        //! logging source
        typedef boost::log::sources::severity_logger<log_severity_level> logger;

        // CONSTRUCTOR, DESTRUCTOR
      public:
        //! constructor
        basic_batcher(boost::filesystem::path lp, unsigned int w, unsigned int g = 0, bool no_log = true);

        //! move constructor
        basic_batcher(basic_batcher&&) = default;

        ~basic_batcher() = default;

        // LOGGING FUNCTION
      public:
        //! Return logger
        logger& get_log() { return (this->log_source); }


      protected:
        //! cache number of fields associated with this integration
        //    const unsigned int Nfields;

        //! Log directory path
        boost::filesystem::path logdir_path;

        //! Worker group associated with this batcher;
        //! usually zero unless we are doing parallel batching.
        //! Later, groups identify different integrations which have been chained together
        unsigned int worker_group;

        //! Worker number associated with this batcher
        unsigned int worker_number;

        // OTHER INTERNAL DATA
        // LOGGING
        //! Logger source
        boost::log::sources::severity_logger<basic_batcher::log_severity_level> log_source;

        //! Logger sink; note we are forced to use boost::shared_ptr<> because this
        //! is what the Boost.Log API expects
        boost::shared_ptr<sink_t> log_sink;
      };


// overload << to push log_severity_level to stream
    std::ostream& operator<<(std::ostream& stream, basic_batcher::log_severity_level level)
      {
        static const std::map<basic_batcher::log_severity_level, std::string> stringize_map =
          {
            {basic_batcher::log_severity_level::datapipe_pull, "datapipe"},
            {basic_batcher::log_severity_level::normal,        "normal"},
            {basic_batcher::log_severity_level::warning,       "warning"},
            {basic_batcher::log_severity_level::error,         "error"},
            {basic_batcher::log_severity_level::critical,      "CRITICAL"}};

        stream << stringize_map.at(level);

        return stream;
      }


// overload << to push log_severity_level to Boost.Log log
    struct sampling_batcher_severity_tag;


    boost::log::formatting_ostream& operator<<(boost::log::formatting_ostream& stream,
                                               const boost::log::to_log_manip<basic_batcher::log_severity_level, sampling_batcher_severity_tag> manip)
      {
        static const std::map<basic_batcher::log_severity_level, std::string> stringize_map =
          {
            {basic_batcher::log_severity_level::datapipe_pull, "data"},
            {basic_batcher::log_severity_level::normal,        "info"},
            {basic_batcher::log_severity_level::warning,       "warn"},
            {basic_batcher::log_severity_level::error,         "err "},
            {basic_batcher::log_severity_level::critical,      "CRIT"}};

        basic_batcher::log_severity_level level = manip.get();
        stream << stringize_map.at(level);

        return stream;
      }


// GENERIC BATCHER METHODS
// CONSTRUCTOR, DESTRUCTOR
    basic_batcher::basic_batcher(boost::filesystem::path lp, unsigned int w, unsigned int g, bool no_log)
      : logdir_path(std::move(lp)),
        worker_group(g),
        worker_number(w)
      {
        // set up logging
        std::ostringstream log_file;
        log_file << CPPTRANSPORT_LOG_FILENAME_A << worker_number << CPPTRANSPORT_LOG_FILENAME_B;

        boost::filesystem::path log_path = logdir_path / log_file.str();

        if(!no_log)
          {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();

            //        core->set_filter(boost::log::trivial::severity >= normal);

            boost::shared_ptr<boost::log::sinks::text_file_backend> backend =
              boost::make_shared<boost::log::sinks::text_file_backend>(
                boost::log::keywords::file_name = log_path.string(),
                boost::log::keywords::open_mode = std::ios::app);

            // enable auto-flushing of log entries
            // this degrades performance, but we are not writing many entries and they
            // will not be lost in the event of a crash
            backend->auto_flush(true);

            // Wrap it into the frontend and register in the core.
            // The backend requires synchronization in the frontend.
            this->log_sink = boost::make_shared<sink_t>(backend);
            this->log_sink->set_formatter(
              boost::log::expressions::stream
                << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
                << " | "
                << boost::log::expressions::attr<basic_batcher::log_severity_level, sampling_batcher_severity_tag>(
                  "Severity")
                << " | "
                << boost::log::expressions::smessage);

            core->add_sink(this->log_sink);

            boost::log::add_common_attributes();
          }
        else // if asking for no-logging, print *nothing* to the terminal -> speeds up sampling
          {
            boost::shared_ptr<boost::log::core> core = boost::log::core::get();
            core->set_logging_enabled(false);
          }
      }

// END OF BASIC BATCHER METHODS

    template <typename number>
    class sampling_integration_batcher;

    template <typename number>
    class sampling_integration_batcher: public basic_batcher
      {
        // CONSTRUCTOR, DESTRUCTOR
      public:
        //! constructor
        sampling_integration_batcher(const boost::filesystem::path& lp, unsigned int w,
                                     model<number>* m, integration_task<number>* tk,
                                     unsigned int g = 0, bool no_log = true);

        //! move constructor
        sampling_integration_batcher(sampling_integration_batcher<number>&&) = default;

        ~sampling_integration_batcher() = default;

        // BATCHER INFORMATION - default batcher functions needed in CppT interface
      public:
        bool is_collecting_initial_conditions()
          { return false; }


        void push_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
          {
            return;
          }


        void
        report_integration_success(boost::timer::nanosecond_type integration, boost::timer::nanosecond_type batching,
                                   unsigned int kserial, size_t steps, unsigned int refinement)
          {
            return;
          }
      };


// DECLARE TWOPF_BATCHER AND METHODS/DATA
    template <typename number>
    class twopf_sampling_batcher: public sampling_integration_batcher<number>
      {
      public:
        //! Constructor
        twopf_sampling_batcher(std::vector<number>& dt_twopf, std::vector<number>& dt_twopf_si,
                               std::vector<number>& dt_tenspf, std::vector<number>& dt_tenspf_si,
                               const boost::filesystem::path& lp, unsigned int w,
                               model<number>* m, twopf_task<number>* tk,
                               unsigned int g = 0, bool no_log = true);

        //! move constructor
        twopf_sampling_batcher(twopf_sampling_batcher<number>&&) = default;

        //! destructor is default
        virtual ~twopf_sampling_batcher() = default;

        //! BATCHERS
        void push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values);

        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                        const std::vector<number>& value, const std::vector<number>& value_si,
                        const std::vector<number>& backg);

        void push_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                           const std::vector<number>& value, const std::vector<number>& backg);

        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial,
                               unsigned int source_serial, const std::vector<number>& value);

        void push_tensor_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                  const std::vector<number>& value);

        // INTERNAL DATA
      private:
        //! std::vector for collecting the zeta-twopf samples in
        std::vector<number>& zeta_twopf_data;

        //! std::vector for collecting the zeta-twopf spectral index sample in
        std::vector<number>& zeta_twopf_spectral_data;

        //! std::vector for collecting the tensor-twopf samples in
        std::vector<number>& tensor_twopf_data;

        //! std::vector for collecting the tensor-twopf spectral index sample in
        std::vector<number>& tensor_twopf_spectral_data;

      protected:
        //! cache number of fields associated with this integration
        const unsigned int Nfields;

        // COMPUTATION AGENT
        // compute delegate
        postprocess_delegate<number> compute_agent;

        //! cache for linear part of gauge transformation
        std::vector<number> gauge_xfm1;
      };


// DECLARE THREEPF_BATCHER AND METHODS/DATA
    template <typename number>
    class threepf_sampling_batcher: public sampling_integration_batcher<number>
      {
      public:
        //! Constructor
        threepf_sampling_batcher(std::vector<number>& dt_twopf, std::vector<number>& dt_twopf_si,
                                 std::vector<number>& dt_tenspf, std::vector<number>& dt_tenspf_si,
                                 std::vector<number>& dt_thrpf, std::vector<number>& dt_redbsp,
                                 const boost::filesystem::path& lp, unsigned int w, model<number>* m,
                                 threepf_task<number>* tk, unsigned int g = 0, bool no_log = true);

        //! move constructor
        threepf_sampling_batcher(threepf_sampling_batcher<number>&&) = default;

        //! destructor is default
        virtual ~threepf_sampling_batcher() = default;

        //! BATCHERS
        // Push the background
        void push_backg(unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values);


        // Push a set of initial condition
        void push_kt_ics(unsigned int k_serial, double t_exit, const std::vector<number>& values)
          {
            return;
          }


        // Functions giving batches needed for different n-point functions
        void push_twopf(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                        const std::vector<number>& value, const std::vector<number>& value_Si,
                        const std::vector<number>& backg, twopf_type t);

        void push_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                           const std::vector<number>& value, const std::vector<number>& backg);

        void push_tensor_twopf(unsigned int time_serial, unsigned int k_serial,
                               unsigned int source_serial, const std::vector<number>& value);

        void push_tensor_twopf_si(unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
                                  const std::vector<number>& value);

        void push_threepf(unsigned int time_serial, double t,
                          const threepf_kconfig& kconfig, unsigned int source_serial,
                          const std::vector<number>& threepf,
                          const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
                          const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
                          const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im,
                          const std::vector<number>& bg);

        // INTERNAL DATA
      private:
        //! std::vector for collecting the zeta-twopf samples in
        std::vector<number>& zeta_twopf_data;

        //! std::vector for collecting the zeta-twopf spectral index sample in
        std::vector<number>& zeta_twopf_spectral_data;

        //! std::vector for collecting the tensor-twopf samples in
        std::vector<number>& tensor_twopf_data;

        //! std::vector for collecting the tensor-twopf spectral index sample in
        std::vector<number>& tensor_twopf_spectral_data;

        //! std::vector for collecting the zeta-threepf samples in
        std::vector<number>& zeta_threepf_data;

        //! std::vector for collecting the reduced bispectrum samples in
        std::vector<number>& redbsp_data;

      protected:
        //! cache number of fields associated with this integration
        const unsigned int Nfields;

        // COMPUTATION AGENT
        // compute delegate
        postprocess_delegate<number> compute_agent;

        //! cache for linear part of gauge transformation
        std::vector<number> gauge_xfm1;

        //! cache for quadratic part of gauge transformation, 123 permutation
        std::vector<number> gauge_xfm2_123;

        //! cache for quadratic part of gauge transformation, 213 permutation
        std::vector<number> gauge_xfm2_213;

        //! cache for quadratic part of gauge transformation, 312 permutation
        std::vector<number> gauge_xfm2_312;
      };


// GENERIC BATCHER METHODS
// CONSTRUCTOR, DESTRUCTOR
    template <typename number>
    sampling_integration_batcher<number>::sampling_integration_batcher
    (const boost::filesystem::path& lp, unsigned int w, model<number>* m,
     integration_task<number>* tk, unsigned int g, bool no_log)
      : basic_batcher(lp, w, g, no_log)
      {
      }


//! TWOPF_SAMPLING METHODS
// CONSTRUCTOR
    template <typename number>
    twopf_sampling_batcher<number>::twopf_sampling_batcher
      (std::vector<number>& dt_twopf, std::vector<number>& dt_twopf_si,
       std::vector<number>& dt_tenspf, std::vector<number>& dt_tenspf_si,
       const boost::filesystem::path& lp, unsigned int w, model<number>* m,
       twopf_task<number>* tk, unsigned int g, bool no_log)
      : sampling_integration_batcher<number>(lp, w, m, tk, g, no_log),
        zeta_twopf_data(dt_twopf),
        zeta_twopf_spectral_data(dt_twopf_si),
        tensor_twopf_data(dt_tenspf),
        tensor_twopf_spectral_data(dt_tenspf_si),
        Nfields(m->get_N_fields()),
        compute_agent(m, tk)
      {
        // Ensure the std::vector for the 1st-order gauge transform has the correct dimensions
        gauge_xfm1.resize(2 * this->Nfields);
      }


// PUSH_BACKG
    template <typename number>
    void
    twopf_sampling_batcher<number>::push_backg
      (unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
      {
        return;
      }


// PUSH_TWOPF
    template <typename number>
    void
    twopf_sampling_batcher<number>::push_twopf
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value, const std::vector<number>& value_si, const std::vector<number>& backg)
      {
        number zeta_twopf = 0.0;
        number ns = 0.0;
        this->compute_agent.zeta_twopf(value, value_si, backg, zeta_twopf, ns, this->gauge_xfm1, false);
        this->zeta_twopf_data.push_back(zeta_twopf);
        this->zeta_twopf_spectral_data.push_back(ns);
      }


// PUSH_TWOPF SPECTRAL INDEX DATA
    template <typename number>
    void
    twopf_sampling_batcher<number>::push_twopf_si
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value, const std::vector<number>& backg)
      {
        return;
      }


// PUSH_TENSOR_TWOPF
    template <typename number>
    void
    twopf_sampling_batcher<number>::push_tensor_twopf
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value)
      {
        number tensor_twopf = 4.0 * value[0]; // 4.0 appears because of tensor polarisations
        this->tensor_twopf_data.emplace_back(tensor_twopf);
      }


// PUSH_TENSOR_TWOPF_SI
    template <typename number>
    void
    twopf_sampling_batcher<number>::push_tensor_twopf_si
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value)
      {
        // assumes corresponding tensor twopf value has already been emplaced
        number nt = 3.0 + (4.0*value[0]) / this->tensor_twopf_data.back();
        this->tensor_twopf_spectral_data.emplace_back(nt);
      }


//! THREEPF_SAMPLING METHODS
// CONSTRUCTOR
    template <typename number>
    threepf_sampling_batcher<number>::threepf_sampling_batcher
      (std::vector<number>& dt_twopf, std::vector<number>& dt_twopf_si,
       std::vector<number>& dt_tenspf, std::vector<number>& dt_tenspf_si,
       std::vector<number>& dt_thrpf, std::vector<number>& dt_redbsp,
       const boost::filesystem::path& lp, unsigned int w,
       model<number>* m, threepf_task<number>* tk, unsigned int g, bool no_log)
      : sampling_integration_batcher<number>(lp, w, m, tk, g, no_log),
        zeta_twopf_data(dt_twopf),
        zeta_twopf_spectral_data(dt_twopf_si),
        tensor_twopf_data(dt_tenspf),
        tensor_twopf_spectral_data(dt_tenspf_si),
        zeta_threepf_data(dt_thrpf),
        redbsp_data(dt_redbsp),
        Nfields(m->get_N_fields()),
        compute_agent(m, tk)
      {
        // Ensure the std::vector for the 1st-order gauge transform has the correct dimensions
        gauge_xfm1.resize(2 * this->Nfields);

        // Ensure the std::vectors for the quadratic gauge transformations have the correct dimensions
        gauge_xfm2_123.resize(2 * this->Nfields * 2 * this->Nfields);
        gauge_xfm2_213.resize(2 * this->Nfields * 2 * this->Nfields);
        gauge_xfm2_312.resize(2 * this->Nfields * 2 * this->Nfields);
      }


// PUSH_BACKG
    template <typename number>
    void
    threepf_sampling_batcher<number>::push_backg
      (unsigned int time_serial, unsigned int source_serial, const std::vector<number>& values)
      {
        return;
      }


// PUSH_TWOPF
    template <typename number>
    void
    threepf_sampling_batcher<number>::push_twopf
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value, const std::vector<number>& value_si,
       const std::vector<number>& backg, twopf_type t)
      {
        number zeta_twopf = 0.0;
        number ns = 0.0;
        this->compute_agent.zeta_twopf(value, value_si, backg, zeta_twopf, ns, this->gauge_xfm1, false);
        this->zeta_twopf_data.push_back(zeta_twopf);
        this->zeta_twopf_spectral_data.push_back(ns);
      }


// PUSH_TWOPF_SI
    template <typename number>
    void
    threepf_sampling_batcher<number>::push_twopf_si
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value, const std::vector<number>& backg)
      {
      }


// PUSH_TENSOR_TWOPF
    template <typename number>
    void
    threepf_sampling_batcher<number>::push_tensor_twopf
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value)
      {
        number tensor_twopf = 4.0 * value[0]; // 4.0 appears because of tensor polarisations
        this->tensor_twopf_data.emplace_back(tensor_twopf);
      }


// PUSH_TENSOR_TWOPF_SI
    template <typename number>
    void
    threepf_sampling_batcher<number>::push_tensor_twopf_si
      (unsigned int time_serial, unsigned int k_serial, unsigned int source_serial,
       const std::vector<number>& value)
      {
        // assumes corresponding tensor twopf value has already been emplaced
        number nt = 3.0 + (4.0*value[0]) / this->tensor_twopf_data.back();
        this->tensor_twopf_spectral_data.emplace_back(nt);
      }


// PUSH_THREEPF
    template <typename number>
    void
    threepf_sampling_batcher<number>::push_threepf
      (unsigned int time_serial, double t, const threepf_kconfig& kconfig,
       unsigned int source_serial, const std::vector<number>& threepf,
       const std::vector<number>& tpf_k1_re, const std::vector<number>& tpf_k1_im,
       const std::vector<number>& tpf_k2_re, const std::vector<number>& tpf_k2_im,
       const std::vector<number>& tpf_k3_re, const std::vector<number>& tpf_k3_im,
       const std::vector<number>& bg)
      {
        number zeta_threepf = 0.0;
        number redbsp = 0.0;

        this->compute_agent.zeta_threepf(kconfig, t, threepf, tpf_k1_re, tpf_k1_im, tpf_k2_re, tpf_k2_im, tpf_k3_re,
                                         tpf_k3_im, bg, zeta_threepf, redbsp,
                                         this->gauge_xfm1, this->gauge_xfm2_123, this->gauge_xfm2_213,
                                         this->gauge_xfm2_312);

        this->zeta_threepf_data.push_back(zeta_threepf);
        this->redbsp_data.push_back(redbsp);
      }

  }   // namespace transport
