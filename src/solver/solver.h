#pragma once

// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h"
#include "qtgl/qtgl.h"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <QVector>
#include <memory>
#include <qglobal.h>
#include <utility>

#include "src/mesh/mesh.h"

using Eigen::MatrixXd;
using Eigen::SparseMatrix;
using Eigen::SparseVector;
using Eigen::VectorXd;
using std::shared_ptr;

class FemAbstractElement;

class Solver : public QObject {
  Q_OBJECT
private:
  unsigned globalMatrixSize = 0;

  inline void setParams(size_t i, const FemAbstractElement *element,
                        unsigned &correction, unsigned &curDof,
                        unsigned &localId, unsigned &nodeId, unsigned &fullDof);

  inline unsigned
  getGlobalIndexAndSetLoad(size_t i, const FemAbstractElement *element,
                           SparseVector<double> &globalLoadVector);

  inline unsigned getGlobalIndex(size_t i, const FemAbstractElement *element);

  std::pair<SparseMatrix<double>, SparseVector<double>>
  getGlobalStiffMatrixAndLoadVector(shared_ptr<MeshData> mesh);

  SparseVector<double> getGlobalLoadVector(shared_ptr<MeshData> mesh);

  void applyBaundaryConditions(SparseMatrix<double> &globalMatrix,
                               SparseVector<double> &globalVector,
                               shared_ptr<MeshData> mesh);

  void setOutputValuesToNodes(shared_ptr<MeshData> mesh,
                              const SparseVector<double> &globalU);

public:
  QVector<double> maxAbsValues;
  QVector<double> maxValues;
  QVector<double> minValues;
  ElementData *data;

  void calculate(QVector<shared_ptr<AbstractElement>> &elements);

signals:
  void newElementStiffMatrixStep(unsigned count);
  void applyBaundaryConditionsStep();
  void solveSystemStep();
  void getOutputStep();
  void calcFinishedStep();

  // public slots:
  //   void updateProgress(QMessageBox *mes, int count);
  //   void showResult(QMessageBox *mes);
};