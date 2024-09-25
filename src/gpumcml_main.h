/*******************************************************
 * Author: Intelligent Medical Systems
 * License: see LICENSE.md file
 *******************************************************/

#ifndef MCML_GPUMCML_MAIN_H
#define MCML_GPUMCML_MAIN_H

static void RunGPUi(HostThreadState *hstate);

void DoOneSimulation(int sim_id, SimulationStruct *simulation, HostThreadState *hstates[], UINT32 num_GPUs, UINT64 *x,
                     UINT32 *a, const char *mcoFile, SimulationResults *simResults);

int RunSimulations(SimulationStruct *simulations, UINT32 num_GPUs, const char *mcoFileName, UINT64 seed,
                   int n_simulations);

#endif // MCML_GPUMCML_MAIN_H
