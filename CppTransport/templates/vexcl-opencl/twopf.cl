// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

kernel void twopffused( ulong __n,         // number of k-configurations we are integrating
                        double __Mp,
                        $$__PARAM_ARGS,
                        global double* __x,
                        global double* __dxdt,
                        global double* __klist,
                        double __a,
                        local double* __smem )
  {
    size_t __chunk_size  = (__n + get_global_size(0) - 1) / get_global_size(0);
    size_t __chunk_start = get_global_id(0) * __chunk_size;
    size_t __chunk_end   = min((size_t)__n, __chunk_start + __chunk_size);
    size_t __thread_idx  = get_local_id(0);
    size_t __local_size  = get_local_size(0);

    #define __U2_SIZE    (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)
    #define __BACKG_SIZE (2*$$__NUMBER_FIELDS)
    #define __TWOPF_SIZE (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)

    // packing of components into the state vector
    // (remember that all k-modes for a particular component are packed together, so reads can be coalesced)
    #define __BACKG_START 0
    #define __TWOPF_START __BACKG_START + __BACKG_SIZE

    // convenience macros for accessing a particular components and k-configuration of the state vector
    #define __IN_BACKG(i,c)    __x[(__BACKG_START+i)*__n + c]
    #define __IN_TWOPF(i,j,c)  __x[(__TWOPF_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]

    #define __OUT_BACKG(i,c)   __dxdt[(__BACKG_START+i)*__n + c]
    #define __OUT_TWOPF(i,j,c) __dxdt[(__TWOPF_START+i*2*$$__NUMBER_FIELDS+j)*__n + c]

    // index into k-modes
    #define __k __smem[__thread_idx]

    // index into u2 tensor
    // we allocate all u2 tensors in a block after the k-modes, so have to offset by __local_size
    #define __U2(a,b) __smem[__local_size + __thread_idx*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    // index into twopf
    // twopfs come in a block after the u2-tensors, so we have to offset by __local_size + __U2_SIZE*__local_size
    #define __TWOPF(a,b) __smem[__local_size + __U2_SIZE*__local_size + __thread_idx*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]

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
        __k = __klist[__idx];
        __TWOPF($$__A, $$__B) = __IN_TWOPF($$__A,$$__B,__idx); $$//

        // compute u2 for the k-mode we are looking at
        $$__TEMP_POOL{"double $1 = $2;"}

        __U2($$__A,$$__B) = $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};

        __OUT_BACKG($$__A, __idx) = __u1_$$__A; $$//

        __OUT_TWOPF($$__A, $$__B, __idx)  = 0 $$// + $$__SUM_COORDS[C] __U2($$__A,$$__C) * __TWOPF($$__C,$$__B);
        __OUT_TWOPF($$__A, $$__B, __idx) += 0 $$// + $$__SUM_COORDS[C] __U2($$__B,$$__C) * __TWOPF($$__A,$$__C);
      }
  }
