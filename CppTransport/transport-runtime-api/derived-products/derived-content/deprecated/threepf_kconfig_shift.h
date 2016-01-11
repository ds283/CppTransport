//
// Created by David Seery on 03/06/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef __threepf_kconfig_shift_H_
#define __threepf_kconfig_shift_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>
#include <array>

#include "transport-runtime-api/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime-api/data/datapipe/datapipe.h"

// forward-declare model class if needed
#include "transport-runtime-api/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime-api/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime-api/repository/records/repository_records_forward_declare.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/extractor.h"

#include "transport-runtime-api/derived-products/derived-content/SQL_query/SQL_query.h"
#include "transport-runtime-api/derived-products/derived-content/correlation-functions/compute-gadgets/common.h"


namespace transport
	{

		namespace derived_data
			{

        template <typename number>
		    class threepf_kconfig_shift
			    {

		        // CONSTRUCTOR, DESTRUCTOR

		      public:

				    //! constructor is default
				    threepf_kconfig_shift() = default;

				    //! destructor is default
				    virtual ~threepf_kconfig_shift() = default;


				    // SHIFT MOMENTUM -> DERIVATIVES

		      public:

		        //! shift a threepf kconfig-line for coordinate labels (l,m,n)
		        //! and supplied time configuration
		        void shift(twopf_list_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
		                   const SQL_threepf_kconfig_query& kquery, std::vector<threepf_kconfig>& configs,
		                   const std::vector<number>& background, std::vector<number>& line_data,
		                   unsigned int l, unsigned int m, unsigned int n, const time_config& t_config) const;

		      protected:

		        //! apply the derivative shift to a particular operator
		        void make_shift(twopf_list_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
		                        const SQL_threepf_kconfig_query& kquery, const std::vector< threepf_kconfig >& configs,
		                        const std::vector<number>& background, std::vector<number>& line_data,
		                        const SQL_threepf_kconfig_query& q_query, const SQL_threepf_kconfig_query& r_query,
		                        const time_config& t_config,
		                        unsigned int p, const extractor& p_extract,
		                        unsigned int q, const extractor& q_extract,
		                        unsigned int r, const extractor& r_extract,
		                        derived_data_impl::operator_position pos) const;

			    };


				template <typename number>
				void threepf_kconfig_shift<number>::shift(twopf_list_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
				                                          const SQL_threepf_kconfig_query& kquery, std::vector<threepf_kconfig>& configs,
				                                          const std::vector<number>& background, std::vector<number>& line_data,
				                                          unsigned int l, unsigned int m, unsigned int n, const time_config& t_config) const
					{
						assert(mdl != nullptr);
						assert(tk != nullptr);

						unsigned int N_fields = mdl->get_N_fields();

						if(l >= N_fields)
							{
								SQL_threepf_kconfig_query q_query(kquery);
								SQL_threepf_kconfig_query r_query(kquery);
								q_query.set_config_type(SQL_config_type::k2_config);
						    r_query.set_config_type(SQL_config_type::k3_config);

						    this->make_shift(tk, mdl, pipe, kquery, configs, background, line_data, q_query, r_query, t_config,
						                     l, extractor(1), m, extractor(2), n, extractor(3), derived_data_impl::operator_position::left_pos);
							}

						if(m >= N_fields)
							{
						    SQL_threepf_kconfig_query q_query(kquery);
						    SQL_threepf_kconfig_query r_query(kquery);
						    q_query.set_config_type(SQL_config_type::k1_config);
						    r_query.set_config_type(SQL_config_type::k3_config);

						    this->make_shift(tk, mdl, pipe, kquery, configs, background, line_data, q_query, r_query, t_config,
						                     m, extractor(2), m, extractor(1), n, extractor(3), derived_data_impl::operator_position::middle_pos);
							}

						if(n >= N_fields)
							{
						    SQL_threepf_kconfig_query q_query(kquery);
						    SQL_threepf_kconfig_query r_query(kquery);
						    q_query.set_config_type(SQL_config_type::k1_config);
						    r_query.set_config_type(SQL_config_type::k2_config);

						    this->make_shift(tk, mdl, pipe, kquery, configs, background, line_data, q_query, r_query, t_config,
						                     n, extractor(3), m, extractor(1), n, extractor(2), derived_data_impl::operator_position::right_pos);
							}
					}


		    template <typename number>
		    void threepf_kconfig_shift<number>::make_shift(twopf_list_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
		                                                   const SQL_threepf_kconfig_query& kquery, const std::vector<threepf_kconfig>& configs,
		                                                   const std::vector<number>& background, std::vector<number>& line_data,
		                                                   const SQL_threepf_kconfig_query& q_query, const SQL_threepf_kconfig_query& r_query,
		                                                   const time_config& t_config,
		                                                   unsigned int p, const extractor& p_extract,
		                                                   unsigned int q, const extractor& q_extract,
		                                                   unsigned int r, const extractor& r_extract,
		                                                   derived_data_impl::operator_position pos) const
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

		        // we need to know which index on each twopf is fixed, and which is summed over
		        derived_data_impl::fixed_index q_fixed = derived_data_impl::fixed_index::first_index;
		        derived_data_impl::fixed_index r_fixed = derived_data_impl::fixed_index::first_index;

		        switch(pos)
			        {
		            case derived_data_impl::operator_position::left_pos:    // our operator is on the far left-hand side, so is to the left of both the q and r operators
			            q_fixed = derived_data_impl::fixed_index::second_index;
		              r_fixed = derived_data_impl::fixed_index::second_index;
		              break;

		            case derived_data_impl::operator_position::middle_pos:  // our operator is in the middle, to the right of the q operator but to the left of the r operator
			            q_fixed = derived_data_impl::fixed_index::first_index;
		              r_fixed = derived_data_impl::fixed_index::second_index;
		              break;

		            case derived_data_impl::operator_position::right_pos:   // our operator is on the right, to the right of both the q and r operators
			            q_fixed = derived_data_impl::fixed_index::first_index;
		              r_fixed = derived_data_impl::fixed_index::first_index;
		              break;
              }

				    // pull out the components of the twopf which we will need
				    typename datapipe<number>::kconfig_data_handle& q_handle = pipe.new_kconfig_data_handle(q_query);
				    typename datapipe<number>::kconfig_data_handle& r_handle = pipe.new_kconfig_data_handle(r_query);

				    for(unsigned int i = 0; i < N_fields; ++i)
					    {
				        unsigned int q_id = mdl->flatten((q_fixed == derived_data_impl::fixed_index::first_index ? q : i), (q_fixed == derived_data_impl::fixed_index::second_index ? q : i));
				        unsigned int r_id = mdl->flatten((r_fixed == derived_data_impl::fixed_index::first_index ? r : i), (r_fixed == derived_data_impl::fixed_index::second_index ? r : i));
				        unsigned int mom_q_id = mdl->flatten((q_fixed == derived_data_impl::fixed_index::first_index ? q : mdl->momentum(i)), (q_fixed == derived_data_impl::fixed_index::second_index ? q : mdl->momentum(i)));
				        unsigned int mom_r_id = mdl->flatten((r_fixed == derived_data_impl::fixed_index::first_index ? r : mdl->momentum(i)), (r_fixed == derived_data_impl::fixed_index::second_index ? r : mdl->momentum(i)));

						    cf_kconfig_data_tag<number> q_re_tag     = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_re, q_id,     t_config.serial);
				        cf_kconfig_data_tag<number> q_im_tag     = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_im, q_id,     t_config.serial);
				        cf_kconfig_data_tag<number> mom_q_re_tag = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_re, mom_q_id, t_config.serial);
				        cf_kconfig_data_tag<number> mom_q_im_tag = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_im, mom_q_id, t_config.serial);

				        cf_kconfig_data_tag<number> r_re_tag     = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_re, r_id,     t_config.serial);
				        cf_kconfig_data_tag<number> r_im_tag     = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_im, r_id,     t_config.serial);
				        cf_kconfig_data_tag<number> mom_r_re_tag = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_re, mom_r_id, t_config.serial);
				        cf_kconfig_data_tag<number> mom_r_im_tag = pipe.new_cf_kconfig_data_tag(cf_data_type::cf_twopf_im, mom_r_id, t_config.serial);

				        const std::vector<number>& q_line_re     = q_handle.lookup_tag(q_re_tag);
				        const std::vector<number>& q_line_im     = q_handle.lookup_tag(q_im_tag);
				        const std::vector<number>& mom_q_line_re = q_handle.lookup_tag(mom_q_re_tag);
				        const std::vector<number>& mom_q_line_im = q_handle.lookup_tag(mom_q_im_tag);
				        const std::vector<number>& r_line_re     = r_handle.lookup_tag(r_re_tag);
				        const std::vector<number>& r_line_im     = r_handle.lookup_tag(r_im_tag);
				        const std::vector<number>& mom_r_line_re = r_handle.lookup_tag(mom_r_re_tag);
				        const std::vector<number>& mom_r_line_im = r_handle.lookup_tag(mom_r_im_tag);

						    for(unsigned int j = 0; j < line_data.size(); ++j)
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
				    for(unsigned int i = 0; i < line_data.size(); ++i)
					    {
				        std::vector<number> B_qrp(N_fields*N_fields*N_fields);
				        std::vector<number> C_pqr(N_fields*N_fields*N_fields);
				        std::vector<number> C_prq(N_fields*N_fields*N_fields);

				        // evaluate B and C tensors for this configuration
				        mdl->B(tk, background, q_extract.comoving(configs[i]), r_extract.comoving(configs[i]), p_extract.comoving(configs[i]), t_config.t, B_qrp);
				        mdl->C(tk, background, p_extract.comoving(configs[i]), q_extract.comoving(configs[i]), r_extract.comoving(configs[i]), t_config.t, C_pqr);
				        mdl->C(tk, background, p_extract.comoving(configs[i]), r_extract.comoving(configs[i]), q_extract.comoving(configs[i]), t_config.t, C_prq);

				        number shift = 0.0;

				        for(unsigned int m = 0; m < N_fields; ++m)
					        {
				            for(unsigned int n = 0; n < N_fields; ++n)
					            {
				                shift -= B_qrp[mdl->fields_flatten(m,n,p_species)] * ( sigma_q_re[i][m]*sigma_r_re[i][n] - sigma_q_im[i][m]*sigma_r_im[i][n] );

				                shift -= C_pqr[mdl->fields_flatten(p_species,m,n)] * ( mom_sigma_q_re[i][m]*sigma_r_re[i][n] - mom_sigma_q_im[i][m]*sigma_r_im[i][n] );
				                shift -= C_prq[mdl->fields_flatten(p_species,m,n)] * ( sigma_q_re[i][n]*mom_sigma_r_re[i][m] - sigma_q_im[i][n]*mom_sigma_r_im[i][m] );
					            }
					        }

				        line_data[i] += shift;
					    }
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__threepf_kconfig_shift_H_
