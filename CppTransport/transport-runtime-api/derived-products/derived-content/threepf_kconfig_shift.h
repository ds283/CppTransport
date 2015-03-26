//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __threepf_kconfig_shift_H_
#define __threepf_kconfig_shift_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/data/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"
#include "transport-runtime-api/derived-products/utilities/extractor.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
		    class threepf_kconfig_shift
			    {

		      public:

		        typedef enum { left, middle, right } operator_position;


		        // CONSTRUCTOR, DESTRUCTOR

		      public:

				    threepf_kconfig_shift() = default;

				    virtual ~threepf_kconfig_shift() = default;


				    // SHIFT MOMENTUM -> DERIVATIVES

		      public:

		        //! shift a threepf kconfig-line for coordinate labels (l,m,n)
		        //! and supplied time configuration
		        void shift(integration_task<number>* tk, model<number>* mdl,
		                   typename data_manager<number>::datapipe& pipe,
		                   const std::vector<number>& background, std::vector< typename data_manager<number>::threepf_configuration>& configs,
		                   std::vector<number>& line_data,
		                   unsigned int l, unsigned int m, unsigned int n,
		                   unsigned int t_serial, double t_value) const;

		      protected:

		        //! apply the derivative shift to a particular operator
		        void make_shift(integration_task<number>* tk, model<number>* mdl,
		                        typename data_manager<number>::datapipe& pipe,
		                        const std::vector< typename data_manager<number>::threepf_configuration >& configs,
		                        std::vector< std::array<extractor<number>, 3> >& extractors,
		                        std::vector<number>& line_data,
		                        unsigned int t_serial, double t_value,
		                        const std::vector<number>& bg_config,
		                        unsigned int p, unsigned int q, unsigned int r, operator_position pos) const;

			    };


				template <typename number>
				void threepf_kconfig_shift<number>::shift(integration_task<number>* tk, model<number>* mdl,
				                                          typename data_manager<number>::datapipe& pipe,
				                                          const std::vector<number>& background, std::vector< typename data_manager<number>::threepf_configuration>& configs,
				                                          std::vector<number>& line_data,
				                                          unsigned int l, unsigned int m, unsigned int n,
				                                          unsigned int t_serial, double t_value) const
					{
						assert(mdl != nullptr);
						assert(tk != nullptr);

						unsigned int N_fields = mdl->get_N_fields();

						if(l >= N_fields)
							{
						    // set up array of k-value extractors for the first operator
						    std::vector< std::array<extractor<number>, 3> > extractors;

						    for(typename std::vector<typename data_manager<number>::threepf_configuration>::iterator t = configs.begin(); t != configs.end(); t++)
							    {
						        std::array<extractor<number>, 3> elt = { extractor<number>(1, *t), extractor<number>(2, *t), extractor<number>(3, *t) };
						        extractors.push_back(elt);
							    }

						    this->make_shift(tk, mdl, pipe, configs, extractors, line_data, t_serial, t_value, background, l, m, n, left);
							}

						if(m >= N_fields)
							{
						    // set up array of k-value extractors for the first operator
						    std::vector< std::array<extractor<number>, 3> > extractors;

						    for(typename std::vector<typename data_manager<number>::threepf_configuration>::iterator t = configs.begin(); t != configs.end(); t++)
							    {
						        std::array<extractor<number>, 3> elt = { extractor<number>(2, *t), extractor<number>(1, *t), extractor<number>(3, *t) };
						        extractors.push_back(elt);
							    }

						    this->make_shift(tk, mdl, pipe, configs, extractors, line_data, t_serial, t_value, background, m, l, n, middle);
							}

						if(n >= N_fields)
							{
						    // set up array of k-value extractors for the first operator
						    std::vector< std::array<extractor<number>, 3> > extractors;

						    for(typename std::vector<typename data_manager<number>::threepf_configuration>::iterator t = configs.begin(); t != configs.end(); t++)
							    {
						        std::array<extractor<number>, 3> elt = { extractor<number>(3, *t), extractor<number>(1, *t), extractor<number>(2, *t) };
						        extractors.push_back(elt);
							    }

						    this->make_shift(tk, mdl, pipe, configs, extractors, line_data, t_serial, t_value, background, n, l, m, right);
							}
					}


		    template <typename number>
		    void threepf_kconfig_shift<number>::make_shift(integration_task<number>* tk, model<number>* mdl,
		                                                   typename data_manager<number>::datapipe& pipe,
		                                                   const std::vector<typename data_manager<number>::threepf_configuration>& configs,
		                                                   std::vector< std::array<extractor<number>, 3> >& extractors,
		                                                   std::vector<number>& line_data,
		                                                   unsigned int t_serial, double t_value,
		                                                   const std::vector<number>& bg_config,
		                                                   unsigned int p, unsigned int q, unsigned int r, operator_position pos) const
			    {
						assert(tk != nullptr);
				    assert(mdl != nullptr);

		        unsigned int N_fields = mdl->get_N_fields();

				    // need multiple components of the 2pf
		        std::vector< std::vector<number> > sigma_q_re(configs.size());
		        std::vector< std::vector<number> > sigma_r_re(configs.size());
		        std::vector< std::vector<number> > mom_sigma_q_re(configs.size());
		        std::vector< std::vector<number> > mom_sigma_r_re(configs.size());
		        std::vector< std::vector<number> > sigma_q_im(configs.size());
		        std::vector< std::vector<number> > sigma_r_im(configs.size());
		        std::vector< std::vector<number> > mom_sigma_q_im(configs.size());
		        std::vector< std::vector<number> > mom_sigma_r_im(configs.size());

		        // p is guaranteed to be a momentum label, but we will want to know what field species it corresponds to
		        unsigned int p_species = mdl->species(p);

		        typedef enum { first_index, second_index } fixed_index;

		        // we need to know which index on each twopf is fixed, and which is summed over
		        fixed_index q_fixed = first_index;
		        fixed_index r_fixed = first_index;

		        switch(pos)
			        {
		            case left:    // our operator is on the far left-hand side, so is to the left of both the q and r operators
			            q_fixed = second_index;
		              r_fixed = second_index;
		              break;

		            case middle:  // our operator is in the middle, to the right of the q operator but to the left of the r operator
			            q_fixed = first_index;
		              r_fixed = second_index;
		              break;

		            case right:   // our operator is on the right, to the right of both the q and r operators
			            q_fixed = first_index;
		              r_fixed = first_index;
		              break;

		            default:
			            throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_TIME_SERIES_UNKNOWN_OPPOS);
			        }

				    // make a list of twopf serial numbers which we need
		        std::vector<unsigned int> q_serials;
		        std::vector<unsigned int> r_serials;
				    for(typename std::vector< typename std::array< extractor<number>, 3 > >::iterator t = extractors.begin(); t != extractors.end(); t++)
					    {
						    q_serials.push_back((*t)[1].serial());
						    r_serials.push_back((*t)[2].serial());
					    }

				    // pull out the components of the twopf which we will need
				    typename data_manager<number>::datapipe::kconfig_data_handle& q_handle = pipe.new_kconfig_data_handle(q_serials);
				    typename data_manager<number>::datapipe::kconfig_data_handle& r_handle = pipe.new_kconfig_data_handle(r_serials);

				    for(unsigned int i = 0; i < N_fields; i++)
					    {
				        unsigned int q_id = mdl->flatten((q_fixed == first_index ? q : i), (q_fixed == second_index ? q : i));
				        unsigned int r_id = mdl->flatten((r_fixed == first_index ? r : i), (r_fixed == second_index ? r : i));
				        unsigned int mom_q_id = mdl->flatten((q_fixed == first_index ? q : mdl->momentum(i)), (q_fixed == second_index ? q : mdl->momentum(i)));
				        unsigned int mom_r_id = mdl->flatten((r_fixed == first_index ? r : mdl->momentum(i)), (r_fixed == second_index ? r : mdl->momentum(i)));

						    typename data_manager<number>::datapipe::cf_kconfig_data_tag q_re_tag     = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, q_id,     t_serial);
				        typename data_manager<number>::datapipe::cf_kconfig_data_tag q_im_tag     = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, q_id,     t_serial);
				        typename data_manager<number>::datapipe::cf_kconfig_data_tag mom_q_re_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, mom_q_id, t_serial);
				        typename data_manager<number>::datapipe::cf_kconfig_data_tag mom_q_im_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, mom_q_id, t_serial);

				        typename data_manager<number>::datapipe::cf_kconfig_data_tag r_re_tag     = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, r_id,     t_serial);
				        typename data_manager<number>::datapipe::cf_kconfig_data_tag r_im_tag     = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, r_id,     t_serial);
				        typename data_manager<number>::datapipe::cf_kconfig_data_tag mom_r_re_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_re, mom_r_id, t_serial);
				        typename data_manager<number>::datapipe::cf_kconfig_data_tag mom_r_im_tag = pipe.new_cf_kconfig_data_tag(data_manager<number>::datapipe::cf_twopf_im, mom_r_id, t_serial);

				        const std::vector<number>& q_line_re     = q_handle.lookup_tag(q_re_tag);
				        const std::vector<number>& q_line_im     = q_handle.lookup_tag(q_im_tag);
				        const std::vector<number>& mom_q_line_re = q_handle.lookup_tag(mom_q_re_tag);
				        const std::vector<number>& mom_q_line_im = q_handle.lookup_tag(mom_q_im_tag);
				        const std::vector<number>& r_line_re     = r_handle.lookup_tag(r_re_tag);
				        const std::vector<number>& r_line_im     = r_handle.lookup_tag(r_im_tag);
				        const std::vector<number>& mom_r_line_re = r_handle.lookup_tag(mom_r_re_tag);
				        const std::vector<number>& mom_r_line_im = r_handle.lookup_tag(mom_r_im_tag);

						    for(unsigned int j = 0; j < line_data.size(); j++)
							    {
						        (sigma_q_re[j]).push_back(q_line_re[j]);
						        (sigma_q_im[j]).push_back(q_line_im[j]);
						        (mom_sigma_q_re[j]).push_back(mom_q_line_re[j]);
						        (mom_sigma_q_im[j]).push_back(mom_q_line_im[j]);
						        (sigma_r_re[j]).push_back(r_line_re[j]);
						        (sigma_r_im[j]).push_back(r_line_im[j]);
						        (mom_sigma_r_re[j]).push_back(mom_r_line_re[j]);
						        (mom_sigma_r_im[j]).push_back(mom_r_line_im[j]);
							    }
					    }

				    // work through the kconfig sample, shifting each value appropriately
				    for(unsigned int i = 0; i < line_data.size(); i++)
					    {
				        std::vector< std::vector< std::vector<number> > > B_qrp;
				        std::vector< std::vector< std::vector<number> > > C_pqr;
				        std::vector< std::vector< std::vector<number> > > C_prq;

				        // evaluate B and C tensors for this configuration
				        mdl->B(tk->get_params(), bg_config, extractors[i][1].comoving(), extractors[i][2].comoving(), extractors[i][0].comoving(), t_value, B_qrp);
				        mdl->C(tk->get_params(), bg_config, extractors[i][0].comoving(), extractors[i][1].comoving(), extractors[i][2].comoving(), t_value, C_pqr);
				        mdl->C(tk->get_params(), bg_config, extractors[i][0].comoving(), extractors[i][2].comoving(), extractors[i][1].comoving(), t_value, C_prq);

				        number shift = 0.0;

				        for(unsigned int m = 0; m < N_fields; m++)
					        {
				            for(unsigned int n = 0; n < N_fields; n++)
					            {
				                shift -= B_qrp[m][n][p_species] * ( sigma_q_re[i][m]*sigma_r_re[i][n] - sigma_q_im[i][m]*sigma_r_im[i][n] );

				                shift -= C_pqr[p_species][m][n] * ( mom_sigma_q_re[i][m]*sigma_r_re[i][n] - mom_sigma_q_im[i][m]*sigma_r_im[i][n] );
				                shift -= C_prq[p_species][m][n] * ( sigma_q_re[i][n]*mom_sigma_r_re[i][m] - sigma_q_im[i][n]*mom_sigma_r_im[i][m] );
					            }
					        }

				        line_data[i] += shift;
					    }
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__threepf_kconfig_shift_H_
