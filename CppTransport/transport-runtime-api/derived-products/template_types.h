//
// Created by David Seery on 03/07/2014.
// Copyright (c) 2014 University of Sussex. All rights reserved.
//


#ifndef __template_types_H_
#define __template_types_H_

#include "transport-runtime-api/localizations/messages_en.h"


namespace transport
	{

		namespace derived_data
			{

				typedef enum { fNLlocal, fNLequi, fNLortho, fNLDBI } template_type;

				std::string template_name(template_type type)
					{
						switch(type)
							{
						    case fNLlocal: return(__CPP_TRANSPORT_FNL_LOCAL);
						    case fNLequi:  return(__CPP_TRANSPORT_FNL_EQUI);
						    case fNLortho: return(__CPP_TRANSPORT_FNL_ORTHO);
						    case fNLDBI:   return(__CPP_TRANSPORT_FNL_DBI);
						    default:
							    assert(false);
									throw runtime_exception(runtime_exception::RUNTIME_ERROR, __CPP_TRANSPORT_PRODUCT_FNL_LINE_UNKNOWN_TEMPLATE);
							}
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__template_types_H_
