#include <qcontainerfwd.h>
#include <qglobal.h>

#include <QList>
#include <QtAlgorithms>
#include <memory>
// #include <stdexcept>

// #include <new>

// #include "../elements/element.h"
#include "fem_elements/element_provider.h"
// #include "../elements/load/load.h"
#include "fem_element.h"
#include "fem_load.h"
#include "fem_types.h"
#include "general_element/displacement/displacement.h"
#include "general_element/element.h"
#include "mesh.h"
#include "meshdata.h"

// Mesh::~Mesh() {
//   for (auto node : nodes) {
//     delete node;
//   }

//   for (auto element : femElements) {
//     delete element;
//   }
// }

bool Mesh::isEqual(const Point3& p1, const Point3& p2) {
  return abs(p1.x - p2.x) < 0.001 && abs(p1.y - p2.y) < 0.001 &&
         abs(p1.y - p2.y) < 0.001;
}

unsigned Mesh::maxNodeIndexInList(const QList<Node>& list) {
  if (list.empty()) throw std::exception();

  int maxIndex = list.first().id;
  for (auto item : list) {
    if (item.id > maxIndex) maxIndex = item.id;
  }
  return maxIndex;
}

void Mesh::meshCreateManager(QVector<shared_ptr<AbstractElement>>* elements,
                             bool standartScheme) {
  if (standartScheme) {
    for (auto element : *elements) {
      if (element->meshData_ == nullptr) createDefaultMesh(element);
    }
  }
}

void Mesh::createDefaultMesh(shared_ptr<AbstractElement> element) {
  ElementType type = element->getType();
  auto& DATA = ElementProvider.at(element->getType());
  double loadv[] = {-100, 0, 0};
  AbstractLoad* load = new AreaLoadFzMxMy(loadv, 3);

  Point3 point00 = element->getStartPoint();

  element->meshData_ = std::make_unique<MeshData>();
  float step = element->meshData_->step;
  float lenght_plate = element->getLenght();  // В мм
  int steps = (int)(lenght_plate / step);
  int element_count = lenght_plate * lenght_plate / (step * step);
  element->meshData_->ids_to_cor_.reserve(element_count);

  unsigned globa_stiffm_size = 0;
  QVector<Node*> nodes;
  QVector<FemAbstractElement*> femElements;
  femElements.reserve(+element_count);
  nodes.reserve(element_count * 20);

  float sin_a = 0;
  float cos_a = 1;

  int elem_counter = 0;
  int node_counter = 0;

  for (int l = 0; l < steps; l++) {
    for (int k = 0; k < steps; k++) {
      Point3 point0{point00.x + l * step * cos_a, point00.y + k * step,
                    point00.z + l * step * sin_a};
      const int ndsCntElm = DATA.NODES_COUNT;
      Node* nodes_to_elem[ndsCntElm];
      float check_value = 0.01f;

      for (int j = 0; j < ndsCntElm; j++) {
        Point3 point_for_node =
            DATA.GET_POINT_FROM_INDEX_FN(j, point0, step, cos_a, sin_a);

        // Проверка на то есть в этой точке уже нод или нет
        Node* node;
        for (auto item : nodes) {
          if (isEqual(item->point, point_for_node)) {
            node = new Node(*item);
            goto node_already_exists;
          }
        }

        node = new Node(node_counter++, point_for_node, DATA.FULL_DOF_COUNT,
                        DATA.OUTPUT_VALUES_COUNT);

        // Add displ
        if (node->point.x == point00.x ||
            node->point.x == point00.x + lenght_plate) {
          NodeDisplacementUzPsixPsiy* disp =
              new NodeDisplacementUzPsixPsiy(true, false, false);
          node->nodeDisplacement = disp;
        }

        if (node->point.y == point00.y ||
            node->point.y == point00.y + lenght_plate) {
          NodeDisplacementUzPsixPsiy* disp =
              new NodeDisplacementUzPsixPsiy(true, false, false);
          node->nodeDisplacement = disp;
        }

        if (node->point.y == (point00.y + lenght_plate) / 2.0 ||
            node->point.x == (point00.x + lenght_plate) / 2.0) {
          double f_z[] = {-10};
          NodeLoadFz* load = new NodeLoadFz(f_z);
          node->nodeLoad = load;
        }

      node_already_exists:
        nodes.push_back(node);
        nodes_to_elem[j] = node;
      }

      auto fem_element = FemAbstractElement::create(
          elem_counter++, type, nodes_to_elem, DATA.NODES_COUNT, element);
      fem_element->setLoad(load);
      FemAbstractElement::setCalcProps(fem_element, globa_stiffm_size, DATA);
      femElements.push_back(fem_element);

      emit progressChanged(elem_counter);
    }
  }

  element->meshData_->setData(std::move(nodes), std::move(femElements),
                              globa_stiffm_size);

  emit meshFinished(element_count);
}
