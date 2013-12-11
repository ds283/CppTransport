// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#  pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#  pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

__kernel void u2fused( ulong n, $$__PARAM_ARGS, $$__COORD_ARGS, $$__U2_ARGS{u2} )
  {
    size_t chunk_size  = (n + get_global_size(0) - 1) / get_global_size(0);
    size_t chunk_start = get_global_id(0) * chunk_size;
    size_t chunk_end   = min(n, chunk_start + chunk_size);

    $$__TEMP_POOL{double $1 = $2;}

    for(size_t idx = chunk_start; idx < chunk_end; ++idx)
      {
        $$__U2_ARG[AB][idx] = ;
      }
  }
