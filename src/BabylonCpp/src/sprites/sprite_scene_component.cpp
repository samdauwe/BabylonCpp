#include <babylon/sprites/sprite_scene_component.h>

#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/sprites/isprite_manager.h>
#include <babylon/sprites/sprite.h>

namespace BABYLON {

SpriteSceneComponent::SpriteSceneComponent(Scene* iScene)
{
  ISceneComponent::name        = SpriteSceneComponent::name;
  scene                        = iScene;
  scene->spriteManagers        = {};
  scene->_tempSpritePickingRay = Ray::Zero();
  _spritePredicate             = [](Sprite* sprite) -> bool {
    if (!sprite->actionManager) {
      return false;
    }
    return sprite->isPickable && sprite->actionManager->hasPointerTriggers();
  };
}

SpriteSceneComponent::~SpriteSceneComponent() = default;

void SpriteSceneComponent::_register()
{
  scene->_pointerMoveStage.registerStep(
    SceneComponentConstants::STEP_POINTERMOVE_SPRITE, this,
    [this](int unTranslatedPointerX, int unTranslatedPointerY,
           const std::optional<PickingInfo>& pickResult, bool isMeshPicked,
           ICanvas* canvas) -> std::optional<PickingInfo> {
      return _pointerMove(unTranslatedPointerX, unTranslatedPointerY, pickResult, isMeshPicked,
                          canvas);
    });
  scene->_pointerDownStage.registerStep(
    SceneComponentConstants::STEP_POINTERDOWN_SPRITE, this,
    [this](int unTranslatedPointerX, int unTranslatedPointerY,
           const std::optional<PickingInfo>& pickResult,
           const PointerEvent& evt) -> std::optional<PickingInfo> {
      return _pointerDown(unTranslatedPointerX, unTranslatedPointerY, pickResult, evt);
    });
  scene->_pointerUpStage.registerStep(
    SceneComponentConstants::STEP_POINTERUP_SPRITE, this,
    [this](int unTranslatedPointerX, int unTranslatedPointerY,
           const std::optional<PickingInfo>& pickResult,
           const PointerEvent& evt) -> std::optional<PickingInfo> {
      return _pointerUp(unTranslatedPointerX, unTranslatedPointerY, pickResult, evt);
    });
}

void SpriteSceneComponent::rebuild()
{
  /** Nothing to do for sprites */
}

void SpriteSceneComponent::dispose()
{
  scene->onBeforeSpritesRenderingObservable.clear();
  scene->onAfterSpritesRenderingObservable.clear();

  for (const auto& spriteManager : scene->spriteManagers) {
    spriteManager->dispose();
  }
}

std::optional<PickingInfo>
SpriteSceneComponent::_pickSpriteButKeepRay(const std::optional<PickingInfo>& originalPointerInfo,
                                            int x, int y, bool fastCheck, const CameraPtr& camera)
{
  auto result = scene->pickSprite(x, y, _spritePredicate, fastCheck, camera);
  if (result.has_value()) {
    result->ray = originalPointerInfo ? originalPointerInfo->ray : std::nullopt;
  }
  return result;
}

std::optional<PickingInfo> SpriteSceneComponent::_pointerMove(int unTranslatedPointerX,
                                                              int unTranslatedPointerY,
                                                              std::optional<PickingInfo> pickResult,
                                                              bool isMeshPicked, ICanvas* element)
{
  if (isMeshPicked) {
    scene->setPointerOverSprite(nullptr);
  }
  else {
    pickResult = _pickSpriteButKeepRay(pickResult, unTranslatedPointerX, unTranslatedPointerY,
                                       false, scene->cameraToUseForPointers);

    if (pickResult && pickResult->hit && pickResult->pickedSprite) {
      scene->setPointerOverSprite(pickResult->pickedSprite);
      if (!scene->doNotHandleCursors) {
        if (scene->_pointerOverSprite && scene->_pointerOverSprite->actionManager
            && !scene->_pointerOverSprite->actionManager->hoverCursor.empty()) {
          element->style.cursor = scene->_pointerOverSprite->actionManager->hoverCursor;
        }
        else {
          element->style.cursor = scene->hoverCursor;
        }
      }
    }
    else {
      scene->setPointerOverSprite(nullptr);
    }
  }

  return pickResult;
}

std::optional<PickingInfo> SpriteSceneComponent::_pointerDown(int unTranslatedPointerX,
                                                              int unTranslatedPointerY,
                                                              std::optional<PickingInfo> pickResult,
                                                              const PointerEvent& evt)
{
  scene->_pickedDownSprite = nullptr;
  if (!scene->spriteManagers.empty()) {
    pickResult = scene->pickSprite(unTranslatedPointerX, unTranslatedPointerY, _spritePredicate,
                                   false, scene->cameraToUseForPointers);

    if (pickResult && pickResult->hit && pickResult->pickedSprite) {
      if (pickResult->pickedSprite->actionManager) {
        scene->_pickedDownSprite = pickResult->pickedSprite;
        switch (evt.button) {
          case MouseButtonType::LEFT:
            pickResult->pickedSprite->actionManager->processTrigger(
              Constants::ACTION_OnLeftPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, scene, evt));
            break;
          case MouseButtonType::MIDDLE:
            pickResult->pickedSprite->actionManager->processTrigger(
              Constants::ACTION_OnCenterPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, scene, evt));
            break;
          case MouseButtonType::RIGHT:
            pickResult->pickedSprite->actionManager->processTrigger(
              Constants::ACTION_OnRightPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, scene, evt));
            break;
          default:
            break;
        }
        if (pickResult->pickedSprite->actionManager) {
          pickResult->pickedSprite->actionManager->processTrigger(
            Constants::ACTION_OnPickDownTrigger,
            ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, scene, evt));
        }
      }
    }
  }

  return pickResult;
}

std::optional<PickingInfo> SpriteSceneComponent::_pointerUp(int unTranslatedPointerX,
                                                            int unTranslatedPointerY,
                                                            std::optional<PickingInfo> pickResult,
                                                            const PointerEvent& evt)
{
  if (!scene->spriteManagers.empty()) {
    auto spritePickResult
      = scene->pickSprite(unTranslatedPointerX, unTranslatedPointerY, _spritePredicate, false,
                          scene->cameraToUseForPointers);

    if (spritePickResult) {
      if (spritePickResult->hit && spritePickResult->pickedSprite) {
        if (spritePickResult->pickedSprite->actionManager) {
          spritePickResult->pickedSprite->actionManager->processTrigger(
            Constants::ACTION_OnPickUpTrigger,
            ActionEvent::CreateNewFromSprite(spritePickResult->pickedSprite, scene, evt));
          if (spritePickResult->pickedSprite->actionManager) {
            /*if (!scene->_isPointerSwiping()) {
              spritePickResult->pickedSprite->actionManager->processTrigger(
                Constants::ACTION_OnPickTrigger,
                ActionEvent::CreateNewFromSprite(spritePickResult->pickedSprite,
                                                 scene, evt));
            }*/
          }
        }
      }
      if (scene->_pickedDownSprite && scene->_pickedDownSprite->actionManager
          && scene->_pickedDownSprite != spritePickResult->pickedSprite) {
        scene->_pickedDownSprite->actionManager->processTrigger(
          Constants::ACTION_OnPickOutTrigger,
          ActionEvent::CreateNewFromSprite(scene->_pickedDownSprite, scene, evt));
      }
    }
  }

  return pickResult;
}

} // end of namespace BABYLON
