#include <QList>
#include <QtAlgorithms>
#include <memory>
#include <qglobal.h>
// #include <stdexcept>

// #include <new>

// #include "../elements/element.h"
#include "elements/elementprovider.h"
// #include "../elements/load/load.h"
#include "femelement.h"
#include "femtypes.h"
#include "generalElement/displacement/displacement.h"
#include "generalElement/element.h"
#include "mesh.h"

// Mesh::~Mesh() {
//   for (auto node : nodes) {
//     delete node;
//   }

//   for (auto element : femElements) {
//     delete element;
//   }
// }

bool Mesh::isEqual(const Point3 &p1, const Point3 &p2) {
  return abs(p1.x - p2.x) < 0.001 && abs(p1.y - p2.y) < 0.001 &&
         abs(p1.y - p2.y) < 0.001;
}

unsigned Mesh::maxNodeIndexInList(const QList<Node> &list) {
  if (list.empty())
    throw std::exception();

  int maxIndex = list.first().id;
  for (auto item : list) {
    if (item.id > maxIndex)
      maxIndex = item.id;
  }
  return maxIndex;
}

void Mesh::meshCreateManager(QVector<shared_ptr<AbstractElement>> *elements,
                             bool standartScheme) {

  if (standartScheme) {
    for (auto element : *elements) {
      if (element->meshData->isEmpty())
        createDefaultMesh(element);
    }
  }
}

void Mesh::createDefaultMesh(shared_ptr<AbstractElement> element) {
  // ElementProvider::initialize();

  ElementType type = element->getType();
  auto DATA = ElementProvider::elementData[type];
  double loadv[] = {-100, 0, 0};
  AbstractLoad *load = new AreaLoadFzMxMy(loadv, 3);

  Point3 point00 = element->getStartPoint();

  float step = element->meshData->step;
  float lenghtPlate = element->getLenght(); // В мм
  int steps = (int)(lenghtPlate / step);
  int elementCount = lenghtPlate * lenghtPlate / (step * step);

  unsigned globaStiffMatrixSize = 0;
  QVector<Node *> nodes;
  QVector<FemAbstractElement *> femElements;
  femElements.reserve(+elementCount);
  nodes.reserve(elementCount * 20);

  float sinA = 0;
  float cosA = 1;

  int elemCounter = 0;
  int nodeCounter = 0;

  for (int l = 0; l < steps; l++) {
    for (int k = 0; k < steps; k++) {

      Point3 point0{point00.x + l * step * cosA, point00.y + k * step,
                    point00.z + l * step * sinA};
      const int ndsCntElm = DATA.NODES_COUNT;
      Node *nodesToElem[ndsCntElm];
      float checkValue = 0.01f;

      for (int j = 0; j < ndsCntElm; j++) {

        Point3 pointForNode =
            DATA.GET_POINT_FROM_INDEX_FN(j, point0, step, cosA, sinA);

        // Проверка на то есть в этой точке уже нод или нет
        Node *node;
        for (auto item : nodes) {
          if (isEqual(item->point, pointForNode)) {

            node = new Node(*item);
            goto nodeAlreadyExists;
          }
        }

        node = new Node(nodeCounter++, pointForNode, DATA.FULL_DOF_COUNT,
                        DATA.OUTPUT_VALUES_COUNT);

        // Add displ
        if (node->point.x == point00.x ||
            node->point.x == point00.x + lenghtPlate) {
          NodeDisplacementUzPsixPsiy *disp =
              new NodeDisplacementUzPsixPsiy(true, false, false);
          node->nodeDisplacement = disp;
        }

        if (node->point.y == point00.y ||
            node->point.y == point00.y + lenghtPlate) {
          NodeDisplacementUzPsixPsiy *disp =
              new NodeDisplacementUzPsixPsiy(true, false, false);
          node->nodeDisplacement = disp;
        }

      nodeAlreadyExists:
        nodes.push_back(node);
        nodesToElem[j] = node;
      }

      auto femElement = FemAbstractElement::create(
          elemCounter++, type, nodesToElem, DATA.NODES_COUNT, element);
      femElement->setLoad(load);
      FemAbstractElement::setCalcProps(femElement, globaStiffMatrixSize);
      femElements.push_back(femElement);

      emit progressChanged(elemCounter);
    }
  }

  element->meshData->setData(std::move(nodes), std::move(femElements),
                             globaStiffMatrixSize);

  emit meshFinished(elementCount);
}
