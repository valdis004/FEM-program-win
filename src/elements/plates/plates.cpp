#include <cstddef>
#include <exception>
#include <qdebug.h>
#include <qglobal.h>

#include "generalELement/element.h"
#include "plates.h"

using ShapeFunc = double (*)(double, double);
using DerivFunc = double (*)(double);

auto &data = ElementProvider::elementData[ElementType::MITC4MY];

constexpr static auto N1 = [](double xi, double eta) {
  return 0.25f * (1 - eta) * (1 - xi);
};
constexpr static auto N2 = [](double xi, double eta) -> double {
  return 0.25 * (1 - eta) * (1 + xi);
};

constexpr static auto N3 = [](double xi, double eta) -> double {
  return 0.25 * (1 + eta) * (1 + xi);
};

constexpr static auto N4 = [](double xi, double eta) -> double {
  return 0.25 * (1 + eta) * (1 - xi);
};

// Массив функций формы
constexpr static ShapeFunc N[] = {N1, N2, N3, N4};

// Производные по xi (зависят только от eta)
constexpr static auto N1dxi = [](double eta) -> double {
  return 0.25 * (-1) * (1 - eta);
};

constexpr static auto N2dxi = [](double eta) -> double {
  return 0.25 * (1 - eta);
};

constexpr static auto N3dxi = [](double eta) -> double {
  return 0.25 * (1 + eta);
};

constexpr static auto N4dxi = [](double eta) -> double {
  return 0.25 * (-1) * (1 + eta);
};

constexpr static DerivFunc Ndxi[] = {N1dxi, N2dxi, N3dxi, N4dxi};

// Производные по eta (зависят только от xi)
constexpr static auto N1deta = [](double xi) -> double {
  return 0.25 * (1 - xi) * (-1);
};

constexpr static auto N2deta = [](double xi) -> double {
  return 0.25 * (1 + xi) * (-1);
};

constexpr static auto N3deta = [](double xi) -> double {
  return 0.25 * (1 + xi);
};

constexpr static auto N4deta = [](double xi) -> double {
  return 0.25 * (1 - xi);
};

constexpr static DerivFunc Ndeta[] = {N1deta, N2deta, N3deta, N4deta};

constexpr static auto Map = [](double xi, double eta, int type,
                               const double xSet[],
                               const double ySet[]) -> double {
  double value = 0.0;
  if (type == 0) {
    for (int i = 0; i < 4; i++) {
      value += N[i](xi, eta) * xSet[i];
    }
  } else {
    for (int i = 0; i < 4; i++) {
      value += N[i](xi, eta) * ySet[i];
    }
  }
  return value;
};

constexpr static auto NDiffAny = [](double *arr, double xi, double eta,
                                    double gm1, double gm2) {
  for (int i = 0; i < 4; i++) {
    arr[i] = Ndxi[i](eta) * gm1 + Ndeta[i](xi) * gm2;
  }
  return arr;
};

MITC4PlateMy::MITC4PlateMy(unsigned id, Node **nodes,
                           std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, 4, ElementType::MITC4MY, generalElement) {};

MITC4PlateMy::MITC4PlateMy(unsigned id, Node **nodes, const Material &material,
                           std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, 4, material, ElementType::MITC4MY,
                         generalElement) {};

MatrixXd MITC4PlateMy::jMatrix(double xi, double eta) {
  double y1 = nodes[0]->point.y / 1000.0;
  double y2 = nodes[1]->point.y / 1000.0;
  double y3 = nodes[2]->point.y / 1000.0;
  double y4 = nodes[3]->point.y / 1000.0;
  double x1 = nodes[0]->point.x / 1000.0;
  double x2 = nodes[1]->point.x / 1000.0;
  double x3 = nodes[2]->point.x / 1000.0;
  double x4 = nodes[3]->point.x / 1000.0;

  double j11 = (-1.0 / 4.0) * (1 - eta) * x1 + (1.0 / 4.0) * (1 - eta) * x2 +
               (1.0 / 4.0) * (1 + eta) * x3 - (1.0 / 4.0) * (1 + eta) * x4;

  double j12 = (-1.0 / 4.0) * x1 * (1 - xi) + (1.0 / 4.0) * x4 * (1 - xi) -
               (1.0 / 4.0) * x2 * (1 + xi) + (1.0 / 4.0) * x3 * (1 + xi);

  double j21 = (-1.0 / 4.0) * (1 - eta) * y1 + (1.0 / 4.0) * (1 - eta) * y2 +
               (1.0 / 4.0) * (1 + eta) * y3 - (1.0 / 4.0) * (1 + eta) * y4;

  double j22 = (-1.0 / 4.0) * (1 - xi) * y1 - (1.0 / 4.0) * (1 + xi) * y2 +
               (1.0 / 4.0) * (1 + xi) * y3 + (1.0 / 4.0) * (1 - xi) * y4;

  return MatrixXd{
      {j11, j12},
      {j21, j22},
  };
}

/* static  */ MatrixXd MITC4PlateMy::cMatrix(std::shared_ptr<Material> material,
                                             int type) {

  double physicalProperties[6];
  material->getMaterialProperties(physicalProperties);
  double Em = physicalProperties[0];
  double K = physicalProperties[1];
  double nu = physicalProperties[2];
  double t = physicalProperties[3];
  double D = physicalProperties[4];
  double G = physicalProperties[5];

  MatrixXd Cb = MatrixXd(3, 3);
  MatrixXd Cs = MatrixXd(2, 2);

  Cb(0, 0) = D;
  Cb(0, 1) = D * nu;
  Cb(1, 0) = D * nu;
  Cb(1, 1) = D;
  Cb(2, 2) = (1 - nu) * D / 2;

  Cs(0, 0) = K * G * t;
  Cs(1, 1) = K * G * t;

  switch (type) {
  case 0:
    return Cb;
  case 1:
    return Cs;
  default:
    throw std::exception();
  }
}

MatrixXd MITC4PlateMy::bMatrix(double xi, double eta, int type) {
  MatrixXd J = jMatrix(xi, eta);
  MatrixXd Gm = J.transpose().inverse();

  double Gm11 = Gm(0, 0), Gm12 = Gm(0, 1), Gm21 = Gm(1, 0), Gm22 = Gm(1, 1);

  double Ndx[4];
  NDiffAny(Ndx, xi, eta, Gm11, Gm12);
  double Ndy[4];
  NDiffAny(Ndy, xi, eta, Gm21, Gm22);

  // var Nbm = Matrix<double>.Build.DenseOfArray(new double(,)
  //{
  //    { 0, Ndx(0), 0, 0, Ndx(1), 0, 0, Ndx(2), 0, 0, Ndx(3), 0 },
  //    { 0, 0, Ndy(0), 0, 0, Ndy(1), 0, 0, Ndy(2), 0, 0, Ndy(3) },
  //    { 0, Ndy(0), Ndx(0), 0, Ndy(1), Ndx(1), 0, Ndy(2), Ndx(2), 0, Ndy(3),
  //    Ndx(3) }
  // });

  MatrixXd Nbm =
      MatrixXd{{0, 0, -Ndx[0], 0, 0, -Ndx[1], 0, 0, -Ndx[2], 0, 0, -Ndx[3]},
               {0, Ndy[0], 0, 0, Ndy[1], 0, 0, Ndy[2], 0, 0, Ndy[3], 0},
               {0, Ndx[0], -Ndy[0], 0, Ndx[1], -Ndy[1], 0, Ndx[2], -Ndy[2], 0,
                Ndx[3], -Ndy[3]}};

  double xCoords[4];
  double yCoords[4];
  for (int i = 0; i < 4; i++) {
    xCoords[i] = nodes[i]->point.x / 1000.0;
    yCoords[i] = nodes[i]->point.y / 1000.0;
  }

  MatrixXd Ngm = MatrixXd{
      {1, eta, 0, 0},
      {0, 0, 1, xi},
  };

  MatrixXd Mgm = MatrixXd{
      {1.0, -1.0, 0, 0},
      {1.0, 1.0, 0, 0},
      {0, 0, 1.0, 1.0},
      {0, 0, 1.0, -1.0},
  };

  double NdxA[4];
  NDiffAny(NdxA, 0, -1, Gm11, Gm12);
  double NdxC[4];
  NDiffAny(NdxC, 0, 1, Gm11, Gm12);
  double NdyB[4];
  NDiffAny(NdyB, 1, 0, Gm21, Gm22);
  double NdyD[4];
  NDiffAny(NdyD, -1, 0, Gm21, Gm22);

  MatrixXd Nd = MatrixXd{
      {NdxA[0], 0, N[0](0, -1), NdxA[1], 0, N[1](0, -1), NdxA[2], 0,
       N[2](0, -1), NdxA[3], 0, N[3](0, -1)},
      {NdxC[0], 0, N[0](0, 1), NdxC[1], 0, N[1](0, 1), NdxC[2], 0, N[2](0, 1),
       NdxC[3], 0, N[3](0, 1)},
      {NdyB[0], -N[0](1, 0), 0, NdyB[1], -N[1](1, 0), 0, NdyB[2], -N[2](1, 0),
       0, NdyB[3], -N[3](1, 0), 0},
      {NdyD[0], -N[0](-1, 0), 0, NdyD[1], -N[1](-1, 0), 0, NdyD[2],
       -N[2](-1, 0), 0, NdyD[3], -N[3](-1, 0), 0},
  };

  MatrixXd Bb = Nbm;
  MatrixXd Bs = Ngm * Mgm.inverse() * Nd;

  if (type == 0) {
    return Bb;
  } else {
    return Bs;
  }
}

MatrixXd MITC4PlateMy::integrateingFn(double xi, double eta, int type) {
  double detJ = jMatrix(xi, eta).determinant();

  MatrixXd Bb = bMatrix(xi, eta, 0);
  MatrixXd Bs = bMatrix(xi, eta, 1);
  MatrixXd Cb = genetalElement->elasticityMatrix[0];
  MatrixXd Cs = genetalElement->elasticityMatrix[1];

  if (type == 0) {
    return Bb.transpose() * Cb * Bb * detJ;
  } else {
    return Bs.transpose() * Cs * Bs * detJ;
  }
}

MatrixXd MITC4PlateMy::getLocalStiffMatrix() {
  MatrixXd kElem = integrateingFn(data.XI_SET[0], data.ETA_SET[0], 0) +
                   integrateingFn(data.XI_SET[0], data.ETA_SET[1], 0) +
                   integrateingFn(data.XI_SET[1], data.ETA_SET[0], 0) +
                   integrateingFn(data.XI_SET[1], data.ETA_SET[1], 0) +
                   integrateingFn(data.XI_SET[0], data.ETA_SET[0], 1) +
                   integrateingFn(data.XI_SET[0], data.ETA_SET[1], 1) +
                   integrateingFn(data.XI_SET[1], data.ETA_SET[0], 1) +
                   integrateingFn(data.XI_SET[1], data.ETA_SET[1], 1);

  return kElem;
}

VectorXd MITC4PlateMy::getLoadVector() {
  VectorXd loadCoefVector = Eigen::VectorXd::Zero(12);

  auto N = [](double xi, double eta) {
    return VectorXd{{N1(xi, eta), 0, 0, N2(xi, eta), 0, 0, N3(xi, eta), 0, 0,
                     N4(xi, eta), 0, 0}};
  };

  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < 2; j++) {

      loadCoefVector += N(data.XI_SET[i], data.ETA_SET[j]) *
                        jMatrix(data.XI_SET[i], data.ETA_SET[j]).determinant() *
                        data.W_COEFS[i * 2 + j] * data.W_COEFS[i * 2 + j];
    }
  }

  // for (size_t k = 0; k < 12; k++) {
  //   qDebug() << loadCoefVector(k) << " ";
  // }

  return loadCoefVector;
};

QVector<double> MITC4PlateMy::getResultVector(VectorXd U, double xi,
                                              double eta) {

  double uz = N1(xi, eta) * U[0] + N2(xi, eta) * U[3] + N3(xi, eta) * U[6] +
              N4(xi, eta) * U[9];
  double psix = (N1(xi, eta) * U[2] + N2(xi, eta) * U[5] + N3(xi, eta) * U[8] +
                 N4(xi, eta) * U[11]);

  double psiy = (N1(xi, eta) * U[1] + N2(xi, eta) * U[4] + N3(xi, eta) * U[7] +
                 N4(xi, eta) * U[10]);

  MatrixXd Bb = bMatrix(xi, eta, 0);
  MatrixXd Bs = bMatrix(xi, eta, 1);

  MatrixXd Cb = -genetalElement->elasticityMatrix[0];
  MatrixXd Cs = genetalElement->elasticityMatrix[1];

  VectorXd M = Cb * Bb * U;
  VectorXd Q = Cs * Bs * U;

  return QVector<double>{{uz, psix, psiy, Q[0], Q[1], M[0], M[1], M[2]}};
};
