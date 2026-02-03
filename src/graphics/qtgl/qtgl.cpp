#include <QOpenGLFunctions>
#include <QtGui>
#include <qglobal.h>
#include <qmainwindow.h>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qpoint.h>

#include "app/mainwindow.h"
#include "elements/elementprovider.h"
#include "qtgl.h"

Qtgl::Qtgl(QWidget *pwgt /*= 0*/)
    : QOpenGLWidget(pwgt), m_xRotate(0), m_yRotate(0), m_scale(1.0f) {}

/*virtual*/ void Qtgl::initializeGL() {
  QOpenGLFunctions *pFunc = QOpenGLContext::currentContext()->functions();
  pFunc->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  pFunc->glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  createMeshDisplayList();
}

/*virtual*/ void Qtgl::resizeGL(int nWidth, int nHeight) {
  glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspect = (float)nWidth / (float)nHeight;

  if (m_meshDataValid) {
    // Используем ортографическую проекцию на основе размеров модели
    float modelSize =
        1.0f; // Для нормализованных данных в диапазоне [-0.5, 0.5]

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

/*virtual*/ void Qtgl::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Управление камерой - убираем фиксированное смещение по Z
  glTranslatef(m_ptPositionDelta.rx(), -m_ptPositionDelta.ry(), 0.0f);

  // Масштабирование
  glScalef(m_scale, m_scale, m_scale);

  // Вращение
  glRotatef(m_xRotate, 1.0, 0.0, 0.0);
  glRotatef(m_yRotate, 0.0, 1.0, 0.0);

  // Рисуем сетку
  if (m_meshDataValid) {
    glCallList(m_nMesh);
  }
}

/*virtual*/ void Qtgl::mousePressEvent(QMouseEvent *pe) {
  if (pe->button() == Qt::LeftButton) {
    isLeftBut = true;
    m_ptPosition = pe->pos();
  } else {
    isLeftBut = false;
  }

  if (pe->button() == Qt::MiddleButton) {
    isMiddleBut = true;
    m_ptPositionOld = pe->pos();
  } else {
    isMiddleBut = false;
  }
}

void Qtgl::wheelEvent(QWheelEvent *pe) {
  GLfloat delta = pe->angleDelta().y();
  if (delta > 0)
    m_scale *= 1.1f; // Увеличиваем масштаб
  else
    m_scale /= 1.1f; // Уменьшаем масштаб
  update();
}

/*virtual*/ void Qtgl::mouseMoveEvent(QMouseEvent *pe) {
  if (isLeftBut) {
    m_xRotate += 180 * (GLfloat)(pe->pos().y() - m_ptPosition.y()) / height();
    m_yRotate += 180 * (GLfloat)(pe->pos().x() - m_ptPosition.x()) / width();
    update();
  }

  if (isMiddleBut) {
    // Более медленное перемещение для ортографической проекции
    m_ptPositionDelta += (pe->position() - m_ptPositionOld) * 0.002;
    m_ptPositionOld = pe->pos();
    update();
  }
  m_ptPosition = pe->pos();
}

void Qtgl::setMeshData(QVector<shared_ptr<AbstractElement>> *elements) {
  this->elements = elements;

  for (auto &element : *elements) {
    auto nodes = element->meshData_->nodes;
    auto elements = element->meshData_->femElements;

    m_meshDataValid = !nodes.empty() && !elements.empty();
    if (!m_meshDataValid)
      break;
  }

  if (m_meshDataValid) {
    // Нормируем данные
    normalizeMeshData();

    // Пересоздаем дисплейный список
    if (m_nMesh) {
      glDeleteLists(m_nMesh, 1);
    }
    createMeshDisplayList();

    // Обновляем проекцию для новой модели
    updateProjection();
    update();
  }
}

void Qtgl::setResulthData(const QVector<double> &maxAbsValues,
                          const QVector<double> &maxValues,
                          const QVector<double> &minValues) {
  this->maxAbsValues = maxAbsValues;
  this->maxValues = maxValues;
  this->minValues = minValues;
}

void Qtgl::setResulthIndex(MainWindow *mainwindow, short index) {
  resultIndex = index;
  QString labelText;

  for (const auto &element : *elements) {
    labelText += QString("max value: %1, ").arg(element->min_values_[index]) +
                 QString("min value: %1").arg(element->max_values_[index]) +
                 "\t";
  }

  mainwindow->statusLabel->setText(labelText);

  double scaleForOutput =
      8000.0 / (*elements).first()->max_abs_values_[resultIndex];

  for (auto &element : *elements) {
    auto nodes = element->meshData_->nodes;

    for (const auto &node : nodes) {
      double value = node->outputValues[resultIndex];
      node->glOutputValue = value * scaleForOutput + node->point.z;
    }
  }

  // Нормализуем выходные данные
  normalizeOutData();

  if (m_nMesh) {
    glDeleteLists(m_nMesh, 1);
  }
  createMeshDisplayList();

  isNeedSetCoods = false;
  update();
}

void Qtgl::createMeshDisplayList() {
  if (!m_meshDataValid)
    return;

  if (m_nMesh) {
    glDeleteLists(m_nMesh, 1);
  }

  m_nMesh = glGenLists(1);
  glNewList(m_nMesh, GL_COMPILE);

  // Loop through str elements
  for (const auto element : *elements) {
    // 1. Рисуем элементы (прямоугольники)
    glColor4f(0.8f, 0.8f, 0.8f, 0.7f); // Полупрозрачный серый для элементов

    auto nodes = element->meshData_->nodes;
    auto femElements = element->meshData_->femElements;

    // Loop through fem elements
    for (const auto &element : femElements) {
      short main_nodes_count =
          ElementProvider.at(element->type).MAIN_NODES_COUNT;

      glBegin(GL_QUADS);
      for (int i = 0; i < main_nodes_count; i++) {
        const Node *node = element->nodes[i];
        glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
      }
      glEnd();

      // Рисуем контур элемента (черный)
      glBegin(GL_LINE_LOOP);
      glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
      for (int i = 0; i < main_nodes_count; i++) {
        const Node *node = element->nodes[i];
        glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
      }
      glEnd();

      // Рисуем расчетный параметр
      if (resultIndex != -1) {
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        for (int i = 0; i < element->nodesCount; i++) {
          const Node *node = element->nodes[i];
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
  m_minX = m_maxX = elements->first()->meshData_->nodes[0]->point.x;
  m_minY = m_maxY = elements->first()->meshData_->nodes[0]->point.y;
  m_minZ = m_maxZ = elements->first()->meshData_->nodes[0]->point.z;

  for (auto &element : *elements) {
    auto nodes = element->meshData_->nodes;

    if (nodes.empty())
      return;

    for (const auto node : nodes) {
      m_minX = std::min(m_minX, node->glPoint.x);
      m_maxX = std::max(m_maxX, node->glPoint.x);
      m_minY = std::min(m_minY, node->glPoint.y);
      m_maxY = std::max(m_maxY, node->glPoint.y);
      m_minZ = std::min(m_minZ, node->glPoint.z);
      m_maxZ = std::max(m_maxZ, node->glPoint.z);
    }

    m_centerX = (m_minX + m_maxX) * 0.5f;
    m_centerY = (m_minY + m_maxY) * 0.5f;
    m_centerZ = (m_minZ + m_maxZ) * 0.5f;

    // Находим максимальный размер для масштабирования
    float dx = m_maxX - m_minX;
    float dy = m_maxY - m_minY;
    float dz = m_maxZ - m_minZ;
    m_scaleFactor = std::max({dx, dy, dz});

    if (m_scaleFactor < 0.001f)
      m_scaleFactor = 1.0f;
    else {
      // Увеличиваем масштаб для лучшего отображения
      m_scaleFactor = m_scaleFactor * 1.5f;
    }
  }
}

void Qtgl::normalizeMeshData() {
  calculateMeshBounds();

  for (auto &element : *elements) {
    auto nodes = element->meshData_->nodes;

    // Центрируем и нормируем узлы
    for (auto node : nodes) {
      // if (node->isNormolize)
      //   continue;

      node->glPoint.x = (node->point.x - m_centerX) / m_scaleFactor;
      node->glPoint.y = (node->point.y - m_centerY) / m_scaleFactor;
      node->glPoint.z = (node->point.z - m_centerZ) / m_scaleFactor;

      // node->isNormolize = true;
    }
  }
}

void Qtgl::normalizeOutData() {
  for (auto &element : *elements) {
    auto nodes = element->meshData_->nodes;
    for (const auto &node : nodes) {
      // Нормализуем выходные данные так же, как и геометрию
      node->glOutputValue = (node->glOutputValue - m_centerZ) / m_scaleFactor;
    }
  }
}

void Qtgl::updateProjection() {
  // Принудительно обновляем проекцию для текущего размера окна
  resizeGL(width(), height());
}