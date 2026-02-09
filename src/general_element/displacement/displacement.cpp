
#include "displacement.h"
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/displacement/femdisplacement.h" #include <stdexcept>

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

DisplacementUzPsixPsiy::DisplacementUzPsixPsiy(bool u_z, bool psi_x, bool psi_y)
    : u_z(u_z), psi_x(psi_x), psi_y(psi_y) {}

/*virtual*/ void DisplacementUzPsixPsiy::setValues(bool *values) {
  values[0] = u_z;
  values[1] = psi_x;
  values[2] = psi_y;
}