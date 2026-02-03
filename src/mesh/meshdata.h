#pragma once

#include <QVector>
// #include <memory>
#include <qglobal.h>

#include "femelement.h"

class Node;
class FemAbstractElement;

class MeshData {
public:
  // Settings of mesh
  int step{8000};

  QVector<Node *> nodes;
  QVector<FemAbstractElement *> femElements;

  unsigned globaStiffMatrixSize{0};

  // Ids after which need to considering correction of previos node (id current
  // type of element need it)
  QVector<unsigned> ids_to_cor_;

  MeshData();

  MeshData(QVector<Node *> &&nodes, QVector<FemAbstractElement *> &&femElements,
           unsigned globaStiffMatrixSize) noexcept;

  void setData(QVector<Node *> &&nodes,
               QVector<FemAbstractElement *> &&femElements,
               unsigned globaStiffMatrixSize) noexcept;

  bool isEmpty() const noexcept;

  ~MeshData();
};
