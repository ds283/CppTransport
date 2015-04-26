//
// Created by David Seery on 25/04/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __cost_wavenumber_series_H_
#define __cost_wavenumber_series_H_


#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <array>
#include <stdexcept>

#include "transport-runtime-api/derived-products/derived-content/integration-analysis/cost_metrics.h"

#include "transport-runtime-api/derived-products/line-collections/data_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/derived_line.h"
#include "transport-runtime-api/derived-products/derived-content/concepts/series/wavenumber_series.h"
#include "transport-runtime-api/derived-products/derived-content/utilities/integration_task_gadget.h"


namespace transport
	{

		namespace derived_data
			{

				template <typename number>
				class cost_wavenumber: public virtual derived_line<number>
					{

						typedef enum { twopf_analysis, threepf_analysis } analysis_type;

						// CONSTRUCTOR, DESTRUCTOR

				  public:

						//! basic user-facing constructor -- 2pf task version
						cost_wavenumber(const twopf_task<number>& tk, filter::twopf_kconfig_filter& kfilter, cost_metric m=time_cost,
						                unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! basic user-facing constructor -- 3pf task version
						cost_wavenumber(const threepf_task<number>& tk, filter::threepf_kconfig_filter& kfilter, cost_metric m=time_cost,
						                unsigned int prec = __CPP_TRANSPORT_DEFAULT_PLOT_PRECISION);

						//! override copy constructor to

						//! deserialization constructor
						cost_wavenumber(Json::Value& reader, typename repository_finder<number>::task_finder& finder);

						//! destructor
						virtual ~twopf_line() = default;


						// WRITE TO A STREAM

				  public:

						//! write self-details to a stream
						virtual void write(std::ostream& out) override;


						// SERIALIZATION -- implements a serializable interface

				  public:

						//! Serialize this object
						virtual void serialize(Json::Value& writer) const override;


						// INTERNAL DATA

				  protected:

						//! integration task gadget
						integration_task_gadget<number> gadget;

						//! cost metric to use
						cost_metric metric;

						//! twopf or threepf analysis?
						analysis_type type;

					};


				template <typename number>
				cost_wavenumber<number>::cost_wavenumber(const twopf_task<number>& tk, filter::twopf_kconfig_filter& kfilter, cost_metric m, unsigned int prec)
					: derived_line<number>(tk, wavenumber_axis, std::list<axis_value>{ k_axis, efolds_exit_axis }, prec),
					  wavenumber_series<number>(tk),
					  type(twopf_analysis),
		        gadget(tk),
		        metric(m)
					{
					}


		    template <typename number>
		    cost_wavenumber<number>::cost_wavenumber(const threepf_task<number>& tk, filter::threepf_kconfig_filter& kfilter, cost_metric m, unsigned int prec)
			    : derived_line<number>(tk, wavenumber_axis, std::list<axis_value>{ k_axis, efolds_exit_axis, alpha_axis, beta_axis, squeezing_fraction_k1_axis, squeezing_fraction_k2_axis, squeezing_fraction_k3_axis }, prec),
			      wavenumber_series<number>(tk),
			      type(threepf_analysis),
			      gadget(tk),
			      metric(m)
			    {
			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__cost_wavenumber_series_H_
