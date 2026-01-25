#pragma once

enum ElementType { MITC4MY = 0, MITC9 = 1, MITC16 = 2, NONE = 3 };

// enum DisplType // Тип степеней свободы, который поддерживает узел
// {
//   Ux,
//   Uy,
//   Uz,
//   UxUy,
//   RUy,
//   UxUyR,
//   UzRxRy,
//   RxRy
// };

enum LoadType {
  Fx,
  Fy,
  FxFy,
  MFy,
  MFx,
  MFxy,
  q,
  FyMxMz,
  MxMz,
};

enum OutputType {
  Ux = 0,
  Uy = 1,
  Uz = 2,
  Rx = 3,
  Ry = 4,
  Rz = 5,
  Nx = 6,
  Ny = 7,
  Nxy = 8,
  Qx = 9,
  Qy = 10,
  Mx = 11,
  My = 12,
  Mxy = 13,
  NONE_OUTPUT_TYPE = 14,
};

//