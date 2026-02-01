#pragma once

#include <QVector>
#include <memory>
#include <qcontainerfwd.h>

#include "elements/femelement.h"
// #include "elementprovider.h"
#include "elementprovider.h"
#include "elements/node.h"
#include "material.h"

class FemPlateMitc4My : public FemAbstractElement {
private:
  MatrixXd jMatrix(double xi, double eta);

  MatrixXd integrateingFn(double xi, double eta, int type);

  MatrixXd bMatrix(double xi, double eta, int type);

public:
  FemPlateMitc4My(unsigned id, Node **nodes,
                  std::shared_ptr<AbstractElement> generalElement);

  FemPlateMitc4My(unsigned id, Node **nodes, const Material &material,
                  std::shared_ptr<AbstractElement> generalElement);

  virtual MatrixXd getLocalStiffMatrix() override;

  virtual VectorXd getLoadVector() override;

  virtual QVector<double> getResultVector(VectorXd U, double xi,
                                          double eta) override;

  static MatrixXd cMatrix(std::shared_ptr<Material> material, int type);

#pragma region CONST_PARAMS

  C_S_C double MAIN_NODES_XI_SET[]{-1, 1, 1, -1};
  C_S_C double MAIN_NODES_ETA_SET[]{-1, -1, 1, 1};

  C_S_C double XI_SET[]{-1.0 / SQRT_3, 1.0 / SQRT_3};
  C_S_C double ETA_SET[]{-1.0 / SQRT_3, 1.0 / SQRT_3};
  C_S_C double W_COEFS[]{1.0, 1.0, 1.0, 1.0};
  C_S_C short DOF_MAP[]{3, 3, 3, 3};
  C_S_C short LOCAL_ID_FROM_STIFFMAT[]{0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3};
  C_S_C short BAD_DOF_MAP[]{-1};
  C_S_C LoadType LOAD_MAP[]{LoadType::FyMxMz, LoadType::FyMxMz,
                            LoadType::FyMxMz, LoadType::FyMxMz};
  C_S_C short OUTPUT_INDEX_MAP[]{-1, -1, 0, 1, 2, -1, -1,
                                 -1, -1, 3, 4, 5, 6,  7};

  C_S_C char *STR_OUTPUT_VALUES[] = {"U_z", "R_x", "R_y", "Q_x",
                                     "Q_y", "M_x", "M_y", "M_xy"};
  C_S_C MethodPtrDisp DISP_FN_MAP[]{
      &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementUzPsixPsiy::create,
      &NodeDisplacementUzPsixPsiy::create,
  };
  C_S_C MethodPtrLoad LOAD_FN_MAP[]{
      &NodeLoadFzMxMy::create,
      &NodeLoadFzMxMy::create,
      &NodeLoadFzMxMy::create,
      &NodeLoadFzMxMy::create,
  };

  C_S_C ElementData DATA{
      4,
      MAIN_NODES_XI_SET,
      MAIN_NODES_ETA_SET,
      4,
      4,
      12,
      true,
      3,
      -1,
      -1,
      8,
      XI_SET,
      ETA_SET,
      W_COEFS,
      DOF_MAP,
      LOCAL_ID_FROM_STIFFMAT,
      BAD_DOF_MAP,
      LOAD_MAP,
      OUTPUT_INDEX_MAP,
      STR_OUTPUT_VALUES,

      DISP_FN_MAP,

      LOAD_FN_MAP,

      [](int index, Point3 &point0, double step, double cosA, double sinA) {
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
        default:
          throw std::runtime_error("Invalid index");
        }
      },
      FemPlateMitc4My::cMatrix,
  };

#pragma endregion CONST_PARAMS
};