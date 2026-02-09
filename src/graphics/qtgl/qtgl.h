#pragma once

#include <qcontainerfwd.h>
#include <qevent.h>
#include <qglobal.h>
#include <qmainwindow.h>
#include <qpoint.h>
#include <qtmetamacros.h>

#include <QLabel>
#include <QList>
#include <QOpenGLWidget>

#include "fem_elements/node.h"
#include "fem_elements/point.h"
#include "general_element/element.h"

class MainWindow;
class AbstractElement;

struct GraphickElement {
  QVector<Point3> points;
};

class Qtgl : public QOpenGLWidget {
  Q_OBJECT

 private:
  GLuint n_mesh_;
  GLfloat x_rotate_;
  GLfloat y_rotate_;
  GLfloat scale_ = 1.0f;
  QPoint pt_position_;
  QPoint pt_position_old_;
  QPointF pt_position_delta_{0, 0};
  bool is_left_but_{false};
  bool is_middle_but_{false};
  bool is_need_set_coods_{true};

  QVector<shared_ptr<AbstractElement>>* elements_;

  QVector<double> max_abs_values_;
  QVector<double> max_values_;
  QVector<double> min_values_;

  short result_index_{-1};

  bool is_mesh_data_valid_ = false;  // Флаг наличия данных
  float min_x_, max_x_, min_y_, max_y_, min_z_, max_z_;
  float center_x_, center_y_, center_z_, scale_factor_;

  void normalizeMeshData();    // Нормировка данных сетки
  void calculateMeshBounds();  // Вычисление границ
  void createMeshDisplayList();
  void normalizeOutData();
  void updateProjection();

 protected:
  virtual void initializeGL();
  virtual void resizeGL(int n_width, int n_height);
  virtual void paintGL();
  virtual void mousePressEvent(QMouseEvent* pe);
  virtual void mouseMoveEvent(QMouseEvent* pe);
  virtual void wheelEvent(QWheelEvent* pe);
  // virtual void mouseReleaseEvent(QMouseEvent *pe);

 public:
  Qtgl(QWidget* pwgt = 0);
  void setMeshData(QVector<shared_ptr<AbstractElement>>* elements);

  void setResulthData(const QVector<double>& max_abs_values,
                      const QVector<double>& max_values,
                      const QVector<double>& min_values);

  void setResulthIndex(MainWindow* mainwindow, short index);

 signals:
  void needOutputTableDock(const QVector<Node*>& nodes, bool is_need_set_coods);
};
