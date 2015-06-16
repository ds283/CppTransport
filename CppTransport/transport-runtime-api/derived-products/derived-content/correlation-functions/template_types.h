//
// Created by David Seery on 03/07/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __template_types_H_
#define __template_types_H_

#include "transport-runtime-api/localizations/messages_en.h"


namespace transport
	{

		namespace derived_data
			{

				typedef enum { fNL_local_template, fNL_equi_template, fNL_ortho_template, fNL_DBI_template } template_type;

				std::string template_name(template_type type)
					{
						switch(type)
							{
						    case fNL_local_template: return(std::string(__CPP_TRANSPORT_FNL_LOCAL));
						    case fNL_equi_template:  return(std::string(__CPP_TRANSPORT_FNL_EQUI));
						    case fNL_ortho_template: return(std::string(__CPP_TRANSPORT_FNL_ORTHO));
						    case fNL_DBI_template:   return(std::string(__CPP_TRANSPORT_FNL_DBI));
						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
							}
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__template_types_H_
