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


    // TRANSACTIONS


    template <typename number>
    template <typename WriterObject>
    transaction_manager data_manager<number>::generate_transaction_manager(WriterObject& writer, std::unique_ptr<transaction_handler> handle)
      {
        if(this->transactions > 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_UNDERWAY);
        this->transactions++;

        // extract location of container and convert to location of lockfile
        boost::filesystem::path ctr_path = writer.get_abs_container_path();

        return transaction_manager(ctr_path.parent_path() / CPPTRANSPORT_DATAMGR_LOCKFILE_LEAF, std::move(handle));
      }


    template <typename number>
    void data_manager<number>::release_transaction()
      {
        if(this->transactions == 0) throw runtime_exception(exception_type::TRANSACTION_ERROR, CPPTRANSPORT_TRANSACTION_OVER_RELEASE);
        this->transactions--;
      }


    // INTEGRITY CHECK


    template <typename number>
    template <typename WriterObject, typename Database>
    void data_manager<number>::advise_missing_content(WriterObject& writer, const std::set<unsigned int>& serials, const Database& db)
      {
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Detected missing data in container";
        writer.set_fail(true);

        for(unsigned int serial : serials)
          {
            // find this configuration
            typename Database::const_config_iterator u = std::find_if(db.config_begin(), db.config_end(),
                                                                      ConfigurationFinder<typename Database::const_config_iterator::type>(serial));

            // emit configuration information
            std::ostringstream msg;
            msg << *u;
            std::string msg_str = msg.str();
            boost::algorithm::trim_right(msg_str);
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** " << msg_str;
          }
      };

    template <typename number>
    template <typename WriterObject, typename Database>
    std::set<unsigned int> data_manager<number>::find_failed_but_undropped_serials(WriterObject& writer, const std::set<unsigned int>& serials, const Database& db)
      {
        std::set<unsigned int> advised_list = writer.get_failed_serials();

        for(unsigned int serial : serials)
          {
            // search for this element in the advised list
            std::set<unsigned int>::const_iterator ad = advised_list.find(serial);

            // was this an item on the list we already knew would be missing?
            if(ad != advised_list.end()) advised_list.erase(ad);
          }

        // return any remainder
        return(advised_list);
      }


    template <typename number>
    std::set<unsigned int> data_manager<number>::compute_twopf_drop_list(const std::set<unsigned int>& serials, const threepf_kconfig_database& threepf_db)
      {
        std::set<unsigned int> drop_serials;

        // TODO: this is a O(N^2) algorithm; it would be nice if it could be replaced with something better
        for(unsigned int serial : serials)
          {
            threepf_kconfig_database::const_record_iterator u = std::find_if(threepf_db.record_begin(), threepf_db.record_end(),
                                                                             RecordFinder<threepf_kconfig_database::const_record_iterator::type>(serial));

            if(u != threepf_db.record_end())
              {
                if(u->is_twopf_k1_stored()) drop_serials.insert((*u)->k1_serial);
                if(u->is_twopf_k2_stored()) drop_serials.insert((*u)->k2_serial);
                if(u->is_twopf_k3_stored()) drop_serials.insert((*u)->k3_serial);
              }
          }

        return(drop_serials);
      }


    template <typename number>
    std::set<unsigned int> data_manager<number>::map_twopf_to_threepf_serials(const std::set<unsigned int>& twopf_list, const threepf_kconfig_database& threepf_db)
      {
        std::set<unsigned int> threepf_list;

        // TODO: this is a O(N^2) algorithm; it would be nice if it could be replaced with something better
        for(unsigned int twopf_serial : twopf_list)
          {
            for(threepf_kconfig_database::const_record_iterator u = threepf_db.record_begin(); u != threepf_db.record_end(); ++u)
              {
                if(u->is_twopf_k1_stored() && (*u)->k1_serial == twopf_serial)
                  {
                    threepf_list.insert((*u)->serial);
                    break;
                  }
                if(u->is_twopf_k2_stored() && (*u)->k2_serial == twopf_serial)
                  {
                    threepf_list.insert((*u)->serial);
                    break;
                  }
                if(u->is_twopf_k3_stored() && (*u)->k3_serial == twopf_serial)
                  {
                    threepf_list.insert((*u)->serial);
                    break;
                  }
              }
          }

        return(threepf_list);
      }


    template <typename number>
    void data_manager<number>::check_twopf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        twopf_task<number>* tk = dynamic_cast< twopf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for twopf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // extract list of twopf kconfigurations which have no entry in the database
        // note that we allow the possibility that there are configurations which are present in the data tables, but missing
        // in the statistics or ics tables.
        // In this case we don't drop the corresponding data; we just live with the missing metadata
        std::set<unsigned int> twopf_serials = this->get_missing_twopf_re_serials(writer);
        std::set<unsigned int> tensor_serials = this->get_missing_tensor_twopf_serials(writer);

        // merge
        std::set<unsigned int> total_serials;
        total_serials.insert(twopf_serials.begin(), twopf_serials.end());
        total_serials.insert(tensor_serials.begin(), tensor_serials.end());

        // compare against backend-supplied list of failed configurations, if one is available
        std::set<unsigned int> failed = this->find_failed_but_undropped_serials(writer, total_serials, tk->get_twopf_database());
        if(failed.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Dropping extra configurations not missing from container, but advised as failed by backend:";

            // merge any remainder with our own list of auto-detected missing serials
            total_serials.insert(failed.begin(), failed.end());
          }

        // if any serial numbers are missing, advise the user
        if(total_serials.size() > 0)
          {
            // advise the user that content is missing; marks set_fail() for writer
            this->advise_missing_content(writer, total_serials, tk->get_twopf_database());

            // push list of missing serial numbers to writer and update metadata suitably
            writer.set_missing_serials(total_serials, tk->get_twopf_database());

            // ensure all tables are consistent
            this->drop_twopf_re_configurations(mgr, writer, total_serials, tk->get_twopf_database());
            this->drop_tensor_twopf_configurations(mgr, writer, total_serials, tk->get_twopf_database());
            if(writer.is_collecting_statistics()) this->drop_statistics_configurations(mgr, writer, total_serials, tk->get_twopf_database());
            if(writer.is_collecting_initial_conditions()) this->drop_initial_conditions_configurations(mgr, writer, total_serials, tk->get_twopf_database());
          }

        mgr.commit();
        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Integrity check complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager<number>::check_threepf_integrity_handler(integration_writer<number>& writer, integration_task<number>* itk)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        threepf_task<number>* tk = dynamic_cast< threepf_task<number>* >(itk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for threepf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // note that we allow the possibility that there are configurations which are present in the data tables, but missing
        // in the statistics or ics tables.
        // In this case we don't drop the corresponding data; we just live with the missing metadata

        // get lists of missing serial numbers for threepf configurations
        std::set<unsigned int> threepf_momentum_serials  = this->get_missing_threepf_momentum_serials(writer);
        std::set<unsigned int> threepf_deriv_serials     = this->get_missing_threepf_deriv_serials(writer);

        // get lists of missing serial numbers for twopf configurations
        std::set<unsigned int> twopf_re_serials          = this->get_missing_twopf_re_serials(writer);
        std::set<unsigned int> twopf_im_serials          = this->get_missing_twopf_im_serials(writer);
        std::set<unsigned int> tensor_serials            = this->get_missing_tensor_twopf_serials(writer);

        // merge missing threepf lists into a single one
        std::set<unsigned int> threepf_total_serials;
        threepf_total_serials.insert(threepf_momentum_serials.begin(), threepf_momentum_serials.end());
        threepf_total_serials.insert(threepf_deriv_serials.begin(), threepf_deriv_serials.end());

        // merge missing twopf lists into a single one
        std::set<unsigned int> twopf_total_serials;
        twopf_total_serials.insert(twopf_re_serials.begin(), twopf_re_serials.end());
        twopf_total_serials.insert(twopf_im_serials.begin(), twopf_im_serials.end());
        twopf_total_serials.insert(tensor_serials.begin(), tensor_serials.end());

        // compare against backend-supplied list of failed configurations, if one is available
        std::set<unsigned int> failed = this->find_failed_but_undropped_serials(writer, threepf_total_serials, tk->get_threepf_database());
        if(failed.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra threepf configurations not missing from container, but advised as failed by backend:";

            // merge any remainder with our list of auto-detected missing serials
            threepf_total_serials.insert(failed.begin(), failed.end());
          }

        // now check whether any missing twopf configurations might require further threepfs to be dropped for consistency
        std::set<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_total_serials, tk->get_threepf_database());
        threepf_total_serials.insert(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end());

        // if any serial numbers are missing, advise the user
        // then ensure that corresponding twopf configurations are missing for consistency
        if(threepf_total_serials.size() > 0)
          {
            // advise the user that content is missing; marks set_fail() for writer
            this->advise_missing_content(writer, threepf_total_serials, tk->get_threepf_database());

            // push list of missing serial numbers to writer and update metadata suitably
            writer.set_missing_serials(threepf_total_serials, tk->get_threepf_database());

            // ensure all threepf-indexed tables are consistent
            this->drop_threepf_momentum_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());
            this->drop_threepf_deriv_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());
            if(writer.is_collecting_statistics())         this->drop_statistics_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());
            if(writer.is_collecting_initial_conditions()) this->drop_initial_conditions_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::set<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_total_serials, tk->get_threepf_database());

            // ensure all twopf-indexed tables are consistent
            this->drop_twopf_re_configurations(mgr, writer, twopf_drop, tk->get_twopf_database());
            this->drop_twopf_im_configurations(mgr, writer, twopf_drop, tk->get_twopf_database());
            this->drop_tensor_twopf_configurations(mgr, writer, twopf_drop, tk->get_twopf_database());
          }

        mgr.commit();
        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Integrity check complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager<number>::check_zeta_twopf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        zeta_twopf_task<number>* tk = dynamic_cast< zeta_twopf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for zeta twopf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // extract list of twopf kconfigurations which have no entry in the database
        // note that we allow the possibility that there are configurations which are present in the data tables, but missing
        // in the statistics or ics tables.
        // In this case we don't drop the corresponding data; we just live with the missing metadata
        std::set<unsigned int> twopf_serials  = this->get_missing_zeta_twopf_serials(writer);
        std::set<unsigned int> xfm1_serials   = this->get_missing_gauge_xfm1_serials(writer);

        // merge
        std::set<unsigned int> total_serials;
        total_serials.insert(twopf_serials.begin(), twopf_serials.end());
        total_serials.insert(xfm1_serials.begin(), xfm1_serials.end());

        // compare against backend-supplied list of failed configurations, if one is available
        std::set<unsigned int> failed = this->find_failed_but_undropped_serials(writer, total_serials, tk->get_twopf_database());
        if(failed.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra configurations not missing from container, but advised as failed by backend:";

            // merge any remainder with auto-detected list
            total_serials.insert(failed.begin(), failed.end());
          }

        // if any serial numbers are missing, advise the user
        if(total_serials.size() > 0)
          {
            // advise the user that content is missing; marks set_fail() for writer
            this->advise_missing_content(writer, total_serials, tk->get_twopf_database());

            // push list of missing serial numbers to writer and update metadata suitably
            writer.set_missing_serials(total_serials, tk->get_twopf_database());

            // ensure all tables are consistent
            this->drop_zeta_twopf_configurations(mgr, writer, total_serials, tk->get_twopf_database());
            this->drop_gauge_xfm1_configurations(mgr, writer, total_serials, tk->get_twopf_database());
          }

        mgr.commit();
        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Integrity check complete in time " << format_time(timer.elapsed().wall);
      }


    template <typename number>
    void data_manager<number>::check_zeta_threepf_integrity_handler(postintegration_writer<number>& writer, postintegration_task<number>* ptk)
      {
        transaction_manager mgr = this->transaction_factory(writer);

        zeta_threepf_task<number>* tk = dynamic_cast< zeta_threepf_task<number>* >(ptk);
        assert(tk != nullptr);

        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Performing integrity check for zeta threepf container '" << writer.get_abs_container_path().string() << "'";

        boost::timer::cpu_timer timer;

        // note that we allow the possibility that there are configurations which are present in the data tables, but missing
        // in the statistics or ics tables.
        // In this case we don't drop the corresponding data; we just live with the missing metadata

        // get lists of missing serial numbers for threepf-configurations
        // (threepf rows store both threepf data and reduced bispectrum data, so the reduced bispectrum
        // does not need to be handled separately)
        std::set<unsigned int> threepf_serials        = this->get_missing_zeta_threepf_serials(writer);
        std::set<unsigned int> gauge_xfm2_123_serials = this->get_missing_gauge_xfm2_123_serials(writer);
        std::set<unsigned int> gauge_xfm2_213_serials = this->get_missing_gauge_xfm2_213_serials(writer);
        std::set<unsigned int> gauge_xfm2_312_serials = this->get_missing_gauge_xfm2_312_serials(writer);

        // get lists of missing serial numbers for twopf-configurations
        std::set<unsigned int> twopf_serials          = this->get_missing_zeta_twopf_serials(writer);
        std::set<unsigned int> gauge_xfm1_serials     = this->get_missing_gauge_xfm1_serials(writer);

        // merge missing threepf lists
        std::set<unsigned int> threepf_total_serials;
        threepf_total_serials.insert(threepf_serials.begin(), threepf_serials.end());
        threepf_total_serials.insert(gauge_xfm2_123_serials.begin(), gauge_xfm2_123_serials.end());
        threepf_total_serials.insert(gauge_xfm2_213_serials.begin(), gauge_xfm2_213_serials.end());
        threepf_total_serials.insert(gauge_xfm2_312_serials.begin(), gauge_xfm2_312_serials.end());

        // merge missing twopf lists
        std::set<unsigned int> twopf_total_serials;
        twopf_total_serials.insert(twopf_serials.begin(), twopf_serials.end());
        twopf_total_serials.insert(gauge_xfm1_serials.begin(), gauge_xfm1_serials.end());

        // compare against backend-supplied list of failed configurations if one is available
        std::set<unsigned int> failed = this->find_failed_but_undropped_serials(writer, threepf_total_serials, tk->get_threepf_database());
        if(failed.size() > 0)
          {
            BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Dropping extra configurations not missing from container, but advised as failed by backend:";

            // merge any remainder with our auto-detected list of missing serials
            threepf_total_serials.insert(failed.begin(), failed.end());
          }

        // now check whether any missing twopf configurations might require even further threepfs to be dropped for consistency
        std::set<unsigned int> twopf_to_threepf_map = this->map_twopf_to_threepf_serials(twopf_total_serials, tk->get_threepf_database());
        threepf_total_serials.insert(twopf_to_threepf_map.begin(), twopf_to_threepf_map.end());

        // if any serial numbers are missing, advise the user
        // then ensure that corresponding twopf configurations are missing for consistency
        if(threepf_total_serials.size() > 0)
          {
            // advise the user that content is missing; marks set_fail() for writer
            this->advise_missing_content(writer, threepf_total_serials, tk->get_threepf_database());

            // push list of missing serial numbers to writer and update metadata suitably
            writer.set_missing_serials(threepf_total_serials, tk->get_threepf_database());

            // ensure all threepf-indexed tables are consistent
            this->drop_zeta_threepf_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());
            this->drop_gauge_xfm2_123_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());
            this->drop_gauge_xfm2_213_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());
            this->drop_gauge_xfm2_312_configurations(mgr, writer, threepf_total_serials, tk->get_threepf_database());

            // build list of twopf configurations which should be dropped for this entire set of threepf configurations
            std::set<unsigned int> twopf_drop = this->compute_twopf_drop_list(threepf_total_serials, tk->get_threepf_database());

            // ensure all twopf-indexed tables are consistent
            this->drop_zeta_twopf_configurations(mgr, writer, twopf_drop, tk->get_twopf_database());
            this->drop_gauge_xfm1_configurations(mgr, writer, twopf_drop, tk->get_twopf_database());
          }

        mgr.commit();
        timer.stop();
        BOOST_LOG_SEV(writer.get_log(), base_writer::log_severity_level::normal) << "** Integrity check complete in time " << format_time(timer.elapsed().wall);
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
        std::set<unsigned int> integration_missing = i_writer.get_missing_serials();
        std::set<unsigned int> postintegration_missing = p_writer.get_missing_serials();

        // merge into a single list
        std::set<unsigned int> total_missing;
        total_missing.insert(integration_missing.begin(), integration_missing.end());
        total_missing.insert(postintegration_missing.begin(), postintegration_missing.end());

        // check for discrepancies
        std::set<unsigned int> integration_discrepant;
        std::set_difference(total_missing.begin(), total_missing.end(),
                            integration_missing.begin(), integration_missing.end(), std::inserter(integration_discrepant, integration_discrepant.begin()));

        if(integration_discrepant.size() > 0)
          {
            BOOST_LOG_SEV(i_writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Synchronizing " << integration_discrepant.size() << " configurations in integration container which are missing in postintegration container";
            i_writer.merge_failure_list(integration_discrepant);
            i_writer.check_integrity(i_tk);
          }

        std::set<unsigned int> postintegration_discrepant;
        std::set_difference(total_missing.begin(), total_missing.end(),
                            postintegration_missing.begin(), postintegration_missing.end(), std::inserter(postintegration_discrepant, postintegration_discrepant.begin()));

        if(postintegration_discrepant.size() > 0)
          {
            BOOST_LOG_SEV(p_writer.get_log(), base_writer::log_severity_level::normal) << '\n' << "** Synchronizing " << postintegration_discrepant.size() << " configurations in postintegration container which are missing in integration container";
            p_writer.merge_failure_list(postintegration_discrepant);
            p_writer.check_integrity(p_tk);
          }
      }

  }   // namespace transport


#endif //CPPTRANSPORT_DATA_MANAGER_IMPL_H
