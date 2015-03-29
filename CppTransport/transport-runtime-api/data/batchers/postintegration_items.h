//
// Created by David Seery on 27/03/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//


#ifndef __postintegration_items_H_
#define __postintegration_items_H_


namespace transport
	{

    // data structures for storing individual sample points from each integration

    template <typename number>
    class postintegration_items
	    {

      public:

        //! Stores a zeta twopf configuration
        class zeta_twopf_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            // value
            number value;
	        };

        //! Stores a zeta threepf configuration
        class zeta_threepf_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            // value
            number value;
	        };

        //! Stores an fNL configuration
        class fNL_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            // value
            number BT;
            number TT;
	        };


		    struct fNL_item_comparator
			    {
		      public:
				    bool operator() (const fNL_item& A, const fNL_item& B)
					    {
						    return(A.time_serial < B.time_serial);
					    }
			    };

	    };

	}   // namespace transport


#endif //__postintegration_items_H_
