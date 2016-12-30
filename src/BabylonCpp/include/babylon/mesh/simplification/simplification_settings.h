#ifndef BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_SETTINGS_H
#define BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_SETTINGS_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/simplification/isimplification_settings.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT SimplificationSettings
  : public ISimplificationSettings {

public:
  SimplificationSettings(float quality, float distance, bool optimizeMesh);
  ~SimplificationSettings();

}; // end of class ISimplificationSettings

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_SIMPLIFICATION_SETTINGS_H
