#include <QOpenGLFunctions>
#include <QtGui>
#include <qglobal.h>
#include <qmainwindow.h>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qpoint.h>

#include "app/mainwindow.h"
#include "qtgl.h"

Qtgl::Qtgl(QWidget *pwgt /*= 0*/)
    : QOpenGLWidget(pwgt), m_xRotate(0), m_yRotate(0) {}

/*virtual*/ void Qtgl::initializeGL() {
  QOpenGLFunctions *pFunc = QOpenGLContext::currentContext()->functions();
  pFunc->glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

  pFunc->glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  createMeshDisplayList();
}

/*virtual*/ void Qtgl::resizeGL(int nWidth, int nHeight) {
  glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-0.2, 0.2, -0.2, 0.2, 0.2, 20.0);
}

/*virtual*/ void Qtgl::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(m_ptPositionDelta.rx(), -m_ptPositionDelta.ry(), -4.0 * m_scale);

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
    m_scale /= 1.1f;
  else
    m_scale *= 1.1f;
  update();
}

/*virtual*/ void Qtgl::mouseMoveEvent(QMouseEvent *pe) {
  if (isLeftBut) {
    m_xRotate += 180 * (GLfloat)(pe->pos().y() - m_ptPosition.y()) / height();
    m_yRotate += 180 * (GLfloat)(pe->pos().x() - m_ptPosition.x()) / width();
    update();
  }

  if (isMiddleBut) {
    m_ptPositionDelta += (pe->position() - m_ptPositionOld) * 0.005;
    m_ptPositionOld = pe->pos();
    update();
  }
  m_ptPosition = pe->pos();
  // m_ptPositionDelta = {0, 0};
}

void Qtgl::setMeshData(QVector<shared_ptr<AbstractElement>> *elements) {
  this->elements = elements;

  for (auto &element : *elements) {
    auto nodes = element->meshData->nodes;
    auto elements = element->meshData->femElements;

    m_meshDataValid = !nodes.empty() && !elements.empty();
    if (!m_meshDataValid)
      break;
  }

  if (m_meshDataValid) {
    // Нормируем данные для лучшего отображения
    normalizeMeshData();

    // Пересоздаем дисплейный список
    if (m_nMesh) {
      glDeleteLists(m_nMesh, 1);
    }
    createMeshDisplayList();
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
  mainwindow->statusLabel->setText(
      QString("max value: %1, ").arg(maxValues[index]) +
      QString("min value: %1").arg(minValues[index]));

  double scaleForOutput = 1000.0 / maxAbsValues[resultIndex];

  for (auto &element : *elements) {
    auto nodes = element->meshData->nodes;

    for (const auto &node : nodes) {
      double value = node->outputValues[resultIndex];
      node->glOutputValue = value * scaleForOutput + node->point.z;
    }
  }

  normalizeOutData();

  if (m_nMesh) {
    glDeleteLists(m_nMesh, 1);
  }
  createMeshDisplayList();

  // if (!mainwindow->resultsView)
  //   emit needOutputTableDock(m_nodes, isNeedSetCoods);

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

  // 1. Рисуем элементы (прямоугольники)
  glColor4f(0.8f, 0.8f, 0.8f, 0.7f); // Полупрозрачный серый для элементов

  for (const auto element : *elements) {
    auto nodes = element->meshData->nodes;
    auto femElements = element->meshData->femElements;

    for (const auto &element : femElements) {
      glBegin(GL_QUADS);
      // Рисуем прямоугольник по 4 узлам
      for (int i = 0; i < element->nodesCount; i++) {
        const Node *node = element->nodes[i];
        glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
      }
      glEnd();

      // Рисуем контур элемента (черный)
      glBegin(GL_LINE_LOOP);
      glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
      for (int i = 0; i < element->nodesCount; i++) {
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

      glColor4f(0.8f, 0.8f, 0.8f,
                0.7f); // Возвращаем цвет для следующих элементов
    }

    // 2. Рисуем узлы (красные точки)
    glPointSize(7.0f);
    glBegin(GL_POINTS);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

    for (const auto node : nodes) {
      glVertex3f(node->glPoint.x, node->glPoint.z, node->glPoint.y);
    }
    glEnd();

    glEndList();
  }
}

void Qtgl::calculateMeshBounds() {
  for (auto &element : *elements) {
    auto nodes = element->meshData->nodes;

    if (nodes.empty())
      return;

    m_minX = m_maxX = nodes[0]->point.x;
    m_minY = m_maxY = nodes[0]->point.y;
    m_minZ = m_maxZ = nodes[0]->point.z;

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
  }
}

void Qtgl::normalizeMeshData() {
  calculateMeshBounds();

  for (auto &element : *elements) {
    auto nodes = element->meshData->nodes;
    auto elements = element->meshData->femElements;

    // Центрируем и нормируем узлы
    for (auto node : nodes) {
      node->glPoint.x = (node->glPoint.x - m_centerX) / m_scaleFactor;
      node->glPoint.y = (node->glPoint.y - m_centerY) / m_scaleFactor;
      node->glPoint.z = (node->glPoint.z - m_centerZ) / m_scaleFactor;

      // node->glOutputValue = (node->glOutputValue - m_centerZ) /
      // m_scaleFactor;
    }
  }
}

void Qtgl::normalizeOutData() {
  for (auto &element : *elements) {
    auto nodes = element->meshData->nodes;
    for (const auto &node : nodes) {
      node->glOutputValue = (node->glOutputValue - m_centerZ) / m_scaleFactor;
    }
  }
}
