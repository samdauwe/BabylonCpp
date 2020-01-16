#ifndef BABYLON_PARTICLES_POINTS_GROUP_H
#define BABYLON_PARTICLES_POINTS_GROUP_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/array_buffer_view.h>

namespace BABYLON {

class CloudPoint;

/**
 * @brief Represents a group of points in a points cloud system
 *  * PCS internal tool, don't use it manually.
 */
class BABYLON_SHARED_EXPORT PointsGroup {

public:
  /**
   * @brief Creates a points group object. This is an internal reference to produce particles for
   * the PCS. PCS internal tool, don't use it manually.
   * @hidden
   */
  PointsGroup(size_t id,
              const std::function<void(CloudPoint* particle, int i, int s)>& posFunction);
  ~PointsGroup(); // default

public:
  /**
   * The group id
   * @hidden
   */
  size_t groupID;

  /**
   * image data for group (internal use)
   * @hidden
   */
  ArrayBufferView _groupImageData;

  /**
   * Image Width (internal use)
   * @hidden
   */
  size_t _groupImgWidth;

  /**
   * Image Height (internal use)
   * @hidden
   */
  size_t _groupImgHeight;

  /**
   * Custom position function (internal use)
   * @hidden
   */
  std::function<void(CloudPoint* particle, int i, int s)> _positionFunction;

  /**
   * Density per facet for surface points
   * @hidden
   */
  Float32Array _groupDensity;

  /**
   * Only when points are colored by texture carries pointer to texture list array
   * @hidden
   */
  size_t _textureNb;

}; // end of class PointsGroup

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_PARTICLE_H
