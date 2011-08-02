// -------------------------------------------------------------
// CUDPP -- CUDA Data Parallel Primitives library
// -------------------------------------------------------------
// $Revision: 4400 $
// $Date: 2008-08-04 10:58:14 -0700 (Mon, 04 Aug 2008) $
// ------------------------------------------------------------- 
// This source code is distributed under the terms of license.txt 
// in the root directory of this source distribution.
// ------------------------------------------------------------- 

/**
 * @file
 * tridiagonal_app.cu
 *
 * @brief CUDPP application-level tridiagonal solver routines
 */

/** \addtogroup cudpp_app
  * @{
  */
/** @name Tridiagonal functions
 * @{
 */

#include "cudpp.h"
#include "cudpp_util.h"
#include "cudpp_plan.h"
#include "cuda_util.h"

#include <cstdlib>
#include <cstdio>
#include <assert.h>

#include "kernel/tridiagonal_kernel.cuh"

/**
 * @brief Hybrid CR-PCR solver (CRPCR)
 *
 * This is a wrapper function for the GPU CR-PCR kernel.
 *
 * @param[out] d_x Solution vector
 * @param[in] d_a Lower diagonal
 * @param[in] d_b Main diagonal
 * @param[in] d_c Upper diagonal
 * @param[in] d_d Right hand side
 * @param[in] systemSize The size of the linear system
 * @param[in] numSystems The number of systems to be solved
 */
template <class T>
void crpcr(T *d_a, 
           T *d_b, 
           T *d_c, 
           T *d_d, 
           T *d_x, 
           unsigned int systemSizeOriginal, 
           unsigned int numSystems)
{
    const unsigned int systemSize = ceilPow2(systemSizeOriginal);
    const unsigned int num_threads_block = systemSize/2;
    const unsigned int restSystemSize = systemSize/2;
    const unsigned int iterations = logBase2Pow2(restSystemSize/2);
  
    // setup execution parameters
    dim3  grid(numSystems, 1, 1);
    dim3  threads(num_threads_block, 1, 1);
    const unsigned int smemSize = 
        (systemSize + 1 + restSystemSize) * 5 * sizeof(T);

    crpcrKernel<<< grid, threads, smemSize>>>(d_a, 
                                              d_b, 
                                              d_c, 
                                              d_d, 
                                              d_x, 
                                              systemSizeOriginal,
                                              iterations);

    CUDA_CHECK_ERROR("crpcr");
}


/**
 * @brief Dispatches the tridiagonal function based on the plan
 *
 * This is the dispatch call for the tridiagonal solver in either float 
 * or double datatype. 
 *
 * @param[out] d_x Solution vector
 * @param[in] d_a Lower diagonal
 * @param[in] d_b Main diagonal
 * @param[in] d_c Upper diagonal
 * @param[in] d_d Right hand side
 * @param[in] systemSize The size of the linear system
 * @param[in] numSystems The number of systems to be solved
 * @param[in] plan pointer to CUDPPTridiagonalPlan
 * @returns CUDPPResult indicating success or error condition
 */
CUDPPResult cudppTridiagonalDispatch(void *d_a, 
                                     void *d_b, 
                                     void *d_c, 
                                     void *d_d, 
                                     void *d_x, 
                                     int systemSize, 
                                     int numSystems, 
                                     const CUDPPTridiagonalPlan * plan)
{
  
    //figure out which algorithm to run
    if (plan->m_config.datatype == CUDPP_FLOAT)
    {
        crpcr<float>((float *)d_a, 
                     (float *)d_b, 
                     (float *)d_c, 
                     (float *)d_d, 
                     (float *)d_x, 
                     systemSize, 
                     numSystems);
        return CUDPP_SUCCESS;
    }
    else if (plan->m_config.datatype == CUDPP_DOUBLE)
    {
        crpcr<double>((double *)d_a, 
                      (double *)d_b, 
                      (double *)d_c, 
                      (double *)d_d, 
                      (double *)d_x, 
                      systemSize, 
                      numSystems);
        return CUDPP_SUCCESS;
    }
    else
        return CUDPP_ERROR_ILLEGAL_CONFIGURATION;
    
}

/** @} */ // end Tridiagonal functions
/** @} */ // end cudpp_app
