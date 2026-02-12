#pragma once

// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/node.h"
// #include "femdisplacement.h"
#include <qglobal.h>

#include <QVector>
class Node;
class ANodeDisplacement;

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

class AStructuralDisplacement {
 protected:
  QVector<bool> values;

 public:
  virtual void setValues(bool* values) = 0;

  // virtual NodeDisplacement *createChildNodeDisplacement() = 0;
};

class DisplacementUzPsixPsiy : public AStructuralDisplacement {
 private:
  bool u_z{false}, psi_x{false}, psi_y{false};

 public:
  DisplacementUzPsixPsiy(bool u_z, bool psi_x, bool psi_y);

  virtual void setValues(bool* values) override;

  // virtual NodeDisplacement *createChildNodeDisplacement() override;
};