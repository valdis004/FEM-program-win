#include <cstddef>
#include <exception>
#include <qdebug.h>
#include <qglobal.h>

#include "fem_plate_dkmq.h"
#include "generalElement/element.h"

using ShapeFunc = double (*)(double, double);
using DerivFunc = double (*)(double);

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

FemPlateDkmq::FemPlateDkmq(unsigned id, Node **nodes,
                           std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, 4, ElementType::DKMQ, generalElement) {};

FemPlateDkmq::FemPlateDkmq(unsigned id, Node **nodes, const Material &material,
                           std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, 4, material, ElementType::DKMQ,
                         generalElement) {};

MatrixXd FemPlateDkmq::jMatrix(double xi, double eta) {
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

/* static */ MatrixXd FemPlateDkmq::cMatrix(std::shared_ptr<Material> material,
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

MatrixXd FemPlateDkmq::bMatrix(double xi, double eta, int type) {
  MatrixXd J = jMatrix(xi, eta);
  MatrixXd Gm = J.transpose().inverse();

  double Gm11 = Gm(0, 0), Gm12 = Gm(0, 1), Gm21 = Gm(1, 0), Gm22 = Gm(1, 1);

  float N1_ξ = 0.25f * (-1) * (1 - eta);
  float N2_ξ = 0.25f * (1 - eta);
  float N3_ξ = 0.25f * (1 + eta);
  float N4_ξ = 0.25f * (-1) * (1 + eta);

  float N1_η = 0.25f * (1 - xi) * (-1);
  float N2_η = 0.25f * (1 + xi) * (-1);
  float N3_η = 0.25f * (1 + xi);
  float N4_η = 0.25f * (1 - xi);

  float N1x = Gm11 * N1_ξ + Gm12 * N1_η;
  float N2x = Gm11 * N2_ξ + Gm12 * N2_η;
  float N3x = Gm11 * N3_ξ + Gm12 * N3_η;
  float N4x = Gm11 * N4_ξ + Gm12 * N4_η;

  float N1y = Gm21 * N1_ξ + Gm22 * N1_η;
  float N2y = Gm21 * N2_ξ + Gm22 * N2_η;
  float N3y = Gm21 * N3_ξ + Gm22 * N3_η;
  float N4y = Gm21 * N4_ξ + Gm22 * N4_η;

  double Ndx[4];
  NDiffAny(Ndx, xi, eta, Gm11, Gm12);
  double Ndy[4];
  NDiffAny(Ndy, xi, eta, Gm21, Gm22);

  MatrixXd Nb = MatrixXd{{0, N1x, 0, 0, N2x, 0, 0, N3x, 0, 0, N4x, 0},
                         {0, 0, N1y, 0, 0, N2y, 0, 0, N3y, 0, 0, N4y},
                         {0, N1y, N1x, 0, N2y, N2x, 0, N3y, N3x, 0, N4y, N4x}};

  float P5_ξ = -((1 - eta) * xi);
  float P6_ξ = 0.5f * (1 - eta * eta);
  float P7_ξ = -((1 + eta) * xi);
  float P8_ξ = 0.5f * (-1 + eta * eta);

  float P5_η = 0.5f * (-1 + xi * xi);
  float P6_η = -eta * (1 + xi);
  float P7_η = 0.5f * (1 - xi * xi);
  float P8_η = -eta * (1 - xi);

  float P5x = Gm11 * P5_ξ + Gm12 * P5_η;
  float P6x = Gm11 * P6_ξ + Gm12 * P6_η;
  float P7x = Gm11 * P7_ξ + Gm12 * P7_η;
  float P8x = Gm11 * P8_ξ + Gm12 * P8_η;

  float P5y = Gm21 * P5_ξ + Gm22 * P5_η;
  float P6y = Gm21 * P6_ξ + Gm22 * P6_η;
  float P7y = Gm21 * P7_ξ + Gm22 * P7_η;
  float P8y = Gm21 * P8_ξ + Gm22 * P8_η;

  float Ck[] = {1.0f, 0, -1.0f, 0};
  float Sk[] = {0, 1.0f, 0, -1.0f};
  float step = genetalElement->meshData_->step / 1000.0;
  float l[] = {step, step, step, step};

  double physicalProperties[6];
  genetalElement->getMaterial()->getMaterialProperties(physicalProperties);
  double Em = physicalProperties[0];
  double K = physicalProperties[1];
  double nu = physicalProperties[2];
  double t = physicalProperties[3];
  double D = physicalProperties[4];
  double G = physicalProperties[5];

  float φ5 = (2.0f / (K * (1 - nu))) * (t / step) * (t / step);
  float φ6 = φ5, φ7 = φ5, φ8 = φ5;
  float L5 = l[0];
  float L6 = l[1];
  float L7 = l[2];
  float L8 = l[3];

  float C5 = Ck[0];
  float C6 = Ck[1];
  float C7 = Ck[2];
  float C8 = Ck[3];

  float S5 = Sk[0];
  float S6 = Sk[1];
  float S7 = Sk[2];
  float S8 = Sk[3];

  MatrixXd Gs = MatrixXd{{Gm11, Gm12}, {Gm21, Gm22}};

  MatrixXd As = MatrixXd{{L5 / 2, 0, 0, 0},
                         {0, L6 / 2, 0, 0},
                         {0, 0, -L7 / 2, 0},
                         {0, 0, 0, -L8 / 2}};

  MatrixXd Ns = MatrixXd{
      {(1 - eta) / 2, 0, (1 + eta) / 2, 0},
      {0, (1 + xi) / 2, 0, (1 - xi) / 2},
  };

  MatrixXd ADeltaDKQM = MatrixXd{{1 + φ5, 0, 0, 0},
                                 {0, 1 + φ6, 0, 0},
                                 {0, 0, 1 + φ7, 0},
                                 {0, 0, 0, 1 + φ8}};

  MatrixXd APhi =
      MatrixXd{{φ5, 0, 0, 0}, {0, φ6, 0, 0}, {0, 0, φ7, 0}, {0, 0, 0, φ8}};

  MatrixXd Ad = MatrixXd{{2 / L5, C5, S5, -2 / L5, C5, S5, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 2 / L6, C6, S6, -2 / L6, C6, S6, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 2 / L7, C7, S7, -2 / L7, C7, S7},
                         {-2 / L8, C8, S8, 0, 0, 0, 0, 0, 0, 2 / L8, C8, S8}};

  MatrixXd BbDeltaPsi = MatrixXd{
      {P5x * C5, P6x * C6, P7x * C7, P8x * C8},
      {P5y * S5, P6y * S6, P7y * S7, P8y * S8},
      {P5x * S5 + P5y * C5, P6x * S6 + P6y * C6, P7x * S7 + P7y * C7,
       P8x * S8 + P8y * C8},
  };

  MatrixXd Bb = Nb + BbDeltaPsi * ADeltaDKQM.inverse() * Ad;
  MatrixXd Bs = Gs * Ns * As * APhi * ADeltaDKQM.inverse() * Ad;

  if (type == 0) {
    return Bb;
  } else {
    return Bs;
  }
}

MatrixXd FemPlateDkmq::integrateingFn(double xi, double eta, int type) {
  double detJ = jMatrix(xi, eta).determinant();

  MatrixXd Bb = bMatrix(xi, eta, 0);
  MatrixXd Bs = bMatrix(xi, eta, 1);
  MatrixXd Cb = genetalElement->elasticity_matrix_[0];
  MatrixXd Cs = genetalElement->elasticity_matrix_[1];

  if (type == 0) {
    return Bb.transpose() * Cb * Bb * detJ;
  } else {
    return Bs.transpose() * Cs * Bs * detJ;
  }
}

MatrixXd FemPlateDkmq::getLocalStiffMatrix() {
  MatrixXd kElem = integrateingFn(DATA.XI_SET[0], DATA.ETA_SET[0], 0) +
                   integrateingFn(DATA.XI_SET[0], DATA.ETA_SET[1], 0) +
                   integrateingFn(DATA.XI_SET[1], DATA.ETA_SET[0], 0) +
                   integrateingFn(DATA.XI_SET[1], DATA.ETA_SET[1], 0) +
                   integrateingFn(DATA.XI_SET[0], DATA.ETA_SET[0], 1) +
                   integrateingFn(DATA.XI_SET[0], DATA.ETA_SET[1], 1) +
                   integrateingFn(DATA.XI_SET[1], DATA.ETA_SET[0], 1) +
                   integrateingFn(DATA.XI_SET[1], DATA.ETA_SET[1], 1);

  return kElem;
}

VectorXd FemPlateDkmq::getLoadVector() {
  VectorXd loadCoefVector = Eigen::VectorXd::Zero(12);

  auto N = [](double xi, double eta) {
    return VectorXd{{N1(xi, eta), 0, 0, N2(xi, eta), 0, 0, N3(xi, eta), 0, 0,
                     N4(xi, eta), 0, 0}};
  };

  for (size_t i = 0; i < 2; i++) {
    for (size_t j = 0; j < 2; j++) {

      loadCoefVector += N(DATA.XI_SET[i], DATA.ETA_SET[j]) *
                        jMatrix(DATA.XI_SET[i], DATA.ETA_SET[j]).determinant() *
                        DATA.W_COEFS[i * 2 + j] * DATA.W_COEFS[i * 2 + j];
    }
  }

  // for (size_t k = 0; k < 12; k++) {
  //   qDebug() << loadCoefVector(k) << " ";
  // }

  return loadCoefVector;
};

QVector<double> FemPlateDkmq::getResultVector(VectorXd U, double xi,
                                              double eta) {

  double uz = N1(xi, eta) * U[0] + N2(xi, eta) * U[3] + N3(xi, eta) * U[6] +
              N4(xi, eta) * U[9];
  double psix = (N1(xi, eta) * U[2] + N2(xi, eta) * U[5] + N3(xi, eta) * U[8] +
                 N4(xi, eta) * U[11]);

  double psiy = (N1(xi, eta) * U[1] + N2(xi, eta) * U[4] + N3(xi, eta) * U[7] +
                 N4(xi, eta) * U[10]);

  MatrixXd Bb = bMatrix(xi, eta, 0);
  MatrixXd Bs = bMatrix(xi, eta, 1);

  MatrixXd Cb = -genetalElement->elasticity_matrix_[0];
  MatrixXd Cs = genetalElement->elasticity_matrix_[1];

  VectorXd M = Cb * Bb * U;
  VectorXd Q = Cs * Bs * U;

  return QVector<double>{{uz, psix, psiy, Q[0], Q[1], M[0], M[1], M[2]}};
};
