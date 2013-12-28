// backend=cuda, minver=0.06

#if defined(_MSC_VER) || defined(__APPLE__)
typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned short      ushort;
typedef unsigned long long  ulong;
#endif

extern "C" __global__ void twopffused( ulong __n,
                                       double __Mp,
                                       $$__PARAM_ARGS,
                                       $$__COORD_ARGS{__x},
                                       double* __klist,
                                       double __a,
                                       $$__TWOPF_ARGS{__twopf},
                                       $$__COORD_ARGS{__dxdt},
                                       $$__TWOPF_ARGS{__dtwopf} )
  {
    size_t __begin = blockDim.x * blockIdx.x + threadIdx.x;

    #define __U2_SIZE    (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)
    #define __TWOPF_SIZE (2*$$__NUMBER_FIELDS*2*$$__NUMBER_FIELDS)

    // SPACE NEEDED IN SHARED MEMORY:
    // k-modes: 1 per thread in the block   = sizeof(double)*blockDim.x
    //                                      = 8*blockDim.x bytes
    // u2-tensor: 1 per thread in the block = __U2_SIZE*blockDim.x*sizeof(double)
    //                                      = 16*8*blockDim.x bytes = 128*blockDim.x bytes
    // twopf: 1 per thread in the block     = __TWOPF_SIZE*blockDim.x*sizeof(double)
    //                                      = 16*8*blockDim.x bytes = 128*blockDim.x bytes

    // TOTAL                                = (8+128+128)*blockDim.x bytes = 264*blockDim.x bytes
    extern double __shared__ __smem[];

    // index into k-modes
    #define __k __smem[threadIdx.x]

    // index into u2 tensor
    // we allocate all u2 tensors in a block after the k-modes, so have to offset by blockDim.x
    #define __U2(a,b) __smem[blockDim.x + threadIdx.x*__U2_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    // index into twopf
    // twopfs come in a block after the u2-tensors, so we have to offset by blockDim.x + __U2_SIZE*blockDim.x
    #define __TWOPF(a,b) __smem[blockDim.x + __U2_SIZE*blockDim.x + threadIdx.x*__TWOPF_SIZE + 2*$$__NUMBER_FIELDS*a + b]

    if(__begin < __n)
      {
        size_t __grid_size = blockDim.x * gridDim.x;  // number of threads in the grid = stride size per kernel

        // read coords into local variables
        // we only need one, which we can choose to be '__begin'
        double $$__COORDINATE[A] = __x_$$__A[__begin];

        double __Hsq = $$__HUBBLE_SQ;
        double __eps = $$__EPSILON;

        // cache u1 tensors for evolution of the background
        $$__TEMP_POOL{"double $1 = $2;"}
        double __u1_$$__A = $$__U1_PREDEF[A]{__Hsq, __eps};

        for(size_t __idx = __begin; __idx < __n; __idx += __grid_size)
          {
            // copy the data we need from global memory into shared memory
            __k = __klist[__idx];
            __TWOPF($$__A, $$__B) = __twopf_$$__A_$$__B[__idx]; $$//

            // compute u2 for the k-mode we are looking at
            $$__TEMP_POOL{"double $1 = $2;"}

            __U2($$__A,$$__B) = $$__U2_PREDEF[AB]{__k, __a, __Hsq, __eps};

            __dxdt_$$__A[__idx] = __u1_$$__A; $$//

            __dtwopf_$$__A_$$__B[__idx]  = 0 $$// + $$__SUM_COORDS[C] __U2($$__A,$$__C) * __TWOPF($$__C,$$__B);
            __dtwopf_$$__A_$$__B[__idx] += 0 $$// + $$__SUM_COORDS[C] __U2($$__B,$$__C) * __TWOPF($$__A,$$__C);
          }
      }
  }