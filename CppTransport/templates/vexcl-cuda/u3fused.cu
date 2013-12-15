// backend=cuda, minver=0.06

typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef unsigned long long  ulong;

extern "C" __global__ void u3fused( ulong n, $$__PARAM_ARGS, $$__COORD_ARGS{__x}, $$__U3_ARGS{__u3}, double* __k1list, double* __k2list, double* __k3list, double __a, double __Mp )
  {
    size_t begin = blockDim.x * blockIdx.x + threadIdx.x;

    if(begin < n)
    {
        #undef  $$__COORDINATE[A]
        #define $$__COORDINATE[A] __x_$$__A[begin]

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

        for(size_t idx = begin, grid_size = blockDim.x * gridDim.x; idx < n; idx += grid_size)
          {
            $$__TEMP_POOL{"double $1 = $2;"}
            __u3_$$__A_$$__B_$$__C[idx] = $$__U3_PREDEF[ABC]{__k1, __k2, __k3, __a, __Hsq, __eps};
          }
      }
  }
