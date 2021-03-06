//
// Created by David Seery on 05/05/2014.
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This file is part of the CppTransport platform.
//
// CppTransport is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// CppTransport is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CppTransport.  If not, see <http://www.gnu.org/licenses/>.
//
// @license: GPL-2
// @contributor: David Seery <D.Seery@sussex.ac.uk>
// --@@
//


#ifndef CPPTRANSPORT_MESSAGES_EN_SERIALIZATION_H
#define CPPTRANSPORT_MESSAGES_EN_SERIALIZATION_H


#define CPPTRANSPORT_SERIAL_PUSHEMPTY         "Serialization error: push stack is empty, but attempt to write new node"
#define CPPTRANSPORT_SERIAL_PUSHEMPTYATTR     "Serialization error: push stack is empty, but attempt to write new attribute"
#define CPPTRANSPORT_SERIAL_PUSHEMPTYVALUE    "Serialization error: push stack is empty, but attempt to write new value"
#define CPPTRANSPORT_SERIAL_PUSHEMTPY_MERGE   "Serialization error: push stack is empty, but attempt to merge with second serialization stack"

#define CPPTRANSPORT_SERIAL_PULLEMPTY         "Serialization error: pull stack is empty, but attempt to read node"
#define CPPTRANSPORT_SERIAL_PULLEMPTYATTR     "Serialization error: pull stack is empty, but attempt to read attribute"
#define CPPTRANSPORT_SERIAL_PULLEMPTYVALUE    "Serialization error: pull stack is empty, but attempt to read value"
#define CPPTRANSPORT_SERIAL_PULLEMPTYARRAY    "Serialization error: pull stack is empty, but attempt to read array element"

#define CPPTRANSPORT_SERIAL_NO_TOP_ARRAY      "Serialization error: attempt to read array element, but top-of-stack object is not array"

#define CPPTRANSPORT_SERIAL_ENDNODE           "Serialization error: stack is empty, but attempt to end node"
#define CPPTRANSPORT_SERIAL_ENDNAME_A         "Serialization error: current node is"
#define CPPTRANSPORT_SERIAL_ENDNAME_B         "but attempt to end node"
#define CPPTRANSPORT_SERIAL_END_ARRAY_ELEMENT "Serialization error: stack is empty, but attempt to end array element"
#define CPPTRANSPORT_TOP_OF_STACK_NOT_ARRAY   "Serialization error: ended array element, but top-of-stack object is not array"

#define CPPTRANSPORT_SERIAL_BOOKMARKS_EMPTY   "Serialization error: bookmark stack empty"
#define CPPTRANSPORT_SERIAL_BOOKMARKS_POPLAST "Serializaiton error: attempt to pop HEAD node from bookmark stack"

#define CPPTRANSPORT_SERIAL_NOT_FINISHED      "Serialization error: node stack is non-empty, but attempt to read contents"

#define CPPTRANSPORT_SERIAL_VALIDATE_FAIL     "Serialization error: validation failed for element"
#define CPPTRANSPORT_SERIAL_TYPEDETECT_FAIL   "Serialization error: run-type type detection failed"

#endif // CPPTRANSPORT_MESSAGES_EN_SERIALIZATION_H
