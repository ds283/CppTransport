//
// Created by David Seery on 25/01/2016.
// Copyright (c) 2016 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_DATA_MANAGER_IMPL_H
#define CPPTRANSPORT_DATA_MANAGER_IMPL_H


namespace transport
  {


    // INTEGRITY CHECK


    namespace integrity_check_impl
      {

        template <typename ConfigurationType>
        class ConfigurationFinder
          {
          public:
            ConfigurationFinder(unsigned int s)
              : serial(s)
              {
              }


            bool operator()(const ConfigurationType& a)
              {
                return (a.serial == this->serial);
              }


          private:
            unsigned int serial;
          };


        template <typename RecordData>
        class RecordFinder
          {
          public:
            RecordFinder(unsigned int s)
              : serial(s)
              {
              }


            bool operator()(const RecordData& a)
              {
                return ((*a).serial == this->serial);
              }


          private:
            unsigned int serial;
          };

      }   // namespace integrity_check_impl


    // pull in namespace integrity_check_impl for this scope
    using namespace integrity_check_impl;


    template <typename number>
    template <typename WriterObject, typename Database>
    std::list<unsigned int> data_manager<number>::advise_missing_content(WriterObject& writer, const std::list<unsigned int>& serials, const Database& db)
      {
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Detected missing data in container";
        writer.set_fail(true);

        std::list<unsigned int> advised_list = writer.get_failed_serials();
        if(advised_list.size() > 0) BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Note: backend provided list of " << advised_list.size() << " missing items to cross-check";

        for(typename std::list<unsigned int>::const_iterator t = serials.begin(); t != serials.end(); ++t)
          {
            // find this configuration
            typename Database::const_config_iterator u = std::find_if(db.config_begin(), db.config_end(),
                                                                      ConfigurationFinder<typename Database::const_config_iterator::type>(*t));

            // emit configuration information
            std::ostringstream msg;
            msg << *u;
            std::string msg_str = msg.str();
            boost::algorithm::trim_right(msg_str);
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** " << msg_str;

            // search for this element in the advised list
            std::list<unsigned int>::iterator ad = std::find(advised_list.begin(), advised_list.end(), *t);

            // was this an item on the list we already knew would be missing?
            if(ad != advised_list.end()) advised_list.erase(ad);
          }

        // return any remainder
        return (advised_list);
      }


    template <typename number>
    std::list<unsigned int> data_manager<number>::compute_twopf_drop_list(const std::list<unsigned int>& serials, const threepf_kconfig_database& threepf_db)
      {
        std::list<unsigned int> drop_serials;

        // TODO: this is a O(N^2) algorithm; it would be nice if it could be replaced with something better
        for(std::list<unsigned int>::const_iterator t = serials.begin(); t != serials.end(); ++t)
          {
            threepf_kconfig_database::const_record_iterator u = std::find_if(threepf_db.record_begin(), threepf_db.record_end(),
                                                                             RecordFinder<threepf_kconfig_database::const_record_iterator::type>(*t));

            if(u != threepf_db.record_end())
              {
                if(u->is_twopf_k1_stored()) drop_serials.push_back((*u)->k1_serial);
                if(u->is_twopf_k2_stored()) drop_serials.push_back((*u)->k2_serial);
                if(u->is_twopf_k3_stored()) drop_serials.push_back((*u)->k3_serial);
              }
          }

        drop_serials.sort();
        drop_serials.unique();

        return (drop_serials);
      }


    template <typename number>
    std::list<unsigned int> data_manager<number>::map_twopf_to_threepf_serials(const std::list<unsigned int>& twopf_list, const threepf_kconfig_database& threepf_db)
      {
        std::list<unsigned int> threepf_list;

        // TODO: this is a O(N^2) algorithm; it would be nice if it could be replaced with something better
        for(std::list<unsigned int>::const_iterator t = twopf_list.begin(); t != twopf_list.end(); ++t)
          {
            for(threepf_kconfig_database::const_record_iterator u = threepf_db.record_begin(); u != threepf_db.record_end(); ++u)
              {
                if(u->is_twopf_k1_stored() && (*u)->k1_serial == *t)
                  {
                    threepf_list.push_back((*u)->serial);
                    break;
                  }
                if(u->is_twopf_k2_stored() && (*u)->k2_serial == *t)
                  {
                    threepf_list.push_back((*u)->serial);
                    break;
                  }
                if(u->is_twopf_k3_stored() && (*u)->k3_serial == *t)
                  {
                    threepf_list.push_back((*u)->serial);
                    break;
                  }
              }
          }

        threepf_list.sort();
        threepf_list.unique();

        return (threepf_list);
      }


    template <typename number>
    void data_manager<number>::check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        twopf_task<number>* tk = dynamic_cast< twopf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for twopf container '" << writer.get_abs_container_path().string() << "'";

        std::list<unsigned int> serials = this->get_missing_twopf_re_serials(writer);

        if(serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, serials, tk->get_twopf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra configurations not missing from container, but advised by backend:";
                this->drop_twopf_re_configurations(writer, remainder, tk->get_twopf_database());
                serials.merge(remainder);
              }

            // push list of missing serial numbers to writer and mark as a fail
            writer.set_missing_serials(serials);

            if(writer.is_collecting_statistics()) this->drop_statistics_configurations(writer, serials, tk->get_twopf_database());
            if(writer.is_collecting_initial_conditions()) this->drop_initial_conditions_configurations(writer, serials, tk->get_twopf_database());
          }
      }


    template <typename number>
    void data_manager<number>::check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        threepf_task<number>* tk = dynamic_cast< threepf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for threepf container '" << writer.get_abs_container_path().string() << "'";

        // get lists of missing serial numbers for threepf, real twopf and imaginary twopf
        std::list<unsigned int> twopf_re_serials = this->get_missing_twopf_re_serials(writer);
        std::list<unsigned int> twopf_im_serials = this->get_missing_twopf_im_serials(writer);
        std::list<unsigned int> threepf_serials  = this->get_missing_threepf_serials(writer);

        // merge missing twopf lists into a single one
        std::list<unsigned int> twopf_total_serials = twopf_re_serials;
        std::list<unsigned int> temp = twopf_im_serials;
        twopf_total_serials.merge(temp);
        twopf_total_serials.unique();

        // map missing twopf serials into threepf serials
        std::list<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_total_serials, tk->get_threepf_database());

        // advise missing threepf serials
        if(threepf_serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, threepf_serials, tk->get_threepf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra threepf configurations not missing from container, but advised by backend:";
                this->drop_threepf_configurations(writer, remainder, tk->get_threepf_database());
                threepf_serials.merge(remainder);   // not necessary to remove duplicates, since there should not be any; result is sorted
              }
          }

        // check if any twopf configurations require dropping even more threepfs
        std::list<unsigned int> undropped;
        std::set_difference(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end(),
                            threepf_serials.begin(), threepf_serials.end(), std::back_inserter(undropped));

        if(undropped.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra threepf configurations not missing from container, but implied by missing twopf configurations:";
            this->drop_threepf_configurations(writer, undropped, tk->get_threepf_database());
            threepf_serials.merge(undropped);   // not necessary to remove duplicates, since there should be any; result is sorted
          }

        if(threepf_serials.size() > 0)
          {
            writer.set_missing_serials(threepf_serials);

            if(writer.is_collecting_statistics())         this->drop_statistics_configurations(writer, threepf_serials, tk->get_threepf_database());
            if(writer.is_collecting_initial_conditions()) this->drop_initial_conditions_configurations(writer, threepf_serials, tk->get_threepf_database());

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::list<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_serials, tk->get_threepf_database());

            // compute real twopf configurations which should be dropped.
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_re_serials.begin(), twopf_re_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping real twopf configurations entailed by these threepf configurations, but present in the container";
                this->drop_twopf_re_configurations(writer, undropped, tk->get_twopf_database());
              }

            // compute imaginary twopf configurations which should be dropped
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_im_serials.begin(), twopf_im_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping imaginary twopf configurations entailed by these threepf configurations, but present in the container";
                this->drop_twopf_im_configurations(writer, undropped, tk->get_twopf_database());
              }
          }
      }


    template <typename number>
    void data_manager<number>::check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        zeta_twopf_task<number>* tk = dynamic_cast< zeta_twopf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for zeta twopf container '" << writer.get_abs_container_path().string() << "'";

        std::list<unsigned int> serials = this->get_missing_zeta_twopf_serials(writer);

        if(serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, serials, tk->get_twopf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra configurations not missing from container, but advised by backend:";
                this->drop_zeta_twopf_configurations(writer, remainder, tk->get_twopf_database());
              }

            // push list of missing serial numbers to writer
            std::list<unsigned int> merged_missing = serials;
            merged_missing.merge(remainder);

            writer.set_missing_serials(merged_missing);
          }
      }


    template <typename number>
    void data_manager<number>::check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        zeta_threepf_task<number>* tk = dynamic_cast< zeta_threepf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for zeta threepf container '" << writer.get_abs_container_path().string() << "'";

        // get lists of missing serial numbers for threepf, redbsp and twopf
        std::list<unsigned int> threepf_serials = this->get_missing_zeta_threepf_serials(writer);
        std::list<unsigned int> twopf_serials   = this->get_missing_zeta_twopf_serials(writer);

        // map missing twopf serials into threepf serials
        std::list<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_serials, tk->get_threepf_database());

        if(threepf_serials.size() > 0)
          {
            std::list<unsigned int> remainder = this->advise_missing_content(writer, threepf_serials, tk->get_threepf_database());  // marks set_fail() for writer

            if(remainder.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra configurations not missing from container, but advised by backend:";
                this->drop_zeta_threepf_configurations(writer, remainder, tk->get_threepf_database());
                threepf_serials.merge(remainder);
              }
          }

        // check if any twopf configurations require dropping even more threepfs
        std::list<unsigned int> undropped;
        std::set_difference(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end(),
                            threepf_serials.begin(), threepf_serials.end(), std::back_inserter(undropped));

        if(undropped.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra threepf configurations not missing from container, but implied by missing twopf configurations:";
            this->drop_zeta_threepf_configurations(writer, undropped, tk->get_threepf_database());
            threepf_serials.merge(undropped);   // not necessary to remove duplicates, since there should be any; result is sorted
          }

        if(threepf_serials.size() > 0)
          {
            writer.set_missing_serials(threepf_serials);

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::list<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_serials, tk->get_threepf_database());

            // compute twopf configurations which should be dropped
            undropped.clear();
            std::set_difference(twopf_drop.begin(), twopf_drop.end(),
                                twopf_serials.begin(), twopf_serials.end(), std::back_inserter(undropped));

            if(undropped.size() > 0)
              {
                BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping twopf configurations entailed by these threepf configurations, but present in the container";
                this->drop_zeta_twopf_configurations(writer, undropped, tk->get_twopf_database());
              }
          }
      }


    template <typename number>
    void data_manager<number>::check_fNL_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* tk)
      {
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for fNL container '" << writer.get_abs_container_path().string() << "'";
      }


    template <typename number>
    void data_manager<number>::synchronize_missing_serials(integration_writer<number>& i_writer, postintegration_writer<number>& p_writer,
                                                           integration_task<number>* i_tk, postintegration_task<number>* p_tk)
      {
        // get serial numbers missing individually from each writer
        std::list<unsigned int> integration_missing = i_writer.get_missing_serials();
        std::list<unsigned int> postintegration_missing = p_writer.get_missing_serials();

        // merge into a single list
        std::list<unsigned int> total_missing = integration_missing;
        std::list<unsigned int> temp = postintegration_missing;
        total_missing.merge(temp);
        total_missing.unique();

        // check for discrepancies
        std::list<unsigned int> integration_discrepant;
        std::set_difference(total_missing.begin(), total_missing.end(),
                            integration_missing.begin(), integration_missing.end(), std::back_inserter(integration_discrepant));

        if(integration_discrepant.size() > 0)
          {
            BOOST_LOG_SEV(i_writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Synchronizing " << integration_discrepant.size() << " configurations in integration container which are missing in postintegration container";
            i_writer.merge_failure_list(integration_discrepant);
            i_writer.check_integrity(i_tk);
          }

        std::list<unsigned int> postintegration_discrepant;
        std::set_difference(total_missing.begin(), total_missing.end(),
                            postintegration_missing.begin(), postintegration_missing.end(), std::back_inserter(postintegration_discrepant));

        if(postintegration_discrepant.size() > 0)
          {
            BOOST_LOG_SEV(p_writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Synchronizing " << postintegration_discrepant.size() << " configurations in postintegration container which are missing in integration container";
            p_writer.merge_failure_list(postintegration_discrepant);
            p_writer.check_integrity(p_tk);
          }
      }

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_IMPL_H
