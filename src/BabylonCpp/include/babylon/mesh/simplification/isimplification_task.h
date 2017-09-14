#ifndef BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H
#define BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/simplification/simplification_settings.h>

namespace BABYLON {

/**
 * @brief
 */
struct BABYLON_SHARED_EXPORT ISimplificationTask {
  vector_t<ISimplificationSettings> settings;
  SimplificationType simplificationType;
  Mesh* mesh;
  ::std::function<void()> successCallback;
  bool parallelProcessing;
}; // end of class ISimplificationTask

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_SIMPLIFICATION_ISIMPLIFICATION_TASK_H
