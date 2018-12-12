#ifndef BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_SETTINGS_H
#define BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_SETTINGS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Expected simplification settings.
 * Quality should be between 0 and 1 (1 being 100%, 0 being 0%)
 * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
 */
struct BABYLON_SHARED_EXPORT ISimplificationSettings {
  /**
   * Gets or sets the expected quality
   */
  float quality;
  /**
   * Gets or sets the distance when this optimized version should be used
   */
  float distance;
  /**
   * Gets an already optimized mesh
   */
  bool optimizeMesh;
}; // end of class ISimplificationSettings

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_SETTINGS_H
