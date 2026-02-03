#include <Eigen/Core>
#include <cstddef>
#include <exception>
#include <memory>
#include <qdebug.h>
#include <qexception.h>
#include <qglobal.h>

#include "elementprovider.h"
#include "fem_plates/fem_plate_dkmq.h"
#include "fem_plates/fem_plate_mitc9my.h"
#include "femelement.h"
#include "femtypes.h"
#include "generalElement/displacement/displacement.h"
#include "load/femload.h"
// #include "load/load.h"
#include "fem_plates/fem_plate_mitc4my.h"
#include "generalElement/element.h"
#include "mesh\mesh.h"

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
  case ElementType::MITC9MY: {
    return new FemPlateMitc9My(id, nodes, generalElement);
  }
  default:
    throw std::exception();
  }
}

/* static */ inline unsigned FemAbstractElement::getCorrection(
    unsigned cur_node_loc_id, unsigned cur_node_glob_id,
    const ElementData &data, const QVector<unsigned> &ids_to_cor) {
  unsigned cor_count = 0;

  for (size_t i = 0; i < ids_to_cor.size(); i++) {
    if (cur_node_glob_id <= ids_to_cor[i]) {
      break;
    } else {
      cor_count++;
    }
  }

  unsigned correctFromPrevNode = cor_count * data.BAD_DOF_COUNT;
  unsigned corFromCurNode = data.BAD_DOF_MAP[cur_node_loc_id];

  return correctFromPrevNode + corFromCurNode;
}

static unsigned bad_dof_begin = 100;
static unsigned element_count = 0;

void FemAbstractElement::setCalcProps(FemAbstractElement *p_element,
                                      unsigned &global_matrix_size,
                                      const ElementData &data) {
  auto general_element = p_element->genetalElement;

  // Set load parameters
  bool is_load = p_element->generalLoad != nullptr;
  bool is_displ = p_element->generalDisp != nullptr;
  VectorXd coefs = p_element->getLoadVector();

  // for (size_t i = 0; i < coefs.size(); i++) {
  //   qDebug() << coefs[i];
  // }

  // Bad dof parameters
  short bad_dof_count = data.BAD_DOF_COUNT;
  short correc_count, correctFromPrevNode, corFromCurNode;
  bool is_full_dof = data.IS_FULL_DOF;
  short full_dof = data.FULL_DOF_COUNT;

  auto dof_map = data.DOF_MAP;
  auto bad_dof_map = data.BAD_DOF_MAP;

  int count_coefs = 0;
  for (size_t i = 0; i < data.NODES_COUNT; i++) {
    const short cur_dof = dof_map[i];
    Node *current_node = p_element->nodes[i];
    double current_coefs[cur_dof];
    short id = current_node->id;

    short correction = 0;
    if (!is_full_dof) {
      auto ids_to_cor = general_element->meshData_->ids_to_cor_;
      correction = getCorrection(i, id, data, ids_to_cor);
    }

    unsigned global_index = id * full_dof - correction;
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
          p_element->type, p_element->generalLoad, current_coefs, i);
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
  // Set index that used to define the correc_count variable
  if (!is_full_dof) {
    for (size_t i = 0; i < data.NODES_COUNT; i++) {
      if (data.IS_NODE_BAD_DOF_MAP[i]) {
        unsigned id_to_add = p_element->nodes[i]->id;
        general_element->meshData_->ids_to_cor_.push_back(id_to_add);
      }
    }
  }

  element_count++;
}
