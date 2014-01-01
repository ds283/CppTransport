//
// Created by David Seery on 27/12/2013.
// Copyright (c) 2013 University of Sussex. All rights reserved.
//


#ifndef __db_xml_H_
#define __db_xml_H_


// the Berkeley DB XML API wants all string provided as (const unsigned char*) types
// Here, this is done with a reinterpret_cast which may be a slight blunt solution
#define __CPP_TRANSPORT_DBXML_STRING(str) reinterpret_cast<const unsigned char*>(str)


#define __CPP_TRANSPORT_XQUERY_VALUES    "distinct-values"
#define __CPP_TRANSPORT_XQUERY_SELF      "."
#define __CPP_TRANSPORT_XQUERY_SEPARATOR "/"
#define __CPP_TRANSPORT_XQUERY_WILDCARD  "*"


#endif //__db_xml_H_
