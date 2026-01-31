#pragma once

#include "elements/femelement.h"
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h" #include
// "/home/vladislav/Документы/FEM/FEM program/src/elements/femtypes.h"
#include "elements/node.h"
#include "material.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/point.h"
#include <QVector>
#include <memory>
// #include <stdexcept>

// #include "../material/PlateMaterial.h"

class MITC4PlateMy : public FemAbstractElement {
private:
  MatrixXd jMatrix(double xi, double eta);

  MatrixXd integrateingFn(double xi, double eta, int type);

  MatrixXd bMatrix(double xi, double eta, int type);

public:
  MITC4PlateMy(unsigned id, Node **nodes,
               std::shared_ptr<AbstractElement> generalElement);

  MITC4PlateMy(unsigned id, Node **nodes, const Material &material,
               std::shared_ptr<AbstractElement> generalElement);

  virtual MatrixXd getLocalStiffMatrix() override;

  virtual VectorXd getLoadVector() override;

  virtual QVector<double> getResultVector(VectorXd U, double xi,
                                          double eta) override;

  static MatrixXd cMatrix(std::shared_ptr<Material> material, int type);
};