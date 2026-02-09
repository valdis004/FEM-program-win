#pragma once

class Material {
 public:
  virtual double* getMaterialProperties(double* physicalPropertiesArr) = 0;

  virtual ~Material() = default;
};