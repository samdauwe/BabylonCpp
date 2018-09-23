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
 * @brief
 */
struct BABYLON_SHARED_EXPORT ISimplificationTask {
  std::vector<ISimplificationSettings> settings;
  SimplificationType simplificationType;
  Mesh* mesh;
  std::function<void()> successCallback;
  bool parallelProcessing;
}; // end of class ISimplificationTask

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H
