#include "load.h"
#include <cassert>
#include <cstddef>
#include <qdebug.h>
// #include <qglobal.h>
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h" #include "femload.h"

AbstractLoad::AbstractLoad(short count) {
  assert(count > 0);
  this->values.resize(count);
};

AbstractLoad::AbstractLoad(double *values, short count) {
  assert(count > 0);
  this->values.resize(count);

  for (size_t i = 0; i < count; i++)
    this->values[i] = values[i];
};

AreaLoadFzMxMy::AreaLoadFzMxMy() : AbstractLoad(3) {};

AreaLoadFzMxMy::AreaLoadFzMxMy(double *values, short count)
    : AbstractLoad(values, count) {};

void AreaLoadFzMxMy::setValues(double *values) {
  for (size_t i = 0; i < 3; i++) {
    values[i] = this->values[i];
  }
}

AreaLoadFxFyFzMxMyMz::AreaLoadFxFyFzMxMyMz() : AbstractLoad(6) {};

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