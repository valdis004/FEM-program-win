#pragma once

#include <QList>
#include <QMessageBox>
#include <QObject>
#include <memory>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>

#include "elements/femelement.h"
#include "elements/femtypes.h"
#include "elements/point.h"

class AbstractElement;
using std::shared_ptr;

class Mesh : public QObject {
  Q_OBJECT
public:
  QVector<shared_ptr<AbstractElement>> elements;

private:
  bool isEqual(const Point3 &p1, const Point3 &p2);

  unsigned maxNodeIndexInList(const QList<Node> &list);

public:
  void createDefaultMesh(shared_ptr<AbstractElement> element);

  void meshCreateManager(QVector<shared_ptr<AbstractElement>> *elements,
                         bool standartScheme = false);

  // ~Mesh();

signals:
  void progressChanged(int count);
  void meshFinished(int count);
};