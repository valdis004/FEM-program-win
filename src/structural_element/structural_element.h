#pragma once

#include <qcontainerfwd.h>
#include <qglobal.h>

#include <QVector>
#include <memory>

#include "fem_elements/fem_types.h"
#include "fem_elements/global.h"
#include "fem_elements/point.h"
#include "material.h"
#include "mesh/meshdata.h"
#include "structural_displacement/displacement.h"
#include "structural_load/load.h"

class Node;
class AFemElement;

using std::shared_ptr;
using std::unique_ptr;

static unsigned count_ = 0;

class AStructuralElement {
 private:
  // Geometry
  ElementType type_{ElementType::NONE};
  Point3 statrt_point_{0, 0, 0};
  unsigned lenght_{0};

  // Structural parameters
  QVector<shared_ptr<AStructuralLoad>> loads_;
  QVector<shared_ptr<AStructuralDisplacement>> displacements_;

 public:
  // Material
  unique_ptr<Material> material_{nullptr};

  // Mesh
  unique_ptr<MeshData> meshData_{nullptr};

  // Other
  QString name_;

  // Elasticity matrix (elasticity matrix, since it will be the same for all
  // finite elements created on the basis of this historical element). Such
  // matrix could be 2 and that why its vector
  QVector<MatrixXd> elasticity_matrix_;

  // Output maximum values for graphic
  QVector<double> max_abs_values_;
  QVector<double> min_values_;
  QVector<double> max_values_;

  AStructuralElement();

  AStructuralElement(ElementType type, unsigned lenght, Point3 start_point);

  AStructuralElement(ElementType type,
                     unsigned lenght,
                     Point3 start_point,
                     unique_ptr<Material> material,
                     shared_ptr<AStructuralLoad> load);

  static sptrAbsElem createByType(ElementType type);

  ElementType getType() const;

  Point3 getStartPoint() const;

  double getLenght() const;

  Material* getMaterial() const;

  void addLoad(shared_ptr<AStructuralLoad> load);

  QVector<shared_ptr<AStructuralLoad>>& getLoads();

  QVector<shared_ptr<AStructuralDisplacement>>& getDisplacements();

  inline short loadCount() const;

  virtual void addMaterial(unique_ptr<Material> material) = 0;

  virtual shared_ptr<AStructuralLoad> createAndAddLoad() = 0;

  virtual void initElasticityMatrixies() = 0;

  // void setMeshData(shared_ptr<MeshData> meshData);
};
