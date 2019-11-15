#ifndef BABYLON_SPRITES_SPRITE_SCENE_COMPONENT_H
#define BABYLON_SPRITES_SPRITE_SCENE_COMPONENT_H

#include <functional>
#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>

namespace BABYLON {

class Camera;
class ICanvas;
class PickingInfo;
struct PointerEvent;
class Sprite;
class SpriteSceneComponent;
using CameraPtr               = std::shared_ptr<Camera>;
using SpriteSceneComponentPtr = std::shared_ptr<SpriteSceneComponent>;

/**
 * @brief Defines the sprite scene component responsible to manage sprites.
 * in a given scene.
 */
class BABYLON_SHARED_EXPORT SpriteSceneComponent : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_SPRITE;

public:
  template <typename... Ts>
  static SpriteSceneComponentPtr New(Ts&&... args)
  {
    return std::shared_ptr<SpriteSceneComponent>(
      new SpriteSceneComponent(std::forward<Ts>(args)...));
  }
  virtual ~SpriteSceneComponent(); // = default

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  SpriteSceneComponent(Scene* scene);

private:
  std::optional<PickingInfo>
  _pickSpriteButKeepRay(const std::optional<PickingInfo>& originalPointerInfo,
                        int x, int y, bool fastCheck = false,
                        const CameraPtr& camera = nullptr);
  std::optional<PickingInfo> _pointerMove(int unTranslatedPointerX,
                                          int unTranslatedPointerY,
                                          std::optional<PickingInfo> pickResult,
                                          bool isMeshPicked, ICanvas* canvas);
  std::optional<PickingInfo> _pointerDown(int unTranslatedPointerX,
                                          int unTranslatedPointerY,
                                          std::optional<PickingInfo> pickResult,
                                          const PointerEvent& evt);
  std::optional<PickingInfo> _pointerUp(int unTranslatedPointerX,
                                        int unTranslatedPointerY,
                                        std::optional<PickingInfo> pickResult,
                                        const PointerEvent& evt);

private:
  /** Hidden */
  std::function<bool(Sprite* sprite)> _spritePredicate;

}; // end of class SpriteSceneComponent

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_SCENE_COMPONENT_H
