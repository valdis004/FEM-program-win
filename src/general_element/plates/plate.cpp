#include <memory>
#include <unordered_map>

#include "element_provider.h"
// #include "fem_plate_mitc4my.h"
#include "fem_types.h"
#include "load/load.h"
#include "material.h"
#include "plate.h"
#include "point.h"

Plate::Plate(ElementType type, unsigned lenght, Point3 startPoint)
    : AbstractElement(type, lenght, startPoint) {}

Plate::Plate(ElementType type,
             unsigned lenght,
             Point3 startPoint,
             unique_ptr<Material> material,
             shared_ptr<AbstractLoad> load)
    : Plate(type, lenght, startPoint) {
  material_ = std::move(material);
  addLoad(load);

  elasticity_matrix_.push_back(
      ElementProvider.at(type).ELASTICITY_MATRIX_FN(getMaterial(), 0));
  elasticity_matrix_.push_back(
      ElementProvider.at(type).ELASTICITY_MATRIX_FN(getMaterial(), 1));
}

/* virtual */ shared_ptr<AbstractLoad> Plate::createAndAddLoad() {
  shared_ptr<AreaLoadFzMxMy> load = std::make_shared<AreaLoadFzMxMy>();
  addLoad(load);
  return load;
}

/* virtual */ void Plate::addMaterial(unique_ptr<Material> material) {
  material_ = std::move(material);

  elasticity_matrix_.push_back(
      ElementProvider.at(getType()).ELASTICITY_MATRIX_FN(getMaterial(), 0));
  elasticity_matrix_.push_back(
      ElementProvider.at(getType()).ELASTICITY_MATRIX_FN(getMaterial(), 1));
}

/* virtual */ void Plate::initElasticityMatrixies() {
  //
  // MITC4PlateMy::cMatrix(material, 0)
}
