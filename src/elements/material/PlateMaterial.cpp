#include "PlateMaterial.h"
#include "material.h"
#include <memory>
#include <stdexcept>

PlateMaterial::PlateMaterial(double youngModule, double kFactor,
                             double poissonRatio, double thickness)
    : youngModule(youngModule), kFactor(kFactor), poissonRatio(poissonRatio),
      thickness(thickness) {};

/* static */ std::shared_ptr<Material> PlateMaterial::getDefaultMaterial() {
  return std::make_shared<PlateMaterial>(3e7, 0.833333, 0.2, 4.0);
};

double *PlateMaterial::getMaterialProperties(double *physicalPropertiesArr) {
  if (youngModule == -1)
    throw std::runtime_error(
        "Trying to get values by material that has not initialized properties");

  physicalPropertiesArr[0] = youngModule;
  physicalPropertiesArr[1] = kFactor;
  physicalPropertiesArr[2] = poissonRatio;
  physicalPropertiesArr[3] = thickness;
  physicalPropertiesArr[4] = Dplate;
  physicalPropertiesArr[5] = shearModule;

  return physicalPropertiesArr;
};

// void PlateMaterial::setMaterialProperties(double *physicalPropertiesArr) {}