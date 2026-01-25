#include <Eigen/Core>
#include <cstddef>
#include <exception>
#include <qdebug.h>
#include <qexception.h>
#include <qglobal.h>

#include "elementprovider.h"
#include "femelement.h"
#include "generalElement/displacement/displacement.h"
#include "load/femload.h"
// #include "load/load.h"
#include "plates/plates.h"

FemAbstractElement::FemAbstractElement(size_t id, Node **nodes, int count,
                                       ElementType type)
    : data(ElementProvider::elementData[type]), id(id), type(type) {
  for (size_t i = 0; i < count; i++) {
    this->nodes.push_back(nodes[i]);
  }
  nodesCount = count;
}

FemAbstractElement::FemAbstractElement(size_t id, Node **nodes, int count,
                                       const Material &material,
                                       ElementType type)
    : FemAbstractElement(id, nodes, count, type) {}

FemAbstractElement *FemAbstractElement::create(size_t id, ElementType type,
                                               Node **nodes, int count) {
  switch (type) {
  case ElementType::MITC4MY: {
    return new MITC4PlateMy(id, nodes);
  }
  default:
    throw std::exception();
  }
}

void FemAbstractElement::setCalcProps(FemAbstractElement *ptr,
                                      unsigned &globalMatrixSize) {

  auto &data = ElementProvider::elementData[ptr->type];
  // Set load parameters
  bool isLoad = ptr->generalLoad != nullptr;
  bool isDispl = ptr->generalDisp != nullptr;
  VectorXd coefs = ptr->getLoadVector();

  // for (size_t i = 0; i < coefs.size(); i++) {
  //   qDebug() << coefs[i];
  // }

  // Bad dof parameters
  short badDofBegin = data.BAD_DOF_BEGIN;
  short badDofCount = data.BAD_DOF_COUNT;
  short correcCount, correctFromPrevNode, corFromCurNode;
  bool isFullDof = data.IS_FULL_DOF;

  auto dofMap = data.DOF_MAP;
  auto badDofMap = data.BAD_DOF_MAP;

  int countCoefs = 0;
  for (size_t i = 0; i < data.NODES_COUNT; i++) {
    const short curDof = dofMap[i];
    Node *currentNode = ptr->nodes[i];
    double currentCoefs[curDof];
    short id = currentNode->id;

    short correction = 0;
    if (!isFullDof) {
      correcCount = id / badDofBegin;
      correctFromPrevNode = correcCount * badDofCount;
      corFromCurNode = badDofMap[i];

      correction = correctFromPrevNode + corFromCurNode;
    }

    unsigned globalIndex = id * curDof - correction;
    currentNode->firstGlobStiffId = globalIndex;

    // Добавление свойства размера глобальной матрицы жесткости для Sovet
    if (globalIndex + curDof > globalMatrixSize) {
      globalMatrixSize = globalIndex + curDof;
    }

    for (size_t j = 0; j < curDof; j++) {
      currentCoefs[j] = coefs[countCoefs++];
    }

    if (currentNode->nodeDisplacement) {
      currentNode->nodeDisplacement->setIndexesToZero(currentNode);
    }

    if (isLoad && !currentNode->nodeLoad) {
      currentNode->nodeLoad = NodeLoad::createNodeLoadFromLoad(
          ptr->type, ptr->generalLoad, currentCoefs, i);
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