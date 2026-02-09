#include "element.h"

#include <qobject.h>

#include "fem_types.h"

AbstractElement::AbstractElement(ElementType type,
                                 unsigned lenght,
                                 Point3 start_point)
    : lenght_(lenght),
      type_(type),
      statrt_point_(start_point),
      name_(QString("Element %1").arg(count_++)) {}

AbstractElement::AbstractElement(ElementType type,
                                 unsigned lenght,
                                 Point3 startPoint,
                                 unique_ptr<Material> material,
                                 shared_ptr<AbstractLoad> load)
    : AbstractElement(type, lenght, startPoint) {
  this->material_ = std::move(material);
  loads_.push_back(load);
}

double AbstractElement::getLenght() const { return lenght_; }

ElementType AbstractElement::getType() const { return type_; }

Point3 AbstractElement::getStartPoint() const { return statrt_point_; };

void AbstractElement::addLoad(shared_ptr<AbstractLoad> load) {
  loads_.push_back(load);
}

Material* AbstractElement::getMaterial() const { return material_.get(); }

inline short AbstractElement::loadCount() const { return loads_.size(); }
