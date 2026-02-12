#pragma once

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"

// #include "femload.h"
#include <QVector>
#include <span>
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

class ANodeLoad;

class AStructuralLoad {
 protected:
  QVector<double> values;

 public:
  virtual void setValues(double* values) = 0;

  AStructuralLoad(short count);

  AStructuralLoad(double* values, short count);

  AStructuralLoad(std::span<double> values);
};

class AreaLoadFzMxMy : public AStructuralLoad {
 private:
  double qz, mx, my;

 public:
  AreaLoadFzMxMy();

  AreaLoadFzMxMy(double* values, short count);

  AreaLoadFzMxMy(std::span<double> values);

  virtual void setValues(double* values) override;
};

class AreaLoadFxFyFzMxMyMz : public AStructuralLoad {
 private:
  double qx, qy, qz, mx, my, mz;

 public:
  AreaLoadFxFyFzMxMyMz();

  AreaLoadFxFyFzMxMyMz(double* values, short count);

  AreaLoadFxFyFzMxMyMz(std::span<double> values);
};