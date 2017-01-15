#include <babylon/extensions/hexplanetgeneration/world.h>

namespace BABYLON {
namespace Extensions {

float World::calculateCollidingElevation(float distanceToPlateBoundary,
                                         float distanceToPlateRoot,
                                         float boundaryElevation,
                                         float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.5f) {
    t = t / 0.5f;
    return plateElevation
           + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateSuperductingElevation(float distanceToPlateBoundary,
                                            float distanceToPlateRoot,
                                            float boundaryElevation,
                                            float plateElevation,
                                            float pressure) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.2f) {
    t = t / 0.2f;
    return boundaryElevation
           + t * (plateElevation - boundaryElevation + pressure / 2.f);
  }
  else if (t < 0.5f) {
    t = (t - 0.2f) / 0.3f;
    return plateElevation + std::pow(t - 1.f, 2.f) * pressure / 2.f;
  }
  else {
    return plateElevation;
  }
}

float World::calculateSubductingElevation(float distanceToPlateBoundary,
                                          float distanceToPlateRoot,
                                          float boundaryElevation,
                                          float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  return plateElevation
         + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
}

float World::calculateDivergingElevation(float distanceToPlateBoundary,
                                         float distanceToPlateRoot,
                                         float boundaryElevation,
                                         float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.3f) {
    t = t / 0.3f;
    return plateElevation
           + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateShearingElevation(float distanceToPlateBoundary,
                                        float distanceToPlateRoot,
                                        float boundaryElevation,
                                        float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  if (t < 0.2f) {
    t = t / 0.2f;
    return plateElevation
           + std::pow(t - 1.f, 2.f) * (boundaryElevation - plateElevation);
  }
  else {
    return plateElevation;
  }
}

float World::calculateDormantElevation(float distanceToPlateBoundary,
                                       float distanceToPlateRoot,
                                       float boundaryElevation,
                                       float plateElevation) const
{
  float t
    = distanceToPlateBoundary / (distanceToPlateBoundary + distanceToPlateRoot);
  float elevationDifference = boundaryElevation - plateElevation;
  return t * t * elevationDifference * (2.f * t - 3.f) + boundaryElevation;
}

} // end of namespace Extensions
} // end of namespace BABYLON
