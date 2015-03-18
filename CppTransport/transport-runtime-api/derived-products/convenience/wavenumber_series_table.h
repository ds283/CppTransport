//
// Created by David Seery on 26/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __wavenumber_series_table_H_
#define __wavenumber_series_table_H_


#include "transport-runtime-api/derived-products/line_asciitable.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"
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
				    void apply_default_labels();

			    };


				template <typename number>
				wavenumber_series_table<number>::wavenumber_series_table(const std::string& name, const boost::filesystem::path& filename)
					: line_asciitable<number>(name, filename)
					{
						apply_default_labels();
					}


				template <typename number>
				void wavenumber_series_table<number>::apply_default_labels()
					{
						this->set_x_label(__CPP_TRANSPORT_PRODUCT_WAVENUMBER_SERIES_TABLE_X_LABEL_NOLATEX);
					}

			}   // namespace derived_data

	}   // namespace transport



#endif //__wavenumber_series_table_H_
