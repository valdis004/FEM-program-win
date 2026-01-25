#pragma once

#include "elements/femelement.h"
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h" #include
// "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"
#include "elements/node.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/point.h"
#include <QVector>
// #include <stdexcept>

// #include "../material/PlateMaterial.h"

class MITC4PlateMy : public FemAbstractElement {
private:
  MatrixXd jMatrix(double xi, double eta);

  MatrixXd cMatrix(int type);

  MatrixXd integrateingFn(double xi, double eta, int type);

  MatrixXd bMatrix(double xi, double eta, int type);

public:
  MITC4PlateMy(size_t id, Node **nodes, int count = 4);

  MITC4PlateMy(size_t id, Node **nodes, const Material &material,
               int count = 4);

  virtual MatrixXd getLocalStiffMatrix() override;

  virtual VectorXd getLoadVector() override;

  virtual QVector<double> getResultVector(VectorXd U, double xi,
                                          double eta) override;
};