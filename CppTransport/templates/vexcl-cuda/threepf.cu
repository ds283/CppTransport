// backend=cuda, minver=0.06

#if defined(_MSC_VER) || defined(__APPLE__)
typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef unsigned long long  ulong;
#endif

extern "C" __global__ void threepffused( ulong __n,         // number of k-configurations we are integrating
                                         double __Mp,
                                         $$__PARAM_ARGS,
                                         double* __x,
                                         double* __dxdt,
                                         double* __k1_list,
                                         double* __k2_list,
                                         double* __k3_list,
                                         double __a )
  {
    size_t __begin = blockDim.x * blockIdx.x + threadIdx.x;

    #define __U2_SIZE      (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)
    #define __U3_SIZE      (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)
    #define __BACKG_SIZE   (2*$$__NUMBER_FIELDS)
    #define __TWOPF_SIZE   (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)
    #define __THREEPF_SIZE (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)

    // packing of components into the state vector
    // (remember that all k-modes for a particular component are packed together, so reads can be coalesced)
    #define __BACKG_START       0
    #define __TWOPF_RE_K1_START __BACKG_START + __BACKG_SIZE
    #define __TWOPF_IM_K1_START __TWOPF_RE_K1_START + __TWOPF_SIZE
    #define __TWOPF_RE_K2_START __TWOPF_IM_K1_START + __TWOPF_SIZE
    #define __TWOPF_IM_K2_START __TWOPF_RE_K2_START + __TWOPF_SIZE
    #define __TWOPF_RE_K3_START __TWOPF_IM_K2_START + __TWOPF_SIZE
    #define __TWOPF_IM_K3_START __TWOPF_RE_K3_START + __TWOPF_SIZE
    #define __THREEPF_START     __TWOPF_IM_K3_START + __TWOPF_SIZE

    // convenience macros for accessing a particular components and k-configuration of the state vector
    #define __IN_BACKG(i,c)          __x[(__BACKG_START+i)*__n + c]
    #define __IN_TWOPF_RE_K1(i,j,c)  __x[(__TWOPF_RE_K1_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __IN_TWOPF_IM_K1(i,j,c)  __x[(__TWOPF_IM_K1_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __IN_TWOPF_RE_K2(i,j,c)  __x[(__TWOPF_RE_K2_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __IN_TWOPF_IM_K2(i,j,c)  __x[(__TWOPF_IM_K2_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __IN_TWOPF_RE_K3(i,j,c)  __x[(__TWOPF_RE_K3_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __IN_TWOPF_IM_K3(i,j,c)  __x[(__TWOPF_IM_K3_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __IN_THREEPF(i,j,k,c)    __x[(__THREEPF_START+i*2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS+j*2*$$__NUMBER_FIELDS+k)*__n + c]

    #define __OUT_BACKG(i,c)         __dxdt[(__BACKG_START+i)*__n + c]
    #define __OUT_TWOPF_RE_K1(i,j,c) __dxdt[(__TWOPF_RE_K1_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __OUT_TWOPF_IM_K1(i,j,c) __dxdt[(__TWOPF_IM_K1_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __OUT_TWOPF_RE_K2(i,j,c) __dxdt[(__TWOPF_RE_K2_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __OUT_TWOPF_IM_K2(i,j,c) __dxdt[(__TWOPF_IM_K2_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __OUT_TWOPF_RE_K3(i,j,c) __dxdt[(__TWOPF_RE_K3_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __OUT_TWOPF_IM_K3(i,j,c) __dxdt[(__TWOPF_IM_K3_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]
    #define __OUT_THREEPF(i,j,k,c)   __dxdt[(__THREEPF_START+i*2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS+j*2*$$__NUMBER_FIELDS+k)*__n + c]

    // SPACE NEEDED IN SHARED MEMORY
    // k-modes: 3 per thread in the block   = 3*blockDim.x*sizeof(double)
    // u2-tensor: 3 per thread in the block = 3*__U2_SIZE*blockDim.x*sizeof(double)
    // u3-tensor: 3 per thread in the block = 3*__U3_SIZE*blockDim.x*sizeof(double)
    // twopf: 6 per thread in the block     = 6*__TWOPF_SIZE*blockDim.x*sizeof(double)
    // threepf: 1 per thread in the block   = __THREEPF_SIZE*blockDim.x*sizeof(double)

    // TOTAL                                = ( 3*(1 + __U2_SIZE + __U3_SIZE + 2*__TWOPF_SIZE) + __THREEPF_SIZE )*blockDim.x*sizeof(double)
    extern double __shared__ __smem[];

    // index into k-modes
    #define __k1 __smem[3*threadIdx.x]
    #define __k2 __smem[3*threadIdx.x+1]
    #define __k3 __smem[3*threadIdx.x+2]

    // index into u2-tensors
    #define __U2_k1(a,b) __smem[3*blockDim.x + 3*threadIdx.x*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __U2_k2(a,b) __smem[3*blockDim.x + 3*threadIdx.x*__U2_SIZE + 1*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __U2_k3(a,b) __smem[3*blockDim.x + 3*threadIdx.x*__U2_SIZE + 2*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    // index into u3-tensors
    #define __U3_k1k2k3(a,b,c) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*threadIdx.x*__U3_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]
    #define __U3_k2k1k3(a,b,c) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*threadIdx.x*__U3_SIZE + 1*__U3_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]
    #define __U3_k3k1k2(a,b,c) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*threadIdx.x*__U3_SIZE + 2*__U3_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]

    // index into twopfs
    #define __TWOPF_RE_k1(a,b) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*threadIdx.x*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_IM_k1(a,b) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*threadIdx.x*__TWOPF_SIZE + 1*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_RE_k2(a,b) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*threadIdx.x*__TWOPF_SIZE + 2*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_IM_k2(a,b) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*threadIdx.x*__TWOPF_SIZE + 3*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_RE_k3(a,b) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*threadIdx.x*__TWOPF_SIZE + 4*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_IM_k3(a,b) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*threadIdx.x*__TWOPF_SIZE + 5*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    // index into threepf
    #define __THREEPF(a,b,c) __smem[3*blockDim.x + 3*blockDim.x*__U2_SIZE + 3*blockDim.x*__U3_SIZE + 6*blockDim.x*__TWOPF_SIZE + threadIdx.x*__THREEPF_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]

    if(__begin < __n)
      {
        size_t __grid_size = blockDim.x * gridDim.x;  // number of threads in the grid = stride size per kernel

        // read coords into local variables
        // we only need one, which we can choose to be '__begin'
        double $$__COORDINATE[A] = __IN_BACKG($$__A,__begin);

        double __Hsq = $$__HUBBLE_SQ;
        double __eps = $$__EPSILON;

        // cache u1 tensors for evolution of the background
        $$__TEMP_POOL{"double $1 = $2;"}
        double __u1_$$__A = $$__U1_PREDEF[A]{__Hsq, __eps};

        for(size_t __idx = __begin; __idx < __n; __idx += __grid_size)
          {
            // copy the data we need from global memory into shared memory
            __k1 = __k1_list[__idx];
            __k2 = __k2_list[__idx];
            __k3 = __k3_list[__idx];
            __TWOPF_RE_k1($$__A, $$__B)    = __IN_TWOPF_RE_K1($$__A,$$__B,__idx); $$//
            __TWOPF_IM_k1($$__A, $$__B)    = __IN_TWOPF_IM_K1($$__A,$$__B,__idx); $$//
            __TWOPF_RE_k2($$__A, $$__B)    = __IN_TWOPF_RE_K2($$__A,$$__B,__idx); $$//
            __TWOPF_IM_k2($$__A, $$__B)    = __IN_TWOPF_IM_K2($$__A,$$__B,__idx); $$//
            __TWOPF_RE_k3($$__A, $$__B)    = __IN_TWOPF_RE_K3($$__A,$$__B,__idx); $$//
            __TWOPF_IM_k3($$__A, $$__B)    = __IN_TWOPF_IM_K3($$__A,$$__B,__idx); $$//
            __THREEPF($$__A, $$__B, $$__C) = __IN_THREEPF($$__A,$$__B,$$__C,__idx); $$//

            $$__TEMP_POOL{"double $1 = $2;"}
            // compute u2 tensors
            __U2_k1($$__A, $$__B) = $$__U2_PREDEF[AB]{__k1, __a, __Hsq, __eps};
            __U2_k2($$__A, $$__B) = $$__U2_PREDEF[AB]{__k2, __a, __Hsq, __eps};
            __U2_k3($$__A, $$__B) = $$__U2_PREDEF[AB]{__k3, __a, __Hsq, __eps};

            // compute u3 tensors
            __U3_k1k2k3($$__A, $$__B, $$__C) = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};
            __U3_k2k1k3($$__A, $$__B, $$__C) = $$__U3_PREDEF[ABC]{__k2, __k1, __k3, __a, __Hsq, __eps};
            __U3_k3k1k2($$__A, $$__B, $$__C) = $$__U3_PREDEF[ABC]{__k3, __k1, __k2, __a, __Hsq, __eps};

            __OUT_BACKG($$__A,__idx) = __u1_$$__A; $$//

            __OUT_TWOPF_RE_K1($$__A,$$__B,__idx)  = 0 $$// + $$__SUM_COORDS[C] __U2_k1($$__A,$$__C) * __TWOPF_RE_k1($$__C,$$__B);
            __OUT_TWOPF_RE_K1($$__A,$$__B,__idx) += 0 $$// + $$__SUM_COORDS[C] __U2_k1($$__B,$$__C) * __TWOPF_RE_k1($$__A,$$__C);

            __OUT_TWOPF_IM_K1($$__A,$$__B,__idx)  = 0 $$// + $$__SUM_COORDS[C] __U2_k1($$__A,$$__C) * __TWOPF_IM_k1($$__C,$$__B);
            __OUT_TWOPF_IM_K1($$__A,$$__B,__idx) += 0 $$// + $$__SUM_COORDS[C] __U2_k1($$__B,$$__C) * __TWOPF_IM_k1($$__A,$$__C);

            __OUT_TWOPF_RE_K2($$__A,$$__B,__idx)  = 0 $$// + $$__SUM_COORDS[C] __U2_k2($$__A,$$__C) * __TWOPF_RE_k2($$__C,$$__B);
            __OUT_TWOPF_RE_K2($$__A,$$__B,__idx) += 0 $$// + $$__SUM_COORDS[C] __U2_k2($$__B,$$__C) * __TWOPF_RE_k2($$__A,$$__C);

            __OUT_TWOPF_IM_K2($$__A,$$__B,__idx)  = 0 $$// + $$__SUM_COORDS[C] __U2_k2($$__A,$$__C) * __TWOPF_IM_k2($$__C,$$__B);
            __OUT_TWOPF_IM_K2($$__A,$$__B,__idx) += 0 $$// + $$__SUM_COORDS[C] __U2_k2($$__B,$$__C) * __TWOPF_IM_k2($$__A,$$__C);

            __OUT_TWOPF_RE_K3($$__A,$$__B,__idx)  = 0 $$// + $$__SUM_COORDS[C] __U2_k3($$__A,$$__C) * __TWOPF_RE_k3($$__C,$$__B);
            __OUT_TWOPF_RE_K3($$__A,$$__B,__idx) += 0 $$// + $$__SUM_COORDS[C] __U2_k3($$__B,$$__C) * __TWOPF_RE_k3($$__A,$$__C);

            __OUT_TWOPF_IM_K3($$__A,$$__B,__idx)  = 0 $$// + $$__SUM_COORDS[C] __U2_k3($$__A,$$__C) * __TWOPF_IM_k3($$__C,$$__B);
            __OUT_TWOPF_IM_K3($$__A,$$__B,__idx) += 0 $$// + $$__SUM_COORDS[C] __U2_k3($$__B,$$__C) * __TWOPF_IM_k3($$__A,$$__C);

            __OUT_THREEPF($$__A,$$__B,$$__C,__idx)  = 0 $$// + $$__SUM_COORDS[M] __U2_k1($$__A,$$__M)*__THREEPF($$__M,$$__B,$$__C);
            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// + $$__SUM_2COORDS[MN] __U3_k1k2k3($$__A,$$__M,$$__N)*__TWOPF_RE_k2($$__M,$$__B)*__TWOPF_RE_k3($$__N,$$__C);
            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// - $$__SUM_2COORDS[MN] __U3_k1k2k3($$__A,$$__M,$$__N)*__TWOPF_IM_k2($$__M,$$__B)*__TWOPF_IM_k3($$__N,$$__C);

            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// + $$__SUM_COORDS[M] __U2_k2($$__B,$$__M)*__THREEPF($$__A,$$__M,$$__C);
            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// + $$__SUM_2COORDS[MN] __U3_k2k1k3($$__B,$$__M,$$__N)*__TWOPF_RE_k1($$__A,$$__M)*__TWOPF_RE_k3($$__N,$$__C);
            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// - $$__SUM_2COORDS[MN] __U3_k2k1k3($$__B,$$__M,$$__N)*__TWOPF_IM_k1($$__A,$$__M)*__TWOPF_IM_k3($$__N,$$__C);

            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// + $$__SUM_COORDS[M] __U2_k3($$__C,$$__M)*__THREEPF($$__A,$$__B,$$__M);
            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// + $$__SUM_2COORDS[MN] __U3_k3k1k2($$__C,$$__M,$$__N)*__TWOPF_RE_k1($$__A,$$__M)*__TWOPF_RE_k2($$__B,$$__N);
            __OUT_THREEPF($$__A,$$__B,$$__C,__idx) += 0 $$// - $$__SUM_2COORDS[MN] __U3_k3k1k2($$__C,$$__M,$$__N)*__TWOPF_IM_k1($$__A,$$__M)*__TWOPF_IM_k2($$__B,$$__N);
          }
      }
  }
