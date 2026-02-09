#include "general_element/element.h"

class Plate : public AbstractElement {
 public:
  Plate(ElementType type, unsigned lenght, Point3 startPoint);

  Plate(ElementType type,
        unsigned lenght,
        Point3 startPoint,
        unique_ptr<Material> material,
        shared_ptr<AbstractLoad> load);

  virtual shared_ptr<AbstractLoad> createAndAddLoad() override;

  virtual void addMaterial(unique_ptr<Material> material) override;

  virtual void initElasticityMatrixies() override;
};