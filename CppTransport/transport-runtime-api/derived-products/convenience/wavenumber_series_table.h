//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __wavenumber_series_table_H_
#define __wavenumber_series_table_H_


#include "transport-runtime-api/derived-products/line-collections/line_asciitable.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"


namespace transport
	{

		namespace derived_data
			{

				//! wavenumber-series-table is a convenience class which defines a line-asciitable with
				//! sensible defaults for wavenumber series

				template <typename number>
		    class wavenumber_series_table: public line_asciitable<number>
			    {

		      public:

				    // CONSTRUCTOR, DESTRUCTOR

						//! Basic user-facing constructor. Accepts a filename.
				    wavenumber_series_table(const std::string& name, const boost::filesystem::path& filename);

				    virtual ~wavenumber_series_table() = default;


				    // SETTING DEFAULTS

		      public:

				    //! (re-)set a default set of labels
				    virtual void apply_default_labels(bool x_label_set=true) override;

			    };


				template <typename number>
				wavenumber_series_table<number>::wavenumber_series_table(const std::string& name, const boost::filesystem::path& filename)
					: line_asciitable<number>(name, filename)
					{
						apply_default_labels();
					}


				template <typename number>
				void wavenumber_series_table<number>::apply_default_labels(bool x_label_set)
					{
				    axis_value x_axis = this->get_x_axis_value();

				    if(x_label_set)
					    {
				        switch(x_axis)
					        {
				            case unset_axis:
				            case k_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_K_VALUE_TABLE_LABEL_NOLATEX);
					            break;

				            case efolds_exit_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_EFOLDS_EXIT_TABLE_LABEL_NOLATEX);
				              break;

				            case alpha_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_ALPHA_TABLE_LABEL_NOLATEX);
				              break;

				            case beta_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_BETA_TABLE_LABEL_NOLATEX);
				              break;

				            case squeezing_fraction_k1_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_SQUEEZING_FRACTION_K1_TABLE_LABEL_NOLATEX);
				              break;

				            case squeezing_fraction_k2_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_SQUEEZING_FRACTION_K2_TABLE_LABEL_NOLATEX);
				              break;

				            case squeezing_fraction_k3_axis:
					            this->internal_set_x_label(__CPP_TRANSPORT_PRODUCT_SQUEEZING_FRACTION_K3_TABLE_LABEL_NOLATEX);
				              break;

				            default:
					            assert(false);
					        }
					    }
					}

			}   // namespace derived_data

	}   // namespace transport



#endif //__wavenumber_series_table_H_
