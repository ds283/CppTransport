//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-2016 University of Sussex. All rights reserved.
//


#ifndef __threepf_time_shift_H_
#define __threepf_time_shift_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>
#include <array>

#include "transport-runtime/serialization/serializable.h"

// get details of datapipe<number>
#include "transport-runtime/data/datapipe/datapipe_decl.h"

// forward-declare model class if needed
#include "transport-runtime/models/model_forward_declare.h"

// forward-declare tasks if needed
#include "transport-runtime/tasks/tasks_forward_declare.h"

// forward-declare repository records if needed
#include "transport-runtime/repository/records/repository_records_forward_declare.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"
#include "transport-runtime/derived-products/utilities/extractor.h"

#include "transport-runtime/derived-products/derived-content/SQL_query/SQL_query.h"
#include "transport-runtime/derived-products/derived-content/correlation-functions/compute-gadgets/common.h"


namespace transport
	{

    namespace derived_data
	    {

				template <typename number>
        class threepf_time_shift
	        {

		        // CONSTRUCTOR, DESTRUCTOR

          public:

						//! constructor is default
		        threepf_time_shift() = default;

						//! destructor is default
		        virtual ~threepf_time_shift() = default;


		        // SHIFT MOMENTUM -> DERIVATIVES

          public:

            //! shift a threepf timeline for coordinate labels (l,m,n)
            //! and supplied 3pf k-configuration
            void shift(twopf_db_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
                       const SQL_time_config_query& tquery, std::vector<number>& line_data, const std::vector<time_config>& t_axis,
                       unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& config) const;

          protected:

            //! apply the derivative shift to a threepf-timeline for a specific
            //! configuration
            void make_shift(twopf_db_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
                            const SQL_time_config_query& tquery, std::vector<number>& line_data,
                            const std::vector<time_config>& t_axis, const std::vector<typename std::vector<number> >& background,
                            unsigned int p, const extractor& p_extract,
                            unsigned int q, const extractor& q_extract,
                            unsigned int r, const extractor& r_extract,
                            const threepf_kconfig& config, derived_data_impl::operator_position pos) const;


		        // INTERNAL DATA

          protected:

		        model<number>* mdl;

	        };


        template <typename number>
        void threepf_time_shift<number>::shift(twopf_db_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
                                               const SQL_time_config_query& tquery, std::vector<number>& line_data, const std::vector<time_config>& t_axis,
                                               unsigned int l, unsigned int m, unsigned int n, const threepf_kconfig& config) const
			    {
						assert(mdl != nullptr);
		        assert(tk != nullptr);

		        assert(t_axis.size() == line_data.size());

		        unsigned int N_fields = mdl->get_N_fields();

		        typename datapipe<number>::time_data_handle& handle = pipe.new_time_data_handle(tquery);

		        // pull the background time evolution from the database for the time_sample we are using.
		        // for future convenience we want this to be a vector of vectors-representing-field-components,
		        // ie., for each time step (outer vector) we have a group of field components (inner vector)
		        std::vector< std::vector<number> > background(t_axis.size());

		        for(unsigned int i = 0; i < 2*N_fields; ++i)
			        {
		            background_time_data_tag<number> tag = pipe.new_background_time_data_tag(i); // DON'T flatten i, because we want to give the background to the model instance in the order it expects

                // safe to take a reference here and avoid a copy
		            const std::vector<number>& bg_line = handle.lookup_tag(tag);

		            assert(bg_line.size() == background.size());
				        for(unsigned int j = 0; j < background.size(); ++j)
			            {
						        background[j].push_back(bg_line[j]);
			            }
			        }

		        if(mdl->is_momentum(l)) this->make_shift(tk, mdl, pipe, tquery, line_data, t_axis, background,
		                                                 l, extractor(1), m, extractor(2), n, extractor(3), config, derived_data_impl::operator_position::left_pos);
		        if(mdl->is_momentum(m)) this->make_shift(tk, mdl, pipe, tquery, line_data, t_axis, background,
		                                                 m, extractor(2), l, extractor(1), n, extractor(3), config, derived_data_impl::operator_position::middle_pos);
		        if(mdl->is_momentum(n)) this->make_shift(tk, mdl, pipe, tquery, line_data, t_axis, background,
		                                                 n, extractor(3), l, extractor(1), m, extractor(2), config, derived_data_impl::operator_position::right_pos);
			    }


        template <typename number>
        void threepf_time_shift<number>::make_shift(twopf_db_task<number>* tk, model<number>* mdl, datapipe<number>& pipe,
                                                    const SQL_time_config_query& tquery, std::vector<number>& line_data,
                                                    const std::vector<time_config>& t_axis, const std::vector<std::vector<number> >& background,
                                                    unsigned int p, const extractor& p_extract,
                                                    unsigned int q, const extractor& q_extract,
                                                    unsigned int r, const extractor& r_extract,
                                                    const threepf_kconfig& config, derived_data_impl::operator_position pos) const
			    {
		        assert(mdl != nullptr);
		        assert(tk != nullptr);

		        unsigned int N_fields = mdl->get_N_fields();

		        // need multiple components of the 2pf at different k-configurations; allocate space to store them
		        std::vector< std::vector<number> > sigma_q_re(t_axis.size());
		        std::vector< std::vector<number> > sigma_r_re(t_axis.size());
		        std::vector< std::vector<number> > mom_sigma_q_re(t_axis.size());
		        std::vector< std::vector<number> > mom_sigma_r_re(t_axis.size());
		        std::vector< std::vector<number> > sigma_q_im(t_axis.size());
		        std::vector< std::vector<number> > sigma_r_im(t_axis.size());
		        std::vector< std::vector<number> > mom_sigma_q_im(t_axis.size());
		        std::vector< std::vector<number> > mom_sigma_r_im(t_axis.size());

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

		        typename datapipe<number>::time_data_handle& t_handle = pipe.new_time_data_handle(tquery);

		        // pull out components of the two-pf that we need
		        for(unsigned int i = 0; i < N_fields; ++i)
			        {
		            unsigned int q_id = mdl->flatten((q_fixed == derived_data_impl::fixed_index::first_index ? q : i), (q_fixed == derived_data_impl::fixed_index::second_index ? q : i));
		            unsigned int r_id = mdl->flatten((r_fixed == derived_data_impl::fixed_index::first_index ? r : i), (r_fixed == derived_data_impl::fixed_index::second_index ? r : i));
		            unsigned int mom_q_id = mdl->flatten((q_fixed == derived_data_impl::fixed_index::first_index ? q : mdl->momentum(i)), (q_fixed == derived_data_impl::fixed_index::second_index ? q : mdl->momentum(i)));
		            unsigned int mom_r_id = mdl->flatten((r_fixed == derived_data_impl::fixed_index::first_index ? r : mdl->momentum(i)), (r_fixed == derived_data_impl::fixed_index::second_index ? r : mdl->momentum(i)));

		            cf_time_data_tag<number> q_re_tag     = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, q_id,     q_extract.serial(config));
		            cf_time_data_tag<number> q_im_tag     = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, q_id,     q_extract.serial(config));
		            cf_time_data_tag<number> mom_q_re_tag = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, mom_q_id, q_extract.serial(config));
		            cf_time_data_tag<number> mom_q_im_tag = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, mom_q_id, q_extract.serial(config));

		            cf_time_data_tag<number> r_re_tag     = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, r_id,     r_extract.serial(config));
		            cf_time_data_tag<number> r_im_tag     = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, r_id,     r_extract.serial(config));
		            cf_time_data_tag<number> mom_r_re_tag = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_re, mom_r_id, r_extract.serial(config));
		            cf_time_data_tag<number> mom_r_im_tag = pipe.new_cf_time_data_tag(cf_data_type::cf_twopf_im, mom_r_id, r_extract.serial(config));

		            const std::vector<number> q_line_re     = t_handle.lookup_tag(q_re_tag);
		            const std::vector<number> q_line_im     = t_handle.lookup_tag(q_im_tag);
		            const std::vector<number> mom_q_line_re = t_handle.lookup_tag(mom_q_re_tag);
		            const std::vector<number> mom_q_line_im = t_handle.lookup_tag(mom_q_im_tag);
		            const std::vector<number> r_line_re     = t_handle.lookup_tag(r_re_tag);
		            const std::vector<number> r_line_im     = t_handle.lookup_tag(r_im_tag);
		            const std::vector<number> mom_r_line_re = t_handle.lookup_tag(mom_r_re_tag);
		            const std::vector<number> mom_r_line_im = t_handle.lookup_tag(mom_r_im_tag);

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

		        // work through the time sample, shifting each value appropriately
		        for(unsigned int i = 0; i < line_data.size(); ++i)
			        {
		            std::vector<number> B_qrp(N_fields*N_fields*N_fields);
		            std::vector<number> C_pqr(N_fields*N_fields*N_fields);
		            std::vector<number> C_prq(N_fields*N_fields*N_fields);

		            // evaluate B and C tensors for this time step
		            // B is symmetric on its first two indices, which are the ones we sum over, so we only need a single copy of that.
		            // C is symmetric on its first two indices but we sum over the last two. So we need to symmetrize the momenta.
		            mdl->B(tk, background[i], q_extract.comoving(config), r_extract.comoving(config), p_extract.comoving(config), t_axis[i].t, B_qrp);
		            mdl->C(tk, background[i], p_extract.comoving(config), q_extract.comoving(config), r_extract.comoving(config), t_axis[i].t, C_pqr);
		            mdl->C(tk, background[i], p_extract.comoving(config), r_extract.comoving(config), q_extract.comoving(config), t_axis[i].t, C_prq);

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


#endif //__threepf_time_shift_H_
