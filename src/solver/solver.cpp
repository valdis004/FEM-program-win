#include "solver.h"

#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/SparseCore/SparseVector.h>
#include <qdebug.h>
#include <qglobal.h>

#include <Eigen/SparseLU>
#include <QVector>
#include <cstddef>
#include <memory>
#include <stdexcept>

#include "fem_elements/fem_element.h"
#include "meshdata.h"

void Solver::setParams(size_t i,
                       const FemAbstractElement* element,
                       unsigned& correction,
                       unsigned& cur_dof,
                       unsigned& local_id,
                       unsigned& node_id,
                       unsigned& full_dof,
                       const ElementData& data) {
  correction = 0;
  local_id = data.LOCAL_ID_FROM_STIFFMAT[i];
  node_id = element->nodes_[local_id]->id;
  cur_dof = data.FULL_DOF_COUNT;
  full_dof = cur_dof;

  if (!data.IS_FULL_DOF) {
    auto ids_to_cor = element->genetal_element_->meshData_->ids_to_cor_;
    correction =
        FemAbstractElement::getCorrection(local_id, node_id, data, ids_to_cor);
    cur_dof = data.DOF_MAP[local_id];
  }
}

unsigned Solver::getGlobalIndex(size_t i,
                                const FemAbstractElement* element,
                                const ElementData& data) {
  unsigned correction;
  unsigned local_id;
  unsigned node_id;
  unsigned cur_dof;
  unsigned full_dof;
  setParams(i, element, correction, cur_dof, local_id, node_id, full_dof, data);

  return node_id * full_dof + i % cur_dof - correction;
}

unsigned Solver::getGlobalIndexAndSetLoad(
    size_t i,
    const FemAbstractElement* element,
    SparseVector<double>& globalLoadVector,
    const ElementData& data) {
  unsigned correction;
  unsigned local_id;
  unsigned node_id;
  unsigned cur_dof;
  unsigned full_dof;
  setParams(i, element, correction, cur_dof, local_id, node_id, full_dof, data);

  unsigned dofIndex = i % cur_dof;
  double value = element->nodes_[local_id]->nodeLoad->values[dofIndex];
  double globalId = node_id * full_dof + dofIndex - correction;
  globalLoadVector.coeffRef(globalId) += value;

  return globalId;
}

std::pair<SparseMatrix<double>, SparseVector<double>>
Solver::getGlobalStiffMatrixAndLoadVector(MeshData* mesh,
                                          const ElementData& data) {
  global_matrix_size = mesh->globaStiffMatrixSize;
  SparseMatrix<double> global_stiff_matrix(global_matrix_size,
                                           global_matrix_size);
  SparseVector<double> global_load_vector(global_matrix_size);
  unsigned count = 0;

  auto elements = mesh->femElements;
  for (FemAbstractElement* element : elements) {
    emit newElementStiffMatrixStep(count++);

    const QVector<Node*>& nodes_ = element->nodes_;
    MatrixXd localStiffMatrix = element->getLocalStiffMatrix();

    for (size_t i = 0; i < data.STIFF_MATRIX_SIZE; i++) {
      unsigned col_glob_id =
          getGlobalIndexAndSetLoad(i, element, global_load_vector, data);
      for (size_t j = 0; j < data.STIFF_MATRIX_SIZE; j++) {
        unsigned row_glob_id = getGlobalIndex(j, element, data);
        double loc = localStiffMatrix(i, j);
        global_stiff_matrix.coeffRef(row_glob_id, col_glob_id) +=
            localStiffMatrix(i, j);
      }
    }
  }
  return {global_stiff_matrix, global_load_vector};
}

void Solver::applyBaundaryConditions(SparseMatrix<double>& globalMatrix,
                                     SparseVector<double>& globalVector,
                                     MeshData* mesh) {
  auto nodes_ = mesh->nodes_;

  for (const auto& node : nodes_) {
    if (!node->nodeDisplacement) continue;

    short countToZero = node->nodeDisplacement->nodesCountToZero;
    for (size_t i = 0; i < countToZero; i++) {
      unsigned id = node->nodeDisplacement->nodeIdsToZero[i];
      globalVector.coeffRef(id) = 0;
      for (size_t j = 0; j < global_matrix_size; j++) {
        if (j == id)
          globalMatrix.coeffRef(id, j) = 1;
        else {
          globalMatrix.coeffRef(id, j) = 0;
          globalMatrix.coeffRef(j, id) = 0;
        }
      }
    }
  }
}

void Solver::calculate(QVector<shared_ptr<AbstractElement>>& elements) {
  this->elements = &elements;

  for (auto& element : elements) {
    MeshData* mesh = element->meshData_.get();
    auto& data = ElementProvider.at(element->getType());

    auto stiffAndLoad = getGlobalStiffMatrixAndLoadVector(mesh, data);
    auto stiff = stiffAndLoad.first;
    auto load = stiffAndLoad.second;

    // for (size_t i = 0; i < stiff.rows(); i++) {
    //   for (size_t j = 0; j < stiff.cols(); j++) {
    //     qDebug() << stiff.coeff(i, j);
    //   }
    //   qDebug() << "\n\n\n";
    // }

    emit applyBaundaryConditionsStep();
    applyBaundaryConditions(stiff, load, mesh);

    // qDebug() << " LOAD VECTOR  ";

    // for (size_t i = 0; i < load.size(); i++) {
    //   qDebug() << i << " " << load.coeff(i) << " ";
    // }

    emit solveSystemStep();
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    solver.compute(stiff);
    if (solver.info() != Eigen::Success) {
      throw std::runtime_error("Solver info not success");
    }

    SparseVector<double> u = solver.solve(load);

    // for (size_t i = 0; i < u.size(); i++) {
    //   qDebug() << i << " " << u.coeff(i) << "\n";
    // }

    void getOutputStep();
    setOutputValuesToNodes(mesh, u, element);
  }
  emit calcFinishedStep();
}

void Solver::setOutputValuesToNodes(MeshData* mesh,
                                    const SparseVector<double>& globalU,
                                    shared_ptr<AbstractElement> element) {
  bool flag = true;
  const auto& data = ElementProvider.at(element->getType());

  double maxAbsValues[16]{0};
  double maxValues[16]{0};
  double minValues[16]{0};
  short count = data.OUTPUT_VALUES_COUNT;

  for (auto& element : mesh->femElements) {
    VectorXd elementU{data.STIFF_MATRIX_SIZE};
    for (size_t i = 0; i < data.STIFF_MATRIX_SIZE; i++) {
      unsigned globalId = getGlobalIndex(i, element, data);
      elementU(i) = globalU.coeff(globalId);
    }

    for (size_t i = 0; i < data.MAIN_NODES_COUNT; i++) {
      Node* curNode = element->nodes_[i];

      QVector<double> outputValues = element->getResultVector(
          elementU, data.MAIN_NODES_XI_SET[i], data.MAIN_NODES_ETA_SET[i]);
      if (flag) {
        for (size_t i = 0; i < outputValues.size(); i++) {
          maxAbsValues[i] = abs(outputValues[i]);
          minValues[i] = outputValues[i];
          maxValues[i] = outputValues[i];
        }
        flag = false;
      }
      curNode->outputValues = outputValues;

      for (size_t j = 0; j < outputValues.size(); j++) {
        if (abs(outputValues[j]) > maxAbsValues[j]) {
          maxAbsValues[j] = abs(outputValues[j]);
        }

        if (outputValues[j] > maxValues[j]) {
          maxValues[j] = outputValues[j];
        }

        if (outputValues[j] < minValues[j]) {
          minValues[j] = outputValues[j];
        }
      }
    }
  }

  for (size_t j = 0; j < count; j++) {
    element->max_abs_values_.push_back(maxAbsValues[j]);
    element->min_values_.push_back(maxValues[j]);
    element->max_values_.push_back(minValues[j]);
  }
}

// void Solver::updateProgress(QMessageBox *mes, int count) {
//   mes->setText(
//       QString("Creating local stiff matrix for element %1").arg(count));
// }

// void Solver::showResult(QMessageBox *mes) {
//   mes->setText(QString("Global stiff matrix sucsesfully created"));
// }