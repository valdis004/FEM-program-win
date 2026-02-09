#include "meshdata.h"
// #include <algorithm>

MeshData::~MeshData() {
  // for (auto node : nodes) {
  //   delete node;
  // }

  // for (auto element : femElements) {
  //   delete element;
  // }
}

MeshData::MeshData() {};

MeshData::MeshData(QVector<Node*>&& nodes,
                   QVector<FemAbstractElement*>&& femElements,
                   unsigned globaStiffMatrixSize) noexcept {
  this->nodes_ = nodes;
  this->femElements = femElements;
  this->globaStiffMatrixSize = globaStiffMatrixSize;
}

void MeshData::setData(QVector<Node*>&& nodes,
                       QVector<FemAbstractElement*>&& femElements,
                       unsigned globaStiffMatrixSize) noexcept {
  this->nodes_ = nodes;
  this->femElements = femElements;
  this->globaStiffMatrixSize = globaStiffMatrixSize;
}

bool MeshData::isEmpty() const noexcept {
  return femElements.isEmpty() && nodes_.isEmpty();
}
