#pragma once

#include <QVector>
#include <memory>
#include <qcontainerfwd.h>
#include <qglobal.h>

#include "displacement/displacement.h"
#include "elements/femtypes.h"
#include "elements/global.h"
#include "elements/point.h"
#include "load/load.h"
#include "material.h"
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
  shared_ptr<Material> material{nullptr};

public:
  // Mesh
  std::shared_ptr<MeshData> meshData{nullptr};

  // Other
  QString name;

  // Elasticity matrix (elasticity matrix, since it will be the same for all
  // finite elements created on the basis of this historical element). Such
  // matrix could be 2 and that why its vector
  QVector<MatrixXd> elasticityMatrix;

  // Output maximum values for graphic
  QVector<double> maxAbsValues;
  QVector<double> maxValues;
  QVector<double> minValues;

  AbstractElement();

  AbstractElement(shared_ptr<AbstractLoad> load, ElementType type,
                  unsigned lenght);

  AbstractElement(shared_ptr<AbstractLoad> load, ElementType type,
                  unsigned lenght, Point3 startPoint);

  AbstractElement(shared_ptr<AbstractLoad> load, ElementType type,
                  unsigned lenght, Point3 startPoint,
                  shared_ptr<Material> material);

  static sptrAbsElem createByType(ElementType type);

  ElementType getType() const;

  Point3 getStartPoint() const;

  double getLenght() const;

  shared_ptr<Material> getMaterial() const;

  void addLoad(shared_ptr<AbstractLoad> load);

  inline short loadCount() const;

  virtual shared_ptr<AbstractLoad> createAndAddLoad() = 0;

  virtual void initElasticityMatrixies() = 0;

  // void setMeshData(shared_ptr<MeshData> meshData);
};
