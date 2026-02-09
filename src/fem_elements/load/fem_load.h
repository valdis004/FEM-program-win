#pragma once

#include <span>

#include "fem_elements/fem_types.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/load/load.h"

// #include <stdexcept>
class Node;
class AbstractLoad;

class NodeLoad {
 public:
  double* values{nullptr};
  int countValues{0};
  //   double fx, fy, fz, mx, my, mz;

  NodeLoad() = default;

  NodeLoad(std::span<double> values);

  virtual void setNodeLoadValues(double* values, double* coefs) = 0;

  virtual void appendValuesToNodeLoad(AbstractLoad* generalLoad,
                                      double* coefs) = 0;

  static NodeLoad* createNodeLoadFromLoad(ElementType type,
                                          AbstractLoad* load,
                                          double* coefs,
                                          int localNodeId);

  virtual ~NodeLoad() = default;
};

class NodeLoadFzMxMy : public NodeLoad {
 public:
  double fz, mx, my;

  virtual void setNodeLoadValues(double* values, double* coefs) override;

  virtual void appendValuesToNodeLoad(AbstractLoad* generalLoad,
                                      double* coefs) override;

  static NodeLoad* create(double* values, double* coefs);
};

class NodeLoadFz : public NodeLoad {
 public:
  double fz;

  NodeLoadFz() = default;

  NodeLoadFz(std::span<double> values);

  virtual void setNodeLoadValues(double* values, double* coefs) override;

  virtual void appendValuesToNodeLoad(AbstractLoad* generalLoad,
                                      double* coefs) override;

  static NodeLoad* create(double* values, double* coefs);
};

class NodeLoadMxMy : public NodeLoad {
 public:
  double mx, my;

  virtual void setNodeLoadValues(double* values, double* coefs) override;

  virtual void appendValuesToNodeLoad(AbstractLoad* generalLoad,
                                      double* coefs) override;

  static NodeLoad* create(double* values, double* coefs);
};