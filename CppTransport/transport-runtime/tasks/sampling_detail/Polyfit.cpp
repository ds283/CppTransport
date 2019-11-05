//
// Created by Alessandro Maraio on 15/10/2019.
//
// Copyright (c) 2019 University of Sussex. All rights reserved.
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
// @contributor: Alessandro Maraio  <am963@sussex.ac.uk>
// @contributor: Sean Butchers      <smlb20@sussex.ac.uk>
// --@@
//

#include <vector>
#include <boost/range/adaptors.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/assert.hpp>

#include "transport-runtime/defaults.h"

namespace transport {

  // Set-up a function that fits an nDegree polynomial to equal-sized vectors oX and oY
  template <typename number = default_number_type>
  std::vector<number> polyfit(const std::vector <number> &oX,
                              const std::vector <number> &oY, int nDegree) {

    using namespace boost::numeric::ublas;

    if (oX.size() != oY.size()) throw std::invalid_argument("X and Y vector sizes do not match");

    // more intuitive this way
    nDegree++;

    size_t nCount = oX.size();
    matrix <number> oXMatrix(nCount, nDegree);
    matrix <number> oYMatrix(nCount, 1);

    // copy y matrix
    for (size_t i = 0; i < nCount; i++)
    {
      oYMatrix(i, 0) = oY[i];
    }

    // create the X matrix
    for (size_t nRow = 0; nRow < nCount; nRow++)
    {
      number nVal = 1.0;
      for (int nCol = 0; nCol < nDegree; nCol++)
      {
        oXMatrix(nRow, nCol) = nVal;
        nVal *= oX[nRow];
      }
    }

    // transpose X matrix
    matrix <number> oXtMatrix(trans(oXMatrix));

    // multiply transposed X matrix with X matrix
    matrix <number> oXtXMatrix(prec_prod(oXtMatrix, oXMatrix));

    // multiply transposed X matrix with Y matrix
    matrix <number> oXtYMatrix(prec_prod(oXtMatrix, oYMatrix));

    // LU decomposition
    permutation_matrix<int> pert(oXtXMatrix.size1());
    const std::size_t singular = lu_factorize(oXtXMatrix, pert);

    // must be singular
    BOOST_ASSERT(singular == 0);

    // backsubstitution
    lu_substitute(oXtXMatrix, pert, oXtYMatrix);

    // copy the result to coeff
    return std::vector<number>(oXtYMatrix.data().begin(), oXtYMatrix.data().end());
  }

}  // namespace transport
