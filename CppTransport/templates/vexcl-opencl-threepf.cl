// backend=opencl, minver=0.06

#if defined(cl_khr_fp64)
#  pragma OPENCL EXTENSION cl_khr_fp64: enable
#elif defined(cl_amd_fp64)
#  pragma OPENCL EXTENSION cl_amd_fp64: enable
#endif

kernel void twopffused( ulong n,
  $$__TWOPF_ARGS{__twopf_re_k1}, \\
  $$__TWOPF_ARGS{__twopf_im_k1}, \\
  $$__TWOPF_ARGS{__twopf_re_k2}, \\
  $$__TWOPF_ARGS{__twopf_im_k2}, \\
  $$__TWOPF_ARGS{__twopf_re_k3}, \\
  $$__TWOPF_ARGS{__twopf_im_k3}, \\
  $$__THREEPF_ARGS{__threepf}, \\
  $$__THREEPF_ARGS{__dthreepf}, \\
  $$__U2_ARGS{__u2_k1}, \\
  $$__U2_ARGS{__u2_k2}, \\
  $$__U2_ARGS{__u2_k3}, \\
  $$__U3_ARGS{__u3_k1k2k3}, \\
  $$__U3_ARGS{__u3_k2k1k3}, \\
  $$__U3_ARGS{__u3_k3k1k2} )
{
  size_t chunk_size  = (n + get_global_size(0) - 1) / get_global_size(0);
  size_t chunk_start = get_global_id(0) * chunk_size;
  size_t chunk_end   = min((size_t)n, chunk_start + chunk_size);

  for(size_t idx = chunk_start; idx < chunk_end; ++idx)
    {
      __dthreepf_$$__A_$$__B_$$__C  = 0 $$// + $$__U2_NAME[AM]{__u2_k1}[idx]*__threepf_$$__M_$$__B_$$__C[idx];
      __dthreepf_$$__A_$$__B_$$__C += 0 $$// + $$__U3_NAME[AMN]{__u3_k1k2k3}[idx]*__twopf_re_k2_$$__M_$$__B[idx]*__twopf_re_k3_$$__N_$$__C[idx];
      __dthreepf_$$__A_$$__B_$$__C += 0 $$// - $$__U3_NAME[AMN]{__u3_k1k2k3}[idx]*__twopf_im_k2_$$__M_$$__B[idx]*__twopf_im_k3_$$__N_$$__C[idx];

      __dthreepf_$$__A_$$__B_$$__C += 0 $$// + $$__U2_NAME[BM]{__u2_k2}[idx]*__threepf_$$__A_$$__M_$$__C[idx];
      __dthreepf_$$__A_$$__B_$$__C += 0 $$// + $$__U3_NAME[BMN]{__u3_k2k1k3}[idx]*__twopf_re_k1_$$__A_$$__M[idx]*__twopf_re_k3_$$__N_$$__C[idx];
      __dthreepf_$$__A_$$__B_$$__C += 0 $$// - $$__U3_NAME[BMN]{__u3_k2k1k3}[idx]*__twopf_im_k1_$$__A_$$__M[idx]*__twopf_im_k3_$$__N_$$__C[idx];

      __dthreepf_$$__A_$$__B_$$__C += 0 $$// + $$__U2_NAME[CM]{__u2_k3}[idx]*__threepf_$$__A_$$__B_$$__M[idx];
      __dthreepf_$$__A_$$__B_$$__C += 0 $$// + $$__U3_NAME[CMN]{__u3_k3k1k2}[idx]*__twopf_re_k1_$$__A_$$__M[idx]*__twopf_re_k2_$$__B_$$__N[idx];
      __dthreepf_$$__A_$$__B_$$__C += 0 $$// - $$__U3_NAME[CMN]{__u3_k3k1k2}[idx]*__twopf_im_k1_$$__A_$$__M[idx]*__twopf_im_k2_$$__B_$$__N[idx];
    }
}
