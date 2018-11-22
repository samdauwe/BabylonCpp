#include <babylon/rendering/utility_layer_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

std::shared_ptr<UtilityLayerRenderer> UtilityLayerRenderer::_DefaultUtilityLayer
  = nullptr;
std::shared_ptr<UtilityLayerRenderer>
  UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer = nullptr;

std::shared_ptr<UtilityLayerRenderer>&
UtilityLayerRenderer::DefaultUtilityLayer()
{
  if (UtilityLayerRenderer::_DefaultUtilityLayer == nullptr) {
    UtilityLayerRenderer::_DefaultUtilityLayer
      = UtilityLayerRenderer::New(Engine::LastCreatedScene());
    UtilityLayerRenderer::_DefaultUtilityLayer->originalScene
      ->onDisposeObservable.addOnce([](Scene* /*scene*/, EventState& /*es*/) {
        UtilityLayerRenderer::_DefaultUtilityLayer = nullptr;
      });
  }
  return UtilityLayerRenderer::_DefaultUtilityLayer;
}

std::shared_ptr<UtilityLayerRenderer>&
UtilityLayerRenderer::DefaultKeepDepthUtilityLayer()
{
  if (UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer == nullptr) {
    UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer
      = UtilityLayerRenderer::New(Engine::LastCreatedScene());
    UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer->utilityLayerScene
      ->autoClearDepthAndStencil
      = false;
    UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer->originalScene
      ->onDisposeObservable.addOnce([](Scene* /*scene*/, EventState& /*es*/) {
        UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer = nullptr;
      });
  }
  return UtilityLayerRenderer::_DefaultKeepDepthUtilityLayer;
}

UtilityLayerRenderer::UtilityLayerRenderer(Scene* iOriginalScene)
    : utilityLayerScene{nullptr}
    , shouldRender{true}
    , onlyCheckPointerDownEvents{true}
    , processAllEvents{false}
    , originalScene{iOriginalScene}
    , mainSceneTrackerPredicate{nullptr}
    , _afterRenderObserver{nullptr}
    , _sceneDisposeObserver{nullptr}
    , _originalPointerObserver{nullptr}
{
  // Create scene which will be rendered in the foreground and remove it from
  // being referenced by engine to avoid interfering with existing app
  utilityLayerScene = Scene::New(iOriginalScene->getEngine());
  utilityLayerScene->useRightHandedSystem
    = originalScene->useRightHandedSystem();
  utilityLayerScene->_allowPostProcessClearColor = false;
  iOriginalScene->getEngine()->scenes.pop_back();

  // Detach controls on utility scene, events will be fired by logic below to
  // handle picking priority
  utilityLayerScene->detachControl();
  _originalPointerObserver = originalScene->onPrePointerObservable.add(
    [&](PointerInfoPre* prePointerInfo, EventState& /*es*/) {
      if (!processAllEvents) {
        if (prePointerInfo->type != PointerEventTypes::POINTERMOVE
            && prePointerInfo->type != PointerEventTypes::POINTERUP
            && prePointerInfo->type != PointerEventTypes::POINTERDOWN) {
          return;
        }
      }

      auto pointerEvent = prePointerInfo->pointerEvent;
      if (originalScene->isPointerCaptured(pointerEvent.pointerId)) {
        _pointerCaptures[pointerEvent.pointerId] = false;
        return;
      }

      auto utilityScenePick
        = prePointerInfo->ray ?
            utilityLayerScene->pickWithRay(*prePointerInfo->ray) :
            utilityLayerScene->pick(originalScene->pointerX(),
                                    originalScene->pointerY());
      if (!prePointerInfo->ray && utilityScenePick) {
        prePointerInfo->ray = (*utilityScenePick).ray;
      }

      // always fire the prepointer oversvable
      utilityLayerScene->onPrePointerObservable.notifyObservers(prePointerInfo);

      // allow every non pointer down event to flow to the utility layer
      if (onlyCheckPointerDownEvents
          && prePointerInfo->type != PointerEventTypes::POINTERDOWN) {
        if (!prePointerInfo->skipOnPointerObservable) {
          PointerInfo pointerInfo(prePointerInfo->type,
                                  prePointerInfo->pointerEvent,
                                  *utilityScenePick);
          utilityLayerScene->onPointerObservable.notifyObservers(&pointerInfo);
        }
        if (prePointerInfo->type == PointerEventTypes::POINTERUP
            && stl_util::contains(_pointerCaptures, pointerEvent.pointerId)
            && _pointerCaptures[pointerEvent.pointerId]) {
          _pointerCaptures[pointerEvent.pointerId] = false;
        }
        return;
      }

      if (utilityLayerScene->autoClearDepthAndStencil) {
        // If this layer is an overlay, check if this layer was hit and if so,
        // skip pointer events for the main scene
        if (utilityScenePick && (*utilityScenePick).hit) {

          if (!prePointerInfo->skipOnPointerObservable) {
            PointerInfo pointerInfo(prePointerInfo->type,
                                    prePointerInfo->pointerEvent,
                                    *utilityScenePick);
            utilityLayerScene->onPointerObservable.notifyObservers(
              &pointerInfo);
          }
          prePointerInfo->skipOnPointerObservable = true;
        }
      }
      else {
        auto originalScenePick
          = prePointerInfo->ray ?
              originalScene->pickWithRay(*prePointerInfo->ray) :
              originalScene->pick(originalScene->pointerX(),
                                  originalScene->pointerY());
        auto pointerEvent = prePointerInfo->pointerEvent;

        // If the layer can be occluded by the original scene, only fire pointer
        // events to the first layer that hit they ray
        if (originalScenePick && utilityScenePick) {

          // No pick in utility scene
          if ((*utilityScenePick).distance == 0.f
              && (*utilityScenePick).pickedMesh) {
            if (mainSceneTrackerPredicate
                && mainSceneTrackerPredicate((*originalScenePick).pickedMesh)) {
              // We touched an utility mesh present in the main scene
              _notifyObservers(*prePointerInfo, *originalScenePick,
                               pointerEvent);
              prePointerInfo->skipOnPointerObservable = true;
            }
            else if (prePointerInfo->type == PointerEventTypes::POINTERDOWN) {
              _pointerCaptures[pointerEvent.pointerId] = true;
            }
            else if (stl_util::contains(_lastPointerEvents,
                                        pointerEvent.pointerId)
                     && _lastPointerEvents[pointerEvent.pointerId]) {
              // We need to send a last pointerup to the utilityLayerScene to
              // make sure animations can complete
              onPointerOutObservable.notifyObservers(&pointerEvent.pointerId);
              _lastPointerEvents.erase(pointerEvent.pointerId);
            }
          }
          else if (stl_util::contains(_pointerCaptures, pointerEvent.pointerId)
                   && !_pointerCaptures[pointerEvent.pointerId]
                   && ((*utilityScenePick).distance
                         < (*originalScenePick).distance
                       || (*originalScenePick).distance == 0.f)) {
            // We pick something in utility scene or the pick in utility is
            // closer than the one in main scene
            _notifyObservers(*prePointerInfo, *utilityScenePick, pointerEvent);
            // If a previous utility layer set this, do not unset this
            if (!prePointerInfo->skipOnPointerObservable) {
              prePointerInfo->skipOnPointerObservable
                = (*utilityScenePick).distance > 0.f;
            }
          }
          else if (stl_util::contains(_pointerCaptures, pointerEvent.pointerId)
                   && !_pointerCaptures[pointerEvent.pointerId]
                   && ((*utilityScenePick).distance
                       > (*originalScenePick).distance)) {
            // We have a pick in both scenes but main is closer than utility

            // We touched an utility mesh present in the main scene
            if (mainSceneTrackerPredicate
                && mainSceneTrackerPredicate((*originalScenePick).pickedMesh)) {
              _notifyObservers(*prePointerInfo, *originalScenePick,
                               pointerEvent);
              prePointerInfo->skipOnPointerObservable = true;
            }
            else if (stl_util::contains(_lastPointerEvents,
                                        pointerEvent.pointerId)) {
              // We need to send a last pointerup to the utilityLayerScene to
              // make sure animations can complete
              onPointerOutObservable.notifyObservers(&pointerEvent.pointerId);
              _lastPointerEvents.erase(pointerEvent.pointerId);
            }
          }

          if (prePointerInfo->type == PointerEventTypes::POINTERUP
              && stl_util::contains(_pointerCaptures, pointerEvent.pointerId)
              && _pointerCaptures[pointerEvent.pointerId]) {
            _pointerCaptures[pointerEvent.pointerId] = false;
          }
        }
      }
    });

  // Render directly on top of existing scene without clearing
  utilityLayerScene->autoClear = false;

  _afterRenderObserver = originalScene->onAfterRenderObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) {
      if (shouldRender) {
        render();
      }
    });

  _sceneDisposeObserver = originalScene->onDisposeObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) { dispose(); });

  _updateCamera();
}

UtilityLayerRenderer::~UtilityLayerRenderer()
{
}

void UtilityLayerRenderer::_notifyObservers(
  const PointerInfoPre& prePointerInfo, const PickingInfo& pickInfo,
  const PointerEvent& pointerEvent)
{
  if (!prePointerInfo.skipOnPointerObservable) {
    PointerInfo pointerInfo(prePointerInfo.type, prePointerInfo.pointerEvent,
                            pickInfo);
    utilityLayerScene->onPointerObservable.notifyObservers(&pointerInfo);
    _lastPointerEvents[pointerEvent.pointerId] = true;
  }
}

void UtilityLayerRenderer::render()
{
  _updateCamera();
  if (utilityLayerScene->activeCamera) {
    // Set the camera's scene to utility layers scene
    auto oldScene  = utilityLayerScene->activeCamera->getScene();
    auto camera    = utilityLayerScene->activeCamera;
    camera->_scene = utilityLayerScene.get();
    if (camera->leftCamera()) {
      camera->leftCamera()->_scene = utilityLayerScene.get();
    }
    if (camera->rightCamera()) {
      camera->rightCamera()->_scene = utilityLayerScene.get();
    }

    utilityLayerScene->render(false);

    // Reset camera's scene back to original
    camera->_scene = oldScene;
    if (camera->leftCamera()) {
      camera->leftCamera()->_scene = oldScene;
    }
    if (camera->rightCamera()) {
      camera->rightCamera()->_scene = oldScene;
    }
  }
}

void UtilityLayerRenderer::dispose(bool /*doNotRecurse*/,
                                   bool /*disposeMaterialAndTextures*/)
{
  onPointerOutObservable.clear();

  if (_afterRenderObserver) {
    originalScene->onAfterRenderObservable.remove(_afterRenderObserver);
  }
  if (_sceneDisposeObserver) {
    originalScene->onDisposeObservable.remove(_sceneDisposeObserver);
  }
  if (_originalPointerObserver) {
    originalScene->onPrePointerObservable.remove(_originalPointerObserver);
  }
  utilityLayerScene->dispose();
}

void UtilityLayerRenderer::_updateCamera()
{
  utilityLayerScene->activeCamera = originalScene->activeCamera;
}

} // end of namespace BABYLON
