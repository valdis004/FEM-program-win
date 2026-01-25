#pragma once

// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/node.h"

#include "elements/femtypes.h"

class Node;
class AbstractDisplacement;

class NodeDisplacement {
public:
  unsigned *nodeIdsToZero{nullptr};
  unsigned nodesCountToZero{0};

  virtual void setIndexesToZero(Node *node) = 0;

  static NodeDisplacement *createNodeDisplacementFromDisplacement(
      ElementType type, AbstractDisplacement *displ, Node *node, int locId);

  // virtual NodeDisplacement *create(bool *value, Node *node) = 0;

  virtual ~NodeDisplacement() = default;
};

class NodeDisplacementUzPsixPsiy : public NodeDisplacement {
public:
  bool u_z{false}, psi_x{false}, psi_y{false};

  NodeDisplacementUzPsixPsiy(bool u_z, bool psi_x, bool psi_y);

  virtual void setIndexesToZero(Node *node) override;

  static NodeDisplacement *create(bool *value, Node *node);
};

class NodeDisplacementUz : public NodeDisplacement {
public:
  bool u_z{false};

  NodeDisplacementUz(bool u_z);

  virtual void setIndexesToZero(Node *node) override;

  static NodeDisplacement *create(bool *value, Node *node);
};

class NodeDisplacementPsixPsiy : public NodeDisplacement {
public:
  bool psi_x{false}, psi_y{false};

  NodeDisplacementPsixPsiy(bool psi_x, bool psi_y);

  virtual void setIndexesToZero(Node *node) override;

  static NodeDisplacement *create(bool *value, Node *node);
};