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

				enum class template_type { fNL_local_template, fNL_equi_template, fNL_ortho_template, fNL_DBI_template };

				std::string template_name(template_type type)
					{
						switch(type)
							{
						    case template_type::fNL_local_template: return(std::string(CPPTRANSPORT_FNL_LOCAL));
						    case template_type::fNL_equi_template:  return(std::string(CPPTRANSPORT_FNL_EQUI));
						    case template_type::fNL_ortho_template: return(std::string(CPPTRANSPORT_FNL_ORTHO));
                case template_type::fNL_DBI_template:   return(std::string(CPPTRANSPORT_FNL_DBI));
							}
					}

			}   // namespace derived_data

	}   // namespace transport


#endif //__template_types_H_
