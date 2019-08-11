#ifndef BABYLON_MESHES_VERTEX_DATA_CONSTANTS_H
#define BABYLON_MESHES_VERTEX_DATA_CONSTANTS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT VertexDataConstants {
  /**
   * Mesh side orientation : usually the external or front surface
   */
  static constexpr unsigned int FRONTSIDE = 0;
  /**
   * Mesh side orientation : usually the internal or back surface
   */
  static constexpr unsigned int BACKSIDE = 1;
  /**
   * Mesh side orientation : both internal and external or front and back
   * surfaces
   */
  static constexpr unsigned int DOUBLESIDE = 2;
  /**
   * Mesh side orientation : by default, `FRONTSIDE`
   */
  static constexpr unsigned int DEFAULTSIDE = 0;
}; // end of struct VertexDataConstants

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_VERTEX_DATA_CONSTANTS_H
