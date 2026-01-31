#pragma once

#include <memory>

class Material {
public:
  virtual double *getMaterialProperties(double *physicalPropertiesArr) = 0;
};