#include "structural_element.h"

#include <qobject.h>

#include "fem_types.h"

AStructuralElement::AStructuralElement(ElementType type,
                                       unsigned lenght,
                                       Point3 start_point)
    : lenght_(lenght),
      type_(type),
      statrt_point_(start_point),
      name_(QString("Element %1").arg(count_++)) {}

AStructuralElement::AStructuralElement(ElementType type,
                                       unsigned lenght,
                                       Point3 startPoint,
                                       unique_ptr<Material> material,
                                       shared_ptr<AStructuralLoad> load)
    : AStructuralElement(type, lenght, startPoint) {
  this->material_ = std::move(material);
  loads_.push_back(load);
}

double AStructuralElement::getLenght() const { return lenght_; }

ElementType AStructuralElement::getType() const { return type_; }

Point3 AStructuralElement::getStartPoint() const { return statrt_point_; };

void AStructuralElement::addLoad(shared_ptr<AStructuralLoad> load) {
  loads_.push_back(load);
}

QVector<shared_ptr<AStructuralLoad>>& AStructuralElement::getLoads() {
  return loads_;
}

QVector<shared_ptr<AStructuralDisplacement>>&
AStructuralElement::getDisplacements() {
  return displacements_;
}

Material* AStructuralElement::getMaterial() const { return material_.get(); }

inline short AStructuralElement::loadCount() const { return loads_.size(); }
