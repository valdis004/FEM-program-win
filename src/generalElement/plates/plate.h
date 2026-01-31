#include "generalElement/element.h"

class Plate : public AbstractElement {
public:
  Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght);

  Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght,
        Point3 startPoint);

  Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght,
        Point3 startPoint, shared_ptr<Material> material);

  virtual shared_ptr<AbstractLoad> createAndAddLoad() override;

  virtual void initElasticityMatrixies() override;
};