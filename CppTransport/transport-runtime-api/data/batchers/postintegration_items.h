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

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            //! twopf value
            number value;

            //! reduced bispectrum not used, but included here to prevent template specialization failure
            number redbsp;
	        };


        //! Stores a zeta threepf configuration
		    //! (we distinguish between threepf configurations and reduced bispectrum configurations, whose classes
		    //! carry the same data, so we can adjust template behaviour using type traits)
        class zeta_threepf_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            //! kconfig serial number of this configuration
            unsigned int kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int source_serial;

            //! threepf
            number value;

            //! reduced bispectrum
            number redbsp;
	        };


        //! Used to tag a zeta bispectrum contribution via data traits, but has no meaning beyond this
        class zeta_redbsp_item
	        {
          public:
	        };


        //! Stores an fNL configuration
        class fNL_item
	        {
          public:

            //! time serial number for this configuration
            unsigned int time_serial;

            // bispectrum.bispectrum
            number BB;

            // bispectrum.template
            number BT;

            // template.template
            number TT;
	        };


        //! Stores a linear gauge transformation
        class gauge_xfm1_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

            // values
            std::vector<number> elements;
          };


        //! Stores a quadratic gauge transformation
        class gauge_xfm2_item
          {
          public:

            //! time serial number for this configuration
            unsigned int        time_serial;

            //! kconfig serial number for this configuration
            unsigned int        kconfig_serial;

            //! kconfig serial number for the integration which produced these values. Used when unwinding a batch.
            unsigned int        source_serial;

            // values
            std::vector<number> elements;
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
