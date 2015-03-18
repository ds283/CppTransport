//
// Created by David Seery on 05/05/2014.
// Copyright (c) 2014-15 University of Sussex. All rights reserved.
//


#ifndef __CPP_TRANSPORT_MESSAGES_EN_SERIALIZATION_H
#define __CPP_TRANSPORT_MESSAGES_EN_SERIALIZATION_H


#define __CPP_TRANSPORT_SERIAL_PUSHEMPTY         "Serialization error: push stack is empty, but attempt to write new node"
#define __CPP_TRANSPORT_SERIAL_PUSHEMPTYATTR     "Serialization error: push stack is empty, but attempt to write new attribute"
#define __CPP_TRANSPORT_SERIAL_PUSHEMPTYVALUE    "Serialization error: push stack is empty, but attempt to write new value"
#define __CPP_TRANSPORT_SERIAL_PUSHEMTPY_MERGE   "Serialization error: push stack is empty, but attempt to merge with second serialization stack"

#define __CPP_TRANSPORT_SERIAL_PULLEMPTY         "Serialization error: pull stack is empty, but attempt to read node"
#define __CPP_TRANSPORT_SERIAL_PULLEMPTYATTR     "Serialization error: pull stack is empty, but attempt to read attribute"
#define __CPP_TRANSPORT_SERIAL_PULLEMPTYVALUE    "Serialization error: pull stack is empty, but attempt to read value"
#define __CPP_TRANSPORT_SERIAL_PULLEMPTYARRAY    "Serialization error: pull stack is empty, but attempt to read array element"

#define __CPP_TRANSPORT_SERIAL_NO_TOP_ARRAY      "Serialization error: attempt to read array element, but top-of-stack object is not array"

#define __CPP_TRANSPORT_SERIAL_ENDNODE           "Serialization error: stack is empty, but attempt to end node"
#define __CPP_TRANSPORT_SERIAL_ENDNAME_A         "Serialization error: current node is"
#define __CPP_TRANSPORT_SERIAL_ENDNAME_B         "but attempt to end node"
#define __CPP_TRANSPORT_SERIAL_END_ARRAY_ELEMENT "Serialization error: stack is empty, but attempt to end array element"
#define __CPP_TRANSPORT_TOP_OF_STACK_NOT_ARRAY   "Serialization error: ended array element, but top-of-stack object is not array"

#define __CPP_TRANSPORT_SERIAL_BOOKMARKS_EMPTY   "Serialization error: bookmark stack empty"
#define __CPP_TRANSPORT_SERIAL_BOOKMARKS_POPLAST "Serializaiton error: attempt to pop HEAD node from bookmark stack"

#define __CPP_TRANSPORT_SERIAL_NOT_FINISHED      "Serialization error: node stack is non-empty, but attempt to read contents"

#define __CPP_TRANSPORT_SERIAL_VALIDATE_FAIL     "Serialization error: validation failed for element"
#define __CPP_TRANSPORT_SERIAL_TYPEDETECT_FAIL   "Serialization error: run-type type detection failed"

#endif // __CPP_TRANSPORT_MESSAGES_EN_SERIALIZATION_H
