#pragma once

#include <cstdlib>
static float delta = 0.01;

struct Point3 {
  float x, y, z;

  bool operator==(Point3& right) {
    return std::abs(x - right.x) < delta && std::abs(y - right.y) < delta &&
           std::abs(z - right.z) < delta;
  }
};