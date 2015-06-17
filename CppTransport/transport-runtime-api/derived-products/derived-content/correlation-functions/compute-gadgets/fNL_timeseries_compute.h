//
// Created by David Seery on 03/07/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __fNL_timeseries_compute_H_
#define __fNL_timeseries_compute_H_


#include <vector>
#include <memory>

#include "transport-runtime-api/derived-products/derived-content/correlation-functions/template_types.h"

// need data_manager for datapipe
#include "transport-runtime-api/data/data_manager.h"


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

				        handle(datapipe<number>& pipe, postintegration_task<number>* tk, const SQL_time_config_query& tq, template_type ty);

                handle(datapipe<number>& pipe, postintegration_task<number>* tk, const SQL_time_config_query& tq,
                       template_type ty, const typename work_queue<threepf_kconfig_record>::device_work_list& wl);

						    ~handle() = default;

              protected:

                void validate();

						    // INTERNAL DATA

				      protected:

						    //! datapipe object
						    datapipe<number>& pipe;

						    //! task pointer
						    zeta_threepf_task<number>* tk;

						    //! SQL query representing time sample
						    const SQL_time_config_query tquery;

						    //! datapipe handle for this set of serial numbers
						    typename datapipe<number>::time_data_handle& t_handle;

                //! time configuration data corresponding to this SQL query, pulled from the datapipe
                std::vector<time_config> t_axis;

						    //! template type
						    template_type type;

                //! restrict integration to a supplied set of triangles?
                bool restrict_triangles;

                //! subset of triangles to integrate, if used
                typename work_queue<threepf_kconfig_record>::device_work_list work_list;

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

				    //! make a handle, integrate over all triangles
				    std::shared_ptr<handle> make_handle(datapipe<number>& pipe, postintegration_task<number>* tk, const SQL_time_config_query& tq, template_type ty) const;

            //! make a handle, integrate over a supplied subset of triangles
            std::shared_ptr<handle> make_handle(datapipe<number>& pipe, postintegration_task<number>* tk, const SQL_time_config_query& tq,
                                                template_type ty, const typename work_queue<threepf_kconfig_record>::device_work_list& wl) const;


				    // COMPUTE FNL PRODUCT

		      public:

            //! compute a timeseries for bispectrum.template inner product
            void BT(std::shared_ptr<handle>& h, std::vector<number>& line_data) const;

            //! compute a timeseries for template.template inner product
            void TT(std::shared_ptr<handle>& h, std::vector<number>& line_data) const;

				    //! compute a timeseries for fNL
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
		    fNL_timeseries_compute<number>::handle::handle(datapipe<number>& p, postintegration_task<number>* t, const SQL_time_config_query& tq, template_type ty)
			    : pipe(p),
			      tk(dynamic_cast<zeta_threepf_task<number>*>(t)),
			      tquery(tq),
			      t_handle(p.new_time_data_handle(tq)),
			      type(ty),
            restrict_triangles(false)
			    {
            this->validate();

            // lookup time configuration data from the database
            time_config_tag<number> t_tag = p.new_time_config_tag();
            t_axis = t_handle.lookup_tag(t_tag);

				    // set up a work list for all threepf k-configurations
            const threepf_kconfig_database& threepf_db = tk->get_threepf_database();

            for(threepf_kconfig_database::const_config_iterator t = threepf_db.config_begin(); t != threepf_db.config_end(); ++t)
              {
		            work_list.enqueue_item(*t);
              }
			    }


        template <typename number>
        fNL_timeseries_compute<number>::handle::handle(datapipe<number>& p, postintegration_task<number>* t, const SQL_time_config_query& tq,
                                                       template_type ty, const typename work_queue<threepf_kconfig_record>::device_work_list& wl)
          : pipe(p),
            tk(dynamic_cast<zeta_threepf_task<number>*>(t)),
            tquery(tq),
            t_handle(p.new_time_data_handle(tq)),
            type(ty),
            restrict_triangles(true),
            work_list(wl)
          {
            this->validate();
          }


        template <typename number>
        void fNL_timeseries_compute<number>::handle::validate()
          {
            assert(this->tk != nullptr);
            if(this->tk == nullptr)
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_PRODUCT_FNL_TASK_NOT_THREEPF;
                throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
              }

            assert(this->tk->is_integrable());
            if(!this->tk->is_integrable())
              {
                std::ostringstream msg;
                msg << CPPTRANSPORT_FNL_TASK_NOT_INTEGRABLE << " " << this->tk->get_name();
                throw runtime_exception(runtime_exception::DERIVED_PRODUCT_ERROR, msg.str());
              }
          }


				// IMPLEMENTATION


		    template <typename number>
		    std::shared_ptr<typename fNL_timeseries_compute<number>::handle>
		    fNL_timeseries_compute<number>::make_handle(datapipe<number>& pipe, postintegration_task<number>* tk, const SQL_time_config_query& tq, template_type ty) const
			    {
		        return std::shared_ptr<handle>(new handle(pipe, tk, tq, ty));
			    }


        template <typename number>
        std::shared_ptr<typename fNL_timeseries_compute<number>::handle>
        fNL_timeseries_compute<number>::make_handle(datapipe<number>& pipe, postintegration_task<number>* tk, const SQL_time_config_query& tq,
                                                    template_type ty, const typename work_queue<threepf_kconfig_record>::device_work_list& wl) const
          {
            return std::shared_ptr<handle>(new handle(pipe, tk, tq, ty, wl));
          }


        template <typename number>
        void fNL_timeseries_compute<number>::BT(std::shared_ptr<typename fNL_timeseries_compute<number>::handle>& h, std::vector<number>& line_data) const
          {
            // set up cache handles
            typename datapipe<number>::time_zeta_handle& z_handle = h->pipe.new_time_zeta_handle(h->tquery);

            line_data.clear();
            line_data.resize(h->t_axis.size());

            // loop over all sampled k-configurations, adding their contributions to the integral
            for(unsigned int i = 0; i < h->work_list.size(); ++i)
              {
                zeta_threepf_time_data_tag<number> bsp_tag = h->pipe.new_zeta_threepf_time_data_tag(*(h->work_list[i]));

                // pull bispectrum information for this triangle
                const std::vector<number> bispectrum = z_handle.lookup_tag(bsp_tag);

                twopf_kconfig k1;
                twopf_kconfig k2;
                twopf_kconfig k3;

                k1.serial         = h->work_list[i]->k1_serial;
                k1.k_comoving     = h->work_list[i]->k1_comoving;
                k1.k_conventional = h->work_list[i]->k1_conventional;

                k2.serial         = h->work_list[i]->k2_serial;
                k2.k_comoving     = h->work_list[i]->k2_comoving;
                k2.k_conventional = h->work_list[i]->k2_conventional;

                k3.serial         = h->work_list[i]->k3_serial;
                k3.k_comoving     = h->work_list[i]->k3_comoving;
                k3.k_conventional = h->work_list[i]->k3_conventional;

                zeta_twopf_time_data_tag<number> k1_tag = h->pipe.new_zeta_twopf_time_data_tag(k1);
                zeta_twopf_time_data_tag<number> k2_tag = h->pipe.new_zeta_twopf_time_data_tag(k2);
                zeta_twopf_time_data_tag<number> k3_tag = h->pipe.new_zeta_twopf_time_data_tag(k3);

                const std::vector<number> twopf_k1 = z_handle.lookup_tag(k1_tag);
                const std::vector<number> twopf_k2 = z_handle.lookup_tag(k2_tag);
                const std::vector<number> twopf_k3 = z_handle.lookup_tag(k3_tag);

                // compute shape functions for bispectrum and template
                std::vector<number> S_bispectrum;
                std::vector<number> S_template;
                this->shape_function(bispectrum, twopf_k1, twopf_k2, twopf_k3, S_bispectrum);
                this->shape_function(h->type, twopf_k1, twopf_k2, twopf_k3, S_template);

                number measure = h->tk->measure(*(h->work_list[i]));
                for(unsigned int j = 0; j < h->t_axis.size(); ++j)
                  {
                    line_data[j] += measure * S_bispectrum[j] * S_template[j];
                  }
              }
          }


        template <typename number>
        void fNL_timeseries_compute<number>::TT(std::shared_ptr<typename fNL_timeseries_compute<number>::handle>& h, std::vector<number>& line_data) const
          {
            // set up cache handles
            typename datapipe<number>::time_zeta_handle& z_handle = h->pipe.new_time_zeta_handle(h->tquery);

            line_data.clear();
            line_data.resize(h->t_axis.size());

            // loop over all sampled k-configurations, adding their contributions to the integral
            for(unsigned int i = 0; i < h->work_list.size(); ++i)
              {
                twopf_kconfig k1;
                twopf_kconfig k2;
                twopf_kconfig k3;

                k1.serial         = h->work_list[i]->k1_serial;
                k1.k_comoving     = h->work_list[i]->k1_comoving;
                k1.k_conventional = h->work_list[i]->k1_conventional;

                k2.serial         = h->work_list[i]->k2_serial;
                k2.k_comoving     = h->work_list[i]->k2_comoving;
                k2.k_conventional = h->work_list[i]->k2_conventional;

                k3.serial         = h->work_list[i]->k3_serial;
                k3.k_comoving     = h->work_list[i]->k3_comoving;
                k3.k_conventional = h->work_list[i]->k3_conventional;

                zeta_twopf_time_data_tag<number> k1_tag = h->pipe.new_zeta_twopf_time_data_tag(k1);
                zeta_twopf_time_data_tag<number> k2_tag = h->pipe.new_zeta_twopf_time_data_tag(k2);
                zeta_twopf_time_data_tag<number> k3_tag = h->pipe.new_zeta_twopf_time_data_tag(k3);

                const std::vector<number> twopf_k1 = z_handle.lookup_tag(k1_tag);
                const std::vector<number> twopf_k2 = z_handle.lookup_tag(k2_tag);
                const std::vector<number> twopf_k3 = z_handle.lookup_tag(k3_tag);

                // compute shape functions for bispectrum and template
                std::vector<number> S_template;
                this->shape_function(h->type, twopf_k1, twopf_k2, twopf_k3, S_template);

                // get integration measure from task
                number measure = h->tk->measure(*(h->work_list[i]));
                for(unsigned int j = 0; j < h->t_axis.size(); ++j)
                  {
                    line_data[j] += measure * S_template[j] * S_template[j];
                  }
              }
          }


		    template <typename number>
		    void fNL_timeseries_compute<number>::fNL(std::shared_ptr<typename fNL_timeseries_compute<number>::handle>& h, std::vector<number>& line_data) const
			    {
		        std::vector<number> BT_line;  // will hold <B, T>
		        std::vector<number> TT_line;  // will hold <T, T>

            this->BT(h, BT_line);
            this->TT(h, TT_line);

				    line_data.clear();
				    line_data.resize(h->t_axis.size());
		        for(unsigned int j = 0; j < h->t_axis.size(); ++j)
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

		        for(unsigned int j = 0; j < bispectrum.size(); ++j)
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

		        for(unsigned int j = 0; j < twopf_k1.size(); ++j)
			        {
		            number T = 0.0;

		            switch(type)
			            {
		                case fNL_local_template:
			                T = this->local_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
			                break;

		                case fNL_equi_template:
			                T = this->equi_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
			                break;

		                case fNL_ortho_template:
			                T = this->ortho_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
			                break;

		                case fNL_DBI_template:
			                T = this->DBI_template(twopf_k1[j], twopf_k2[j], twopf_k3[j]);
		                  break;

		                default:
			                assert(false);
		                  throw runtime_exception(runtime_exception::RUNTIME_ERROR, CPPTRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
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
