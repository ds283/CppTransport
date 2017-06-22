//
// Created by Sean Butchers on 20/06/2017.
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
// @contributor: Sean Butchers <smlb20@sussex.ac.uk>
// --@@
//

#ifndef CPPTRANSPORT_NONCANONICAL_CURVATURE_CLASSES_H
#define CPPTRANSPORT_NONCANONICAL_CURVATURE_CLASSES_H

#include <iostream>
#include <ginac/ginac.h>
#include <vector>

using namespace GiNaC;

class Christoffel {
private:
    std::vector<ex> gamma;
public:
    ex temp;

    void setGamma (int N, GiNaC::matrix G, GiNaC::matrix Ginv, std::vector<GiNaC::symbol> coords);

    ex getGamma (int i, int j, int k, int N);

    long getLength ();
};

class Riemann_T {
private:
    std::vector<ex> rie_t;
public:
    ex temp;

    void setRieT (int N, GiNaC::matrix G, std::vector<GiNaC::symbol> coords, Christoffel crstfl);

    ex get_RieT (int i, int j, int k, int l, int N);

    long getLength ();
};

#endif //CPPTRANSPORT_NONCANONICAL_CURVATURE_CLASSES_H