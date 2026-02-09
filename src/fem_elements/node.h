#pragma once

#include <QVector>
#include <qglobal.h>

#include "displacement/fem_displacement.h"
#include "fem_elements/load/fem_load.h"
#include "fem_elements/point.h"

// class NodeLoad;

static unsigned nodeCounter = 0;

class Node {
public:
  Point3 point;
  double glOutputValue;
  int dofCount;
  unsigned id;
  NodeLoad *nodeLoad{nullptr};
  NodeDisplacement *nodeDisplacement{nullptr};
  unsigned firstGlobStiffId;
  QVector<double> outputValues;

  // OpenGL
  Point3 glPoint;
  bool isNormolize{false};

  Node() = default;

  Node(unsigned id, const Node &other)
      : point(other.point), glPoint(other.point), dofCount(other.dofCount),
        id(id), nodeLoad(other.nodeLoad),
        nodeDisplacement(other.nodeDisplacement),
        firstGlobStiffId(other.firstGlobStiffId) {}

  Node(unsigned id, Point3 point, int dofCount, short outputCount)
      : point(point), glPoint(point), dofCount(dofCount), id(id) {
    outputValues.resize(outputCount + 1);
  }

  ~Node() {
    if (nodeLoad)
      delete nodeLoad;

    if (nodeDisplacement)
      delete nodeDisplacement;
  }

  void setValues(Point3 point, int dofCount, int id) {
    this->point = point, this->dofCount = dofCount, this->id = id;
  }
};