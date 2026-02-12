#pragma once

#include <QVector>
// #include <memory>
#include <qglobal.h>

#include "fem_element.h"

class Node;
class AFemElement;

class MeshData {
 public:
  // Settings of mesh
  int step{1000};

  QVector<Node*> nodes_;
  QVector<AFemElement*> femElements;

  unsigned globaStiffMatrixSize{0};

  // Ids after which need to considering correction of previos node (id current
  // type of element need it)
  QVector<unsigned> ids_to_cor_;

  MeshData();

  MeshData(QVector<Node*>&& nodes,
           QVector<AFemElement*>&& femElements,
           unsigned globaStiffMatrixSize) noexcept;

  void setData(QVector<Node*>&& nodes,
               QVector<AFemElement*>&& femElements,
               unsigned globaStiffMatrixSize) noexcept;

  bool isEmpty() const noexcept;

  ~MeshData();
};
