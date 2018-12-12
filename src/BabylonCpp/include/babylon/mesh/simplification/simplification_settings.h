#ifndef BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_SETTINGS_H
#define BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_SETTINGS_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/simplification/isimplification_settings.h>

namespace BABYLON {

/**
 * @brief Class used to specify simplification options.
 * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
 */
class BABYLON_SHARED_EXPORT SimplificationSettings
    : public ISimplificationSettings {

public:
  /**
   * @brief Creates a SimplificationSettings.
   * @param quality expected quality
   * @param distance distance when this optimized version should be used
   * @param optimizeMesh already optimized mesh
   */
  SimplificationSettings(float quality, float distance, bool optimizeMesh);
  ~SimplificationSettings();

}; // end of class ISimplificationSettings

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_SETTINGS_H
