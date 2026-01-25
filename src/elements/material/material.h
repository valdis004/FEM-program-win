#pragma once

class Material {
public:
  static Material getDefaultMaterial();

  virtual double *getMaterialProperties(double *physicalPropertiesArr);
};