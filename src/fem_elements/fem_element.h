#pragma once

#include <qcontainerfwd.h>

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
#include <qglobal.h>
#include <qmap.h>
#include <qnamespace.h>

#include <cstddef>
#include <memory>

// #include "element.h"
// #include "elementprovider.h"
#include "element_provider.h"
#include "fem_elements/global.h"
#include "fem_types.h"
#include "material/material.h"
#include "node.h"
#include "structural_element/structural_displacement/displacement.h"
// #include "structural_element/structural_load/load.h"

class AStructuralElement;

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::make_unique;
using std::unique_ptr;

static unsigned counterOfElements = 0;

class AFemElement {
 public:
  QVector<Node*> nodes_;
  std::shared_ptr<AStructuralElement> genetal_element_{nullptr};
  size_t id_;
  const ElementType type_;
  unsigned nodes_count_;

 public:
  AFemElement(unsigned id,
              Node** nodes,
              int count,
              ElementType type,
              std::shared_ptr<AStructuralElement> generalElement);

  AFemElement(unsigned id,
              Node** nodes,
              int count,
              const Material& material,
              ElementType type,
              std::shared_ptr<AStructuralElement> generalElement);

  virtual MatrixXd getLocalStiffMatrix() = 0;

  virtual VectorXd getLoadVector() = 0;

  virtual QVector<double> getResultVector(VectorXd U,
                                          double xi,
                                          double eta) = 0;

  virtual ~AFemElement() = default;

  static AFemElement* create(
      unsigned id,
      ElementType type,
      Node** nodes,
      int count,
      std::shared_ptr<AStructuralElement> generalElement);

  static void setCalcProps(AFemElement* ptr,
                           unsigned& globalMatrixSize,
                           const ElementData& data);

  static unsigned getCorrection(unsigned cur_node_loc_id,
                                unsigned cur_node_glob_id,
                                const ElementData& data,
                                const QVector<unsigned>& ids_to_cor);
};