// backend=cuda, minver=0.06

typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef unsigned long long  ulong;

extern "C" __global__ void twopffused( ulong n, $$__TWOPF_ARGS{__twopf}, $$__TWOPF_ARGS{__dtwopf}, $$__U2_ARGS{__u2} )
  {
    for(size_t idx = blockDim.x * blockIdx.x + threadIdx.x, grid_size = blockDim.x * gridDim.x; idx < n; idx += grid_size)
      {
        __dtwopf_$$__A_$$__B[idx]  = 0 $$// + $$__U2_NAME[AC]{__u2}[idx] * __twopf_$$__C_$$__B[idx];
        __dtwopf_$$__A_$$__B[idx] += 0 $$// + $$__U2_NAME[BC]{__u2}[idx] * __twopf_$$__A_$$__C[idx];
      }
  }
