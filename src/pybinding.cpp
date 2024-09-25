/*******************************************************
 * Author: Intelligent Medical Systems
 * License: see LICENSE.md file
 *******************************************************/

#include <cstring>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

#include "gpumcml.h"
#include "gpumcml_main.h"

namespace py = pybind11;

int RunSimulationsPython(const std::vector<SimulationStruct> &simulations, int num_GPUs, const std::string &mcoFileName,
                         uint64_t seed, int n_simulations)
{
    py::gil_scoped_release release;
    auto simulations_ptr = const_cast<SimulationStruct *>(simulations.data());
    auto res = RunSimulations(simulations_ptr, num_GPUs, mcoFileName.c_str(), seed, n_simulations);
    py::gil_scoped_acquire acquire;
    return res;
}

// Getters and setters for char arrays
std::string get_outp_filename(const SimulationStruct &s)
{
    return {s.outp_filename};
}

void set_outp_filename(SimulationStruct &s, const std::string &filename)
{
    strncpy(s.outp_filename, filename.c_str(), STR_LEN);
    s.outp_filename[STR_LEN - 1] = '\0'; // Ensure null termination
}

std::string get_inp_filename(const SimulationStruct &s)
{
    return {s.inp_filename};
}

void set_inp_filename(SimulationStruct &s, const std::string &filename)
{
    strncpy(s.inp_filename, filename.c_str(), STR_LEN);
    s.inp_filename[STR_LEN - 1] = '\0'; // Ensure null termination
}

void set_layers(SimulationStruct &s, const std::vector<LayerStruct> &layers)
{
    delete[] s.layers; // Clean up existing layers
    s.layers = new LayerStruct[layers.size()];
    std::memcpy(s.layers, layers.data(), layers.size() * sizeof(LayerStruct));
    s.n_layers = layers.size();
}

std::vector<LayerStruct> get_layers(const SimulationStruct &s)
{
    return std::vector<LayerStruct>(s.layers, s.layers + s.n_layers);
}

PYBIND11_MODULE(mcmlgpu, m)
{
    m.def("run_simulations", &RunSimulationsPython, "Function to run simulations", py::arg("simulations"),
          py::arg("num_GPUs"), py::arg("mcoFileName"), py::arg("seed"), py::arg("n_simulations"));
    m.attr("__version__") = "0.0.4";
    py::class_<LayerStruct>(m, "LayerStruct")
        .def(py::init<>())
        .def_readwrite("n", &LayerStruct::n)
        .def_readwrite("g", &LayerStruct::g)
        .def_readwrite("mua", &LayerStruct::mua)
        .def_readwrite("mutr", &LayerStruct::mutr)
        .def_readwrite("z_max", &LayerStruct::z_max)
        .def_readwrite("z_min", &LayerStruct::z_min);

    py::class_<DetStruct>(m, "DetStruct")
        .def(py::init<>())
        .def_readwrite("dr", &DetStruct::dr)
        .def_readwrite("dz", &DetStruct::dz)
        .def_readwrite("na", &DetStruct::na)
        .def_readwrite("nr", &DetStruct::nr)
        .def_readwrite("nz", &DetStruct::nz);

    // Define the SimulationStruct binding
    py::class_<SimulationStruct>(m, "SimulationStruct")
        .def(py::init<>())
        .def_property("outp_filename", &get_outp_filename, &set_outp_filename)
        .def_property("inp_filename", &get_inp_filename, &set_inp_filename)
        .def_readwrite("AorB", &SimulationStruct::AorB)
        .def_readwrite("number_of_photons", &SimulationStruct::number_of_photons)
        .def_readwrite("ignoreAdetection", &SimulationStruct::ignoreAdetection)
        .def_readwrite("start_weight", &SimulationStruct::start_weight)
        .def_readwrite("det", &SimulationStruct::det)
        .def_readwrite("n_layers", &SimulationStruct::n_layers)
        .def_property("layers", &get_layers, &set_layers);
}
