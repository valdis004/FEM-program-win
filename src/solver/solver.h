#pragma once

// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementprovider.h"
#include <qglobal.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <QVector>
#include <memory>
#include <utility>

#include "element_provider.h"
#include "qtgl/qtgl.h"
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
  unsigned global_matrix_size = 0;

  inline void setParams(size_t i,
                        const FemAbstractElement* element,
                        unsigned& correction,
                        unsigned& cur_dof,
                        unsigned& local_id,
                        unsigned& node_id,
                        unsigned& full_dof,
                        const ElementData& data);

  inline unsigned getGlobalIndexAndSetLoad(
      size_t i,
      const FemAbstractElement* element,
      SparseVector<double>& global_load_vector,
      const ElementData& data);

  inline unsigned getGlobalIndex(size_t i,
                                 const FemAbstractElement* element,
                                 const ElementData& data);

  std::pair<SparseMatrix<double>, SparseVector<double>>
  getGlobalStiffMatrixAndLoadVector(MeshData* mesh, const ElementData& data);

  SparseVector<double> getglobal_load_vector(shared_ptr<MeshData> mesh);

  void applyBaundaryConditions(SparseMatrix<double>& global_matrix,
                               SparseVector<double>& global_vector,
                               MeshData* mesh);

  void setOutputValuesToNodes(MeshData* mesh,
                              const SparseVector<double>& global_u,
                              shared_ptr<AbstractElement> elements);

 public:
  QVector<double> maxAbsValues;
  QVector<double> maxValues;
  QVector<double> minValues;
  QVector<shared_ptr<AbstractElement>>* elements;

  void calculate(QVector<shared_ptr<AbstractElement>>& elements);

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