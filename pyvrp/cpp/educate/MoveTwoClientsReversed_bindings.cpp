#include "MoveTwoClientsReversed.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(MoveTwoClientsReversed, m)
{
    py::class_<LocalSearchOperator<Node>>(
        m, "NodeOperator", py::module_local());

    py::class_<MoveTwoClientsReversed, LocalSearchOperator<Node>>(
        m, "MoveTwoClientsReversed")
        .def(py::init<ProblemData const &, PenaltyManager const &>(),
             py::arg("data"),
             py::arg("penalty_manager"));
}