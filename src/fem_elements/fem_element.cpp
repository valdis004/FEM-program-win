#include <qdebug.h>
#include <qexception.h>
#include <qglobal.h>
#include <Eigen/Core>
#include <cstddef>
#include <exception>
#include <memory>

#include "element_provider.h"
#include "fem_element.h"
#include "fem_plates/fem_plate_dkmq.h"
#include "fem_plates/fem_plate_mitc9my.h"
#include "fem_types.h"
#include "general_element/displacement/displacement.h"
#include "load/fem_load.h"
// #include "load/load.h"
#include "fem_plates/fem_plate_mitc4my.h"
#include "general_element/element.h"
#include "mesh\mesh.h"

FemAbstractElement::FemAbstractElement(
    unsigned id, Node** nodes, int count, ElementType type,
    std::shared_ptr<AbstractElement> generalElement)
    : id_(id),
      nodes_count_(count),
      type_(type),
      genetal_element_(generalElement) {
  for (size_t i = 0; i < count; i++) {
    nodes_.push_back(nodes[i]);
  }
}

FemAbstractElement::FemAbstractElement(
    unsigned id, Node** nodes, int count, const Material& material,
    ElementType type, std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, count, type, generalElement) {}

FemAbstractElement* FemAbstractElement::create(
    unsigned id, ElementType type, Node** nodes, int count,
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

/* static */ unsigned FemAbstractElement::getCorrection(
    unsigned cur_node_loc_id, unsigned cur_node_glob_id,
    const ElementData& data, const QVector<unsigned>& ids_to_cor) {
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

static unsigned element_count = 0;

void FemAbstractElement::setCalcProps(FemAbstractElement* p_element,
                                      unsigned& global_matrix_size,
                                      const ElementData& data) {
  auto general_element = p_element->genetal_element_;

  // Set load parameters
  bool is_load = p_element->general_Load_ != nullptr;
  bool is_displ = p_element->general_disp_ != nullptr;
  VectorXd coefs = p_element->getLoadVector();

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
    Node* current_node = p_element->nodes_[i];
    double current_coefs[cur_dof];
    short id = current_node->id;

    short correction = 0;
    if (!is_full_dof) {
      auto ids_to_cor = general_element->meshData_->ids_to_cor_;
      correction = getCorrection(i, id, data, ids_to_cor);
    }

    unsigned global_index = id * full_dof - correction;
    current_node->firstGlobStiffId = global_index;

    // add global stiff matrix size property to ref variable global_matrix_size
    // that provided by mesh data of current general element
    if (global_index + cur_dof > global_matrix_size) {
      global_matrix_size = global_index + cur_dof;
    }

    // Coefficients for load vector in isoparametric formulation (for
    // defflection \int Nw q detJ dxi deta)
    for (size_t j = 0; j < cur_dof; j++) {
      current_coefs[j] = coefs[count_coefs++];
    }

    // if node has displacement create array with indexies that need to set to 0
    if (current_node->nodeDisplacement) {
      current_node->nodeDisplacement->setIndexesToZero(current_node);
    }

    // if general element has load and current node dont has node load create
    // from general load node load
    if (is_load && !current_node->nodeLoad) {
      current_node->nodeLoad = NodeLoad::createNodeLoadFromLoad(
          p_element->type_, p_element->general_Load_, current_coefs, i);
    }
  }
  // Set index that used to define the correc_count variable
  if (!is_full_dof) {
    for (size_t i = 0; i < data.NODES_COUNT; i++) {
      if (data.IS_NODE_BAD_DOF_MAP[i]) {
        unsigned id_to_add = p_element->nodes_[i]->id;
        general_element->meshData_->ids_to_cor_.push_back(id_to_add);
      }
    }
  }

  element_count++;
}
