#pragma once

// #include "/home/vladislav/Документы/FEM/FEM
// program/src/elements/elementdata.h"
// #include "/home/vladislav/Документы/FEM/FEM program/src/elements/element.h"
#include <QMap>
#include <QVector>

#include "elements/load/femload.h"
#include "elements/node.h"
#include "elements/point.h"
#include "femtypes.h"
#include "generalElement/load/load.h"
// #include "plates/plates.h"
#include "../generalElement/displacement/displacement.h"
#include "displacement/femdisplacement.h"

#define SQRT_3 1.7320508075688772

using MethodPtrDisp = NodeDisplacement *(*)(bool *value, Node *node);
using MethodPtrLoad = NodeLoad *(*)(double *values, double *coefs);
using GetPointFunc = Point3 (*)(int index, Point3 &point0, double step,
                                double cosA, double sinA);

struct ElementData {
  short MAIN_NODES_COUNT;
  QVector<double> MAIN_NODES_XI_SET;
  QVector<double> MAIN_NODES_ETA_SET;
  short NODES_COUNT;
  short INT_POINTS_COUNT;
  short STIFF_MATRIX_SIZE;
  bool IS_FULL_DOF;
  short FULL_DOF_COUNT;
  short BAD_DOF_BEGIN; // С какого индекса надо начинат учитывать коррекцию от
                       // предыдущего элемента (для MITC16 = 16)
  short BAD_DOF_COUNT; // Количество степеней свободы, которое надо добавить //
                       // чтобы получить fullDoff
  short OUTPUT_VALUES_COUNT;
  QVector<double> XI_SET;
  QVector<double> ETA_SET;
  QVector<double> W_COEFS;
  QVector<short> DOF_MAP;
  QVector<short> LOCAL_ID_FROM_STIFFMAT;
  QVector<short> BAD_DOF_MAP;
  QVector<LoadType> LOAD_MAP;
  QVector<short> OUTPUT_INDEX_MAP;
  QVector<QString> STR_OUTPUT_VALUES;

  QVector<MethodPtrDisp> DISP_FN_MAP;

  QVector<MethodPtrLoad> LOAD_FN_MAP;

  GetPointFunc GET_POINT_FROM_INDEX_FN;
};

class ElementProvider {
public:
  constexpr static short outputCounts = 14;

  static QMap<ElementType, ElementData> elementData;

  static void init();
};