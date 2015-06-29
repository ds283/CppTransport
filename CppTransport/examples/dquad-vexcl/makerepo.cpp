//
// Created by David Seery on 30/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#include "dq_vexcl-cuda.h"

#include "transport/manager/repository_creation_key.h"


// ****************************************************************************


// set up parameters and initial conditions
// we work in units where M_p=1, but that's up to us;
// we could choose something different

const double M_Planck = 1.0;

const double mass		  = 1E-5 * M_Planck;
const double m_phi    = 9.0 * mass;
const double m_chi    = 1.0 * mass;

const double phi_init = 10;
const double chi_init = 12.9;


// ****************************************************************************


int main(int argc, char* argv[])
  {
    transport::repository_creation_key key;

    // create a new repository
    transport::json_interface_repository<double>* repo = transport::repository_factory<double>("/Users/ds283/Documents/CppTransport-repository/test", key);
//    transport::repository<double>* repo = transport::repository_factory<double>("/Volumes/sulis/CppTransport-repository/test", key);

    // set up an instance of a manager
    std::shared_ptr< transport::task_manager<double> > mgr = std::make_shared< transport::task_manager<double> >(0, nullptr, repo);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    transport::dquad_vexcl<double>* model = new transport::dquad_vexcl<double>(mgr);

    // set up parameter choices
    const std::vector<double>     init_params = { m_phi, m_chi };
    transport::parameters<double> params      =
                                    transport::parameters<double>(M_Planck, init_params, model->get_param_names(),
                                                                  model->params_validator_factory());

    const std::vector<double> init_values = { phi_init, chi_init };

    const double Ninit  = 0.0;  // start counting from N=0 at the beginning of the integration
    const double Ncross = 9.0;  // horizon-crossing occurs at 9 e-folds from init_values
    const double Npre   = 7.0;  // how many e-folds do we wish to track the mode prior to horizon exit?
    const double Nmax   = 59.0; // how many e-folds to integrate after horizon crossing

    // set up initial conditions
    transport::initial_conditions<double> ics =
                                            transport::initial_conditions<double>("dquad-1", params, init_values, model->get_state_names(),
                                                                                  Ninit, Ncross, Npre,
                                                                                  model->ics_validator_factory(),
                                                                                  model->ics_finder_factory());

    const unsigned int t_samples = 5000;       // record 5000 samples - enough to find a good stepsize

    transport::range<double> times = transport::range<double >(Ninit, Nmax+Npre, t_samples);

    // the conventions for k-numbers are as follows:
    // k=1 is the mode which crosses the horizon at time N*,
    // where N* is the 'offset' we pass to the integration method (see below)
    const double        kmin      = exp(0.0);   // begin with the mode which crosses the horizon at N=N*
    const double        kmax      = exp(2.0);   // end with the mode which exits the horizon at N=N*+2
    const unsigned int  k_samples = 4;          // number of k-points

    transport::range<double> ks = transport::range<double>(kmin, kmax, k_samples);

    // construct a threepf task
    transport::threepf_task<double> tk3 = transport::threepf_task<double>("dquad.threepf-1", ics, times, ks, model->kconfig_kstar_factory());

    // construct a twopf task
    transport::twopf_task<double> tk2 = transport::twopf_task<double>("dquad.twopf-1", ics, times, ks, model->kconfig_kstar_factory());

    // write each initial conditions/parameter specification and integration specification into the model repository
    mgr->write_integration(tk2, model);
    mgr->write_integration(tk3, model);

    std::string package_xml = repo->extract_package_document(ics.get_name());
    std::cout << "Package XML document:" << '\n' << package_xml << '\n' << '\n';

    std::string task2_xml = repo->extract_integration_document(tk2.get_name());
    std::cout << "2pf integration XML document:" << '\n' << task2_xml << '\n' << '\n';

    std::string task3_xml = repo->extract_integration_document(tk3.get_name());
    std::cout << "3pf integration XML document:" << '\n' << task3_xml << '\n' << '\n';

    return(EXIT_SUCCESS);
  }
