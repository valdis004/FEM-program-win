#pragma once

#include <qcontainerfwd.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include <QList>
#include <QMessageBox>
#include <QObject>
#include <memory>

#include "fem_elements/fem_element.h"
#include "fem_elements/point.h"

class AStructuralElement;
using std::shared_ptr;

class Mesh : public QObject {
  Q_OBJECT
 private:
  bool isEqual(const Point3& p1, const Point3& p2);

  unsigned maxNodeIndexInList(const QList<Node>& list);

 public:
  QVector<shared_ptr<AStructuralElement>> elements;

  void createDefaultMesh(shared_ptr<AStructuralElement> element);

  void meshCreateManager(QVector<shared_ptr<AStructuralElement>>* elements,
                         bool standartScheme = false);

  void writeElementData(shared_ptr<AStructuralElement> element);

  // ~Mesh();

 signals:
  void progressChanged(int count);
  void meshFinished(int count);
};