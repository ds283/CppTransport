//
// Created by David Seery on 17/05/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_WAVENUMBER_SERIES_PLOT_H
#define CPPTRANSPORT_WAVENUMBER_SERIES_PLOT_H


#include "transport-runtime/derived-products/line-collections/line_plot2d.h"

#include "transport-runtime/derived-products/utilities/index_selector.h"
#include "transport-runtime/derived-products/utilities/wrapper.h"

#include "transport-runtime/defaults.h"
#include "transport-runtime/messages.h"

#include "boost/log/utility/formatting_ostream.hpp"


namespace transport
	{

		namespace derived_data
			{

				//! wavenumber-series-plot is a convenience class which defines a line-plot2d with
				//! sensible defaults for a wavenumber series plot

		    template <typename number=default_number_type>
		    class wavenumber_series_plot : public line_plot2d<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        //! Basic user-facing constructor. Accepts a filename.
		        wavenumber_series_plot(const std::string& name, const boost::filesystem::path& filename);

				    virtual ~wavenumber_series_plot() = default;


		        // SETTING DEFAULTS

		      public:

		        //! (re-)set a default set of labels; should account for the LaTeX setting if desired
		        virtual void apply_default_labels(bool x_label_set=true, bool y_label_set=true, bool title_set=true) override;

		        //! (re-)set a default list of settings
		        virtual void apply_default_settings() override;

			    };


		    template <typename number>
		    wavenumber_series_plot<number>::wavenumber_series_plot(const std::string& name, const boost::filesystem::path& filename)
			    : line_plot2d<number>(name, filename)
			    {
		        apply_default_settings();
		        apply_default_labels();
			    }


		    template <typename number>
		    void wavenumber_series_plot<number>::apply_default_settings()
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
		        this->set_legend_position(legend_pos::top_right);
		        this->set_typeset_with_LaTeX(false);
			    }


		    template <typename number>
		    void wavenumber_series_plot<number>::apply_default_labels(bool x_label_set, bool y_label_set, bool title_set)
			    {
		        // only use LaTeX x-label if typesetting with LaTeX; otherwise it doesn't work

		        axis_value x_axis = this->get_x_axis_value();

		        if(x_label_set)
			        {
								switch(x_axis)
									{
								    case axis_value::unset:
								    case axis_value::k:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_K_VALUE_AXIS_LABEL_LATEX);
												else                this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_K_VALUE_AXIS_LABEL_NOLATEX);
												break;
											}

								    case axis_value::efolds_exit:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_EFOLDS_EXIT_AXIS_LABEL_LATEX);
												else                         this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_EFOLDS_EXIT_AXIS_LABEL_NOLATEX);
												break;
											}

								    case axis_value::alpha:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_ALPHA_AXIS_LABEL_LATEX);
												else                this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_ALPHA_AXIS_LABEL_NOLATEX);
												break;
											}

								    case axis_value::beta:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_BETA_AXIS_LABEL_LATEX);
												else                this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_BETA_AXIS_LABEL_NOLATEX);
												break;
											}

								    case axis_value::squeeze_k1:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K1_AXIS_LABEL_LATEX);
												else                this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K1_AXIS_LABEL_NOLATEX);
												break;
											}

								    case axis_value::squeeze_k2:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K2_AXIS_LABEL_LATEX);
												else                this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K2_AXIS_LABEL_NOLATEX);
												break;
											}

								    case axis_value::squeeze_k3:
											{
												if(this->use_LaTeX) this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K3_AXIS_LABEL_LATEX);
												else                this->internal_set_x_label_text(CPPTRANSPORT_PRODUCT_SQUEEZING_FRACTION_K3_AXIS_LABEL_NOLATEX);
												break;
											}

								    default:
									    assert(false);
									}
					    }

		        if(y_label_set) this->internal_clear_y_label_text();
		        if(title_set)   this->internal_clear_title_text();
			    }


		    template <typename number, typename Char, typename Traits>
		    std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, wavenumber_series_plot<number>& obj)
			    {
		        obj.write(out);
		        return(out);
			    }


        template <typename number, typename Char, typename Traits, typename Allocator>
        boost::log::basic_formatting_ostream<Char, Traits, Allocator>& operator<<(boost::log::basic_formatting_ostream<Char, Traits, Allocator>& out, wavenumber_series_plot<number>& obj)
          {
            obj.write(out);
            return(out);
          }


			}   // namespace derived_data


	}   // namespace transport


#endif //CPPTRANSPORT_WAVENUMBER_SERIES_PLOT_H
