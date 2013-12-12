// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#  pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#  pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

kernel void backg( ulong n, $$__PARAM_ARGS, $$__COORD_ARGS{__x}, $$__COORD_ARGS{__dxdt}, double __Mp )
  {
    size_t chunk_size  = (n + get_global_size(0) - 1) / get_global_size(0);
    size_t chunk_start = get_global_id(0) * chunk_size;
    size_t chunk_end   = min((size_t)n, chunk_start + chunk_size);

    #undef  $$__COORDINATE[A]
    #define $$__COORDINATE[A] __x_$$__A[chunk_start]

    double __Hsq = $$__HUBBLE_SQ;
    double __eps = $$__EPSILON;

    #undef  $$__COORDINATE[A]
    #undef  __k
    #define $$__COORDINATE[A] __x_$$__A[idx]

    for(size_t idx = chunk_start; idx < chunk_end; ++idx)
      {
        $$__TEMP_POOL{"double $1 = $2;"}
        __dxdt_$$__A[idx] = $$__U1_PREDEF[A]{__Hsq, __eps};
      }
  }
