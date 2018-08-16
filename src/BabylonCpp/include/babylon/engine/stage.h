#ifndef BABYLON_ENGINE_STAGE_H
#define BABYLON_ENGINE_STAGE_H

#include <babylon/babylon_global.h>
#include <babylon/babylon_stl_util.h>

namespace BABYLON {

/**
 * Strong typing of a Mesh related stage step action
 */
using MeshStageAction
  = ::std::function<bool(AbstractMesh* mesh, bool hardwareInstancedRendering)>;

/**
 * Strong typing of a Evaluate Sub Mesh related stage step action
 */
using EvaluateSubMeshStageAction
  = ::std::function<void(AbstractMesh* mesh, SubMesh* subMesh)>;

/**
 * Strong typing of a Active Mesh related stage step action
 */
using ActiveMeshStageAction
  = ::std::function<void(AbstractMesh* sourceMesh, AbstractMesh* mesh)>;

/**
 * Strong typing of a Camera related stage step action
 */
using CameraStageAction = ::std::function<void(Camera* camera)>;

/**
 * Strong typing of a RenderingGroup related stage step action
 */
using RenderingGroupStageAction
  = ::std::function<void(unsigned int renderingGroupId)>;

/**
 * Strong typing of a simple stage step action
 */
using SimpleStageAction = ::std::function<void()>;

/**
 * @brief Representation of a step.
 */
template <class T>
struct BABYLON_SHARED_EXPORT Step {
  unsigned int index         = 0;
  ISceneComponent* component = nullptr;
  T action;
};

/**
 * @brief Representation of a stage in the scene (Basically a list of ordered
 * steps).
 */
template <class T>
class BABYLON_SHARED_EXPORT Stage {

public:
  Stage()
  {
  }

  ~Stage()
  {
  }

  /**
   * @brief Registers a step in an ordered way in the targeted stage.
   * @param index Defines the position to register the step in
   * @param component Defines the component attached to the step
   * @param action Defines the action to launch during the step
   */
  void registerStep(unsigned int index, ISceneComponent* component,
                    const T& action)
  {
    size_t i        = 0;
    size_t maxIndex = numeric_limits_t<size_t>::max();
    for (; i < _steps.size() && i < maxIndex; ++i) {
      const auto& step = _steps[i];
      maxIndex         = step.index;
    }
    stl_util::splice<Step<T>>(_steps, static_cast<int>(i), 0,
                              {Step<T>{index, component, action}});
  }

  /**
   * @brief Clears all the steps from the stage.
   */
  void clear()
  {
    _steps.clear();
  }

private:
  vector_t<Step<T>> _steps;

}; // end of class Stage

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_STAGE_H
