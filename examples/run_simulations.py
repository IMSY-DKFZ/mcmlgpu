# SPDX-FileCopyrightText: Intelligent Medical Systems Group, DKFZ
# see LICENSE for more details

from mcmlgpu import mcmlgpu

layer = mcmlgpu.LayerStruct()
layer.n = 1.524
layer.g = 0.879
layer.mua = 78.7405
layer.mutr = 0.00107596
layer.z_min = 0.0
layer.z_max = 0.066

layer_top = mcmlgpu.LayerStruct()
layer_top.n = 1.0
layer_top.z_max = 4.5573e-41
layer_top.z_min = -7.6409e+33
layer_top.g = 0
layer_top.mua = 4.5573e-41
layer_top.mutr = -7.6409e+33

layer_below = mcmlgpu.LayerStruct()
layer_below.n = 1.0

det = mcmlgpu.DetStruct()
det.dz = 0.002
det.dr = 2
det.nz = 500
det.nr = 1
det.na = 1


sim_struct = mcmlgpu.SimulationStruct()
sim_struct.number_of_photons = 10000
sim_struct.AorB = "A"
sim_struct.inp_filename = "test_filename.mci"
sim_struct.outp_filename = "MCML_Bat_NA__Sim_0_3.00e-07.mco"
sim_struct.ignoreAdetection = False
sim_struct.start_weight = 0.97596
sim_struct.n_layers = 1
sim_struct.layers = [layer_top, layer, layer_below]
sim_struct.det = det

mcmlgpu.run_simulations(
    simulations=[sim_struct],
    num_GPUs=1,
    mcoFileName = "batch.mco",
    seed= 13,
    n_simulations = 1
)
