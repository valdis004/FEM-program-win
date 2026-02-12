#include <qcontainerfwd.h>
#include <qglobal.h>

#include <QList>
#include <QtAlgorithms>
#include <cstddef>
#include <fstream>
#include <memory>
#include <stdexcept>

// #include <stdexcept>

// #include <new>

// #include "../elements/element.h"
#include "fem_displacement.h"
#include "fem_elements/element_provider.h"
// #include "../elements/load/load.h"
#include "fem_element.h"
#include "fem_load.h"
#include "fem_types.h"
#include "mesh.h"
#include "meshdata.h"
#include "structural_displacement/displacement.h"
#include "structural_element/structural_element.h"

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

void Mesh::meshCreateManager(QVector<shared_ptr<AStructuralElement>>* elements,
                             bool standartScheme) {
  if (standartScheme) {
    for (auto element : *elements) {
      if (element->meshData_ == nullptr) {
        createDefaultMesh(element);
        writeElementData(element);
      }
    }
  }
}

void Mesh::createDefaultMesh(shared_ptr<AStructuralElement> element) {
  ElementType type = element->getType();
  auto& DATA = ElementProvider.at(element->getType());

  Point3 point00 = element->getStartPoint();

  element->meshData_ = std::make_unique<MeshData>();
  float step = element->meshData_->step;
  float lenght_plate = element->getLenght();  // В мм
  int steps = (int)(lenght_plate / step);
  int element_count = lenght_plate * lenght_plate / (step * step);
  element->meshData_->ids_to_cor_.reserve(element_count);

  unsigned globa_stiffm_size = 0;
  QVector<Node*> nodes;
  QVector<AFemElement*> femElements;
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

        // Load opensees
        // if (node->point.x == lenght_plate && node->glPoint.y == 0) {
        //   double v[] = {-10, 0, 0};
        //   NodeLoad* load = new NodeLoadFzMxMy(v);
        //   node->nodeLoad = load;
        // }

        // // disp opensees
        // if (node->point == point00) {
        //   NodeDisplacement* disp =
        //       new NodeDisplacementUzPsixPsiy(true, false, false);
        //   node->nodeDisplacement = disp;
        // }

        // if (node->point.x == lenght_plate && node->point.y == lenght_plate) {
        //   NodeDisplacement* disp =
        //       new NodeDisplacementUzPsixPsiy(true, false, false);
        //   node->nodeDisplacement = disp;
        // }

        // if (node->point.x == 0 && node->point.y == lenght_plate) {
        //   NodeDisplacement* disp =
        //       new NodeDisplacementUzPsixPsiy(true, false, false);
        //   node->nodeDisplacement = disp;
        // }

      node_already_exists:
        nodes.push_back(node);
        nodes_to_elem[j] = node;
      }

      auto fem_element = AFemElement::create(
          elem_counter++, type, nodes_to_elem, DATA.NODES_COUNT, element);
      AFemElement::setCalcProps(fem_element, globa_stiffm_size, DATA);
      femElements.push_back(fem_element);

      emit progressChanged(elem_counter);
    }
  }

  element->meshData_->setData(std::move(nodes), std::move(femElements),
                              globa_stiffm_size);

  emit meshFinished(element_count);
}

void Mesh::writeElementData(shared_ptr<AStructuralElement> str_element) {
  auto& mesh = str_element->meshData_;
  auto fem_elements = mesh->femElements;
  auto fem_nodes = mesh->nodes_;

  // load and displacement text for shell
  std::ostringstream load_text;
  load_text << "pattern Plain 1 Linear {\n";
  std::ostringstream displacement_text;

  std::ofstream out("C:\\Users\\vlada\\Documents\\FEM\\OpenSees\\mesh.txt.txt");

  if (!out.is_open()) {
    throw std::runtime_error("Mesh file cant open and dont know why");
  }

  out << "# nodes\n";
  for (size_t i = 0; i < fem_nodes.size(); i++) {
    Node* node = fem_nodes[i];

    out << "node " << node->id << " " << node->point.x << " " << node->point.y
        << " " << node->point.z << ";\n";

    if (node->nodeDisplacement) {
      displacement_text << "fix " << node->id << " ";

      auto disp_data = node->nodeDisplacement->getDisplacementInfo();

      for (auto value : disp_data) {
        displacement_text << value << " ";
      }
      displacement_text << ";\n";
    }

    if (node->nodeLoad) {
      double fz = node->nodeLoad->values_[0];
      double mx = node->nodeLoad->values_[1];
      double my = node->nodeLoad->values_[2];
      load_text << "load " << node->id << " 0 0 " << fz << " " << mx << " "
                << my << " 0;\n";
    }
  }
  load_text << "}";

  out << "\n\n# loads\n";
  out << load_text.str();
  out << "\n\n# displacements\n";
  out << displacement_text.str();

  out << "\n\n# Elements\n";
  // Write element mesh code
  for (size_t i = 0; i < fem_elements.size(); i++) {
    out << "element $EleType ";

    for (size_t j = 0; j < fem_elements[i]->nodes_count_; j++) {
      out << fem_elements[i]->nodes_[j]->id << " ";
    }

    out << "$secArgs;\n";

    // After one iteration: element $EleType 1 1 2 3 4 $secArgs;
  }

  out.close();
}
