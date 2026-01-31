#include "element.h"
#include "femtypes.h"
#include "meshdata.h"
#include <memory>
#include <qobject.h>
#include <stdexcept>

AbstractElement::AbstractElement(shared_ptr<AbstractLoad> load,
                                 ElementType type, unsigned lenght) {
  this->lenght = lenght;
  this->type = type;
  loads.push_back(load);
  meshData = std::make_shared<MeshData>();
  this->name = QString("Element %1").arg(count++);
}

AbstractElement::AbstractElement(shared_ptr<AbstractLoad> load,
                                 ElementType type, unsigned lenght,
                                 Point3 startPoint)
    : AbstractElement(load, type, lenght) {
  this->statrtPoint = startPoint;
}

AbstractElement::AbstractElement(shared_ptr<AbstractLoad> load,
                                 ElementType type, unsigned lenght,
                                 Point3 startPoint,
                                 shared_ptr<Material> material)
    : AbstractElement(load, type, lenght, startPoint) {

  this->material = material;
}

double AbstractElement::getLenght() const { return lenght; }

ElementType AbstractElement::getType() const { return type; }

Point3 AbstractElement::getStartPoint() const { return statrtPoint; };

void AbstractElement::addLoad(shared_ptr<AbstractLoad> load) {
  loads.push_back(load);
}

shared_ptr<Material> AbstractElement::getMaterial() const { return material; }

inline short AbstractElement::loadCount() const { return loads.size(); }
