#ifndef BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_WORLD_H
#define BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_WORLD_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Extensions {

struct IcoNode;
struct IRandomFunction;

class BABYLON_SHARED_EXPORT World {

public:
  World();
  ~World();

  float calculateCollidingElevation(float distanceToPlateBoundary,
                                    float distanceToPlateRoot,
                                    float boundaryElevation,
                                    float plateElevation) const;
  float calculateSuperductingElevation(float distanceToPlateBoundary,
                                       float distanceToPlateRoot,
                                       float boundaryElevation,
                                       float plateElevation,
                                       float pressure) const;
  float calculateSubductingElevation(float distanceToPlateBoundary,
                                     float distanceToPlateRoot,
                                     float boundaryElevation,
                                     float plateElevation) const;
  float calculateDivergingElevation(float distanceToPlateBoundary,
                                    float distanceToPlateRoot,
                                    float boundaryElevation,
                                    float plateElevation) const;
  float calculateShearingElevation(float distanceToPlateBoundary,
                                   float distanceToPlateRoot,
                                   float boundaryElevation,
                                   float plateElevation) const;
  float calculateDormantElevation(float distanceToPlateBoundary,
                                  float distanceToPlateRoot,
                                  float boundaryElevation,
                                  float plateElevation) const;

private:
  using RotationPredicateType = std::function<bool(
    const IcoNode&, const IcoNode&, const IcoNode&, const IcoNode&)>;

}; // end of class World

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_HEX_PLANET_GENERATION_WORLD_H
