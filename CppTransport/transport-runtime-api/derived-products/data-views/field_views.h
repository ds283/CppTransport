//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __field_views_H_
#define __field_views_H_


#include <array>


#include "transport-runtime-api/derived-products/data_view.h"


namespace transport
	{

		namespace derived_data
			{

		    //! A twopf-field-kslice is a specialization of a data-view which
		    //! makes visible a component of the twopf of field fluctuations at
		    //! fixed time, over a slice of k-configuraitons

		    template <typename number>
		    class twopf_field_kslice: public data_view<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        twopf_field_kslice(std::array<2> ind, bool df=true)
			        : indices(ind), dotfield(df)
			        {
			        }

		        virtual ~twopf_field_kslice() = default;


		        // EXTRACT DATA -- implements a 'data_view' interface

		        //! Extract a 1d slice of the data, indexed by the time serial number and taken at fixed k-configuration
		        virtual std::vector<number> slice() override;


		        // INTERNAL DATA

		      protected:

		        //! Array representing the indices associated with this view
		        const std::array<2> indices;

		        //! Switch to indicate whether correlation functions involves the momentum or just dot(field)
		        const bool dotfield;
			    };


		    //! A twopf-field-tslice is a specialization of a data-view which
		    //! makes visible a component of the twopf of field fluctuations at
		    //! fixed k-configuration, over a slice of times

		    template <typename number>
		    class twopf_field_tslice: public data_view<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        twopf_field_tslice(std::array<2> ind, bool df=true)
			        : indices(ind), dotfield(df)
			        {
			        }

		        virtual ~twopf_field_tslice() = default;


		        // EXTRACT DATA -- implements a 'data_view' interface

		        //! Extract a 1d slice of the data, indexed by the time serial number and taken at fixed k-configuration
		        virtual std::vector<number> slice() override;


		        // INTERNAL DATA

		      protected:

		        //! Array representing the indices associated with this view
		        const std::array<2> indices;

		        //! Switch to indicate whether correlation functions involves the momentum or just dot(field)
		        const bool dotfield;
			    };


		    //! A threepf-field-kslice is a specialization of a data-view which
		    //! makes visible a component of the threepf of field fluctuations at
		    //! fixed time, over a slice of k-configurations

		    template <typename number>
		    class threepf_field_kslice: public data_view<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        threepf_field_kslice(std::array<3> ind, bool df=true)
			        : indices(ind), dotfield(df)
			        {
			        }

		        virtual ~threepf_field_kslice() = default;


		        // INTERNAL DATA

		      protected:

		        //! Array representing the indices associated with this view
		        const std::array<3> indices;

		        //! Switch to indicate whether correlation functions involve the momentum or just dot(field)
		        const bool dotfield;
			    };


		    //! A threepf-field-tslice is a specialization of a data-view which
		    //! makes visible a component of the threepf of field fluctuations at
		    //! fixed k-configuration, over a slice of times

		    template <typename number>
		    class threepf_field_tslice: public data_view<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        threepf_field_tslice(std::array<3> ind, bool df=true)
			        : indices(ind), dotfield(df)
			        {
			        }

		        virtual ~threepf_field_tslice() = default;


		        // INTERNAL DATA

		      protected:

		        //! Array representing the indices associated with this view
		        const std::array<3> indices;

		        //! Switch to indicate whether correlation functions involve the momentum or just dot(field)
		        const bool dotfield;
			    };

			}   // namespace derived_data

	}   // namespace transport


#endif //__field_views_H_
