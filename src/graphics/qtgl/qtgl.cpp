#include "qtgl.h"

#include <qglobal.h>
#include <qmainwindow.h>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qpoint.h>

#include <QOpenGLFunctions>
#include <QtGui>

#include "app/main_window.h"
#include "fem_elements/element_provider.h"

Qtgl::Qtgl(QWidget* pwgt /*= 0*/)
    : QOpenGLWidget(pwgt), x_rotate_(0), y_rotate_(0), scale_(1.0f) {}

/*virtual*/
void Qtgl::initializeGL() {
  QOpenGLFunctions* pFunc = QOpenGLContext::currentContext()->functions();
  pFunc->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  pFunc->glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  createMeshDisplayList();
}

/*virtual*/ void Qtgl::resizeGL(int n_width, int n_height) {
  glViewport(0, 0, (GLint)n_width, (GLint)n_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspect = (float)n_width / (float)n_height;

  if (is_mesh_data_valid_) {
    // Используем ортографическую проекцию на основе размеров модели
    float modelSize =
        1.0f;  // Для нормализованных данных в диапазоне [-0.5, 0.5]

    // Добавляем запас 20%
    float padding = 1.2f;

    if (aspect > 1.0f) {
      // Широкое окно
      glOrtho(-modelSize * padding * aspect, modelSize * padding * aspect,
              -modelSize * padding, modelSize * padding, -10.0, 10.0);
    } else {
      // Высокое окно
      glOrtho(-modelSize * padding, modelSize * padding,
              -modelSize * padding / aspect, modelSize * padding / aspect,
              -10.0, 10.0);
    }
  } else {
    // Стандартная проекция, если модель не загружена
    if (aspect > 1.0f) {
      glOrtho(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -10.0, 10.0);
    } else {
      glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -10.0, 10.0);
    }
  }
}

/*virtual*/
void Qtgl::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Управление камерой - убираем фиксированное смещение по Z
  glTranslatef(pt_position_delta_.rx(), -pt_position_delta_.ry(), 0.0f);

  // Масштабирование
  glScalef(scale_, scale_, scale_);

  // Вращение
  glRotatef(x_rotate_, 1.0, 0.0, 0.0);
  glRotatef(y_rotate_, 0.0, 1.0, 0.0);

  // Рисуем сетку
  if (is_mesh_data_valid_) {
    glCallList(n_mesh_);
  }
}

/*virtual*/ void Qtgl::mousePressEvent(QMouseEvent* pe) {
  if (pe->button() == Qt::LeftButton) {
    is_left_but_ = true;
    pt_position_ = pe->pos();
  } else {
    is_left_but_ = false;
  }

  if (pe->button() == Qt::MiddleButton) {
    is_middle_but_ = true;
    pt_position_old_ = pe->pos();
  } else {
    is_middle_but_ = false;
  }
}

void Qtgl::wheelEvent(QWheelEvent* pe) {
  GLfloat delta = pe->angleDelta().y();
  if (delta > 0)
    scale_ *= 1.1f;  // Увеличиваем масштаб
  else
    scale_ /= 1.1f;  // Уменьшаем масштаб
  update();
}

/*virtual*/ void Qtgl::mouseMoveEvent(QMouseEvent* pe) {
  if (is_left_but_) {
    x_rotate_ += 180 * (GLfloat)(pe->pos().y() - pt_position_.y()) / height();
    y_rotate_ += 180 * (GLfloat)(pe->pos().x() - pt_position_.x()) / width();
    update();
  }

  if (is_middle_but_) {
    // Более медленное перемещение для ортографической проекции
    pt_position_delta_ += (pe->position() - pt_position_old_) * 0.002;
    pt_position_old_ = pe->pos();
    update();
  }
  pt_position_ = pe->pos();
}

void Qtgl::setMeshData(QVector<shared_ptr<AStructuralElement>>* elements) {
  this->elements_ = elements;

  for (auto& element : *elements) {
    auto nodes = element->meshData_->nodes_;
    auto elements = element->meshData_->femElements;

    is_mesh_data_valid_ = !nodes.empty() && !elements.empty();
    if (!is_mesh_data_valid_) break;
  }

  if (is_mesh_data_valid_) {
    // Нормируем данные
    normalizeMeshData();

    // Пересоздаем дисплейный список
    if (n_mesh_) {
      glDeleteLists(n_mesh_, 1);
    }
    createMeshDisplayList();

    // Обновляем проекцию для новой модели
    updateProjection();
    update();
  }
}

void Qtgl::setResulthData(const QVector<double>& max_abs_values,
                          const QVector<double>& max_values,
                          const QVector<double>& min_values) {
  this->max_abs_values_ = max_abs_values;
  this->max_values_ = max_values;
  this->min_values_ = min_values;
}

void Qtgl::setResulthIndex(MainWindow* main_window, short index) {
  result_index_ = index;
  QString labelText;

  for (const auto& element : *elements_) {
    labelText += QString("max value: %1, ").arg(element->min_values_[index]) +
                 QString("min value: %1").arg(element->max_values_[index]) +
                 "\t";
  }

  main_window->status_label_->setText(labelText);

  double scaleForOutput =
      8000.0 / (*elements_).first()->max_abs_values_[result_index_];

  for (auto& element : *elements_) {
    auto nodes = element->meshData_->nodes_;

    for (const auto& node : nodes) {
      double value = node->outputValues[result_index_];
      node->glOutputValue = value * scaleForOutput + node->point.z;
    }
  }

  // Нормализуем выходные данные
  normalizeOutData();

  if (n_mesh_) {
    glDeleteLists(n_mesh_, 1);
  }
  createMeshDisplayList();

  is_need_set_coods_ = false;
  update();
}

void Qtgl::createMeshDisplayList() {
  if (!is_mesh_data_valid_) return;

  if (n_mesh_) {
    glDeleteLists(n_mesh_, 1);
  }

  n_mesh_ = glGenLists(1);
  glNewList(n_mesh_, GL_COMPILE);

  // Loop through str elements
  for (const auto element : *elements_) {
    // 1. Рисуем элементы (прямоугольники)
    glColor4f(0.8f, 0.8f, 0.8f,
              0.7f);  // Полупрозрачный серый для элементов

    auto nodes = element->meshData_->nodes_;
    auto femElements = element->meshData_->femElements;

    // Loop through fem elements
    for (const auto& element : femElements) {
      short main_nodes_count =
          ElementProvider.at(element->type_).MAIN_NODES_COUNT;

      glBegin(GL_QUADS);
      for (int i = 0; i < main_nodes_count; i++) {
        const Node* node = element->nodes_[i];
        glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
      }
      glEnd();

      // Рисуем контур элемента (черный)
      glBegin(GL_LINE_LOOP);
      glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
      for (int i = 0; i < main_nodes_count; i++) {
        const Node* node = element->nodes_[i];
        glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
      }
      glEnd();

      // Рисуем расчетный параметр
      if (result_index_ != -1) {
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        for (int i = 0; i < main_nodes_count; i++) {
          const Node* node = element->nodes_[i];
          glVertex3f(node->glPoint.x, node->glOutputValue, node->glPoint.y);
        }
        glEnd();
      }
      // Return color for next elements
      glColor4f(0.8f, 0.8f, 0.8f, 0.7f);
    }

    // 2. Draw nodes (black dots)
    glPointSize(7.0f);
    glBegin(GL_POINTS);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

    for (const auto node : nodes) {
      glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
    }
    glEnd();
  }

  glEndList();
}

void Qtgl::calculateMeshBounds() {
  min_x_ = max_x_ = elements_->first()->meshData_->nodes_[0]->point.x;
  min_y_ = max_y_ = elements_->first()->meshData_->nodes_[0]->point.y;
  min_z_ = max_z_ = elements_->first()->meshData_->nodes_[0]->point.z;

  for (auto& element : *elements_) {
    auto nodes = element->meshData_->nodes_;

    if (nodes.empty()) return;

    for (const auto node : nodes) {
      min_x_ = std::min(min_x_, node->glPoint.x);
      max_x_ = std::max(max_x_, node->glPoint.x);
      min_y_ = std::min(min_y_, node->glPoint.y);
      max_y_ = std::max(max_y_, node->glPoint.y);
      min_z_ = std::min(min_z_, node->glPoint.z);
      max_z_ = std::max(max_z_, node->glPoint.z);
    }

    center_x_ = (min_x_ + max_x_) * 0.5f;
    center_y_ = (min_y_ + max_y_) * 0.5f;
    center_z_ = (min_z_ + max_z_) * 0.5f;

    // Находим максимальный размер для масштабирования
    float dx = max_x_ - min_x_;
    float dy = max_y_ - min_y_;
    float dz = max_z_ - min_z_;
    scale_factor_ = std::max({dx, dy, dz});

    if (scale_factor_ < 0.001f)
      scale_factor_ = 1.0f;
    else {
      // Увеличиваем масштаб для лучшего отображения
      scale_factor_ = scale_factor_ * 1.5f;
    }
  }
}

void Qtgl::normalizeMeshData() {
  calculateMeshBounds();

  for (auto& element : *elements_) {
    auto nodes = element->meshData_->nodes_;

    // Центрируем и нормируем узлы
    for (auto node : nodes) {
      // if (node->isNormolize)
      //   continue;

      node->glPoint.x = (node->point.x - center_x_) / scale_factor_;
      node->glPoint.y = (node->point.y - center_y_) / scale_factor_;
      node->glPoint.z = (node->point.z - center_z_) / scale_factor_;

      // node->isNormolize = true;
    }
  }
}

void Qtgl::normalizeOutData() {
  for (auto& element : *elements_) {
    auto nodes = element->meshData_->nodes_;
    for (const auto& node : nodes) {
      // Нормализуем выходные данные так же, как и геометрию
      node->glOutputValue = (node->glOutputValue - center_z_) / scale_factor_;
    }
  }
}

void Qtgl::updateProjection() {
  // Принудительно обновляем проекцию для текущего размера окна
  resizeGL(width(), height());
}