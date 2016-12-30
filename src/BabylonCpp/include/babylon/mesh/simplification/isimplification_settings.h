#ifndef BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_SETTINGS_H
#define BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_SETTINGS_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Expected simplification settings.
 * Quality should be between 0 and 1 (1 being 100%, 0 being 0%);
 */
struct BABYLON_SHARED_EXPORT ISimplificationSettings {
  float quality;
  float distance;
  bool optimizeMesh;
}; // end of class ISimplificationSettings

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_SETTINGS_H
