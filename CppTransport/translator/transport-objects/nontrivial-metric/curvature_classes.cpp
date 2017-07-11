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


#include "curvature_classes.h"

#include "msg_en.h"

// Class member function definitions

//! Christoffel functions

Christoffel::Christoffel(const GiNaC::matrix& G_, const GiNaC::matrix& Ginv_, const symbol_list& c_)
  : N(G_.rows()),
    G(G_),
    Ginv(Ginv_),
    coords(c_)
  {
    if(G.rows() != G.cols()) throw std::runtime_error(ERROR_METRIC_NOT_SQUARE);
    if(Ginv.rows() != Ginv.cols()) throw std::runtime_error(ERROR_METRIC_NOT_SQUARE);
    if(G.rows() != Ginv.rows()) throw std::runtime_error(ERROR_METRIC_DIMENSION);

    // compute components of the connexion and cache them
    for(unsigned int i = 0; i < N; ++i)
      {
        for(unsigned int j = 0; j < N; ++j)
          {
            for(unsigned int k = 0; k <= j; ++k)
              {
                GiNaC::ex temp = 0;

                for(unsigned int m = 0; m < N; ++m)
                  {
                    temp += Ginv(i, m) * (diff(G(m, j), coords[k]) + diff(G(m, k), coords[j]) - diff(G(j, k), coords[m])) / 2;
                  }

                gamma.push_back(temp);
              }
          }
      }
  }


const GiNaC::ex& Christoffel::operator()(unsigned int i, unsigned int j, unsigned int k) const
  {
    if(k > j) std::swap(k, j);

    // flatten (i,j,k) index arrangement
    unsigned int index = (i * N * (N+1)) / 2 + (j*(j + 1))/2 + k;

    return gamma[index];
  }


size_t Christoffel::size() const
  {
    return gamma.size();
  }


//! Riemann_T functions

Riemann_T::Riemann_T(const Christoffel& Gamma_)
  : N(Gamma_.get_number_fields()),
    Gamma(Gamma_)
  {
    const symbol_list& coords = Gamma_.get_coords();
    const GiNaC::matrix& G = Gamma_.get_G();

    for(unsigned int i = 0; i < N; ++i)
      {
        for(unsigned int j = 0; j < i; ++j)
          {
            for(unsigned int k = 0; k < N; ++k)
              {
                for(unsigned int l = 0; l < k; ++l)
                  {
                    // compute the flatten of the index groups (i,j) and (k,l)
                    // since the Riemann tensor is invariant under these exchanges we need
                    // only compute the lower triangle r <= s
                    unsigned int r = i * (i - 1) / 2 + j;
                    unsigned int s = k * (k - 1) / 2 + l;

                    if(s > r) continue;

                    GiNaC::ex temp = 0;

                    for(int n = 0; n < N; ++n)
                      {
                        temp += G(n, i) * (diff(Gamma(n, l, j), coords[k]) - (diff(Gamma(n, k, j), coords[l])));

                        for(int m = 0; m < N; ++m)
                          {
                            temp += G(n, i) * (Gamma(n, k, m) * Gamma(m, l, j) - (Gamma(n, l, m) * Gamma(m, k, j)));
                          }
                      }

                    rie_t.push_back(temp);
                  }
              }
          }
      }
  }


GiNaC::ex Riemann_T::operator()(unsigned int i, unsigned int j, unsigned int k, unsigned int l) const
  {
    // Riemann tensor is antisymmetric on (i,j) and (k,l)
    if(i == j or k == l) return 0;

    int num = 1; // numeric factor from symmetries

    if(j > i)
      {
        // anti-symmetry in first 2 indices
        std::swap(i, j);
        num *= -1;
      }

    if(l > k)
      {
        // anti-symmetry in second 2 indices
        std::swap(k, l);
        num *= -1;
      }
    
    // flattened indices for each pair of indices
    unsigned int r = i * (i - 1) / 2 + j;
    unsigned int s = k * (k - 1) / 2 + l;

    if(s > r)
      {
        // symmetry in pairs of indices
        std::swap(i, k);
        std::swap(j, l);
        std::swap(r, s);
        num *= 1;
      }
    
    // index of flattened vector after all swaps
    unsigned int index = (r * (r + 1)) / 2 + s;
    
    // return numeric factor * component from flat vector
    return num * rie_t[index];
  }


size_t Riemann_T::size() const
  {
    return this->rie_t.size();
  }


DRiemann_T::DRiemann_T(const Riemann_T& R_)
  : N(R_.get_connexion().get_number_fields()),
    N_Rie_T((N*(N-1)*(N*N-N-2)/8)),
    R(R_)
  {
    const Christoffel& Gamma = R.get_connexion();
    const symbol_list& coords = Gamma.get_coords();
    
    for(unsigned int m = 0; m < N; ++m)
      {
        for(unsigned int i = 0; i < N; ++i)
          {
            for(unsigned int j = 0; j < i; ++j)
              {
                for(unsigned int k = 0; k < N; ++k)
                  {
                    for(unsigned int l = 0; l < k; ++l)
                      {
                        // compute the flatten of the index groups (i,j) and (k,l)
                        // since the Riemann tensor is invariant under these exchanges we need
                        // only compute the lower triangle r <= s
                        unsigned int r = i * (i - 1) / 2 + j;
                        unsigned int s = k * (k - 1) / 2 + l;

                        if(s > r) continue;

                        GiNaC::ex temp = diff(R(i, j, k, l), coords[m]);

                        for(unsigned int n = 0; n < N; ++n)
                          {
                            temp -= Gamma(n, m, i) * R(n, j, k, l);
                            temp -= Gamma(n, m, j) * R(i, n, k, l);
                            temp -= Gamma(n, m, k) * R(i, j, n, l);
                            temp -= Gamma(n, m, l) * R(i, j, k, n);
                          }

                        rie_t_covar_deriv.push_back(temp);
                      }
                  }
              }
          }
      }
  }


GiNaC::ex DRiemann_T::operator()(unsigned int i, unsigned int j, unsigned int k, unsigned int l, unsigned int m) const
  {
    // Riemann tensor is antisymmetric on (i,j) and (k,l)
    if (i==j or k==l) return 0;
    
    // flattened DRiemann consists of N copies of the flattened Riemann tensor stacked
    // next to each other
    // the m index gives us the correct offset into this stack
    int mIndex = m * N_Rie_T;
    
    int num = 1; // numeric factor from symmetries
    
    if(j > i)
      {
        // anti-symmetry in first 2 indices
        std::swap(i, j);
        num *= -1;
      }

    if(l > k)
      {
        // anti-symmetry in second 2 indices
        std::swap(k, l);
        num *= -1;
      }
    
    // flattened indices for each pair of indices
    unsigned int r = i * (i - 1) / 2 + j;
    unsigned int s = k * (k - 1) / 2 + l;
    
    if(s > r)
      {
        // symmetry in pairs of indices
        std::swap(i, k);
        std::swap(j, l);
        std::swap(r, s);
        num *= 1;
      }
    
    // index of flattened vector after all swaps with mIndex to give covariant derivative of chosen coordinate
    int index = (r * (r + 1)) / 2 + s + mIndex;

    // return numeric factor * component from flat vector
    return num * rie_t_covar_deriv[index];
  }


size_t DRiemann_T::size() const
  {
    return this->rie_t_covar_deriv.size();
  }
