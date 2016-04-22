// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

kernel void threepffused( ulong __n,         // number of k-configurations we are integrating
                          double __Mp,
                          $$__PARAM_ARGS,
                          global double* __x,
                          global double* __dxdt,
                          global double* __k1_list,
                          global double* __k2_list,
                          global double* __k3_list,
                          double __a,
                          local double* __smem )
  {
    size_t __chunk_size  = (__n + get_global_size(0) - 1) / get_global_size(0);
    size_t __chunk_start = get_global_id(0) * __chunk_size;
    size_t __chunk_end   = min((size_t)__n, __chunk_start + __chunk_size);
    size_t __thread_idx  = get_local_id(0);
    size_t __local_size  = get_local_size(0);

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

    // index into k-modes
    #define __k1 __smem[3*__thread_idx]
    #define __k2 __smem[3*__thread_idx+1]
    #define __k3 __smem[3*__thread_idx+2]

    // index into u2-tensors
    #define __U2_k1(a,b) __smem[3*__local_size + 3*__thread_idx*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __U2_k2(a,b) __smem[3*__local_size + 3*__thread_idx*__U2_SIZE + 1*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __U2_k3(a,b) __smem[3*__local_size + 3*__thread_idx*__U2_SIZE + 2*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    // index into u3-tensors
    #define __U3_k1k2k3(a,b,c) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__thread_idx*__U3_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]
    #define __U3_k2k1k3(a,b,c) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__thread_idx*__U3_SIZE + 1*__U3_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]
    #define __U3_k3k1k2(a,b,c) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__thread_idx*__U3_SIZE + 2*__U3_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]

    // index into twopfs
    #define __TWOPF_RE_k1(a,b) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__thread_idx*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_IM_k1(a,b) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__thread_idx*__TWOPF_SIZE + 1*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_RE_k2(a,b) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__thread_idx*__TWOPF_SIZE + 2*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_IM_k2(a,b) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__thread_idx*__TWOPF_SIZE + 3*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_RE_k3(a,b) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__thread_idx*__TWOPF_SIZE + 4*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]
    #define __TWOPF_IM_k3(a,b) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__thread_idx*__TWOPF_SIZE + 5*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    // index into threepf
    #define __THREEPF(a,b,c) __smem[3*__local_size + 3*__local_size*__U2_SIZE + 3*__local_size*__U3_SIZE + 6*__local_size*__TWOPF_SIZE + __thread_idx*__THREEPF_SIZE + 2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS*a + 2*$$__NUMBER_FIELDS*b + c]

    // read coords into local variables
    // we only need one, which we can choose to be '__chunk_start'
    double $$__COORDINATE[A] = __IN_BACKG($$__A,__chunk_start);

    double __Hsq = $$__HUBBLE_SQ;
    double __eps = $$__EPSILON;

    // cache u1 tensors for evolution of the background
    $$__TEMP_POOL{"double $1 = $2;"}
    double __u1_$$__A = $$__U1_PREDEF[A]{__Hsq, __eps};

    for(size_t __idx = __chunk_start; __idx < __chunk_end; ++__idx)
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
