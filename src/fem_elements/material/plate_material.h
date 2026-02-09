#pragma once

#include "material.h"
#include <memory>

class PlateMaterial : public Material {
public:
  double youngModule{-1}, kFactor, poissonRatio, thickness;
  double Dplate = youngModule * thickness * thickness * thickness /
                  (12.0 * (1 - poissonRatio * poissonRatio));
  double shearModule = youngModule / (2.0 * (1 + poissonRatio));

  PlateMaterial(const PlateMaterial &) = default;
  PlateMaterial(PlateMaterial &&) = default;
  PlateMaterial &operator=(const PlateMaterial &) = default;
  PlateMaterial &operator=(PlateMaterial &&) = default;

  PlateMaterial(double youngModule, double kFactor, double poissonRatio,
                double thickness);
  static std::shared_ptr<Material> getDefaultMaterial();

  virtual double *getMaterialProperties(double *physicalPropertiesArr) override;

  // void setMaterialProperties(double *physicalPropertiesArr);
};