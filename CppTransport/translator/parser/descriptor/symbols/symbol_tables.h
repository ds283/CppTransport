//
// --@@
// Copyright (c) 2017 University of Sussex. All rights reserved.
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

#ifndef CPPTRANSPORT_SYMBOL_TABLES_H
#define CPPTRANSPORT_SYMBOL_TABLES_H


#include "symbol_database.h"


// forward-declare declaration records
class field_declaration;
class parameter_declaration;
class subexpr_declaration;


//! typedef for field symbol table
typedef symbol_database<field_declaration> field_symbol_table;

//! typedef for parameter symbol table
typedef symbol_database<parameter_declaration> parameter_symbol_table;

//! typedef for subexpression symbol table
typedef symbol_database<subexpr_declaration> subexpr_symbol_table;


#endif //CPPTRANSPORT_SYMBOL_TABLES_H
