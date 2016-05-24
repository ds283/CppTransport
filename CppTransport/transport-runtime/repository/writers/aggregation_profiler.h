//
// Created by David Seery on 23/05/2016.
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

#ifndef CPPTRANSPORT_AGGREGATION_PROFILER_H
#define CPPTRANSPORT_AGGREGATION_PROFILER_H


#include <list>
#include <string>
#include <memory>
#include <fstream>

#include "transport-runtime/utilities/formatter.h"

#include "boost/timer/timer.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/optional.hpp"
#include "boost/lexical_cast.hpp"


namespace transport
  {


    enum class aggregation_profile_record_type
      {
        twopf, threepf, zeta_twopf, zeta_threepf, fNL
      };


    namespace aggregation_profiler_impl
      {

        template <aggregation_profile_record_type type> struct record_traits;

        template <>
        struct record_traits<aggregation_profile_record_type::twopf>
          {
            std::string                get_filename() { return "twopf.csv"; }
            std::vector< std::string > get_headings() { return { "size", "attach", "detach", "total", "backg", "twopf_re", "tensor_twopf", "statistics", "ics", "workers" }; }
          };

        template <>
        struct record_traits<aggregation_profile_record_type::threepf>
          {
            std::string                get_filename() { return "threepf.csv"; }
            std::vector< std::string > get_headings() { return { "size", "attach", "detach", "total", "backg", "twopf_re", "twopf_im", "threepf_momentum", "threepf_Nderiv", "tensor_twopf", "statistics", "ics", "ics_kt", "workers" }; }
          };

        template <>
        struct record_traits<aggregation_profile_record_type::zeta_twopf>
          {
            std::string                get_filename() { return "zeta_twopf.csv"; }
            std::vector< std::string > get_headings() { return { "size", "attach", "detach", "total", "twopf", "gauge_xfm1" }; }
          };

        template <>
        struct record_traits<aggregation_profile_record_type::zeta_threepf>
          {
            std::string                get_filename() { return "zeta_threepf.csv"; }
            std::vector< std::string > get_headings() { return { "size", "attach", "detach", "total", "twopf", "threepf", "gauge_xfm1", "gauge_xfm2_123", "gauge_xfm2_213", "gauge_xfm2_312" }; }
          };

        template <>
        struct record_traits<aggregation_profile_record_type::fNL>
          {
            std::string                get_filename() { return "fNL.csv"; }
            std::vector< std::string > get_headings() { return { "size", "attach", "detach", "total", "fNL" }; }
          };


        template <typename ValueType>
        std::string format(const boost::optional<ValueType>& v)
          {
            if(v)
              {
                return boost::lexical_cast<std::string>(*v);
              }
            else
              {
                return "NaN";
              }
          }


      }   // namespace aggregation_profiler_impl


    class aggregation_profile_record
      {
      public:
        aggregation_profile_record(const boost::filesystem::path& p);
        virtual ~aggregation_profile_record() = default;

      public:
        virtual aggregation_profile_record_type get_type() const = 0;
        virtual void write_row(std::ofstream& out) const;
        void stop();

      public:
        boost::optional< boost::uintmax_t > database_size;
        boost::optional< boost::timer::nanosecond_type > attach_time;
        boost::optional< boost::timer::nanosecond_type > detach_time;
        boost::optional< boost::timer::nanosecond_type > total_time;

      private:
        boost::timer::cpu_timer timer;
      };


    aggregation_profile_record::aggregation_profile_record(const boost::filesystem::path& p)
      {
        if(boost::filesystem::exists(p) && boost::filesystem::is_regular_file(p))
          {
            this->database_size = boost::filesystem::file_size(p);
          }
      }


    void aggregation_profile_record::stop()
      {
        this->timer.stop();
        this->total_time = this->timer.elapsed().wall;
      }


    void aggregation_profile_record::write_row(std::ofstream& out) const
      {
        out << aggregation_profiler_impl::format(this->database_size)
            << "," << aggregation_profiler_impl::format(this->attach_time)
            << "," << aggregation_profiler_impl::format(this->detach_time)
            << "," << aggregation_profiler_impl::format(this->total_time);
        // newline must be supplied by implementations
      }


    class twopf_aggregation_profile_record: public aggregation_profile_record
      {
      public:
        twopf_aggregation_profile_record(const boost::filesystem::path& p)
          : aggregation_profile_record(p)
          {
          }

      public:
        aggregation_profile_record_type get_type() const override { return aggregation_profile_record_type::twopf; }
        void write_row(std::ofstream& out) const override;

      public:
        boost::optional< boost::timer::nanosecond_type > backg;
        boost::optional< boost::timer::nanosecond_type > twopf_re;
        boost::optional< boost::timer::nanosecond_type > tensor_twopf;
        boost::optional< boost::timer::nanosecond_type > statistics;
        boost::optional< boost::timer::nanosecond_type > ics;
        boost::optional< boost::timer::nanosecond_type > workers;
      };


    void twopf_aggregation_profile_record::write_row(std::ofstream& out) const
      {
        this->aggregation_profile_record::write_row(out);
        out << "," << aggregation_profiler_impl::format(this->backg)
            << "," << aggregation_profiler_impl::format(this->twopf_re)
            << "," << aggregation_profiler_impl::format(this->tensor_twopf)
            << "," << aggregation_profiler_impl::format(this->statistics)
            << "," << aggregation_profiler_impl::format(this->ics)
            << "," << aggregation_profiler_impl::format(this->workers)
            << '\n';
      }


    class threepf_aggregation_profile_record: public aggregation_profile_record
      {
      public:
        threepf_aggregation_profile_record(const boost::filesystem::path& p)
          : aggregation_profile_record(p)
          {
          }

      public:
        aggregation_profile_record_type get_type() const override { return aggregation_profile_record_type::threepf; }
        void write_row(std::ofstream& out) const override;

      public:
        boost::optional< boost::timer::nanosecond_type > backg;
        boost::optional< boost::timer::nanosecond_type > twopf_re;
        boost::optional< boost::timer::nanosecond_type > twopf_im;
        boost::optional< boost::timer::nanosecond_type > threepf_momentum;
        boost::optional< boost::timer::nanosecond_type > threepf_Nderiv;
        boost::optional< boost::timer::nanosecond_type > tensor_twopf;
        boost::optional< boost::timer::nanosecond_type > statistics;
        boost::optional< boost::timer::nanosecond_type > ics;
        boost::optional< boost::timer::nanosecond_type > ics_kt;
        boost::optional< boost::timer::nanosecond_type > workers;
      };


    void threepf_aggregation_profile_record::write_row(std::ofstream& out) const
      {
        this->aggregation_profile_record::write_row(out);
        out << "," << aggregation_profiler_impl::format(this->backg)
            << "," << aggregation_profiler_impl::format(this->twopf_re)
            << "," << aggregation_profiler_impl::format(this->twopf_im)
            << "," << aggregation_profiler_impl::format(this->threepf_momentum)
            << "," << aggregation_profiler_impl::format(this->threepf_Nderiv)
            << "," << aggregation_profiler_impl::format(this->tensor_twopf)
            << "," << aggregation_profiler_impl::format(this->statistics)
            << "," << aggregation_profiler_impl::format(this->ics)
            << "," << aggregation_profiler_impl::format(this->ics_kt)
            << "," << aggregation_profiler_impl::format(this->workers)
            << '\n';
      }


    class zeta_twopf_aggregation_profile_record: public aggregation_profile_record
      {
      public:
        zeta_twopf_aggregation_profile_record(const boost::filesystem::path& p)
          : aggregation_profile_record(p)
          {
          }

      public:
        aggregation_profile_record_type get_type() const override { return aggregation_profile_record_type::zeta_twopf; }
        void write_row(std::ofstream& out) const override;

      public:
        boost::optional< boost::timer::nanosecond_type > twopf;
        boost::optional< boost::timer::nanosecond_type > gauge_xfm1;
      };


    void zeta_twopf_aggregation_profile_record::write_row(std::ofstream& out) const
      {
        this->aggregation_profile_record::write_row(out);
        out << "," << aggregation_profiler_impl::format(this->twopf)
            << "," << aggregation_profiler_impl::format(this->gauge_xfm1)
            << '\n';
      }


    class zeta_threepf_aggregation_profile_record: public aggregation_profile_record
      {
      public:
        zeta_threepf_aggregation_profile_record(const boost::filesystem::path& p)
          : aggregation_profile_record(p)
          {
          }

      public:
        aggregation_profile_record_type get_type() const override { return aggregation_profile_record_type::zeta_threepf; }
        void write_row(std::ofstream& out) const override;

      public:
        boost::optional< boost::timer::nanosecond_type > twopf;
        boost::optional< boost::timer::nanosecond_type > threepf;
        boost::optional< boost::timer::nanosecond_type > gauge_xfm1;
        boost::optional< boost::timer::nanosecond_type > gauge_xfm2_123;
        boost::optional< boost::timer::nanosecond_type > gauge_xfm2_213;
        boost::optional< boost::timer::nanosecond_type > gauge_xfm2_312;
      };


    void zeta_threepf_aggregation_profile_record::write_row(std::ofstream& out) const
      {
        this->aggregation_profile_record::write_row(out);
        out << "," << aggregation_profiler_impl::format(this->twopf)
            << "," << aggregation_profiler_impl::format(this->threepf)
            << "," << aggregation_profiler_impl::format(this->gauge_xfm1)
            << "," << aggregation_profiler_impl::format(this->gauge_xfm2_123)
            << "," << aggregation_profiler_impl::format(this->gauge_xfm2_213)
            << "," << aggregation_profiler_impl::format(this->gauge_xfm2_312)
            << '\n';
      }


    class fNL_aggregation_profile_record: public aggregation_profile_record
      {
      public:
        fNL_aggregation_profile_record(const boost::filesystem::path& p)
          : aggregation_profile_record(p)
          {
          }

      public:
        aggregation_profile_record_type get_type() const override { return aggregation_profile_record_type::fNL; }
        void write_row(std::ofstream& out) const override;

      public:
        boost::optional< boost::timer::nanosecond_type > fNL;
      };


    void fNL_aggregation_profile_record::write_row(std::ofstream& out) const
      {
        this->aggregation_profile_record::write_row(out);
        out << "," << aggregation_profiler_impl::format(this->fNL)
            << '\n';
      }


    class aggregation_profiler
      {

        // CONSTRUCTOR, DESTRUCTOR

      public:

        //! constructor
        aggregation_profiler(std::string n)
          : group_name(std::move(n))
          {
          }


        // WRITE DETAILS TO THE PROFILER

      public:

        //! add a record
        aggregation_profiler& add_record(std::unique_ptr< aggregation_profile_record > r);

        //! write records to a named root folder
        void write_to_csv(const boost::filesystem::path& root) const;

      private:

        //! write records of a specific type to
        template <aggregation_profile_record_type type>
        void write_csv(const boost::filesystem::path& folder) const;


        // INTERNAL DATA

      private:

        //! name of task we are profiling
        std::string group_name;

        // LIST OF TIMES

        //! records: profiles of individual aggregation events
        std::list< std::unique_ptr< aggregation_profile_record > > events_db;

      };


    aggregation_profiler& aggregation_profiler::add_record(std::unique_ptr< aggregation_profile_record > r)
      {
        this->events_db.push_back(std::move(r));
        return *this;
      }


    void aggregation_profiler::write_to_csv(const boost::filesystem::path& root) const
      {
        boost::filesystem::path folder = root / this->group_name;
        if(!boost::filesystem::exists(folder)) boost::filesystem::create_directories(folder);

        unsigned int twopf = 0;
        unsigned int threepf = 0;
        unsigned int zeta_twopf = 0;
        unsigned int zeta_threepf = 0;
        unsigned int fNL = 0;
        for(const std::unique_ptr< aggregation_profile_record >& rec : this->events_db)
          {
            switch(rec->get_type())
              {
                case aggregation_profile_record_type::twopf:
                  {
                    ++twopf;
                    break;
                  }

                case aggregation_profile_record_type::threepf:
                  {
                    ++threepf;
                    break;
                  }

                case aggregation_profile_record_type::zeta_twopf:
                  {
                    ++zeta_twopf;
                    break;
                  }

                case aggregation_profile_record_type::zeta_threepf:
                  {
                    ++zeta_threepf;
                    break;
                  }

                case aggregation_profile_record_type::fNL:
                  {
                    ++fNL;
                    break;
                  }
              }
          }

        if(twopf > 0)        this->write_csv<aggregation_profile_record_type::twopf>(folder);
        if(threepf > 0)      this->write_csv<aggregation_profile_record_type::threepf>(folder);
        if(zeta_twopf > 0)   this->write_csv<aggregation_profile_record_type::zeta_twopf>(folder);
        if(zeta_threepf > 0) this->write_csv<aggregation_profile_record_type::zeta_threepf>(folder);
        if(fNL > 0)          this->write_csv<aggregation_profile_record_type::fNL>(folder);
      }


    template <aggregation_profile_record_type type>
    void aggregation_profiler::write_csv(const boost::filesystem::path& folder) const
      {
        boost::filesystem::path file = folder / aggregation_profiler_impl::record_traits<type>().get_filename();
        std::ofstream out(file.string(), std::ios::out | std::ios::trunc);

        auto headings = aggregation_profiler_impl::record_traits<type>().get_headings();
        unsigned int count = 0;
        for(const std::string& col_title : headings)
          {
            if(count > 0) out << ",";
            out << col_title;
            ++count;
          }
        out << '\n';

        for(const std::unique_ptr< aggregation_profile_record >& rec : this->events_db)
          {
            if(rec->get_type() == type)
              {
                rec->write_row(out);
              }
          }

        out.close();
      }


  }   // namespace transport


#endif //CPPTRANSPORT_AGGREGATION_PROFILER_H
