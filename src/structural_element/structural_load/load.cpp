#include "load.h"

#include <qdebug.h>

#include <cassert>
#include <cstddef>
// #include <qglobal.h>
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h" #include "femload.h"

AStructuralLoad::AStructuralLoad(short count) {
  assert(count > 0);
  this->values.resize(count);
};

AStructuralLoad::AStructuralLoad(double* values, short count) {
  assert(count > 0);
  this->values.resize(count);

  for (size_t i = 0; i < count; i++) {
    this->values[i] = values[i];
  }
};

AStructuralLoad::AStructuralLoad(std::span<double> values) {
  this->values.resize(values.size());

  for (size_t i = 0; i < values.size(); i++) {
    this->values[i] = values[i];
  }
};

AreaLoadFzMxMy::AreaLoadFzMxMy() : AStructuralLoad(3) {};

AreaLoadFzMxMy::AreaLoadFzMxMy(double* values, short count)
    : AStructuralLoad(values, count) {};

AreaLoadFzMxMy::AreaLoadFzMxMy(std::span<double> values)
    : AStructuralLoad(values) {};

void AreaLoadFzMxMy::setValues(double* values) {
  for (size_t i = 0; i < 3; i++) {
    values[i] = this->values[i];
  }
}

AreaLoadFxFyFzMxMyMz::AreaLoadFxFyFzMxMyMz() : AStructuralLoad(6) {};

AreaLoadFxFyFzMxMyMz::AreaLoadFxFyFzMxMyMz(std::span<double> values)
    : AStructuralLoad(values) {};

// AreaLoadQzMxMy::AreaLoadQzMxMy(double qz, double mx, double my)
//     : qz(qz), mx(mx), my(my) {};

// /*virtual*/ void AreaLoadQzMxMy::setValues(double *values) {
//   values[0] = qz;
//   values[1] = mx;
//   values[2] = my;
// }
// AreaLoad::AreaLoad(double qx, double qy, double qz) : AreaLoadPlate(qz) {
//   this->qx = qx;
//   this->qy = qy;
// }