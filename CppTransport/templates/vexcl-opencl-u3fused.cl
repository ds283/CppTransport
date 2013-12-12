// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#  pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#  pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

kernel void u3fused( ulong n, $$__PARAM_ARGS, $$__COORD_ARGS{__x}, $$__U3_ARGS{__u3}, global double* __k1list, global double* __k2list, global double* __k3list, double __a, double __Mp )
  {
    size_t chunk_size  = (n + get_global_size(0) - 1) / get_global_size(0);
    size_t chunk_start = get_global_id(0) * chunk_size;
    size_t chunk_end   = min((size_t)n, chunk_start + chunk_size);

    #undef  $$__COORDINATE[A]
    #define $$__COORDINATE[A] __x_$$__A[chunk_start]

    double __Hsq = $$__HUBBLE_SQ;
    double __eps = $$__EPSILON;

    #undef  $$__COORDINATE[A]
    #undef  __k1
    #undef  __k2
    #undef  __k3
    #define $$__COORDINATE[A] __x_$$__A[idx]
    #define __k1 __k1list[idx]
    #define __k2 __k2list[idx]
    #define __k3 __k3list[idx]

    for(size_t idx = chunk_start; idx < chunk_end; ++idx)
      {
        $$__TEMP_POOL{"double $1 = $2;"}
        __u3_$$__A_$$__B_$$__C[idx] = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};
      }
  }
