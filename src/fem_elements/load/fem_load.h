#pragma once

#include <qcontainerfwd.h>

#include <QVector>
#include <span>

// #include "fem_element.h"
// #include "fem_element.h"
#include "fem_elements/fem_types.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

// #include <stdexcept>
class Node;
class AStructuralLoad;
class AFemElement;

class ANodeLoad {
 public:
  QVector<double> values_;

  ANodeLoad() = default;

  ANodeLoad(std::span<double> values);

  virtual void setNodeLoadValues(double* values, double* coefs) = 0;

  virtual void appendValuesToNodeLoad(double* values, double* coefs) = 0;

  static ANodeLoad* createNodeLoadFromLoad(ElementType type,
                                           AStructuralLoad* load,
                                           double* coefs,
                                           int localNodeId);

  static void setNodeLoadToNodeFromLoad(Node* node,
                                        AFemElement* fem_element,
                                        double* coefs,
                                        int localNodeId);

  virtual ~ANodeLoad() = default;
};

class NodeLoadFzMxMy : public ANodeLoad {
 public:
  double fz, mx, my;

  NodeLoadFzMxMy() = default;

  NodeLoadFzMxMy(std::span<double> values) : ANodeLoad(values) {}

  virtual void setNodeLoadValues(double* values, double* coefs) override;

  virtual void appendValuesToNodeLoad(double* values, double* coefs) override;

  static ANodeLoad* create(double* values, double* coefs);
};

class NodeLoadFz : public ANodeLoad {
 public:
  double fz;

  NodeLoadFz() = default;

  NodeLoadFz(std::span<double> values) : ANodeLoad(values) {}

  virtual void setNodeLoadValues(double* values, double* coefs) override;

  virtual void appendValuesToNodeLoad(double* values, double* coefs) override;

  static ANodeLoad* create(double* values, double* coefs);
};

class NodeLoadMxMy : public ANodeLoad {
 public:
  double mx, my;

  NodeLoadMxMy() = default;

  NodeLoadMxMy(std::span<double> values) : ANodeLoad(values) {};

  virtual void setNodeLoadValues(double* values, double* coefs) override;

  virtual void appendValuesToNodeLoad(double* values, double* coefs) override;

  static ANodeLoad* create(double* values, double* coefs);
};