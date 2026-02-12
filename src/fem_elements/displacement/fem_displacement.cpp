#include "fem_displacement.h"
// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/displacement/displacement.h"
#include "fem_elements/element_provider.h"
#include "fem_elements/node.h"

// /*static*/ NodeDisplacement *NodeDisplacement::create(DisplType type,
//                                                       bool *value) {
//   switch (type) {
//   case DisplType::UzRxRy:
//     return new NodeDisplacementUzPsixPsiy(value[0], value[1], value[2]);
//     break;
//   case DisplType::RxRy:
//     return new NodeDisplacementPsixPsiy(value[1], value[2]);
//   case DisplType::Uz:
//     return new NodeDisplacementUz(value[0]);
//   default:
//     throw std::runtime_error("Unknown displacement type");
//   }
// }

ANodeDisplacement* ANodeDisplacement::createNodeDisplacementFromDisplacement(
    ElementType type, AStructuralDisplacement* displ, Node* node, int locId) {
  auto data = ElementProvider.at(type);

  bool values[data.NODES_COUNT];
  displ->setValues(
      values);  // To get disp value from parameter Displacement *displ

  // Cretate for those parameteres node Disp
  return data.DISP_FN_MAP[locId](values, node);
}

std::array<int, 6> ANodeDisplacement::getDisplacementInfo() {
  return {u_x_, u_y_, u_z_, psi_x_, psi_y_, psi_z_};
}

/* NodeDisplacementUzPsixPsiy */

NodeDisplacementUzPsixPsiy::NodeDisplacementUzPsixPsiy(bool u_z,
                                                       bool psi_x,
                                                       bool psi_y)
    : ANodeDisplacement(0, 0, u_z, psi_x, psi_y, 0) {
  short count = 0;
  u_z_ ? count++ : 0;
  psi_x_ ? count++ : 0;
  psi_y_ ? count++ : 0;
  node_ids_to_zero_.resize(count);
}

/*virtual*/ void NodeDisplacementUzPsixPsiy::setIndexesToZero(Node* node) {
  unsigned firstGlobalIndex = node->firstGlobStiffId;

  bool dislps[] = {u_z_, psi_x_, psi_y_};
  int count = 0;
  for (std::size_t i = 0; i < 3 /* 3 dof in this class */; i++) {
    if (dislps[i]) {
      node_ids_to_zero_[count++] = firstGlobalIndex + i;
    }
  }
}

/*virtual*/ ANodeDisplacement* NodeDisplacementUzPsixPsiy::create(bool* value,
                                                                  Node* node) {
  ANodeDisplacement* disp =
      new NodeDisplacementUzPsixPsiy(value[0], value[1], value[2]);

  disp->setIndexesToZero(node);

  return disp;
}

/* NodeDisplacementUz */

NodeDisplacementUz::NodeDisplacementUz(bool u_z)
    : ANodeDisplacement(0, 0, u_z, 0, 0, 0) {
  if (u_z) node_ids_to_zero_.resize(1);
}

/*virtual*/ void NodeDisplacementUz::setIndexesToZero(Node* node) {
  unsigned firstGlobalIndex = node->firstGlobStiffId;
  u_z ? node_ids_to_zero_[0] = firstGlobalIndex : 0;
}

/*virtual*/ ANodeDisplacement* NodeDisplacementUz::create(bool* value,
                                                          Node* node) {
  ANodeDisplacement* disp = new NodeDisplacementUz(value[0]);

  disp->setIndexesToZero(node);

  return disp;
}

/* NodeDisplacementPsixPsiy */

NodeDisplacementPsixPsiy::NodeDisplacementPsixPsiy(bool psi_x, bool psi_y)
    : ANodeDisplacement(0, 0, 0, psi_x, psi_y, 0) {
  short count = 0;
  psi_x_ ? count++ : 0;
  psi_y_ ? count++ : 0;

  node_ids_to_zero_.resize(count);
}

/*virtual*/ void NodeDisplacementPsixPsiy::setIndexesToZero(Node* node) {
  unsigned firstGlobalIndex = node->firstGlobStiffId;

  bool dislps[] = {psi_x_, psi_y_};
  int count = 0;
  for (std::size_t i = 0; i < 2 /* 3 dof in this class */; i++) {
    if (dislps[i]) {
      node_ids_to_zero_[count++] = firstGlobalIndex + i;
    }
  }
}

/*virtual*/ ANodeDisplacement* NodeDisplacementPsixPsiy::create(bool* value,
                                                                Node* node) {
  ANodeDisplacement* disp = new NodeDisplacementPsixPsiy(value[0], value[1]);

  disp->setIndexesToZero(node);

  return disp;
}
