#include "PlateMaterial.h"
#include "material.h"

PlateMaterial::PlateMaterial(double youngModule, double kFactor,
                             double poissonRatio, double thickness)
    : youngModule(youngModule), kFactor(kFactor), poissonRatio(poissonRatio),
      thickness(thickness) {};

Material Material::getDefaultMaterial() {
  return PlateMaterial(10000, 0.833333, 0.25, 0.5);
};

double *Material::getMaterialProperties(double *physicalPropertiesArr) {
  //   physicalPropertiesArr[0] = youngModule;
  //   physicalPropertiesArr[1] = kFactor;
  //   physicalPropertiesArr[2] = poissonRatio;
  //   physicalPropertiesArr[3] = thickness;
  //   physicalPropertiesArr[4] = Dplate;
  //   physicalPropertiesArr[5] = shearModule;

  return physicalPropertiesArr;
};