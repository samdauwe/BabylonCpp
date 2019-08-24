#include <babylon/engines/scene.h>

#include <babylon/actions/abstract_action_manager.h>
#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/animations/animatable.h>
#include <babylon/animations/animation_group.h>
#include <babylon/animations/runtime_animation.h>
#include <babylon/audio/audio_scene_component.h>
#include <babylon/audio/sound.h>
#include <babylon/audio/sound_track.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/target_camera.h>
#include <babylon/collisions/collision_coordinator.h>
#include <babylon/collisions/icollision_coordinator.h>
#include <babylon/core/logging.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/octrees/octree_scene_component.h>
#include <babylon/culling/ray.h>
#include <babylon/debug/debug_layer.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/iscene_serializable_component.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info_pre.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/gamepads/gamepad_manager.h>
#include <babylon/gamepads/gamepad_system_scene_component.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/inputs/click_info.h>
#include <babylon/inputs/input_manager.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/glow_layer.h>
#include <babylon/layer/highlight_layer.h>
#include <babylon/layer/layer.h>
#include <babylon/lensflares/lens_flare_system.h>
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
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/mesh_simplification_scene_component.h>
#include <babylon/meshes/simplification/simplification_queue.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/physics/physics_engine_component.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/postprocesses/post_process_manager.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline_manager_scene_component.h>
#include <babylon/probes/reflection_probe.h>
#include <babylon/rendering/bounding_box_renderer.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/irendering_manager_auto_clear_setup.h>
#include <babylon/rendering/outline_renderer.h>
#include <babylon/rendering/rendering_manager.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

size_t Scene::_uniqueIdCounter = 0;

microseconds_t Scene::MinDeltaTime = std::chrono::milliseconds(1);
microseconds_t Scene::MaxDeltaTime = std::chrono::milliseconds(1000);

unsigned int Scene::DragMovementThreshold()
{
  return InputManager::DragMovementThreshold;
}

void Scene::setDragMovementThreshold(unsigned int value)
{
  InputManager::DragMovementThreshold = value;
}

milliseconds_t Scene::LongPressDelay()
{
  return InputManager::LongPressDelay;
}

void Scene::setLongPressDelay(milliseconds_t value)
{
  InputManager::LongPressDelay = value;
}

milliseconds_t Scene::DoubleClickDelay()
{
  return InputManager::DoubleClickDelay;
}

void Scene::setDoubleClickDelay(milliseconds_t value)
{
  InputManager::DoubleClickDelay = value;
}

bool Scene::ExclusiveDoubleClickMode()
{
  return InputManager::ExclusiveDoubleClickMode;
}

void Scene::setExclusiveDoubleClickMode(bool value)
{
  InputManager::ExclusiveDoubleClickMode = value;
}

Scene::Scene(Engine* engine, const std::optional<SceneOptions>& options)
    : AbstractScene{}
    , _inputManager{std::make_unique<InputManager>(this)}
    , cameraToUseForPointers{nullptr}
    , _isScene{true}
    , autoClear{true}
    , autoClearDepthAndStencil{true}
    , clearColor{Color4(0.2f, 0.2f, 0.3f, 1.f)}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , environmentBRDFTexture{nullptr}
    , imageProcessingConfiguration{this,
                                   &Scene::get_imageProcessingConfiguration}
    , onDispose{this, &Scene::set_onDispose}
    , beforeRender{this, &Scene::set_beforeRender}
    , afterRender{this, &Scene::set_afterRender}
    , beforeCameraRender{this, &Scene::set_beforeCameraRender}
    , afterCameraRender{this, &Scene::set_afterCameraRender}
    , pointerDownPredicate{nullptr}
    , pointerUpPredicate{nullptr}
    , pointerMovePredicate{nullptr}
    , forceWireframe{this, &Scene::get_forceWireframe,
                     &Scene::set_forceWireframe}
    , forcePointsCloud{this, &Scene::get_forcePointsCloud,
                       &Scene::set_forcePointsCloud}
    , clipPlane{std::nullopt}
    , clipPlane2{std::nullopt}
    , clipPlane3{std::nullopt}
    , clipPlane4{std::nullopt}
    , forceShowBoundingBoxes{this, &Scene::get_forceShowBoundingBoxes,
                             &Scene::set_forceShowBoundingBoxes}
    , animationsEnabled{true}
    , useConstantAnimationDeltaTime{false}
    , constantlyUpdateMeshUnderPointer{false}
    , hoverCursor{"pointer"}
    , defaultCursor{""}
    , preventDefaultOnPointerDown{true}
    , preventDefaultOnPointerUp{true}
    , gamepadManager{this, &Scene::get_gamepadManager}
    , unTranslatedPointer{this, &Scene::get_unTranslatedPointer}
    , _mirroredCameraPosition{nullptr}
    , useRightHandedSystem{this, &Scene::get_useRightHandedSystem,
                           &Scene::set_useRightHandedSystem}
    , fogEnabled{this, &Scene::get_fogEnabled, &Scene::set_fogEnabled}
    , fogMode{this, &Scene::get_fogMode, &Scene::set_fogMode}
    , fogColor{Color3(0.2f, 0.2f, 0.3f)}
    , fogDensity{0.1f}
    , fogStart{0.f}
    , fogEnd{1000.f}
    , shadowsEnabled{this, &Scene::get_shadowsEnabled,
                     &Scene::set_shadowsEnabled}
    , lightsEnabled{this, &Scene::get_lightsEnabled, &Scene::set_lightsEnabled}
    , _activeCamera{nullptr}
    , activeCamera{this, &Scene::get_activeCamera, &Scene::set_activeCamera}
    , defaultMaterial{this, &Scene::get_defaultMaterial,
                      &Scene::set_defaultMaterial}
    , texturesEnabled{this, &Scene::get_texturesEnabled,
                      &Scene::set_texturesEnabled}
    , particlesEnabled{true}
    , spritesEnabled{true}
    , _pointerOverSprite{nullptr}
    , _pickedDownSprite{nullptr}
    , _tempSpritePickingRay{std::nullopt}
    , skeletonsEnabled{this, &Scene::get_skeletonsEnabled,
                       &Scene::set_skeletonsEnabled}
    , lensFlaresEnabled{true}
    , collisionsEnabled{true}
    , collisionCoordinator{this, &Scene::get_collisionCoordinator}
    , gravity{Vector3(0.f, -9.807f, 0.f)}
    , postProcessesEnabled{true}
    , postProcessManager{nullptr}
    , postProcessRenderPipelineManager{this,
                                       &Scene::
                                         get_postProcessRenderPipelineManager}
    , renderTargetsEnabled{true}
    , dumpNextRenderTargets{false}
    , useDelayedTextureLoading{false}
    , probesEnabled{true}
    , actionManager{nullptr}
    , proceduralTexturesEnabled{true}
    , mainSoundTrack{this, &Scene::get_mainSoundTrack}
    , simplificationQueue{this, &Scene::get_simplificationQueue,
                          &Scene::set_simplificationQueue}
    , _animationTimeLast{std::nullopt}
    , _animationTime(0)
    , animationTimeScale{1}
    , _cachedMaterial{nullptr}
    , _cachedEffect{nullptr}
    , _cachedVisibility{0.f}
    , dispatchAllSubMeshesOfActiveMeshes{false}
    , _frustumPlanes{}
    , _forcedViewPosition{nullptr}
    , frustumPlanes{this, &Scene::get_frustumPlanes}
    , requireLightSorting{false}
    , useMaterialMeshMap{false}
    , useClonedMeshhMap{false}
    , depthRenderer{this, &Scene::get_depthRenderer}
    , geometryBufferRenderer{this, &Scene::get_geometryBufferRenderer,
                             &Scene::set_geometryBufferRenderer}
    , debugLayer{this, &Scene::get_debugLayer}
    , selectionOctree{this, &Scene::get_selectionOctree}
    , meshUnderPointer{this, &Scene::get_meshUnderPointer}
    , pointerX{this, &Scene::get_pointerX, &Scene::set_pointerX}
    , pointerY{this, &Scene::get_pointerY, &Scene::set_pointerY}
    , totalVerticesPerfCounter{this, &Scene::get_totalVerticesPerfCounter}
    , totalActiveIndicesPerfCounter{this,
                                    &Scene::get_totalActiveIndicesPerfCounter}
    , activeParticlesPerfCounter{this, &Scene::get_activeParticlesPerfCounter}
    , activeBonesPerfCounter{this, &Scene::get_activeBonesPerfCounter}
    , isLoading{this, &Scene::get_isLoading}
    , uid{this, &Scene::get_uid}
    , audioEnabled{this, &Scene::get_audioEnabled, &Scene::set_audioEnabled}
    , headphone{this, &Scene::get_headphone, &Scene::set_headphone}
    , isDisposed{this, &Scene::get_isDisposed}
    , _allowPostProcessClearColor{true}
    , blockMaterialDirtyMechanism{this, &Scene::get_blockMaterialDirtyMechanism,
                                  &Scene::set_blockMaterialDirtyMechanism}
    , getActiveMeshCandidates{nullptr}
    , getActiveSubMeshCandidates{nullptr}
    , getIntersectingSubMeshCandidates{nullptr}
    , getCollidingSubMeshCandidates{nullptr}
    , _physicsEngine{nullptr}
    , blockfreeActiveMeshesAndRenderingGroups{this,
                                              &Scene::
                                                get_blockfreeActiveMeshesAndRenderingGroups,
                                              &Scene::
                                                set_blockfreeActiveMeshesAndRenderingGroups}
    , _environmentTexture{nullptr}
    , _animationPropertiesOverride{nullptr}
    , _spritePredicate{nullptr}
    , _onDisposeObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _onBeforeCameraRenderObserver{nullptr}
    , _onAfterCameraRenderObserver{nullptr}
    , _timeAccumulator{0}
    , _currentStepId{0}
    , _currentInternalStep{0}
    , _onKeyDown{nullptr}
    , _onKeyUp{nullptr}
    , _onCanvasFocusObserver{nullptr}
    , _onCanvasBlurObserver{nullptr}
    , _useRightHandedSystem{false}
    , _forceWireframe{false}
    , _forcePointsCloud{false}
    , _fogEnabled{true}
    , _fogMode{Scene::FOGMODE_NONE}
    , _shadowsEnabled{true}
    , _lightsEnabled{true}
    , _defaultMaterial{nullptr}
    , _texturesEnabled{true}
    , _skeletonsEnabled{true}
    , _postProcessRenderPipelineManager{nullptr}
    , _collisionCoordinator{nullptr}
    , _hasAudioEngine{false}
    , _mainSoundTrack{nullptr}
    , _engine{engine ? engine : Engine::LastCreatedEngine()}
    , _animationRatio{1.f}
    , _renderId{0}
    , _frameId{0}
    , _executeWhenReadyTimeoutId{-1}
    , _intermediateRendering{false}
    , _viewUpdateFlag{-1}
    , _projectionUpdateFlag{-1}
    , _isDisposed{false}
    , _activeMeshCandidateProvider{nullptr}
    , _activeMeshesFrozen{false}
    , _renderingManager{nullptr}
    , _transformMatrix{Matrix::Zero()}
    , _sceneUbo{nullptr}
    , _pickWithRayInverseMatrix{nullptr}
    , _simplificationQueue{nullptr}
    , _boundingBoxRenderer{nullptr}
    , _forceShowBoundingBoxes{false}
    , _outlineRenderer{nullptr}
    , _selectionOctree{nullptr}
    , _frustumPlanesSet{false}
    , _debugLayer{nullptr}
    , _geometryBufferRenderer{nullptr}
    , _uid{Tools::RandomId()}
    , _blockMaterialDirtyMechanism{false}
    , _tempPickingRay{std::make_unique<Ray>(Ray::Zero())}
    , _cachedRayForTransform{nullptr}
    , _audioEnabled{std::nullopt}
    , _headphone{std::nullopt}
    , _transformMatrixR{Matrix::Zero()}
    , _multiviewSceneUbo{nullptr}
    , _preventFreeActiveMeshesAndRenderingGroups{false}
{
  engine->scenes.emplace_back(this);

  _renderingManager = std::make_unique<RenderingManager>(this);

  // if (PostProcessManager) {
  {
    postProcessManager = std::make_unique<PostProcessManager>(this);
  }

  attachControl();

  // Uniform Buffer
  _createUbo();

  // Default Image processing definition.
  // if (ImageProcessingConfiguration)
  {
    _imageProcessingConfiguration
      = std::make_shared<ImageProcessingConfiguration>();
  }

  getDeterministicFrameTime = []() -> float {
    return 1000.f / 60.f; // frame time in ms
  };

  setDefaultCandidateProviders();

  if (options.has_value() && *options->useGeometryUniqueIdsMap == true) {
    geometriesByUniqueId = {};
  }

  useMaterialMeshMap = options.has_value() ?
                         options->useGeometryUniqueIdsMap.value_or(false) :
                         false;
  useClonedMeshhMap = options.has_value() ?
                        options->useGeometryUniqueIdsMap.value_or(false) :
                        false;

  _engine->onNewSceneAddedObservable.notifyObservers(this);
}

Scene::~Scene()
{
}

Type Scene::type() const
{
  return Type::SCENE;
}

void Scene::_registerTransientComponents()
{
  // Register components that have been associated lately to the scene.
  if (!_transientComponents.empty()) {
    for (const auto& component : _transientComponents) {
      component->_register();
    }
    _transientComponents.clear();
  }
}

void Scene::_addComponent(const ISceneComponentPtr& component)
{
  _components.emplace_back(component);
  _transientComponents.emplace_back(component);

  auto serializableComponent
    = std::static_pointer_cast<ISceneSerializableComponent>(component);
  if (serializableComponent) {
    _serializableComponents.emplace_back(serializableComponent);
  }
}

ISceneComponentPtr Scene::_getComponent(const std::string& name) const
{
  auto it = std::find_if(_components.begin(), _components.end(),
                         [&name](const ISceneComponentPtr& component) {
                           return component->name == name;
                         });
  return (it == _components.end()) ? nullptr : *it;
}

// Events
void Scene::set_onDispose(
  const std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Scene::set_beforeRender(
  const std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  if (callback) {
    _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
  }
}

void Scene::set_afterRender(
  const std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onAfterRenderObserver) {
    onAfterRenderObservable.remove(_onAfterRenderObserver);
  }
  if (callback) {
    _onAfterRenderObserver = onAfterRenderObservable.add(callback);
  }
}

void Scene::set_beforeCameraRender(
  const std::function<void(Camera* camera, EventState& es)>& callback)
{
  if (_onBeforeCameraRenderObserver) {
    onBeforeCameraRenderObservable.remove(_onBeforeCameraRenderObserver);
  }

  _onBeforeCameraRenderObserver = onBeforeCameraRenderObservable.add(callback);
}

void Scene::set_afterCameraRender(
  const std::function<void(Camera* camera, EventState& es)>& callback)
{
  if (_onAfterCameraRenderObserver) {
    onAfterCameraRenderObservable.remove(_onAfterCameraRenderObserver);
  }
  _onAfterCameraRenderObserver = onAfterCameraRenderObservable.add(callback);
}

// Gamepads
std::unique_ptr<GamepadManager>& Scene::get_gamepadManager()
{
  if (!_gamepadManager) {
    _gamepadManager = std::make_unique<GamepadManager>(this);
    auto component  = std::static_pointer_cast<GamepadSystemSceneComponent>(
      _getComponent(SceneComponentConstants::NAME_GAMEPAD));
    if (!component) {
      auto newComponent = GamepadSystemSceneComponent::New(this);
      _addComponent(newComponent);
    }
  }

  return _gamepadManager;
}

// Pointers
Vector2& Scene::get_unTranslatedPointer()
{
  return _inputManager->unTranslatedPointer();
}

// Properties

BaseTexturePtr& Scene::get_environmentTexture()
{
  return _environmentTexture;
}

void Scene::set_environmentTexture(const BaseTexturePtr& value)
{
  if (_environmentTexture == value) {
    return;
  }

  _environmentTexture = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool Scene::get_useRightHandedSystem() const
{
  return _useRightHandedSystem;
}

void Scene::set_useRightHandedSystem(bool value)
{
  if (_useRightHandedSystem == value) {
    return;
  }
  _useRightHandedSystem = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_MiscDirtyFlag);
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

ImageProcessingConfigurationPtr& Scene::get_imageProcessingConfiguration()
{
  return _imageProcessingConfiguration;
}

void Scene::set_forceWireframe(bool value)
{
  if (_forceWireframe == value) {
    return;
  }
  _forceWireframe = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_MiscDirtyFlag);
}

bool Scene::get_forceWireframe() const
{
  return _forceWireframe;
}

bool Scene::get_forcePointsCloud() const
{
  return _forcePointsCloud;
}

void Scene::set_forcePointsCloud(bool value)
{
  if (_forcePointsCloud == value) {
    return;
  }
  _forcePointsCloud = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_MiscDirtyFlag);
}

bool Scene::get_forceShowBoundingBoxes() const
{
  return _forceShowBoundingBoxes;
}

void Scene::set_forceShowBoundingBoxes(bool value)
{
  _forceShowBoundingBoxes = value;
  // Lazyly creates a BB renderer if needed.
  if (value) {
    getBoundingBoxRenderer();
  }
}

AnimationPropertiesOverride*& Scene::get_animationPropertiesOverride()
{
  return _animationPropertiesOverride;
}

void Scene::set_animationPropertiesOverride(
  AnimationPropertiesOverride* const& value)
{
  _animationPropertiesOverride = value;
}

void Scene::set_fogEnabled(bool value)
{
  if (_fogEnabled == value) {
    return;
  }
  _fogEnabled = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_MiscDirtyFlag);
}

bool Scene::get_fogEnabled() const
{
  return _fogEnabled;
}

void Scene::set_fogMode(unsigned int value)
{
  if (_fogMode == value) {
    return;
  }
  _fogMode = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_MiscDirtyFlag);
}

unsigned int Scene::get_fogMode() const
{
  return _fogMode;
}

void Scene::set_shadowsEnabled(bool value)
{
  if (_shadowsEnabled == value) {
    return;
  }
  _shadowsEnabled = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_LightDirtyFlag);
}

bool Scene::get_shadowsEnabled() const
{
  return _shadowsEnabled;
}

void Scene::set_lightsEnabled(bool value)
{
  if (_lightsEnabled == value) {
    return;
  }
  _lightsEnabled = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_LightDirtyFlag);
}

bool Scene::get_lightsEnabled() const
{
  return _lightsEnabled;
}

CameraPtr& Scene::get_activeCamera()
{
  return _activeCamera;
}

void Scene::set_activeCamera(const CameraPtr& value)
{
  if (value == _activeCamera) {
    return;
  }

  _activeCamera = value;
  onActiveCameraChanged.notifyObservers(this);
}

void Scene::set_texturesEnabled(bool value)
{
  if (_texturesEnabled == value) {
    return;
  }
  _texturesEnabled = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

bool Scene::get_texturesEnabled() const
{
  return _texturesEnabled;
}

void Scene::set_skeletonsEnabled(bool value)
{
  if (_skeletonsEnabled == value) {
    return;
  }
  _skeletonsEnabled = value;
  markAllMaterialsAsDirty(Constants::MATERIAL_AttributesDirtyFlag);
}

bool Scene::get_skeletonsEnabled() const
{
  return _skeletonsEnabled;
}

std::unique_ptr<ICollisionCoordinator>& Scene::get_collisionCoordinator()
{
  if (!_collisionCoordinator) {
    _collisionCoordinator = std::make_unique<DefaultCollisionCoordinator>();
    _collisionCoordinator->init(this);
  }

  return _collisionCoordinator;
}

std::unique_ptr<PostProcessRenderPipelineManager>&
Scene::get_postProcessRenderPipelineManager()
{
  if (!_postProcessRenderPipelineManager) {
    // Register the G Buffer component to the scene.
    auto component = std::static_pointer_cast<
      PostProcessRenderPipelineManagerSceneComponent>(_getComponent(
      SceneComponentConstants::NAME_POSTPROCESSRENDERPIPELINEMANAGER));
    if (!component) {
      component = PostProcessRenderPipelineManagerSceneComponent::New(this);
      _addComponent(component);
    }
    _postProcessRenderPipelineManager
      = std::make_unique<PostProcessRenderPipelineManager>();
  }

  return _postProcessRenderPipelineManager;
}

SoundTrackPtr& Scene::get_mainSoundTrack()
{
  auto compo = _getComponent(SceneComponentConstants::NAME_AUDIO);
  if (!compo) {
    compo = AudioSceneComponent::New(this);
    _addComponent(compo);
  }

  if (!_mainSoundTrack) {
    _mainSoundTrack = SoundTrack::New(this, ISoundTrackOptions{
                                              std::nullopt, // volume
                                              true,         // mainTrack
                                            });
  }

  return _mainSoundTrack;
}

SimplificationQueuePtr& Scene::get_simplificationQueue()
{
  if (!_simplificationQueue) {
    _simplificationQueue = std::make_shared<SimplificationQueue>();
    auto component = std::static_pointer_cast<SimplicationQueueSceneComponent>(
      _getComponent(SceneComponentConstants::NAME_SIMPLIFICATIONQUEUE));
    if (!component) {
      component = SimplicationQueueSceneComponent::New(this);
      _addComponent(component);
    }
  }
  return _simplificationQueue;
}

void Scene::set_simplificationQueue(const SimplificationQueuePtr& value)
{
  _simplificationQueue = value;
}

std::unordered_map<std::string, DepthRendererPtr>& Scene::get_depthRenderer()
{
  return _depthRenderer;
}

GeometryBufferRendererPtr& Scene::get_geometryBufferRenderer()
{
  return _geometryBufferRenderer;
}

void Scene::set_geometryBufferRenderer(const GeometryBufferRendererPtr& value)
{
  if (value && value->isSupported()) {
    _geometryBufferRenderer = value;
  }
}

void Scene::setMirroredCameraPosition(const Vector3& newPosition)
{
  _mirroredCameraPosition = std::make_unique<Vector3>(newPosition);
}

MaterialPtr& Scene::get_defaultMaterial()
{
  if (!_defaultMaterial) {
    _defaultMaterial = StandardMaterial::New("default material", this);
  }

  return _defaultMaterial;
}

void Scene::set_defaultMaterial(const MaterialPtr& value)
{
  _defaultMaterial = value;
}

std::array<Plane, 6>& Scene::get_frustumPlanes()
{
  return _frustumPlanes;
}

std::unique_ptr<DebugLayer>& Scene::get_debugLayer()
{
  if (!_debugLayer) {
    _debugLayer = std::make_unique<DebugLayer>(this);
  }
  return _debugLayer;
}

std::vector<AbstractMeshPtr> Scene::getMeshes() const
{
  std::vector<AbstractMeshPtr> _meshes;
  _meshes.reserve(meshes.size());

  std::for_each(
    meshes.begin(), meshes.end(),
    [&_meshes](const AbstractMeshPtr& mesh) { _meshes.emplace_back(mesh); });

  return _meshes;
}

Octree<AbstractMesh*>*& Scene::get_selectionOctree()
{
  return _selectionOctree;
}

AbstractMeshPtr& Scene::get_meshUnderPointer()
{
  return _inputManager->meshUnderPointer;
}

int Scene::get_pointerX() const
{
  return _inputManager->pointerX();
}

void Scene::set_pointerX(int value)
{
  _inputManager->pointerX = value;
}

int Scene::get_pointerY() const
{
  return _inputManager->pointerY();
}

void Scene::set_pointerY(int value)
{
  _inputManager->pointerY = value;
}

const std::string Scene::getClassName() const
{
  return "Scene";
}

std::vector<AbstractMesh*> Scene::_getDefaultMeshCandidates()
{
  _defaultMeshCandidates = stl_util::to_raw_ptr_vector(meshes);
  return _defaultMeshCandidates;
}

std::vector<SubMesh*> Scene::_getDefaultSubMeshCandidates(AbstractMesh* mesh)
{
  _defaultSubMeshCandidates = stl_util::to_raw_ptr_vector(mesh->subMeshes);
  return _defaultSubMeshCandidates;
}

void Scene::setDefaultCandidateProviders()
{
  getActiveMeshCandidates = [this]() { return _getDefaultMeshCandidates(); };

  getActiveSubMeshCandidates
    = [this](AbstractMesh* mesh) { return _getDefaultSubMeshCandidates(mesh); };
  getIntersectingSubMeshCandidates
    = [this](AbstractMesh* mesh, const Ray& /*localRay*/) {
        return _getDefaultSubMeshCandidates(mesh);
      };
  getCollidingSubMeshCandidates
    = [this](AbstractMesh* mesh, const Collider& /*collider*/) {
        return _getDefaultSubMeshCandidates(mesh);
      };
}

Material* Scene::getCachedMaterial()
{
  return _cachedMaterial;
}

EffectPtr& Scene::getCachedEffect()
{
  return _cachedEffect;
}

std::optional<float> Scene::getCachedVisibility()
{
  return _cachedVisibility;
}

bool Scene::isCachedMaterialInvalid(Material* material, const EffectPtr& effect,
                                    float visibility)
{
  return _cachedEffect != effect || _cachedMaterial != material
         || !stl_util::almost_equal(*_cachedVisibility, visibility);
}

BoundingBoxRendererPtr& Scene::getBoundingBoxRenderer()
{
  if (!_boundingBoxRenderer) {
    _boundingBoxRenderer = BoundingBoxRenderer::New(this);
  }

  return _boundingBoxRenderer;
}

OutlineRendererPtr& Scene::getOutlineRenderer()
{
  if (!_outlineRenderer) {
    _outlineRenderer = OutlineRenderer::New(this);
  }
  return _outlineRenderer;
}

Engine* Scene::getEngine()
{
  return _engine;
}

size_t Scene::getTotalVertices() const
{
  return _totalVertices.current();
}

PerfCounter& Scene::get_totalVerticesPerfCounter()
{
  return _totalVertices;
}

size_t Scene::getActiveIndices() const
{
  return _activeIndices.current();
}

PerfCounter& Scene::get_totalActiveIndicesPerfCounter()
{
  return _activeIndices;
}

size_t Scene::getActiveParticles() const
{
  return _activeParticles.current();
}

PerfCounter& Scene::get_activeParticlesPerfCounter()
{
  return _activeParticles;
}

size_t Scene::getActiveBones() const
{
  return _activeBones.current();
}

PerfCounter& Scene::get_activeBonesPerfCounter()
{
  return _activeBones;
}

std::vector<AbstractMesh*>& Scene::getActiveMeshes()
{
  return _activeMeshes;
}

const std::vector<AbstractMesh*>& Scene::getActiveMeshes() const
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

int Scene::getFrameId() const
{
  return _frameId;
}

void Scene::incrementRenderId()
{
  ++_renderId;
}

void Scene::_createUbo()
{
  _sceneUbo = std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _sceneUbo->addUniform("viewProjection", 16);
  _sceneUbo->addUniform("view", 16);
}

Scene& Scene::simulatePointerMove(std::optional<PickingInfo>& pickResult)
{
  _inputManager->simulatePointerMove(pickResult);

  return *this;
}

Scene& Scene::simulatePointerDown(std::optional<PickingInfo>& pickResult)
{
  _inputManager->simulatePointerDown(pickResult);

  return *this;
}

Scene& Scene::simulatePointerUp(std::optional<PickingInfo>& pickResult,
                                bool doubleTap)
{
  _inputManager->simulatePointerUp(pickResult, doubleTap);

  return *this;
}

bool Scene::isPointerCaptured(int pointerId)
{
  return _inputManager->isPointerCaptured(pointerId);
}

void Scene::attachControl(bool attachUp, bool attachDown, bool attachMove)
{
  _inputManager->attachControl(attachUp, attachDown, attachMove);
}

void Scene::detachControl()
{
  _inputManager->detachControl();
}

bool Scene::isReady()
{
  if (_isDisposed) {
    return false;
  }

  auto engine = getEngine();

  // Effects
  if (!engine->areAllEffectsReady()) {
    return false;
  }

  // Pending data
  if (!_pendingData.empty()) {
    return false;
  }

  // Meshes
  for (const auto& mesh : meshes) {
    if (!mesh->isEnabled()) {
      continue;
    }

    if (mesh->subMeshes.empty()) {
      continue;
    }

    if (!mesh->isReady(true)) {
      return false;
    }

    // Effect layers
    auto _mesh = static_cast<Mesh*>(mesh.get());
    auto hardwareInstancedRendering
      = mesh->getClassName() == std::string("InstancedMesh")
        || mesh->getClassName() == std::string("InstancedLinesMesh")
        || (engine->getCaps().instancedArrays && _mesh->instances.size() > 0);

    // Is Ready For Mesh
    for (const auto& step : _isReadyForMeshStage) {
      if (!step.action(mesh.get(), hardwareInstancedRendering)) {
        return false;
      }
    }
  }

  // Geometries
  for (const auto& geometry : geometries) {
    if (geometry->delayLoadState == Constants::DELAYLOADSTATE_LOADING) {
      return false;
    }
  }

  // Post-processes
  if (!activeCameras.empty()) {
    for (const auto& camera : activeCameras) {
      if (!camera->isReady(true)) {
        return false;
      }
    }
  }
  else if (_activeCamera) {
    if (!_activeCamera->isReady(true)) {
      return false;
    }
  }

  // Particles
  for (const auto& particleSystem : particleSystems) {
    if (!particleSystem->isReady()) {
      return false;
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
  const std::function<void(Scene* scene, EventState& es)>& func)
{
  onBeforeRenderObservable.add(func);
}

void Scene::unregisterBeforeRender(
  const std::function<void(Scene* scene, EventState& es)>& func)
{
  onBeforeRenderObservable.removeCallback(func);
}

void Scene::registerAfterRender(
  const std::function<void(Scene* scene, EventState& es)>& func)
{
  onAfterRenderObservable.add(func);
}

void Scene::unregisterAfterRender(
  const std::function<void(Scene* scene, EventState& es)>& func)
{
  onAfterRenderObservable.removeCallback(func);
}

void Scene::_addPendingData(Mesh* /*mesh*/)
{
}

void Scene::_addPendingData(const InternalTexturePtr& /*texure*/)
{
}

void Scene::_removePendingData(const InternalTexturePtr& /*texture*/)
{
  auto wasLoading = isLoading();

  if (wasLoading && !isLoading()) {
    onDataLoadedObservable.notifyObservers(this);
  }
}

void Scene::getWaitingItemsCount()
{
}

bool Scene::get_isLoading() const
{
  return _pendingData.size() > 0;
}

void Scene::executeWhenReady(
  const std::function<void(Scene* scene, EventState& es)>& func)
{
  onReadyObservable.add(func);

  if (_executeWhenReadyTimeoutId != -1) {
    return;
  }
}

void Scene::_checkIsReady()
{
  _registerTransientComponents();

  if (isReady()) {
    onReadyObservable.notifyObservers(this);

    onReadyObservable.clear();
    _executeWhenReadyTimeoutId = -1;
    return;
  }
}

AnimatablePtr Scene::beginWeightedAnimation(
  const IAnimatablePtr& target, float from, float to, float weight, bool loop,
  float speedRatio, const std::function<void()>& onAnimationEnd,
  AnimatablePtr animatable,
  const std::function<bool(IAnimatable* target)>& targetMask,
  const std::function<void()>& onAnimationLoop)
{
  auto returnedAnimatable
    = beginAnimation(target, from, to, loop, speedRatio, onAnimationEnd,
                     animatable, false, targetMask, onAnimationLoop);
  returnedAnimatable->weight = weight;

  return returnedAnimatable;
}

AnimatablePtr Scene::beginAnimation(
  const IAnimatablePtr& target, float from, float to, bool loop,
  float speedRatio, const std::function<void()>& onAnimationEnd,
  AnimatablePtr animatable, bool stopCurrent,
  const std::function<bool(IAnimatable* target)>& targetMask,
  const std::function<void()>& onAnimationLoop)
{
  if (from > to && speedRatio > 0.f) {
    speedRatio *= -1.f;
  }

  if (stopCurrent) {
    stopAnimation(target, "", targetMask);
  }

  if (!animatable) {
    std::vector<AnimationPtr> animationList;
    animatable
      = Animatable::New(this, target, from, to, loop, speedRatio,
                        onAnimationEnd, animationList, onAnimationLoop);
  }

  auto shouldRunTargetAnimations = targetMask ? targetMask(target.get()) : true;

  // Local animations
  auto _animations = target->getAnimations();
  if (!_animations.empty() && shouldRunTargetAnimations) {
    animatable->appendAnimations(target, _animations);
  }

  // Children animations
  std::vector<IAnimatablePtr> animatables;
  if (auto skeleton = std::dynamic_pointer_cast<Skeleton>(target)) {
    animatables = skeleton->getAnimatables();
  }
  else if (auto mat = std::dynamic_pointer_cast<StandardMaterial>(target)) {
    animatables = mat->getAnimatables();
  }
  else if (auto mesh = std::dynamic_pointer_cast<Mesh>(target)) {
    animatables = mesh->getAnimatables();
  }

  if (!animatables.empty()) {
    for (const auto& childAnimatable : animatables) {
      beginAnimation(childAnimatable, from, to, loop, speedRatio,
                     onAnimationEnd, animatable, stopCurrent, targetMask,
                     onAnimationLoop);
    }
  }

  animatable->reset();

  return animatable;
}

std::vector<AnimatablePtr> Scene::beginHierarchyAnimation(
  const NodePtr& target, bool directDescendantsOnly, float from, float to,
  bool loop, float speedRatio, const std::function<void()>& onAnimationEnd,
  AnimatablePtr animatable, bool stopCurrent,
  const std::function<bool(IAnimatable* target)>& targetMask,
  const std::function<void()>& onAnimationLoop)
{
  auto children = target->getDescendants(directDescendantsOnly);

  std::vector<AnimatablePtr> result;
  result.emplace_back(beginAnimation(target, from, to, loop, speedRatio,
                                     onAnimationEnd, animatable, stopCurrent,
                                     targetMask, onAnimationLoop));
  for (const auto& child : children) {
    result.emplace_back(beginAnimation(child, from, to, loop, speedRatio,
                                       onAnimationEnd, animatable, stopCurrent,
                                       targetMask, onAnimationLoop));
  }

  return result;
}

AnimatablePtr
Scene::beginDirectAnimation(const IAnimatablePtr& target,
                            const std::vector<AnimationPtr>& _animations,
                            float from, float to, bool loop, float speedRatio,
                            const std::function<void()>& onAnimationEnd,
                            const std::function<void()>& onAnimationLoop)
{
  return Animatable::New(this, target, from, to, loop, speedRatio,
                         onAnimationEnd, _animations, onAnimationLoop);
}

std::vector<AnimatablePtr> Scene::beginDirectHierarchyAnimation(
  const NodePtr& target, bool directDescendantsOnly,
  const std::vector<AnimationPtr>& iAnimations, float from, float to, bool loop,
  float speedRatio, const std::function<void()>& onAnimationEnd,
  const std::function<void()>& onAnimationLoop)
{
  auto children = target->getDescendants(directDescendantsOnly);

  std::vector<AnimatablePtr> result;
  result.emplace_back(beginDirectAnimation(target, iAnimations, from, to, loop,
                                           speedRatio, onAnimationEnd,
                                           onAnimationLoop));
  for (const auto& child : children) {
    result.emplace_back(
      beginDirectAnimation(child, iAnimations, from, static_cast<float>(to),
                           loop, speedRatio, onAnimationEnd, onAnimationLoop));
  }

  return result;
}

AnimatablePtr Scene::getAnimatableByTarget(const IAnimatablePtr& target)
{
  auto it = std::find_if(_activeAnimatables.begin(), _activeAnimatables.end(),
                         [&target](const AnimatablePtr& animatable) {
                           return animatable->target == target;
                         });
  return (it == _activeAnimatables.end()) ? nullptr : *it;
}

std::vector<AnimatablePtr>
Scene::getAllAnimatablesByTarget(const IAnimatablePtr& target)
{
  return getAllAnimatablesByTarget(target.get());
}

std::vector<AnimatablePtr> Scene::getAllAnimatablesByTarget(IAnimatable* target)
{
  std::vector<AnimatablePtr> result;
  for (const auto& activeAnimatable : _activeAnimatables) {
    if (activeAnimatable->target.get() == target) {
      result.emplace_back(activeAnimatable);
    }
  }

  return result;
}

void Scene::resetLastAnimationTimeFrame()
{
  _animationTimeLast = Time::highresTimepointNow();
}

void Scene::stopAnimation(
  const IAnimatablePtr& target, const std::string& animationName,
  const std::function<bool(IAnimatable* target)>& targetMask)
{
  return stopAnimation(target.get(), animationName, targetMask);
}

void Scene::stopAnimation(
  IAnimatable* target, const std::string& animationName,
  const std::function<bool(IAnimatable* target)>& targetMask)
{
  auto animatables = getAllAnimatablesByTarget(target);

  for (const auto& animatable : animatables) {
    animatable->stop(animationName, targetMask);
  }
}

void Scene::stopAllAnimations()
{
  if (!_activeAnimatables.empty()) {
    for (const auto& activeAnimatable : _activeAnimatables) {
      activeAnimatable->stop();
    }
    _activeAnimatables.clear();
  }

  for (const auto& group : animationGroups) {
    group->stop();
  }
}

void Scene::_animate()
{
  if (!animationsEnabled) {
    return;
  }

  const auto& animatables = _activeAnimatables;
  if (animatables.empty()) {
    return;
  }

  // Getting time
  auto now = Time::highresTimepointNow();
  if (!_animationTimeLast.has_value()) {
    if (!_pendingData.empty()) {
      return;
    }
    _animationTimeLast = now;
  }
  const auto deltaTime
    = useConstantAnimationDeltaTime ?
        16.f :
        Time::fpTimeSince<size_t, std::milli>(*_animationTimeLast)
          * animationTimeScale;
  _animationTime += static_cast<int>(deltaTime);
  const auto& animationTime = _animationTime;
  _animationTimeLast        = now;

  for (const auto& animatable : animatables) {
    if (animatable) {
      animatable->_animate(std::chrono::milliseconds(animationTime));
    }
  }

  // Late animation bindings
  _processLateAnimationBindings();
}

void Scene::_registerTargetForLateAnimationBinding(
  RuntimeAnimation* /*runtimeAnimation*/,
  const AnimationValue& /*originalValue*/)
{
  // TODO Implement
}

AnimationValue Scene::_processLateAnimationBindingsForMatrices(
  float holderTotalWeight, std::vector<RuntimeAnimation*>& holderAnimations,
  Matrix& holderOriginalValue)
{
  auto normalizer         = 1.f;
  auto& finalPosition     = Tmp::Vector3Array[0];
  auto& finalScaling      = Tmp::Vector3Array[1];
  auto& finalQuaternion   = Tmp::QuaternionArray[0];
  auto startIndex         = 0u;
  auto& originalAnimation = holderAnimations[0];
  auto& originalValue     = holderOriginalValue;

  auto scale = 1.f;
  if (holderTotalWeight < 1.f) {
    // We need to mix the original value in
    originalValue.decompose(finalScaling, finalQuaternion, finalPosition);
    scale = 1.f - holderTotalWeight;
  }
  else {
    startIndex = 1;
    // We need to normalize the weights
    normalizer = holderTotalWeight;
    (*originalAnimation->currentValue())
      .get<Matrix>()
      .decompose(finalScaling, finalQuaternion, finalPosition);
    scale = originalAnimation->weight / normalizer;
    if (scale == 1.f) {
      return *originalAnimation->currentValue();
    }
  }

  finalScaling.scaleInPlace(scale);
  finalPosition.scaleInPlace(scale);
  finalQuaternion.scaleInPlace(scale);

  for (size_t animIndex = startIndex; animIndex < holderAnimations.size();
       ++animIndex) {
    auto& runtimeAnimation  = holderAnimations[animIndex];
    auto iScale             = runtimeAnimation->weight / normalizer;
    auto& currentPosition   = Tmp::Vector3Array[2];
    auto& currentScaling    = Tmp::Vector3Array[3];
    auto& currentQuaternion = Tmp::QuaternionArray[1];

    (*runtimeAnimation->currentValue())
      .get<Matrix>()
      .decompose(currentScaling, currentQuaternion, currentPosition);
    currentScaling.scaleAndAddToRef(iScale, finalScaling);
    currentQuaternion.scaleAndAddToRef(iScale, finalQuaternion);
    currentPosition.scaleAndAddToRef(iScale, finalPosition);
  }

  auto& workValue = *originalAnimation->_animationState.workValue;
  Matrix::ComposeToRef(finalScaling, finalQuaternion, finalPosition,
                       workValue.get<Matrix>());
  originalAnimation->_animationState.workValue = workValue;
  return (*originalAnimation->currentValue());
}

Quaternion Scene::_processLateAnimationBindingsForQuaternions(
  float holderTotalWeight, std::vector<RuntimeAnimation*>& holderAnimations,
  Quaternion& holderOriginalValue, Quaternion& refQuaternion)
{
  auto& originalAnimation = holderAnimations[0];
  auto& originalValue     = holderOriginalValue;

  if (holderAnimations.size() == 1) {
    Quaternion::SlerpToRef(
      originalValue, (*originalAnimation->currentValue()).get<Quaternion>(),
      std::min(1.f, holderTotalWeight), refQuaternion);
    return refQuaternion;
  }

  auto normalizer = 1.f;
  std::vector<Quaternion> quaternions;
  Float32Array weights;

  if (holderTotalWeight < 1.f) {
    auto scale = 1.f - holderTotalWeight;

    quaternions.clear();
    weights.clear();

    quaternions.emplace_back(originalValue);
    weights.emplace_back(scale);
  }
  else {
    if (holderAnimations.size() == 2) { // Slerp as soon as we can
      Quaternion::SlerpToRef(
        (*holderAnimations[0]->currentValue()).get<Quaternion>(),
        (*holderAnimations[1]->currentValue()).get<Quaternion>(),
        holderAnimations[1]->weight / holderTotalWeight, refQuaternion);
      return refQuaternion;
    }
    quaternions.clear();
    weights.clear();

    normalizer = holderTotalWeight;
  }
  for (const auto& runtimeAnimation : holderAnimations) {
    quaternions.emplace_back(
      (*runtimeAnimation->currentValue()).get<Quaternion>());
    weights.emplace_back(runtimeAnimation->weight / normalizer);
  }

  // https://gamedev.stackexchange.com/questions/62354/method-for-interpolation-between-3-quaternions

  auto cumulativeAmount                          = 0.f;
  std::optional<Quaternion> cumulativeQuaternion = std::nullopt;
  for (size_t index = 0; index < quaternions.size();) {
    if (!cumulativeQuaternion) {
      Quaternion::SlerpToRef(quaternions[index], quaternions[index + 1],
                             weights[index + 1]
                               / (weights[index] + weights[index + 1]),
                             refQuaternion);
      cumulativeQuaternion = refQuaternion;
      cumulativeAmount     = weights[index] + weights[index + 1];
      index += 2;
      continue;
    }
    cumulativeAmount += weights[index];
    auto _cumulativeQuaternion = *cumulativeQuaternion;
    Quaternion::SlerpToRef(*cumulativeQuaternion, quaternions[index],
                           weights[index] / cumulativeAmount,
                           _cumulativeQuaternion);
    cumulativeQuaternion = _cumulativeQuaternion;
    ++index;
  }

  return cumulativeQuaternion ? *cumulativeQuaternion : Quaternion();
}

void Scene::_processLateAnimationBindings()
{
  // TODO Implement
}

Matrix& Scene::getViewMatrix()
{
  return _viewMatrix;
}

const Matrix& Scene::getViewMatrix() const
{
  return _viewMatrix;
}

Matrix& Scene::getProjectionMatrix()
{
  return _projectionMatrix;
}

const Matrix& Scene::getProjectionMatrix() const
{
  return _projectionMatrix;
}

Matrix& Scene::getTransformMatrix()
{
  return _transformMatrix;
}

const Matrix& Scene::getTransformMatrix() const
{
  return _transformMatrix;
}

void Scene::setTransformMatrix(const Matrix& viewL, const Matrix& projectionL,
                               const std::optional<Matrix>& viewR,
                               const std::optional<Matrix>& projectionR)
{
  if (_viewUpdateFlag == viewL.updateFlag
      && _projectionUpdateFlag == projectionL.updateFlag) {
    return;
  }

  _viewUpdateFlag       = viewL.updateFlag;
  _projectionUpdateFlag = projectionL.updateFlag;
  _viewMatrix           = viewL;
  _projectionMatrix     = projectionL;

  _viewMatrix.multiplyToRef(_projectionMatrix, _transformMatrix);

  // Update frustum
  if (!_frustumPlanesSet) {
    _frustumPlanesSet = true;
    _frustumPlanes    = Frustum::GetPlanes(_transformMatrix);
  }
  else {
    Frustum::GetPlanesToRef(_transformMatrix, _frustumPlanes);
  }

  if (_multiviewSceneUbo && _multiviewSceneUbo->useUbo()) {
    _updateMultiviewUbo(viewR, projectionR);
  }
  else if (_sceneUbo->useUbo()) {
    _sceneUbo->updateMatrix("viewProjection", _transformMatrix);
    _sceneUbo->updateMatrix("view", _viewMatrix);
    _sceneUbo->update();
  }
}

std::unique_ptr<UniformBuffer>& Scene::getSceneUniformBuffer()
{
  return _multiviewSceneUbo ? _multiviewSceneUbo : _sceneUbo;
}

size_t Scene::getUniqueId()
{
  auto result = Scene::_uniqueIdCounter;
  Scene::_uniqueIdCounter++;
  return result;
}

void Scene::addMesh(const AbstractMeshPtr& newMesh, bool recursive)
{
  meshes.emplace_back(newMesh);

  newMesh->_resyncLightSources();

  onNewMeshAddedObservable.notifyObservers(newMesh.get());

  if (recursive) {
    // for (const auto& m : newMesh->getChildMeshes()) {
    //   addMesh(m);
    // }
  }
}

int Scene::removeMesh(const AbstractMeshPtr& toRemove, bool recursive)
{
  return removeMesh(toRemove.get(), recursive);
}

int Scene::removeMesh(AbstractMesh* toRemove, bool recursive)
{
  auto it = std::find_if(
    meshes.begin(), meshes.end(),
    [toRemove](const AbstractMeshPtr& mesh) { return mesh.get() == toRemove; });
  auto index = static_cast<int>(it - meshes.begin());
  if (it != meshes.end()) {
    // Remove from the scene if mesh found
    meshes.erase(it);
  }

  onMeshRemovedObservable.notifyObservers(toRemove);

  if (recursive) {
    for (const auto& m : toRemove->getChildMeshes()) {
      removeMesh(m);
    }
  }

  return index;
}

void Scene::addTransformNode(const TransformNodePtr& newTransformNode)
{
  newTransformNode->_indexInSceneTransformNodesArray
    = static_cast<int>(transformNodes.size());
  transformNodes.emplace_back(newTransformNode);

  onNewTransformNodeAddedObservable.notifyObservers(newTransformNode.get());
}

int Scene::removeTransformNode(const TransformNodePtr& toRemove)
{
  return removeTransformNode(toRemove.get());
}

int Scene::removeTransformNode(TransformNode* toRemove)
{
  auto index = toRemove->_indexInSceneTransformNodesArray;
  if (index != -1) {

    if (index != static_cast<int>(transformNodes.size()) - 1) {
      const auto& lastNode                       = transformNodes.back();
      transformNodes[static_cast<size_t>(index)] = lastNode;
      lastNode->_indexInSceneTransformNodesArray = index;
    }

    toRemove->_indexInSceneTransformNodesArray = -1;
    transformNodes.pop_back();
  }

  onTransformNodeRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeSkeleton(const SkeletonPtr& toRemove)
{
  return removeSkeleton(toRemove.get());
}

int Scene::removeSkeleton(Skeleton* toRemove)
{
  auto it    = std::find_if(skeletons.begin(), skeletons.end(),
                         [toRemove](const SkeletonPtr& skeleton) {
                           return skeleton.get() == toRemove;
                         });
  auto index = static_cast<int>(it - skeletons.begin());
  if (it != skeletons.end()) {
    // Remove from the scene if found
    skeletons.erase(it);
    onSkeletonRemovedObservable.notifyObservers(toRemove);
  }

  return index;
}

int Scene::removeMorphTargetManager(const MorphTargetManagerPtr& toRemove)
{
  auto it    = std::find(morphTargetManagers.begin(), morphTargetManagers.end(),
                      toRemove);
  auto index = static_cast<int>(it - morphTargetManagers.begin());
  if (it != morphTargetManagers.end()) {
    // Remove from the scene if found
    morphTargetManagers.erase(it);
  }

  return index;
}

int Scene::removeLight(const LightPtr& toRemove)
{
  return removeLight(toRemove.get());
}

int Scene::removeLight(Light* toRemove)
{
  auto it = std::find_if(
    lights.begin(), lights.end(),
    [&toRemove](const LightPtr& light) { return light.get() == toRemove; });
  auto index = static_cast<int>(it - lights.begin());
  if (it != lights.end()) {
    // Remove from meshes
    for (const auto& mesh : meshes) {
      mesh->_removeLightSource(toRemove);
    }
    // Remove from the scene if mesh found
    lights.erase(it);
    sortLightsByPriority();
  }

  onLightRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeCamera(const CameraPtr& toRemove)
{
  return removeCamera(toRemove.get());
}

int Scene::removeCamera(Camera* toRemove)
{
  auto it1 = std::find_if(
    cameras.begin(), cameras.end(),
    [&toRemove](const CameraPtr& camera) { return camera.get() == toRemove; });
  auto index = static_cast<int>(it1 - cameras.begin());
  if (it1 != cameras.end()) {
    // Remove from the scene if camera found
    cameras.erase(it1);
  }
  // Remove from activeCameras
  auto it2 = std::find_if(
    activeCameras.begin(), activeCameras.end(),
    [toRemove](const CameraPtr& camera) { return camera.get() == toRemove; });
  if (it2 != activeCameras.end()) {
    // Remove from the scene if camera found
    activeCameras.erase(it2);
  }
  // Reset the activeCamera
  if (_activeCamera.get() == toRemove) {
    if (!cameras.empty()) {
      activeCamera = cameras.front();
    }
    else {
      activeCamera = nullptr;
    }
  }

  onCameraRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeParticleSystem(IParticleSystem* toRemove)
{
  auto it    = std::find_if(particleSystems.begin(), particleSystems.end(),
                         [&toRemove](const IParticleSystemPtr& particleSystem) {
                           return particleSystem.get() == toRemove;
                         });
  auto index = static_cast<int>(it - particleSystems.begin());
  if (it != particleSystems.end()) {
    particleSystems.erase(it);
  }
  return index;
}

int Scene::removeAnimation(const AnimationPtr& toRemove)
{
  auto it    = std::find(animations.begin(), animations.end(), toRemove);
  auto index = static_cast<int>(it - animations.begin());
  if (it != animations.end()) {
    animations.erase(it);
  }
  return index;
}

int Scene::removeAnimationGroup(const AnimationGroupPtr& toRemove)
{
  auto it = std::find(animationGroups.begin(), animationGroups.end(), toRemove);
  auto index = static_cast<int>(it - animationGroups.begin());
  if (it != animationGroups.end()) {
    animationGroups.erase(it);
  }
  return index;
}

int Scene::removeMultiMaterial(const MultiMaterialPtr& toRemove)
{
  auto it = std::find(multiMaterials.begin(), multiMaterials.end(), toRemove);
  auto index = static_cast<int>(it - multiMaterials.begin());
  if (it != multiMaterials.end()) {
    multiMaterials.erase(it);
  }
  return index;
}

int Scene::removeMaterial(const MaterialPtr& toRemove)
{
  return removeMaterial(toRemove.get());
}

int Scene::removeMaterial(Material* toRemove)
{
  auto index = toRemove->_indexInSceneMaterialArray;
  if (index != -1 && index < static_cast<int>(materials.size())) {
    if (index != static_cast<int>(materials.size()) - 1) {
      const auto& lastMaterial                 = materials.back();
      materials[static_cast<size_t>(index)]    = lastMaterial;
      lastMaterial->_indexInSceneMaterialArray = index;
    }

    toRemove->_indexInSceneMaterialArray = -1;
    materials.pop_back();
  }

  onMaterialRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeActionManager(const AbstractActionManagerPtr& toRemove)
{
  return removeActionManager(toRemove.get());
}

int Scene::removeActionManager(AbstractActionManager* toRemove)
{
  auto it
    = std::find_if(actionManagers.begin(), actionManagers.end(),
                   [toRemove](const AbstractActionManagerPtr& actionManager) {
                     return actionManager.get() == toRemove;
                   });
  int index = static_cast<int>(it - actionManagers.begin());
  if (it != actionManagers.end()) {
    actionManagers.erase(it);
  }
  return index;
}

int Scene::removeTexture(const BaseTexturePtr& toRemove)
{
  return removeTexture(toRemove.get());
}

int Scene::removeTexture(BaseTexture* toRemove)
{
  auto it    = std::find_if(textures.begin(), textures.end(),
                         [toRemove](const BaseTexturePtr& texture) {
                           return texture.get() == toRemove;
                         });
  auto index = static_cast<int>(it - textures.begin());
  if (it != textures.end()) {
    textures.erase(it);
  }
  onTextureRemovedObservable.notifyObservers(toRemove);

  return index;
}

void Scene::addLight(const LightPtr& newLight)
{
  lights.emplace_back(newLight);
  sortLightsByPriority();

  // Add light to all meshes (To support if the light is removed and then
  // readded)
  for (const auto& mesh : meshes) {
    if (!stl_util::contains(mesh->lightSources(), newLight)) {
      mesh->lightSources().emplace_back(newLight);
      mesh->_resyncLightSources();
    }
  }

  onNewLightAddedObservable.notifyObservers(newLight.get());
}

void Scene::sortLightsByPriority()
{
  if (requireLightSorting) {
    std::sort(lights.begin(), lights.end(),
              [](const LightPtr& a, const LightPtr& b) {
                return Light::CompareLightsPriority(a.get(), b.get());
              });
  }
}

void Scene::addCamera(const CameraPtr& newCamera)
{
  cameras.emplace_back(newCamera);
  onNewCameraAddedObservable.notifyObservers(newCamera.get());
}

void Scene::addSkeleton(const SkeletonPtr& newSkeleton)
{
  skeletons.emplace_back(newSkeleton);
  onNewSkeletonAddedObservable.notifyObservers(newSkeleton.get());
}

void Scene::addParticleSystem(const IParticleSystemPtr& newParticleSystem)
{
  particleSystems.emplace_back(newParticleSystem);
}

void Scene::addAnimation(const AnimationPtr& newAnimation)
{
  animations.emplace_back(newAnimation);
}

void Scene::addAnimationGroup(const AnimationGroupPtr& newAnimationGroup)
{
  animationGroups.emplace_back(newAnimationGroup);
}

void Scene::addMultiMaterial(const MultiMaterialPtr& newMultiMaterial)
{
  multiMaterials.emplace_back(newMultiMaterial);
}

void Scene::addMaterial(const MaterialPtr& newMaterial)
{
  newMaterial->_indexInSceneMaterialArray = static_cast<int>(materials.size());
  materials.emplace_back(newMaterial);
  onNewMaterialAddedObservable.notifyObservers(newMaterial.get());
}

void Scene::addMorphTargetManager(
  const MorphTargetManagerPtr& newMorphTargetManager)
{
  morphTargetManagers.emplace_back(newMorphTargetManager);
}

void Scene::addGeometry(const GeometryPtr& newGeometry)
{
  geometriesByUniqueId[newGeometry->uniqueId] = geometries.size();
  geometries.emplace_back(newGeometry);
}

void Scene::addActionManager(const AbstractActionManagerPtr& newActionManager)
{
  actionManagers.emplace_back(newActionManager);
}

void Scene::addTexture(const BaseTexturePtr& newTexture)
{
  textures.emplace_back(newTexture);
  onNewTextureAddedObservable.notifyObservers(newTexture.get());
}

void Scene::switchActiveCamera(const CameraPtr& newCamera, bool attachControl)
{
  auto canvas = _engine->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  if (_activeCamera) {
    _activeCamera->detachControl(canvas);
  }

  activeCamera = newCamera;
  if (attachControl) {
    newCamera->attachControl(canvas);
  }
}

CameraPtr Scene::setActiveCameraByID(const std::string& id)
{
  auto camera = getCameraByID(id);

  if (camera) {
    activeCamera = camera;
    return camera;
  }

  return nullptr;
}

CameraPtr Scene::setActiveCameraByName(const std::string& name)
{
  auto camera = getCameraByName(name);

  if (camera) {
    activeCamera = camera;
    return camera;
  }

  return nullptr;
}

AnimationGroupPtr Scene::getAnimationGroupByName(const std::string& name)
{
  auto it = std::find_if(animationGroups.begin(), animationGroups.end(),
                         [&name](const AnimationGroupPtr& animationGroup) {
                           return animationGroup->name == name;
                         });

  return (it == animationGroups.end()) ? nullptr : *it;
}

MaterialPtr Scene::getMaterialByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(materials.begin(), materials.end(),
                         [uniqueId](const MaterialPtr& material) {
                           return material->uniqueId == uniqueId;
                         });

  return (it == materials.end()) ? nullptr : *it;
}

MaterialPtr Scene::getMaterialByID(const std::string& id)
{
  auto it = std::find_if(
    materials.begin(), materials.end(),
    [&id](const MaterialPtr& material) { return material->id == id; });

  return (it == materials.end()) ? nullptr : *it;
}

MaterialPtr Scene::getMaterialByName(const std::string& name)
{
  auto it = std::find_if(
    materials.begin(), materials.end(),
    [&name](const MaterialPtr& material) { return material->name == name; });

  return (it == materials.end()) ? nullptr : *it;
}

CameraPtr Scene::getCameraByID(const std::string& id)
{
  auto it
    = std::find_if(cameras.begin(), cameras.end(),
                   [&id](const CameraPtr& camera) { return camera->id == id; });

  return (it == cameras.end()) ? nullptr : *it;
}

CameraPtr Scene::getCameraByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(cameras.begin(), cameras.end(),
                         [uniqueId](const CameraPtr& camera) {
                           return camera->uniqueId == uniqueId;
                         });

  return (it == cameras.end()) ? nullptr : *it;
}

CameraPtr Scene::getCameraByName(const std::string& name)
{
  auto it = std::find_if(
    cameras.begin(), cameras.end(),
    [&name](const CameraPtr& camera) { return camera->name == name; });

  return (it == cameras.end()) ? nullptr : *it;
}

BonePtr Scene::getBoneByID(const std::string& id)
{
  for (const auto& skeleton : skeletons) {
    for (const auto& bone : skeleton->bones) {
      if (bone->id == id) {
        return bone;
      }
    }
  }

  return nullptr;
}

BonePtr Scene::getBoneByUniqueID(size_t uniqueId)
{
  for (const auto& skeleton : skeletons) {
    for (const auto& bone : skeleton->bones) {
      if (bone->uniqueId == uniqueId) {
        return bone;
      }
    }
  }

  return nullptr;
}

BonePtr Scene::getBoneByName(const std::string& name)
{
  for (const auto& skeleton : skeletons) {
    for (const auto& bone : skeleton->bones) {
      if (bone->name == name) {
        return bone;
      }
    }
  }

  return nullptr;
}

LightPtr Scene::getLightByName(const std::string& name)
{
  auto it = std::find_if(
    lights.begin(), lights.end(),
    [&name](const LightPtr& light) { return light->name == name; });

  return (it == lights.end()) ? nullptr : *it;
}

LightPtr Scene::getLightByID(const std::string& id)
{
  auto it
    = std::find_if(lights.begin(), lights.end(),
                   [&id](const LightPtr& light) { return light->id == id; });

  return (it == lights.end()) ? nullptr : *it;
}

LightPtr Scene::getLightByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(
    lights.begin(), lights.end(),
    [uniqueId](const LightPtr& light) { return light->uniqueId == uniqueId; });

  return (it == lights.end()) ? nullptr : *it;
}

IParticleSystemPtr Scene::getParticleSystemByID(const std::string& id)
{
  auto it = std::find_if(particleSystems.begin(), particleSystems.end(),
                         [&id](const IParticleSystemPtr& particleSystem) {
                           return particleSystem->id == id;
                         });

  return (it == particleSystems.end()) ? nullptr : *it;
}

GeometryPtr Scene::getGeometryByID(const std::string& id)
{
  auto it = std::find_if(
    geometries.begin(), geometries.end(),
    [&id](const GeometryPtr& geometry) { return geometry->id == id; });

  return (it == geometries.end()) ? nullptr : *it;
}

GeometryPtr Scene::_getGeometryByUniqueID(size_t uniqueId)
{
  if (!geometriesByUniqueId.empty()) {
    if (!stl_util::contains(geometriesByUniqueId, uniqueId)) {
      const auto index = geometriesByUniqueId[uniqueId];
      return geometries[index];
    }
  }
  else {
    for (const auto& geometry : geometries) {
      if (geometry->uniqueId == uniqueId) {
        return geometry;
      }
    }
  }

  return nullptr;
}

bool Scene::pushGeometry(const GeometryPtr& geometry, bool force)
{
  if (!force && _getGeometryByUniqueID(geometry->uniqueId)) {
    return false;
  }

  addGeometry(geometry);

  onNewGeometryAddedObservable.notifyObservers(geometry.get());

  return true;
}

bool Scene::removeGeometry(const GeometryPtr& geometry)
{
  return removeGeometry(geometry.get());
}

bool Scene::removeGeometry(Geometry* geometry)
{
  size_t index = 0;
  if (!geometriesByUniqueId.empty()) {
    if (!stl_util::contains(geometriesByUniqueId, geometry->uniqueId)) {
      return false;
    }
    index = geometriesByUniqueId[geometry->uniqueId];
  }
  else {
    auto it = std::find_if(geometries.begin(), geometries.end(),
                           [geometry](const GeometryPtr& _geometry) {
                             return _geometry.get() == geometry;
                           });
    if (it == geometries.end()) {
      return false;
    }
    index = static_cast<size_t>(it - geometries.begin());
  }

  if (index != geometries.size() - 1) {
    auto lastGeometry = geometries.back();
    geometries[index] = lastGeometry;
    /* if (!geometriesByUniqueId.empty()) */ {
      geometriesByUniqueId[lastGeometry->uniqueId] = index;
      geometriesByUniqueId.erase(geometry->uniqueId);
    }
  }

  geometries.pop_back();

  onGeometryRemovedObservable.notifyObservers(geometry);

  return true;
}

std::vector<GeometryPtr>& Scene::getGeometries()
{
  return geometries;
}

AbstractMeshPtr Scene::getMeshByID(const std::string& id)
{
  auto it = std::find_if(
    meshes.begin(), meshes.end(),
    [&id](const AbstractMeshPtr& mesh) { return mesh->id == id; });

  return (it == meshes.end()) ? nullptr : *it;
}

std::vector<AbstractMeshPtr> Scene::getMeshesByID(const std::string& id)
{
  std::vector<AbstractMeshPtr> filteredMeshes;
  std::for_each(meshes.begin(), meshes.end(),
                [&filteredMeshes, &id](const AbstractMeshPtr& mesh) {
                  if (mesh->id == id) {
                    filteredMeshes.emplace_back(mesh);
                  }
                });
  return filteredMeshes;
}

TransformNodePtr Scene::getTransformNodeByID(const std::string& id)
{
  auto it = std::find_if(transformNodes.begin(), transformNodes.end(),
                         [&id](const TransformNodePtr& transformNode) {
                           return transformNode->id == id;
                         });

  return (it == transformNodes.end()) ? nullptr : *it;
}

TransformNodePtr Scene::getTransformNodeByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(transformNodes.begin(), transformNodes.end(),
                         [&uniqueId](const TransformNodePtr& transformNode) {
                           return transformNode->uniqueId == uniqueId;
                         });

  return (it == transformNodes.end()) ? nullptr : *it;
}

std::vector<TransformNodePtr>
Scene::getTransformNodesByID(const std::string& id)
{
  std::vector<TransformNodePtr> filteredTransformNodes;
  std::for_each(
    transformNodes.begin(), transformNodes.end(),
    [&filteredTransformNodes, &id](const TransformNodePtr& transformNode) {
      if (transformNode->id == id) {
        filteredTransformNodes.emplace_back(transformNode);
      }
    });
  return filteredTransformNodes;
}

AbstractMeshPtr Scene::getMeshByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(meshes.begin(), meshes.end(),
                         [&uniqueId](const AbstractMeshPtr& mesh) {
                           return mesh->uniqueId == uniqueId;
                         });

  return (it == meshes.end()) ? nullptr : *it;
}

AbstractMeshPtr Scene::getLastMeshByID(const std::string& id)
{
  if (meshes.empty()) {
    return nullptr;
  }

  for (size_t index = meshes.size(); index > 0; --index) {
    if (meshes[index - 1]->id == id) {
      return meshes[index - 1];
    }
  }

  return nullptr;
}

NodePtr Scene::getLastEntryByID(const std::string& id)
{
  size_t index;
  for (index = meshes.size(); index > 0; --index) {
    if (meshes[index - 1]->id == id) {
      return meshes[index - 1];
    }
  }

  for (index = transformNodes.size(); index > 0; --index) {
    if (transformNodes[index - 1]->id == id) {
      return transformNodes[index - 1];
    }
  }

  for (index = cameras.size(); index > 0; --index) {
    if (cameras[index - 1]->id == id) {
      return cameras[index - 1];
    }
  }

  for (index = lights.size(); index > 0; --index) {
    if (lights[index - 1]->id == id) {
      return lights[index - 1];
    }
  }

  return nullptr;
}

NodePtr Scene::getNodeByID(const std::string& id)
{
  auto mesh = getMeshByID(id);

  if (mesh) {
    return mesh;
  }

  auto transformNode = getTransformNodeByID(id);

  if (transformNode) {
    return transformNode;
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

  if (bone) {
    return bone;
  }

  return nullptr;
}

NodePtr Scene::getNodeByName(const std::string& name)
{
  auto mesh = getMeshByName(name);

  if (mesh) {
    return mesh;
  }

  auto transformNode = getTransformNodeByName(name);

  if (transformNode) {
    return transformNode;
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

  if (bone) {
    return bone;
  }

  return nullptr;
}

AbstractMeshPtr Scene::getMeshByName(const std::string& name)
{
  auto it = std::find_if(
    meshes.begin(), meshes.end(),
    [&name](const AbstractMeshPtr& mesh) { return mesh->name == name; });

  return (it == meshes.end()) ? nullptr : *it;
}

TransformNodePtr Scene::getTransformNodeByName(const std::string& name)
{
  auto it = std::find_if(transformNodes.begin(), transformNodes.end(),
                         [&name](const TransformNodePtr& transformNode) {
                           return transformNode->name == name;
                         });

  return (it == transformNodes.end()) ? nullptr : *it;
}

SoundPtr Scene::getSoundByName(const std::string& name)
{
  size_t index = 0;
  for (index = 0; index < mainSoundTrack()->soundCollection.size(); ++index) {
    if (mainSoundTrack()->soundCollection[index]->name == name) {
      return mainSoundTrack()->soundCollection[index];
    }
  }

  if (!soundTracks.empty()) {
    for (size_t sdIndex = 0; sdIndex < soundTracks.size(); ++sdIndex) {
      for (index = 0; index < soundTracks[sdIndex]->soundCollection.size();
           ++index) {
        if (soundTracks[sdIndex]->soundCollection[index]->name == name) {
          return soundTracks[sdIndex]->soundCollection[index];
        }
      }
    }
  }

  return nullptr;
}

SkeletonPtr Scene::getLastSkeletonByID(const std::string& id)
{
  for (size_t index = skeletons.size() - 1;; --index) {
    if (skeletons[index]->id == id) {
      return skeletons[index];
    }

    if (index == 0) {
      break;
    }
  }

  return nullptr;
}

SkeletonPtr Scene::getSkeletonByUniqueId(size_t uniqueId)
{
  auto it = std::find_if(skeletons.begin(), skeletons.end(),
                         [&uniqueId](const SkeletonPtr& skeleton) {
                           return skeleton->uniqueId() == uniqueId;
                         });

  return (it == skeletons.end()) ? nullptr : *it;
}

SkeletonPtr Scene::getSkeletonById(const std::string& id)
{
  auto it = std::find_if(
    skeletons.begin(), skeletons.end(),
    [&id](const SkeletonPtr& skeleton) { return skeleton->id == id; });

  return (it == skeletons.end()) ? nullptr : *it;
}

SkeletonPtr Scene::getSkeletonByName(const std::string& name)
{
  auto it = std::find_if(
    skeletons.begin(), skeletons.end(),
    [&name](const SkeletonPtr& skeleton) { return skeleton->name == name; });

  return (it == skeletons.end()) ? nullptr : *it;
}

MorphTargetManagerPtr Scene::getMorphTargetManagerById(size_t id)
{
  auto it
    = std::find_if(morphTargetManagers.begin(), morphTargetManagers.end(),
                   [&id](const MorphTargetManagerPtr& morphTargetManager) {
                     return morphTargetManager->uniqueId() == id;
                   });

  return (it == morphTargetManagers.end()) ? nullptr : *it;
}

MorphTargetPtr Scene::getMorphTargetById(const std::string& id)
{
  for (const auto& morphTargetManager : morphTargetManagers) {
    for (size_t index = 0; index < morphTargetManager->numTargets(); ++index) {
      const auto target = morphTargetManager->getTarget(index);
      if (target->id == id) {
        return target;
      }
    }
  }
  return nullptr;
}

bool Scene::isActiveMesh(const AbstractMeshPtr& mesh)
{
  return std::find_if(_activeMeshes.begin(), _activeMeshes.end(),
                      [&mesh](const AbstractMesh* activeMesh) {
                        return activeMesh == mesh.get();
                      })
         != _activeMeshes.end();
}

std::string Scene::get_uid() const
{
  return _uid;
}

void Scene::freeProcessedMaterials()
{
  _processedMaterials.clear();
}

bool Scene::get_blockfreeActiveMeshesAndRenderingGroups() const
{
  return _preventFreeActiveMeshesAndRenderingGroups;
}

void Scene::set_blockfreeActiveMeshesAndRenderingGroups(bool value)
{
  if (_preventFreeActiveMeshesAndRenderingGroups == value) {
    return;
  }

  if (value) {
    freeActiveMeshes();
    freeRenderingGroups();
  }

  _preventFreeActiveMeshesAndRenderingGroups = value;
}

void Scene::freeActiveMeshes()
{
  if (blockfreeActiveMeshesAndRenderingGroups()) {
    return;
  }

  _activeMeshes.clear();
  if (_activeCamera && !_activeCamera->_activeMeshes.empty()) {
    _activeCamera->_activeMeshes.clear();
  }
  if (!activeCameras.empty()) {
    for (const auto& iActiveCamera : activeCameras) {
      if (iActiveCamera && !iActiveCamera->_activeMeshes.empty()) {
        iActiveCamera->_activeMeshes.clear();
      }
    }
  }
}

void Scene::freeRenderingGroups()
{
  if (blockfreeActiveMeshesAndRenderingGroups()) {
    return;
  }

  if (_renderingManager) {
    _renderingManager->freeRenderingGroups();
  }
  if (!textures.empty()) {
    for (const auto& texture : textures) {
      if (texture) {
        auto renderTargetTexture
          = std::dynamic_pointer_cast<RenderTargetTexture>(texture);
        if (renderTargetTexture && !renderTargetTexture->renderList().empty()) {
          renderTargetTexture->freeRenderingGroups();
        }
      }
    }
  }
}

void Scene::_evaluateSubMesh(SubMesh* subMesh, AbstractMesh* mesh,
                             AbstractMesh* initialMesh)
{
  if (initialMesh->isAnInstance() || dispatchAllSubMeshesOfActiveMeshes
      || mesh->alwaysSelectAsActiveMesh || mesh->subMeshes.size() == 1
      || subMesh->isInFrustum(_frustumPlanes)) {

    for (const auto& step : _evaluateSubMeshStage) {
      step.action(mesh, subMesh);
    }

    auto material = subMesh->getMaterial();
    if (material) {
      // Render targets
      if (material->hasRenderTargetTextures
          && material->getRenderTargetTextures != nullptr) {
        if (std::find(_processedMaterials.begin(), _processedMaterials.end(),
                      material)
            == _processedMaterials.end()) {
          _processedMaterials.emplace_back(material);
          for (const auto& renderTarget : material->getRenderTargetTextures()) {
            if (std::find(_renderTargets.begin(), _renderTargets.end(),
                          renderTarget)
                == _renderTargets.end()) {
              _renderTargets.emplace_back(renderTarget);
            }
          }
        }
      }

      // Dispatch
      _activeIndices.addCount(subMesh->indexCount, false);
      _renderingManager->dispatch(subMesh, mesh, material);
    }
  }
}

bool Scene::_isInIntermediateRendering() const
{
  return _intermediateRendering;
}

void Scene::setActiveMeshCandidateProvider(
  IActiveMeshCandidateProvider* provider)
{
  _activeMeshCandidateProvider = provider;
}

IActiveMeshCandidateProvider* Scene::getActiveMeshCandidateProvider() const
{
  return _activeMeshCandidateProvider;
}

Scene& Scene::freezeActiveMeshes()
{
  if (!_activeCamera) {
    return *this;
  }

  if (!_frustumPlanesSet) {
    _frustumPlanesSet = true;
    setTransformMatrix(_activeCamera->getViewMatrix(),
                       _activeCamera->getProjectionMatrix());
  }

  _evaluateActiveMeshes();
  _activeMeshesFrozen = true;

  for (const auto& mesh : _activeMeshes) {
    mesh->_freeze();
  }
  return *this;
}

Scene& Scene::unfreezeActiveMeshes()
{
  for (const auto& mesh : _activeMeshes) {
    mesh->_unFreeze();
  }

  _activeMeshesFrozen = false;
  return *this;
}

void Scene::_evaluateActiveMeshes()
{
  if (_activeMeshesFrozen && !_activeMeshes.empty()) {

    for (const auto& mesh : _activeMeshes) {
      mesh->computeWorldMatrix();
    }

    return;
  }

  if (!_activeCamera) {
    return;
  }

  onBeforeActiveMeshesEvaluationObservable.notifyObservers(this);

  _activeCamera->_activeMeshes.clear();
  _activeMeshes.clear();
  _renderingManager->reset();
  _processedMaterials.clear();
  _activeParticleSystems.clear();
  _activeSkeletons.clear();
  _softwareSkinnedMeshes.clear();

  for (const auto& step : _beforeEvaluateActiveMeshStage) {
    step.action();
  }

  // Determine mesh candidates
  auto _meshes = getActiveMeshCandidates();

  // Check each mesh
  for (const auto& mesh : _meshes) {
    if (mesh->isBlocked()) {
      continue;
    }

    _totalVertices.addCount(mesh->getTotalVertices(), false);

    if (!mesh->isReady() || !mesh->isEnabled()
        || mesh->scaling().lengthSquared() == 0.f) {
      continue;
    }

    mesh->computeWorldMatrix();

    // Intersections
    if (mesh->actionManager
        && mesh->actionManager->hasSpecificTriggers2(
          Constants::ACTION_OnIntersectionEnterTrigger,
          Constants::ACTION_OnIntersectionExitTrigger)) {
      if (std::find(_meshesForIntersections.begin(),
                    _meshesForIntersections.end(), mesh)
          == _meshesForIntersections.end()) {
        _meshesForIntersections.emplace_back(mesh);
      }
    }

    // Switch to current LOD
    auto meshToRender = customLODSelector ?
                          customLODSelector(mesh, _activeCamera) :
                          mesh->getLOD(activeCamera);
    if (meshToRender == nullptr) {
      continue;
    }

    // Compute world matrix if LOD is billboard
    if (meshToRender != mesh
        && meshToRender->billboardMode != TransformNode::BILLBOARDMODE_NONE) {
      meshToRender->computeWorldMatrix();
    }

    mesh->_preActivate();

    if (mesh->isVisible && mesh->visibility > 0
        && ((mesh->alwaysSelectAsActiveMesh
             || (((mesh->layerMask & _activeCamera->layerMask) != 0)
                 && (mesh->alwaysSelectAsActiveMesh
                     || mesh->isInFrustum(_frustumPlanes)))))) {
      _activeMeshes.emplace_back(mesh);
      _activeCamera->_activeMeshes.emplace_back(_activeMeshes.back());

      if (meshToRender != mesh) {
        meshToRender->_activate(_renderId, false);
      }

      if (mesh->_activate(_renderId, false)) {
        if (!mesh->isAnInstance) {
          meshToRender->_internalAbstractMeshDataInfo._onlyForInstances = false;
        }
        meshToRender->_internalAbstractMeshDataInfo._isActive = true;
        _activeMesh(mesh, meshToRender);
      }

      mesh->_postActivate();
    }
  }

  onAfterActiveMeshesEvaluationObservable.notifyObservers(this);

  // Particle systems
  if (particlesEnabled) {
    onBeforeParticlesRenderingObservable.notifyObservers(this);
    for (const auto& particleSystem : particleSystems) {
      if (!particleSystem->isStarted() || !particleSystem->hasEmitter()) {
        continue;
      }

      if (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)
          && std::get<AbstractMeshPtr>(particleSystem->emitter)->isEnabled()) {
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
  if (_skeletonsEnabled && mesh->skeleton()) {
    if (std::find(_activeSkeletons.begin(), _activeSkeletons.end(),
                  mesh->skeleton())
        == _activeSkeletons.end()) {
      _activeSkeletons.emplace_back(mesh->skeleton());
      mesh->skeleton()->prepare();
    }

    if (!mesh->computeBonesUsingShaders()) {
      if (auto _mesh = static_cast<Mesh*>(mesh)) {
        if (std::find(_softwareSkinnedMeshes.begin(),
                      _softwareSkinnedMeshes.end(), _mesh)
            == _softwareSkinnedMeshes.end()) {
          _softwareSkinnedMeshes.emplace_back(_mesh);
        }
      }
    }
  }

  for (const auto& step : _activeMeshStage) {
    step.action(sourceMesh, mesh);
  }

  if (mesh && !mesh->subMeshes.empty()) {
    auto subMeshes = getActiveSubMeshCandidates(mesh);
    for (const auto& subMesh : subMeshes) {
      _evaluateSubMesh(subMesh, mesh, sourceMesh);
    }
  }
}

void Scene::updateTransformMatrix(bool force)
{
  if (!_activeCamera) {
    return;
  }

  setTransformMatrix(_activeCamera->getViewMatrix(force),
                     _activeCamera->getProjectionMatrix(force));
}

void Scene::_bindFrameBuffer()
{
  if (_activeCamera && _activeCamera->_multiviewTexture) {
    _activeCamera->_multiviewTexture->_bindFrameBuffer();
  }
  else if (_activeCamera && _activeCamera->outputRenderTarget) {
    auto useMultiview
      = getEngine()->getCaps().multiview && _activeCamera->outputRenderTarget
        && _activeCamera->outputRenderTarget->getViewCount() > 1;
    if (useMultiview) {
      _activeCamera->outputRenderTarget->_bindFrameBuffer();
    }
    else {
      auto internalTexture
        = _activeCamera->outputRenderTarget->getInternalTexture();
      if (internalTexture) {
        getEngine()->bindFramebuffer(internalTexture);
      }
      else {
        BABYLON_LOG_ERROR("Engine",
                          "Camera contains invalid customDefaultRenderTarget")
      }
    }
  }
  else {
    getEngine()->restoreDefaultFramebuffer(); // Restore back buffer if needed
  }
}

void Scene::_renderForCamera(const CameraPtr& camera,
                             const CameraPtr& rigParent)
{
  if (camera && camera->_skipRendering) {
    return;
  }

  auto engine = _engine;

  // Use _activeCamera instead of activeCamera to avoid onActiveCameraChanged
  _activeCamera = camera;

  if (!_activeCamera) {
    BABYLON_LOG_ERROR("Scene", "Active camera not set")
    return;
  }

  // Viewport
  engine->setViewport(_activeCamera->viewport);

  // Camera
  resetCachedMaterial();
  ++_renderId;

  auto useMultiview = getEngine()->getCaps().multiview
                      && camera->outputRenderTarget
                      && camera->outputRenderTarget->getViewCount() > 1;
  if (useMultiview) {
    setTransformMatrix(camera->_rigCameras[0]->getViewMatrix(),
                       camera->_rigCameras[0]->getProjectionMatrix(),
                       camera->_rigCameras[1]->getViewMatrix(),
                       camera->_rigCameras[1]->getProjectionMatrix());
  }
  else {
    updateTransformMatrix();
  }

  onBeforeCameraRenderObservable.notifyObservers(_activeCamera.get());

  // Meshes
  _evaluateActiveMeshes();

  // Software skinning
  for (const auto& mesh : _softwareSkinnedMeshes) {
    mesh->applySkeleton(mesh->skeleton());
  }

  // Render targets
  onBeforeRenderTargetsRenderObservable.notifyObservers(this);

  if (!camera->customRenderTargets.empty()) {
    stl_util::concat_with_no_duplicates(_renderTargets,
                                        camera->customRenderTargets);
  }

  if (rigParent && !rigParent->customRenderTargets.empty()) {
    stl_util::concat_with_no_duplicates(_renderTargets,
                                        rigParent->customRenderTargets);
  }

  // Collects render targets from external components.
  for (const auto& step : _gatherActiveCameraRenderTargetsStage) {
    step.action(_renderTargets);
  }

  if (renderTargetsEnabled) {
    _intermediateRendering = true;
    auto needRebind        = false;

    if (!_renderTargets.empty()) {
      Tools::StartPerformanceCounter("Render targets", !_renderTargets.empty());
      for (const auto& renderTarget : _renderTargets) {
        if (renderTarget->_shouldRender()) {
          ++_renderId;
          bool hasSpecialRenderTargetCamera
            = renderTarget->activeCamera
              && renderTarget->activeCamera != _activeCamera;
          renderTarget->render(hasSpecialRenderTargetCamera,
                               dumpNextRenderTargets);
          needRebind = true;
        }
      }
      Tools::EndPerformanceCounter("Render targets", !_renderTargets.empty());

      ++_renderId;
    }

    for (const auto& step : _cameraDrawRenderTargetStage) {
      needRebind = step.action(_activeCamera.get()) || needRebind;
    }

    _intermediateRendering = false;

    // Restore framebuffer after rendering to targets
    if (needRebind) {
      _bindFrameBuffer();
    }
  }

  onAfterRenderTargetsRenderObservable.notifyObservers(this);

  // Prepare Frame
  if (postProcessManager && !camera->_multiviewTexture) {
    postProcessManager->_prepareFrame(nullptr);
  }

  // Before Camera Draw
  for (const auto& step : _beforeCameraDrawStage) {
    step.action(_activeCamera.get());
  }

  // Render
  onBeforeDrawPhaseObservable.notifyObservers(this);
  _renderingManager->render(nullptr, {}, true, true);
  onAfterDrawPhaseObservable.notifyObservers(this);

  // After Camera Draw
  for (const auto& step : _afterCameraDrawStage) {
    step.action(_activeCamera.get());
  }

  // Finalize frame
  if (postProcessManager && !camera->_multiviewTexture) {
    postProcessManager->_finalizeFrame(camera->isIntermediate);
  }

  // Reset some special arrays
  _renderTargets.clear();

  onAfterCameraRenderObservable.notifyObservers(_activeCamera.get());
}

void Scene::_processSubCameras(const CameraPtr& camera)
{
  if (camera->cameraRigMode == Camera::RIG_MODE_NONE
      || (camera->outputRenderTarget
          && camera->outputRenderTarget->getViewCount() > 1
          && getEngine()->getCaps().multiview)) {
    _renderForCamera(camera);
    return;
  }

  if (camera->_useMultiviewToSingleView) {
    _renderMultiviewToSingleView(camera);
  }
  else {
    // rig cameras
    for (const auto& rigCamera : camera->_rigCameras) {
      _renderForCamera(rigCamera, camera);
    }
  }

  // Use _activeCamera instead of activeCamera to avoid onActiveCameraChanged
  _activeCamera = camera;
  setTransformMatrix(_activeCamera->getViewMatrix(),
                     _activeCamera->getProjectionMatrix());
}

void Scene::_checkIntersections()
{
}

void Scene::animate()
{
  if (_engine->isDeterministicLockStep()) {
    auto deltaTime
      = (std::max(static_cast<float>(Scene::MinDeltaTime.count()),
                  std::min(_engine->getDeltaTime() * 1000.f,
                           static_cast<float>(Scene::MaxDeltaTime.count())))
         / 1000.f)
        + _timeAccumulator;

    auto defaultFPS = (60.f / 1000.f);

    auto defaultFrameTime = getDeterministicFrameTime();

    auto stepsTaken = 0u;

    auto maxSubSteps = _engine->getLockstepMaxSteps();

    // compute the amount of fixed steps we should have taken since the last
    // step
    auto internalSteps = static_cast<unsigned int>(
      std::floor(deltaTime / (1000.f * defaultFPS)));
    internalSteps = std::min(internalSteps, maxSubSteps);

    do {
      onBeforeStepObservable.notifyObservers(this);

      // Animations
      _animationRatio = defaultFrameTime * defaultFPS;
      _animate();
      onAfterAnimationsObservable.notifyObservers(this);

      // Physics
      _advancePhysicsEngineStep(defaultFrameTime);

      onAfterStepObservable.notifyObservers(this);
      ++_currentStepId;

      ++stepsTaken;
      deltaTime -= defaultFrameTime;

    } while (deltaTime > 0 && stepsTaken < internalSteps);

    _timeAccumulator = deltaTime < 0 ? 0 : deltaTime;
  }
  else {
    // Animations
    const float deltaTime
      = useConstantAnimationDeltaTime ?
          16.f :
          std::max(
            Time::fpMillisecondsDuration<float>(Scene::MinDeltaTime),
            std::min(_engine->getDeltaTime(),
                     Time::fpMillisecondsDuration<float>(Scene::MaxDeltaTime)));
    _animationRatio = deltaTime * (60.f / 1000.f);
    _animate();
    onAfterAnimationsObservable.notifyObservers(this);

    // Physics
    _advancePhysicsEngineStep(deltaTime);
  }
}

void Scene::render(bool updateCameras, bool ignoreAnimations)
{
  if (isDisposed()) {
    return;
  }

  ++_frameId;

  // Register components that have been associated lately to the scene.
  _registerTransientComponents();

  _activeParticles.fetchNewFrame();
  _totalVertices.fetchNewFrame();
  _activeIndices.fetchNewFrame();
  _activeBones.fetchNewFrame();
  _meshesForIntersections.clear();
  resetCachedMaterial();

  onBeforeAnimationsObservable.notifyObservers(this);

  // Actions
  if (actionManager) {
    actionManager->processTrigger(Constants::ACTION_OnEveryFrameTrigger);
  }

  // Animations
  if (!ignoreAnimations) {
    animate();
  }

  // Before camera update steps
  for (const auto& step : _beforeCameraUpdateStage) {
    step.action();
  }

  // Update Cameras
  if (updateCameras) {
    if (!activeCameras.empty()) {
      for (const auto& camera : activeCameras) {
        camera->update();
        if (camera->cameraRigMode != Camera::RIG_MODE_NONE) {
          // Rig cameras
          for (const auto& rigCamera : camera->_rigCameras) {
            rigCamera->update();
          }
        }
      }
    }
    else if (_activeCamera) {
      _activeCamera->update();
      if (_activeCamera->cameraRigMode != Camera::RIG_MODE_NONE) {
        // rig cameras
        for (const auto& rigCamera : _activeCamera->_rigCameras) {
          rigCamera->update();
        }
      }
    }
  }

  // Before render
  onBeforeRenderObservable.notifyObservers(this);

  // Customs render targets
  onBeforeRenderTargetsRenderObservable.notifyObservers(this);
  auto engine               = getEngine();
  auto& currentActiveCamera = _activeCamera;
  if (renderTargetsEnabled) {
    Tools::StartPerformanceCounter("Custom render targets",
                                   !customRenderTargets.empty());
    _intermediateRendering = true;
    for (const auto& renderTarget : customRenderTargets) {
      if (renderTarget->_shouldRender()) {
        ++_renderId;

        activeCamera = renderTarget->activeCamera ? renderTarget->activeCamera :
                                                    activeCamera;

        if (!_activeCamera) {
          BABYLON_LOG_ERROR("Scene", "Active camera not set")
          return;
        }

        // Viewport
        engine->setViewport(_activeCamera->viewport);

        // Camera
        updateTransformMatrix();

        renderTarget->render(currentActiveCamera != _activeCamera,
                             dumpNextRenderTargets);
      }
    }
    Tools::EndPerformanceCounter("Custom render targets",
                                 !customRenderTargets.empty());

    _intermediateRendering = false;
    ++_renderId;
  }

  // Restore back buffer
  activeCamera = currentActiveCamera;
  _bindFrameBuffer();

  onAfterRenderTargetsRenderObservable.notifyObservers(this);

  for (const auto& step : _beforeClearStage) {
    step.action();
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
                     autoClear || forceWireframe() || forcePointsCloud(),
                     autoClearDepthAndStencil, autoClearDepthAndStencil);
    }
  }

  // Collects render targets from external components.
  for (const auto& step : _gatherRenderTargetsStage) {
    step.action(_renderTargets);
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
    if (!_activeCamera) {
      BABYLON_LOG_ERROR("Scene", "No camera defined")
      return;
    }

    _processSubCameras(activeCamera);
  }

  // Intersection checks
  _checkIntersections();

  // Executes the after render stage actions.
  for (const auto& step : _afterRenderStage) {
    step.action();
  }

  // After render

  onAfterRenderObservable.notifyObservers(this);

  // Cleaning
  if (!_toBeDisposed.empty()) {
    for (auto& item : _toBeDisposed) {
      if (item) {
        item->dispose();
        item = nullptr;
      }
    }

    _toBeDisposed.clear();
  }

  if (dumpNextRenderTargets) {
    dumpNextRenderTargets = false;
  }

  _activeBones.addCount(0, true);
  _activeIndices.addCount(0, true);
  _activeParticles.addCount(0, true);
}

std::optional<bool>& Scene::get_audioEnabled()
{
  auto compo = std::static_pointer_cast<AudioSceneComponent>(
    _getComponent(SceneComponentConstants::NAME_AUDIO));
  if (!compo) {
    compo = AudioSceneComponent::New(this);
    _addComponent(compo);
  }

  _audioEnabled = compo->audioEnabled();

  return _audioEnabled;
}

void Scene::set_audioEnabled(const std::optional<bool>& value)
{
  if (!value.has_value()) {
    return;
  }

  auto compo = std::static_pointer_cast<AudioSceneComponent>(
    _getComponent(SceneComponentConstants::NAME_AUDIO));
  if (!compo) {
    compo = AudioSceneComponent::New(this);
    _addComponent(compo);
  }

  if (*value) {
    compo->enableAudio();
  }
  else {
    compo->disableAudio();
  }
}

std::optional<bool>& Scene::get_headphone()
{
  auto compo = std::static_pointer_cast<AudioSceneComponent>(
    _getComponent(SceneComponentConstants::NAME_AUDIO));
  if (!compo) {
    compo = AudioSceneComponent::New(this);
    _addComponent(compo);
  }

  headphone = compo->headphone();

  return _headphone;
}

void Scene::set_headphone(const std::optional<bool>& value)
{
  if (!value.has_value()) {
    return;
  }

  auto compo = std::static_pointer_cast<AudioSceneComponent>(
    _getComponent(SceneComponentConstants::NAME_AUDIO));
  if (!compo) {
    compo = AudioSceneComponent::New(this);
    _addComponent(compo);
  }

  if (*value) {
    compo->switchAudioModeForHeadphones();
  }
  else {
    compo->switchAudioModeForHeadphones();
  }
}

DepthRendererPtr Scene::enableDepthRenderer(const CameraPtr& camera)
{
  auto _camera = camera ? camera : activeCamera;
  if (!_camera) {
    throw std::runtime_error("No camera available to enable depth renderer");
  }
  if (!stl_util::contains(_depthRenderer, _camera->id)
      || !_depthRenderer[_camera->id]) {
    auto textureType = 0u;
    if (_engine->getCaps().textureHalfFloatRender) {
      textureType = Constants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (_engine->getCaps().textureFloatRender) {
      textureType = Constants::TEXTURETYPE_FLOAT;
    }
    else {
      throw std::runtime_error(
        "Depth renderer does not support int texture type");
    }
    _depthRenderer[_camera->id]
      = std::make_shared<DepthRenderer>(this, textureType, _camera);
  }

  return _depthRenderer[_camera->id];
}

void Scene::disableDepthRenderer(const CameraPtr& camera)
{
  auto _camera = camera ? camera : activeCamera;
  if (!_camera || !stl_util::contains(_depthRenderer, _camera->id)
      || !_depthRenderer[_camera->id]) {
    return;
  }

  _depthRenderer[camera->id]->dispose();
  _depthRenderer.erase(camera->id);
}

GeometryBufferRendererPtr& Scene::enableGeometryBufferRenderer(float ratio)
{
  if (_geometryBufferRenderer) {
    return _geometryBufferRenderer;
  }

  _geometryBufferRenderer
    = std::make_shared<GeometryBufferRenderer>(this, ratio);
  if (!_geometryBufferRenderer->isSupported()) {
    _geometryBufferRenderer = nullptr;
  }

  return _geometryBufferRenderer;
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
  for (const auto& material : materials) {
    material->freeze();
  }
}

void Scene::unfreezeMaterials()
{
  for (const auto& material : materials) {
    material->unfreeze();
  }
}

void Scene::dispose()
{
  beforeRender = nullptr;
  afterRender  = nullptr;

  skeletons.clear();
  morphTargetManagers.clear();
  _transientComponents.clear();
  _isReadyForMeshStage.clear();
  _beforeEvaluateActiveMeshStage.clear();
  _evaluateSubMeshStage.clear();
  _activeMeshStage.clear();
  _cameraDrawRenderTargetStage.clear();
  _beforeCameraDrawStage.clear();
  _beforeRenderTargetDrawStage.clear();
  _beforeRenderingGroupDrawStage.clear();
  _beforeRenderingMeshStage.clear();
  _afterRenderingMeshStage.clear();
  _afterRenderingGroupDrawStage.clear();
  _afterCameraDrawStage.clear();
  _afterRenderTargetDrawStage.clear();
  _afterRenderStage.clear();
  _beforeCameraUpdateStage.clear();
  _beforeClearStage.clear();
  _gatherRenderTargetsStage.clear();
  _gatherActiveCameraRenderTargetsStage.clear();
  _pointerMoveStage.clear();
  _pointerDownStage.clear();
  _pointerUpStage.clear();

  for (const auto& component : _components) {
    component->dispose();
  }

  importedMeshesFiles.clear();

  stopAllAnimations();

  resetCachedMaterial();

  // Smart arrays
  if (_activeCamera) {
    _activeCamera->_activeMeshes.clear();
    activeCamera = nullptr;
  }
  _activeMeshes.clear();
  _renderingManager->dispose();
  _processedMaterials.clear();
  _activeParticleSystems.clear();
  _activeSkeletons.clear();
  _softwareSkinnedMeshes.clear();
  _renderTargets.clear();
  _registeredForLateAnimationBindings.clear();
  _meshesForIntersections.clear();
  _toBeDisposed.clear();

  // Abort active requests
  for (const auto& request : _activeRequests) {
    if (request.abort) {
      request.abort();
    }
  }

  // Events
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBeforeRenderObservable.clear();
  onAfterRenderObservable.clear();
  onBeforeRenderTargetsRenderObservable.clear();
  onAfterRenderTargetsRenderObservable.clear();
  onAfterStepObservable.clear();
  onBeforeStepObservable.clear();
  onBeforeActiveMeshesEvaluationObservable.clear();
  onAfterActiveMeshesEvaluationObservable.clear();
  onBeforeParticlesRenderingObservable.clear();
  onAfterParticlesRenderingObservable.clear();
  onBeforeDrawPhaseObservable.clear();
  onAfterDrawPhaseObservable.clear();
  onBeforeAnimationsObservable.clear();
  onAfterAnimationsObservable.clear();
  onDataLoadedObservable.clear();
  onBeforeRenderingGroupObservable.clear();
  onAfterRenderingGroupObservable.clear();
  onMeshImportedObservable.clear();
  onBeforeCameraRenderObservable.clear();
  onAfterCameraRenderObservable.clear();
  onReadyObservable.clear();
  onNewCameraAddedObservable.clear();
  onCameraRemovedObservable.clear();
  onNewLightAddedObservable.clear();
  onLightRemovedObservable.clear();
  onNewGeometryAddedObservable.clear();
  onGeometryRemovedObservable.clear();
  onNewTransformNodeAddedObservable.clear();
  onTransformNodeRemovedObservable.clear();
  onNewMeshAddedObservable.clear();
  onMeshRemovedObservable.clear();
  onNewSkeletonAddedObservable.clear();
  onSkeletonRemovedObservable.clear();
  onNewMaterialAddedObservable.clear();
  onMaterialRemovedObservable.clear();
  onNewTextureAddedObservable.clear();
  onTextureRemovedObservable.clear();
  onPrePointerObservable.clear();
  onPointerObservable.clear();
  onPreKeyboardObservable.clear();
  onKeyboardObservable.clear();
  onActiveCameraChanged.clear();

  detachControl();

  // Detach cameras
  auto canvas = _engine->getRenderingCanvas();
  if (canvas) {
    for (const auto& camera : cameras) {
      camera->detachControl(canvas);
    }
  }

  // Release animation groups
  for (const auto& animationGroup : animationGroups) {
    animationGroup->dispose();
  }

  // Release lights
  for (const auto& light : lights) {
    light->dispose();
  }

  // Release meshes
  for (const auto& mesh : meshes) {
    mesh->dispose(true);
  }

  // Release transform nodes
  for (const auto& transformNode : transformNodes) {
    transformNode->dispose();
  }

  // Release cameras
  for (const auto& camera : cameras) {
    camera->dispose();
  }

  // Release materials
  if (_defaultMaterial) {
    _defaultMaterial->dispose();
  }
  for (const auto& multiMaterial : multiMaterials) {
    multiMaterial->dispose();
  }
  for (const auto& material : materials) {
    material->dispose();
  }

  // Release particles
  for (const auto& particleSystem : particleSystems) {
    particleSystem->dispose();
  }

  // Release postProcesses
  for (const auto& postProcess : postProcesses) {
    postProcess->dispose();
  }

  // Release textures
  for (const auto& texture : textures) {
    texture->dispose();
  }

  // Release UBO
  _sceneUbo->dispose();

  if (_multiviewSceneUbo) {
    _multiviewSceneUbo->dispose();
  }

  // Post-processes
  postProcessManager->dispose();

  // Remove from engine
  _engine->scenes.erase(
    std::remove(_engine->scenes.begin(), _engine->scenes.end(), this),
    _engine->scenes.end());

  _engine->wipeCaches(true);
  _isDisposed = true;
}

bool Scene::get_isDisposed() const
{
  return _isDisposed;
}

bool Scene::get_blockMaterialDirtyMechanism() const
{
  return _blockMaterialDirtyMechanism;
}

void Scene::set_blockMaterialDirtyMechanism(bool value)
{
  if (_blockMaterialDirtyMechanism == value) {
    return;
  }

  _blockMaterialDirtyMechanism = value;

  if (!value) { // Do a complete update
    markAllMaterialsAsDirty(Constants::MATERIAL_AllDirtyFlag);
  }
}

void Scene::clearCachedVertexData()
{
  for (const auto& abstractmesh : meshes) {
    auto mesh = static_cast<Mesh*>(abstractmesh.get());
    if (mesh) {
      auto geometry = mesh->geometry();

      if (geometry) {
        geometry->_indices.clear();

        for (const auto& vb : geometry->_vertexBuffers) {
          if (!stl_util::contains(geometry->_vertexBuffers, vb.first)) {
            continue;
          }
          geometry->_vertexBuffers[vb.first]->_getBuffer()->_data.clear();
        }
      }
    }
  }
}

void Scene::cleanCachedTextureBuffer()
{
  for (const auto& baseTexture : textures) {
    auto texture = static_cast<Texture*>(baseTexture.get());
    if (texture) {
      auto buffer = texture->_buffer;

      if (buffer) {
        texture->_buffer = std::nullopt;
      }
    }
  }
}

/** Octrees **/
MinMax Scene::getWorldExtends(
  const std::function<bool(const AbstractMeshPtr& mesh)>& filterPredicate)
{
  Vector3 min(std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max());
  Vector3 max(std::numeric_limits<float>::lowest(),
              std::numeric_limits<float>::lowest(),
              std::numeric_limits<float>::lowest());

  std::vector<AbstractMeshPtr> filteredMeshes;
  if (filterPredicate) {
    for (const auto& mesh : meshes) {
      if (filterPredicate(mesh)) {
        filteredMeshes.emplace_back(mesh);
      }
    }
  }
  else {
    filteredMeshes = meshes;
  }

  for (const auto& mesh : filteredMeshes) {
    if (mesh->subMeshes.empty() || mesh->infiniteDistance) {
      continue;
    }
    mesh->computeWorldMatrix(true);
    const auto& boundingInfo = *mesh->getBoundingInfo();

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
  auto component = _getComponent(SceneComponentConstants::NAME_OCTREE);
  if (!component) {
    component = OctreeSceneComponent::New(this);
    _addComponent(component);
  }

  if (!_selectionOctree) {
    _selectionOctree = new Octree<AbstractMesh*>(
      [](AbstractMesh*& entry, OctreeBlock<AbstractMesh*>& block) {
        Octree<AbstractMesh*>::CreationFuncForMeshes(entry, block);
      },
      maxCapacity, maxDepth);
  }

  auto worldExtends = getWorldExtends();

  // Update octree
  auto rawMeshPtrs = stl_util::to_raw_ptr_vector(meshes);
  _selectionOctree->update(worldExtends.min, worldExtends.max, rawMeshPtrs);

  return _selectionOctree;
}

/** Picking **/
Ray Scene::createPickingRay(int x, int y, Matrix& world,
                            const CameraPtr& camera, bool cameraViewSpace)
{
  auto result = Ray::Zero();

  createPickingRayToRef(x, y, world, result, camera, cameraViewSpace);

  return result;
}

Scene& Scene::createPickingRayToRef(int x, int y,
                                    const std::optional<Matrix>& world,
                                    Ray& result, CameraPtr camera,
                                    bool cameraViewSpace)
{
  auto engine = _engine;

  if (!camera) {
    if (!_activeCamera) {
      BABYLON_LOG_ERROR("Scene", "Active camera not set")
      return *this;
    }

    camera = activeCamera;
  }

  auto& cameraViewport = camera->viewport;
  auto viewport        = cameraViewport.toGlobal(engine->getRenderWidth(),
                                          engine->getRenderHeight());

  auto identity = Matrix::Identity();

  // Moving coordinates to local viewport world
  auto _x = static_cast<float>(x);
  _x /= static_cast<float>(_engine->getHardwareScalingLevel() - viewport.x);
  auto _y = static_cast<float>(y);
  _y /= static_cast<float>(
    _engine->getHardwareScalingLevel()
    - (_engine->getRenderHeight() - viewport.y - viewport.height));

  auto _world = world.has_value() ? *world : identity;
  result.update(_x, _y, static_cast<float>(viewport.width),
                static_cast<float>(viewport.height), _world,
                cameraViewSpace ? identity : camera->getViewMatrix(),
                camera->getProjectionMatrix());
  return *this;
}

Ray Scene::createPickingRayInCameraSpace(int x, int y, const CameraPtr& camera)
{
  auto result = Ray::Zero();

  createPickingRayInCameraSpaceToRef(x, y, result, camera);

  return result;
}

Scene& Scene::createPickingRayInCameraSpaceToRef(int x, int y, Ray& result,
                                                 CameraPtr camera)
{
  auto engine = _engine;

  if (!camera) {
    if (!_activeCamera) {
      BABYLON_LOG_ERROR("Scene", "Active camera not set")
      return *this;
    }

    camera = activeCamera;
  }

  auto& cameraViewport = camera->viewport;
  auto viewport        = cameraViewport.toGlobal(engine->getRenderWidth(),
                                          engine->getRenderHeight());
  auto identity        = Matrix::Identity();

  // Moving coordinates to local viewport world
  auto _x = static_cast<float>(x);
  _x /= static_cast<float>(_engine->getHardwareScalingLevel() - viewport.x);
  auto _y = static_cast<float>(y);
  _y /= static_cast<float>(
    _engine->getHardwareScalingLevel()
    - (_engine->getRenderHeight() - viewport.y - viewport.height));
  result.update(_x, _y, static_cast<float>(viewport.width),
                static_cast<float>(viewport.height), identity, identity,
                camera->getProjectionMatrix());
  return *this;
}

std::optional<PickingInfo> Scene::_internalPick(
  const std::function<Ray(Matrix& world)>& rayFunction,
  const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
  bool fastCheck, const TrianglePickingPredicate& trianglePredicate)
{
  std::optional<PickingInfo> pickingInfo = std::nullopt;

  for (const auto& mesh : meshes) {
    if (predicate) {
      if (!predicate(mesh)) {
        continue;
      }
    }
    else if (!mesh->isEnabled() || !mesh->isVisible || !mesh->isPickable) {
      continue;
    }

    auto world = mesh->getWorldMatrix();
    auto ray   = rayFunction(world);

    auto result = mesh->intersects(ray, fastCheck, trianglePredicate);
    if (/*!result || */ !result.hit) {
      continue;
    }

    if (!fastCheck && pickingInfo != std::nullopt
        && result.distance >= (*pickingInfo).distance) {
      continue;
    }

    pickingInfo = result;

    if (fastCheck) {
      break;
    }
  }

  return pickingInfo ? pickingInfo : PickingInfo();
}

std::vector<std::optional<PickingInfo>> Scene::_internalMultiPick(
  const std::function<Ray(Matrix& world)>& rayFunction,
  const std::function<bool(AbstractMesh* mesh)>& predicate,
  const TrianglePickingPredicate& trianglePredicate)
{
  std::vector<std::optional<PickingInfo>> pickingInfos;

  for (const auto& mesh : meshes) {
    if (predicate) {
      if (!predicate(mesh.get())) {
        continue;
      }
    }
    else if (!mesh->isEnabled() || !mesh->isVisible || !mesh->isPickable) {
      continue;
    }

    auto world = mesh->getWorldMatrix();
    auto ray   = rayFunction(world);

    auto result = mesh->intersects(ray, false, trianglePredicate);
    if (/*!result || */ !result.hit) {
      continue;
    }

    pickingInfos.emplace_back(result);
  }

  return pickingInfos;
}

std::optional<PickingInfo> Scene::_internalPickSprites(
  const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate,
  bool fastCheck, CameraPtr camera)
{
  std::optional<PickingInfo> pickingInfo = std::nullopt;

  if (!camera) {
    if (!_activeCamera) {
      return std::nullopt;
    }
    camera = activeCamera;
  }

  if (spriteManagers.size() > 0) {
    for (const auto& spriteManager : spriteManagers) {
      if (!spriteManager->isPickable) {
        continue;
      }

      auto result
        = spriteManager->intersects(ray, camera, predicate, fastCheck);
      if (!result || !result->hit) {
        continue;
      }

      if (!fastCheck && (pickingInfo.has_value())
          && (result->distance >= (*pickingInfo).distance)) {
        continue;
      }

      pickingInfo = *result;

      if (fastCheck) {
        break;
      }
    }
  }

  return pickingInfo ? pickingInfo : PickingInfo();
}

std::optional<PickingInfo>
Scene::pick(int x, int y,
            const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
            bool fastCheck, const CameraPtr& camera,
            const TrianglePickingPredicate& trianglePredicate)
{
  auto result = _internalPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      createPickingRayToRef(x, y, world, *_tempPickingRay, camera);
      return *_tempPickingRay;
    },
    predicate, fastCheck, trianglePredicate);
  if (result) {
    auto _result     = *result;
    auto identityMat = Matrix::Identity();
    _result.ray
      = createPickingRay(x, y, identityMat, camera ? camera : nullptr);
    result = _result;
  }
  return result;
}

std::optional<PickingInfo>
Scene::pickSprite(int x, int y,
                  const std::function<bool(Sprite* sprite)>& predicate,
                  bool fastCheck, const CameraPtr& camera)
{
  createPickingRayInCameraSpaceToRef(x, y, *_tempPickingRay, camera);

  return _internalPickSprites(*_tempPickingRay, predicate, fastCheck, camera);
}

std::optional<PickingInfo>
Scene::pickSpriteWithRay(const Ray& ray,
                         const std::function<bool(Sprite* sprite)>& predicate,
                         bool fastCheck, CameraPtr& camera)
{
  if (!_tempSpritePickingRay) {
    return std::nullopt;
  }

  if (!camera) {
    if (!_activeCamera) {
      return std::nullopt;
    }
    camera = activeCamera;
  }

  Ray::TransformToRef(ray, camera->getViewMatrix(), *_tempSpritePickingRay);

  return _internalPickSprites(*_tempSpritePickingRay, predicate, fastCheck,
                              camera);
}

std::optional<PickingInfo> Scene::pickWithRay(
  const Ray& ray,
  const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
  bool fastCheck, const TrianglePickingPredicate& trianglePredicate)
{
  auto result = _internalPick(
    [this, &ray](Matrix& world) -> Ray {
      if (!_pickWithRayInverseMatrix) {
        _pickWithRayInverseMatrix
          = std::make_unique<Matrix>(Matrix::Identity());
      }
      world.invertToRef(*_pickWithRayInverseMatrix);

      if (!_cachedRayForTransform) {
        _cachedRayForTransform = std::make_unique<Ray>(Ray::Zero());
      }

      Ray::TransformToRef(ray, *_pickWithRayInverseMatrix,
                          *_cachedRayForTransform);
      return *_cachedRayForTransform;
    },
    predicate, fastCheck, trianglePredicate);
  if (result) {
    auto _result = *result;
    _result.ray  = ray;
    result       = _result;
  }
  return result;
}

std::vector<std::optional<PickingInfo>> Scene::multiPick(
  int x, int y, const std::function<bool(AbstractMesh* mesh)>& predicate,
  const CameraPtr& camera, const TrianglePickingPredicate& trianglePredicate)
{
  return _internalMultiPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      return createPickingRay(x, y, world, camera);
    },
    predicate, trianglePredicate);
}

std::vector<std::optional<PickingInfo>> Scene::multiPickWithRay(
  const Ray& ray, const std::function<bool(AbstractMesh* mesh)>& predicate,
  const TrianglePickingPredicate& trianglePredicate)
{
  return _internalMultiPick(
    [this, &ray](Matrix& world) -> Ray {
      if (!_pickWithRayInverseMatrix) {
        _pickWithRayInverseMatrix
          = std::make_unique<Matrix>(Matrix::Identity());
      }
      world.invertToRef(*_pickWithRayInverseMatrix);

      if (!_cachedRayForTransform) {
        _cachedRayForTransform = std::make_unique<Ray>(Ray::Zero());
      }

      Ray::TransformToRef(ray, *_pickWithRayInverseMatrix,
                          *_cachedRayForTransform);
      return *_cachedRayForTransform;
    },
    predicate, trianglePredicate);
}

AbstractMeshPtr& Scene::getPointerOverMesh()
{
  return _inputManager->getPointerOverMesh();
}

void Scene::setPointerOverMesh(const AbstractMeshPtr& mesh)
{
  _inputManager->setPointerOverMesh(mesh);
}

void Scene::setPointerOverSprite(const SpritePtr& sprite)
{
  if (_pointerOverSprite == sprite) {
    return;
  }

  if (_pointerOverSprite && _pointerOverSprite->actionManager) {
    Event evt;
    _pointerOverSprite->actionManager->processTrigger(
      Constants::ACTION_OnPointerOutTrigger,
      ActionEvent::CreateNewFromSprite(_pointerOverSprite, this, evt));
  }

  _pointerOverSprite = sprite;
  if (_pointerOverSprite && _pointerOverSprite->actionManager) {
    Event evt;
    _pointerOverSprite->actionManager->processTrigger(
      Constants::ACTION_OnPointerOverTrigger,
      ActionEvent::CreateNewFromSprite(_pointerOverSprite, this, evt));
  }
}

SpritePtr& Scene::getPointerOverSprite()
{
  return _pointerOverSprite;
}

/** Physics **/
IPhysicsEnginePtr& Scene::getPhysicsEngine()
{
  return _physicsEngine;
}

bool Scene::enablePhysics(const std::optional<Vector3>& iGravity,
                          IPhysicsEnginePlugin* plugin)
{
  if (_physicsEngine) {
    return true;
  }

  // Register the component to the scene
  auto component = _getComponent(SceneComponentConstants::NAME_PHYSICSENGINE);
  if (!component) {
    component = PhysicsEngineSceneComponent::New(this);
    _addComponent(component);
  }

  _physicsEngine     = PhysicsEngine::New(iGravity, plugin);
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

void Scene::_advancePhysicsEngineStep(float step)
{
  if (_physicsEngine) {
    onBeforePhysicsObservable.notifyObservers(this);
    _physicsEngine->_step(step / 1000.f);
    onAfterPhysicsObservable.notifyObservers(this);
  }
}

void Scene::_rebuildGeometries()
{
  for (const auto& geometry : geometries) {
    geometry->_rebuild();
  }

  for (const auto& mesh : meshes) {
    mesh->_rebuild();
  }

  if (postProcessManager) {
    postProcessManager->_rebuild();
  }

  for (const auto& component : _components) {
    component->rebuild();
  }

  for (const auto& system : particleSystems) {
    system->rebuild();
  }
}
void Scene::_rebuildTextures()
{
  for (const auto& texture : textures) {
    texture->_rebuild();
  }

  markAllMaterialsAsDirty(Constants::MATERIAL_TextureDirtyFlag);
}

void Scene::createDefaultLight(bool replace)
{
  // Dispose existing light in replace mode.
  if (replace) {
    if (!lights.empty()) {
      for (const auto& light : lights) {
        light->dispose();
      }
    }
  }

  // Light
  if (lights.empty()) {
    HemisphericLight::New("default light", Vector3::Up(), this);
  }
}

void Scene::createDefaultCamera(bool createArcRotateCamera, bool replace,
                                bool attachCameraControls)
{
  // Dispose existing camera in replace mode.
  if (replace) {
    if (_activeCamera) {
      _activeCamera->dispose();
      activeCamera = nullptr;
    }
  }

  // Camera
  if (!_activeCamera) {
    auto worldExtends = getWorldExtends();
    auto worldSize    = worldExtends.max.subtract(worldExtends.min);
    auto worldCenter  = worldExtends.min.add(worldSize.scale(0.5f));

    TargetCameraPtr camera = nullptr;
    auto radius            = worldSize.length() * 1.5f;
    // empty scene scenario!
    if (std::isinf(radius)) {
      radius = 1.f;
      worldCenter.copyFromFloats(0.f, 0.f, 0.f);
    }
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
    camera->maxZ  = radius * 1000.f;
    camera->speed = radius * 0.2f;
    activeCamera  = camera;

    auto canvas = getEngine()->getRenderingCanvas();
    if (attachCameraControls && canvas) {
      camera->attachControl(canvas);
    }
  }
}

void Scene::createDefaultCameraOrLight(bool createArcRotateCamera, bool replace,
                                       bool attachCameraControls)
{
  createDefaultLight(replace);
  createDefaultCamera(createArcRotateCamera, replace, attachCameraControls);
}

MeshPtr Scene::createDefaultSkybox(BaseTexturePtr iEnvironmentTexture, bool pbr,
                                   float scale, float blur,
                                   bool setGlobalEnvTexture)
{
  if (!environmentTexture()) {
    BABYLON_LOG_WARN(
      "Scene", "Can not create default skybox without environment texture.")
    return nullptr;
  }

  if (setGlobalEnvTexture) {
    if (iEnvironmentTexture) {
      environmentTexture = iEnvironmentTexture;
    }
  }

  // Skybox
  auto hdrSkybox = Mesh::CreateBox("hdrSkyBox", scale, this);
  if (pbr) {
    auto hdrSkyboxMaterial               = PBRMaterial::New("skyBox", this);
    hdrSkyboxMaterial->backFaceCulling   = false;
    hdrSkyboxMaterial->reflectionTexture = environmentTexture();
    if (hdrSkyboxMaterial->reflectionTexture()) {
      hdrSkyboxMaterial->reflectionTexture()->coordinatesMode
        = TextureConstants::SKYBOX_MODE;
    }
    hdrSkyboxMaterial->microSurface     = 1.f - blur;
    hdrSkyboxMaterial->disableLighting  = true;
    hdrSkyboxMaterial->twoSidedLighting = true;
    hdrSkybox->infiniteDistance         = true;
    hdrSkybox->material                 = hdrSkyboxMaterial;
  }
  else {
    auto skyboxMaterial               = StandardMaterial::New("skyBox", this);
    skyboxMaterial->backFaceCulling   = false;
    skyboxMaterial->reflectionTexture = environmentTexture();
    if (skyboxMaterial->reflectionTexture()) {
      skyboxMaterial->reflectionTexture()->coordinatesMode
        = TextureConstants::SKYBOX_MODE;
    }
    skyboxMaterial->disableLighting = true;
    hdrSkybox->infiniteDistance     = true;
    hdrSkybox->material             = skyboxMaterial;
  }

  hdrSkybox->isPickable = false;
  return hdrSkybox;
}

std::unique_ptr<EnvironmentHelper> Scene::createDefaultEnvironment(
  const std::optional<IEnvironmentHelperOptions>& options)
{
  return std::make_unique<EnvironmentHelper>(options, this);
}

// Tags

std::vector<std::string> Scene::_getByTags()
{
  return std::vector<std::string>();
}

std::vector<Mesh*> Scene::getMeshesByTags()
{
  return std::vector<Mesh*>();
}

std::vector<Camera*> Scene::getCamerasByTags()
{
  return std::vector<Camera*>();
}

std::vector<Light*> Scene::getLightsByTags()
{
  return std::vector<Light*>();
}

std::vector<Material*> Scene::getMaterialByTags()
{
  return std::vector<Material*>();
}

void Scene::setRenderingOrder(
  unsigned int renderingGroupId,
  const std::function<int(const SubMesh* a, const SubMesh* b)>&
    opaqueSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>&
    alphaTestSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>&
    transparentSortCompareFn)
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

std::optional<IRenderingManagerAutoClearSetup>
Scene::getAutoClearDepthStencilSetup(size_t index)
{
  return _renderingManager->getAutoClearDepthStencilSetup(index);
}

void Scene::markAllMaterialsAsDirty(
  unsigned int flag, const std::function<bool(Material* mat)>& predicate)
{
  if (_blockMaterialDirtyMechanism) {
    return;
  }

  for (const auto& material : materials) {
    if (predicate && !predicate(material.get())) {
      continue;
    }
    if (material) {
      material->markAsDirty(flag);
    }
  }
}

IFileRequest
Scene::_loadFile(const std::string& /*url*/,
                 const std::function<
                   void(const std::variant<std::string, ArrayBuffer>& /*data*/,
                        const std::string&)>& /*onSuccess*/)
{
  return IFileRequest();
}

std::variant<std::string, ArrayBuffer>
Scene::_loadFileAsync(const std::string& /*url*/,
                      const std::optional<bool>& /*useDatabase*/,
                      const std::optional<bool>& /*useArrayBuffer*/)
{
  return ArrayBuffer();
}

void Scene::_createMultiviewUbo()
{
  _multiviewSceneUbo
    = std::make_unique<UniformBuffer>(getEngine(), Float32Array(), true);
  _multiviewSceneUbo->addUniform("viewProjection", 16);
  _multiviewSceneUbo->addUniform("viewProjectionR", 16);
  _multiviewSceneUbo->addUniform("view", 16);
}

void Scene::_updateMultiviewUbo(std::optional<Matrix> viewR,
                                std::optional<Matrix> projectionR)
{
  if (viewR && projectionR) {
    viewR->multiplyToRef(*projectionR, _transformMatrixR);
  }

  if (viewR && projectionR) {
    viewR->multiplyToRef(*projectionR, Tmp::MatrixArray[0]);
    Frustum::GetRightPlaneToRef(
      Tmp::MatrixArray[0],
      _frustumPlanes[3]); // Replace right plane by second camera right plane
  }

  if (_multiviewSceneUbo) {
    _multiviewSceneUbo->updateMatrix("viewProjection", getTransformMatrix());
    _multiviewSceneUbo->updateMatrix("viewProjectionR", _transformMatrixR);
    _multiviewSceneUbo->updateMatrix("view", _viewMatrix);
    _multiviewSceneUbo->update();
  }
}

void Scene::_renderMultiviewToSingleView(const CameraPtr& camera)
{
  // Multiview is only able to be displayed directly for API's such as webXR
  // This displays a multiview image by rendering to the multiview image and
  // then copying the result into the sub cameras instead of rendering them and
  // proceeding as normal from there

  // Render to a multiview texture
  camera->_resizeOrCreateMultiviewTexture(
    (camera->_rigPostProcess && camera->_rigPostProcess
     && camera->_rigPostProcess->width > 0) ?
      camera->_rigPostProcess->width / 2 :
      getEngine()->getRenderWidth(true) / 2,
    (camera->_rigPostProcess && camera->_rigPostProcess
     && camera->_rigPostProcess->height > 0) ?
      camera->_rigPostProcess->height :
      getEngine()->getRenderHeight(true));
  if (!_multiviewSceneUbo) {
    _createMultiviewUbo();
  }
  camera->outputRenderTarget = camera->_multiviewTexture;
  _renderForCamera(camera);
  camera->outputRenderTarget = nullptr;

  // Consume the multiview texture through a shader for each eye
  auto engine = getEngine();
  for (size_t index = 0; index < camera->_rigCameras.size(); index++) {
    activeCamera = camera->_rigCameras[index];
    engine->setViewport(_activeCamera->viewport);
    if (postProcessManager) {
      postProcessManager->_prepareFrame();
      postProcessManager->_finalizeFrame(_activeCamera->isIntermediate);
    }
  }
}

} // end of namespace BABYLON
