//
// Created by David Seery on 15/06/15.
// Copyright (c) 2015 University of Sussex. All rights reserved.
//

#ifndef CPPTRANSPORT_Y_COMMON_H
#define CPPTRANSPORT_Y_COMMON_H


#include "lexeme.h"
#include "lexical.h"
#include "lexstream.h"


namespace y
	{

    typedef lexeme::lexeme<keyword_type, character_type> lexeme_type;

    typedef lexstream<keyword_type, character_type> lexstream_type;

	}   // namespace y


#endif //CPPTRANSPORT_Y_COMMON_H
