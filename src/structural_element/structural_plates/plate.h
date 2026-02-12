#include "structural_element/structural_element.h"

class Plate : public AStructuralElement {
 public:
  Plate(ElementType type, unsigned lenght, Point3 startPoint);

  Plate(ElementType type,
        unsigned lenght,
        Point3 startPoint,
        unique_ptr<Material> material,
        shared_ptr<AStructuralLoad> load);

  virtual shared_ptr<AStructuralLoad> createAndAddLoad() override;

  virtual void addMaterial(unique_ptr<Material> material) override;

  virtual void initElasticityMatrixies() override;
};