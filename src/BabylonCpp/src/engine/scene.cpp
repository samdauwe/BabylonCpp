#include <babylon/engine/scene.h>

#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/animations/animatable.h>
#include <babylon/audio/sound_track.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/collisions/collision_coordinator_legacy.h>
#include <babylon/collisions/collision_coordinator_worker.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/core/logging.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/ray.h>
#include <babylon/debug/debug_layer.h>
#include <babylon/engine/click_info.h>
#include <babylon/engine/engine.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/gamepad/gamepad_manager.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/layer/layer.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/multi_render_target.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/uniform_buffer.h>
#include <babylon/math/frustum.h>
#include <babylon/math/tmp.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/simplification/simplification_queue.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/probes/reflection_probe.h>
#include <babylon/rendering/bounding_box_renderer.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/outline_renderer.h>
#include <babylon/rendering/rendering_manager.h>
#include <babylon/sprites/sprite_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

size_t Scene::_uniqueIdCounter = 0;

microseconds_t Scene::MinDeltaTime = std::chrono::milliseconds(1);
microseconds_t Scene::MaxDeltaTime = std::chrono::milliseconds(1000);

unsigned int Scene::DragMovementThreshold = 10;
milliseconds_t Scene::LongPressDelay      = std::chrono::milliseconds(500);
milliseconds_t Scene::DoubleClickDelay    = std::chrono::milliseconds(300);
bool Scene::ExclusiveDoubleClickMode      = false;

Scene::Scene(Engine* engine)
    : autoClear{true}
    , autoClearDepthAndStencil{true}
    , clearColor{Color4(0.2f, 0.2f, 0.3f, 1.f)}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , _environmentBRDFTexture{nullptr}
    , pointerDownPredicate{nullptr}
    , pointerUpPredicate{nullptr}
    , pointerMovePredicate{nullptr}
    , forceWireframe{false}
    , forceShowBoundingBoxes{false}
    , _clipPlane{nullptr}
    , animationsEnabled{true}
    , constantlyUpdateMeshUnderPointer{false}
    , hoverCursor{"pointer"}
    , defaultCursor{""}
    , preventDefaultOnPointerDown{true}
    , cameraToUseForPointers(nullptr)
    , _mirroredCameraPosition{nullptr}
    , fogColor{Color3(0.2f, 0.2f, 0.3f)}
    , fogDensity{0.1f}
    , fogStart{0.f}
    , fogEnd{1000.f}
    , activeCamera{nullptr}
    , particlesEnabled{true}
    , spritesEnabled{true}
    , lensFlaresEnabled{true}
    , collisionsEnabled{true}
    , collisionCoordinator{nullptr}
    , gravity{Vector3(0.f, -9.807f, 0.f)}
    , postProcessesEnabled{true}
    , postProcessManager{nullptr}
    , renderTargetsEnabled{true}
    , dumpNextRenderTargets{false}
    , probesEnabled{true}
    , actionManager{nullptr}
    , proceduralTexturesEnabled{true}
    , simplificationQueue{nullptr}
    , _cachedMaterial{nullptr}
    , _cachedEffect{nullptr}
    , _cachedVisibility{0.f}
    , requireLightSorting{false}
    , _environmentTexture{nullptr}
    , _spritePredicate{nullptr}
    , _onDisposeObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _onBeforeCameraRenderObserver{nullptr}
    , _onAfterCameraRenderObserver{nullptr}
    , _onPointerMove{nullptr}
    , _onPointerDown{nullptr}
    , _onPointerUp{nullptr}
    , _gamepadManager{nullptr}
    , _initClickEvent{nullptr}
    , _initActionManager{nullptr}
    , _delayedSimpleClick{nullptr}
    , _meshPickProceed{false}
    , _previousHasSwiped{false}
    , _currentPickResult{nullptr}
    , _previousPickResult{nullptr}
    , _totalPointersPressed{0}
    , _doubleClickOccured{false}
    , _startingPointerPosition{Vector2(0.f, 0.f)}
    , _previousStartingPointerPosition{Vector2(0.f, 0.f)}
    , _startingPointerTime{high_res_time_point_t()}
    , _previousStartingPointerTime{high_res_time_point_t()}
    , beforeRender{nullptr}
    , afterRender{nullptr}
    , _timeAccumulator{0}
    , _currentStepId{0}
    , _currentInternalStep{0}
    , _onKeyDown{nullptr}
    , _onKeyUp{nullptr}
    , _onCanvasFocusObserver{nullptr}
    , _onCanvasBlurObserver{nullptr}
    , _useRightHandedSystem{false}
    , _forcePointsCloud{false}
    , _fogEnabled{true}
    , _fogMode{Scene::FOGMODE_NONE}
    , _shadowsEnabled{true}
    , _lightsEnabled{true}
    , _defaultMaterial{nullptr}
    , _texturesEnabled{true}
    , _skeletonsEnabled{true}
    , _postProcessRenderPipelineManager{nullptr}
    , _hasAudioEngine{false}
    , _mainSoundTrack{nullptr}
    , _audioEnabled{true}
    , _headphone{false}
    , _engine{engine ? engine : Engine::LastCreatedEngine()}
    , _animationRatio{0}
    , _animationTimeLastSet{false}
    , animationTimeScale{1}
    , _renderId{0}
    , _executeWhenReadyTimeoutId{-1}
    , _intermediateRendering{false}
    , _viewUpdateFlag{-1}
    , _projectionUpdateFlag{-1}
    , _alternateViewUpdateFlag{-1}
    , _alternateProjectionUpdateFlag{-1}
    , _isDisposed{false}
    , _activeMeshesFrozen{false}
    , _renderingManager{nullptr}
    , _physicsEngine{nullptr}
    , _transformMatrix{Matrix::Zero()}
    , _sceneUbo{nullptr}
    , _alternateSceneUbo{nullptr}
    , _pickWithRayInverseMatrix{nullptr}
    , _boundingBoxRenderer{nullptr}
    , _outlineRenderer{nullptr}
    , _alternateTransformMatrix{nullptr}
    , _useAlternateCameraConfiguration{false}
    , _alternateRendering{false}
    , _frustumPlanesSet{false}
    , _selectionOctree{nullptr}
    , _pointerOverMesh{nullptr}
    , _pointerOverSprite{nullptr}
    , _debugLayer{nullptr}
    , _depthRenderer{nullptr}
    , _geometryBufferRenderer{nullptr}
    , _pickedDownMesh{nullptr}
    , _pickedUpMesh{nullptr}
    , _pickedDownSprite{nullptr}
    , _uid{""}
    , _tempPickingRay{::std::make_unique<Ray>(Ray::Zero())}
    , _cachedRayForTransform{nullptr}
{
  engine->scenes.emplace_back(this);

  _renderingManager = ::std::make_unique<RenderingManager>(this);

  postProcessManager = ::std::make_unique<PostProcessManager>(this);

  _outlineRenderer = ::std::make_unique<OutlineRenderer>(this);

  attachControl();

  // simplification queue
  simplificationQueue = ::std::make_unique<SimplificationQueue>();

  // Collision coordinator initialization.
  setWorkerCollisions(false);

  // Uniform Buffer
  _createUbo();

  // Default Image processing definition.
  _imageProcessingConfiguration
    = ::std::make_unique<ImageProcessingConfiguration>();
}

Scene::~Scene()
{
}

IReflect::Type Scene::type() const
{
  return IReflect::Type::SCENE;
}

// Events
void Scene::setOnDispose(
  const ::std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Scene::setBeforeRender(
  const ::std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  if (callback) {
    _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
  }
}

void Scene::setAfterRender(
  const ::std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  if (callback) {
    _onAfterRenderObserver = onAfterRenderObservable.add(callback);
  }
}

void Scene::setBeforeCameraRender(
  const ::std::function<void(Camera* camera, EventState& es)>& callback)
{
  if (_onBeforeCameraRenderObserver) {
    onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  }

  _onBeforeCameraRenderObserver = onBeforeCameraRenderObservable.add(callback);
}

void Scene::setAfterCameraRender(
  const ::std::function<void(Camera* camera, EventState& es)>& callback)
{
  if (_onAfterCameraRenderObserver) {
    onAfterCameraRenderObservable.remove(_onAfterCameraRenderObserver);
  }
  _onAfterCameraRenderObserver = onAfterCameraRenderObservable.add(callback);
}

// Gamepads
GamepadManager& Scene::gamepadManager()
{
  if (!_gamepadManager) {
    _gamepadManager = ::std::make_unique<GamepadManager>(this);
  }

  return *_gamepadManager.get();
}

// Pointers
Vector2 Scene::unTranslatedPointer() const
{
  return Vector2(static_cast<float>(_unTranslatedPointerX),
                 static_cast<float>(_unTranslatedPointerY));
}

// Properties

BaseTexture* Scene::environmentTexture()
{
  return _environmentTexture;
}

void Scene::setEnvironmentTexture(BaseTexture* value)
{
  if (_environmentTexture == value) {
    return;
  }

  _environmentTexture = value;
  markAllMaterialsAsDirty(Material::TextureDirtyFlag);
}

void Scene::setUseRightHandedSystem(bool value)
{
  if (_useRightHandedSystem == value) {
    return;
  }
  _useRightHandedSystem = value;
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
}

bool Scene::useRightHandedSystem() const
{
  return _useRightHandedSystem;
}

void Scene::setStepId(unsigned int newStepId)
{
  _currentStepId = newStepId;
}

unsigned int Scene::getStepId() const
{
  return _currentStepId;
}

unsigned int Scene::getInternalStep() const
{
  return _currentInternalStep;
}

ImageProcessingConfiguration* Scene::imageProcessingConfiguration()
{
  return _imageProcessingConfiguration.get();
}

bool Scene::forcePointsCloud() const
{
  return _forcePointsCloud;
}

void Scene::setForcePointsCloud(bool value)
{
  if (_forcePointsCloud == value) {
    return;
  }
  _forcePointsCloud = value;
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
}

bool Scene::fogEnabled() const
{
  return _fogEnabled;
}

void Scene::setFogEnabled(bool value)
{
  if (_fogEnabled == value) {
    return;
  }
  _fogEnabled = value;
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
}

bool Scene::fogMode() const
{
  return _fogMode;
}

void Scene::setFogMode(unsigned int value)
{
  if (_fogMode == value) {
    return;
  }
  _fogMode = value;
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
}

bool Scene::shadowsEnabled() const
{
  return _shadowsEnabled;
}

void Scene::setShadowsEnabled(bool value)
{
  if (_shadowsEnabled == value) {
    return;
  }
  _shadowsEnabled = value;
  markAllMaterialsAsDirty(Material::LightDirtyFlag);
}

bool Scene::lightsEnabled() const
{
  return _lightsEnabled;
}

void Scene::setLightsEnabled(bool value)
{
  if (_lightsEnabled == value) {
    return;
  }
  _lightsEnabled = value;
  markAllMaterialsAsDirty(Material::LightDirtyFlag);
}

bool Scene::texturesEnabled() const
{
  return _texturesEnabled;
}

void Scene::setTexturesEnabled(bool value)
{
  if (_texturesEnabled == value) {
    return;
  }
  _texturesEnabled = value;
  markAllMaterialsAsDirty(Material::TextureDirtyFlag);
}

bool Scene::skeletonsEnabled() const
{
  return _skeletonsEnabled;
}

void Scene::setSkeletonsEnabled(bool value)
{
  if (_skeletonsEnabled == value) {
    return;
  }
  _skeletonsEnabled = value;
  markAllMaterialsAsDirty(Material::AttributesDirtyFlag);
}

PostProcessRenderPipelineManager* Scene::postProcessRenderPipelineManager()
{
  if (!_postProcessRenderPipelineManager) {
    _postProcessRenderPipelineManager
      = ::std::make_unique<PostProcessRenderPipelineManager>();
  }

  return _postProcessRenderPipelineManager.get();
}

SoundTrack* Scene::mainSoundTrack()
{
  if (!_mainSoundTrack) {
    _mainSoundTrack = ::std::make_unique<SoundTrack>(this, true);
  }

  return _mainSoundTrack.get();
}

Plane* Scene::clipPlane()
{
  return _clipPlane ? _clipPlane.get() : nullptr;
}

void Scene::setClipPlane(const Plane& plane)
{
  _clipPlane = ::std::make_unique<Plane>(plane);
}

void Scene::resetClipPlane()
{
  _clipPlane.reset(nullptr);
}

void Scene::setMirroredCameraPosition(const Vector3& newPosition)
{
  _mirroredCameraPosition = ::std::make_unique<Vector3>(newPosition);
}

Material* Scene::defaultMaterial()
{
  if (!_defaultMaterial) {
    _defaultMaterial = StandardMaterial::New("default material", this);
  }

  return _defaultMaterial;
}

void Scene::setDefaultMaterial(Material* value)
{
  _defaultMaterial = value;
}

bool Scene::_isAlternateRenderingEnabled() const
{
  return _alternateRendering;
}

array_t<Plane, 6>& Scene::frustumPlanes()
{
  return _frustumPlanes;
}

const array_t<Plane, 6>& Scene::frustumPlanes() const
{
  return _frustumPlanes;
}

DebugLayer* Scene::debugLayer()
{
  if (!_debugLayer) {
    _debugLayer = ::std::make_unique<DebugLayer>(this);
  }
  return _debugLayer.get();
}

void Scene::setWorkerCollisions(bool enabled)
{
  _workerCollisions = enabled;
  if (collisionCoordinator) {
    collisionCoordinator->destroy();
    collisionCoordinator.reset(nullptr);
  }

  if (enabled) {
    collisionCoordinator = ::std::make_unique<CollisionCoordinatorWorker>();
  }
  else {
    collisionCoordinator = ::std::make_unique<CollisionCoordinatorLegacy>();
  }

  collisionCoordinator->init(this);
}

bool Scene::workerCollisions() const
{
  return _workerCollisions;
}

vector_t<AbstractMesh*> Scene::getMeshes() const
{
  vector_t<AbstractMesh*> _meshes;
  _meshes.reserve(meshes.size());

  ::std::for_each(meshes.begin(), meshes.end(),
                  [&_meshes](const unique_ptr_t<AbstractMesh>& mesh) {
                    _meshes.emplace_back(mesh.get());
                  });

  return _meshes;
}

Octree<AbstractMesh*>* Scene::selectionOctree()
{
  return _selectionOctree;
}

AbstractMesh* Scene::meshUnderPointer()
{
  return _pointerOverMesh;
}

int Scene::pointerX()
{
  return _pointerX;
}

int Scene::pointerY()
{
  return _pointerY;
}

Material* Scene::getCachedMaterial()
{
  return _cachedMaterial;
}

Effect* Scene::getCachedEffect()
{
  return _cachedEffect;
}

float Scene::getCachedVisibility()
{
  return _cachedVisibility;
}

bool Scene::isCachedMaterialInvalid(Material* material, Effect* effect,
                                    float visibility)
{
  return _cachedEffect != effect || _cachedMaterial != material
         || !stl_util::almost_equal(*_cachedVisibility, visibility);
}

BoundingBoxRenderer* Scene::getBoundingBoxRenderer()
{
  if (!_boundingBoxRenderer) {
    _boundingBoxRenderer = ::std::make_unique<BoundingBoxRenderer>(this);
  }

  return _boundingBoxRenderer.get();
}

OutlineRenderer* Scene::getOutlineRenderer()
{
  return _outlineRenderer.get();
}

Engine* Scene::getEngine()
{
  return _engine;
}

size_t Scene::getTotalVertices() const
{
  return _totalVertices.current();
}

PerfCounter& Scene::totalVerticesPerfCounter()
{
  return _totalVertices;
}

size_t Scene::getActiveIndices() const
{
  return _activeIndices.current();
}

PerfCounter& Scene::totalActiveIndicesPerfCounter()
{
  return _activeIndices;
}

size_t Scene::getActiveParticles() const
{
  return _activeParticles.current();
}

PerfCounter& Scene::activeParticlesPerfCounter()
{
  return _activeParticles;
}

size_t Scene::getActiveBones() const
{
  return _activeBones.current();
}

PerfCounter& Scene::activeBonesPerfCounter()
{
  return _activeBones;
}

vector_t<AbstractMesh*>& Scene::getActiveMeshes()
{
  return _activeMeshes;
}

const vector_t<AbstractMesh*>& Scene::getActiveMeshes() const
{
  return _activeMeshes;
}

float Scene::getAnimationRatio() const
{
  return _animationRatio;
}

int Scene::getRenderId() const
{
  return _renderId;
}

void Scene::incrementRenderId()
{
  ++_renderId;
}

void Scene::_updatePointerPosition(const PointerEvent evt)
{
  auto canvasRect = _engine->getRenderingCanvasClientRect();

  if (!canvasRect) {
    return;
  }

  _pointerX = evt.clientX - (*canvasRect).left;
  _pointerY = evt.clientY - (*canvasRect).top;

  _unTranslatedPointerX = _pointerX;
  _unTranslatedPointerY = _pointerY;
}

void Scene::_createUbo()
{
  _sceneUbo = ::std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _sceneUbo->addUniform("viewProjection", 16);
  _sceneUbo->addUniform("view", 16);
}

void Scene::_createAlternateUbo()
{
  _alternateSceneUbo
    = ::std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _alternateSceneUbo->addUniform("viewProjection", 16);
  _alternateSceneUbo->addUniform("view", 16);
}

Scene& Scene::simulatePointerMove(const PickingInfo* pickResult)
{
  PointerEvent evt("pointermove");
  return _processPointerMove(pickResult, evt);
}

Scene& Scene::_processPointerMove(const PickingInfo* pickResult,
                                  const PointerEvent& evt)
{
  auto canvas = _engine->getRenderingCanvas();

  if (!canvas) {
    return *this;
  }

  if (pickResult && pickResult->hit && pickResult->pickedMesh) {
    setPointerOverSprite(nullptr);

    setPointerOverMesh(pickResult->pickedMesh);

    if (_pointerOverMesh && _pointerOverMesh->actionManager
        && _pointerOverMesh->actionManager->hasPointerTriggers()) {
      if (!_pointerOverMesh->actionManager->hoverCursor.empty()) {
        canvas->style.cursor = _pointerOverMesh->actionManager->hoverCursor;
      }
      else {
        canvas->style.cursor = hoverCursor;
      }
    }
    else {
      canvas->style.cursor = defaultCursor;
    }
  }
  else {
    setPointerOverMesh(nullptr);
    // Sprites
    pickResult = pickSprite(_unTranslatedPointerX, _unTranslatedPointerY,
                            _spritePredicate, false, cameraToUseForPointers);

    if (pickResult && pickResult->hit && pickResult->pickedSprite) {
      setPointerOverSprite(pickResult->pickedSprite);
      if (_pointerOverSprite && _pointerOverSprite->actionManager
          && !_pointerOverSprite->actionManager->hoverCursor.empty()) {
        canvas->style.cursor = _pointerOverSprite->actionManager->hoverCursor;
      }
      else {
        canvas->style.cursor = hoverCursor;
      }
    }
    else {
      setPointerOverSprite(nullptr);
      // Restore pointer
      canvas->style.cursor = defaultCursor;
    }
  }

  if (pickResult) {
    if (onPointerMove) {
      onPointerMove(evt, pickResult);
    }

    if (onPointerObservable.hasObservers()) {
      auto type = evt.type == EventType::MOUSE_WHEEL
                      || evt.type == EventType::DOM_MOUSE_SCROLL ?
                    PointerEventTypes::POINTERWHEEL :
                    PointerEventTypes::POINTERMOVE;
      auto pi = new PointerInfo(type, evt, *pickResult);
      onPointerObservable.notifyObservers(pi, static_cast<int>(type));
    }
  }

  return *this;
}

Scene& Scene::simulatePointerDown(const PickingInfo* pickResult)
{
  PointerEvent evt("pointerdown");
  return _processPointerDown(pickResult, evt);
}

Scene& Scene::_processPointerDown(const PickingInfo* pickResult,
                                  const PointerEvent& evt)
{
  if (pickResult && pickResult->hit && pickResult->pickedMesh) {
    _pickedDownMesh    = pickResult->pickedMesh;
    auto actionManager = pickResult->pickedMesh->actionManager;
    if (actionManager) {
      if (actionManager->hasPickTriggers()) {
        actionManager->processTrigger(
          ActionManager::OnPickDownTrigger,
          ActionEvent::CreateNew(pickResult->pickedMesh, evt));
        switch (evt.button) {
          case MouseButtonType::LEFT:
            actionManager->processTrigger(
              ActionManager::OnLeftPickTrigger,
              ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            break;
          case MouseButtonType::MIDDLE:
            actionManager->processTrigger(
              ActionManager::OnCenterPickTrigger,
              ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            break;
          case MouseButtonType::RIGHT:
            actionManager->processTrigger(
              ActionManager::OnRightPickTrigger,
              ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            break;
          default:
            break;
        }
      }

      if (actionManager->hasSpecificTrigger(
            ActionManager::OnLongPressTrigger)) {
#if 0
        window.setTimeout(
          (function() {
            var pickResult = pick(
              _unTranslatedPointerX, _unTranslatedPointerY, (mesh
                                                             : AbstractMesh)
              : boolean = > mesh.isPickable && mesh.isVisible && mesh.isReady()
                          && mesh.actionManager
                          && mesh.actionManager.hasSpecificTrigger(
                               ActionManager.OnLongPressTrigger)
                          && mesh == _pickedDownMesh,
                false, cameraToUseForPointers);

            if (pickResult && pickResult.hit && pickResult.pickedMesh) {
              if (_totalPointersPressed != 0
                  && ((new Date().getTime() - _startingPointerTime)
                      > Scene.LongPressDelay)
                  && (Math.abs(_startingPointerPosition.x - _pointerX)
                        < Scene.DragMovementThreshold
                      && Math.abs(_startingPointerPosition.y - _pointerY)
                           < Scene.DragMovementThreshold)) {
                _startingPointerTime = 0;
                actionManager.processTrigger(
                  ActionManager.OnLongPressTrigger,
                  ActionEvent.CreateNew(pickResult.pickedMesh, evt));
              }
            }
          }).bind(this),
          Scene.LongPressDelay);
#endif
      }
    }
  }

  if (pickResult) {
    if (onPointerDown) {
      onPointerDown(evt, pickResult);
    }

    if (onPointerObservable.hasObservers()) {
      auto type = PointerEventTypes::POINTERDOWN;
      auto pi   = new PointerInfo(type, evt, *pickResult);
      onPointerObservable.notifyObservers(pi, static_cast<int>(type));
    }
  }

  return *this;
}

Scene& Scene::simulatePointerUp(const PickingInfo* pickResult)
{
  PointerEvent evt("pointerup");
  ClickInfo clickInfo;
  clickInfo.setSingleClick(true);
  clickInfo.setIgnore(true);

  return _processPointerUp(pickResult, evt, clickInfo);
}

Scene& Scene::_processPointerUp(const PickingInfo* pickResult,
                                const PointerEvent& evt,
                                const ClickInfo& clickInfo)
{
  if (pickResult && pickResult && pickResult->pickedMesh) {
    _pickedUpMesh = pickResult->pickedMesh;
    if (_pickedDownMesh == _pickedUpMesh) {
      if (onPointerPick) {
        onPointerPick(evt, pickResult);
      }
      if (clickInfo.singleClick() && !clickInfo.ignore()
          && onPointerObservable.hasObservers()) {
        auto type = PointerEventTypes::POINTERPICK;
        auto pi   = new PointerInfo(type, evt, *pickResult);
        onPointerObservable.notifyObservers(pi, static_cast<int>(type));
      }
    }
    if (pickResult->pickedMesh->actionManager) {
      if (clickInfo.ignore()) {
        pickResult->pickedMesh->actionManager->processTrigger(
          ActionManager::OnPickUpTrigger,
          ActionEvent::CreateNew(pickResult->pickedMesh, evt));
      }
      if (!clickInfo.hasSwiped() && !clickInfo.ignore()
          && clickInfo.singleClick()) {
        pickResult->pickedMesh->actionManager->processTrigger(
          ActionManager::OnPickTrigger,
          ActionEvent::CreateNew(pickResult->pickedMesh, evt));
      }
      if (clickInfo.doubleClick() && !clickInfo.ignore()
          && pickResult->pickedMesh->actionManager->hasSpecificTrigger(
               ActionManager::OnDoublePickTrigger)) {
        pickResult->pickedMesh->actionManager->processTrigger(
          ActionManager::OnDoublePickTrigger,
          ActionEvent::CreateNew(pickResult->pickedMesh, evt));
      }
    }
  }
  if (_pickedDownMesh && _pickedDownMesh->actionManager
      && _pickedDownMesh->actionManager->hasSpecificTrigger(
           ActionManager::OnPickOutTrigger)
      && _pickedDownMesh != _pickedUpMesh) {
    _pickedDownMesh->actionManager->processTrigger(
      ActionManager::OnPickOutTrigger,
      ActionEvent::CreateNew(_pickedDownMesh, evt));
  }

  if (onPointerUp) {
    onPointerUp(evt, pickResult);
  }

  if (onPointerObservable.hasObservers()) {
    if (!clickInfo.ignore()) {
      if (!clickInfo.hasSwiped()) {
        if (clickInfo.singleClick()
            && onPointerObservable.hasSpecificMask(
                 PointerEventTypes::POINTERTAP)) {
          auto type = PointerEventTypes::POINTERTAP;
          auto pi   = new PointerInfo(type, evt, *pickResult);
          onPointerObservable.notifyObservers(pi, static_cast<int>(type));
        }
        if (clickInfo.doubleClick()
            && onPointerObservable.hasSpecificMask(
                 PointerEventTypes::POINTERDOUBLETAP)) {
          auto type = PointerEventTypes::POINTERDOUBLETAP;
          auto pi   = new PointerInfo(type, evt, *pickResult);
          onPointerObservable.notifyObservers(pi, static_cast<int>(type));
        }
      }
    }
    else {
      auto type = PointerEventTypes::POINTERUP;
      auto pi   = new PointerInfo(type, evt, *pickResult);
      onPointerObservable.notifyObservers(pi, static_cast<int>(type));
    }
  }

  return *this;
}

void Scene::attachControl(bool attachUp, bool attachDown, bool attachMove)
{
  spritePredicate = [](Sprite* sprite) {
    return sprite->isPickable && sprite->actionManager
           && sprite->actionManager->hasPointerTriggers();
  };

  _onPointerMove
    = [this](PointerEvent&& evt) { _onPointerMoveEvent(::std::move(evt)); };

  _onPointerDown
    = [this](PointerEvent&& evt) { _onPointerDownEvent(::std::move(evt)); };

  _onPointerUp
    = [this](PointerEvent&& evt) { _onPointerUpEvent(::std::move(evt)); };

  // _onKeyDown
  // = [this](KeyboardEvent&& evt) { _onKeyDownEvent(::std::move(evt)); };

  // _onKeyUp = [this](KeyboardEvent&& evt) { _onKeyUpEvent(::std::move(evt));
  // };

  auto canvas = _engine->getRenderingCanvas();
  if (attachMove) {
    canvas->addMouseEventListener(EventType::MOUSE_MOVE, _onPointerMove, false);
    // Wheel
    canvas->addMouseEventListener(EventType::MOUSE_WHEEL, _onPointerMove,
                                  false);
  }

  if (attachDown) {
    canvas->addMouseEventListener(EventType::MOUSE_BUTTON_DOWN, _onPointerDown,
                                  false);
  }

  if (attachUp) {
    canvas->addMouseEventListener(EventType::MOUSE_BUTTON_UP, _onPointerUp,
                                  false);
  }

  canvas->tabIndex = 1;

  // canvas->addKeyEventListener(EventType::KEY_DOWN, _onKeyDown, false);
  // canvas->addKeyEventListener(EventType::KEY_UP, _onKeyUp, false);
}

void Scene::detachControl()
{
  auto engine = getEngine();
  auto canvas = engine->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  canvas->removeMouseEventListener(EventType::MOUSE_MOVE, _onPointerMove);
  canvas->removeMouseEventListener(EventType::MOUSE_BUTTON_DOWN,
                                   _onPointerDown);
  canvas->removeMouseEventListener(EventType::MOUSE_BUTTON_UP, _onPointerUp);

  if (_onCanvasBlurObserver) {
    engine->onCanvasBlurObservable.remove(_onCanvasBlurObserver);
  }

  if (_onCanvasFocusObserver) {
    engine->onCanvasFocusObservable.remove(_onCanvasFocusObserver);
  }

  // Wheel
  canvas->removeMouseEventListener(EventType::MOUSE_WHEEL, _onPointerMove);
  canvas->removeMouseEventListener(EventType::DOM_MOUSE_SCROLL, _onPointerMove);

  // Keyboard
  // canvas->removeKeyEventListener(EventType::KEY_DOWN, _onKeyDown);
  // canvas->removeKeyEventListener(EventType::KEY_UP, _onKeyUp);

  // Observables
  onKeyboardObservable.clear();
  onPreKeyboardObservable.clear();
  onPointerObservable.clear();
  onPrePointerObservable.clear();
}

void Scene::_onPointerMoveEvent(PointerEvent&& evt)
{
  _updatePointerPosition(evt);

  // PreObservable support
  if (onPrePointerObservable.hasObservers()) {
    auto type = evt.type == EventType::MOUSE_WHEEL ?
                  PointerEventTypes::POINTERWHEEL :
                  PointerEventTypes::POINTERMOVE;
    auto pi = ::std::make_unique<PointerInfoPre>(
      type, evt, static_cast<float>(_unTranslatedPointerX),
      static_cast<float>(_unTranslatedPointerY));
    onPrePointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
    if (pi->skipOnPointerObservable) {
      return;
    }
  }

  if (!cameraToUseForPointers && !activeCamera) {
    return;
  }

  auto canvas = _engine->getRenderingCanvas();

  if (!pointerMovePredicate) {
    pointerMovePredicate = [this](AbstractMesh* mesh) {
      return mesh->isPickable && mesh->isVisible && mesh->isReady()
             && mesh->isEnabled()
             && (constantlyUpdateMeshUnderPointer
                 || mesh->actionManager != nullptr);
    };
  }

  // Meshes
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY,
                         pointerMovePredicate, false, cameraToUseForPointers);

  if (pickResult->hit && pickResult->pickedMesh) {
    setPointerOverSprite(nullptr);

    setPointerOverMesh(pickResult->pickedMesh);

    if (_pointerOverMesh->actionManager
        && _pointerOverMesh->actionManager->hasPointerTriggers()) {
      if (!_pointerOverMesh->actionManager->hoverCursor.empty()) {
        canvas->style.cursor = _pointerOverMesh->actionManager->hoverCursor;
      }
      else {
        canvas->style.cursor = hoverCursor;
      }
    }
    else {
      canvas->style.cursor = defaultCursor;
    }
  }
  else {
    setPointerOverMesh(nullptr);
    // Sprites
    pickResult = pickSprite(_unTranslatedPointerX, _unTranslatedPointerY,
                            spritePredicate, false, cameraToUseForPointers);

    if (pickResult->hit && pickResult->pickedSprite) {
      setPointerOverSprite(pickResult->pickedSprite);
      if (_pointerOverSprite->actionManager
          && !_pointerOverSprite->actionManager->hoverCursor.empty()) {
        canvas->style.cursor = _pointerOverSprite->actionManager->hoverCursor;
      }
      else {
        canvas->style.cursor = hoverCursor;
      }
    }
    else {
      setPointerOverSprite(nullptr);
      // Restore pointer
      canvas->style.cursor = defaultCursor;
    }
  }

  if (onPointerObservable.hasObservers()) {
    auto type = evt.type == EventType::MOUSE_WHEEL ?
                  PointerEventTypes::POINTERWHEEL :
                  PointerEventTypes::POINTERMOVE;
    auto pi = ::std::make_unique<PointerInfo>(type, evt, *pickResult);
    onPointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
  }
}

void Scene::_onPointerDownEvent(PointerEvent&& evt)
{
  _updatePointerPosition(evt);

  // PreObservable support
  if (onPrePointerObservable.hasObservers()) {
    auto type = PointerEventTypes::POINTERDOWN;
    auto pi   = ::std::make_unique<PointerInfoPre>(
      type, evt, static_cast<float>(_unTranslatedPointerX),
      static_cast<float>(_unTranslatedPointerY));
    onPrePointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
    if (pi->skipOnPointerObservable) {
      return;
    }
  }

  if (!cameraToUseForPointers && !activeCamera) {
    return;
  }

  _startingPointerPosition.x = static_cast<float>(_pointerX);
  _startingPointerPosition.y = static_cast<float>(_pointerY);
  _startingPointerTime       = Time::highresTimepointNow();

  if (!pointerDownPredicate) {
    pointerDownPredicate = [](AbstractMesh* mesh) {
      return mesh->isPickable && mesh->isVisible && mesh->isReady()
             && mesh->isEnabled();
    };
  }

  // Meshes
  _pickedDownMesh = nullptr;
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY,
                         pointerDownPredicate, false, cameraToUseForPointers);

  if (pickResult->hit && pickResult->pickedMesh) {
    if (pickResult->pickedMesh->actionManager) {
      _pickedDownMesh = pickResult->pickedMesh;
      if (pickResult->pickedMesh->actionManager->hasPickTriggers()) {
        switch (evt.button) {
          case MouseButtonType::LEFT:
            pickResult->pickedMesh->actionManager->processTrigger(
              ActionManager::OnLeftPickTrigger,
              ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            break;
          case MouseButtonType::MIDDLE:
            pickResult->pickedMesh->actionManager->processTrigger(
              ActionManager::OnCenterPickTrigger,
              ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            break;
          case MouseButtonType::RIGHT:
          default:
            pickResult->pickedMesh->actionManager->processTrigger(
              ActionManager::OnRightPickTrigger,
              ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            break;
        }
        if (pickResult->pickedMesh->actionManager) {
          pickResult->pickedMesh->actionManager->processTrigger(
            ActionManager::OnPickDownTrigger,
            ActionEvent::CreateNew(pickResult->pickedMesh, evt));
        }
      }

      if (pickResult->pickedMesh->actionManager
          && pickResult->pickedMesh->actionManager->hasSpecificTrigger(
               ActionManager::OnLongPressTrigger)) {
        // Long press trigger
        auto pickResult
          = pick(_unTranslatedPointerX, _unTranslatedPointerY,
                 [](AbstractMesh* mesh) {
                   return mesh->isPickable && mesh->isVisible && mesh->isReady()
                          && mesh->actionManager
                          && mesh->actionManager->hasSpecificTrigger(
                               ActionManager::OnLongPressTrigger);
                 },
                 false, cameraToUseForPointers);

        if (pickResult->hit && pickResult->pickedMesh) {
          if (pickResult->pickedMesh->actionManager) {
            if (_startingPointerTime != high_res_time_point_t()
                && ((Time::fpTimeSince<float, ::std::milli>(
                       _startingPointerTime)
                     > ActionManager::LongPressDelay)
                    && (::std::abs(_startingPointerPosition.x - _pointerX)
                          < ActionManager::DragMovementThreshold
                        && ::std::abs(_startingPointerPosition.y - _pointerY)
                             < ActionManager::DragMovementThreshold))) {
              _startingPointerTime = high_res_time_point_t();
              pickResult->pickedMesh->actionManager->processTrigger(
                ActionManager::OnLongPressTrigger,
                ActionEvent::CreateNew(pickResult->pickedMesh, evt));
            }
          }
        }
      }
    }
  }

  if (onPointerObservable.hasObservers()) {
    auto type = PointerEventTypes::POINTERDOWN;
    auto pi   = ::std::make_unique<PointerInfo>(type, evt, *pickResult);
    onPointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
  }

  // Sprites
  _pickedDownSprite = nullptr;
  if (!spriteManagers.empty()) {
    pickResult = pickSprite(_unTranslatedPointerX, _unTranslatedPointerY,
                            spritePredicate, false, cameraToUseForPointers);

    if (pickResult->hit && pickResult->pickedSprite) {
      if (pickResult->pickedSprite->actionManager) {
        _pickedDownSprite = pickResult->pickedSprite;
        switch (evt.button) {
          case MouseButtonType::LEFT:
            pickResult->pickedSprite->actionManager->processTrigger(
              ActionManager::OnLeftPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, this,
                                               evt));
            break;
          case MouseButtonType::MIDDLE:
            pickResult->pickedSprite->actionManager->processTrigger(
              ActionManager::OnCenterPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, this,
                                               evt));
            break;
          case MouseButtonType::RIGHT:
          default:
            pickResult->pickedSprite->actionManager->processTrigger(
              ActionManager::OnRightPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, this,
                                               evt));
            break;
        }
        if (pickResult->pickedSprite->actionManager) {
          pickResult->pickedSprite->actionManager->processTrigger(
            ActionManager::OnPickDownTrigger,
            ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, this,
                                             evt));
        }
      }
    }
  }
}

void Scene::_onPointerUpEvent(PointerEvent&& evt)
{
  _updatePointerPosition(evt);

  // PreObservable support
  if (onPrePointerObservable.hasObservers()) {
    auto type = PointerEventTypes::POINTERUP;
    auto pi   = ::std::make_unique<PointerInfoPre>(
      type, evt, static_cast<float>(_unTranslatedPointerX),
      static_cast<float>(_unTranslatedPointerY));
    onPrePointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
    if (pi->skipOnPointerObservable) {
      return;
    }
  }

  if (!cameraToUseForPointers && !activeCamera) {
    return;
  }

  if (!pointerUpPredicate) {
    pointerUpPredicate = [](AbstractMesh* mesh) {
      return mesh->isPickable && mesh->isVisible && mesh->isReady()
             && mesh->isEnabled();
    };
  }

  // Meshes
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY,
                         pointerUpPredicate, false, cameraToUseForPointers);

  if (pickResult->hit && pickResult->pickedMesh) {
    if (_pickedDownMesh != nullptr
        && pickResult->pickedMesh == _pickedDownMesh) {
      if (onPointerObservable.hasObservers()) {
        auto type = PointerEventTypes::POINTERPICK;
        auto pi   = ::std::make_unique<PointerInfo>(type, evt, *pickResult);
        onPointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
      }
    }
    if (pickResult->pickedMesh->actionManager) {
      pickResult->pickedMesh->actionManager->processTrigger(
        ActionManager::OnPickUpTrigger,
        ActionEvent::CreateNew(pickResult->pickedMesh, evt));
      if (pickResult->pickedMesh->actionManager) {
        if (::std::abs(_startingPointerPosition.x - _pointerX)
              < ActionManager::DragMovementThreshold
            && ::std::abs(_startingPointerPosition.y - _pointerY)
                 < ActionManager::DragMovementThreshold) {
          pickResult->pickedMesh->actionManager->processTrigger(
            ActionManager::OnPickTrigger,
            ActionEvent::CreateNew(pickResult->pickedMesh, evt));
        }
      }
    }
  }
  if (_pickedDownMesh && _pickedDownMesh->actionManager
      && _pickedDownMesh != pickResult->pickedMesh) {
    _pickedDownMesh->actionManager->processTrigger(
      ActionManager::OnPickOutTrigger,
      ActionEvent::CreateNew(_pickedDownMesh, evt));
  }

  if (onPointerObservable.hasObservers()) {
    auto type = PointerEventTypes::POINTERUP;
    auto pi   = ::std::make_unique<PointerInfo>(type, evt, *pickResult);
    onPointerObservable.notifyObservers(pi.get(), static_cast<int>(type));
  }

  _startingPointerTime = high_res_time_point_t();

  // Sprites
  if (!spriteManagers.empty()) {
    pickResult = pickSprite(_unTranslatedPointerX, _unTranslatedPointerY,
                            spritePredicate, false, cameraToUseForPointers);

    if (pickResult->hit && pickResult->pickedSprite) {
      if (pickResult->pickedSprite->actionManager) {
        pickResult->pickedSprite->actionManager->processTrigger(
          ActionManager::OnPickUpTrigger,
          ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, this,
                                           evt));
        if (pickResult->pickedSprite->actionManager) {
          if (::std::abs(_startingPointerPosition.x - _pointerX)
                < ActionManager::DragMovementThreshold
              && ::std::abs(_startingPointerPosition.y - _pointerY)
                   < ActionManager::DragMovementThreshold) {
            pickResult->pickedSprite->actionManager->processTrigger(
              ActionManager::OnPickTrigger,
              ActionEvent::CreateNewFromSprite(pickResult->pickedSprite, this,
                                               evt));
          }
        }
      }
    }
    if (_pickedDownSprite && _pickedDownSprite->actionManager
        && _pickedDownSprite != pickResult->pickedSprite) {
      _pickedDownSprite->actionManager->processTrigger(
        ActionManager::OnPickOutTrigger,
        ActionEvent::CreateNewFromSprite(_pickedDownSprite, this, evt));
    }
  }
}

void Scene::_onKeyDownEvent(Event&& evt)
{
  if (actionManager) {
    actionManager->processTrigger(ActionManager::OnKeyDownTrigger,
                                  ActionEvent::CreateNewFromScene(this, evt));
  }
}

void Scene::_onKeyUpEvent(Event&& evt)
{
  if (actionManager) {
    actionManager->processTrigger(ActionManager::OnKeyUpTrigger,
                                  ActionEvent::CreateNewFromScene(this, evt));
  }
}

bool Scene::isReady()
{
  if (_isDisposed) {
    return false;
  }

  if (!_pendingData.empty()) {
    return false;
  }

  // Geometries
  for (const auto& geometry : _geometries) {
    if (geometry->delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
      return false;
    }
  }

  // Meshes
  for (const auto& mesh : meshes) {
    if (!mesh->isEnabled()) {
      continue;
    }

    if (mesh->subMeshes.empty()) {
      continue;
    }

    if (!mesh->isReady()) {
      return false;
    }

    auto mat = mesh->material();
    if (mat) {
      if (!mat->isReady(mesh.get())) {
        return false;
      }
    }
  }

  return true;
}

void Scene::resetCachedMaterial()
{
  _cachedMaterial   = nullptr;
  _cachedEffect     = nullptr;
  _cachedVisibility = 0.f;
}

void Scene::registerBeforeRender(
  const ::std::function<void(Scene* scene, EventState& es)>& func)
{
  onBeforeRenderObservable.add(func);
}

void Scene::unregisterBeforeRender(
  const ::std::function<void(Scene* scene, EventState& es)>& func)
{
  onBeforeRenderObservable.removeCallback(func);
}

void Scene::registerAfterRender(
  const ::std::function<void(Scene* scene, EventState& es)>& func)
{
  onAfterRenderObservable.add(func);
}

void Scene::unregisterAfterRender(
  const ::std::function<void(Scene* scene, EventState& es)>& func)
{
  onAfterRenderObservable.removeCallback(func);
}

void Scene::_addPendingData(Mesh* /*mesh*/)
{
}

void Scene::_addPendingData(InternalTexture* /*texure*/)
{
}

void Scene::_removePendingData(InternalTexture* /*texture*/)
{
  auto wasLoading = isLoading();

  if (wasLoading && !isLoading()) {
    onDataLoadedObservable.notifyObservers(this);
  }
}

void Scene::getWaitingItemsCount()
{
}

bool Scene::isLoading() const
{
  return _pendingData.size() > 0;
}

void Scene::executeWhenReady(
  const ::std::function<void(Scene* scene, EventState& es)>& func)
{
  onReadyObservable.add(func);

  if (_executeWhenReadyTimeoutId != -1) {
    return;
  }
}

void Scene::_checkIsReady()
{
  if (isReady()) {
    onReadyObservable.notifyObservers(this);

    onReadyObservable.clear();
    _executeWhenReadyTimeoutId = -1;
    return;
  }
}

vector_t<Animation*> Scene::getAnimations()
{
  return vector_t<Animation*>();
}

Animatable* Scene::beginAnimation(IAnimatable* target, int from, int to,
                                  bool loop, float speedRatio,
                                  const ::std::function<void()>& onAnimationEnd,
                                  Animatable* animatable)
{
  if (from > to && speedRatio > 0.f) {
    speedRatio *= -1;
  }

  stopAnimation(target);

  if (!animatable) {
    animatable = new Animatable(this, target, from, to, loop, speedRatio,
                                onAnimationEnd);
  }

  // Local animations
  auto _animations = target->getAnimations();
  if (!_animations.empty()) {
    animatable->appendAnimations(target, _animations);
  }

  // Children animations
  vector_t<IAnimatable*> animatables;
  if (Skeleton* s = dynamic_cast<Skeleton*>(target)) {
    animatables = s->getAnimatables();
  }
  else if (auto _s = dynamic_cast<StandardMaterial*>(target)) {
    animatables = _s->getAnimatables();
  }
  else if (auto m = dynamic_cast<Mesh*>(target)) {
    animatables = m->getAnimatables();
  }

  for (auto& childAnimatable : animatables) {
    beginAnimation(childAnimatable, from, to, loop, speedRatio, onAnimationEnd,
                   animatable);
  }

  animatable->reset();

  return animatable;
}

Animatable*
Scene::beginDirectAnimation(IAnimatable* target,
                            const vector_t<Animation*>& _animations, int from,
                            int to, bool loop, float speedRatio,
                            const ::std::function<void()>& onAnimationEnd)
{
  return new Animatable(this, target, from, to, loop, speedRatio,
                        onAnimationEnd, _animations);
}

Animatable* Scene::getAnimatableByTarget(IAnimatable* target)
{
  auto it = ::std::find_if(_activeAnimatables.begin(), _activeAnimatables.end(),
                           [&target](const Animatable* animatable) {
                             return animatable->target == target;
                           });
  return (it == _activeAnimatables.end()) ? nullptr : *it;
}

vector_t<Animatable*>& Scene::animatables()
{
  return _activeAnimatables;
}

void Scene::stopAnimation(IAnimatable* target, const string_t& animationName)
{
  auto animatable = getAnimatableByTarget(target);

  if (animatable) {
    animatable->stop(animationName);
  }
}

void Scene::stopAllAnimations()
{
  if (!_activeAnimatables.empty()) {
    for (auto& activeAnimatable : _activeAnimatables) {
      activeAnimatable->stop();
    }
    _activeAnimatables.clear();
  }
}

void Scene::_animate()
{
  if (!animationsEnabled || _activeAnimatables.empty()) {
    return;
  }

  // Getting time
  auto now = Time::highresTimepointNow();
  if (!_animationTimeLastSet) {
    if (!_pendingData.empty()) {
      return;
    }
    _animationTimeLast    = now;
    _animationTimeLastSet = true;
  }
  auto deltaTime = Time::fpTimeSince<size_t, ::std::milli>(_animationTimeLast)
                   * animationTimeScale;
  _animationTime += static_cast<int>(deltaTime);
  _animationTimeLast = now;
  for (auto& activeAnimatable : _activeAnimatables) {
    activeAnimatable->_animate(std::chrono::milliseconds(_animationTime));
  }
}

void Scene::_switchToAlternateCameraConfiguration(bool active)
{
  _useAlternateCameraConfiguration = active;
}

Matrix Scene::getViewMatrix()
{
  return _useAlternateCameraConfiguration ? _alternateViewMatrix : _viewMatrix;
}

Matrix& Scene::getProjectionMatrix()
{
  return _useAlternateCameraConfiguration ? _alternateProjectionMatrix :
                                            _projectionMatrix;
}

const Matrix& Scene::getProjectionMatrix() const
{
  return _useAlternateCameraConfiguration ? _alternateProjectionMatrix :
                                            _projectionMatrix;
}

Matrix Scene::getTransformMatrix()
{
  return _useAlternateCameraConfiguration ? *_alternateTransformMatrix :
                                            _transformMatrix;
}

void Scene::setTransformMatrix(Matrix& view, Matrix& projection)
{
  if (_viewUpdateFlag == view.updateFlag
      && _projectionUpdateFlag == projection.updateFlag) {
    return;
  }

  _viewUpdateFlag       = view.updateFlag;
  _projectionUpdateFlag = projection.updateFlag;
  _viewMatrix           = view;
  _projectionMatrix     = projection;

  _viewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);

  // Update frustum
  if (_frustumPlanes.empty()) {
    _frustumPlanes = Frustum::GetPlanes(_transformMatrix);
  }
  else {
    Frustum::GetPlanesToRef(_transformMatrix, _frustumPlanes);
  }

  if (activeCamera && activeCamera->_alternateCamera) {
    auto& otherCamera = activeCamera->_alternateCamera;
    otherCamera->getViewMatrix().multiplyToRef(
      otherCamera->getProjectionMatrix(), Tmp::MatrixArray[0]);
    // Replace right plane by second camera right plane
    Frustum::GetRightPlaneToRef(Tmp::MatrixArray[0], _frustumPlanes[3]);
  }

  if (_sceneUbo->useUbo()) {
    _sceneUbo->updateMatrix("viewProjection", _transformMatrix);
    _sceneUbo->updateMatrix("view", _viewMatrix);
    _sceneUbo->update();
  }
}

void Scene::_setAlternateTransformMatrix(Matrix& view, Matrix& projection)
{
  if (_alternateViewUpdateFlag == view.updateFlag
      && _alternateProjectionUpdateFlag == projection.updateFlag) {
    return;
  }

  _alternateViewUpdateFlag       = view.updateFlag;
  _alternateProjectionUpdateFlag = projection.updateFlag;
  _alternateViewMatrix           = view;
  _alternateProjectionMatrix     = projection;

  if (!_alternateTransformMatrix) {
    _alternateTransformMatrix = ::std::make_unique<Matrix>(Matrix::Zero());
  }

  _alternateViewMatrix.multiplyToRef(_alternateProjectionMatrix,
                                     *_alternateTransformMatrix);

  if (!_alternateSceneUbo) {
    _createAlternateUbo();
  }

  if (_alternateSceneUbo->useUbo()) {
    _alternateSceneUbo->updateMatrix("viewProjection",
                                     *_alternateTransformMatrix);
    _alternateSceneUbo->updateMatrix("view", _alternateViewMatrix);
    _alternateSceneUbo->update();
  }
}

UniformBuffer* Scene::getSceneUniformBuffer()
{
  return _useAlternateCameraConfiguration ? _alternateSceneUbo.get() :
                                            _sceneUbo.get();
}

size_t Scene::getUniqueId()
{
  auto result = Scene::_uniqueIdCounter;
  Scene::_uniqueIdCounter++;
  return result;
}

void Scene::addMesh(unique_ptr_t<AbstractMesh>&& newMesh)
{
  auto _newMesh = newMesh.get();
  meshes.emplace_back(::std::move(newMesh));

  // notify the collision coordinator
  if (collisionCoordinator) {
    collisionCoordinator->onMeshAdded(_newMesh);
  }

  onNewMeshAddedObservable.notifyObservers(_newMesh);
}

int Scene::removeMesh(AbstractMesh* toRemove)
{
  auto it   = ::std::find_if(meshes.begin(), meshes.end(),
                           [&toRemove](const unique_ptr_t<AbstractMesh>& mesh) {
                             return mesh.get() == toRemove;
                           });
  int index = static_cast<int>(it - meshes.begin());
  if (it != meshes.end()) {
    // Remove from the scene if mesh found
    meshes.erase(it);
  }

  onMeshRemovedObservable.notifyObservers(toRemove);

  return index;
}

void Scene::addTransformNode(TransformNode* newTransformNode)
{
  transformNodes.emplace_back(newTransformNode);

  onNewTransformNodeAddedObservable.notifyObservers(newTransformNode);
}

int Scene::removeTransformNode(TransformNode* toRemove)
{
  auto it   = ::std::find_if(transformNodes.begin(), transformNodes.end(),
                           [&toRemove](const TransformNode* transformNode) {
                             return transformNode == toRemove;
                           });
  int index = static_cast<int>(it - transformNodes.begin());
  if (it != transformNodes.end()) {
    // Remove from the scene if found
    transformNodes.erase(it);
  }

  onTransformNodeRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeSkeleton(Skeleton* toRemove)
{
  auto it   = ::std::find_if(skeletons.begin(), skeletons.end(),
                           [&toRemove](const unique_ptr_t<Skeleton>& skeleton) {
                             return skeleton.get() == toRemove;
                           });
  int index = static_cast<int>(it - skeletons.begin());
  if (it != skeletons.end()) {
    // Remove from the scene if found
    skeletons.erase(it);
  }

  return index;
}

int Scene::removeMorphTargetManager(MorphTargetManager* toRemove)
{
  auto it = ::std::find_if(
    morphTargetManagers.begin(), morphTargetManagers.end(),
    [&toRemove](const unique_ptr_t<MorphTargetManager>& morphTargetManager) {
      return morphTargetManager.get() == toRemove;
    });
  int index = static_cast<int>(it - morphTargetManagers.begin());
  if (it != morphTargetManagers.end()) {
    // Remove from the scene if found
    morphTargetManagers.erase(it);
  }

  return index;
}

int Scene::removeLight(Light* toRemove)
{
  auto it   = ::std::find_if(lights.begin(), lights.end(),
                           [&toRemove](const unique_ptr_t<Light>& light) {
                             return light.get() == toRemove;
                           });
  int index = static_cast<int>(it - lights.begin());
  if (it != lights.end()) {
    // Remove from the scene if mesh found
    lights.erase(it);
    sortLightsByPriority();
  }

  onLightRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeCamera(Camera* toRemove)
{
  auto it1  = ::std::find_if(cameras.begin(), cameras.end(),
                            [&toRemove](const unique_ptr_t<Camera>& camera) {
                              return camera.get() == toRemove;
                            });
  int index = static_cast<int>(it1 - cameras.begin());
  if (it1 != cameras.end()) {
    // Remove from the scene if camera found
    cameras.erase(it1);
  }
  // Remove from activeCameras
  auto it2 = ::std::find(activeCameras.begin(), activeCameras.end(), toRemove);
  if (it2 != activeCameras.end()) {
    // Remove from the scene if camera found
    activeCameras.erase(it2);
  }
  // Reset the activeCamera
  if (activeCamera == toRemove) {
    if (!cameras.empty()) {
      activeCamera = cameras[0].get();
    }
    else {
      activeCamera = nullptr;
    }
  }

  onCameraRemovedObservable.notifyObservers(toRemove);

  return index;
}

void Scene::addLight(unique_ptr_t<Light>&& newLight)
{
  auto _newLight = newLight.get();
  lights.emplace_back(::std::move(newLight));
  sortLightsByPriority();

  onNewLightAddedObservable.notifyObservers(_newLight);
}

void Scene::sortLightsByPriority()
{
  if (requireLightSorting) {
    ::std::sort(lights.begin(), lights.end(),
                [](const unique_ptr_t<Light>& a, const unique_ptr_t<Light>& b) {
                  return Light::compareLightsPriority(a.get(), b.get());
                });
  }
}

void Scene::addCamera(unique_ptr_t<Camera>&& newCamera)
{
  auto _newCamera = newCamera.get();
  cameras.emplace_back(::std::move(newCamera));
  onNewCameraAddedObservable.notifyObservers(_newCamera);
}

void Scene::switchActiveCamera(Camera* newCamera, bool attachControl)
{
  auto canvas = _engine->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  if (activeCamera) {
    activeCamera->detachControl(canvas);
  }

  activeCamera = newCamera;
  if (attachControl) {
    newCamera->attachControl(canvas);
  }
}

Camera* Scene::setActiveCameraByID(const string_t& id)
{
  auto camera = getCameraByID(id);

  if (camera) {
    activeCamera = camera;
    return camera;
  }

  return nullptr;
}

Camera* Scene::setActiveCameraByName(const string_t& name)
{
  auto camera = getCameraByName(name);

  if (camera) {
    activeCamera = camera;
    return camera;
  }

  return nullptr;
}

Material* Scene::getMaterialByID(const string_t& id)
{
  auto it = ::std::find_if(materials.begin(), materials.end(),
                           [&id](const unique_ptr_t<Material>& material) {
                             return material->id == id;
                           });

  return (it == materials.end()) ? nullptr : (*it).get();
}

Material* Scene::getMaterialByName(const string_t& name)
{
  auto it = ::std::find_if(materials.begin(), materials.end(),
                           [&name](const unique_ptr_t<Material>& material) {
                             return material->name == name;
                           });

  return (it == materials.end()) ? nullptr : (*it).get();
}

LensFlareSystem* Scene::getLensFlareSystemByName(const string_t& name)
{
  auto it = ::std::find_if(
    lensFlareSystems.begin(), lensFlareSystems.end(),
    [&name](const unique_ptr_t<LensFlareSystem>& lensFlareSystem) {
      return lensFlareSystem->name == name;
    });

  return (it == lensFlareSystems.end()) ? nullptr : (*it).get();
}

LensFlareSystem* Scene::getLensFlareSystemByID(const string_t& id)
{
  auto it = ::std::find_if(
    lensFlareSystems.begin(), lensFlareSystems.end(),
    [&id](const unique_ptr_t<LensFlareSystem>& lensFlareSystem) {
      return lensFlareSystem->id == id;
    });

  return (it == lensFlareSystems.end()) ? nullptr : (*it).get();
}

vector_t<Camera*> Scene::getCameras() const
{
  vector_t<Camera*> _cameras;
  _cameras.reserve(cameras.size());
  ::std::for_each(cameras.begin(), cameras.end(),
                  [&_cameras](const unique_ptr_t<Camera>& camera) {
                    return _cameras.emplace_back(camera.get());
                  });

  return _cameras;
}

Camera* Scene::getCameraByID(const string_t& id)
{
  auto it = ::std::find_if(
    cameras.begin(), cameras.end(),
    [&id](const unique_ptr_t<Camera>& camera) { return camera->id == id; });

  return (it == cameras.end()) ? nullptr : (*it).get();
}

Camera* Scene::getCameraByUniqueID(unsigned int uniqueId)
{
  auto it = ::std::find_if(cameras.begin(), cameras.end(),
                           [uniqueId](const unique_ptr_t<Camera>& camera) {
                             return camera->uniqueId == uniqueId;
                           });

  return (it == cameras.end()) ? nullptr : (*it).get();
}

Camera* Scene::getCameraByName(const string_t& name)
{
  auto it = ::std::find_if(cameras.begin(), cameras.end(),
                           [&name](const unique_ptr_t<Camera>& camera) {
                             return camera->name == name;
                           });

  return (it == cameras.end()) ? nullptr : (*it).get();
}

Bone* Scene::getBoneByID(const string_t& id)
{
  for (auto& skeleton : skeletons) {
    for (auto& bone : skeleton->bones) {
      if (bone->id == id) {
        return bone.get();
      }
    }
  }

  return nullptr;
}

Bone* Scene::getBoneByName(const string_t& name)
{
  for (auto& skeleton : skeletons) {
    for (auto& bone : skeleton->bones) {
      if (bone->name == name) {
        return bone.get();
      }
    }
  }

  return nullptr;
}

Light* Scene::getLightByName(const string_t& name)
{
  auto it = ::std::find_if(
    lights.begin(), lights.end(),
    [&name](const unique_ptr_t<Light>& light) { return light->name == name; });

  return (it == lights.end()) ? nullptr : (*it).get();
}

Light* Scene::getLightByID(const string_t& id)
{
  auto it = ::std::find_if(
    lights.begin(), lights.end(),
    [&id](const unique_ptr_t<Light>& light) { return light->id == id; });

  return (it == lights.end()) ? nullptr : (*it).get();
}

Light* Scene::getLightByUniqueID(unsigned int uniqueId)
{
  auto it = ::std::find_if(lights.begin(), lights.end(),
                           [uniqueId](const unique_ptr_t<Light>& light) {
                             return light->uniqueId == uniqueId;
                           });

  return (it == lights.end()) ? nullptr : (*it).get();
}

IParticleSystem* Scene::getParticleSystemByID(const string_t& id)
{
  auto it = ::std::find_if(
    particleSystems.begin(), particleSystems.end(),
    [&id](const unique_ptr_t<IParticleSystem>& particleSystem) {
      return particleSystem->id == id;
    });

  return (it == particleSystems.end()) ? nullptr : (*it).get();
}

Geometry* Scene::getGeometryByID(const string_t& id)
{
  auto it = ::std::find_if(_geometries.begin(), _geometries.end(),
                           [&id](const unique_ptr_t<Geometry>& geometry) {
                             return geometry->id == id;
                           });

  return (it == _geometries.end()) ? nullptr : (*it).get();
}

bool Scene::pushGeometry(unique_ptr_t<Geometry>&& geometry, bool force)
{
  if (!force && getGeometryByID(geometry->id)) {
    return false;
  }

  auto _geometry = geometry.get();
  _geometries.emplace_back(::std::move(geometry));

  // Notify the collision coordinator
  if (collisionCoordinator) {
    collisionCoordinator->onGeometryAdded(_geometry);
  }

  onNewGeometryAddedObservable.notifyObservers(_geometry);

  return true;
}

bool Scene::removeGeometry(Geometry* geometry)
{
  auto it
    = ::std::find_if(_geometries.begin(), _geometries.end(),
                     [&geometry](const unique_ptr_t<Geometry>& _geometry) {
                       return _geometry.get() == geometry;
                     });
  if (it != _geometries.end()) {
    _geometries.erase(it);

    // notify the collision coordinator
    if (collisionCoordinator) {
      collisionCoordinator->onGeometryDeleted(geometry);
    }

    onGeometryRemovedObservable.notifyObservers(geometry);

    return true;
  }
  return false;
}

vector_t<unique_ptr_t<Geometry>>& Scene::getGeometries()
{
  return _geometries;
}

AbstractMesh* Scene::getMeshByID(const string_t& id)
{
  auto it = ::std::find_if(
    meshes.begin(), meshes.end(),
    [&id](const unique_ptr_t<AbstractMesh>& mesh) { return mesh->id == id; });

  return (it == meshes.end()) ? nullptr : (*it).get();
}

vector_t<AbstractMesh*> Scene::getMeshesByID(const string_t& id)
{
  vector_t<AbstractMesh*> filteredMeshes;
  ::std::for_each(
    meshes.begin(), meshes.end(),
    [&filteredMeshes, &id](const unique_ptr_t<AbstractMesh>& mesh) {
      if (mesh->id == id) {
        filteredMeshes.emplace_back(mesh.get());
      }
    });
  return filteredMeshes;
}

TransformNode* Scene::getTransformNodeByID(const string_t& id)
{
  auto it = ::std::find_if(transformNodes.begin(), transformNodes.end(),
                           [&id](const TransformNode* transformNode) {
                             return transformNode->id == id;
                           });

  return (it == transformNodes.end()) ? nullptr : (*it);
}

vector_t<TransformNode*> Scene::getTransformNodesByID(const string_t& id)
{
  vector_t<TransformNode*> filteredTransformNodes;
  ::std::for_each(transformNodes.begin(), transformNodes.end(),
                  [&filteredTransformNodes, &id](TransformNode* transformNode) {
                    if (transformNode->id == id) {
                      filteredTransformNodes.emplace_back(transformNode);
                    }
                  });
  return filteredTransformNodes;
}

AbstractMesh* Scene::getMeshByUniqueID(unsigned int uniqueId)
{
  auto it = ::std::find_if(meshes.begin(), meshes.end(),
                           [&uniqueId](const unique_ptr_t<AbstractMesh>& mesh) {
                             return mesh->uniqueId == uniqueId;
                           });

  return (it == meshes.end()) ? nullptr : (*it).get();
}

AbstractMesh* Scene::getLastMeshByID(const string_t& id)
{
  if (meshes.empty()) {
    return nullptr;
  }

  for (size_t index = meshes.size(); index > 0; --index) {
    if (meshes[index - 1]->id == id) {
      return meshes[index - 1].get();
    }
  }

  return nullptr;
}

Node* Scene::getLastEntryByID(const string_t& id)
{
  size_t index;
  for (index = meshes.size(); index > 0; --index) {
    if (meshes[index - 1]->id == id) {
      return meshes[index - 1].get();
    }
  }

  for (index = transformNodes.size(); index > 0; --index) {
    if (transformNodes[index - 1]->id == id) {
      return transformNodes[index - 1];
    }
  }

  for (index = cameras.size(); index > 0; --index) {
    if (cameras[index - 1]->id == id) {
      return cameras[index - 1].get();
    }
  }

  for (index = lights.size(); index > 0; --index) {
    if (lights[index - 1]->id == id) {
      return lights[index - 1].get();
    }
  }

  return nullptr;
}

Node* Scene::getNodeByID(const string_t& id)
{
  auto mesh = getMeshByID(id);

  if (mesh) {
    return mesh;
  }

  auto light = getLightByID(id);

  if (light) {
    return light;
  }

  auto camera = getCameraByID(id);

  if (camera) {
    return camera;
  }

  auto bone = getBoneByID(id);

  return bone;
}

Node* Scene::getNodeByName(const string_t& name)
{
  auto mesh = getMeshByName(name);

  if (mesh) {
    return mesh;
  }

  auto light = getLightByName(name);

  if (light) {
    return light;
  }

  auto camera = getCameraByName(name);

  if (camera) {
    return camera;
  }

  auto bone = getBoneByName(name);

  return bone;
}

AbstractMesh* Scene::getMeshByName(const string_t& name)
{
  auto it = ::std::find_if(meshes.begin(), meshes.end(),
                           [&name](const unique_ptr_t<AbstractMesh>& mesh) {
                             return mesh->name == name;
                           });

  return (it == meshes.end()) ? nullptr : (*it).get();
}

TransformNode* Scene::getTransformNodeByName(const string_t& name)
{
  auto it = ::std::find_if(transformNodes.begin(), transformNodes.end(),
                           [&name](const TransformNode* transformNode) {
                             return transformNode->name == name;
                           });

  return (it == transformNodes.end()) ? nullptr : (*it);
}

Sound* Scene::getSoundByName(const string_t& /*name*/)
{
  return nullptr;
}

Skeleton* Scene::getLastSkeletonByID(const string_t& id)
{
  for (size_t index = skeletons.size() - 1;; --index) {
    if (skeletons[index]->id == id) {
      return skeletons[index].get();
    }

    if (index == 0) {
      break;
    }
  }

  return nullptr;
}

Skeleton* Scene::getSkeletonById(const string_t& id)
{
  auto it = ::std::find_if(skeletons.begin(), skeletons.end(),
                           [&id](const unique_ptr_t<Skeleton>& skeleton) {
                             return skeleton->id == id;
                           });

  return (it == skeletons.end()) ? nullptr : (*it).get();
}

Skeleton* Scene::getSkeletonByName(const string_t& name)
{
  auto it = ::std::find_if(skeletons.begin(), skeletons.end(),
                           [&name](const unique_ptr_t<Skeleton>& skeleton) {
                             return skeleton->name == name;
                           });

  return (it == skeletons.end()) ? nullptr : (*it).get();
}

MorphTargetManager* Scene::getMorphTargetManagerById(unsigned int id)
{
  auto it = ::std::find_if(
    morphTargetManagers.begin(), morphTargetManagers.end(),
    [&id](const unique_ptr_t<MorphTargetManager>& morphTargetManager) {
      return morphTargetManager->uniqueId() == id;
    });

  return (it == morphTargetManagers.end()) ? nullptr : (*it).get();
}

bool Scene::isActiveMesh(AbstractMesh* mesh)
{
  return ::std::find(_activeMeshes.begin(), _activeMeshes.end(), mesh)
         != _activeMeshes.end();
}

HighlightLayer* Scene::getHighlightLayerByName(const string_t& name)
{
  auto it = ::std::find_if(
    highlightLayers.begin(), highlightLayers.end(),
    [&name](const unique_ptr_t<HighlightLayer>& highlightLayer) {
      return highlightLayer->name == name;
    });

  return (it == highlightLayers.end()) ? nullptr : (*it).get();
}

string_t Scene::uid()
{
  if (_uid.empty()) {
    _uid = Tools::RandomId();
  }
  return _uid;
}

void Scene::_evaluateSubMesh(SubMesh* subMesh, AbstractMesh* mesh)
{
  if (mesh->alwaysSelectAsActiveMesh || mesh->subMeshes.size() == 1
      || subMesh->isInFrustum(_frustumPlanes)) {
    auto material = subMesh->getMaterial();

    if (mesh->showSubMeshesBoundingBox) {
      auto boundingInfo = subMesh->getBoundingInfo();
      getBoundingBoxRenderer()->renderList.emplace_back(
        boundingInfo.boundingBox);
    }

    if (material) {
      // Render targets
      if (material->getRenderTargetTextures) {
        if (::std::find(_processedMaterials.begin(), _processedMaterials.end(),
                        material)
            == _processedMaterials.end()) {
          _processedMaterials.emplace_back(material);
          for (auto& renderTarget : material->getRenderTargetTextures()) {
            if (::std::find(_renderTargets.begin(), _renderTargets.end(),
                            renderTarget)
                == _renderTargets.end()) {
              _renderTargets.emplace_back(renderTarget);
            }
          }
        }
      }

      // Dispatch
      _activeIndices.addCount(subMesh->indexCount, false);
      _renderingManager->dispatch(subMesh);
    }
  }
}

bool Scene::_isInIntermediateRendering() const
{
  return _intermediateRendering;
}

Scene& Scene::freezeActiveMeshes()
{
  _evaluateActiveMeshes();
  _activeMeshesFrozen = true;
  return *this;
}

Scene& Scene::unfreezeActiveMeshes()
{
  _activeMeshesFrozen = false;
  return *this;
}

void Scene::_evaluateActiveMeshes()
{
  if (_activeMeshesFrozen && !_activeMeshes.empty()) {
    return;
  }

  if (!activeCamera) {
    return;
  }

  onBeforeActiveMeshesEvaluationObservable.notifyObservers(this);

  activeCamera->_activeMeshes.clear();
  _activeMeshes.clear();
  _renderingManager->reset();
  _processedMaterials.clear();
  _activeParticleSystems.clear();
  _activeSkeletons.clear();
  _softwareSkinnedMeshes.clear();
  if (_boundingBoxRenderer) {
    _boundingBoxRenderer->reset();
  }

  if (!_frustumPlanesSet) {
    _frustumPlanes    = Frustum::GetPlanes(_transformMatrix);
    _frustumPlanesSet = true;
  }
  else {
    Frustum::GetPlanesToRef(_transformMatrix, _frustumPlanes);
  }

  // Meshes
  vector_t<AbstractMesh*> _meshes;

  if (_selectionOctree) { // Octree
    _meshes = _selectionOctree->select(_frustumPlanes);
  }
  else { // Full scene traversal
    _meshes = getMeshes();
  }

  for (auto& mesh : meshes) {
    if (mesh->isBlocked()) {
      continue;
    }

    _totalVertices.addCount(mesh->getTotalVertices(), false);

    if (!mesh->isReady() || !mesh->isEnabled()) {
      continue;
    }

    mesh->computeWorldMatrix();

    // Intersections
    if (mesh->actionManager
        && mesh->actionManager->hasSpecificTriggers(
             {ActionManager::OnIntersectionEnterTrigger,
              ActionManager::OnIntersectionExitTrigger})) {
      if (::std::find(_meshesForIntersections.begin(),
                      _meshesForIntersections.end(), mesh.get())
          == _meshesForIntersections.end()) {
        _meshesForIntersections.emplace_back(mesh.get());
      }
    }

    // Switch to current LOD
    auto meshLOD = mesh->getLOD(activeCamera);

    if (!meshLOD) {
      continue;
    }

    mesh->_preActivate();

    if (mesh->alwaysSelectAsActiveMesh
        || ((mesh->isVisible && mesh->visibility > 0)
            && ((mesh->layerMask() & activeCamera->layerMask) != 0)
            && mesh->isInFrustum(_frustumPlanes))) {
      _activeMeshes.emplace_back(dynamic_cast<Mesh*>(mesh.get()));
      activeCamera->_activeMeshes.emplace_back(_activeMeshes.back());

      mesh->_activate(_renderId);
      if (meshLOD != mesh.get()) {
        meshLOD->_activate(_renderId);
      }

      _activeMesh(mesh.get(), meshLOD);
    }
  }

  // Particle systems
  if (particlesEnabled) {
    onBeforeParticlesRenderingObservable.notifyObservers(this);
    for (auto& particleSystem : particleSystems) {
      if (!particleSystem->isStarted()) {
        continue;
      }

      if (particleSystem->emitter.is<AbstractMesh*>()
          && particleSystem->emitter.get<AbstractMesh*>()->isEnabled()) {
        _activeParticleSystems.emplace_back(particleSystem.get());
        particleSystem->animate();
        _renderingManager->dispatchParticles(particleSystem.get());
      }
    }
    onAfterParticlesRenderingObservable.notifyObservers(this);
  }
}

void Scene::_activeMesh(AbstractMesh* sourceMesh, AbstractMesh* mesh)
{
  if (mesh->skeleton() && skeletonsEnabled()) {
    if (::std::find(_activeSkeletons.begin(), _activeSkeletons.end(),
                    mesh->skeleton())
        == _activeSkeletons.end()) {
      _activeSkeletons.emplace_back(mesh->skeleton());
      mesh->skeleton()->prepare();
    }

    if (!mesh->computeBonesUsingShaders()) {
      auto _mesh = dynamic_cast<Mesh*>(mesh);
      if (_mesh) {
        if (::std::find(_softwareSkinnedMeshes.begin(),
                        _softwareSkinnedMeshes.end(), _mesh)
            == _softwareSkinnedMeshes.end()) {
          _softwareSkinnedMeshes.emplace_back(_mesh);
        }
      }
    }
  }

  if (sourceMesh->showBoundingBox || forceShowBoundingBoxes) {
    auto boundingInfo = sourceMesh->getBoundingInfo();
    getBoundingBoxRenderer()->renderList.emplace_back(boundingInfo.boundingBox);
  }

  if (mesh && !mesh->subMeshes.empty()) {
    // Submeshes Octrees
    vector_t<SubMesh*> subMeshes;

    if (mesh->_submeshesOctree && mesh->useOctreeForRenderingSelection) {
      subMeshes = mesh->_submeshesOctree->select(_frustumPlanes);
    }
    else {
      subMeshes.reserve(mesh->subMeshes.size());
      for (auto& subMesh : mesh->subMeshes) {
        subMeshes.emplace_back(subMesh.get());
      }
    }

    for (auto& subMesh : mesh->subMeshes) {
      _evaluateSubMesh(subMesh.get(), mesh);
    }
  }
}

void Scene::updateTransformMatrix(bool force)
{
  if (!activeCamera) {
    return;
  }

  setTransformMatrix(activeCamera->getViewMatrix(force),
                     activeCamera->getProjectionMatrix(force));
}

void Scene::updateAlternateTransformMatrix(Camera* alternateCamera)
{
  _setAlternateTransformMatrix(alternateCamera->getViewMatrix(),
                               alternateCamera->getProjectionMatrix());
}

void Scene::_renderForCamera(Camera* camera)
{
  if (camera && camera->_skipRendering) {
    return;
  }

  auto engine = _engine;

  activeCamera = camera;

  if (!activeCamera) {
    BABYLON_LOG_ERROR("Scene", "Active camera not set");
    return;
  }

  Tools::StartPerformanceCounter("Rendering camera " + activeCamera->name);

  // Viewport
  engine->setViewport(activeCamera->viewport);

  // Camera
  resetCachedMaterial();
  ++_renderId;
  activeCamera->update();
  updateTransformMatrix();

  if (camera->_alternateCamera) {
    updateAlternateTransformMatrix(camera->_alternateCamera);
    _alternateRendering = true;
  }

  onBeforeCameraRenderObservable.notifyObservers(activeCamera);

  // Meshes
  _evaluateActiveMeshes();

  // Software skinning
  for (auto& mesh : _softwareSkinnedMeshes) {
    mesh->applySkeleton(mesh->skeleton());
  }

  // Render targets
  OnBeforeRenderTargetsRenderObservable.notifyObservers(this);
  bool needsRestoreFrameBuffer = false;

  // if (camera->customRenderTargets && !camera.customRenderTargets.empty) {
  //  _renderTargets.concatWithNoDuplicate(camera->customRenderTargets);
  // }

  if (renderTargetsEnabled && !_renderTargets.empty()) {
    _intermediateRendering = true;
    Tools::StartPerformanceCounter("Render targets", !_renderTargets.empty());
    for (auto& renderTarget : _renderTargets) {
      if (renderTarget->_shouldRender()) {
        ++_renderId;
        bool hasSpecialRenderTargetCamera
          = renderTarget->activeCamera
            && renderTarget->activeCamera != activeCamera;
        renderTarget->render(hasSpecialRenderTargetCamera,
                             dumpNextRenderTargets);
      }
    }
    Tools::EndPerformanceCounter("Render targets", !_renderTargets.empty());

    _intermediateRendering = false;
    ++_renderId;

    needsRestoreFrameBuffer = true; // Restore back buffer
  }

  // Render HighlightLayer Texture
  auto stencilState     = _engine->getStencilBuffer();
  bool renderhighlights = false;
  if (renderTargetsEnabled && !highlightLayers.empty()) {
    _intermediateRendering = true;
    for (auto& highlightLayer : highlightLayers) {
      if (highlightLayer->shouldRender()
          && (!highlightLayer->camera()
              || (highlightLayer->camera()->cameraRigMode
                    == Camera::RIG_MODE_NONE
                  && camera == highlightLayer->camera())
              || (highlightLayer->camera()->cameraRigMode
                    != Camera::RIG_MODE_NONE
                  && (::std::find(highlightLayer->camera()->_rigCameras.begin(),
                                  highlightLayer->camera()->_rigCameras.end(),
                                  camera)
                      != highlightLayer->camera()->_rigCameras.end())))) {

        renderhighlights = true;

        auto renderTarget = highlightLayer->mainTexture();
        if (renderTarget->_shouldRender()) {
          ++_renderId;
          renderTarget->render(false, false);
          needsRestoreFrameBuffer = true;
        }
      }
    }

    _intermediateRendering = false;
    ++_renderId;
  }

  if (needsRestoreFrameBuffer) {
    engine->restoreDefaultFramebuffer(); // Restore back buffer
  }

  OnAfterRenderTargetsRenderObservable.notifyObservers(this);

  // Prepare Frame
  postProcessManager->_prepareFrame(nullptr);

  // Backgrounds
  if (!layers.empty()) {
    engine->setDepthBuffer(false);
    for (auto& layer : layers) {
      if (layer->isBackground
          && ((layer->layerMask & activeCamera->layerMask) != 0)) {
        layer->render();
      }
    }
    engine->setDepthBuffer(true);
  }

  // Activate HighlightLayer stencil
  if (renderhighlights) {
    _engine->setStencilBuffer(true);
  }

  // Render
  onBeforeDrawPhaseObservable.notifyObservers(this);
  _renderingManager->render(nullptr, {}, true, true);
  onAfterDrawPhaseObservable.notifyObservers(this);

  // Restore HighlightLayer stencil
  if (renderhighlights) {
    _engine->setStencilBuffer(stencilState);
  }

  // Bounding boxes
  if (_boundingBoxRenderer) {
    _boundingBoxRenderer->render();
  }

  // Lens flares
  if (lensFlaresEnabled) {
    Tools::StartPerformanceCounter("Lens flares", !lensFlareSystems.empty());
    for (auto& lensFlareSystem : lensFlareSystems) {
      if ((camera->layerMask & lensFlareSystem->layerMask) != 0) {
        lensFlareSystem->render();
      }
    }
    Tools::EndPerformanceCounter("Lens flares", !lensFlareSystems.empty());
  }

  // Foregrounds
  if (!layers.empty()) {
    engine->setDepthBuffer(false);
    for (auto& layer : layers) {
      if (!layer->isBackground
          && ((layer->layerMask & activeCamera->layerMask) != 0)) {
        layer->render();
      }
    }
    engine->setDepthBuffer(true);
  }

  // Highlight Layer
  if (renderhighlights) {
    engine->setDepthBuffer(false);
    for (auto& highlightLayer : highlightLayers) {
      if (highlightLayer->shouldRender()) {
        highlightLayer->render();
      }
    }
    engine->setDepthBuffer(true);
  }

  // Finalize frame
  postProcessManager->_finalizeFrame(camera->isIntermediate);

  // Reset some special arrays
  _renderTargets.clear();

  _alternateRendering = false;

  onAfterCameraRenderObservable.notifyObservers(activeCamera);

  Tools::EndPerformanceCounter("Rendering camera " + activeCamera->name);
}

void Scene::_processSubCameras(Camera* camera)
{
  if (camera->cameraRigMode == Camera::RIG_MODE_NONE) {
    _renderForCamera(camera);
    return;
  }

  // Update camera
  if (activeCamera) {
    activeCamera->update();
  }

  // rig cameras
  for (auto& rigCamera : camera->_rigCameras) {
    _renderForCamera(rigCamera);
  }

  activeCamera = camera;
  setTransformMatrix(activeCamera->getViewMatrix(),
                     activeCamera->getProjectionMatrix());
}

void Scene::_checkIntersections()
{
}

void Scene::render()
{
  if (isDisposed()) {
    return;
  }

  _activeParticles.fetchNewFrame();
  _totalVertices.fetchNewFrame();
  _activeIndices.fetchNewFrame();
  _activeBones.fetchNewFrame();
  _meshesForIntersections.clear();
  resetCachedMaterial();

  onBeforeAnimationsObservable.notifyObservers(this);

  // Actions
  if (actionManager) {
    actionManager->processTrigger(ActionManager::OnEveryFrameTrigger);
  }

  // Simplification Queue
  if (simplificationQueue && !simplificationQueue->running) {
    simplificationQueue->executeNext();
  }

  if (_engine->isDeterministicLockStep()) {
    auto deltaTime
      = (::std::max(static_cast<float>(Scene::MinDeltaTime.count()),
                    ::std::min(_engine->getDeltaTime() * 1000.f,
                               static_cast<float>(Scene::MaxDeltaTime.count())))
         / 1000.f)
        + _timeAccumulator;

    auto defaultFPS = (60.f / 1000.f);

    auto defaultFrameTime = 1000.f / 60.f; // frame time in MS

    if (_physicsEngine) {
      defaultFrameTime = _physicsEngine->getTimeStep();
    }

    auto stepsTaken = 0u;

    auto maxSubSteps = _engine->getLockstepMaxSteps();

    // compute the amount of fixed steps we should have taken since the last
    // step
    auto internalSteps = static_cast<unsigned int>(
      ::std::floor(deltaTime / (1000.f * defaultFPS)));
    internalSteps = ::std::min(internalSteps, maxSubSteps);

    do {
      onBeforeStepObservable.notifyObservers(this);

      // Animations
      _animationRatio = defaultFrameTime * defaultFPS;
      _animate();
      onAfterAnimationsObservable.notifyObservers(this);

      // Physics
      if (_physicsEngine) {
        onBeforePhysicsObservable.notifyObservers(this);
        _physicsEngine->_step(defaultFPS);
        onAfterPhysicsObservable.notifyObservers(this);
      }

      onAfterStepObservable.notifyObservers(this);
      ++_currentStepId;

      if ((internalSteps > 1) && (_currentInternalStep != internalSteps - 1)) {
        _evaluateActiveMeshes();
      }

      ++stepsTaken;
      deltaTime -= defaultFrameTime;

    } while (deltaTime > 0 && stepsTaken < maxSubSteps);

    _timeAccumulator = deltaTime;
  }
  else {
    // Animations
    const float deltaTime = ::std::max(
      Time::fpMillisecondsDuration<float>(Scene::MinDeltaTime),
      ::std::min(_engine->getDeltaTime(),
                 Time::fpMillisecondsDuration<float>(Scene::MaxDeltaTime)));
    _animationRatio = deltaTime * (60.f / 1000.f);
    _animate();
    onAfterAnimationsObservable.notifyObservers(this);

    // Physics
    if (_physicsEngine) {
      onBeforePhysicsObservable.notifyObservers(this);
      _physicsEngine->_step(deltaTime / 1000.f);
      onAfterPhysicsObservable.notifyObservers(this);
    }
  }

  // Before render
  onBeforeRenderObservable.notifyObservers(this);

  // Customs render targets
  OnBeforeRenderTargetsRenderObservable.notifyObservers(this);
  auto engine              = getEngine();
  auto currentActiveCamera = activeCamera;
  if (renderTargetsEnabled) {
    Tools::StartPerformanceCounter("Custom render targets",
                                   !customRenderTargets.empty());
    _intermediateRendering = true;
    for (auto& renderTarget : customRenderTargets) {
      if (renderTarget->_shouldRender()) {
        ++_renderId;

        activeCamera = renderTarget->activeCamera ? renderTarget->activeCamera :
                                                    activeCamera;

        if (!activeCamera) {
          BABYLON_LOG_ERROR("Scene", "Active camera not set");
          return;
        }

        // Viewport
        engine->setViewport(activeCamera->viewport);

        // Camera
        updateTransformMatrix();

        renderTarget->render(currentActiveCamera != activeCamera,
                             dumpNextRenderTargets);
      }
    }
    Tools::EndPerformanceCounter("Custom render targets",
                                 !customRenderTargets.empty());

    _intermediateRendering = false;
    ++_renderId;
  }

  // Restore back buffer
  if (!customRenderTargets.empty()) {
    engine->restoreDefaultFramebuffer();
  }

  // Restore back buffer
  if (!customRenderTargets.empty()) {
    engine->restoreDefaultFramebuffer();
  }

  OnAfterRenderTargetsRenderObservable.notifyObservers(this);
  activeCamera = currentActiveCamera;

  // Procedural textures
  if (proceduralTexturesEnabled) {
    Tools::StartPerformanceCounter("Procedural textures",
                                   !_proceduralTextures.empty());
    for (auto& proceduralTexture : _proceduralTextures) {
      if (proceduralTexture->_shouldRender()) {
        proceduralTexture->render();
      }
    }
    Tools::EndPerformanceCounter("Procedural textures",
                                 !_proceduralTextures.empty());
  }

  // Clear
  if (_engine->getRenderingCanvas()->onlyRenderBoundingClientRect()) {
    const auto& rec = _engine->getRenderingCanvas()->getBoundingClientRect();
    _engine->scissorClear(rec.left, rec.bottom, rec.width, rec.height,
                          clearColor);
  }
  else {
    if (autoClearDepthAndStencil || autoClear) {
      _engine->clear(clearColor,
                     autoClear || forceWireframe || forcePointsCloud(),
                     autoClearDepthAndStencil, autoClearDepthAndStencil);
    }
  }

  // Shadows
  if (shadowsEnabled()) {
    for (auto& light : lights) {
      auto shadowGenerator = light->getShadowGenerator();
      if (light->isEnabled() && light->shadowEnabled && shadowGenerator) {
        auto shadowMap  = shadowGenerator->getShadowMap();
        auto& _textures = shadowGenerator->getShadowMap()->getScene()->textures;
        auto it         = ::std::find_if(
          _textures.begin(), _textures.end(),
          [&shadowMap](const unique_ptr_t<BaseTexture>& texture) {
            return texture.get() == shadowMap;
          });
        if (it != _textures.end()) {
          _renderTargets.emplace_back(shadowMap);
        }
      }
    }
  }

  // Depth renderer
  if (_depthRenderer) {
    _renderTargets.emplace_back(_depthRenderer->getDepthMap());
  }

  // Geometry renderer
  if (_geometryBufferRenderer) {
    _renderTargets.emplace_back(_geometryBufferRenderer->getGBuffer());
  }

  // RenderPipeline
  if (_postProcessRenderPipelineManager) {
    _postProcessRenderPipelineManager->update();
  }

  // Multi-cameras?
  if (!activeCameras.empty()) {
    for (unsigned int cameraIndex = 0; cameraIndex < activeCameras.size();
         ++cameraIndex) {
      if (cameraIndex > 0) {
        _engine->clear(true, true);
      }

      _processSubCameras(activeCameras[cameraIndex]);
    }
  }
  else {
    if (!activeCamera) {
      BABYLON_LOG_ERROR("Scene", "No camera defined");
      return;
    }

    _processSubCameras(activeCamera);
  }

  // Intersection checks
  _checkIntersections();

  // Update the audio listener attached to the camera
  if (_hasAudioEngine) {
    _updateAudioParameters();
  }

  // After render
  if (afterRender) {
    afterRender();
  }

  onAfterRenderObservable.notifyObservers(this);

  // Cleaning
  for (auto& item : _toBeDisposed) {
    if (item) {
      item->dispose();
      item = nullptr;
    }
  }

  _toBeDisposed.clear();

  if (dumpNextRenderTargets) {
    dumpNextRenderTargets = false;
  }

  _activeBones.addCount(0, true);
  _activeIndices.addCount(0, true);
  _activeParticles.addCount(0, true);
}

void Scene::_updateAudioParameters()
{
}

bool Scene::audioEnabled() const
{
  return _audioEnabled;
}

void Scene::setAudioEnabled(bool value)
{
  _audioEnabled = value;
  if (_hasAudioEngine) {
    if (_audioEnabled) {
      _enableAudio();
    }
    else {
      _disableAudio();
    }
  }
}

void Scene::_disableAudio()
{
}

void Scene::_enableAudio()
{
}

bool Scene::headphone() const
{
  return _headphone;
}

void Scene::setHeadphone(bool value)
{
  _headphone = value;
  if (_hasAudioEngine) {
    if (_headphone) {
      _switchAudioModeForHeadphones();
    }
    else {
      _switchAudioModeForNormalSpeakers();
    }
  }
}

void Scene::_switchAudioModeForHeadphones()
{
}

void Scene::_switchAudioModeForNormalSpeakers()
{
}

DepthRenderer* Scene::enableDepthRenderer()
{
  if (_depthRenderer) {
    return _depthRenderer.get();
  }

  _depthRenderer = ::std::make_unique<DepthRenderer>(this);

  return _depthRenderer.get();
}

void Scene::disableDepthRenderer()
{
  if (!_depthRenderer) {
    return;
  }

  _depthRenderer->dispose();
  _depthRenderer = nullptr;
}

GeometryBufferRenderer* Scene::enableGeometryBufferRenderer(float ratio)
{
  if (_geometryBufferRenderer) {
    return _geometryBufferRenderer.get();
  }

  _geometryBufferRenderer
    = ::std::make_unique<GeometryBufferRenderer>(this, ratio);
  if (!_geometryBufferRenderer->isSupported()) {
    _geometryBufferRenderer = nullptr;
  }

  return _geometryBufferRenderer.get();
}

void Scene::disableGeometryBufferRenderer()
{
  if (!_geometryBufferRenderer) {
    return;
  }

  _geometryBufferRenderer->dispose();
  _geometryBufferRenderer = nullptr;
}

void Scene::freezeMaterials()
{
  for (auto& material : materials) {
    material->freeze();
  }
}

void Scene::unfreezeMaterials()
{
  for (auto& material : materials) {
    material->unfreeze();
  }
}

void Scene::dispose(bool /*doNotRecurse*/)
{
  beforeRender = nullptr;
  afterRender  = nullptr;

  skeletons.clear();
  morphTargetManagers.clear();

  importedMeshesFiles.clear();

  stopAllAnimations();

  resetCachedMaterial();

  if (_depthRenderer) {
    _depthRenderer->dispose();
  }

  if (_gamepadManager) {
    _gamepadManager->dispose();
    _gamepadManager = nullptr;
  }

  // Smart arrays
  if (activeCamera) {
    activeCamera->_activeMeshes.clear();
    activeCamera = nullptr;
  }
  _activeMeshes.clear();
  _renderingManager->dispose();
  _processedMaterials.clear();
  _activeParticleSystems.clear();
  _activeSkeletons.clear();
  _softwareSkinnedMeshes.clear();
  _renderTargets.clear();

  if (_boundingBoxRenderer) {
    _boundingBoxRenderer->dispose();
  }
  _meshesForIntersections.clear();
  _toBeDisposed.clear();

  // Debug layer
  if (_debugLayer) {
    _debugLayer->hide();
  }

  // Events
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBeforeRenderObservable.clear();
  onAfterRenderObservable.clear();
  OnBeforeRenderTargetsRenderObservable.clear();
  OnAfterRenderTargetsRenderObservable.clear();
  onAfterStepObservable.clear();
  onBeforeStepObservable.clear();
  onBeforeActiveMeshesEvaluationObservable.clear();
  onAfterActiveMeshesEvaluationObservable.clear();
  onBeforeParticlesRenderingObservable.clear();
  onAfterParticlesRenderingObservable.clear();
  onBeforeSpritesRenderingObservable.clear();
  onAfterSpritesRenderingObservable.clear();
  onBeforeDrawPhaseObservable.clear();
  onAfterDrawPhaseObservable.clear();
  onBeforePhysicsObservable.clear();
  onAfterPhysicsObservable.clear();
  onBeforeAnimationsObservable.clear();
  onAfterAnimationsObservable.clear();
  onDataLoadedObservable.clear();

  detachControl();

  if (_hasAudioEngine) {
    // Release sounds & sounds tracks
    disposeSounds();
  }

  // Detach cameras
  auto canvas = _engine->getRenderingCanvas();
  if (canvas) {
    for (auto& camera : cameras) {
      camera->detachControl(canvas);
    }
  }

  // Release lights
  for (auto& light : lights) {
    light->dispose();
  }

  // Release meshes
  for (auto& mesh : meshes) {
    mesh->dispose(true);
  }

  // Release transform nodes
  for (auto& transformNode : transformNodes) {
    removeTransformNode(transformNode);
  }

  // Release cameras
  for (auto& camera : cameras) {
    camera->dispose();
  }

  // Release materials
  if (defaultMaterial()) {
    defaultMaterial()->dispose();
  }
  for (auto& multiMaterial : multiMaterials) {
    multiMaterial->dispose();
  }
  for (auto& material : materials) {
    material->dispose();
  }

  // Release particles
  for (auto& particleSystem : particleSystems) {
    particleSystem->dispose();
  }

  // Release sprites
  for (auto& spriteManager : spriteManagers) {
    spriteManager->dispose();
  }

  // Release postProcesses
  for (auto& postProcess : postProcesses) {
    postProcess->dispose();
  }

  // Release layers
  for (auto& layer : layers) {
    layer->dispose();
  }

  for (auto& highlightLayer : highlightLayers) {
    highlightLayer->dispose();
  }

  // Release textures
  for (auto& texture : textures) {
    texture->dispose();
  }

  // Release UBO
  _sceneUbo->dispose();

  if (_alternateSceneUbo) {
    _alternateSceneUbo->dispose();
  }

  // Post-processes
  postProcessManager->dispose();

  if (_postProcessRenderPipelineManager) {
    _postProcessRenderPipelineManager->dispose();
  }

  // Physics
  if (_physicsEngine) {
    disablePhysicsEngine();
  }

  // Remove from engine
  _engine->scenes.erase(
    ::std::remove(_engine->scenes.begin(), _engine->scenes.end(), this),
    _engine->scenes.end());

  _engine->wipeCaches();
  _isDisposed = true;
}

bool Scene::isDisposed() const
{
  return _isDisposed;
}

/** Release sounds & sounds tracks **/
void Scene::disposeSounds()
{
}

/** Octrees **/
MinMax Scene::getWorldExtends()
{
  Vector3 min(numeric_limits_t<float>::max(), numeric_limits_t<float>::max(),
              numeric_limits_t<float>::max());
  Vector3 max(numeric_limits_t<float>::lowest(),
              numeric_limits_t<float>::lowest(),
              numeric_limits_t<float>::lowest());
  for (auto& mesh : meshes) {
    if (mesh->subMeshes.empty() || mesh->infiniteDistance) {
      continue;
    }
    mesh->computeWorldMatrix(true);
    auto boundingInfo = mesh->getBoundingInfo();

    auto minBox = boundingInfo.boundingBox.minimumWorld;
    auto maxBox = boundingInfo.boundingBox.maximumWorld;

    Tools::CheckExtends(minBox, min, max);
    Tools::CheckExtends(maxBox, min, max);
  }

  return {min, max};
}

Octree<AbstractMesh*>* Scene::createOrUpdateSelectionOctree(size_t maxCapacity,
                                                            size_t maxDepth)
{
  if (!_selectionOctree) {
    _selectionOctree = new Octree<AbstractMesh*>(
      [](AbstractMesh* entry, OctreeBlock<AbstractMesh*>& block) {
        Octree<AbstractMesh*>::CreationFuncForMeshes(entry, block);
      },
      maxCapacity, maxDepth);
  }

  auto worldExtends = getWorldExtends();

  // Update octree
  auto _meshes = getMeshes();
  _selectionOctree->update(worldExtends.min, worldExtends.max, _meshes);

  return _selectionOctree;
}

/** Picking **/
Ray Scene::createPickingRay(int x, int y, Matrix* world, Camera* camera,
                            bool cameraViewSpace)
{
  auto result = Ray::Zero();

  createPickingRayToRef(x, y, world, result, camera, cameraViewSpace);

  return result;
}

Scene& Scene::createPickingRayToRef(int x, int y, Matrix* world, Ray& result,
                                    Camera* camera, bool cameraViewSpace)
{
  auto engine = _engine;

  if (!camera) {
    if (!activeCamera) {
      BABYLON_LOG_ERROR("Scene", "Active camera not set");
      return *this;
    }

    camera = activeCamera;
  }

  auto cameraViewport = camera->viewport;
  auto viewport       = cameraViewport.toGlobal(engine->getRenderWidth(),
                                          engine->getRenderHeight());

  auto identity = Matrix::Identity();

  // Moving coordinates to local viewport world
  float _x = static_cast<float>(x);
  _x /= static_cast<float>(_engine->getHardwareScalingLevel() - viewport.x);
  float _y = static_cast<float>(y);
  _y /= static_cast<float>(
    _engine->getHardwareScalingLevel()
    - (_engine->getRenderHeight() - viewport.y - viewport.height));

  result.update(_x, _y, static_cast<float>(viewport.width),
                static_cast<float>(viewport.height), world ? *world : identity,
                cameraViewSpace ? identity : camera->getViewMatrix(),
                camera->getProjectionMatrix());
  return *this;
}

Ray Scene::createPickingRayInCameraSpace(int x, int y, Camera* camera)
{
  auto result = Ray::Zero();

  createPickingRayInCameraSpaceToRef(x, y, result, camera);

  return result;
}

Scene& Scene::createPickingRayInCameraSpaceToRef(int x, int y, Ray& result,
                                                 Camera* camera)
{
  auto engine = _engine;

  if (!camera) {
    if (!activeCamera) {
      BABYLON_LOG_ERROR("Scene", "Active camera not set");
      return *this;
    }

    camera = activeCamera;
  }

  auto cameraViewport = camera->viewport;
  auto viewport       = cameraViewport.toGlobal(engine->getRenderWidth(),
                                          engine->getRenderHeight());
  auto identity       = Matrix::Identity();

  // Moving coordinates to local viewport world
  float _x = static_cast<float>(x);
  _x /= static_cast<float>(_engine->getHardwareScalingLevel() - viewport.x);
  float _y = static_cast<float>(y);
  _y /= static_cast<float>(
    _engine->getHardwareScalingLevel()
    - (_engine->getRenderHeight() - viewport.y - viewport.height));
  result.update(_x, _y, static_cast<float>(viewport.width),
                static_cast<float>(viewport.height), identity, identity,
                camera->getProjectionMatrix());
  return *this;
}

PickingInfo*
Scene::_internalPick(const ::std::function<Ray(Matrix& world)>& rayFunction,
                     const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                     bool fastCheck)
{
  PickingInfo* pickingInfo = nullptr;

  for (auto& mesh : meshes) {
    if (predicate) {
      if (!predicate(mesh.get())) {
        continue;
      }
    }
    else if (!mesh->isEnabled() || !mesh->isVisible || !mesh->isPickable) {
      continue;
    }

    auto world = mesh->getWorldMatrix();
    auto ray   = rayFunction(*world);

    auto result = mesh->intersects(ray, fastCheck);
    if (/*!result || */ !result.hit) {
      continue;
    }

    if (!fastCheck && pickingInfo != nullptr
        && result.distance >= pickingInfo->distance) {
      continue;
    }

    // pickingInfo = result;

    if (fastCheck) {
      break;
    }
  }

  return pickingInfo;
}

vector_t<PickingInfo*> Scene::_internalMultiPick(
  const ::std::function<Ray(Matrix& world)>& rayFunction,
  const ::std::function<bool(AbstractMesh* mesh)>& predicate)
{
  vector_t<PickingInfo*> pickingInfos;

  for (auto& mesh : meshes) {
    if (predicate) {
      if (!predicate(mesh.get())) {
        continue;
      }
    }
    else if (!mesh->isEnabled() || !mesh->isVisible || !mesh->isPickable) {
      continue;
    }

    auto world = mesh->getWorldMatrix();
    auto ray   = rayFunction(*world);

    auto result = mesh->intersects(ray, false);
    if (/*!result || */ !result.hit) {
      continue;
    }

    pickingInfos.emplace_back(&result);
  }

  return pickingInfos;
}

PickingInfo* Scene::_internalPickSprites(
  const Ray& ray, const ::std::function<bool(Sprite* sprite)>& predicate,
  bool fastCheck, Camera* camera)
{
  PickingInfo* pickingInfo = nullptr;

  if (!camera) {
    if (!activeCamera) {
      return nullptr;
    }
    camera = activeCamera;
  }

  if (spriteManagers.size() > 0) {
    for (auto& spriteManager : spriteManagers) {
      if (!spriteManager->isPickable) {
        continue;
      }

      auto result
        = spriteManager->intersects(ray, camera, predicate, fastCheck);
      if (!result || !result->hit) {
        continue;
      }

      if (!fastCheck && (pickingInfo != nullptr)
          && (result->distance >= pickingInfo->distance)) {
        continue;
      }

      pickingInfo = result;

      if (fastCheck) {
        break;
      }
    }
  }

  return pickingInfo;
}

PickingInfo*
Scene::pick(int x, int y,
            const ::std::function<bool(AbstractMesh* mesh)>& predicate,
            bool fastCheck, Camera* camera)
{
  return _internalPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      createPickingRayToRef(x, y, &world, *_tempPickingRay, camera);
      return *_tempPickingRay;
    },
    predicate, fastCheck);
}

PickingInfo*
Scene::pickSprite(int x, int y,
                  const ::std::function<bool(Sprite* sprite)>& predicate,
                  bool fastCheck, Camera* camera)
{
  createPickingRayInCameraSpaceToRef(x, y, *_tempPickingRay, camera);

  return _internalPickSprites(*_tempPickingRay, predicate, fastCheck, camera);
}

PickingInfo*
Scene::pickWithRay(const Ray& ray,
                   const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                   bool fastCheck)
{
  return _internalPick(
    [this, &ray](Matrix& world) -> Ray {
      if (!_pickWithRayInverseMatrix) {
        _pickWithRayInverseMatrix
          = ::std::make_unique<Matrix>(Matrix::Identity());
      }
      world.invertToRef(*_pickWithRayInverseMatrix);

      if (!_cachedRayForTransform) {
        _cachedRayForTransform = ::std::make_unique<Ray>(Ray::Zero());
      }

      Ray::TransformToRef(ray, *_pickWithRayInverseMatrix,
                          *_cachedRayForTransform);
      return *_cachedRayForTransform;
    },
    predicate, fastCheck);
}

vector_t<PickingInfo*>
Scene::multiPick(int x, int y,
                 const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                 Camera* camera)
{
  return _internalMultiPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      return createPickingRay(x, y, &world, camera);
    },
    predicate);
}

vector_t<PickingInfo*> Scene::multiPickWithRay(
  const Ray& ray, const ::std::function<bool(AbstractMesh* mesh)>& predicate)
{
  return _internalMultiPick(
    [this, &ray](Matrix& world) -> Ray {
      if (!_pickWithRayInverseMatrix) {
        _pickWithRayInverseMatrix
          = ::std::make_unique<Matrix>(Matrix::Identity());
      }
      world.invertToRef(*_pickWithRayInverseMatrix);

      if (!_cachedRayForTransform) {
        _cachedRayForTransform = ::std::make_unique<Ray>(Ray::Zero());
      }

      Ray::TransformToRef(ray, *_pickWithRayInverseMatrix,
                          *_cachedRayForTransform);
      return *_cachedRayForTransform;
    },
    predicate);
}

void Scene::setPointerOverMesh(AbstractMesh* mesh)
{
  if (_pointerOverMesh == mesh) {
    return;
  }

  if (_pointerOverMesh && _pointerOverMesh->actionManager) {
    _pointerOverMesh->actionManager->processTrigger(
      ActionManager::OnPointerOutTrigger,
      ActionEvent::CreateNew(_pointerOverMesh));
  }

  _pointerOverMesh = mesh;
  if (_pointerOverMesh && _pointerOverMesh->actionManager) {
    _pointerOverMesh->actionManager->processTrigger(
      ActionManager::OnPointerOverTrigger,
      ActionEvent::CreateNew(_pointerOverMesh));
  }
}

AbstractMesh* Scene::getPointerOverMesh()
{
  return _pointerOverMesh;
}

void Scene::setPointerOverSprite(Sprite* sprite)
{
  if (_pointerOverSprite == sprite) {
    return;
  }

  if (_pointerOverSprite && _pointerOverSprite->actionManager) {
    Event evt;
    _pointerOverSprite->actionManager->processTrigger(
      ActionManager::OnPointerOutTrigger,
      ActionEvent::CreateNewFromSprite(_pointerOverSprite, this, evt));
  }

  _pointerOverSprite = sprite;
  if (_pointerOverSprite && _pointerOverSprite->actionManager) {
    Event evt;
    _pointerOverSprite->actionManager->processTrigger(
      ActionManager::OnPointerOverTrigger,
      ActionEvent::CreateNewFromSprite(_pointerOverSprite, this, evt));
  }
}

Sprite* Scene::getPointerOverSprite() const
{
  return _pointerOverSprite;
}

/** Physics **/
PhysicsEngine* Scene::getPhysicsEngine()
{
  return _physicsEngine.get();
}

bool Scene::enablePhysics(const Vector3& iGravity, IPhysicsEnginePlugin* plugin)
{
  if (_physicsEngine) {
    return true;
  }

  _physicsEngine     = ::std::make_unique<PhysicsEngine>(iGravity, plugin);
  bool isInitialized = _physicsEngine->isInitialized();
  if (!isInitialized) {
    _physicsEngine = nullptr;
  }

  return isInitialized;
}

void Scene::disablePhysicsEngine()
{
  if (!_physicsEngine) {
    return;
  }

  _physicsEngine->dispose();
  _physicsEngine = nullptr;
}

bool Scene::isPhysicsEnabled()
{
  return _physicsEngine != nullptr;
}

void Scene::_rebuildGeometries()
{
  for (auto& geometry : _geometries) {
    geometry->_rebuild();
  }

  for (auto& mesh : meshes) {
    mesh->_rebuild();
  }

  if (postProcessManager) {
    postProcessManager->_rebuild();
  }

  for (auto& layer : layers) {
    layer->_rebuild();
  }

  for (auto& highlightLayer : highlightLayers) {
    highlightLayer->_rebuild();
  }

  if (_boundingBoxRenderer) {
    _boundingBoxRenderer->_rebuild();
  }

  for (auto& system : particleSystems) {
    system->rebuild();
  }

  if (_postProcessRenderPipelineManager) {
    _postProcessRenderPipelineManager->_rebuild();
  }
}
void Scene::_rebuildTextures()
{
  for (auto& texture : textures) {
    texture->_rebuild();
  }

  markAllMaterialsAsDirty(Material::TextureDirtyFlag);
}

void Scene::createDefaultCameraOrLight(bool createArcRotateCamera, bool replace,
                                       bool attachCameraControls)
{
  // Dispose existing camera or light in replace mode.
  if (replace) {
    if (activeCamera) {
      activeCamera->dispose();
      activeCamera = nullptr;
    }

    if (!lights.empty()) {
      for (auto& light : lights) {
        light->dispose();
      }
    }
  }

  // Light
  if (lights.empty()) {
    HemisphericLight::New("default light", Vector3::Up(), this);
  }

  // Camera
  if (!activeCamera) {
    auto worldExtends = getWorldExtends();
    auto worldSize    = worldExtends.max.subtract(worldExtends.min);
    auto worldCenter  = worldExtends.min.add(worldSize.scale(0.5f));

    TargetCamera* camera = nullptr;
    auto radius          = worldSize.length() * 1.5f;
    if (createArcRotateCamera) {
      auto arcRotateCamera = ArcRotateCamera::New(
        "default camera", -Math::PI_2, Math::PI_2, radius, worldCenter, this);
      arcRotateCamera->lowerRadiusLimit = radius * 0.01f;
      arcRotateCamera->wheelPrecision   = 100.f / radius;
      camera                            = arcRotateCamera;
    }
    else {
      auto freeCamera = FreeCamera::New(
        "default camera", Vector3(worldCenter.x, worldCenter.y, -radius), this);
      freeCamera->setTarget(worldCenter);
      camera = freeCamera;
    }
    camera->minZ  = radius * 0.01f;
    camera->maxZ  = radius * 100.f;
    camera->speed = radius * 0.2f;
    activeCamera  = camera;

    auto canvas = getEngine()->getRenderingCanvas();
    if (attachCameraControls && canvas) {
      camera->attachControl(canvas);
    }
  }
}

Mesh* Scene::createDefaultSkybox(BaseTexture* iEnvironmentTexture, bool pbr,
                                 float scale, float blur)
{
  if (environmentTexture()) {
    setEnvironmentTexture(iEnvironmentTexture);
  }

  if (!environmentTexture()) {
    BABYLON_LOG_WARN(
      "Scene", "Can not create default skybox without environment texture.");
    return nullptr;
  }

  // Skybox
  auto hdrSkybox = Mesh::CreateBox("hdrSkyBox", scale, this);
  if (pbr) {
    auto hdrSkyboxMaterial
      = static_cast<PBRMaterial*>(PBRMaterial::New("skyBox", this));
    hdrSkyboxMaterial->setBackFaceCulling(false);
    hdrSkyboxMaterial->reflectionTexture = environmentTexture();
    if (hdrSkyboxMaterial->reflectionTexture) {
      hdrSkyboxMaterial->reflectionTexture->setCoordinatesMode(
        TextureConstants::SKYBOX_MODE);
    }
    hdrSkyboxMaterial->microSurface     = 1.f - blur;
    hdrSkyboxMaterial->disableLighting  = true;
    hdrSkyboxMaterial->twoSidedLighting = true;
    hdrSkybox->infiniteDistance         = true;
    hdrSkybox->setMaterial(hdrSkyboxMaterial);
  }
  else {
    auto skyboxMaterial = StandardMaterial::New("skyBox", this);
    skyboxMaterial->setBackFaceCulling(false);
    skyboxMaterial->setReflectionTexture(environmentTexture());
    if (skyboxMaterial->reflectionTexture()) {
      skyboxMaterial->reflectionTexture()->setCoordinatesMode(
        TextureConstants::SKYBOX_MODE);
    }
    skyboxMaterial->setDisableLighting(true);
    hdrSkybox->infiniteDistance = true;
    hdrSkybox->setMaterial(skyboxMaterial);
  }

  return hdrSkybox;
}

unique_ptr_t<EnvironmentHelper>
Scene::createDefaultEnvironment(const IEnvironmentHelperOptions& options)
{
  return ::std::make_unique<EnvironmentHelper>(options, this);
}

// Tags

vector_t<string_t> Scene::_getByTags()
{
  return vector_t<string_t>();
}

vector_t<Mesh*> Scene::getMeshesByTags()
{
  return vector_t<Mesh*>();
}

vector_t<Camera*> Scene::getCamerasByTags()
{
  return vector_t<Camera*>();
}

vector_t<Light*> Scene::getLightsByTags()
{
  return vector_t<Light*>();
}

vector_t<Material*> Scene::getMaterialByTags()
{
  return vector_t<Material*>();
}

void Scene::setRenderingOrder(
  unsigned int renderingGroupId,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
{
  _renderingManager->setRenderingOrder(renderingGroupId, opaqueSortCompareFn,
                                       alphaTestSortCompareFn,
                                       transparentSortCompareFn);
}

void Scene::setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                              bool autoClearDepthStencil,
                                              bool depth, bool stencil)
{
  _renderingManager->setRenderingAutoClearDepthStencil(
    renderingGroupId, autoClearDepthStencil, depth, stencil);
}

void Scene::markAllMaterialsAsDirty(
  unsigned int flag, const ::std::function<bool(Material* mat)>& predicate)
{
  for (auto& material : materials) {
    if (predicate && !predicate(material.get())) {
      continue;
    }
    material->markAsDirty(flag);
  }
}

} // end of namespace BABYLON
