#pragma once

#include "femelement.h"
#include <QVector>
// #include <memory>
#include <qglobal.h>

class Node;
class FemAbstractElement;

class MeshData {
public:
  // Settings of mesh
  int step{1000};

  QVector<Node *> nodes;
  QVector<FemAbstractElement *> femElements;
  unsigned globaStiffMatrixSize{0};

  MeshData();

  MeshData(QVector<Node *> &&nodes, QVector<FemAbstractElement *> &&femElements,
           unsigned globaStiffMatrixSize) noexcept;

  void setData(QVector<Node *> &&nodes,
               QVector<FemAbstractElement *> &&femElements,
               unsigned globaStiffMatrixSize) noexcept;

  bool isEmpty() const noexcept;

  ~MeshData();
};
