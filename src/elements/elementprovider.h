#pragma once

// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementdata.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/element.h"
#include <Eigen/Dense>
#include <QMap>
#include <QVector>
#include <span>

#include "displacement/femdisplacement.h"
#include "elements/load/femload.h"
#include "elements/node.h"
#include "elements/point.h"
#include "femtypes.h"
#include "generalElement/displacement/displacement.h"
#include "generalElement/load/load.h"
#include "material.h"
// #include "plates.h"

class Material;
using Eigen::MatrixXd;

#define SQRT_3 1.7320508075688772

#define C_S_C const static constexpr

using MethodCMatrix = MatrixXd (*)(std::shared_ptr<Material> material,
                                   int type);
using MethodPtrDisp = NodeDisplacement *(*)(bool *value, Node *node);
using MethodPtrLoad = NodeLoad *(*)(double *values, double *coefs);
using GetPointFunc = Point3 (*)(int index, Point3 &point0, double step,
                                double cosA, double sinA);

struct ElementData {
  short MAIN_NODES_COUNT;
  std::span<const double> MAIN_NODES_XI_SET; // For qtgl quad draw
  std::span<const double> MAIN_NODES_ETA_SET;
  short NODES_COUNT;
  short INT_POINTS_COUNT;
  short STIFF_MATRIX_SIZE;
  bool IS_FULL_DOF;
  short FULL_DOF_COUNT; // Count of usual dof count in node
  short BAD_DOF_BEGIN;  // DELETE, its actually NODES_COUNT
  short BAD_DOF_COUNT;  // Количество степеней свободы, которое надо добавить //
                        // чтобы получить fullDoff
  const short OUTPUT_VALUES_COUNT;
  std::span<const double> XI_SET;
  std::span<const double> ETA_SET;
  std::span<const double> W_COEFS;
  std::span<const short> DOF_MAP;
  std::span<const short> LOCAL_ID_FROM_STIFFMAT;
  std::span<const bool> IS_NODE_BAD_DOF_MAP;
  std::span<const short> BAD_DOF_MAP;
  std::span<const LoadType> LOAD_MAP;
  std::span<const short> OUTPUT_INDEX_MAP;
  std::span<const char *const> STR_OUTPUT_VALUES;

  std::span<const MethodPtrDisp> DISP_FN_MAP;

  std::span<const MethodPtrLoad> LOAD_FN_MAP;

  GetPointFunc GET_POINT_FROM_INDEX_FN;

  MethodCMatrix ELASTICITY_MATRIX_FN;
};

const extern std::unordered_map<ElementType, ElementData> ElementProvider;