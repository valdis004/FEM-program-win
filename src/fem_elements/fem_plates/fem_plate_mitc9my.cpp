#include "fem_plate_mitc9my.h"

#include <qdebug.h>
#include <qglobal.h>

#include <Eigen/Core>
#include <cstddef>
#include <exception>

#include "element_provider.h"
#include "structural_element/structural_element.h"

using ShapeFunc = double (*)(double, double);

// Функции формы для 8-узлового элемента
C_S_C auto N1w = [](double xi, double eta) -> double {
  return 0.25 * (1 - xi) * (1 - eta) * (-xi - eta - 1);
};

C_S_C auto N2w = [](double xi, double eta) -> double {
  return 0.25 * (1 + xi) * (1 - eta) * (xi - eta - 1);
};

C_S_C auto N3w = [](double xi, double eta) -> double {
  return 0.25 * (1 + xi) * (1 + eta) * (xi + eta - 1);
};

C_S_C auto N4w = [](double xi, double eta) -> double {
  return 0.25 * (1 - xi) * (1 + eta) * (-xi + eta - 1);
};

C_S_C auto N5w = [](double xi, double eta) -> double {
  return 0.5 * (1 - xi * xi) * (1 - eta);
};

C_S_C auto N6w = [](double xi, double eta) -> double {
  return 0.5 * (1 + xi) * (1 - eta * eta);
};

C_S_C auto N7w = [](double xi, double eta) -> double {
  return 0.5 * (1 - xi * xi) * (1 + eta);
};

C_S_C auto N8w = [](double xi, double eta) -> double {
  return 0.5 * (1 - xi) * (1 - eta * eta);
};

// Функции формы для 9-узлового элемента
C_S_C auto N1b = [](double xi, double eta) -> double {
  return 0.25 * xi * eta * (xi - 1) * (eta - 1);
};

C_S_C auto N2b = [](double xi, double eta) -> double {
  return 0.25 * xi * eta * (xi + 1) * (eta - 1);
};

C_S_C auto N3b = [](double xi, double eta) -> double {
  return 0.25 * xi * eta * (xi + 1) * (eta + 1);
};

C_S_C auto N4b = [](double xi, double eta) -> double {
  return 0.25 * xi * eta * (xi - 1) * (eta + 1);
};

C_S_C auto N5b = [](double xi, double eta) -> double {
  return 0.5 * eta * (1 - xi * xi) * (eta - 1);
};

C_S_C auto N6b = [](double xi, double eta) -> double {
  return 0.5 * xi * (xi + 1) * (1 - eta * eta);
};

C_S_C auto N7b = [](double xi, double eta) -> double {
  return 0.5 * eta * (1 - xi * xi) * (eta + 1);
};

C_S_C auto N8b = [](double xi, double eta) -> double {
  return 0.5 * xi * (xi - 1) * (1 - eta * eta);
};

C_S_C auto N9b = [](double xi, double eta) -> double {
  return (1 - xi * xi) * (1 - eta * eta);
};

// Массивы функций формы
C_S_C ShapeFunc Nb[] = {N1b, N2b, N3b, N4b, N5b, N6b, N7b, N8b, N9b};
C_S_C ShapeFunc Nw[] = {N1w, N2w, N3w, N4w, N5w, N6w, N7w, N8w};

// Производные функций формы Nw по xi
C_S_C auto Nwdxi1 = [](double xi, double eta) -> double {
  return -0.25 * (1 - eta) * (1 - xi) - 0.25 * (1 - eta) * (-1 - eta - xi);
};

C_S_C auto Nwdxi2 = [](double xi, double eta) -> double {
  return 0.25 * (1 - eta) * (1 + xi) + 0.25 * (1 - eta) * (-1 - eta + xi);
};

C_S_C auto Nwdxi3 = [](double xi, double eta) -> double {
  return 0.25 * (1 + eta) * (1 + xi) + 0.25 * (1 + eta) * (-1 + eta + xi);
};

C_S_C auto Nwdxi4 = [](double xi, double eta) -> double {
  return -0.25 * (1 + eta) * (1 - xi) - 0.25 * (1 + eta) * (-1 + eta - xi);
};

C_S_C auto Nwdxi5 = [](double xi, double eta) -> double {
  return -((1 - eta) * xi);
};

C_S_C auto Nwdxi6 = [](double xi, double eta) -> double {
  return 0.5 * (1 - eta * eta);
};

C_S_C auto Nwdxi7 = [](double xi, double eta) -> double {
  return -((1 + eta) * xi);
};

C_S_C auto Nwdxi8 = [](double xi, double eta) -> double {
  return 0.5 * (-1 + eta * eta);
};

// Производные функций формы Nw по eta
C_S_C auto Nwdeta1 = [](double xi, double eta) -> double {
  return -0.25 * (1 - eta) * (1 - xi) - 0.25 * (1 - xi) * (-1 - eta - xi);
};

C_S_C auto Nwdeta2 = [](double xi, double eta) -> double {
  return -0.25 * (1 - eta) * (1 + xi) - 0.25 * (1 + xi) * (-1 - eta + xi);
};

C_S_C auto Nwdeta3 = [](double xi, double eta) -> double {
  return 0.25 * (1 + eta) * (1 + xi) + 0.25 * (1 + xi) * (-1 + eta + xi);
};

C_S_C auto Nwdeta4 = [](double xi, double eta) -> double {
  return 0.25 * (1 + eta) * (1 - xi) + 0.25 * (1 - xi) * (-1 + eta - xi);
};

C_S_C auto Nwdeta5 = [](double xi, double eta) -> double {
  return 0.5 * (-1 + xi * xi);
};

C_S_C auto Nwdeta6 = [](double xi, double eta) -> double {
  return -eta * (1 + xi);
};

C_S_C auto Nwdeta7 = [](double xi, double eta) -> double {
  return 0.5 * (1 - xi * xi);
};

C_S_C auto Nwdeta8 = [](double xi, double eta) -> double {
  return -eta * (1 - xi);
};

// Производные функций формы Nb по xi
C_S_C auto Nbdxi1 = [](double xi, double eta) -> double {
  return 0.25 * (-1 + eta) * eta * (-1 + xi) + 0.25 * (-1 + eta) * eta * xi;
};

C_S_C auto Nbdxi2 = [](double xi, double eta) -> double {
  return 0.25 * (-1 + eta) * eta * xi + 0.25 * (-1 + eta) * eta * (1 + xi);
};

C_S_C auto Nbdxi3 = [](double xi, double eta) -> double {
  return 0.25 * eta * (1 + eta) * xi + 0.25 * eta * (1 + eta) * (1 + xi);
};

C_S_C auto Nbdxi4 = [](double xi, double eta) -> double {
  return 0.25 * eta * (1 + eta) * (-1 + xi) + 0.25 * eta * (1 + eta) * xi;
};

C_S_C auto Nbdxi5 = [](double xi, double eta) -> double {
  return -((-1 + eta) * eta * xi);
};

C_S_C auto Nbdxi6 = [](double xi, double eta) -> double {
  return 0.5 * (1 - eta * eta) * xi + 0.5 * (1 - eta * eta) * (1 + xi);
};

C_S_C auto Nbdxi7 = [](double xi, double eta) -> double {
  return -eta * (1 + eta) * xi;
};

C_S_C auto Nbdxi8 = [](double xi, double eta) -> double {
  return 0.5 * (1 - eta * eta) * (-1 + xi) + 0.5 * (1 - eta * eta) * xi;
};

C_S_C auto Nbdxi9 = [](double xi, double eta) -> double {
  return -2 * (1 - eta * eta) * xi;
};

// Производные функций формы Nb по eta
C_S_C auto Nbdeta1 = [](double xi, double eta) -> double {
  return 0.25 * (-1 + eta) * (-1 + xi) * xi + 0.25 * eta * (-1 + xi) * xi;
};

C_S_C auto Nbdeta2 = [](double xi, double eta) -> double {
  return 0.25 * (-1 + eta) * xi * (1 + xi) + 0.25 * eta * xi * (1 + xi);
};

C_S_C auto Nbdeta3 = [](double xi, double eta) -> double {
  return 0.25 * eta * xi * (1 + xi) + 0.25 * (1 + eta) * xi * (1 + xi);
};

C_S_C auto Nbdeta4 = [](double xi, double eta) -> double {
  return 0.25 * eta * (-1 + xi) * xi + 0.25 * (1 + eta) * (-1 + xi) * xi;
};

C_S_C auto Nbdeta5 = [](double xi, double eta) -> double {
  return 0.5 * (-1 + eta) * (1 - xi * xi) + 0.5 * eta * (1 - xi * xi);
};

C_S_C auto Nbdeta6 = [](double xi, double eta) -> double {
  return -eta * xi * (1 + xi);
};

C_S_C auto Nbdeta7 = [](double xi, double eta) -> double {
  return 0.5 * eta * (1 - xi * xi) + 0.5 * (1 + eta) * (1 - xi * xi);
};

C_S_C auto Nbdeta8 = [](double xi, double eta) -> double {
  return -eta * (-1 + xi) * xi;
};

C_S_C auto Nbdeta9 = [](double xi, double eta) -> double {
  return -2 * eta * (1 - xi * xi);
};

// Массивы производных
C_S_C ShapeFunc Nwdxi[] = {Nwdxi1, Nwdxi2, Nwdxi3, Nwdxi4,
                           Nwdxi5, Nwdxi6, Nwdxi7, Nwdxi8};
C_S_C ShapeFunc Nwdeta[] = {Nwdeta1, Nwdeta2, Nwdeta3, Nwdeta4,
                            Nwdeta5, Nwdeta6, Nwdeta7, Nwdeta8};
C_S_C ShapeFunc Nbdxi[] = {Nbdxi1, Nbdxi2, Nbdxi3, Nbdxi4, Nbdxi5,
                           Nbdxi6, Nbdxi7, Nbdxi8, Nbdxi9};
C_S_C ShapeFunc Nbdeta[] = {Nbdeta1, Nbdeta2, Nbdeta3, Nbdeta4, Nbdeta5,
                            Nbdeta6, Nbdeta7, Nbdeta8, Nbdeta9};

// C_S_C auto Map = [](double xi, double eta, int type, const double xSet[],
//                     const double ySet[]) -> double {
//   double value = 0.0;
//   if (type == 0) {
//     for (int i = 0; i < 4; i++) {
//       value += N[i](xi, eta) * xSet[i];
//     }
//   } else {
//     for (int i = 0; i < 4; i++) {
//       value += N[i](xi, eta) * ySet[i];
//     }
//   }
//   return value;
// };

C_S_C auto NbDiffAny =
    [](double* arr, double xi, double eta, double gm1, double gm2) {
      for (int i = 0; i < 9; i++) {
        arr[i] = Nbdxi[i](xi, eta) * gm1 + Nbdeta[i](xi, eta) * gm2;
      }
      return arr;
    };

C_S_C auto NwDiffAny =
    [](double* arr, double xi, double eta, double gm1, double gm2) {
      for (int i = 0; i < 8; i++) {
        arr[i] = Nwdxi[i](xi, eta) * gm1 + Nwdeta[i](xi, eta) * gm2;
      }
      return arr;
    };

FemPlateMitc9My::FemPlateMitc9My(
    unsigned id,
    Node** nodes,
    std::shared_ptr<AStructuralElement> generalElement)
    : AFemElement(id, nodes, 9, ElementType::MITC9MY, generalElement) {};

FemPlateMitc9My::FemPlateMitc9My(
    unsigned id,
    Node** nodes,
    const Material& material,
    std::shared_ptr<AStructuralElement> generalElement)
    : AFemElement(
          id, nodes, 9, material, ElementType::MITC9MY, generalElement) {};

MatrixXd FemPlateMitc9My::jMatrix(double xi, double eta) {
  float coord_x[9];
  float coord_y[9];
  float coord_z[9];

  for (int i = 0; i < DATA.NODES_COUNT; i++) {
    coord_x[i] = nodes_[i]->point.x / 1000.0;
    coord_y[i] = nodes_[i]->point.y / 1000.0;
    coord_z[i] = nodes_[i]->point.z / 1000.0;
  }

  float j11 = 0, j12 = 0, j21 = 0, j22 = 0;

  for (int i = 0; i < 9; i++) {
    j11 += coord_x[i] * Nbdxi[i](xi, eta);
    j12 += coord_x[i] * Nbdeta[i](xi, eta);
    j21 += coord_y[i] * Nbdxi[i](xi, eta);
    j22 += coord_y[i] * Nbdeta[i](xi, eta);
  }

  return MatrixXd{
      {j11, j12},
      {j21, j22},
  };
}

/* static  */ MatrixXd FemPlateMitc9My::cMatrix(Material* material, int type) {
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

double sum(MatrixXd m) {
  float sum = 0;
  for (int i = 0; i < m.rows(); i++) {
    for (int j = 0; j < m.cols(); j++) {
      sum += m(i, j);
    }
  }
  return sum;
}

double sum(Eigen::VectorXd m) {
  double sum = 0;
  for (int j = 0; j < m.size(); j++) {
    double d = m(j);
    sum += m(j);
  }
  return sum;
}

std::pair<MatrixXd, MatrixXd> FemPlateMitc9My::bMatrix(double xi, double eta) {
  MatrixXd J = jMatrix(xi, eta);
  MatrixXd Gm = J.transpose().inverse();

  double Gm11 = Gm(0, 0), Gm12 = Gm(0, 1), Gm21 = Gm(1, 0), Gm22 = Gm(1, 1);

  double Nwdx[8];
  double Nwdy[8];
  NwDiffAny(Nwdx, xi, eta, Gm11, Gm12);
  NwDiffAny(Nwdy, xi, eta, Gm21, Gm22);

  double Nbdx[9];
  double Nbdy[9];
  NbDiffAny(Nbdx, xi, eta, Gm11, Gm12);
  NbDiffAny(Nbdy, xi, eta, Gm21, Gm22);

  MatrixXd Nbm = MatrixXd{
      {0, 0, -Nbdx[0], 0, 0, -Nbdx[1], 0, 0,       -Nbdx[2],
       0, 0, -Nbdx[3], 0, 0, -Nbdx[4], 0, 0,       -Nbdx[5],
       0, 0, -Nbdx[6], 0, 0, -Nbdx[7], 0, -Nbdx[8]},
      {0, Nbdy[0], 0, 0, Nbdy[1], 0, 0,       Nbdy[2], 0,
       0, Nbdy[3], 0, 0, Nbdy[4], 0, 0,       Nbdy[5], 0,
       0, Nbdy[6], 0, 0, Nbdy[7], 0, Nbdy[8], 0},
      {0, Nbdx[0], -Nbdy[0], 0, Nbdx[1], -Nbdy[1], 0,       Nbdx[2], -Nbdy[2],
       0, Nbdx[3], -Nbdy[3], 0, Nbdx[4], -Nbdy[4], 0,       Nbdx[5], -Nbdy[5],
       0, Nbdx[6], -Nbdy[6], 0, Nbdx[7], -Nbdy[7], Nbdx[8], -Nbdy[8]}};

  MatrixXd Ngm = MatrixXd{
      {1, xi, eta, xi * eta, eta * eta, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, xi, eta, xi * eta, xi * xi},
  };

  double b = sqrt(1.0 / 3.0);

  MatrixXd Mgm = MatrixXd{
      {1.0, -b, -1.0, -b * -1.0, -1.0 * -1.0, 0, 0, 0, 0, 0},
      {1.0, b, -1.0, b * -1.0, -1.0 * -1.0, 0, 0, 0, 0, 0},
      {1.0, b, 1.0, b * 1.0, 1.0 * 1.0, 0, 0, 0, 0, 0},
      {1.0, -b, 1.0, -b * 1.0, 1.0 * 1.0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1.0, 1.0, -b, 1.0 * -b, 1.0 * 1.0},
      {0, 0, 0, 0, 0, 1.0, 1.0, b, 1.0 * b, 1.0 * 1.0},
      {0, 0, 0, 0, 0, 1.0, -1.0, b, -1.0 * b, -1.0 * -1.0},
      {0, 0, 0, 0, 0, 1.0, -1.0, -b, -1.0 * -b, -1.0 * -1.0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  VectorXd m9 = VectorXd::Zero(10);
  VectorXd m10 = VectorXd::Zero(10);

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      double xi = DATA.XI_SET[i];
      double eta = DATA.ETA_SET[j];
      double det_j = J.determinant();
      m9 += det_j * DATA.W_COEFS[i] * DATA.W_COEFS[j] *
            VectorXd{{1, xi, eta, xi * eta, eta * eta, 0, 0, 0, 0, 0}};
      m10 += det_j * DATA.W_COEFS[i] * DATA.W_COEFS[j] *
             VectorXd{{0, 0, 0, 0, 0, 1, xi, eta, xi * eta, xi * xi}};
    }
  }

  Mgm.row(8) = m9;
  Mgm.row(9) = m10;

  double Nwdx_a[8];
  double Nwdx_b[8];
  double Nwdx_e[8];
  double Nwdx_f[8];
  NwDiffAny(Nwdx_a, -b, -1, Gm11, Gm12);
  NwDiffAny(Nwdx_b, b, -1, Gm11, Gm12);
  NwDiffAny(Nwdx_e, b, 1, Gm11, Gm12);
  NwDiffAny(Nwdx_f, -b, 1, Gm11, Gm12);

  double Nwdy_c[8];
  double Nwdy_d[8];
  double Nwdy_g[8];
  double Nwdy_h[8];
  NwDiffAny(Nwdy_c, 1, -b, Gm21, Gm22);
  NwDiffAny(Nwdy_d, 1, b, Gm21, Gm22);
  NwDiffAny(Nwdy_g, -1, b, Gm21, Gm22);
  NwDiffAny(Nwdy_h, -1, -b, Gm21, Gm22);

  MatrixXd Nd = MatrixXd{
      {Nwdx_a[0],
       0,
       Nb[0](-b, -1),
       Nwdx_a[1],
       0,
       Nb[1](-b, -1),
       Nwdx_a[2],
       0,
       Nb[2](-b, -1),
       Nwdx_a[3],
       0,
       Nb[3](-b, -1),
       Nwdx_a[4],
       0,
       Nb[4](-b, -1),
       Nwdx_a[5],
       0,
       Nb[5](-b, -1),
       Nwdx_a[6],
       0,
       Nb[6](-b, -1),
       Nwdx_a[7],
       0,
       Nb[7](-b, -1),
       0,
       Nb[8](-b, -1)},
      {Nwdx_b[0], 0,           Nb[0](b, -1), Nwdx_b[1], 0, Nb[1](b, -1),
       Nwdx_b[2], 0,           Nb[2](b, -1), Nwdx_b[3], 0, Nb[3](b, -1),
       Nwdx_b[4], 0,           Nb[4](b, -1), Nwdx_b[5], 0, Nb[5](b, -1),
       Nwdx_b[6], 0,           Nb[6](b, -1), Nwdx_b[7], 0, Nb[7](b, -1),
       0,         Nb[8](b, -1)},
      {Nwdx_e[0], 0,          Nb[0](b, 1), Nwdx_e[1], 0, Nb[1](b, 1),
       Nwdx_e[2], 0,          Nb[2](b, 1), Nwdx_e[3], 0, Nb[3](b, 1),
       Nwdx_e[4], 0,          Nb[4](b, 1), Nwdx_e[5], 0, Nb[5](b, 1),
       Nwdx_e[6], 0,          Nb[6](b, 1), Nwdx_e[7], 0, Nb[7](b, 1),
       0,         Nb[8](b, 1)},
      {Nwdx_f[0], 0,           Nb[0](-b, 1), Nwdx_f[1], 0, Nb[1](-b, 1),
       Nwdx_f[2], 0,           Nb[2](-b, 1), Nwdx_f[3], 0, Nb[3](-b, 1),
       Nwdx_f[4], 0,           Nb[4](-b, 1), Nwdx_f[5], 0, Nb[5](-b, 1),
       Nwdx_f[6], 0,           Nb[6](-b, 1), Nwdx_f[7], 0, Nb[7](-b, 1),
       0,         Nb[8](-b, 1)},
      {Nwdy_c[0],
       -Nb[0](1, -b),
       0,
       Nwdy_c[1],
       -Nb[1](1, -b),
       0,
       Nwdy_c[2],
       -Nb[2](1, -b),
       0,
       Nwdy_c[3],
       -Nb[3](1, -b),
       0,
       Nwdy_c[4],
       -Nb[4](1, -b),
       0,
       Nwdy_c[5],
       -Nb[5](1, -b),
       0,
       Nwdy_c[6],
       -Nb[6](1, -b),
       0,
       Nwdy_c[7],
       -Nb[7](1, -b),
       0,
       -Nb[8](1, -b),
       0},
      {Nwdy_d[0],
       -Nb[0](1, b),
       0,
       Nwdy_d[1],
       -Nb[1](1, b),
       0,
       Nwdy_d[2],
       -Nb[2](1, b),
       0,
       Nwdy_d[3],
       -Nb[3](1, b),
       0,
       Nwdy_d[4],
       -Nb[4](1, b),
       0,
       Nwdy_d[5],
       -Nb[5](1, b),
       0,
       Nwdy_d[6],
       -Nb[6](1, b),
       0,
       Nwdy_d[7],
       -Nb[7](1, b),
       0,
       -Nb[8](1, b),
       0},
      {Nwdy_g[0],
       -Nb[0](-1, b),
       0,
       Nwdy_g[1],
       -Nb[1](-1, b),
       0,
       Nwdy_g[2],
       -Nb[2](-1, b),
       0,
       Nwdy_g[3],
       -Nb[3](-1, b),
       0,
       Nwdy_g[4],
       -Nb[4](-1, b),
       0,
       Nwdy_g[5],
       -Nb[5](-1, b),
       0,
       Nwdy_g[6],
       -Nb[6](-1, b),
       0,
       Nwdy_g[7],
       -Nb[7](-1, b),
       0,
       -Nb[8](-1, b),
       0},
      {Nwdy_h[0],
       -Nb[0](-1, -b),
       0,
       Nwdy_h[1],
       -Nb[1](-1, -b),
       0,
       Nwdy_h[2],
       -Nb[2](-1, -b),
       0,
       Nwdy_h[3],
       -Nb[3](-1, -b),
       0,
       Nwdy_h[4],
       -Nb[4](-1, -b),
       0,
       Nwdy_h[5],
       -Nb[5](-1, -b),
       0,
       Nwdy_h[6],
       -Nb[6](-1, -b),
       0,
       Nwdy_h[7],
       -Nb[7](-1, -b),
       0,
       -Nb[8](-1, -b),
       0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  // double d41 = sum(Nd);

  VectorXd nd9 = VectorXd::Zero(26);
  VectorXd nd10 = VectorXd::Zero(26);

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      double xi = DATA.XI_SET[i];
      double eta = DATA.ETA_SET[j];
      double det_j = J.determinant();

      double cur_Nwdx[8];
      NwDiffAny(cur_Nwdx, xi, eta, Gm11, Gm12);
      double cur_Nwdy[8];
      NwDiffAny(cur_Nwdy, xi, eta, Gm21, Gm22);

      nd9 += det_j * DATA.W_COEFS[i] * DATA.W_COEFS[j] *
             VectorXd{{cur_Nwdx[0],
                       0,
                       Nb[0](xi, eta),
                       cur_Nwdx[1],
                       0,
                       Nb[1](xi, eta),
                       cur_Nwdx[2],
                       0,
                       Nb[2](xi, eta),
                       cur_Nwdx[3],
                       0,
                       Nb[3](xi, eta),
                       cur_Nwdx[4],
                       0,
                       Nb[4](xi, eta),
                       cur_Nwdx[5],
                       0,
                       Nb[5](xi, eta),
                       cur_Nwdx[6],
                       0,
                       Nb[6](xi, eta),
                       cur_Nwdx[7],
                       0,
                       Nb[7](xi, eta),
                       0,
                       Nb[8](xi, eta)}};

      nd10 += det_j * DATA.W_COEFS[i] * DATA.W_COEFS[j] *
              VectorXd{{cur_Nwdy[0],
                        -Nb[0](xi, eta),
                        0,
                        cur_Nwdy[1],
                        -Nb[1](xi, eta),
                        0,
                        cur_Nwdy[2],
                        -Nb[2](xi, eta),
                        0,
                        cur_Nwdy[3],
                        -Nb[3](xi, eta),
                        0,
                        cur_Nwdy[4],
                        -Nb[4](xi, eta),
                        0,
                        cur_Nwdy[5],
                        -Nb[5](xi, eta),
                        0,
                        cur_Nwdy[6],
                        -Nb[6](xi, eta),
                        0,
                        cur_Nwdy[7],
                        -Nb[7](xi, eta),
                        0,
                        -Nb[8](xi, eta),
                        0}};
    }
  }

  Nd.row(8) = nd9;
  Nd.row(9) = nd10;

  MatrixXd Bb = Nbm;
  MatrixXd Bs = Ngm * Mgm.inverse() * Nd;

  return {Bb, Bs};
}

std::pair<MatrixXd, MatrixXd> FemPlateMitc9My::integratingFn(double xi,
                                                             double eta) {
  double detJ = jMatrix(xi, eta).determinant();

  auto b_ms = bMatrix(xi, eta);
  MatrixXd Bb = b_ms.first;
  MatrixXd Bs = b_ms.second;
  MatrixXd Cb = genetal_element_->elasticity_matrix_[0];
  MatrixXd Cs = genetal_element_->elasticity_matrix_[1];
  return {Bb.transpose() * Cb * Bb * detJ, Bs.transpose() * Cs * Bs * detJ};
}

MatrixXd FemPlateMitc9My::getLocalStiffMatrix() {
  MatrixXd k_elem = MatrixXd::Zero(26, 26);

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      double xi = DATA.XI_SET[i];
      double eta = DATA.ETA_SET[j];
      auto int_fns = integratingFn(xi, eta);

      k_elem += int_fns.first * W_COEFS[i] * W_COEFS[j];
      k_elem += int_fns.second * W_COEFS[i] * W_COEFS[j];
    }
  }

  return k_elem;
}

VectorXd FemPlateMitc9My::getLoadVector() {
  VectorXd load_coef_vector = Eigen::VectorXd::Zero(26);

  auto N = [](double xi, double eta) {
    return VectorXd{{Nw[0](xi, eta),
                     0,
                     0,
                     Nw[1](xi, eta),
                     0,
                     0,
                     Nw[2](xi, eta),
                     0,
                     0,
                     Nw[3](xi, eta),
                     0,
                     0,
                     Nw[4](xi, eta),
                     0,
                     0,
                     Nw[5](xi, eta),
                     0,
                     0,
                     Nw[6](xi, eta),
                     0,
                     0,
                     Nw[7](xi, eta),
                     0,
                     0,
                     0,
                     0}};
  };

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      double xi = DATA.XI_SET[i];
      double eta = DATA.ETA_SET[j];
      double det_j = jMatrix(xi, eta).determinant();
      load_coef_vector += N(xi, eta) * W_COEFS[i] * W_COEFS[j] * det_j;
    }
  }

  // for (size_t k = 0; k < 12; k++) {
  //   qDebug() << loadCoefVector(k) << " ";
  // }

  return load_coef_vector;
};

QVector<double> FemPlateMitc9My::getResultVector(VectorXd U,
                                                 double xi,
                                                 double eta) {
  double uz = N1w(xi, eta) * U[0] + N2w(xi, eta) * U[3] + N3w(xi, eta) * U[6] +
              N4w(xi, eta) * U[9] + N5w(xi, eta) * U[12] +
              N6w(xi, eta) * U[15] + N7w(xi, eta) * U[18] +
              N8w(xi, eta) * U[21];
  double psix =
      (N1b(xi, eta) * U[2] + N2b(xi, eta) * U[5] + N3b(xi, eta) * U[8] +
       N4b(xi, eta) * U[11] + N5b(xi, eta) * U[14] + N6b(xi, eta) * U[17] +
       N7b(xi, eta) * U[20] + N8b(xi, eta) * U[23] + N9b(xi, eta) * U[25]);

  double psiy =
      (N1b(xi, eta) * U[1] + N2b(xi, eta) * U[4] + N3b(xi, eta) * U[7] +
       N4b(xi, eta) * U[10] + N5b(xi, eta) * U[13] + N6b(xi, eta) * U[16] +
       N7b(xi, eta) * U[19] + N8b(xi, eta) * U[22] + N9b(xi, eta) * U[24]);

  auto b_ms = bMatrix(xi, eta);
  MatrixXd Bb = b_ms.first;
  MatrixXd Bs = b_ms.second;

  MatrixXd Cb = -genetal_element_->elasticity_matrix_[0];
  MatrixXd Cs = genetal_element_->elasticity_matrix_[1];

  VectorXd M = Cb * Bb * U;
  VectorXd Q = Cs * Bs * U;

  return QVector<double>{{uz, psix, psiy, Q[0], Q[1], M[0], M[1], M[2]}};
};
