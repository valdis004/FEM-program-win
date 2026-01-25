#pragma once

#include "material.h"

class PlateMaterial : public Material {
public:
  double youngModule, kFactor, poissonRatio, thickness;
  double Dplate = youngModule * thickness * thickness * thickness /
                  (12 * (1 - poissonRatio * poissonRatio));
  double shearModule = youngModule / (2 * (1 + poissonRatio));

  PlateMaterial(const PlateMaterial &) = default;
  PlateMaterial(PlateMaterial &&) = default;
  PlateMaterial &operator=(const PlateMaterial &) = default;
  PlateMaterial &operator=(PlateMaterial &&) = default;

  PlateMaterial(double youngModule, double kFactor, double poissonRatio,
                double thickness);
  static Material getDefaultMaterial();

  virtual double *getMaterialProperties(double *physicalPropertiesArr) override;
};