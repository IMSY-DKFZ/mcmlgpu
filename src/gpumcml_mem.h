/*******************************************************
 * Author: Intelligent Medical Systems
 * License: see LICENSE.md file
 *******************************************************/

#ifndef MCML_GPUMCML_MEM_H
#define MCML_GPUMCML_MEM_H

#include "gpumcml_kernel.h"

int InitDCMem(SimulationStruct *sim, UINT32 A_rz_overflow);

int InitSimStates(SimState *HostMem, SimState *DeviceMem, GPUThreadStates *tstates, SimulationStruct *sim,
                  int n_threads);

int CopyDeviceToHostMem(SimState *HostMem, SimState *DeviceMem, SimulationStruct *sim, int n_threads);

void FreeHostSimState(SimState *hstate);

void FreeDeviceSimStates(SimState *dstate, GPUThreadStates *tstates);

#endif // MCML_GPUMCML_MEM_H
