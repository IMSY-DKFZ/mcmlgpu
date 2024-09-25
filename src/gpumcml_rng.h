/*******************************************************
 * Author: Intelligent Medical Systems
 * License: see LICENSE.md file
 *******************************************************/

#ifndef MCML_GPUMCML_RNG_H
#define MCML_GPUMCML_RNG_H

#include <iostream>

std::string getExecutablePath();

__device__ GFLOAT rand_MWC_co(UINT64 *x, UINT32 *a);

__device__ GFLOAT rand_MWC_oc(UINT64 *x, UINT32 *a);

int init_RNG(UINT64 *x, UINT32 *a, const UINT32 n_rng, UINT64 xinit);

#endif // MCML_GPUMCML_RNG_H
