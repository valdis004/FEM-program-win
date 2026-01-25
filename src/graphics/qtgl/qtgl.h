#pragma once

#include <QLabel>
#include <QList>
#include <QOpenGLWidget>
#include <qevent.h>
#include <qglobal.h>
#include <qmainwindow.h>
#include <qtmetamacros.h>

#include "elements/node.h"
#include "elements/point.h"
#include "generalElement/element.h"

class MainWindow;

struct GraphickElement {
  QVector<Point3> points;
};

class Qtgl : public QOpenGLWidget {
  Q_OBJECT

private:
  GLuint m_nMesh;
  GLfloat m_xRotate;
  GLfloat m_yRotate;
  GLfloat m_scale = 1.0f;
  QPoint m_ptPosition;
  bool isLeftBut = false;
  bool isNeedSetCoods = true;

  QVector<shared_ptr<AbstractElement>> *elements;

  QVector<double> maxAbsValues;
  QVector<double> maxValues;
  QVector<double> minValues;

  short resultIndex{-1};

  bool m_meshDataValid = false; // Флаг наличия данных
  float m_minX, m_maxX, m_minY, m_maxY, m_minZ, m_maxZ;
  float m_centerX, m_centerY, m_centerZ, m_scaleFactor;

  void normalizeMeshData();   // Нормировка данных сетки
  void calculateMeshBounds(); // Вычисление границ
  void createMeshDisplayList();
  void normalizeOutData();

protected:
  virtual void initializeGL();
  virtual void resizeGL(int nWidth, int nHeight);
  virtual void paintGL();
  virtual void mousePressEvent(QMouseEvent *pe);
  virtual void mouseMoveEvent(QMouseEvent *pe);
  virtual void wheelEvent(QWheelEvent *pe);

public:
  Qtgl(QWidget *pwgt = 0);
  void setMeshData(QVector<shared_ptr<AbstractElement>> *elements);

  void setResulthData(const QVector<double> &maxAbsValues,
                      const QVector<double> &maxValues,
                      const QVector<double> &minValues);

  void setResulthIndex(MainWindow *mainwindow, short index);

signals:
  void needOutputTableDock(const QVector<Node *> &nodes, bool isNeedSetCoods);
};
