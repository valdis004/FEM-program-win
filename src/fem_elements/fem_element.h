#pragma once

#include <qcontainerfwd.h>
#define FEM_ELEMENT

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
#include <memory>
#include <qglobal.h>
#include <qmap.h>
#include <qnamespace.h>

// #include "element.h"
// #include "elementprovider.h"
#include "element_provider.h"
#include "fem_elements/global.h"
#include "fem_types.h"
#include "general_element/displacement/displacement.h"
#include "general_element/load/load.h"
#include "material/material.h"
#include "node.h"

class AbstractElement;

using Eigen::MatrixXd;
using Eigen::VectorXd;

static unsigned counterOfElements = 0;

class FemAbstractElement {
public:
  const ElementType type_;
  QVector<Node *> nodes_;
  unsigned nodes_count_;
  size_t id_;
  AbstractLoad *general_Load_;
  AbstractDisplacement *general_disp_;
  std::shared_ptr<AbstractElement> genetal_element_;

public:
  FemAbstractElement(unsigned id, Node **nodes, int count, ElementType type,
                     std::shared_ptr<AbstractElement> generalElement);

  FemAbstractElement(unsigned id, Node **nodes, int count,
                     const Material &material, ElementType type,
                     std::shared_ptr<AbstractElement> generalElement);

  virtual MatrixXd getLocalStiffMatrix() = 0;

  virtual VectorXd getLoadVector() = 0;

  virtual QVector<double> getResultVector(VectorXd U, double xi,
                                          double eta) = 0;

  virtual ~FemAbstractElement() = default;

  static FemAbstractElement *
  create(unsigned id, ElementType type, Node **nodes, int count,
         std::shared_ptr<AbstractElement> generalElement);

  static void setCalcProps(FemAbstractElement *ptr, unsigned &globalMatrixSize,
                           const ElementData &data);

  void setLoad(AbstractLoad *load) { this->general_Load_ = load; }

  void setDisp(AbstractDisplacement *disp) { this->general_disp_ = disp; }

  static unsigned getCorrection(unsigned cur_node_loc_id,
                                unsigned cur_node_glob_id,
                                const ElementData &data,
                                const QVector<unsigned> &ids_to_cor);
};