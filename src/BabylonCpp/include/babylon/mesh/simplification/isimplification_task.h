#ifndef BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H
#define BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H

#include <functional>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_enums.h>
#include <babylon/mesh/simplification/simplification_settings.h>

namespace BABYLON {

class Mesh;

/**
 * @brief Interface used to define a simplification task.
 */
struct BABYLON_SHARED_EXPORT ISimplificationTask {
  /**
   * Array of settings
   */
  std::vector<ISimplificationSettings> settings;
  /**
   * Simplification type
   */
  SimplificationType simplificationType;
  /**
   * Mesh to simplify
   */
  Mesh* mesh;
  /**
   * Callback called on success
   */
  std::function<void()> successCallback;
  /**
   * Defines if parallel processing can be used
   */
  bool parallelProcessing;
}; // end of class ISimplificationTask

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H
