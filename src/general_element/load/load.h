#pragma once

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"

// #include "femload.h"
#include <QVector>
#include <span>
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

class NodeLoad;

class AbstractLoad {
 protected:
  QVector<double> values;

 public:
  virtual void setValues(double* values) = 0;

  AbstractLoad(short count);

  AbstractLoad(double* values, short count);

  AbstractLoad(std::span<double> values);
};

class AreaLoadFzMxMy : public AbstractLoad {
 private:
  double qz, mx, my;

 public:
  AreaLoadFzMxMy();

  AreaLoadFzMxMy(double* values, short count);

  AreaLoadFzMxMy(std::span<double> values);

  virtual void setValues(double* values) override;
};

class AreaLoadFxFyFzMxMyMz : public AbstractLoad {
 private:
  double qx, qy, qz, mx, my, mz;

 public:
  AreaLoadFxFyFzMxMyMz();

  AreaLoadFxFyFzMxMyMz(double* values, short count);

  AreaLoadFxFyFzMxMyMz(std::span<double> values);
};