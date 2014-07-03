//
// Created by David Seery on 03/07/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __fNL_timeseries_compute_H_
#define __fNL_timeseries_compute_H_


#include <vector>
#include <memory>

#include "transport-runtime-api/derived-products/template_types.h"

// need data_manager for datapipe
#include "transport-runtime-api/manager/data_manager.h"


namespace transport
	{

		namespace derived_data
			{

				//! fNL_timeseries_compute is a utility class which computes derived lines for fNL amplitudes
				template <typename number>
		    class fNL_timeseries_compute
			    {

		      public:

				    class handle
					    {

					      // CONSTRUCTOR, DESTRUCTOR

				      public:

				        handle(typename data_manager<number>::datapipe& pipe, integration_task<number>* tk,
				               const std::vector<unsigned int>& tsample, const std::vector<double>& taxis, unsigned int Nf,
				               template_type ty);

						    ~handle() = default;

						    // INTERNAL DATA

				      protected:

						    //! datapipe object
						    typename data_manager<number>::datapipe& pipe;

						    //! task pointer
						    threepf_task<number>* tk;

						    //! set of time serial numbers for which we are computing
						    const std::vector<unsigned int>& time_sample_sns;

						    //! cache of time axis
						    const std::vector<double>& time_axis;

						    //! datapipe handle for this set of serial numbers
						    typename data_manager<number>::datapipe::time_data_handle& t_handle;

						    //! number of fields
						    unsigned int N_fields;

						    //! template type
						    template_type type;

						    friend class fNL_timeseries_compute;

					    };


				    // CONSTRUCTOR, DESTRUCTOR

		      public:

				    // constructor is default
				    fNL_timeseries_compute() = default;

				    //! destructor is default
				    ~fNL_timeseries_compute() = default;


				    // HANDLE

		      public:

				    //! make a handle
				    std::shared_ptr<handle> make_handle(typename data_manager<number>::datapipe& pipe, integration_task<number>* tk,
				                                        const std::vector<unsigned int>& tsample, const std::vector<double>& taxis,
				                                        unsigned int Nf, template_type ty) const;


				    // COMPUTE FNL PRODUCT

		      public:

				    //! compute a timeseries
				    void fNL(std::shared_ptr<handle>& h, std::vector<number>& line_data) const;


		        // COMPUTE SELECTED SHAPE FUNCTIONS

		      protected:

		        //! compute shape function for a given bispectrum
		        void shape_function(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
		                            const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
		                            std::vector<number>& shape) const;

		        //! compute shape function for a selected template
		        void shape_function(template_type type,
		                            const std::vector<number>& twopf_k1, const std::vector<number>& twopf_k2,
		                            const std::vector<number>& twopf_k3, std::vector<number>& shape) const;


		        // BISPECTRUM SHAPES AND TEMPLATES

		      protected:

		        //! compute reference shape
		        number reference_bispectrum(number Pk1, number Pk2, number Pk3) const;

		        //! compute local template
		        number local_template(number Pk1, number Pk2, number Pk3) const;

		        //! compute equilateral template
		        number equi_template(number Pk1, number Pk2, number Pk3) const;

		        //! compute orthogonal template
		        number ortho_template(number Pk1, number Pk2, number Pk3) const;

		        //! DBI template
		        number DBI_template(number Pk1, number Pk2, number Pk3) const;

			    };


				// HANDLE METHODS


		    template <typename number>
		    fNL_timeseries_compute<number>::handle::handle(typename data_manager<number>::datapipe& p, integration_task<number>* t,
		                                                   const std::vector<unsigned int>& tsample, const std::vector<double>& taxis,
		                                                   unsigned int Nf, template_type ty)
			    : pipe(p),
			      tk(dynamic_cast<threepf_task<number>*>(t)),
			      time_sample_sns(tsample),
			      time_axis(taxis),
			      t_handle(p.new_time_data_handle(tsample)),
			      N_fields(Nf),
			      type(ty)
			    {
		        assert(tk != nullptr);
				    if(tk == nullptr)
					    {
				        std::ostringstream msg;
						    msg << __CPP_TRANSPORT_PRODUCT_FNL_TASK_NOT_THREEPF;
				        if(t != nullptr)
					        {
						        msg << " " << t->get_name();
					        }
						    throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
					    }

				    assert(tk->is_integrable());
				    if(!tk->is_integrable())
					    {
				        std::ostringstream msg;
						    msg << __CPP_TRANSPORT_PRODUCT_FNL_TASK_NOT_INTEGRABLE << " " << tk->get_name();
				        throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
					    }
			    }


				// IMPLEMENTATION


		    template <typename number>
		    std::shared_ptr<typename fNL_timeseries_compute<number>::handle>
		    fNL_timeseries_compute<number>::make_handle(typename data_manager<number>::datapipe& pipe, integration_task<number>* t,
		                                                const std::vector<unsigned int>& tsample, const std::vector<double>& taxis, unsigned int Nf,
		                                                template_type ty) const
			    {
		        return std::shared_ptr<handle>(new handle(pipe, t, tsample, taxis, Nf, ty));
			    }


		    template <typename number>
		    void fNL_timeseries_compute<number>::fNL(std::shared_ptr<typename fNL_timeseries_compute<number>::handle>& h, std::vector<number>& line_data) const
			    {
				    // get all threepf configurations from the task
		        const std::vector<threepf_kconfig>& raw_kconfigs = h->tk->get_threepf_kconfig_list();

		        std::vector<unsigned int> kserials(raw_kconfigs.size());
		        for(unsigned int i = 0; i < raw_kconfigs.size(); i++)
			        {
		            kserials[i] = raw_kconfigs[i].serial;
			        }

		        // set up cache handles
		        typename data_manager<number>::datapipe::threepf_kconfig_handle& kc_handle = h->pipe.new_threepf_kconfig_handle(kserials);
		        typename data_manager<number>::datapipe::time_zeta_handle& z_handle = h->pipe.new_time_zeta_handle(h->time_sample_sns);

		        // pull 3pf k-configuration information from the database
		        typename data_manager<number>::datapipe::threepf_kconfig_tag k_tag = h->pipe.new_threepf_kconfig_tag();
		        const typename std::vector< typename data_manager<number>::threepf_configuration > k_values = kc_handle.lookup_tag(k_tag);

		        std::vector<number> BT_line;  // will hold <B, T>
		        std::vector<number> TT_line;  // will hold <T, T>
		        BT_line.assign(h->time_sample_sns.size(), 0.0);
		        TT_line.assign(h->time_sample_sns.size(), 0.0);

				    // loop over all sampled k-configurations, adding their contributions to the integral
		        for(unsigned int i = 0; i < k_values.size(); i++)
			        {
		            typename data_manager<number>::datapipe::zeta_threepf_time_data_tag bsp_tag = h->pipe.new_zeta_threepf_time_data_tag(k_values[i]);

		            // pull bispectrum information for this triangle
		            const std::vector<number> bispectrum = z_handle.lookup_tag(bsp_tag);

		            typename data_manager<number>::twopf_configuration k1;
		            typename data_manager<number>::twopf_configuration k2;
		            typename data_manager<number>::twopf_configuration k3;

		            k1.serial         = k_values[i].k1_serial;
		            k1.k_comoving     = k_values[i].k1_comoving;
		            k1.k_conventional = k_values[i].k1_conventional;

		            k2.serial         = k_values[i].k2_serial;
		            k2.k_comoving     = k_values[i].k2_comoving;
		            k2.k_conventional = k_values[i].k2_conventional;

		            k3.serial         = k_values[i].k3_serial;
		            k3.k_comoving     = k_values[i].k3_comoving;
		            k3.k_conventional = k_values[i].k3_conventional;

		            typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k1_tag = h->pipe.new_zeta_twopf_time_data_tag(k1);
		            typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k2_tag = h->pipe.new_zeta_twopf_time_data_tag(k2);
		            typename data_manager<number>::datapipe::zeta_twopf_time_data_tag k3_tag = h->pipe.new_zeta_twopf_time_data_tag(k3);

		            const std::vector<number> twopf_k1 = z_handle.lookup_tag(k1_tag);
		            const std::vector<number> twopf_k2 = z_handle.lookup_tag(k2_tag);
		            const std::vector<number> twopf_k3 = z_handle.lookup_tag(k3_tag);

				        // compute shape functions for bispectrum and template
		            std::vector<number> S_bispectrum;
		            std::vector<number> S_template;
		            this->shape_function(bispectrum, twopf_k1, twopf_k2, twopf_k3, S_bispectrum);
		            this->shape_function(h->type, twopf_k1, twopf_k2, twopf_k3, S_template);

				        // get integration measure from task
				        number measure = h->tk->measure(raw_kconfigs[i]);
		            for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
			            {
		                BT_line[j] += S_bispectrum[j]*S_template[j];
		                TT_line[j] += S_template[j]  *S_template[j];
			            }
			        }

				    line_data.clear();
				    line_data.resize(h->time_sample_sns.size());
		        for(unsigned int j = 0; j < h->time_sample_sns.size(); j++)
			        {
		            line_data[j] = (5.0/3.0) * BT_line[j]/TT_line[j];
			        }
			    }


		    // compute shape function for a supplied bispectrum
		    template <typename number>
		    void fNL_timeseries_compute<number>::shape_function(const std::vector<number>& bispectrum, const std::vector<number>& twopf_k1,
		                                                        const std::vector<number>& twopf_k2, const std::vector<number>& twopf_k3,
		                                                        std::vector<number>& shape) const
			    {
		        shape.clear();
		        shape.resize(bispectrum.size());

		        for(unsigned int j = 0; j < bispectrum.size(); j++)
			        {
		            number Bref = this->reference_bispectrum(twopf_k1[j], twopf_k2[j], twopf_k3[j]);

		            shape[j] = bispectrum[j] / Bref;
			        }
			    }


		    // compute shape function for the intended template
		    template <typename number>
		    void fNL_timeseries_compute<number>::shape_function(template_type type,
		                                                        const std::vector<number>& twopf_k1, const std::vector<number>& twopf_k2,
		                                                        const std::vector<number>& twopf_k3, std::vector<number>& shape) const
			    {
		        shape.clear();
		        shape.resize(twopf_k1.size());

		        for(unsigned int j = 0; j < twopf_k1.size(); j++)
			        {
		            number T = 0.0;

		            switch(type)
			            {
		                case fNLlocal:
			                T = this->local_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
			                break;

		                case fNLequi:
			                T = this->equi_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
			                break;

		                case fNLortho:
			                T = this->ortho_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
			                break;

		                case fNLDBI:
			                T = this->DBI_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
		                  break;

		                default:
			                assert(false);
		                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
			            }

		            number Bref = this->reference_bispectrum(twopf_k1[j], twopf_k2[j], twopf_k3[j]);

		            shape[j] = T/Bref;
			        }

			    }


		    // compute reference bispectrum -- currently uses the constant bispectrum
		    template <typename number>
		    number fNL_timeseries_compute<number>::reference_bispectrum(number Pk1, number Pk2, number Pk3) const
			    {
		        return(pow(Pk1*Pk2*Pk3, 2.0/3.0));
			    }


		    // local template
		    template <typename number>
		    number fNL_timeseries_compute<number>::local_template(number Pk1, number Pk2, number Pk3) const
			    {
		        return(2.0 * (Pk1*Pk2 + Pk1*Pk3 + Pk2*Pk3));
			    }


		    // equilateral template
		    template <typename number>
		    number fNL_timeseries_compute<number>::equi_template(number Pk1, number Pk2, number Pk3) const
			    {
		        return(6.0 * (-Pk1*Pk2 - Pk1*Pk3 -Pk2*Pk3 - 2.0*pow(Pk1*Pk2*Pk3, 2.0/3.0)
			        + pow(Pk1*Pk2*Pk2, 1.0/3.0)*Pk3 + pow(Pk1*Pk3*Pk3, 1.0/3.0)*Pk2
			        + pow(Pk2*Pk1*Pk1, 1.0/3.0)*Pk3 + pow(Pk2*Pk3*Pk3, 1.0/3.0)*Pk1
			        + pow(Pk3*Pk1*Pk1, 1.0/3.0)*Pk2 + pow(Pk3*Pk2*Pk2, 1.0/3.0)*Pk1) );
			    }


		    // orthogonal template
		    template <typename number>
		    number fNL_timeseries_compute<number>::ortho_template(number Pk1, number Pk2, number Pk3) const
			    {
		        return(6.0 * (-3.0*Pk1*Pk2 - 3.0*Pk1*Pk3 - 3.0*Pk2*Pk3 - 8.0*pow(Pk1*Pk2*Pk3, 2.0/3.0)
			        + 3.0*pow(Pk1*Pk2*Pk2, 1.0/3.0)*Pk3 + 3.0*pow(Pk1*Pk3*Pk3, 1.0/3.0)*Pk2
			        + 3.0*pow(Pk2*Pk1*Pk1, 1.0/3.0)*Pk3 + 3.0*pow(Pk2*Pk3*Pk3, 1.0/3.0)*Pk1
			        + 3.0*pow(Pk3*Pk1*Pk1, 1.0/3.0)*Pk2 + 3.0*pow(Pk3*Pk2*Pk2, 1.0/3.0)*Pk1) );
			    }


		    // DBI template
		    template <typename number>
		    number fNL_timeseries_compute<number>::DBI_template(number Pk1, number Pk2, number Pk3) const
			    {
		        return(1.0);
			    }

			}

	}


#endif //__fNL_timeseries_compute_H_
