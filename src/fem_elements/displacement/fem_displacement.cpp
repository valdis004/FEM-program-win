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

NodeDisplacement *NodeDisplacement::createNodeDisplacementFromDisplacement(
    ElementType type, AbstractDisplacement *displ, Node *node, int locId) {
  auto data = ElementProvider.at(type);

  bool values[data.NODES_COUNT];
  displ->setValues(
      values); // To get disp value from parameter Displacement *displ

  // Cretate for those parameteres node Disp
  return data.DISP_FN_MAP[locId](values, node);
}

/* NodeDisplacementUzPsixPsiy */

NodeDisplacementUzPsixPsiy::NodeDisplacementUzPsixPsiy(bool u_z, bool psi_x,
                                                       bool psi_y)
    : u_z(u_z), psi_x(psi_x), psi_y(psi_y) {
  u_z ? nodesCountToZero++ : 0;
  psi_x ? nodesCountToZero++ : 0;
  psi_y ? nodesCountToZero++ : 0;
}

/*virtual*/ void NodeDisplacementUzPsixPsiy::setIndexesToZero(Node *node) {
  unsigned firstGlobalIndex = node->firstGlobStiffId;
  nodeIdsToZero = new unsigned[nodesCountToZero];

  bool dislps[] = {u_z, psi_x, psi_y};
  int count = 0;
  for (std::size_t i = 0; i < 3 /* 3 dof in this class */; i++) {
    if (dislps[i]) {
      nodeIdsToZero[count++] = firstGlobalIndex + i;
    }
  }
}

/*virtual*/ NodeDisplacement *NodeDisplacementUzPsixPsiy::create(bool *value,
                                                                 Node *node) {
  NodeDisplacement *disp =
      new NodeDisplacementUzPsixPsiy(value[0], value[1], value[2]);

  disp->setIndexesToZero(node);

  return disp;
}

/* NodeDisplacementUz */

NodeDisplacementUz::NodeDisplacementUz(bool u_z) : u_z(u_z) {
  u_z ? nodesCountToZero++ : 0;
}

/*virtual*/ void NodeDisplacementUz::setIndexesToZero(Node *node) {
  unsigned firstGlobalIndex = node->firstGlobStiffId;
  nodeIdsToZero = new unsigned[nodesCountToZero];

  u_z ? *nodeIdsToZero = firstGlobalIndex : 0;
}

/*virtual*/ NodeDisplacement *NodeDisplacementUz::create(bool *value,
                                                         Node *node) {
  NodeDisplacement *disp = new NodeDisplacementUz(value[0]);

  disp->setIndexesToZero(node);

  return disp;
}

/* NodeDisplacementPsixPsiy */

NodeDisplacementPsixPsiy::NodeDisplacementPsixPsiy(bool psi_x, bool psi_y)
    : psi_x(psi_x), psi_y(psi_y) {
  psi_x ? nodesCountToZero++ : 0;
  psi_y ? nodesCountToZero++ : 0;
}

/*virtual*/ void NodeDisplacementPsixPsiy::setIndexesToZero(Node *node) {
  unsigned firstGlobalIndex = node->firstGlobStiffId;
  nodeIdsToZero = new unsigned[nodesCountToZero];

  bool dislps[] = {psi_x, psi_y};
  int count = 0;
  for (std::size_t i = 0; i < 2 /* 3 dof in this class */; i++) {
    if (dislps[i]) {
      nodeIdsToZero[count++] = firstGlobalIndex + i;
    }
  }
}

/*virtual*/ NodeDisplacement *NodeDisplacementPsixPsiy::create(bool *value,
                                                               Node *node) {
  NodeDisplacement *disp = new NodeDisplacementPsixPsiy(value[0], value[1]);

  disp->setIndexesToZero(node);

  return disp;
}
