// backend=cuda, minver=0.06

typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef unsigned long long  ulong;

extern "C" __global__ void u2fused( ulong n, $$__PARAM_ARGS, $$__COORD_ARGS{__x}, $$__U2_ARGS{__u2}, double* __klist, double __a, double __Mp )
  {
    size_t begin = blockDim.x * blockIdx.x + threadIdx.x;

    if(begin < n)
      {
        #undef  $$__COORDINATE[A]
        #define $$__COORDINATE[A] __x_$$__A[begin]

        double __Hsq = $$__HUBBLE_SQ;
        double __eps = $$__EPSILON;

        #undef  $$__COORDINATE[A]
        #undef  __k
        #define $$__COORDINATE[A] __x_$$__A[idx]
        #define __k __klist[idx]

        for(size_t idx = begin, grid_size = blockDim.x * gridDim.x; idx < n; idx += grid_size)
          {
            $$__TEMP_POOL{"double $1 = $2;"}
            __u2_$$__A_$$__B[idx] = $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};
          }
      }
  }
