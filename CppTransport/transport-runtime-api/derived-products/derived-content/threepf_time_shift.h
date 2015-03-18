//
// Created by David Seery on 02/06/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __threepf_time_shift_H_
#define __threepf_time_shift_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <stdexcept>

#include "transport-runtime-api/serialization/serializable.h"
// need repository in order to get the details of a repository<number>::output_group_record
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/repository.h"

// need data_manager in order to get the details of a data_manager<number>::datapipe
// (can't forward-declare because it is a nested class)
#include "transport-runtime-api/manager/data_manager.h"

#include "transport-runtime-api/derived-products/derived-content/derived_line.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"
#include "transport-runtime-api/derived-products/utilities/extractor.h"

namespace transport
	{

    // forward-declare model class
    template <typename number> class model;

    namespace derived_data
	    {

				template <typename number>
        class threepf_time_shift
	        {

          public:

            typedef enum { left, middle, right } operator_position;


		        // CONSTRUCTOR, DESTRUCTOR

          public:

		        threepf_time_shift() = default;

		        virtual ~threepf_time_shift() = default;


		        // SHIFT MOMENTUM -> DERIVATIVES

          public:

            //! shift a threepf timeline for coordinate labels (l,m,n)
            //! and supplied 2pf k-configuration serial numbers
            void shift(integration_task<number>* tk, model<number>* mdl,
                       typename data_manager<number>::datapipe& pipe,
                       const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
                       const std::vector<double>& time_axis,
                       unsigned int l, unsigned int m, unsigned int n,
                       const typename data_manager<number>::threepf_configuration& config) const;

          protected:

            //! apply the derivative shift to a threepf-timeline for a specific
            //! configuration
            void make_shift(integration_task<number>* tk, model<number>* mdl,
                            typename data_manager<number>::datapipe& pipe,
                            const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
                            const std::vector<double>& time_axis,
                            const std::vector<typename std::vector<number> >& background,
                            unsigned int p, const extractor<number>& p_config,
                            unsigned int q, const extractor<number>& q_config,
                            unsigned int r, const extractor<number>& r_config,
                            operator_position pos) const;


		        // INTERNAL DATA

          protected:

		        model<number>* mdl;

	        };


        template <typename number>
        void threepf_time_shift<number>::shift(integration_task<number>* tk, model<number>* mdl,
                                               typename data_manager<number>::datapipe& pipe,
                                               const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
                                               const std::vector<double>& time_axis,
                                               unsigned int l, unsigned int m, unsigned int n,
                                               const typename data_manager<number>::threepf_configuration& config) const
			    {
						assert(mdl != nullptr);
		        assert(tk != nullptr);

		        unsigned int N_fields = mdl->get_N_fields();

		        typename data_manager<number>::datapipe::time_data_handle& handle = pipe.new_time_data_handle(time_sample);

		        // pull the background time evolution from the database for the time_sample we are using.
		        // for future convenience we want this to be a vector of vectors-representing-field-components,
		        // ie., for each time step (outer vector) we have a group of field components (inner vector)
		        std::vector< std::vector<number> > background(time_sample.size());

		        for(unsigned int i = 0; i < 2*N_fields; i++)
			        {
		            typename data_manager<number>::datapipe::background_time_data_tag tag = pipe.new_background_time_data_tag(i); // DON'T flatten i, because we want to give the background to the model instance in the order it expects
                // safe to take a reference here and avoid a copy
		            const std::vector<number>& bg_line = handle.lookup_tag(tag);

		            assert(bg_line.size() == background.size());
		            for(unsigned int j = 0; j < time_sample.size(); j++)
			            {
		                background[j].push_back(bg_line[j]);
			            }
			        }

		        if(l >= N_fields) this->make_shift(tk, mdl, pipe, time_sample, line_data, time_axis, background, l, extractor<number>(1, config), m, extractor<number>(2, config), n, extractor<number>(3, config), left);
		        if(m >= N_fields) this->make_shift(tk, mdl, pipe, time_sample, line_data, time_axis, background, m, extractor<number>(2, config), l, extractor<number>(1, config), n, extractor<number>(3, config), middle);
		        if(n >= N_fields) this->make_shift(tk, mdl, pipe, time_sample, line_data, time_axis, background, n, extractor<number>(3, config), l, extractor<number>(1, config), m, extractor<number>(2, config), right);
			    }


        template <typename number>
        void threepf_time_shift<number>::make_shift(integration_task<number>* tk, model<number>* mdl,
                                                    typename data_manager<number>::datapipe& pipe,
                                                    const std::vector<unsigned int>& time_sample, std::vector<number>& line_data,
                                                    const std::vector<double>& time_axis, const std::vector<std::vector<number> >& background,
                                                    unsigned int p, const extractor<number>& p_config,
                                                    unsigned int q, const extractor<number>& q_config,
                                                    unsigned int r, const extractor<number>& r_config,
                                                    operator_position pos) const
			    {
		        assert(time_sample.size() == line_data.size());
		        assert(mdl != nullptr);
		        assert(tk != nullptr);

		        unsigned int N_fields = mdl->get_N_fields();

		        // need multiple components of the 2pf at different k-configurations; allocate space to store them
		        std::vector< std::vector<number> > sigma_q_re(time_sample.size());
		        std::vector< std::vector<number> > sigma_r_re(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_q_re(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_r_re(time_sample.size());
		        std::vector< std::vector<number> > sigma_q_im(time_sample.size());
		        std::vector< std::vector<number> > sigma_r_im(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_q_im(time_sample.size());
		        std::vector< std::vector<number> > mom_sigma_r_im(time_sample.size());

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

		        typename data_manager<number>::datapipe::time_data_handle& t_handle = pipe.new_time_data_handle(time_sample);

		        // pull out components of the two-pf that we need
		        for(unsigned int i = 0; i < N_fields; i++)
			        {
		            unsigned int q_id = mdl->flatten((q_fixed == first_index ? q : i), (q_fixed == second_index ? q : i));
		            unsigned int r_id = mdl->flatten((r_fixed == first_index ? r : i), (r_fixed == second_index ? r : i));
		            unsigned int mom_q_id = mdl->flatten((q_fixed == first_index ? q : mdl->momentum(i)), (q_fixed == second_index ? q : mdl->momentum(i)));
		            unsigned int mom_r_id = mdl->flatten((r_fixed == first_index ? r : mdl->momentum(i)), (r_fixed == second_index ? r : mdl->momentum(i)));

		            typename data_manager<number>::datapipe::cf_time_data_tag q_re_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, q_id,     q_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag q_im_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, q_id,     q_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_q_re_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, mom_q_id, q_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_q_im_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, mom_q_id, q_config.serial());

		            typename data_manager<number>::datapipe::cf_time_data_tag r_re_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, r_id,     r_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag r_im_tag     = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, r_id,     r_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_r_re_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_re, mom_r_id, r_config.serial());
		            typename data_manager<number>::datapipe::cf_time_data_tag mom_r_im_tag = pipe.new_cf_time_data_tag(data_manager<number>::datapipe::cf_twopf_im, mom_r_id, r_config.serial());

		            const std::vector<number> q_line_re     = t_handle.lookup_tag(q_re_tag);
		            const std::vector<number> q_line_im     = t_handle.lookup_tag(q_im_tag);
		            const std::vector<number> mom_q_line_re = t_handle.lookup_tag(mom_q_re_tag);
		            const std::vector<number> mom_q_line_im = t_handle.lookup_tag(mom_q_im_tag);
		            const std::vector<number> r_line_re     = t_handle.lookup_tag(r_re_tag);
		            const std::vector<number> r_line_im     = t_handle.lookup_tag(r_im_tag);
		            const std::vector<number> mom_r_line_re = t_handle.lookup_tag(mom_r_re_tag);
		            const std::vector<number> mom_r_line_im = t_handle.lookup_tag(mom_r_im_tag);

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

		        // work through the time sample, shifting each value appropriately
		        for(unsigned int i = 0; i < line_data.size(); i++)
			        {
		            std::vector< std::vector< std::vector<number> > > B_qrp;
		            std::vector< std::vector< std::vector<number> > > C_pqr;
		            std::vector< std::vector< std::vector<number> > > C_prq;

		            // evaluate B and C tensors for this time step
		            // B is symmetric on its first two indices, which are the ones we sum over, so we only need a single copy of that.
		            // C is symmetric on its first two indices but we sum over the last two. So we need to symmetrize the momenta.
		            mdl->B(tk->get_params(), background[i], q_config.comoving(), r_config.comoving(), p_config.comoving(), time_axis[i], B_qrp);
		            mdl->C(tk->get_params(), background[i], p_config.comoving(), q_config.comoving(), r_config.comoving(), time_axis[i], C_pqr);
		            mdl->C(tk->get_params(), background[i], p_config.comoving(), r_config.comoving(), q_config.comoving(), time_axis[i], C_prq);

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


#endif //__threepf_time_shift_H_
