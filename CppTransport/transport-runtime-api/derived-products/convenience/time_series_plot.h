//
// Created by David Seery on 17/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __general_time_plot_H_
#define __general_time_plot_H_


#include "transport-runtime-api/serialization/serializable.h"

#include "transport-runtime-api/derived-products/line_plot2d.h"

#include "transport-runtime-api/derived-products/utilities/index_selector.h"
#include "transport-runtime-api/derived-products/utilities/filter.h"
#include "transport-runtime-api/derived-products/utilities/wrapper.h"

#include "transport-runtime-api/defaults.h"
#include "transport-runtime-api/messages.h"


namespace transport
	{

		namespace derived_data
			{

				//! time-series-plot is a convenience class which defines a line-plot2d with
				//! sensible defaults for a time series plot

		    template <typename number>
		    class time_series_plot : public line_plot2d<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor. Accepts a filename.
		        time_series_plot(const std::string& name, const boost::filesystem::path& filename);

				    virtual ~time_series_plot() = default;


		        // SETTING DEFAULTS -- implements a 'line_plot2d' interface

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        void apply_default_labels();

		        //! (re-)set a default list of settings
		        void apply_default_settings();

			    };


		    template <typename number>
		    time_series_plot<number>::time_series_plot(const std::string& name, const boost::filesystem::path& filename)
			    : line_plot2d<number>(name, filename)
			    {
		        apply_default_settings();
		        apply_default_labels();
			    }


		    template <typename number>
		    void time_series_plot<number>::apply_default_settings()
			    {
		        // default settings are: logarithmic y axis, absolute y values, linear x axis, no reversals, x-axis label only, use LaTeX, legend

		        this->set_log_x(false);
		        this->set_log_y(true);
		        this->set_reverse_x(false);
		        this->set_reverse_y(false);
		        this->set_abs_y(true);
		        this->set_use_LaTeX(true);
		        this->set_x_label(true);
		        this->set_y_label(false);
		        this->set_title(false);
		        this->set_legend(true);
		        this->set_legend_position(line_plot2d<number>::top_right);
		        this->set_typeset_with_LaTeX(false);
			    }


		    template <typename number>
		    void time_series_plot<number>::apply_default_labels()
			    {
		        // default label set is: no y-axis label, x-axis label is time in e-folds; no title

		        if(this->get_use_LaTeX()) this->set_x_label_text(__CPP_TRANSPORT_PRODUCT_TIMEPLOT_X_LABEL_LATEX);
		        else                      this->set_x_label_text(__CPP_TRANSPORT_PRODUCT_TIMEPLOT_X_LABEL_NOLATEX);

		        this->clear_y_label_text();
		        this->clear_title_text();
			    }


		    template <typename number>
		    std::ostream& operator<<(std::ostream& out, time_series_plot<number>& obj)
			    {
		        obj.write(out);
		        return(out);
			    }


			}   // namespace derived_data


	}   // namespace transport


#endif //__general_time_plot_H_
