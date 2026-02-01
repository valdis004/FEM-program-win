#include "element.h"
#include "femtypes.h"
#include "meshdata.h"
#include <memory>
#include <qobject.h>

AbstractElement::AbstractElement(shared_ptr<AbstractLoad> load,
                                 ElementType type, unsigned lenght) {
  this->lenght_ = lenght;
  this->type_ = type;
  loads_.push_back(load);
  meshData_ = std::make_shared<MeshData>();
  this->name_ = QString("Element %1").arg(count_++);
}

AbstractElement::AbstractElement(shared_ptr<AbstractLoad> load,
                                 ElementType type, unsigned lenght,
                                 Point3 startPoint)
    : AbstractElement(load, type, lenght) {
  this->statrt_point_ = startPoint;
}

AbstractElement::AbstractElement(shared_ptr<AbstractLoad> load,
                                 ElementType type, unsigned lenght,
                                 Point3 startPoint,
                                 shared_ptr<Material> material)
    : AbstractElement(load, type, lenght, startPoint) {

  this->material_ = material;
}

double AbstractElement::getLenght() const { return lenght_; }

ElementType AbstractElement::getType() const { return type_; }

Point3 AbstractElement::getStartPoint() const { return statrt_point_; };

void AbstractElement::addLoad(shared_ptr<AbstractLoad> load) {
  loads_.push_back(load);
}

shared_ptr<Material> AbstractElement::getMaterial() const { return material_; }

inline short AbstractElement::loadCount() const { return loads_.size(); }
