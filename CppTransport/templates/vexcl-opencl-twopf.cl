// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#  pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#  pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

kernel void twopffused( ulong n, $$__TWOPF_ARGS{__twopf}, $$__TWOPF_ARGS{__dtwopf}, $$__U2_ARGS{__u2} )
{
size_t chunk_size  = (n + get_global_size(0) - 1) / get_global_size(0);
size_t chunk_start = get_global_id(0) * chunk_size;
size_t chunk_end   = min((size_t)n, chunk_start + chunk_size);

for(size_t idx = chunk_start; idx < chunk_end; ++idx)
{
__dtwopf_$$__A_$$__B[idx]  = 0 $$// + $$__U2_NAME[AC]{__u2}[idx] * __twopf_$$__C_$$__B[idx];
  __dtwopf_$$__A_$$__B[idx] += 0 $$// + $$__U2_NAME[BC]{__u2}[idx] * __twopf_$$__A_$$__C[idx];
}
}
