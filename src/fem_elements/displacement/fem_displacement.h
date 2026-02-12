#pragma once

#include <QVector>
#include <array>
#include <cstddef>
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/node.h"

#include "fem_elements/fem_types.h"

class Node;
class AStructuralDisplacement;

class ANodeDisplacement {
 protected:
  bool u_x_{false}, u_y_{false}, u_z_{false}, psi_x_{false}, psi_y_{false},
      psi_z_{false};

 public:
  QVector<unsigned> node_ids_to_zero_;

  ANodeDisplacement() = default;

  ANodeDisplacement(
      bool u_x, bool u_y, bool u_z, bool psi_x, bool psi_y, bool psi_z)
      : u_x_(u_x),
        u_y_(u_y),
        u_z_(u_z),
        psi_x_(psi_x),
        psi_y_(psi_y),
        psi_z_(psi_z) {}

  virtual void setIndexesToZero(Node* node) = 0;

  static ANodeDisplacement* createNodeDisplacementFromDisplacement(
      ElementType type, AStructuralDisplacement* displ, Node* node, int locId);

  // virtual NodeDisplacement *create(bool *value, Node *node) = 0;

  virtual ~ANodeDisplacement() = default;

  std::array<int, 6> getDisplacementInfo();
};

class NodeDisplacementUzPsixPsiy : public ANodeDisplacement {
 public:
  bool u_z{false}, psi_x{false}, psi_y{false};

  NodeDisplacementUzPsixPsiy(bool u_z, bool psi_x, bool psi_y);

  virtual void setIndexesToZero(Node* node) override;

  static ANodeDisplacement* create(bool* value, Node* node);
};

class NodeDisplacementUz : public ANodeDisplacement {
 public:
  bool u_z{false};

  NodeDisplacementUz(bool u_z);

  virtual void setIndexesToZero(Node* node) override;

  static ANodeDisplacement* create(bool* value, Node* node);
};

class NodeDisplacementPsixPsiy : public ANodeDisplacement {
 public:
  bool psi_x{false}, psi_y{false};

  NodeDisplacementPsixPsiy(bool psi_x, bool psi_y);

  virtual void setIndexesToZero(Node* node) override;

  static ANodeDisplacement* create(bool* value, Node* node);
};