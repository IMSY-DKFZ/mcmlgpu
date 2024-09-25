/*******************************************************
 * Author: Intelligent Medical Systems
 * License: see LICENSE.md file
 *******************************************************/

#include <cstring>
//#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
#include <nanobind/nanobind.h>
#include <vector>

#include "gpumcml.h"
#include "gpumcml_main.h"

// namespace py = pybind11;
namespace py = nanobind;

// namespace py = pybind11;

int RunSimulationsPython(const py::list &simulations, int num_GPUs, const py::str mcoFileName, uint64_t seed,
                         int n_simulations)
{
    py::gil_scoped_release release;
    std::vector<SimulationStruct> simulations_vector;
    for (const auto &simulation : simulations)
    {
        simulations_vector.push_back(py::cast<SimulationStruct>(simulation));
    }
    auto simulations_ptr = const_cast<SimulationStruct *>(simulations_vector.data());
    auto res = RunSimulations(simulations_ptr, num_GPUs, mcoFileName.c_str(), seed, n_simulations);
    py::gil_scoped_acquire acquire;
    return res;
}

// Getters and setters for char arrays
std::string get_outp_filename(const SimulationStruct &s)
{
    return {s.outp_filename};
}

void set_outp_filename(SimulationStruct &s, const py::str filename)
{
    strncpy(s.outp_filename, filename.c_str(), STR_LEN);
    s.outp_filename[STR_LEN - 1] = '\0'; // Ensure null termination
}

std::string get_inp_filename(const SimulationStruct &s)
{
    return {s.inp_filename};
}

void set_inp_filename(SimulationStruct &s, const py::str filename)
{
    strncpy(s.inp_filename, filename.c_str(), STR_LEN);
    s.inp_filename[STR_LEN - 1] = '\0'; // Ensure null termination
}

void set_layers(SimulationStruct &s, const py::list &layers)
{
    delete[] s.layers; // Clean up existing layers
    s.layers = new LayerStruct[layers.size()];
    for (size_t i = 0; i < layers.size(); ++i)
    {
        s.layers[i] = py::cast<LayerStruct>(layers[i]);
    }
    s.n_layers = layers.size();
}

py::list get_layers(const SimulationStruct &s)
{
    py::list layers;
    for (size_t i = 0; i < s.n_layers; ++i)
    {
        layers.append(s.layers[i]);
    }
    return layers;
}

// PYBIND11_MODULE(mcmlgpu, m)
NB_MODULE(mcmlgpu, m)
{
    m.def("run_simulations", &RunSimulationsPython, "Function to run simulations", py::arg("simulations"),
          py::arg("num_GPUs"), py::arg("mcoFileName"), py::arg("seed"), py::arg("n_simulations"));
    m.attr("__version__") = "0.0.4";
    py::class_<LayerStruct>(m, "LayerStruct")
        .def(py::init<>())
        .def_rw("n", &LayerStruct::n)
        .def_rw("g", &LayerStruct::g)
        .def_rw("mua", &LayerStruct::mua)
        .def_rw("mutr", &LayerStruct::mutr)
        .def_rw("z_max", &LayerStruct::z_max)
        .def_rw("z_min", &LayerStruct::z_min);

    py::class_<DetStruct>(m, "DetStruct")
        .def(py::init<>())
        .def_rw("dr", &DetStruct::dr)
        .def_rw("dz", &DetStruct::dz)
        .def_rw("na", &DetStruct::na)
        .def_rw("nr", &DetStruct::nr)
        .def_rw("nz", &DetStruct::nz);

    // Define the SimulationStruct binding
    py::class_<SimulationStruct>(m, "SimulationStruct")
        .def(py::init<>())
        .def_prop_rw("outp_filename", &get_outp_filename, &set_outp_filename)
        .def_prop_rw("inp_filename", &get_inp_filename, &set_inp_filename)
        .def_rw("AorB", &SimulationStruct::AorB)
        .def_rw("number_of_photons", &SimulationStruct::number_of_photons)
        .def_rw("ignoreAdetection", &SimulationStruct::ignoreAdetection)
        .def_rw("start_weight", &SimulationStruct::start_weight)
        .def_rw("det", &SimulationStruct::det)
        .def_rw("n_layers", &SimulationStruct::n_layers)
        .def_prop_rw("layers", &get_layers, &set_layers);
}
