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