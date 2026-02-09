#include "fem_plate_dkmq.h"

#include <qdebug.h>
#include <qglobal.h>

#include <Eigen/Core>
#include <cstddef>
#include <exception>

#include "general_element/element.h"

using ShapeFunc = double (*)(double, double);
using DerivFunc = double (*)(double);

constexpr static auto N1 = [](double xi, double eta) {
  return 0.25 * (1 - eta) * (1 - xi);
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

constexpr static auto Map = [](double xi,
                               double eta,
                               int type,
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

constexpr static auto NDiffAny =
    [](double* arr, double xi, double eta, double gm1, double gm2) {
      for (int i = 0; i < 4; i++) {
        arr[i] = Ndxi[i](eta) * gm1 + Ndeta[i](xi) * gm2;
      }
      return arr;
    };

FemPlateDkmq::FemPlateDkmq(unsigned id,
                           Node** nodes,
                           std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(id, nodes, 4, ElementType::DKMQ, generalElement) {};

FemPlateDkmq::FemPlateDkmq(unsigned id,
                           Node** nodes,
                           const Material& material,
                           std::shared_ptr<AbstractElement> generalElement)
    : FemAbstractElement(
          id, nodes, 4, material, ElementType::DKMQ, generalElement) {};

MatrixXd FemPlateDkmq::jMatrix(double xi, double eta) {
  double y1 = nodes_[0]->point.y / 1000.0;
  double y2 = nodes_[1]->point.y / 1000.0;
  double y3 = nodes_[2]->point.y / 1000.0;
  double y4 = nodes_[3]->point.y / 1000.0;
  double x1 = nodes_[0]->point.x / 1000.0;
  double x2 = nodes_[1]->point.x / 1000.0;
  double x3 = nodes_[2]->point.x / 1000.0;
  double x4 = nodes_[3]->point.x / 1000.0;

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

/* static */ MatrixXd FemPlateDkmq::cMatrix(Material* material, int type) {
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
  Cb(2, 2) = (1 - nu) * D / 2.0;

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

  double N1_ξ = 0.25 * (-1) * (1 - eta);
  double N2_ξ = 0.25 * (1 - eta);
  double N3_ξ = 0.25 * (1 + eta);
  double N4_ξ = 0.25 * (-1) * (1 + eta);

  double N1_η = 0.25 * (1 - xi) * (-1);
  double N2_η = 0.25 * (1 + xi) * (-1);
  double N3_η = 0.25 * (1 + xi);
  double N4_η = 0.25 * (1 - xi);

  double N1x = Gm11 * N1_ξ + Gm12 * N1_η;
  double N2x = Gm11 * N2_ξ + Gm12 * N2_η;
  double N3x = Gm11 * N3_ξ + Gm12 * N3_η;
  double N4x = Gm11 * N4_ξ + Gm12 * N4_η;

  double N1y = Gm21 * N1_ξ + Gm22 * N1_η;
  double N2y = Gm21 * N2_ξ + Gm22 * N2_η;
  double N3y = Gm21 * N3_ξ + Gm22 * N3_η;
  double N4y = Gm21 * N4_ξ + Gm22 * N4_η;

  double Ndx[4];
  NDiffAny(Ndx, xi, eta, Gm11, Gm12);
  double Ndy[4];
  NDiffAny(Ndy, xi, eta, Gm21, Gm22);

  MatrixXd Nb = MatrixXd{{0, N1x, 0, 0, N2x, 0, 0, N3x, 0, 0, N4x, 0},
                         {0, 0, N1y, 0, 0, N2y, 0, 0, N3y, 0, 0, N4y},
                         {0, N1y, N1x, 0, N2y, N2x, 0, N3y, N3x, 0, N4y, N4x}};

  double P5_ξ = -((1.0 - eta) * xi);
  double P6_ξ = 0.5 * (1.0 - eta * eta);
  double P7_ξ = -((1 + eta) * xi);
  double P8_ξ = 0.5 * (-1.0 + eta * eta);

  double P5_η = 0.5 * (-1.0 + xi * xi);
  double P6_η = -eta * (1.0 + xi);
  double P7_η = 0.5 * (1.0 - xi * xi);
  double P8_η = -eta * (1.0 - xi);

  double P5x = Gm11 * P5_ξ + Gm12 * P5_η;
  double P6x = Gm11 * P6_ξ + Gm12 * P6_η;
  double P7x = Gm11 * P7_ξ + Gm12 * P7_η;
  double P8x = Gm11 * P8_ξ + Gm12 * P8_η;

  double P5y = Gm21 * P5_ξ + Gm22 * P5_η;
  double P6y = Gm21 * P6_ξ + Gm22 * P6_η;
  double P7y = Gm21 * P7_ξ + Gm22 * P7_η;
  double P8y = Gm21 * P8_ξ + Gm22 * P8_η;

  double Ck[] = {1.0, 0, -1.0, 0};
  double Sk[] = {0, 1.0, 0, -1.0};
  double step = genetal_element_->meshData_->step / 1000.0;
  double l[] = {step, step, step, step};

  double physicalProperties[6];
  genetal_element_->getMaterial()->getMaterialProperties(physicalProperties);
  double Em = physicalProperties[0];
  double K = physicalProperties[1];
  double nu = physicalProperties[2];
  double t = physicalProperties[3];
  double D = physicalProperties[4];
  double G = physicalProperties[5];

  double K1 = 1.0f;
  double v1 = 0.3f;
  double t1 = 7.0f;

  double φ5 = (2.0 / (K1 * (1.0 - v1))) * (t1 / step) * (t1 / step);
  double φ6 = φ5, φ7 = φ5, φ8 = φ5;
  double L5 = l[0];
  double L6 = l[1];
  double L7 = l[2];
  double L8 = l[3];

  double C5 = Ck[0];
  double C6 = Ck[1];
  double C7 = Ck[2];
  double C8 = Ck[3];

  double S5 = Sk[0];
  double S6 = Sk[1];
  double S7 = Sk[2];
  double S8 = Sk[3];

  MatrixXd Gs = MatrixXd{{Gm11, Gm12}, {Gm21, Gm22}};

  MatrixXd As = MatrixXd{{L5 / 2.0, 0, 0, 0},
                         {0, L6 / 2.0, 0, 0},
                         {0, 0, -L7 / 2.0, 0},
                         {0, 0, 0, -L8 / 2.0}};

  MatrixXd Ns = MatrixXd{
      {(1 - eta) / 2.0, 0, (1 + eta) / 2.0, 0},
      {0, (1 + xi) / 2.0, 0, (1 - xi) / 2.0},
  };

  MatrixXd ADeltaDKQM = (2.0 / 3.0) * MatrixXd{{1 + φ5, 0, 0, 0},
                                               {0, 1 + φ6, 0, 0},
                                               {0, 0, 1 + φ7, 0},
                                               {0, 0, 0, 1 + φ8}};

  MatrixXd APhi =
      (2.0 / 3.0) *
      MatrixXd{{φ5, 0, 0, 0}, {0, φ6, 0, 0}, {0, 0, φ7, 0}, {0, 0, 0, φ8}};

  MatrixXd Ad = (-1.0 / 2.0) *
                MatrixXd{{2 / L5, C5, S5, -2 / L5, C5, S5, 0, 0, 0, 0, 0, 0},
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
  MatrixXd Cb = genetal_element_->elasticity_matrix_[0];
  MatrixXd Cs = genetal_element_->elasticity_matrix_[1];

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

QVector<double> FemPlateDkmq::getResultVector(VectorXd U,
                                              double xi,
                                              double eta) {
  double Ck[] = {1.0, 0, -1.0, 0};
  double Sk[] = {0, 1.0, 0, -1.0};
  double step = genetal_element_->meshData_->step / 1000.0;
  double l[] = {step, step, step, step};

  double physicalProperties[6];
  genetal_element_->getMaterial()->getMaterialProperties(physicalProperties);
  double Em = physicalProperties[0];
  double K = physicalProperties[1];
  double nu = physicalProperties[2];
  double t = physicalProperties[3];
  double D = physicalProperties[4];
  double G = physicalProperties[5];

  double φ5 = (2.0 / (K * (1 - nu))) * (t / step) * (t / step);
  double φ6 = φ5, φ7 = φ5, φ8 = φ5;
  double L5 = l[0];
  double L6 = l[1];
  double L7 = l[2];
  double L8 = l[3];

  double C5 = Ck[0];
  double C6 = Ck[1];
  double C7 = Ck[2];
  double C8 = Ck[3];

  double S5 = Sk[0];
  double S6 = Sk[1];
  double S7 = Sk[2];
  double S8 = Sk[3];

  MatrixXd Ad = (-1.0 / 2.0) *
                MatrixXd{{2 / L5, C5, S5, -2 / L5, C5, S5, 0, 0, 0, 0, 0, 0},
                         {0, 0, 0, 2 / L6, C6, S6, -2 / L6, C6, S6, 0, 0, 0},
                         {0, 0, 0, 0, 0, 0, 2 / L7, C7, S7, -2 / L7, C7, S7},
                         {-2 / L8, C8, S8, 0, 0, 0, 0, 0, 0, 2 / L8, C8, S8}};

  MatrixXd ADeltaDKQM = (2.0 / 3.0) * MatrixXd{{1 + φ5, 0, 0, 0},
                                               {0, 1 + φ6, 0, 0},
                                               {0, 0, 1 + φ7, 0},
                                               {0, 0, 0, 1 + φ8}};

  MatrixXd deltaPsi = ADeltaDKQM.inverse() * Ad * U;

  double deltaPsi5 = deltaPsi(0);
  double deltaPsi6 = deltaPsi(1);
  double deltaPsi7 = deltaPsi(2);
  double deltaPsi8 = deltaPsi(3);

  double P5 = (1.0 / 2.0) * (1 - xi * xi) * (1 - eta);
  double P6 = (1.0 / 2.0) * (1 + xi) * (1 - eta * eta);
  double P7 = (1.0 / 2.0) * (1 - xi * xi) * (1 + eta);
  double P8 = (1.0 / 2.0) * (1 - xi) * (1 - eta * eta);

  double uz = N1(xi, eta) * U[0] + N2(xi, eta) * U[3] + N3(xi, eta) * U[6] +
              N4(xi, eta) * U[9];

  double psix = N1(xi, eta) * U[1] + N2(xi, eta) * U[4] + N3(xi, eta) * U[7] +
                N4(xi, eta) * U[10] + P5 * C5 * deltaPsi5 +
                P6 * C6 * deltaPsi6 + P7 * C7 * deltaPsi7 + P8 * C8 * deltaPsi8;

  double psiy = N1(xi, eta) * U[2] + N2(xi, eta) * U[5] + N3(xi, eta) * U[8] +
                N4(xi, eta) * U[11] + P5 * C5 * deltaPsi5 +
                P6 * C6 * deltaPsi6 + P7 * C7 * deltaPsi7 + P8 * C8 * deltaPsi8;

  MatrixXd Bb = bMatrix(xi, eta, 0);
  MatrixXd Bs = bMatrix(xi, eta, 1);

  MatrixXd Cb = -genetal_element_->elasticity_matrix_[0];
  MatrixXd Cs = genetal_element_->elasticity_matrix_[1];

  VectorXd M = Cb * Bb * U;
  VectorXd Q = Cs * Bs * U;

  return QVector<double>{{uz, psix, psiy, Q[0], Q[1], M[0], M[1], M[2]}};
};
