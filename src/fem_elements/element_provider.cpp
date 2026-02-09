// #include "displacement/femdisplacement.h"
// #include "elements/load/femload.h"
// #include "elements/node.h"
// #include "elements/point.h"
// #include "femtypes.h"
// // #include "generalElement/displacement/displacement.h"
// // #include "generalElement/load/load.h"
#include "fem_elements/element_provider.h"
#include "fem_plate_mitc4my.h"
#include "fem_plates/fem_plate_dkmq.h"
#include "fem_plates/fem_plate_mitc9my.h"

const std::unordered_map<ElementType, ElementData> ElementProvider = {
    {ElementType::MITC4MY, FemPlateMitc4My::DATA},
    {ElementType::DKMQ, FemPlateDkmq::DATA},
    {ElementType::MITC9MY, FemPlateMitc9My::DATA},
};
