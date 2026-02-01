#include <memory>
#include <unordered_map>

#include "elementprovider.h"
// #include "fem_plate_mitc4my.h"
#include "femtypes.h"
#include "load/load.h"
#include "material.h"
#include "plate.h"
#include "point.h"

Plate::Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght)
    : AbstractElement(load, type, lenght) {}

Plate::Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght,
             Point3 startPoint)
    : AbstractElement(load, type, lenght, startPoint) {}

Plate::Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght,
             Point3 startPoint, shared_ptr<Material> material)
    : AbstractElement(load, type, lenght, startPoint, material) {

  elasticity_matrix_.push_back(
      ElementProvider.at(type).ELASTICITY_MATRIX_FN(material, 0));
  elasticity_matrix_.push_back(
      ElementProvider.at(type).ELASTICITY_MATRIX_FN(material, 1));
}

/* virtual */ shared_ptr<AbstractLoad> Plate::createAndAddLoad() {
  shared_ptr<AreaLoadFzMxMy> load = std::make_shared<AreaLoadFzMxMy>();
  addLoad(load);
  return load;
}

/* virtual */ void Plate::initElasticityMatrixies() {
  //
  // MITC4PlateMy::cMatrix(material, 0)
}
