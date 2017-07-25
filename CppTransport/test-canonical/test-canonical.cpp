//
// main.cpp (axion-mpi)
// axion
//
// Created by David Seery on 26/06/2013.
//
// --@@
// Copyright (c) 2016 University of Sussex. All rights reserved.
//
// This template file is part of the CppTransport platform.
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


#include "transport-runtime/transport.h"
#include "axion_mpi.h"


using DataType = double;
using StateType = std::vector<DataType>;


// ****************************************************************************


int main(int argc, char* argv[])
  {
    // set up an instance of a manager
    auto mgr = std::make_unique< transport::task_manager<DataType> >(argc, argv);

    // set up an instance of the double quadratic model,
    // using doubles, with given parameter choices
    auto model = mgr->create_model< transport::axion_mpi<DataType, StateType> >();

    mgr->process();

    return(EXIT_SUCCESS);
  }
