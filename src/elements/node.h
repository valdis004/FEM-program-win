#pragma once

#include <QVector>
#include <qglobal.h>

#include "displacement/femdisplacement.h"
#include "elements/load/femload.h"
#include "elements/point.h"

// class NodeLoad;

class Node {
public:
  Point3 point;
  Point3 glPoint;
  double glOutputValue;
  int dofCount;
  unsigned id;
  NodeLoad *nodeLoad{nullptr};
  NodeDisplacement *nodeDisplacement{nullptr};
  unsigned firstGlobStiffId;
  QVector<double> outputValues;

  Node() = default;

  Node(const Node &other)
      : point(other.point), glPoint(other.point), dofCount(other.dofCount),
        id(other.id), nodeLoad(other.nodeLoad),
        nodeDisplacement(other.nodeDisplacement),
        firstGlobStiffId(other.firstGlobStiffId) {}

  Node(Point3 point, int dofCount, int id, short outputCount)
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