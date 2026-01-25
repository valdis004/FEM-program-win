#pragma once

// #include "displacement/displacement.h"
// #include "displacement/displacement.h"
// #include "elementprovider.h
// #include "node.h"
// #include "plates/plates.h"
// #include "plates/plates.h"
#include <Eigen/Dense>
#include <QVector>
// #include <exception>
// #include "elementprovider.h"
#include <cstddef>
#include <qglobal.h>
#include <qmap.h>
#include <qnamespace.h>

#include "elementprovider.h"
#include "generalElement/displacement/displacement.h"
#include "material/material.h"
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h"
#include "generalElement/load/load.h"
#include "node.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/point.h"
#include "femtypes.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

class FemAbstractElement {
private:
  // const ElementData &check(ElementType type);

public:
  const ElementType type;
  ElementData &data;
  QVector<Node *> nodes;
  unsigned nodesCount;
  size_t id;
  AbstractLoad *generalLoad;
  AbstractDisplacement *generalDisp;

public:
  FemAbstractElement(size_t id, Node **nodes, int count,
                     ElementType type = ElementType::NONE);

  FemAbstractElement(size_t id, Node **nodes, int count,
                     const Material &material,
                     ElementType type = ElementType::NONE);

  virtual MatrixXd getLocalStiffMatrix() = 0;

  virtual VectorXd getLoadVector() = 0;

  virtual QVector<double> getResultVector(VectorXd U, double xi,
                                          double eta) = 0;

  virtual ~FemAbstractElement() = default;

  static FemAbstractElement *create(size_t id, ElementType type, Node **nodes,
                                    int count);

  static void setCalcProps(FemAbstractElement *ptr, unsigned &globalMatrixSize);

  void setLoad(AbstractLoad *load) { this->generalLoad = load; }

  void setDisp(AbstractDisplacement *disp) { this->generalDisp = disp; }
};