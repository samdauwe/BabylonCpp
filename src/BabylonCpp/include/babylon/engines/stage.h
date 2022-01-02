#ifndef BABYLON_ENGINES_STAGE_H
#define BABYLON_ENGINES_STAGE_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

class AbstractMesh;
class Camera;
class Effect;
class ICanvas;
struct ISceneComponent;
class Mesh;
class PickingInfo;
struct PointerEvent;
class SubMesh;
FWD_STRUCT_SPTR(_InstancesBatch)
FWD_CLASS_SPTR(RenderTargetTexture)

/**
 * Strong typing of a Mesh related stage step action
 */
using MeshStageAction = std::function<bool(AbstractMesh* mesh, bool hardwareInstancedRendering)>;

/**
 * Strong typing of a Evaluate Sub Mesh related stage step action
 */
using EvaluateSubMeshStageAction = std::function<void(AbstractMesh* mesh, SubMesh* subMesh)>;

/**
 * Strong typing of a pre active Mesh related stage step action
 */
using PreActiveMeshStageAction = std::function<void(AbstractMesh* mesh)>;

/**
 * Strong typing of a Camera related stage step action
 */
using CameraStageAction = std::function<bool(Camera* camera)>;

/**
 * Strong typing of a Camera Frame buffer related stage step action
 */
using CameraStageFrameBufferAction = std::function<bool(Camera* camera)>;

/**
 * Strong typing of a Render Target related stage step action
 */
using RenderTargetStageAction
  = std::function<void(const RenderTargetTexturePtr& renderTarget, int faceIndex, int layer)>;

/**
 * Strong typing of a RenderingGroup related stage step action
 */
using RenderingGroupStageAction = std::function<void(int renderingGroupId)>;

/**
 * Strong typing of a Mesh Render related stage step action
 */
using RenderingMeshStageAction = std::function<void(
  Mesh* mesh, SubMesh* subMesh, const _InstancesBatchPtr& batch, Effect* effect)>;

/**
 * Strong typing of a simple stage step action
 */
using SimpleStageAction = std::function<void()>;

/**
 * Strong typing of a render target action.
 */
using RenderTargetsStageAction
  = std::function<void(std::vector<RenderTargetTexturePtr>& renderTargets)>;

/**
 * Strong typing of a pointer move action.
 */
using PointerMoveStageAction = std::function<std::optional<PickingInfo>(
  int unTranslatedPointerX, int unTranslatedPointerY, std::optional<PickingInfo> pickResult,
  bool isMeshPicked, ICanvas* canvas)>;

/**
 * Strong typing of a pointer up/down action.
 */
using PointerUpDownStageAction = std::function<std::optional<PickingInfo>(
  int unTranslatedPointerX, int unTranslatedPointerY, std::optional<PickingInfo> pickResult,
  const PointerEvent& evt)>;

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
  using iterator = typename std::vector<Step<T>>::iterator;

  iterator begin()
  {
    return _steps.begin();
  }

  iterator end()
  {
    return _steps.end();
  }

public:
  Stage()  = default;
  ~Stage() = default;

  std::vector<Step<T>> splice(std::vector<Step<T>>& v, int index, int howmany,
                              const std::vector<Step<T>>& itemsToAdd)
  {
    const auto start = std::min(v.end(), v.begin() + index);
    const auto end   = std::min(v.end(), start + howmany);
    std::vector<Step<T>> removedItems(start, end);
    v.erase(start, end);
    v.insert(end - static_cast<int>(removedItems.size()), itemsToAdd.begin(), itemsToAdd.end());
    return removedItems;
  }

  /**
   * @brief Registers a step in an ordered way in the targeted stage.
   * @param index Defines the position to register the step in
   * @param component Defines the component attached to the step
   * @param action Defines the action to launch during the step
   */
  void registerStep(unsigned int index, ISceneComponent* component, const T& action)
  {
    size_t i = 0;
    // Additional parenthesis below are here in order to defeat WinDef.h's "max"
    // macro.
    size_t maxIndex = (std::numeric_limits<size_t>::max)();
    for (; i < _steps.size(); ++i) {
      const auto& step = _steps[i];
      maxIndex         = step.index;
      if (index < maxIndex) {
        break;
      }
    }
    splice(_steps, static_cast<int>(i), 0, {Step<T>{index, component, action}});
  }

  /**
   * @brief Clears all the steps from the stage.
   */
  void clear()
  {
    _steps.clear();
  }

private:
  std::vector<Step<T>> _steps;

}; // end of class Stage

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_STAGE_H
