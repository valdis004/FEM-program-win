#pragma once

#include <QList>
#include <QMessageBox>
#include <QObject>
#include <memory>
#include <qcontainerfwd.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "elements/femelement.h"
#include "elements/point.h"

class AbstractElement;
using std::shared_ptr;

class Mesh : public QObject {
  Q_OBJECT
private:
  bool isEqual(const Point3 &p1, const Point3 &p2);

  unsigned maxNodeIndexInList(const QList<Node> &list);

public:
  QVector<shared_ptr<AbstractElement>> elements;

  void createDefaultMesh(shared_ptr<AbstractElement> element);

  void meshCreateManager(QVector<shared_ptr<AbstractElement>> *elements,
                         bool standartScheme = false);

  // ~Mesh();

signals:
  void progressChanged(int count);
  void meshFinished(int count);
};