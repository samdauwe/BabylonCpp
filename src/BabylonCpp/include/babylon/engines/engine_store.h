#ifndef BABYLON_ENGINES_ENGINE_STORE_H
#define BABYLON_ENGINES_ENGINE_STORE_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Engine;
class Scene;
using EnginePtr = std::shared_ptr<Engine>;
using ScenePtr  = std::shared_ptr<Scene>;

/**
 * @brief The engine store class is responsible to hold all the instances of
 * Engine and Scene created during the life time of the application.
 */
struct BABYLON_SHARED_EXPORT EngineStore {

  /** Gets the list of created engines */
  static std::vector<EnginePtr> Instances;

  /** Hidden */
  static ScenePtr _LastCreatedScene;

  /**
   * @brief Gets the latest created engine.
   */
  static EnginePtr LastCreatedEngine();

  /**
   * @brief Gets the latest created scene.
   */
  static ScenePtr LastCreatedScene();

}; // end of struct EngineStore

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_ENGINE_STORE_H
