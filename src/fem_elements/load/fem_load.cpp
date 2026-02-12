#include "fem_load.h"

#include <cstddef>

#include "fem_elements/element_provider.h"
#include "fem_elements/fem_element.h"
#include "fem_elements/node.h"
#include "structural_element/structural_element.h"
#include "structural_element/structural_load/load.h"

ANodeLoad::ANodeLoad(std::span<double> values) {
  values_.resize(values.size());
  for (std::size_t i = 0; i < values.size(); i++) {
    values_[i] = values[i];
  }
}

ANodeLoad* ANodeLoad::createNodeLoadFromLoad(ElementType type,
                                             AStructuralLoad* load,
                                             double* coefs,
                                             int localNodeId) {
  auto data = ElementProvider.at(type);
  const int dof = data.FULL_DOF_COUNT;

  double values[dof];
  load->setValues(values);

  return data.LOAD_FN_MAP[localNodeId](values, coefs);
}

/* static */ void ANodeLoad::setNodeLoadToNodeFromLoad(Node* node,
                                                       AFemElement* fem_element,
                                                       double* coefs,
                                                       int localNodeId) {
  auto data = ElementProvider.at(fem_element->type_);
  const int dof = data.FULL_DOF_COUNT;

  double values[dof];
  for (auto& load : fem_element->genetal_element_->getLoads()) {
    load->setValues(values);

    // If node load dont existst create load, else append values to node load
    // which already exists
    if (!node->nodeLoad) {
      node->nodeLoad = data.LOAD_FN_MAP[localNodeId](values, coefs);
    } else {
      node->nodeLoad->appendValuesToNodeLoad(values, coefs);
    }
  }
};

/* virtual */ void NodeLoadFzMxMy::appendValuesToNodeLoad(double* values,
                                                          double* coefs) {
  fz += values[0] * coefs[0];
  mx += values[1] * coefs[1];
  my += values[2] * coefs[2];

  for (size_t i = 0; i < 3; i++) {
    values_[i] += values[i] * coefs[i];
  }
}

/*virtual*/ ANodeLoad* NodeLoadFzMxMy::create(double* values, double* coefs) {
  ANodeLoad* nodeLoad = new NodeLoadFzMxMy();
  nodeLoad->setNodeLoadValues(values, coefs);

  return nodeLoad;
}

/* virtual */ void NodeLoadFzMxMy::setNodeLoadValues(double* values,
                                                     double* coefs) {
  fz = values[0] * coefs[0];
  mx = values[1] * coefs[1];
  my = values[2] * coefs[2];

  values_.resize(3);
  for (size_t i = 0; i < 3; i++) {
    values_[i] += values[i] * coefs[i];
  }
};

/* virtual */ void NodeLoadFz::appendValuesToNodeLoad(double* values,
                                                      double* coefs) {
  fz += values[0] * coefs[0];

  values_[0] += values[0] * coefs[0];
}

/*virtual */ void NodeLoadFz::setNodeLoadValues(double* value, double* coefs) {
  fz = value[0] * coefs[0];

  values_.resize(1);
  values_[0] = fz;
};

/*virtual*/ ANodeLoad* NodeLoadFz::create(double* values, double* coefs) {
  ANodeLoad* nodeLoad = new NodeLoadFz();
  nodeLoad->setNodeLoadValues(values, coefs);

  return nodeLoad;
}

/* virtual */ void NodeLoadMxMy::appendValuesToNodeLoad(double* values,
                                                        double* coefs) {
  mx += values[1] * coefs[1];
  my += values[2] * coefs[2];

  values_[0] += values[1] * coefs[1];
  values_[1] += values[2] * coefs[2];
}

/*virtual */ void NodeLoadMxMy::setNodeLoadValues(double* value,
                                                  double* coefs) {
  mx = value[1] * coefs[1];
  my = value[2] * coefs[1];

  values_.resize(2);
  values_[0] += mx;
  values_[1] += my;
};

/*virtual*/ ANodeLoad* NodeLoadMxMy::create(double* values, double* coefs) {
  ANodeLoad* nodeLoad = new NodeLoadMxMy();
  nodeLoad->setNodeLoadValues(values, coefs);

  return nodeLoad;
}