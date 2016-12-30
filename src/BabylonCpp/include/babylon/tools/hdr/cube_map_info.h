#ifndef BABYLON_TOOLS_HDR_CUBE_MAP_INFO_H
#define BABYLON_TOOLS_HDR_CUBE_MAP_INFO_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Internals {

/**
 * CubeMap information grouping all the data for each faces as well as the
 * cubemap size.
 */
struct BABYLON_SHARED_EXPORT CubeMapInfo {
  /**
   * The pixel array for the front face.
   * This is stored in RGB, left to right, up to down format.
   */
  Float32Array front;

  /**
   * The pixel array for the back face.
   * This is stored in RGB, left to right, up to down format.
   */
  Float32Array back;

  /**
   * The pixel array for the left face.
   * This is stored in RGB, left to right, up to down format.
   */
  Float32Array left;

  /**
   * The pixel array for the right face.
   * This is stored in RGB, left to right, up to down format.
   */
  Float32Array right;

  /**
   * The pixel array for the up face.
   * This is stored in RGB, left to right, up to down format.
   */
  Float32Array up;

  /**
   * The pixel array for the down face.
   * This is stored in RGB, left to right, up to down format.
   */
  Float32Array down;

  /**
   * The size of the cubemap stored.
   *
   * Each faces will be size * size pixels.
   */
  size_t size;
}; // end of struct CubeMapInfo

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_CUBE_MAP_INFO_H