#include "femload.h"
#include "generalElement/load/load.h"
// #include <exception>
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/node.h"
// #include <stdexcept>
#include "elements/elementprovider.h"

NodeLoad *NodeLoad::createNodeLoadFromLoad(ElementType type, AbstractLoad *load,
                                           double *coefs, int localNodeId) {

  auto data = ElementProvider.at(type);
  const int dof = data.FULL_DOF_COUNT;

  double values[dof];
  load->setValues(values);

  return data.LOAD_FN_MAP[localNodeId](values, coefs);
}

/* virtual */ void
NodeLoadFzMxMy::appendValuesToNodeLoad(AbstractLoad *generalLoad,
                                       double *coefs) {
  double values[3];
  generalLoad->setValues(values);
  this->values[0] += values[0] * coefs[0];
  this->values[1] += values[1] * coefs[1];
  this->values[2] += values[2] * coefs[2];
  fz += values[0] * coefs[0];
  mx += values[1] * coefs[1];
  my += values[2] * coefs[2];
}

/* virtual */ void NodeLoadFzMxMy::setNodeLoadValues(double *value,
                                                     double *coefs) {
  fz = value[0] * coefs[0];
  mx = value[1] * coefs[1];
  my = value[2] * coefs[2];
  values = new double[3]{fz, mx, my};
  countValues = 3;
};

/*virtual*/ NodeLoad *NodeLoadFzMxMy::create(double *values, double *coefs) {
  NodeLoad *nodeLoad = new NodeLoadFzMxMy();
  nodeLoad->setNodeLoadValues(values, coefs);

  return nodeLoad;
}

/* virtual */ void NodeLoadFz::appendValuesToNodeLoad(AbstractLoad *generalLoad,
                                                      double *coefs) {
  double values[3];
  generalLoad->setValues(values);
  this->values[0] += values[0] * coefs[0];
  fz += values[0] * coefs[0];
}

/*virtual */ void NodeLoadFz::setNodeLoadValues(double *value, double *coefs) {
  fz = value[0] * coefs[0];
  values = new double[1]{fz};
  countValues = 1;
};

/*virtual*/ NodeLoad *NodeLoadFz::create(double *values, double *coefs) {
  NodeLoad *nodeLoad = new NodeLoadFz();
  nodeLoad->setNodeLoadValues(values, coefs);

  return nodeLoad;
}

/* virtual */ void
NodeLoadMxMy::appendValuesToNodeLoad(AbstractLoad *generalLoad, double *coefs) {
  double values[3];
  generalLoad->setValues(values);
  this->values[0] += values[1] * coefs[1];
  this->values[1] += values[2] * coefs[2];
  mx += values[1] * coefs[1];
  my += values[2] * coefs[2];
}

/*virtual */ void NodeLoadMxMy::setNodeLoadValues(double *value,
                                                  double *coefs) {
  mx = value[1] * coefs[1];
  my = value[2] * coefs[1];
  values = new double[2]{mx, my};
  countValues = 2;
};

/*virtual*/ NodeLoad *NodeLoadMxMy::create(double *values, double *coefs) {
  NodeLoad *nodeLoad = new NodeLoadFzMxMy();
  nodeLoad->setNodeLoadValues(values, coefs);

  return nodeLoad;
}