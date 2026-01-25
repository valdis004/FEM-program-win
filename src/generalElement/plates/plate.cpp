#include <memory>

#include "femtypes.h"
#include "load/load.h"
#include "plate.h"

Plate::Plate(shared_ptr<AbstractLoad> load, ElementType type, unsigned lenght)
    : AbstractElement(load, type, lenght) {}

/* virtual */ shared_ptr<AbstractLoad> Plate::createAndAddLoad() {
  shared_ptr<AreaLoadFzMxMy> load = std::make_shared<AreaLoadFzMxMy>();
  addLoad(load);
  return load;
}
