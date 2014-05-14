//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __slicing_H_
#define __slicing_H_


#include <vector>


namespace transport
	{

		namespace derived_data
			{

		    //! Abstract class representing a slicing, eg. by k-configuration or time
		    template <typename number>
		    class slicing
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        slicing() = default;
		        virtual ~slicing() = default;


		        // EXTRACT SLICE DATA

		      public:

		        //! extract list of serial numbers representing the slicing
		        virtual std::vector<unsigned int> serial_numbers() = 0;

		        //! extract list of numerical values representing the slicing
		        virtual std::vector<number> values() = 0;

			    };


		    //! Time-slice
		    template <typename number>
		    class time_slice: public slicing<number>
			    {

		      public:

		        // CONSTRUCTOR, DESTRUCTOR

		        time_slice()
			        : slicing<number>()
			        {
			        }

		        virtual ~time_slice() = default;


		        // EXTRACT SLICE DATA

		      public:

		        //! extract list of serial numbers representing the slicing
		        virtual std::vector<unsigned int> serial_numbers() override;

		        //! extract list of numerical values representing the slicing
		        virtual std::vector<number> values() override;


			    };


		    template <typename number>
		    std::vector<unsigned int> time_slice<number>::serial_numbers()
			    {

			    }


		    template <typename number>
		    std::vector<number> time_slice<number>::values()
			    {

			    }

			}   // namespace derived_data

	}   // namespace transport


#endif //__slicing_H_
