#pragma once

// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/node.h"
// #include "femdisplacement.h"
#include <QVector>
#include <qglobal.h>
class Node;
class NodeDisplacement;

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

class AbstractDisplacement {
protected:
  QVector<bool> values;

public:
  virtual void setValues(bool *values) = 0;

  // virtual NodeDisplacement *createChildNodeDisplacement() = 0;
};

class DisplacementUzPsixPsiy : public AbstractDisplacement {
private:
  bool u_z{false}, psi_x{false}, psi_y{false};

public:
  DisplacementUzPsixPsiy(bool u_z, bool psi_x, bool psi_y);

  virtual void setValues(bool *values) override;

  // virtual NodeDisplacement *createChildNodeDisplacement() override;
};