#pragma once

#include <QVector>
// #include <cmath>
#include <qcontainerfwd.h>

#include <memory>
#include <utility>

#include "fem_elements/fem_element.h"

// #include "elementprovider.h"
#include "element_provider.h"
#include "fem_displacement.h"
#include "fem_elements/node.h"
#include "fem_load.h"
#include "material.h"

class FemPlateMitc9My : public FemAbstractElement {
 private:
  MatrixXd jMatrix(double xi, double eta);

  inline std::pair<MatrixXd, MatrixXd> integratingFn(double xi, double eta);

  std::pair<MatrixXd, MatrixXd> bMatrix(double xi, double eta);

 public:
  FemPlateMitc9My(unsigned id,
                  Node** nodes,
                  std::shared_ptr<AbstractElement> generalElement);

  FemPlateMitc9My(unsigned id,
                  Node** nodes,
                  const Material& material,
                  std::shared_ptr<AbstractElement> generalElement);

  virtual MatrixXd getLocalStiffMatrix() override;

  inline virtual VectorXd getLoadVector() override;

  virtual QVector<double> getResultVector(VectorXd U,
                                          double xi,
                                          double eta) override;

  static MatrixXd cMatrix(Material* material, int type);

#pragma region CONST_PARAMS

#define SQRT_0_6 0.7745966692414834

  C_S_C double MAIN_NODES_XI_SET[]{-1, 1, 1, -1};
  C_S_C double MAIN_NODES_ETA_SET[]{-1, -1, 1, 1};

  C_S_C double XI_SET[]{-SQRT_0_6, 0, SQRT_0_6};
  C_S_C double ETA_SET[]{-SQRT_0_6, 0, SQRT_0_6};
  C_S_C double W_COEFS[]{5.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0};
  C_S_C short DOF_MAP[]{3, 3, 3, 3, 3, 3, 3, 3, 2};
  C_S_C short LOCAL_ID_FROM_STIFFMAT[]{0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4,
                                       4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8};
  C_S_C bool IS_NODE_BAD_DOF_MAP[]{false, false, false, false, false,
                                   false, false, false, true};
  C_S_C short BAD_DOF_MAP[]{0, 0, 0, 0, 0, 0, 0, 0, 0};
  C_S_C LoadType LOAD_MAP[]{
      LoadType::FyMxMz, LoadType::FyMxMz, LoadType::FyMxMz,
      LoadType::FyMxMz, LoadType::FyMxMz, LoadType::FyMxMz,
      LoadType::FyMxMz, LoadType::FyMxMz, LoadType::MxMz};
  C_S_C short OUTPUT_INDEX_MAP[]{-1, -1, 0, 1, 2, -1, -1,
                                 -1, -1, 3, 4, 5, 6,  7};

  C_S_C char* STR_OUTPUT_VALUES[] = {"U_z", "R_x", "R_y", "Q_x",
                                     "Q_y", "M_x", "M_y", "M_xy"};
  C_S_C MethodPtrDisp DISP_FN_MAP[]{
      &NodeDisplacementUzPsixPsiy::create, &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementUzPsixPsiy::create, &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementUzPsixPsiy::create, &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementUzPsixPsiy::create, &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementPsixPsiy::create,
  };
  C_S_C MethodPtrLoad LOAD_FN_MAP[]{
      &NodeLoadFzMxMy::create, &NodeLoadFzMxMy::create, &NodeLoadFzMxMy::create,
      &NodeLoadFzMxMy::create, &NodeLoadFzMxMy::create, &NodeLoadFzMxMy::create,
      &NodeLoadFzMxMy::create, &NodeLoadFzMxMy::create, &NodeLoadMxMy::create};

  C_S_C ElementData DATA{
      4,
      MAIN_NODES_XI_SET,
      MAIN_NODES_ETA_SET,
      9,
      9,
      26,
      false,
      3,
      9,
      1,
      8,
      XI_SET,
      ETA_SET,
      W_COEFS,
      DOF_MAP,
      LOCAL_ID_FROM_STIFFMAT,
      IS_NODE_BAD_DOF_MAP,
      BAD_DOF_MAP,
      LOAD_MAP,
      OUTPUT_INDEX_MAP,
      STR_OUTPUT_VALUES,

      DISP_FN_MAP,

      LOAD_FN_MAP,

      [](int index, Point3& point0, double step, double cosA, double sinA) {
        float x1 = point0.x;
        float y1 = point0.y;
        float z1 = point0.z;

        switch (index) {
          case 0:
            return Point3(x1, y1, z1);
          case 1:
            return Point3(x1 + step * cosA, y1, z1 + step * sinA);
          case 2:
            return Point3(x1 + step * cosA, y1 + step, z1 + step * sinA);
          case 3:
            return Point3(x1, y1 + step, z1);
          case 4:
            return Point3((x1 + step * cosA - x1) / 2 + x1, y1,
                          (z1 + step * sinA - z1) / 2 + z1);
          case 5:
            return Point3(x1 + step * cosA, (y1 + step - y1) / 2 + y1,
                          z1 + step * sinA);
          case 6:
            return Point3((x1 + step * cosA - x1) / 2 + x1, y1 + step,
                          (z1 + step * sinA - z1) / 2 + z1);
          case 7:
            return Point3(x1, (y1 + step - y1) / 2 + y1, z1);
          case 8:
            return Point3((x1 + step * cosA - x1) / 2 + x1,
                          (y1 + step - y1) / 2 + y1,
                          (z1 + step * sinA - z1) / 2 + z1);
          default:
            throw std::runtime_error("Invalid index");
        }
      },
      FemPlateMitc9My::cMatrix,
  };

#pragma endregion CONST_PARAMS
};