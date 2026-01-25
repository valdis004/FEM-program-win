#include "generalElement/element.h"

class Plate : public AbstractElement {
public:
  Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght);

  virtual shared_ptr<AbstractLoad> createAndAddLoad() override;
};