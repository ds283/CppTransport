//
// Created by David Seery on 13/05/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __model_list_H_
#define __model_list_H_


#include <vector>


namespace transport
	{

		// Forward declare a model object.
		// There is a circular dependency issue (for the definition of model<number>)
		// with model.h and data_manager.h, and the compromise adopted is that data_manager.h
		// is included *first*, and forward-declares model<number>.
		// This file is included by repository.h, which is included by data_manager.h, so if
		// we included model.h then we would break this structure.
		template <typename number> class model;

		//! Holds a list of models used for processing tasks

		template <typename number>
    class model_list
	    {

      public:

	      // CONSTRUCTOR, DESTRUCTOR

		    model_list() = default;
		    ~model_list() = default;


		    // INTERFACE

		    //! Push a model to the list
		    void push_back(model<number>* m) { this->models.push_back(m); }

		    //! Get a model by index
		    model<number>* get(unsigned int i) const
			    {
				    if(i >= this->models.size())
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MODEL_LIST_RANGE);

				    return(this->models[i]);
			    }

		    //! Get number of available models
		    unsigned int size() const { return(this->models.size()); }

		    //! Get front model
		    model<number>* front() const
			    {
				    if(this->models.size() == 0)
					    throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_MODEL_LIST_RANGE);

				    return(this->models.front());
			    }

		    // INTERNAL DATA

      protected:

		    //! List of models
				std::vector< model<number>* > models;

	    };

	}   // namespace transport


#endif //__model_list_H_
