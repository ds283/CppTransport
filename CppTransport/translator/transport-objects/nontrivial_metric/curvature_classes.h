#include <iostream>
#include <ginac/ginac.h>
#include <vector>

using namespace GiNaC;

class Christoffel {
private:
    std::vector<ex> gamma;
public:
    ex temp;

    void setGamma (int N, GiNaC::matrix G, GiNaC::matrix Ginv, std::vector<GiNaC::symbol> coords)
    {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                for (int k = 0; k <=j ; ++k) {
                    temp = 0.0;
                    for (int m = 0; m < N; ++m) {
                        temp += 0.5 * Ginv(i, m) * (diff(G(m, j), coords[k]) + diff(G(m, k), coords[j]) - diff(G(j, k), coords[m]));
                    }
                    gamma.push_back(temp);
                }
            }
        }
    }

    ex getGamma (int i, int j, int k, int N)
    {
        if (k > j) std::swap(k, j);
        int index = (i*N*(N+1))/2 + (j*(j+1))/2 + k;
        return gamma[index];
    }

    long getLength ()
    {
        return gamma.size();
    }
};

class Riemann_T {
private:
    std::vector<ex> rie_t;
public:
    ex temp;

    void setRieT (int N, GiNaC::matrix G, std::vector<GiNaC::symbol> coords, Christoffel crstfl)
    {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < i; ++j) {
                for (int k = 0; k < N; ++k) {
                    for (int l = 0; l < k; ++l) {
                        int r = i*(i-1)/2 + j;
                        int s = k*(k-1)/2 + l;
                        if (s > r) continue;
                        temp = 0.0;
                        for (int n = 0; n < N; ++n) {
                            temp += G(n, i) * (diff(crstfl.getGamma(n, l, j, N), coords[k]) -
                                               (diff(crstfl.getGamma(n, k, j, N), coords[l])) );
                            for (int m = 0; m < N; ++m) {
                                temp += G(n, i) * (crstfl.getGamma(n, k, m, N) * crstfl.getGamma(m, l, j, N) -
                                                   (crstfl.getGamma(n, l, m, N) * crstfl.getGamma(m, k, j, N)) );
                            }
                        }
                        rie_t.push_back(temp);
                    }
                }
            }
        }
        std::cout << "Riemann tensor computed: " << rie_t.size() << " independent components." << std::endl;
    }

    // - Function that returns the correct Rie_T from the flattened vector.
    ex get_RieT (int i, int j, int k, int l, int N)
    {
        if (i==j or k==l) return 0;

        int num = 1; // numeric factor from symmetries
        if (j > i) { // anti-symmetry in first 2 indices
            std::swap(i, j);
            num *= -1;
        }
        if (l > k) { // anti-symmetry in second 2 indices
            std::swap(k, l);
            num *= -1;
        }
        int r = i*(i-1)/2 + j; // flattened indices for each pair of indices
        int s = k*(k-1)/2 + l;
        if (s > r) { // symmetry in pairs of indices
            std::swap(i, k);
            std::swap(j, l);
            std::swap(r, s);
            num *= 1;
        }
        int index = (r*(r+1))/2 + s; // index of flattened vector after all swaps
        return num * rie_t[index]; // return numeric factor * component from flat vector
    }

    long getLength ()
    {
        return rie_t.size();
    }
};