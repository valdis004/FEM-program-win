#pragma once

#include <QVector>
#include <memory>
#include <qglobal.h>

#include "displacement/displacement.h"
#include "elements/femtypes.h"
#include "elements/point.h"
#include "load/load.h"
#include "mesh/meshdata.h"

class Node;
class FemAbstractElement;

using std::shared_ptr;

static unsigned count = 0;

class AbstractElement {
private:
  // Geometry
  ElementType type{ElementType::NONE};
  Point3 statrtPoint{0, 0, 0};
  unsigned lenght{0};

  // Structural parameters
  QVector<shared_ptr<AbstractLoad>> loads;
  QVector<shared_ptr<AbstractDisplacement>> displacements;

public:
  // Mesh
  std::shared_ptr<MeshData> meshData{nullptr};

  // Other
  QString name;

  AbstractElement();

  AbstractElement(shared_ptr<AbstractLoad> load, ElementType type,
                  unsigned lenght);

  ElementType getType() const;

  Point3 getStartPoint() const;

  double getLenght() const;

  void addLoad(shared_ptr<AbstractLoad> load);

  inline short loadCount() const;

  virtual shared_ptr<AbstractLoad> createAndAddLoad() = 0;

  // void setMeshData(shared_ptr<MeshData> meshData);
};
