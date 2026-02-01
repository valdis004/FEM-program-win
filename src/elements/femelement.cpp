#include <Eigen/Core>
#include <cstddef>
#include <exception>
#include <memory>
#include <qdebug.h>
#include <qexception.h>
#include <qglobal.h>

#include "elementprovider.h"
#include "fem_plates/fem_plate_dkmq.h"
#include "femelement.h"
#include "femtypes.h"
#include "generalElement/displacement/displacement.h"
#include "load/femload.h"
// #include "load/load.h"
#include "fem_plates/fem_plate_mitc4my.h"

FemAbstractElement::FemAbstractElement(
    unsigned id, Node **nodes, int count, ElementType type,
    std::shared_ptr<AbstractElement> generalElement)
    : id(id), nodesCount(count), type(type), genetalElement(generalElement) {
  for (size_t i = 0; i < count; i++) {
    this->nodes.push_back(nodes[i]);
  }
}

FemAbstractElement::FemAbstractElement(
    unsigned id, Node **nodes, int count, const Material &material,
    ElementType type, std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, count, type, generalElement) {}

FemAbstractElement *
FemAbstractElement::create(unsigned id, ElementType type, Node **nodes,
                           int count,
                           std::shared_ptr<AbstractElement> generalElement) {
  switch (type) {
  case ElementType::MITC4MY: {
    return new FemPlateMitc4My(id, nodes, generalElement);
  }
  case ElementType::DKMQ: {
    return new FemPlateDkmq(id, nodes, generalElement);
  }
  default:
    throw std::exception();
  }
}

void FemAbstractElement::setCalcProps(FemAbstractElement *ptr,
                                      unsigned &global_matrix_size,
                                      const ElementData &data) {

  // Set load parameters
  bool is_load = ptr->generalLoad != nullptr;
  bool is_displ = ptr->generalDisp != nullptr;
  VectorXd coefs = ptr->getLoadVector();

  // for (size_t i = 0; i < coefs.size(); i++) {
  //   qDebug() << coefs[i];
  // }

  // Bad dof parameters
  short bad_dof_begin = data.BAD_DOF_BEGIN;
  short bad_dof_count = data.BAD_DOF_COUNT;
  short correc_count, correctFromPrevNode, corFromCurNode;
  bool is_full_dof = data.IS_FULL_DOF;

  auto dof_map = data.DOF_MAP;
  auto bad_dof_map = data.BAD_DOF_MAP;

  int count_coefs = 0;
  for (size_t i = 0; i < data.NODES_COUNT; i++) {
    const short cur_dof = dof_map[i];
    Node *current_node = ptr->nodes[i];
    double current_coefs[cur_dof];
    short id = current_node->id;

    short correction = 0;
    if (!is_full_dof) {
      correc_count = id / bad_dof_begin;
      correctFromPrevNode = correc_count * bad_dof_count;
      corFromCurNode = bad_dof_map[i];

      correction = correctFromPrevNode + corFromCurNode;
    }

    unsigned global_index = id * cur_dof - correction;
    current_node->firstGlobStiffId = global_index;

    // Добавление свойства размера глобальной матрицы жесткости для Sovet
    if (global_index + cur_dof > global_matrix_size) {
      global_matrix_size = global_index + cur_dof;
    }

    for (size_t j = 0; j < cur_dof; j++) {
      current_coefs[j] = coefs[count_coefs++];
    }

    if (current_node->nodeDisplacement) {
      current_node->nodeDisplacement->setIndexesToZero(current_node);
    }

    if (is_load && !current_node->nodeLoad) {
      current_node->nodeLoad = NodeLoad::createNodeLoadFromLoad(
          ptr->type, ptr->generalLoad, current_coefs, i);
    }

    // if (isLoad) {
    //   if (currentNode->nodeLoad) {
    //     currentNode->nodeLoad->appendValuesToNodeLoad(ptr->generalLoad,
    //                                                   currentCoefs);
    //   } else {
    //     currentNode->nodeLoad = NodeLoad::createNodeLoadFromLoad(
    //         ptr->type, ptr->generalLoad, currentCoefs, i);
    //   }
    // }

    // if (isDispl) {
    //   currentNode->nodeDisplacement =
    //       createNodeDisplacementFromDisplacement<type>(ptr->generalDisp,
    //                                                    currentNode, i);
    // }
  }
}
