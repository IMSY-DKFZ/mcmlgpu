/*****************************************************************************
 *
 *   Header file for GPU-related data structures and kernel configurations
 *
 ****************************************************************************/
/*
 *   This file is part of GPUMCML.
 *
 *   GPUMCML is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   GPUMCML is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with GPUMCML.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GPUMCML_KERNEL_H_
#define _GPUMCML_KERNEL_H_

// We use different math intrinsics for single- and double-precision.
#ifdef SINGLE_PRECISION
#define FAST_DIV(x, y) __fdividef(x, y)
#define SQRT(x) sqrtf(x)
#define RSQRT(x) rsqrtf(x)
#define LOG(x) logf(x)
#define SINCOS(x, sptr, cptr) __sincosf(x, sptr, cptr)
#else
#define FAST_DIV(x, y) __ddiv_rn(x, y)
#define SQRT(x) sqrt(x)
#define RSQRT(x) rsqrt(x)
#define LOG(x) log(x)
#define SINCOS(x, sptr, cptr) sincos(x, sptr, cptr)
#endif

#include "gpumcml.h"
#include "gpumcml_rng.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * MCML kernel optimization parameters
 * You can tune them for the target GPU and the input model.
 *
 * - NUM_THREADS_PER_BLOCK:
 *      number of threads per thread block
 *
 * - CACHE_A_RZ_IN_SMEM:
 *      Use the shared memory to cache a portion of the absorption array A_rz
 *      that is frequently accessed.
 *      On GPUs with Compute Capability 2.0, the L1 cache is configured to
 *      have 48KB of shared memory and 16KB of true cache if this flag is set.
 *      Otherwise, the L1 is configured to have 16KB of shared memory and 48KB
 *      of true cache.
 *
 * - MAX_IR, MAX_IZ:
 *      If shared memory is used to cache A_rz (i.e., USE_TRUE_CACHE
 *      is not set), cache the portion MAX_IR x MAX_IZ of A_rz.
 *
 * - USE_32B_ELEM_FOR_ARZ_SMEM:
 *      If shared memory is used to cache A_rz (i.e., USE_TRUE_CACHE
 *      is not set), each element of the MAX_IR x MAX_IZ portion can be
 *      either 32-bit or 64-bit. To use 32-bit, enable this option.
 *      Using 32-bit saves space and allows caching more of A_rz,
 *      but requires the explicit handling of element overflow.
 *
 * - N_A_RZ_COPIES:
 *      number of copies of A_rz allocated in global memory
 *      Each block is assigned a copy to write to in a round-robin fashion.
 *      Using more copies can reduce access contention, but it increases
 *      global memory handleArgInterpretError and reduces the benefit of the L2 cache on
 *      Fermi GPUs (Compute Capability 2.0).
 *      This number should not exceed the number of thread blocks.
 *
 * - USE_64B_ATOMIC_SMEM:
 *      If the elements of A_rz cached in shared memory are 64-bit (i.e.
 *      USE_32B_ELEM_FOR_ARZ_SMEM is not set), atomically update data in the
 *      shared memory using 64-bit atomic instructions, as opposed to
 *      emulating it using two 32-bit atomic instructions.
 *      ** This feature is only available in Compute Capability 2.0.
 *
 * - USE_64B_ATOMIC_GMEM:
 *      Atomic update of the A_rz array in the global memory is done directly
 *      using a 64-bit atomic instruction, as opposed to being emulated using
 *      two 32-bit atomic instructions.
 *      ** This feature is only available in Compute Capability 1.2 and above.
 *
 * There are two potential parameters to tune:
 * - number of thread blocks
 * - the number of registers usaged by each thread
 *
 * For the first parameter, we think that it should be the same as the number
 * of SMs in a GPU, regardless of the GPU's Compute Capability. Therefore,
 * this is dynamically set in gpumcml_main.cu and not exposed as a tunable
 * parameter here.
 *
 * Since the second parameter is set at compile time, you have to tune it in
 * the makefile. This parameter is strongly correlated with parameter
 * NUM_THREADS_PER_BLOCK. Using more registers per thread forces
 * NUM_THREADS_PER_BLOCK to decrease (due to hardware resource constraint).
 */

#define NUM_THREADS_PER_BLOCK 1024
// Disable this option to test the effect of true L1 cache (48KB).
#define CACHE_A_RZ_IN_SMEM
#define MAX_IR 48
#define MAX_IZ 128
#define N_A_RZ_COPIES 4
#define USE_64B_ATOMIC_GMEM

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * Derived macros and typedefs
 *
 * You should not modify them unless you know what you are doing.
 */

#ifdef USE_32B_ELEM_FOR_ARZ_SMEM
typedef UINT32 ARZ_SMEM_TY;
#else
typedef UINT64 ARZ_SMEM_TY;
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*  Number of simulation steps performed by each thread in one kernel call
 */
#define NUM_STEPS 50000 // Use 5000 for faster response time

/*  Multi-GPU support:
    Sets the maximum number of GPUs to 6
    (assuming 3 dual-GPU cards)
*/
#define MAX_GPU_COUNT 6

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

typedef struct __align__(16)
{
    GFLOAT init_photon_w; // initial photon weight

    GFLOAT dz; // z grid separation.[cm]
    GFLOAT dr; // r grid separation.[cm]

    UINT32 na; // array range 0..na-1.
    UINT32 nz; // array range 0..nz-1.
    UINT32 nr; // array range 0..nr-1.

    UINT32 num_layers;    // number of layers.
    UINT32 A_rz_overflow; // overflow threshold for A_rz_shared
}
SimParamGPU;

typedef struct __align__(16)
{
    GFLOAT z0, z1; // z coordinates of a layer. [cm]
    GFLOAT n;      // refractive index of a layer.

    GFLOAT muas;     // mua + mus
    GFLOAT rmuas;    // 1/(mua+mus)
    GFLOAT mua_muas; // mua/(mua+mus)

    GFLOAT g; // anisotropy.

    GFLOAT cos_crit0, cos_crit1;
}
LayerStructGPU;

// The max number of layers supported (MAX_LAYERS including 2 ambient layers)
#define MAX_LAYERS 100

extern __constant__ SimParamGPU d_simparam;

extern __constant__ LayerStructGPU d_layerspecs[MAX_LAYERS];

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Thread-private states that live across batches of kernel invocations
// Each field is an array of length NUM_THREADS.
//
// We use a struct of arrays as opposed to an array of structs to enable
// global memory coalescing.
//
typedef struct
{
    // cartesian coordinates of the photon [cm]
    GFLOAT *photon_x;
    GFLOAT *photon_y;
    GFLOAT *photon_z;

    // directional cosines of the photon
    GFLOAT *photon_ux;
    GFLOAT *photon_uy;
    GFLOAT *photon_uz;

    GFLOAT *photon_w; // photon weight

    // index to layer where the photon resides
    UINT32 *photon_layer;

    UINT32 *is_active; // is this thread active?
} GPUThreadStates;

typedef struct
{
    // cartesian coordinates of the photon [cm]
    GFLOAT x;
    GFLOAT y;
    GFLOAT z;

    // directional cosines of the photon
    GFLOAT ux;
    GFLOAT uy;
    GFLOAT uz;

    GFLOAT w; // photon weight

    GFLOAT s; // step size [cm]
    // GFLOAT sleft;        // leftover step size [cm]
    // removed as an optimization to reduce code divergence

    // index to layer where the photon resides
    UINT32 layer;

    // flag to indicate if photon hits a boundary
    UINT32 hit;
} PhotonStructGPU;

UINT32 compute_Arz_overflow_count(GFLOAT init_photon_w, LayerStruct *layers, UINT32 n_layers,
                                  UINT32 n_threads_per_tblk);

__device__ void LaunchPhoton(PhotonStructGPU *photon);

__global__ void InitThreadState(GPUThreadStates tstates, UINT32 n_photons);

__device__ void SaveThreadState(SimState *d_state, GPUThreadStates *tstates, PhotonStructGPU *photon, UINT64 rnd_x,
                                UINT32 is_active);

__device__ void RestoreThreadState(SimState *d_state, GPUThreadStates *tstates, PhotonStructGPU *photon, UINT64 *rnd_x,
                                   UINT32 *rnd_a, UINT32 *is_active);

__device__ void Flush_Arz(UINT64 *g_A_rz, ARZ_SMEM_TY *s_A_rz, UINT32 saddr);

__device__ void AtomicAddULL_Shared(UINT64 *address, UINT32 add);

__device__ void AtomicAddULL_Global(UINT64 *address, UINT32 add);

__device__ void ComputeStepSize(PhotonStructGPU *photon, UINT64 *rnd_x, UINT32 *rnd_a);

__device__ int HitBoundary(PhotonStructGPU *photon);

__device__ void Hop(PhotonStructGPU *photon);

__device__ void FastReflectTransmit(PhotonStructGPU *photon, SimState *d_state_ptr, UINT64 *rnd_x, UINT32 *rnd_a);

__device__ void Spin(GFLOAT g, PhotonStructGPU *photon, UINT64 *rnd_x, UINT32 *rnd_a);

__global__ void sum_A_rz(UINT64 *g_A_rz);

//////////////////////////////////////////////////////////////////////////////
//   Main Kernel for MCML (Calls the above inline device functions)
//////////////////////////////////////////////////////////////////////////////

template <int ignoreAdetection> __global__ void MCMLKernel(SimState d_state, GPUThreadStates tstates)
{
    // photon structure stored in registers
    PhotonStructGPU photon;

    // random number seeds
    UINT64 rnd_x;
    UINT32 rnd_a;

    // Flag to indicate if this thread is active
    UINT32 is_active;

    // Restore the thread state from global memory.
    RestoreThreadState(&d_state, &tstates, &photon, &rnd_x, &rnd_a, &is_active);

    //////////////////////////////////////////////////////////////////////////

    // Coalesce consecutive weight drops to the same address.
    UINT32 last_w = 0;
    UINT32 last_ir = 0, last_iz = 0, last_addr = 0;

    //////////////////////////////////////////////////////////////////////////

#ifdef CACHE_A_RZ_IN_SMEM
    // Cache the frequently acessed region of A_rz in the shared memory.
    __shared__ ARZ_SMEM_TY A_rz_shared[MAX_IR * MAX_IZ];

    if (ignoreAdetection == 0)
    {
        // Clear the cache.
        for (int i = threadIdx.x; i < MAX_IR * MAX_IZ; i += blockDim.x)
        {
            A_rz_shared[i] = 0;
        }
        __syncthreads();
    }

#ifdef USE_32B_ELEM_FOR_ARZ_SMEM
    // Overflow handling:
    //
    // It is too spacious to keep track of whether or not each element in
    // the shared memory is about to overflow. Therefore, we divide all the
    // elements into NUM_THREADS_PER_BLOCK groups (cyclic distribution). For
    // each group, we use a single flag to keep track of if ANY element in it
    // is about to overflow. This results in the following array.
    //
    // At the end of each simulation step, if the flag for any of the groups
    // is set, the corresponding thread (with id equal to the group index)
    // flushes ALL elements in the group to the global memory.
    //
    // This array is dynamically allocated.
    //
    UINT32 *A_rz_overflow = (UINT32 *)MCMLKernel_smem;
    if (ignoreAdetection == 0)
    {
        // Clear the flags.
        A_rz_overflow[threadIdx.x] = 0;
    }
#endif

#endif

    //////////////////////////////////////////////////////////////////////////

    // Get the copy of A_rz (in the global memory) this thread writes to.
    UINT64 *g_A_rz = d_state.A_rz + (blockIdx.x % N_A_RZ_COPIES) * (d_simparam.nz * d_simparam.nr);

    //////////////////////////////////////////////////////////////////////////

    for (int iIndex = 0; iIndex < NUM_STEPS; ++iIndex)
    {
        // Only process photon if the thread is active.
        if (is_active)
        {
            //>>>>>>>>> StepSizeInTissue() in MCML
            ComputeStepSize(&photon, &rnd_x, &rnd_a);

            //>>>>>>>>> HitBoundary() in MCML
            photon.hit = HitBoundary(&photon);

            Hop(&photon);

            if (photon.hit)
            {
                FastReflectTransmit(&photon, &d_state, &rnd_x, &rnd_a);
            }
            else
            {
                //>>>>>>>>> Drop() in MCML
                GFLOAT dwa = photon.w * d_layerspecs[photon.layer].mua_muas;
                photon.w -= dwa;

                if (ignoreAdetection == 0)
                {
                    // automatic __float2uint_rz
                    UINT32 iz = FAST_DIV(photon.z, d_simparam.dz);
                    // automatic __float2uint_rz
                    UINT32 ir = FAST_DIV(SQRT(photon.x * photon.x + photon.y * photon.y), d_simparam.dr);

                    // Only record if photon is not at the edge!!
                    // This will be ignored anyways.
                    if (iz < d_simparam.nz && ir < d_simparam.nr)
                    {
                        UINT32 addr = ir * d_simparam.nz + iz;

                        if (addr != last_addr)
                        {
#ifdef CACHE_A_RZ_IN_SMEM
                            // Commit the weight drop to memory.
                            if (last_ir < MAX_IR && last_iz < MAX_IZ)
                            {
                                // Write it to the shared memory.
                                last_addr = last_ir * MAX_IZ + last_iz;
#ifdef USE_32B_ELEM_FOR_ARZ_SMEM
                                // Use 32-bit atomicAdd.
                                UINT32 oldval = atomicAdd(&A_rz_shared[last_addr], last_w);
                                // Detect overflow.
                                if (oldval >= d_simparam.A_rz_overflow)
                                {
                                    A_rz_overflow[last_addr % blockDim.x] = 1;
                                }
#else
                                // 64-bit atomic instruction
                                AtomicAddULL_Shared(&A_rz_shared[last_addr], last_w);
#endif
                            }
                            else
#endif
                            {
                                // Write it to the global memory directly.
                                AtomicAddULL_Global(&g_A_rz[last_addr], last_w);
                            }

                            last_ir = ir;
                            last_iz = iz;
                            last_addr = addr;

                            // Reset the last weight.
                            last_w = 0;
                        }

                        // Accumulate to the last weight.
                        last_w += (UINT32)(dwa * WEIGHT_SCALE);
                    }
                }
                //>>>>>>>>> end of Drop()

                Spin(d_layerspecs[photon.layer].g, &photon, &rnd_x, &rnd_a);
            }

            /***********************************************************
             *  >>>>>>>>> Roulette()
             *  If the photon weight is small, the photon packet tries
             *  to survive a roulette.
             ****/
            if (photon.w < WEIGHT)
            {
                GFLOAT rand = rand_MWC_co(&rnd_x, &rnd_a);

                // This photon survives the roulette.
                if (photon.w != MCML_FP_ZERO && rand < CHANCE)
                    photon.w *= (FP_ONE / CHANCE);
                // This photon is terminated.
                else if (atomicSub(d_state.n_photons_left, 1) > gridDim.x * blockDim.x)
                    LaunchPhoton(&photon); // Launch a new photon.
                // No need to process any more photons.
                else
                    is_active = 0;
            }
        }

        //////////////////////////////////////////////////////////////////////////

#if defined(CACHE_A_RZ_IN_SMEM) && defined(USE_32B_ELEM_FOR_ARZ_SMEM)
        if (ignoreAdetection == 0)
        {
            // Enter a phase of handling overflow in A_rz_shared.
            __syncthreads();

            if (A_rz_overflow[threadIdx.x])
            {
                // Flush all elements I am responsible for to the global memory.
                for (int i = threadIdx.x; i < MAX_IR * MAX_IZ; i += blockDim.x)
                {
                    Flush_Arz(g_A_rz, A_rz_shared, i);
                    A_rz_shared[i] = 0;
                }
                // Reset the flag.
                A_rz_overflow[threadIdx.x] = 0;
            }

            __syncthreads();
        }
#endif

        //////////////////////////////////////////////////////////////////////
    } // end of the main loop

    __syncthreads();

    if (ignoreAdetection == 0)
    {
        // Commit the last weight drop.
        // NOTE: last_w == 0 if inactive.
        if (last_w > 0)
        {
            // Commit to the global memory directly.
            // TODO: could we commit it to the shared memory, or does it matter?
            AtomicAddULL_Global(&g_A_rz[last_addr], last_w);
        }
    }

    //////////////////////////////////////////////////////////////////////////

#ifdef CACHE_A_RZ_IN_SMEM
    if (ignoreAdetection == 0)
    {
        // Flush A_rz_shared to the global memory.
        for (int i = threadIdx.x; i < MAX_IR * MAX_IZ; i += blockDim.x)
        {
            Flush_Arz(g_A_rz, A_rz_shared, i);
        }
    }
#endif

    //////////////////////////////////////////////////////////////////////////

    // Save the thread state to the global memory.
    SaveThreadState(&d_state, &tstates, &photon, rnd_x, is_active);
}
#endif // _GPUMCML_KERNEL_H_
