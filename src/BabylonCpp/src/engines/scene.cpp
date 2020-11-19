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
#include <babylon/engines/engine_store.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/iscene_serializable_component.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info_pre.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/gamepads/gamepad_manager.h>
#include <babylon/gamepads/gamepad_system_scene_component.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/inputs/click_info.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/layers/effect_layer.h>
#include <babylon/layers/glow_layer.h>
#include <babylon/layers/highlight_layer.h>
#include <babylon/layers/layer.h>
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
#include <babylon/maths/frustum.h>
#include <babylon/maths/tmp_vectors.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/buffer.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/mesh_simplification_scene_component.h>
#include <babylon/meshes/simplification/simplification_queue.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/guid.h>
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
#include <babylon/rendering/pre_pass_renderer.h>
#include <babylon/rendering/rendering_manager.h>
#include <babylon/rendering/sub_surface_configuration.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

size_t Scene::_uniqueIdCounter = 0;

microseconds_t Scene::MinDeltaTime = std::chrono::milliseconds(1);
microseconds_t Scene::MaxDeltaTime = std::chrono::milliseconds(1000);

unsigned int Scene::DragMovementThreshold = 10;
milliseconds_t Scene::LongPressDelay      = std::chrono::milliseconds(500);
milliseconds_t Scene::DoubleClickDelay    = std::chrono::milliseconds(300);
bool Scene::ExclusiveDoubleClickMode      = false;

Scene::Scene(Engine* engine, const std::optional<SceneOptions>& options)
    : _blockEntityCollection{false}
    , autoClear{true}
    , autoClearDepthAndStencil{true}
    , clearColor{Color4(0.2f, 0.2f, 0.3f, 1.f)}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , environmentBRDFTexture{nullptr}
    , environmentIntensity{this, &Scene::get_environmentIntensity, &Scene::set_environmentIntensity}
    , imageProcessingConfiguration{this, &Scene::get_imageProcessingConfiguration}
    , onDispose{this, &Scene::set_onDispose}
    , beforeRender{this, &Scene::set_beforeRender}
    , afterRender{this, &Scene::set_afterRender}
    , beforeCameraRender{this, &Scene::set_beforeCameraRender}
    , afterCameraRender{this, &Scene::set_afterCameraRender}
    , pointerDownPredicate{nullptr}
    , pointerUpPredicate{nullptr}
    , pointerMovePredicate{nullptr}
    , forceWireframe{this, &Scene::get_forceWireframe, &Scene::set_forceWireframe}
    , skipFrustumClipping{this, &Scene::get_skipFrustumClipping, &Scene::set_skipFrustumClipping}
    , forcePointsCloud{this, &Scene::get_forcePointsCloud, &Scene::set_forcePointsCloud}
    , forceShowBoundingBoxes{this, &Scene::get_forceShowBoundingBoxes,
                             &Scene::set_forceShowBoundingBoxes}
    , animationsEnabled{true}
    , useConstantAnimationDeltaTime{false}
    , constantlyUpdateMeshUnderPointer{false}
    , hoverCursor{"pointer"}
    , defaultCursor{""}
    , doNotHandleCursors{false}
    , preventDefaultOnPointerDown{true}
    , preventDefaultOnPointerUp{true}
    , gamepadManager{this, &Scene::get_gamepadManager}
    , unTranslatedPointer{this, &Scene::get_unTranslatedPointer}
    , cameraToUseForPointers(nullptr)
    , _mirroredCameraPosition{nullptr}
    , useRightHandedSystem{this, &Scene::get_useRightHandedSystem, &Scene::set_useRightHandedSystem}
    , fogEnabled{this, &Scene::get_fogEnabled, &Scene::set_fogEnabled}
    , fogMode{this, &Scene::get_fogMode, &Scene::set_fogMode}
    , fogColor{Color3(0.2f, 0.2f, 0.3f)}
    , fogDensity{0.1f}
    , fogStart{0.f}
    , fogEnd{1000.f}
    , prePass{false}
    , shadowsEnabled{this, &Scene::get_shadowsEnabled, &Scene::set_shadowsEnabled}
    , lightsEnabled{this, &Scene::get_lightsEnabled, &Scene::set_lightsEnabled}
    , _activeCamera{nullptr}
    , activeCamera{this, &Scene::get_activeCamera, &Scene::set_activeCamera}
    , defaultMaterial{this, &Scene::get_defaultMaterial, &Scene::set_defaultMaterial}
    , texturesEnabled{this, &Scene::get_texturesEnabled, &Scene::set_texturesEnabled}
    , physicsEnabled{true}
    , particlesEnabled{true}
    , spritesEnabled{true}
    , _pointerOverSprite{nullptr}
    , _pickedDownSprite{nullptr}
    , _tempSpritePickingRay{std::nullopt}
    , skeletonsEnabled{this, &Scene::get_skeletonsEnabled, &Scene::set_skeletonsEnabled}
    , lensFlaresEnabled{true}
    , collisionsEnabled{true}
    , collisionCoordinator{this, &Scene::get_collisionCoordinator}
    , gravity{Vector3(0.f, -9.807f, 0.f)}
    , postProcessesEnabled{true}
    , postProcessManager{nullptr}
    , postProcessRenderPipelineManager{this, &Scene::get_postProcessRenderPipelineManager}
    , renderTargetsEnabled{true}
    , dumpNextRenderTargets{false}
    , useDelayedTextureLoading{false}
    , probesEnabled{true}
    , actionManager{nullptr}
    , proceduralTexturesEnabled{true}
    , mainSoundTrack{this, &Scene::get_mainSoundTrack}
    , simplificationQueue{this, &Scene::get_simplificationQueue, &Scene::set_simplificationQueue}
    , animationTimeScale{1}
    , _cachedMaterial{nullptr}
    , _cachedEffect{nullptr}
    , _cachedVisibility{0.f}
    , dispatchAllSubMeshesOfActiveMeshes{false}
    , _forcedViewPosition{nullptr}
    , _isAlternateRenderingEnabled{this, &Scene::get_isAlternateRenderingEnabled}
    , frustumPlanes{this, &Scene::get_frustumPlanes}
    , requireLightSorting{false}
    , useMaterialMeshMap{false}
    , useClonedMeshMap{false}
    , depthRenderer{this, &Scene::get_depthRenderer}
    , geometryBufferRenderer{this, &Scene::get_geometryBufferRenderer,
                             &Scene::set_geometryBufferRenderer}
    , prePassRenderer{this, &Scene::get_prePassRenderer, &Scene::set_prePassRenderer}
    , _edgeRenderLineShader{nullptr}
    , debugLayer{this, &Scene::get_debugLayer}
    , selectionOctree{this, &Scene::get_selectionOctree}
    , meshUnderPointer{this, &Scene::get_meshUnderPointer}
    , pointerX{this, &Scene::get_pointerX, &Scene::set_pointerX}
    , pointerY{this, &Scene::get_pointerY, &Scene::set_pointerY}
    , totalVerticesPerfCounter{this, &Scene::get_totalVerticesPerfCounter}
    , totalActiveIndicesPerfCounter{this, &Scene::get_totalActiveIndicesPerfCounter}
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
    , _activeMeshesFrozen{false}
    , _physicsEngine{nullptr}
    , _physicsTimeAccumulator{0.f}
    , _transformMatrixR{Matrix::Zero()}
    , _environmentIntensity{1.f}
    , _animationPropertiesOverride{nullptr}
    , _spritePredicate{nullptr}
    , _onDisposeObserver{nullptr}
    , _onBeforeRenderObserver{nullptr}
    , _onAfterRenderObserver{nullptr}
    , _onBeforeCameraRenderObserver{nullptr}
    , _onAfterCameraRenderObserver{nullptr}
    , _onPointerMove{nullptr}
    , _onPointerDown{nullptr}
    , _onPointerUp{nullptr}
    , _initClickEvent{nullptr}
    , _initActionManager{nullptr}
    , _delayedSimpleClick{nullptr}
    , _meshPickProceed{false}
    , _previousHasSwiped{false}
    , _currentPickResult{std::nullopt}
    , _previousPickResult{std::nullopt}
    , _totalPointersPressed{0}
    , _doubleClickOccured{false}
    , _pointerX{0}
    , _pointerY{0}
    , _startingPointerPosition{Vector2(0.f, 0.f)}
    , _previousStartingPointerPosition{Vector2(0.f, 0.f)}
    , _startingPointerTime{high_res_time_point_t()}
    , _previousStartingPointerTime{high_res_time_point_t()}
    , _timeAccumulator{0}
    , _currentStepId{0}
    , _currentInternalStep{0}
    , _onKeyDown{nullptr}
    , _onKeyUp{nullptr}
    , _onCanvasFocusObserver{nullptr}
    , _onCanvasBlurObserver{nullptr}
    , _useRightHandedSystem{false}
    , _forceWireframe{false}
    , _skipFrustumClipping{false}
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
    , _animationRatio{1.f}
    , _animationTimeLast{std::nullopt}
    , _animationTime(0)
    , _renderId{0}
    , _frameId{0}
    , _executeWhenReadyTimeoutId{-1}
    , _intermediateRendering{false}
    , _viewUpdateFlag{-1}
    , _projectionUpdateFlag{-1}
    , _alternateViewUpdateFlag{-1}
    , _alternateProjectionUpdateFlag{-1}
    , _isDisposed{false}
    , _activeMeshCandidateProvider{nullptr}
    , _skipEvaluateActiveMeshesCompletely{false}
    , _renderingManager{nullptr}
    , _transformMatrix{Matrix::Zero()}
    , _sceneUbo{nullptr}
    , _alternateSceneUbo{nullptr}
    , _pickWithRayInverseMatrix{nullptr}
    , _simplificationQueue{nullptr}
    , _boundingBoxRenderer{nullptr}
    , _forceShowBoundingBoxes{false}
    , _outlineRenderer{nullptr}
    , _alternateTransformMatrix{nullptr}
    , _useAlternateCameraConfiguration{false}
    , _alternateRendering{false}
    , _frustumPlanesSet{false}
    , _frustumPlanes{}
    , _selectionOctree{nullptr}
    , _pointerOverMesh{nullptr}
    , _debugLayer{nullptr}
    , _geometryBufferRenderer{nullptr}
    , _pickedDownMesh{nullptr}
    , _pickedUpMesh{nullptr}
    , _uid{GUID::RandomId()}
    , _blockMaterialDirtyMechanism{false}
    , _prePassRenderer{nullptr}
    , _tempPickingRay{std::make_unique<Ray>(Ray::Zero())}
    , _cachedRayForTransform{nullptr}
    , _audioEnabled{std::nullopt}
    , _headphone{std::nullopt}
    , _multiviewSceneUbo{nullptr}
{
  SceneOptions fullOptions;
  fullOptions.useGeometryUniqueIdsMap = true;
  fullOptions.useMaterialMeshMap      = true;
  fullOptions.useClonedMeshMap        = true;
  fullOptions.isVirtual               = false;
  if (options) {
    fullOptions.useGeometryUniqueIdsMap
      = options->useGeometryUniqueIdsMap.value_or(fullOptions.useGeometryUniqueIdsMap.value());
    fullOptions.useMaterialMeshMap
      = options->useMaterialMeshMap.value_or(fullOptions.useMaterialMeshMap.value());
    fullOptions.useClonedMeshMap
      = options->useClonedMeshMap.value_or(fullOptions.useClonedMeshMap.value());
    fullOptions.isVirtual = options->isVirtual.value_or(fullOptions.isVirtual.value());
  }

  _engine = engine ? engine : Engine::LastCreatedEngine();

  if (!fullOptions.isVirtual.value()) {
    EngineStore::_LastCreatedScene = this;
    _engine->scenes.emplace_back(this);
  }

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
    _imageProcessingConfiguration = std::make_shared<ImageProcessingConfiguration>();
  }

  getDeterministicFrameTime = [this]() -> float { return _engine->getTimeStep(); };

  setDefaultCandidateProviders();

  if (fullOptions.useGeometryUniqueIdsMap.value()) {
    geometriesByUniqueId = {};
  }

  useMaterialMeshMap = fullOptions.useMaterialMeshMap.value();
  useClonedMeshMap   = fullOptions.useClonedMeshMap.value();

  if (!options || !options->isVirtual.value_or(false)) {
    _engine->onNewSceneAddedObservable.notifyObservers(this);
  }
}

Scene::~Scene() = default;

void Scene::postInitialize()
{
  environmentTexture = nullptr;
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

  auto serializableComponent = std::static_pointer_cast<ISceneSerializableComponent>(component);
  if (serializableComponent) {
    _serializableComponents.emplace_back(serializableComponent);
  }
}

ISceneComponentPtr Scene::_getComponent(const std::string& name) const
{
  auto it = std::find_if(
    _components.begin(), _components.end(),
    [&name](const ISceneComponentPtr& component) { return component->name == name; });
  return (it == _components.end()) ? nullptr : *it;
}

// Events
void Scene::set_onDispose(const std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

void Scene::set_beforeRender(const std::function<void(Scene* scene, EventState& es)>& callback)
{
  if (_onBeforeRenderObserver) {
    onBeforeRenderObservable.remove(_onBeforeRenderObserver);
  }
  if (callback) {
    _onBeforeRenderObserver = onBeforeRenderObservable.add(callback);
  }
}

void Scene::set_afterRender(const std::function<void(Scene* scene, EventState& es)>& callback)
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
  _unTranslatedPointer
    = Vector2(static_cast<float>(_unTranslatedPointerX), static_cast<float>(_unTranslatedPointerY));

  return _unTranslatedPointer;
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
  markAllMaterialsAsDirty(Material::TextureDirtyFlag);
}

float Scene::get_environmentIntensity() const
{
  return _environmentIntensity;
}

void Scene::set_environmentIntensity(float value)
{
  if (stl_util::almost_equal(_environmentIntensity, value)) {
    return;
  }

  _environmentIntensity = value;
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
}

bool Scene::get_forceWireframe() const
{
  return _forceWireframe;
}

void Scene::set_skipFrustumClipping(bool value)
{
  if (_skipFrustumClipping == value) {
    return;
  }
  _skipFrustumClipping = value;
}

bool Scene::get_skipFrustumClipping() const
{
  return _skipFrustumClipping;
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
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

AnimationPropertiesOverridePtr& Scene::get_animationPropertiesOverride()
{
  return _animationPropertiesOverride;
}

void Scene::set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value)
{
  _animationPropertiesOverride = value;
}

void Scene::set_fogEnabled(bool value)
{
  if (_fogEnabled == value) {
    return;
  }
  _fogEnabled = value;
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag);
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
  markAllMaterialsAsDirty(Material::LightDirtyFlag);
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
  markAllMaterialsAsDirty(Material::LightDirtyFlag);
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
  markAllMaterialsAsDirty(Material::TextureDirtyFlag);
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
  markAllMaterialsAsDirty(Material::AttributesDirtyFlag);
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

std::unique_ptr<PostProcessRenderPipelineManager>& Scene::get_postProcessRenderPipelineManager()
{
  if (!_postProcessRenderPipelineManager) {
    // Register the G Buffer component to the scene.
    auto component = std::static_pointer_cast<PostProcessRenderPipelineManagerSceneComponent>(
      _getComponent(SceneComponentConstants::NAME_POSTPROCESSRENDERPIPELINEMANAGER));
    if (!component) {
      component = PostProcessRenderPipelineManagerSceneComponent::New(this);
      _addComponent(component);
    }
    _postProcessRenderPipelineManager = std::make_unique<PostProcessRenderPipelineManager>();
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
    auto component       = std::static_pointer_cast<SimplicationQueueSceneComponent>(
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

PrePassRendererPtr& Scene::get_prePassRenderer()
{
  return _prePassRenderer;
}

void Scene::set_prePassRenderer(const PrePassRendererPtr& value)
{
  if (value && value->isSupported()) {
    _prePassRenderer = value;
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

bool Scene::get_isAlternateRenderingEnabled() const
{
  return _alternateRendering;
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

  std::for_each(meshes.begin(), meshes.end(),
                [&_meshes](const AbstractMeshPtr& mesh) { _meshes.emplace_back(mesh); });

  return _meshes;
}

Octree<AbstractMesh*>*& Scene::get_selectionOctree()
{
  return _selectionOctree;
}

AbstractMeshPtr& Scene::get_meshUnderPointer()
{
  return _pointerOverMesh;
}

int Scene::get_pointerX() const
{
  return _pointerX;
}

void Scene::set_pointerX(int value)
{
  _pointerX = value;
}

int Scene::get_pointerY() const
{
  return _pointerY;
}

void Scene::set_pointerY(int value)
{
  _pointerY = value;
}

std::string Scene::getClassName() const
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
  getIntersectingSubMeshCandidates = [this](AbstractMesh* mesh, const Ray& /*localRay*/) {
    return _getDefaultSubMeshCandidates(mesh);
  };
  getCollidingSubMeshCandidates = [this](AbstractMesh* mesh, const Collider& /*collider*/) {
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

bool Scene::isCachedMaterialInvalid(Material* material, const EffectPtr& effect, float visibility)
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

void Scene::_updatePointerPosition(const PointerEvent& evt)
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
  _sceneUbo = std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _sceneUbo->addUniform("viewProjection", 16);
  _sceneUbo->addUniform("view", 16);
}

void Scene::_createAlternateUbo()
{
  _alternateSceneUbo = std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _alternateSceneUbo->addUniform("viewProjection", 16);
  _alternateSceneUbo->addUniform("view", 16);
}

std::optional<PickingInfo>
Scene::_pickSpriteButKeepRay(const std::optional<PickingInfo>& originalPointerInfo, int x, int y,
                             const std::function<bool(Sprite* sprite)>& predicate, bool fastCheck,
                             const CameraPtr& camera)
{
  auto result = pickSprite(x, y, predicate, fastCheck, camera);
  if (result) {
    auto _result = *result;
    _result.ray  = originalPointerInfo ? _result.ray : std::nullopt;
    result       = _result;
  }
  return result;
}

void Scene::_setRayOnPointerInfo(PointerInfo& pointerInfo)
{
  /* if (pointerInfo.pickInfo) */ {
    if (!pointerInfo.pickInfo.ray) {
      auto identityMatrix = Matrix::Identity();
      if (pointerInfo.type == PointerEventTypes::POINTERWHEEL) {
        pointerInfo.pickInfo.ray
          = createPickingRay(pointerInfo.mouseWheelEvent.offsetX,
                             pointerInfo.mouseWheelEvent.offsetY, identityMatrix, _activeCamera);
      }
      else if (pointerInfo.type == PointerEventTypes::POINTERMOVE) {
        pointerInfo.pickInfo.ray
          = createPickingRay(pointerInfo.pointerEvent.offsetX, pointerInfo.pointerEvent.offsetY,
                             identityMatrix, _activeCamera);
      }
    }
  }
}

Scene& Scene::simulatePointerMove(std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointermove");

  if (_checkPrePointerObservable(pickResult, evt, PointerEventTypes::POINTERMOVE)) {
    return *this;
  }
  return _processPointerMove(pickResult, evt);
}

Scene& Scene::_processPointerMove(std::optional<PickingInfo>& pickResult, const PointerEvent& evt)
{
  auto canvas = _engine->getRenderingCanvas();

  if (!canvas) {
    return *this;
  }

  // Restore pointer
  canvas->style.cursor = defaultCursor;

  auto isMeshPicked = pickResult && pickResult->hit && pickResult->pickedMesh;
  if (isMeshPicked) {
    setPointerOverMesh(pickResult->pickedMesh.get());

    if (_pointerOverMesh && _pointerOverMesh->actionManager
        && _pointerOverMesh->actionManager->hasPointerTriggers()) {
      if (!_pointerOverMesh->actionManager->hoverCursor.empty()) {
        canvas->style.cursor = _pointerOverMesh->actionManager->hoverCursor;
      }
      else {
        canvas->style.cursor = hoverCursor;
      }
    }
  }
  else {
    setPointerOverMesh(nullptr);
  }

  for (const auto& step : _pointerMoveStage) {
    pickResult
      = step.action(_unTranslatedPointerX, _unTranslatedPointerY, pickResult, isMeshPicked, canvas);
  }

  if (pickResult) {
    auto type = (evt.type == EventType::MOUSE_WHEEL || evt.type == EventType::DOM_MOUSE_SCROLL) ?
                  PointerEventTypes::POINTERWHEEL :
                  PointerEventTypes::POINTERMOVE;

    if (onPointerMove) {
      onPointerMove(evt, pickResult, type);
    }

    if (onPointerObservable.hasObservers()) {
      auto iType = evt.type == EventType::MOUSE_WHEEL || evt.type == EventType::DOM_MOUSE_SCROLL ?
                     PointerEventTypes::POINTERWHEEL :
                     PointerEventTypes::POINTERMOVE;

      if (iType == PointerEventTypes::POINTERWHEEL) {
        PointerInfo pi(iType, *static_cast<MouseWheelEvent const*>(&evt), *pickResult);
        _setRayOnPointerInfo(pi);
        onPointerObservable.notifyObservers(&pi, static_cast<int>(iType));
      }
      else {
        PointerInfo pi(iType, evt, *pickResult);
        _setRayOnPointerInfo(pi);
        onPointerObservable.notifyObservers(&pi, static_cast<int>(iType));
      }
    }
  }

  return *this;
}

bool Scene::_checkPrePointerObservable(const std::optional<PickingInfo>& pickResult,
                                       const PointerEvent& evt, PointerEventTypes type)
{
  PointerInfoPre pi(type, evt, static_cast<float>(_unTranslatedPointerX),
                    static_cast<float>(_unTranslatedPointerY));
  if (pickResult) {
    pi.ray = (*pickResult).ray;
  }
  onPrePointerObservable.notifyObservers(&pi, static_cast<int>(type));
  return pi.skipOnPointerObservable;
}

Scene& Scene::simulatePointerDown(std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointerdown");

  if (_checkPrePointerObservable(pickResult, evt, PointerEventTypes::POINTERDOWN)) {
    return *this;
  }

  return _processPointerDown(pickResult, evt);
}

Scene& Scene::_processPointerDown(std::optional<PickingInfo>& pickResult, const PointerEvent& evt)
{
  if (pickResult && (*pickResult).hit && (*pickResult).pickedMesh) {
    _pickedDownMesh     = (*pickResult).pickedMesh.get();
    auto iActionManager = _pickedDownMesh->actionManager;
    if (iActionManager) {
#if 0
      if (iActionManager->hasPickTriggers()) {
        iActionManager->processTrigger(
          ActionManager::OnPickDownTrigger,
          ActionEvent::CreateNew(_pickedDownMesh, evt));
        switch (evt.button) {
          case MouseButtonType::LEFT:
            iActionManager->processTrigger(
              ActionManager::OnLeftPickTrigger,
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          case MouseButtonType::MIDDLE:
            iActionManager->processTrigger(
              ActionManager::OnCenterPickTrigger,
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          case MouseButtonType::RIGHT:
            iActionManager->processTrigger(
              ActionManager::OnRightPickTrigger,
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          default:
            break;
        }
      }
#endif

      if (iActionManager->hasSpecificTrigger(ActionManager::OnLongPressTrigger)) {
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
                  && !_isPointerSwiping()) {
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
  else {
    for (const auto& step : _pointerDownStage) {
      pickResult = step.action(_unTranslatedPointerX, _unTranslatedPointerY, pickResult, evt);
    }
  }

  if (pickResult) {
    auto type = PointerEventTypes::POINTERDOWN;

    if (onPointerDown) {
      onPointerDown(evt, pickResult, type);
    }

    if (onPointerObservable.hasObservers()) {
      PointerInfo pi(type, evt, *pickResult);
      _setRayOnPointerInfo(pi);
      onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
    }
  }

  return *this;
}

Scene& Scene::simulatePointerUp(std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointerup");
  ClickInfo clickInfo;
  clickInfo.singleClick = true;
  clickInfo.ignore      = true;

  if (_checkPrePointerObservable(pickResult, evt, PointerEventTypes::POINTERUP)) {
    return *this;
  }

  return _processPointerUp(pickResult, evt, clickInfo);
}

Scene& Scene::_processPointerUp(std::optional<PickingInfo>& pickResult, const PointerEvent& evt,
                                const ClickInfo& clickInfo)
{
  if (pickResult && pickResult && (*pickResult).pickedMesh) {
    const auto& _pickResult = *pickResult;
    _pickedUpMesh           = _pickResult.pickedMesh.get();
    if (_pickedDownMesh == _pickedUpMesh) {
      if (onPointerPick) {
        onPointerPick(evt, pickResult);
      }
      if (clickInfo.singleClick() && !clickInfo.ignore() && onPointerObservable.hasObservers()) {
        auto type = PointerEventTypes::POINTERPICK;
        PointerInfo pi(type, evt, *pickResult);
        _setRayOnPointerInfo(pi);
        onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
      }
    }
    if (_pickedUpMesh->actionManager) {
#if 0
      if (clickInfo.ignore()) {
        _pickedUpMesh->actionManager->processTrigger(
          ActionManager::OnPickUpTrigger,
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
      if (!clickInfo.hasSwiped() && !clickInfo.ignore()
          && clickInfo.singleClick()) {
        _pickedUpMesh->actionManager->processTrigger(
          ActionManager::OnPickTrigger,
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
      if (clickInfo.doubleClick() && !clickInfo.ignore()
          && _pickedUpMesh->actionManager->hasSpecificTrigger(
            ActionManager::OnDoublePickTrigger)) {
        _pickedUpMesh->actionManager->processTrigger(
          ActionManager::OnDoublePickTrigger,
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
#endif
    }
  }
  else {
    if (!clickInfo.ignore) {
      for (const auto& step : _pointerUpStage) {
        pickResult = step.action(_unTranslatedPointerX, _unTranslatedPointerY, pickResult, evt);
      }
    }
  }

  if (_pickedDownMesh && _pickedDownMesh->actionManager
      && _pickedDownMesh->actionManager->hasSpecificTrigger(ActionManager::OnPickOutTrigger)
      && _pickedDownMesh != _pickedUpMesh) {
#if 0
    _pickedDownMesh->actionManager->processTrigger(
      ActionManager::OnPickOutTrigger,
      ActionEvent::CreateNew(_pickedDownMesh, evt));
#endif
  }

  auto type = PointerEventTypes::POINTERUP;
  if (onPointerObservable.hasObservers()) {
    if (!clickInfo.ignore()) {
      if (!clickInfo.hasSwiped()) {
        if (clickInfo.singleClick()
            && onPointerObservable.hasSpecificMask(
              static_cast<int>(PointerEventTypes::POINTERTAP))) {
          auto iType = PointerEventTypes::POINTERTAP;
          PointerInfo pi(iType, evt, *pickResult);
          _setRayOnPointerInfo(pi);
          onPointerObservable.notifyObservers(&pi, static_cast<int>(iType));
        }
        if (clickInfo.doubleClick()
            && onPointerObservable.hasSpecificMask(
              static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))) {
          auto iType = PointerEventTypes::POINTERDOUBLETAP;
          PointerInfo pi(iType, evt, *pickResult);
          _setRayOnPointerInfo(pi);
          onPointerObservable.notifyObservers(&pi, static_cast<int>(iType));
        }
      }
    }
    else {
      auto iType = PointerEventTypes::POINTERUP;
      PointerInfo pi(iType, evt, *pickResult);
      _setRayOnPointerInfo(pi);
      onPointerObservable.notifyObservers(&pi, static_cast<int>(iType));
    }
  }

  if (onPointerUp) {
    onPointerUp(evt, pickResult, type);
  }

  return *this;
}

bool Scene::isPointerCaptured(int pointerId)
{
  return stl_util::contains(_pointerCaptures, pointerId) && _pointerCaptures[pointerId];
}

bool Scene::_isPointerSwiping() const
{
  return std::abs(_startingPointerPosition.x - _pointerX) > Scene::DragMovementThreshold
         || std::abs(_startingPointerPosition.y - _pointerY) > Scene::DragMovementThreshold;
}

void Scene::attachControl(bool attachUp, bool attachDown, bool attachMove)
{
  _initActionManager = [this](const AbstractActionManagerPtr& act,
                              const ClickInfo& /*clickInfo*/) -> AbstractActionManagerPtr {
    if (!_meshPickProceed) {
      auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY, pointerDownPredicate,
                             false, cameraToUseForPointers);
      if (pickResult) {
        _currentPickResult = *pickResult;
      }
      else {
        _currentPickResult = std::nullopt;
      }
      if (pickResult) {
#if 0
        const auto _pickResult = *pickResult;
        act                    = (_pickResult.hit && _pickResult.pickedMesh) ?
                _pickResult.pickedMesh->actionManager :
                nullptr;
#endif
      }
      _meshPickProceed = true;
    }
    return act;
  };

  _initClickEvent = [this](Observable<PointerInfoPre>& obs1, Observable<PointerInfo>& obs2,
                           const PointerEvent& evt,
                           const std::function<void(const ClickInfo& clickInfo,
                                                    std::optional<PickingInfo>& pickResult)>& cb) {
    ClickInfo clickInfo;
    _currentPickResult           = std::nullopt;
    AbstractActionManagerPtr act = nullptr;

    auto checkPicking
      = obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERPICK))
        || obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERPICK))
        || obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERTAP))
        || obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERTAP))
        || obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
        || obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));
    if (!checkPicking && ActionManager::HasPickTriggers()) {
      act = _initActionManager(act, clickInfo);
      if (act) {
        checkPicking = act->hasPickTriggers();
      }
    }
    if (checkPicking) {
      auto btn            = evt.button;
      clickInfo.hasSwiped = _isPointerSwiping();

      if (!clickInfo.hasSwiped) {
        auto checkSingleClickImmediately = !Scene::ExclusiveDoubleClickMode;

        if (!checkSingleClickImmediately) {
          checkSingleClickImmediately
            = !obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
              && !obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));

          if (checkSingleClickImmediately
              && !ActionManager::HasSpecificTrigger(ActionManager::OnDoublePickTrigger)) {
            act = _initActionManager(act, clickInfo);
            if (act) {
              checkSingleClickImmediately
                = !act->hasSpecificTrigger(ActionManager::OnDoublePickTrigger);
            }
          }
        }

        if (checkSingleClickImmediately) {
          // single click detected if double click delay is over or two
          // different successive keys pressed without exclusive double click
          // or no double click required
          if (Time::fpTimeSince<long, std::milli>(_previousStartingPointerTime)
                > Scene::DoubleClickDelay.count()
              || btn != _previousButtonPressed) {
            clickInfo.singleClick = true;

            cb(clickInfo, _currentPickResult);
          }
        }
        // at least one double click is required to be check and exclusive
        // double click is enabled
        else {
          // wait that no double click has been raised during the double click
          // delay
          _previousDelayedSimpleClickTimeout = _delayedSimpleClickTimeout;
        }

        auto checkDoubleClick
          = obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
            || obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));
        if (!checkDoubleClick
            && ActionManager::HasSpecificTrigger(ActionManager::OnDoublePickTrigger)) {
          act = _initActionManager(act, clickInfo);
          if (act) {
            checkDoubleClick = act->hasSpecificTrigger(ActionManager::OnDoublePickTrigger);
          }
        }
        if (checkDoubleClick) {
          // two successive keys pressed are equal, double click delay is not
          // over and double click has not just occurred
          if (btn == _previousButtonPressed
              && Time::fpTimeSince<long, std::milli>(_previousStartingPointerTime)
                   < Scene::DoubleClickDelay.count()
              && !_doubleClickOccured) {
            // pointer has not moved for 2 clicks, it's a double click
            if (!clickInfo.hasSwiped && !_isPointerSwiping()) {
              _previousStartingPointerTime = high_res_time_point_t();
              _doubleClickOccured          = true;
              clickInfo.doubleClick        = true;
              clickInfo.ignore             = false;
              if (Scene::ExclusiveDoubleClickMode
                  && _previousDelayedSimpleClickTimeout.count() > 0) {
                // clearTimeout(_previousDelayedSimpleClickTimeout);
              }
              _previousDelayedSimpleClickTimeout = _delayedSimpleClickTimeout;
              cb(clickInfo, _currentPickResult);
            }
            // if the two successive clicks are too far, it's just two simple
            // clicks
            else {
              _doubleClickOccured                = false;
              _previousStartingPointerTime       = _startingPointerTime;
              _previousStartingPointerPosition.x = _startingPointerPosition.x;
              _previousStartingPointerPosition.y = _startingPointerPosition.y;
              _previousButtonPressed             = btn;
              if (Scene::ExclusiveDoubleClickMode) {
                if (_previousDelayedSimpleClickTimeout.count() > 0) {
                  // clearTimeout(_previousDelayedSimpleClickTimeout);
                }
                _previousDelayedSimpleClickTimeout = _delayedSimpleClickTimeout;

                cb(clickInfo, _previousPickResult);
              }
              else {
                cb(clickInfo, _currentPickResult);
              }
            }
          }
          // just the first click of the double has been raised
          else {
            _doubleClickOccured                = false;
            _previousStartingPointerTime       = _startingPointerTime;
            _previousStartingPointerPosition.x = _startingPointerPosition.x;
            _previousStartingPointerPosition.y = _startingPointerPosition.y;
            _previousButtonPressed             = btn;
          }
        }
      }
    }
    clickInfo.ignore = true;
    cb(clickInfo, _currentPickResult);
  };

  spritePredicate = [](Sprite* sprite) {
    return sprite->isPickable && sprite->actionManager
           && sprite->actionManager->hasPointerTriggers();
  };

  _onPointerMove = [this](PointerEvent&& evt) { _onPointerMoveEvent(std::move(evt)); };

  _onPointerDown = [this](PointerEvent&& evt) { _onPointerDownEvent(std::move(evt)); };

  _onPointerUp = [this](PointerEvent&& evt) { _onPointerUpEvent(std::move(evt)); };

  _onKeyDown = [this](KeyboardEvent&& evt) { _onKeyDownEvent(std::move(evt)); };

  _onKeyUp = [this](KeyboardEvent&& evt) { _onKeyUpEvent(std::move(evt)); };

  auto canvas = _engine->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  canvas->addKeyEventListener(EventType::KEY_DOWN, _onKeyDown, false);
  canvas->addKeyEventListener(EventType::KEY_UP, _onKeyUp, false);

  if (attachMove) {
    canvas->addMouseEventListener(EventType::MOUSE_MOVE, _onPointerMove, false);
    // Wheel
    canvas->addMouseEventListener(EventType::MOUSE_WHEEL, _onPointerMove, false);
  }

  if (attachDown) {
    canvas->addMouseEventListener(EventType::MOUSE_BUTTON_DOWN, _onPointerDown, false);
  }

  if (attachUp) {
    canvas->addMouseEventListener(EventType::MOUSE_BUTTON_UP, _onPointerUp, false);
  }

  canvas->tabIndex = 1;
}

void Scene::detachControl()
{
  auto engine = getEngine();
  auto canvas = engine->getRenderingCanvas();

  if (!canvas) {
    return;
  }

  canvas->removeMouseEventListener(EventType::MOUSE_MOVE, _onPointerMove);
  canvas->removeMouseEventListener(EventType::MOUSE_BUTTON_DOWN, _onPointerDown);
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
  canvas->removeKeyEventListener(EventType::KEY_DOWN, _onKeyDown);
  canvas->removeKeyEventListener(EventType::KEY_UP, _onKeyUp);

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
  if (_checkPrePointerObservable(
        std::nullopt, evt,
        (evt.type == EventType::MOUSE_WHEEL || evt.type == EventType::DOM_MOUSE_SCROLL) ?
          PointerEventTypes::POINTERWHEEL :
          PointerEventTypes::POINTERMOVE)) {
    return;
  }

  if (!cameraToUseForPointers && !_activeCamera) {
    return;
  }

  if (!pointerMovePredicate) {
    pointerMovePredicate = [this](const AbstractMeshPtr& mesh) -> bool {
      return mesh->isPickable && mesh->isVisible && mesh->isReady() && mesh->isEnabled()
             && (mesh->enablePointerMoveEvents || constantlyUpdateMeshUnderPointer
                 || mesh->actionManager != nullptr);
    };
  }

  // Meshes
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY, pointerMovePredicate, false,
                         cameraToUseForPointers);

  _processPointerMove(pickResult, evt);
}

void Scene::_onPointerDownEvent(PointerEvent&& evt)
{
  ++_totalPointersPressed;
  _pickedDownMesh  = nullptr;
  _meshPickProceed = false;

  _updatePointerPosition(evt);

  if (preventDefaultOnPointerDown && _engine->getRenderingCanvas()) {
    evt.preventDefault();
    _engine->getRenderingCanvas()->focus();
  }

  // PreObservable support
  if (_checkPrePointerObservable(std::nullopt, evt, PointerEventTypes::POINTERDOWN)) {
    return;
  }

  if (!cameraToUseForPointers && !_activeCamera) {
    return;
  }

  _pointerCaptures[evt.pointerId] = true;
  _startingPointerPosition.x      = static_cast<float>(_pointerX);
  _startingPointerPosition.y      = static_cast<float>(_pointerY);
  _startingPointerTime            = Time::highresTimepointNow();

  if (!pointerDownPredicate) {
    pointerDownPredicate = [](const AbstractMeshPtr& mesh) -> bool {
      return mesh->isPickable && mesh->isVisible && mesh->isReady() && mesh->isEnabled();
    };
  }

  // Meshes
  _pickedDownMesh = nullptr;
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY, pointerDownPredicate, false,
                         cameraToUseForPointers);

  _processPointerDown(pickResult, evt);
}

void Scene::_onPointerUpEvent(PointerEvent&& evt)
{
  if (_totalPointersPressed == 0) { // We are attaching the pointer up to
                                    // windows because of a bug in FF
    return;                         // So we need to test it the pointer down was pressed before.
  }

  --_totalPointersPressed;
  _pickedUpMesh    = nullptr;
  _meshPickProceed = false;

  _updatePointerPosition(evt);
  _initClickEvent(
    onPrePointerObservable, onPointerObservable, evt,
    [this, evt](const ClickInfo& clickInfo, std::optional<PickingInfo>& pickResult) {
      // PreObservable support
      if (onPrePointerObservable.hasObservers()) {
        if (!clickInfo.ignore) {
          if (!clickInfo.hasSwiped) {
            if (clickInfo.singleClick
                && onPrePointerObservable.hasSpecificMask(
                  static_cast<int>(PointerEventTypes::POINTERTAP))) {
              if (_checkPrePointerObservable(std::nullopt, evt, PointerEventTypes::POINTERTAP)) {
                return;
              }
            }
            if (clickInfo.doubleClick
                && onPrePointerObservable.hasSpecificMask(
                  static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))) {
              if (_checkPrePointerObservable(std::nullopt, evt,
                                             PointerEventTypes::POINTERDOUBLETAP)) {
                return;
              }
            }
          }
        }
        else {
          if (_checkPrePointerObservable(std::nullopt, evt, PointerEventTypes::POINTERUP)) {
            return;
          }
        }
      }

      if (!cameraToUseForPointers && !_activeCamera) {
        return;
      }

      _pointerCaptures[evt.pointerId] = false;

      if (!pointerUpPredicate) {
        pointerUpPredicate = [](const AbstractMeshPtr& mesh) -> bool {
          return mesh->isPickable && mesh->isVisible && mesh->isReady() && mesh->isEnabled();
        };
      }

      // Meshes
      if (!_meshPickProceed
          && (ActionManager::HasTriggers() || onPointerObservable.hasObservers())) {
        _initActionManager(nullptr, clickInfo);
      }
      if (!pickResult && _currentPickResult.has_value()) {
        pickResult = *_currentPickResult;
      }

      _processPointerUp(pickResult, evt, clickInfo);

      _previousPickResult = _currentPickResult;
    });
}

void Scene::_onKeyDownEvent(KeyboardEvent&& evt)
{
  auto type = KeyboardEventTypes::KEYDOWN;
  if (onPreKeyboardObservable.hasObservers()) {
    KeyboardInfoPre pi(type, evt);
    onPreKeyboardObservable.notifyObservers(&pi, static_cast<int>(type));
    if (pi.skipOnPointerObservable) {
      return;
    }
  }

  if (onKeyboardObservable.hasObservers()) {
    KeyboardInfo pi(type, evt);
    onKeyboardObservable.notifyObservers(&pi, static_cast<int>(type));
  }

  if (actionManager) {
#if 0
    actionManager->processTrigger(ActionManager::OnKeyDownTrigger,
                                  ActionEvent::CreateNewFromScene(this, evt));
#endif
  }
}

void Scene::_onKeyUpEvent(KeyboardEvent&& evt)
{
  auto type = KeyboardEventTypes::KEYUP;
  if (onPreKeyboardObservable.hasObservers()) {
    KeyboardInfoPre pi(type, evt);
    onPreKeyboardObservable.notifyObservers(&pi, static_cast<int>(type));
    if (pi.skipOnPointerObservable) {
      return;
    }
  }

  if (onKeyboardObservable.hasObservers()) {
    KeyboardInfo pi(type, evt);
    onKeyboardObservable.notifyObservers(&pi, static_cast<int>(type));
  }

  if (actionManager) {
#if 0
    actionManager->processTrigger(ActionManager::OnKeyUpTrigger,
                                  ActionEvent::CreateNewFromScene(this, evt));
#endif
  }
}

bool Scene::isReady()
{
  if (_isDisposed) {
    return false;
  }

  auto engine = getEngine();

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
      = mesh->hasThinInstances() || mesh->getClassName() == "InstancedMesh"
        || mesh->getClassName() == "InstancedLinesMesh"
        || (engine->getCaps().instancedArrays && _mesh->instances.size() > 0);

    // Is Ready For Mesh
    for (const auto& step : _isReadyForMeshStage) {
      if (!step.action(mesh.get(), hardwareInstancedRendering)) {
        return false;
      }
    }
  }

  // Pending data
  if (!_pendingData.empty()) {
    return false;
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

void Scene::registerBeforeRender(const std::function<void(Scene* scene, EventState& es)>& func)
{
  onBeforeRenderObservable.add(func);
}

void Scene::unregisterBeforeRender(const std::function<void(Scene* scene, EventState& es)>& func)
{
  onBeforeRenderObservable.removeCallback(func);
}

void Scene::registerAfterRender(const std::function<void(Scene* scene, EventState& es)>& func)
{
  onAfterRenderObservable.add(func);
}

void Scene::unregisterAfterRender(const std::function<void(Scene* scene, EventState& es)>& func)
{
  onAfterRenderObservable.removeCallback(func);
}

void Scene::_addPendingData(Mesh* /*mesh*/)
{
}

void Scene::_addPendingData(const InternalTexturePtr& /*texure*/)
{
}

void Scene::_removePendingData(Mesh* /*mesh*/)
{
  const auto wasLoading = isLoading();

  if (wasLoading && !isLoading()) {
    onDataLoadedObservable.notifyObservers(this);
  }
}

void Scene::_removePendingData(const InternalTexturePtr& /*texture*/)
{
  const auto wasLoading = isLoading();

  if (wasLoading && !isLoading()) {
    onDataLoadedObservable.notifyObservers(this);
  }
}

void Scene::getWaitingItemsCount()
{
}

bool Scene::get_isLoading() const
{
  return !_pendingData.empty();
}

void Scene::executeWhenReady(const std::function<void(Scene* scene, EventState& es)>& func)
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

std::vector<AnimationPtr> Scene::getAnimations()
{
  return std::vector<AnimationPtr>();
}

AnimatablePtr
Scene::beginWeightedAnimation(const IAnimatablePtr& target, float from, float to, float weight,
                              bool loop, float speedRatio,
                              const std::function<void()>& onAnimationEnd, AnimatablePtr animatable,
                              const std::function<bool(IAnimatable* target)>& targetMask,
                              const std::function<void()>& onAnimationLoop)
{
  auto returnedAnimatable    = beginAnimation(target, from, to, loop, speedRatio, onAnimationEnd,
                                           animatable, false, targetMask, onAnimationLoop);
  returnedAnimatable->weight = weight;

  return returnedAnimatable;
}

AnimatablePtr Scene::beginAnimation(const IAnimatablePtr& target, float from, float to, bool loop,
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
    animatable = Animatable::New(this, target, from, to, loop, speedRatio, onAnimationEnd,
                                 animationList, onAnimationLoop);
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
      beginAnimation(childAnimatable, from, to, loop, speedRatio, onAnimationEnd, animatable,
                     stopCurrent, targetMask, onAnimationLoop);
    }
  }

  animatable->reset();

  return animatable;
}

std::vector<AnimatablePtr> Scene::beginHierarchyAnimation(
  const NodePtr& target, bool directDescendantsOnly, float from, float to, bool loop,
  float speedRatio, const std::function<void()>& onAnimationEnd, AnimatablePtr animatable,
  bool stopCurrent, const std::function<bool(IAnimatable* target)>& targetMask,
  const std::function<void()>& onAnimationLoop)
{
  auto children = target->getDescendants(directDescendantsOnly);

  std::vector<AnimatablePtr> result;
  result.emplace_back(beginAnimation(target, from, to, loop, speedRatio, onAnimationEnd, animatable,
                                     stopCurrent, targetMask, onAnimationLoop));
  for (const auto& child : children) {
    result.emplace_back(beginAnimation(child, from, to, loop, speedRatio, onAnimationEnd,
                                       animatable, stopCurrent, targetMask, onAnimationLoop));
  }

  return result;
}

AnimatablePtr Scene::beginDirectAnimation(const IAnimatablePtr& target,
                                          const std::vector<AnimationPtr>& _animations, float from,
                                          float to, bool loop, float speedRatio,
                                          const std::function<void()>& onAnimationEnd,
                                          const std::function<void()>& onAnimationLoop,
                                          const std::optional<bool>& isAdditive)
{
  if (from > to && speedRatio > 0.f) {
    speedRatio *= -1.f;
  }

  auto animatable = Animatable::New(this, target, from, to, loop, speedRatio, onAnimationEnd,
                                    _animations, onAnimationLoop, isAdditive);

  return animatable;
}

std::vector<AnimatablePtr> Scene::beginDirectHierarchyAnimation(
  const NodePtr& target, bool directDescendantsOnly, const std::vector<AnimationPtr>& iAnimations,
  float from, float to, bool loop, float speedRatio, const std::function<void()>& onAnimationEnd,
  const std::function<void()>& onAnimationLoop)
{
  auto children = target->getDescendants(directDescendantsOnly);

  std::vector<AnimatablePtr> result;
  result.emplace_back(beginDirectAnimation(target, iAnimations, from, to, loop, speedRatio,
                                           onAnimationEnd, onAnimationLoop));
  for (const auto& child : children) {
    result.emplace_back(beginDirectAnimation(child, iAnimations, from, static_cast<float>(to), loop,
                                             speedRatio, onAnimationEnd, onAnimationLoop));
  }

  return result;
}

AnimatablePtr Scene::getAnimatableByTarget(const IAnimatablePtr& target)
{
  auto it = std::find_if(
    _activeAnimatables.begin(), _activeAnimatables.end(),
    [&target](const AnimatablePtr& animatable) { return animatable->target == target; });
  return (it == _activeAnimatables.end()) ? nullptr : *it;
}

std::vector<AnimatablePtr> Scene::getAllAnimatablesByTarget(const IAnimatablePtr& target)
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

void Scene::stopAnimation(const IAnimatablePtr& target, const std::string& animationName,
                          const std::function<bool(IAnimatable* target)>& targetMask)
{
  return stopAnimation(target.get(), animationName, targetMask);
}

void Scene::stopAnimation(IAnimatable* target, const std::string& animationName,
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

  // Getting time
  auto now = Time::highresTimepointNow();
  if (!_animationTimeLast.has_value()) {
    if (!_pendingData.empty()) {
      return;
    }
    _animationTimeLast = now;
  }
  deltaTime          = useConstantAnimationDeltaTime ?
                         16.f :
                         Time::fpTimeSince<size_t, std::milli>(*_animationTimeLast) * animationTimeScale;
  _animationTimeLast = now;

  // Animatable::_animate can remove elements from _activeAnimatables we need to make a copy of it
  // in order to iterate on a vector that will not be modified during the loop!
  const auto& animatables = _activeAnimatables;
  if (animatables.empty()) {
    return;
  }

  _animationTime += static_cast<int>(deltaTime);
  const auto& animationTime = _animationTime;

  // We make a copy of "animatables" because animatable->_animate can suppress
  // elements from "animatables"
  auto animatables_copy = animatables;
  for (const auto& animatable : animatables_copy) {
    if (animatable) {
      if (!animatable->_animate(std::chrono::milliseconds(animationTime))
          && animatable->disposeOnEnd) {
        // The animation removed itself from _activeAnimatables
        // during the call to _animate()
      }
    }
  }

  // Late animation bindings
  _processLateAnimationBindings();
}

void Scene::_registerTargetForLateAnimationBinding(RuntimeAnimation* /*runtimeAnimation*/,
                                                   const AnimationValue& /*originalValue*/)
{
  // TODO Implement
}

AnimationValue
Scene::_processLateAnimationBindingsForMatrices(float holderTotalWeight,
                                                std::vector<RuntimeAnimation*>& holderAnimations,
                                                Matrix& holderOriginalValue)
{
  auto normalizer                           = 1.f;
  std::optional<Vector3> finalPosition      = TmpVectors::Vector3Array[0];
  std::optional<Vector3> finalScaling       = TmpVectors::Vector3Array[1];
  std::optional<Quaternion> finalQuaternion = TmpVectors::QuaternionArray[0];
  auto startIndex                           = 0u;
  auto& originalAnimation                   = holderAnimations[0];
  auto& originalValue                       = holderOriginalValue;

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

  finalScaling->scaleInPlace(scale);
  finalPosition->scaleInPlace(scale);
  finalQuaternion->scaleInPlace(scale);

  for (size_t animIndex = startIndex; animIndex < holderAnimations.size(); ++animIndex) {
    auto& runtimeAnimation                      = holderAnimations[animIndex];
    auto iScale                                 = runtimeAnimation->weight / normalizer;
    std::optional<Vector3> currentPosition      = TmpVectors::Vector3Array[2];
    std::optional<Vector3> currentScaling       = TmpVectors::Vector3Array[3];
    std::optional<Quaternion> currentQuaternion = TmpVectors::QuaternionArray[1];

    (*runtimeAnimation->currentValue())
      .get<Matrix>()
      .decompose(currentScaling, currentQuaternion, currentPosition);
    currentScaling->scaleAndAddToRef(iScale, *finalScaling);
    currentQuaternion->scaleAndAddToRef(iScale, *finalQuaternion);
    currentPosition->scaleAndAddToRef(iScale, *finalPosition);
  }

  auto& workValue = *originalAnimation->_animationState.workValue;
  Matrix::ComposeToRef(*finalScaling, *finalQuaternion, *finalPosition, workValue.get<Matrix>());
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
    Quaternion::SlerpToRef(originalValue, (*originalAnimation->currentValue()).get<Quaternion>(),
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
      Quaternion::SlerpToRef((*holderAnimations[0]->currentValue()).get<Quaternion>(),
                             (*holderAnimations[1]->currentValue()).get<Quaternion>(),
                             holderAnimations[1]->weight / holderTotalWeight, refQuaternion);
      return refQuaternion;
    }
    quaternions.clear();
    weights.clear();

    normalizer = holderTotalWeight;
  }
  for (const auto& runtimeAnimation : holderAnimations) {
    quaternions.emplace_back((*runtimeAnimation->currentValue()).get<Quaternion>());
    weights.emplace_back(runtimeAnimation->weight / normalizer);
  }

  // https://gamedev.stackexchange.com/questions/62354/method-for-interpolation-between-3-quaternions

  auto cumulativeAmount                          = 0.f;
  std::optional<Quaternion> cumulativeQuaternion = std::nullopt;
  for (size_t index = 0; index < quaternions.size();) {
    if (!cumulativeQuaternion) {
      Quaternion::SlerpToRef(quaternions[index], quaternions[index + 1],
                             weights[index + 1] / (weights[index] + weights[index + 1]),
                             refQuaternion);
      cumulativeQuaternion = refQuaternion;
      cumulativeAmount     = weights[index] + weights[index + 1];
      index += 2;
      continue;
    }
    cumulativeAmount += weights[index];
    auto _cumulativeQuaternion = *cumulativeQuaternion;
    Quaternion::SlerpToRef(*cumulativeQuaternion, quaternions[index],
                           weights[index] / cumulativeAmount, _cumulativeQuaternion);
    cumulativeQuaternion = _cumulativeQuaternion;
    ++index;
  }

  return cumulativeQuaternion ? *cumulativeQuaternion : Quaternion();
}

void Scene::_processLateAnimationBindings()
{
  // TODO Implement
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
  return _useAlternateCameraConfiguration ? _alternateProjectionMatrix : _projectionMatrix;
}

const Matrix& Scene::getProjectionMatrix() const
{
  return _useAlternateCameraConfiguration ? _alternateProjectionMatrix : _projectionMatrix;
}

Matrix Scene::getTransformMatrix()
{
  return _useAlternateCameraConfiguration ? *_alternateTransformMatrix : _transformMatrix;
}

void Scene::setTransformMatrix(Matrix& viewL, Matrix& projectionL,
                               const std::optional<Matrix>& viewR,
                               const std::optional<Matrix>& projectionR)
{
  if (_viewUpdateFlag == viewL.updateFlag && _projectionUpdateFlag == projectionL.updateFlag) {
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
    _alternateTransformMatrix = std::make_unique<Matrix>(Matrix::Zero());
  }

  _alternateViewMatrix.multiplyToRef(_alternateProjectionMatrix, *_alternateTransformMatrix);

  if (!_alternateSceneUbo) {
    _createAlternateUbo();
  }

  if (_alternateSceneUbo->useUbo()) {
    _alternateSceneUbo->updateMatrix("viewProjection", *_alternateTransformMatrix);
    _alternateSceneUbo->updateMatrix("view", _alternateViewMatrix);
    _alternateSceneUbo->update();
  }
}

UniformBuffer* Scene::getSceneUniformBuffer()
{
  return _useAlternateCameraConfiguration ? _alternateSceneUbo.get() : _sceneUbo.get();
}

size_t Scene::getUniqueId()
{
  auto result = Scene::_uniqueIdCounter;
  Scene::_uniqueIdCounter++;
  return result;
}

void Scene::addMesh(const AbstractMeshPtr& newMesh, bool recursive)
{
  if (_blockEntityCollection) {
    return;
  }

  meshes.emplace_back(newMesh);

  newMesh->_resyncLightSources();

  if (!newMesh->parent()) {
    newMesh->_addToSceneRootNodes();
  }

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
  auto it    = std::find_if(meshes.begin(), meshes.end(), [toRemove](const AbstractMeshPtr& mesh) {
    return mesh.get() == toRemove;
  });
  auto index = static_cast<int>(it - meshes.begin());
  if (it != meshes.end()) {
    // Remove from the scene if mesh found
    meshes.erase(it);

    if (!toRemove->parent()) {
      toRemove->_removeFromSceneRootNodes();
    }
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
  if (_blockEntityCollection) {
    return;
  }
  newTransformNode->_indexInSceneTransformNodesArray = static_cast<int>(transformNodes.size());
  transformNodes.emplace_back(newTransformNode);

  if (!newTransformNode->parent()) {
    newTransformNode->_addToSceneRootNodes();
  }

  onNewTransformNodeAddedObservable.notifyObservers(newTransformNode.get());
}

int Scene::removeTransformNode(const TransformNodePtr& toRemove)
{
  return removeTransformNode(toRemove.get());
}

int Scene::removeTransformNode(TransformNode* toRemove)
{
  auto it = std::find_if(
    transformNodes.begin(), transformNodes.end(),
    [toRemove](const TransformNodePtr& transformNode) { return transformNode.get() == toRemove; });
  auto index = static_cast<int>(it - transformNodes.begin());
  if (it != transformNodes.end()) {
    // Remove from the scene if found
    transformNodes.erase(it);
    if (!toRemove->parent()) {
      toRemove->_removeFromSceneRootNodes();
    }
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
  auto it
    = std::find_if(skeletons.begin(), skeletons.end(),
                   [toRemove](const SkeletonPtr& skeleton) { return skeleton.get() == toRemove; });
  int index = static_cast<int>(it - skeletons.begin());
  if (it != skeletons.end()) {
    // Remove from the scene if found
    skeletons.erase(it);
  }

  return index;
}

int Scene::removeMorphTargetManager(const MorphTargetManagerPtr& toRemove)
{
  auto it   = std::find(morphTargetManagers.begin(), morphTargetManagers.end(), toRemove);
  int index = static_cast<int>(it - morphTargetManagers.begin());
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
  auto it    = std::find_if(lights.begin(), lights.end(),
                         [&toRemove](const LightPtr& light) { return light.get() == toRemove; });
  auto index = static_cast<int>(it - lights.begin());
  if (it != lights.end()) {
    // Remove from meshes
    for (const auto& mesh : meshes) {
      mesh->_removeLightSource(toRemove, false);
    }

    // Remove from the scene if mesh found
    lights.erase(it);
    sortLightsByPriority();

    if (!toRemove->parent()) {
      toRemove->_removeFromSceneRootNodes();
    }
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
  auto it1   = std::find_if(cameras.begin(), cameras.end(), [&toRemove](const CameraPtr& camera) {
    return camera.get() == toRemove;
  });
  auto index = static_cast<int>(it1 - cameras.begin());
  if (it1 != cameras.end()) {
    // Remove from the scene if camera found
    cameras.erase(it1);
    if (!toRemove->parent()) {
      toRemove->_removeFromSceneRootNodes();
    }
  }
  // Remove from activeCameras
  auto it2 = std::find_if(activeCameras.begin(), activeCameras.end(),
                          [toRemove](const CameraPtr& camera) { return camera.get() == toRemove; });
  if (it2 != activeCameras.end()) {
    // Remove from the scene if camera found
    activeCameras.erase(it2);
  }
  // Reset the _activeCamera
  if (_activeCamera.get() == toRemove) {
    if (!cameras.empty()) {
      _activeCamera = cameras.front();
    }
    else {
      _activeCamera = nullptr;
    }
  }

  onCameraRemovedObservable.notifyObservers(toRemove);

  return index;
}

int Scene::removeParticleSystem(IParticleSystem* toRemove)
{
  auto it   = std::find_if(particleSystems.begin(), particleSystems.end(),
                         [&toRemove](const IParticleSystemPtr& particleSystem) {
                           return particleSystem.get() == toRemove;
                         });
  int index = static_cast<int>(it - particleSystems.begin());
  if (it != particleSystems.end()) {
    particleSystems.erase(it);
  }
  return index;
}

int Scene::removeAnimation(const AnimationPtr& toRemove)
{
  auto it   = std::find(animations.begin(), animations.end(), toRemove);
  int index = static_cast<int>(it - animations.begin());
  if (it != animations.end()) {
    animations.erase(it);
  }
  return index;
}

int Scene::removeAnimationGroup(const AnimationGroupPtr& toRemove)
{
  auto it   = std::find(animationGroups.begin(), animationGroups.end(), toRemove);
  int index = static_cast<int>(it - animationGroups.begin());
  if (it != animationGroups.end()) {
    animationGroups.erase(it);
  }
  return index;
}

int Scene::removeMultiMaterial(const MultiMaterialPtr& toRemove)
{
  auto it   = std::find(multiMaterials.begin(), multiMaterials.end(), toRemove);
  int index = static_cast<int>(it - multiMaterials.begin());
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
  auto it
    = std::find_if(materials.begin(), materials.end(),
                   [toRemove](const MaterialPtr& material) { return material.get() == toRemove; });
  int index = static_cast<int>(it - materials.begin());
  if (it != materials.end()) {
    materials.erase(it);
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
  auto it   = std::find_if(actionManagers.begin(), actionManagers.end(),
                         [toRemove](const AbstractActionManagerPtr& actionManager_) {
                           return actionManager_.get() == toRemove;
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
  auto it
    = std::find_if(textures.begin(), textures.end(),
                   [toRemove](const BaseTexturePtr& texture) { return texture.get() == toRemove; });
  int index = static_cast<int>(it - textures.begin());
  if (it != textures.end()) {
    textures.erase(it);
  }
  onTextureRemovedObservable.notifyObservers(toRemove);

  return index;
}

void Scene::addLight(const LightPtr& newLight)
{
  if (_blockEntityCollection) {
    return;
  }
  lights.emplace_back(newLight);
  sortLightsByPriority();

  if (!newLight->parent()) {
    newLight->_addToSceneRootNodes();
  }

  // Add light to all meshes (To support if the light is removed and then re-added)
  for (const auto& mesh : meshes) {
    if (!stl_util::contains(mesh->_lightSources, newLight)) {
      mesh->_lightSources.emplace_back(newLight);
      mesh->_resyncLightSources();
    }
  }

  onNewLightAddedObservable.notifyObservers(newLight.get());
}

void Scene::sortLightsByPriority()
{
  if (requireLightSorting) {
    BABYLON::stl_util::sort_js_style(lights, [](const LightPtr& a, const LightPtr& b) {
      return Light::CompareLightsPriority(a.get(), b.get());
    });
  }
}

void Scene::addCamera(const CameraPtr& newCamera)
{
  if (_blockEntityCollection) {
    return;
  }
  cameras.emplace_back(newCamera);
  onNewCameraAddedObservable.notifyObservers(newCamera.get());

  if (!newCamera->parent()) {
    newCamera->_addToSceneRootNodes();
  }
}

void Scene::addSkeleton(const SkeletonPtr& newSkeleton)
{
  if (_blockEntityCollection) {
    return;
  }
  skeletons.emplace_back(newSkeleton);
}

void Scene::addParticleSystem(const IParticleSystemPtr& newParticleSystem)
{
  if (_blockEntityCollection) {
    return;
  }
  particleSystems.emplace_back(newParticleSystem);
}

void Scene::addAnimation(const AnimationPtr& newAnimation)
{
  if (_blockEntityCollection) {
    return;
  }
  animations.emplace_back(newAnimation);
}

void Scene::addAnimationGroup(const AnimationGroupPtr& newAnimationGroup)
{
  if (_blockEntityCollection) {
    return;
  }
  animationGroups.emplace_back(newAnimationGroup);
}

void Scene::addMultiMaterial(const MultiMaterialPtr& newMultiMaterial)
{
  if (_blockEntityCollection) {
    return;
  }
  multiMaterials.emplace_back(newMultiMaterial);
}

void Scene::addMaterial(const MaterialPtr& newMaterial)
{
  if (_blockEntityCollection) {
    return;
  }
  materials.emplace_back(newMaterial);
  onNewMaterialAddedObservable.notifyObservers(newMaterial.get());
}

void Scene::addMorphTargetManager(const MorphTargetManagerPtr& newMorphTargetManager)
{
  if (_blockEntityCollection) {
    return;
  }
  morphTargetManagers.emplace_back(newMorphTargetManager);
}

void Scene::addGeometry(const GeometryPtr& newGeometry)
{
  if (_blockEntityCollection) {
    return;
  }
  geometriesById[newGeometry->id] = geometries.size();
  geometries.emplace_back(newGeometry);
}

void Scene::addActionManager(const AbstractActionManagerPtr& newActionManager)
{
  actionManagers.emplace_back(newActionManager);
}

void Scene::addTexture(const BaseTexturePtr& newTexture)
{
  if (_blockEntityCollection) {
    return;
  }
  textures.emplace_back(newTexture);
  onNewTextureAddedObservable.notifyObservers(newTexture.get());
}

void Scene::switchActiveCamera(const CameraPtr& newCamera, bool attachControl)
{
  auto canvas = _engine->getInputElement();

  if (!canvas) {
    return;
  }

  if (_activeCamera) {
    _activeCamera->detachControl(canvas);
  }

  _activeCamera = newCamera;
  if (attachControl) {
    newCamera->attachControl(canvas);
  }
}

CameraPtr Scene::setActiveCameraByID(const std::string& id)
{
  auto camera = getCameraByID(id);

  if (camera) {
    _activeCamera = camera;
    return camera;
  }

  return nullptr;
}

CameraPtr Scene::setActiveCameraByName(const std::string& name)
{
  auto camera = getCameraByName(name);

  if (camera) {
    _activeCamera = camera;
    return camera;
  }

  return nullptr;
}

AnimationGroupPtr Scene::getAnimationGroupByName(const std::string& name)
{
  auto it = std::find_if(
    animationGroups.begin(), animationGroups.end(),
    [&name](const AnimationGroupPtr& animationGroup) { return animationGroup->name == name; });

  return (it == animationGroups.end()) ? nullptr : *it;
}

MaterialPtr Scene::getMaterialByID(const std::string& id)
{
  auto it = std::find_if(materials.begin(), materials.end(),
                         [&id](const MaterialPtr& material) { return material->id == id; });

  return (it == materials.end()) ? nullptr : *it;
}

MaterialPtr Scene::getLastMaterialByID(const std::string& id)
{
  if (materials.empty()) {
    return nullptr;
  }

  for (size_t index = materials.size(); index-- > 0;) {
    if (materials[index]->id == id) {
      return materials[index];
    }
  }

  return nullptr;
}

MaterialPtr Scene::getMaterialByUniqueID(size_t uniqueId)
{
  auto it
    = std::find_if(materials.begin(), materials.end(), [uniqueId](const MaterialPtr& material) {
        return material->uniqueId == uniqueId;
      });

  return (it == materials.end()) ? nullptr : *it;
}

MaterialPtr Scene::getMaterialByName(const std::string& name)
{
  auto it = std::find_if(materials.begin(), materials.end(),
                         [&name](const MaterialPtr& material) { return material->name == name; });

  return (it == materials.end()) ? nullptr : *it;
}

CameraPtr Scene::getCameraByID(const std::string& id)
{
  auto it = std::find_if(cameras.begin(), cameras.end(),
                         [&id](const CameraPtr& camera) { return camera->id == id; });

  return (it == cameras.end()) ? nullptr : *it;
}

CameraPtr Scene::getCameraByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(cameras.begin(), cameras.end(), [uniqueId](const CameraPtr& camera) {
    return camera->uniqueId == uniqueId;
  });

  return (it == cameras.end()) ? nullptr : *it;
}

CameraPtr Scene::getCameraByName(const std::string& name)
{
  auto it = std::find_if(cameras.begin(), cameras.end(),
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
  auto it = std::find_if(lights.begin(), lights.end(),
                         [&name](const LightPtr& light) { return light->name == name; });

  return (it == lights.end()) ? nullptr : *it;
}

LightPtr Scene::getLightByID(const std::string& id)
{
  auto it = std::find_if(lights.begin(), lights.end(),
                         [&id](const LightPtr& light) { return light->id == id; });

  return (it == lights.end()) ? nullptr : *it;
}

LightPtr Scene::getLightByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(lights.begin(), lights.end(),
                         [uniqueId](const LightPtr& light) { return light->uniqueId == uniqueId; });

  return (it == lights.end()) ? nullptr : *it;
}

IParticleSystemPtr Scene::getParticleSystemByID(const std::string& id)
{
  auto it = std::find_if(
    particleSystems.begin(), particleSystems.end(),
    [&id](const IParticleSystemPtr& particleSystem) { return particleSystem->id == id; });

  return (it == particleSystems.end()) ? nullptr : *it;
}

GeometryPtr Scene::getGeometryByID(const std::string& id)
{
  if (!geometriesById.empty() && stl_util::contains(geometriesById, id)) {
    const auto index = geometriesById[id];
    if (index < geometries.size()) {
      return geometries[index];
    }
  }
  else {
    auto it = std::find_if(geometries.begin(), geometries.end(),
                           [&id](const GeometryPtr& geometry) { return geometry->id == id; });

    return (it == geometries.end()) ? nullptr : *it;
  }

  return nullptr;
}

bool Scene::pushGeometry(const GeometryPtr& geometry, bool force)
{
  if (!force && getGeometryByID(geometry->id)) {
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
  if (!geometriesById.empty()) {
    if (!stl_util::contains(geometriesById, geometry->id)) {
      return false;
    }
    index = geometriesById[geometry->id];
  }
  else {
    auto it = std::find_if(
      geometries.begin(), geometries.end(),
      [geometry](const GeometryPtr& _geometry) { return _geometry.get() == geometry; });
    if (it == geometries.end()) {
      return false;
    }
    index = static_cast<size_t>(it - geometries.begin());
  }

  std::string id = geometry->id;
  if (index != geometries.size() - 1) {
    auto lastGeometry = geometries.back();
    if (lastGeometry) {
      geometries[index] = lastGeometry;
      if (!geometriesById.empty()) {
        geometriesById[lastGeometry->id] = index;
        geometriesById.erase(id);
      }
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
  auto it = std::find_if(meshes.begin(), meshes.end(),
                         [&id](const AbstractMeshPtr& mesh) { return mesh->id == id; });

  return (it == meshes.end()) ? nullptr : *it;
}

std::vector<AbstractMeshPtr> Scene::getMeshesByID(const std::string& id)
{
  std::vector<AbstractMeshPtr> filteredMeshes;
  std::for_each(meshes.begin(), meshes.end(), [&filteredMeshes, &id](const AbstractMeshPtr& mesh) {
    if (mesh->id == id) {
      filteredMeshes.emplace_back(mesh);
    }
  });
  return filteredMeshes;
}

TransformNodePtr Scene::getTransformNodeByID(const std::string& id)
{
  auto it = std::find_if(
    transformNodes.begin(), transformNodes.end(),
    [&id](const TransformNodePtr& transformNode) { return transformNode->id == id; });

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

std::vector<TransformNodePtr> Scene::getTransformNodesByID(const std::string& id)
{
  std::vector<TransformNodePtr> filteredTransformNodes;
  std::for_each(transformNodes.begin(), transformNodes.end(),
                [&filteredTransformNodes, &id](const TransformNodePtr& transformNode) {
                  if (transformNode->id == id) {
                    filteredTransformNodes.emplace_back(transformNode);
                  }
                });
  return filteredTransformNodes;
}

AbstractMeshPtr Scene::getMeshByUniqueID(size_t uniqueId)
{
  auto it = std::find_if(meshes.begin(), meshes.end(), [&uniqueId](const AbstractMeshPtr& mesh) {
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

NodePtr Scene::getNodeByName(const std::string& name)
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

AbstractMeshPtr Scene::getMeshByName(const std::string& name)
{
  auto it = std::find_if(meshes.begin(), meshes.end(),
                         [&name](const AbstractMeshPtr& mesh) { return mesh->name == name; });

  return (it == meshes.end()) ? nullptr : *it;
}

TransformNodePtr Scene::getTransformNodeByName(const std::string& name)
{
  auto it = std::find_if(
    transformNodes.begin(), transformNodes.end(),
    [&name](const TransformNodePtr& transformNode) { return transformNode->name == name; });

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
    for (auto& soundTrack : soundTracks) {
      for (index = 0; index < soundTrack->soundCollection.size(); ++index) {
        if (soundTrack->soundCollection[index]->name == name) {
          return soundTrack->soundCollection[index];
        }
      }
    }
  }

  return nullptr;
}

SkeletonPtr Scene::getLastSkeletonByID(const std::string& id)
{
  for (size_t index = skeletons.size(); index-- > 0;) {
    if (skeletons[index]->id == id) {
      return skeletons[index];
    }

    if (index == 0) {
      break;
    }
  }

  return nullptr;
}

SkeletonPtr Scene::getSkeletonById(const std::string& id)
{
  auto it = std::find_if(skeletons.begin(), skeletons.end(),
                         [&id](const SkeletonPtr& skeleton) { return skeleton->id == id; });

  return (it == skeletons.end()) ? nullptr : *it;
}

SkeletonPtr Scene::getSkeletonByUniqueID(size_t uniqueId)
{
  auto it
    = std::find_if(skeletons.begin(), skeletons.end(), [uniqueId](const SkeletonPtr& skeleton) {
        return skeleton->uniqueId == uniqueId;
      });

  return (it == skeletons.end()) ? nullptr : *it;
}

SkeletonPtr Scene::getSkeletonByName(const std::string& name)
{
  auto it = std::find_if(skeletons.begin(), skeletons.end(),
                         [&name](const SkeletonPtr& skeleton) { return skeleton->name == name; });

  return (it == skeletons.end()) ? nullptr : *it;
}

MorphTargetManagerPtr Scene::getMorphTargetManagerById(unsigned int id)
{
  auto it = std::find_if(morphTargetManagers.begin(), morphTargetManagers.end(),
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
      if (target && target->id == id) {
        return target;
      }
    }
  }
  return nullptr;
}

MorphTargetPtr Scene::getMorphTargetByName(const std::string& name)
{
  for (const auto& morphTargetManager : morphTargetManagers) {
    for (size_t index = 0; index < morphTargetManager->numTargets(); ++index) {
      const auto target = morphTargetManager->getTarget(index);
      if (target && target->name == name) {
        return target;
      }
    }
  }
  return nullptr;
}

PostProcessPtr Scene::getPostProcessByName(const std::string& name)
{
  auto it = std::find_if(
    postProcesses.begin(), postProcesses.end(),
    [&name](const PostProcessPtr& postProcess) { return postProcess->name == name; });

  return (it == postProcesses.end()) ? nullptr : *it;
}

BaseTexturePtr Scene::getTextureByUniqueID(size_t uniqueId)
{
  auto it
    = std::find_if(textures.begin(), textures.end(), [&uniqueId](const BaseTexturePtr& texture) {
        return texture->uniqueId == uniqueId;
      });

  return (it == textures.end()) ? nullptr : *it;
}

bool Scene::isActiveMesh(const AbstractMeshPtr& mesh)
{
  return std::find_if(_activeMeshes.begin(), _activeMeshes.end(),
                      [&mesh](const AbstractMesh* activeMesh) { return activeMesh == mesh.get(); })
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

void Scene::freeActiveMeshes()
{
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
  if (_renderingManager) {
    _renderingManager->freeRenderingGroups();
  }
  if (!textures.empty()) {
    for (const auto& texture : textures) {
      if (texture) {
        auto renderTargetTexture = std::dynamic_pointer_cast<RenderTargetTexture>(texture);
        if (renderTargetTexture && !renderTargetTexture->renderList().empty()) {
          renderTargetTexture->freeRenderingGroups();
        }
      }
    }
  }
}

void Scene::_evaluateSubMesh(SubMesh* subMesh, AbstractMesh* mesh, AbstractMesh* initialMesh)
{
  if (initialMesh->hasInstances() || initialMesh->isAnInstance()
      || dispatchAllSubMeshesOfActiveMeshes || _skipFrustumClipping
      || mesh->alwaysSelectAsActiveMesh || mesh->subMeshes.size() == 1
      || subMesh->isInFrustum(_frustumPlanes)) {

    for (const auto& step : _evaluateSubMeshStage) {
      step.action(mesh, subMesh);
    }

    auto material = subMesh->getMaterial();
    if (material) {
      // Render targets
      if (material->hasRenderTargetTextures && material->getRenderTargetTextures) {
        if (std::find(_processedMaterials.begin(), _processedMaterials.end(), material)
            == _processedMaterials.end()) {
          _processedMaterials.emplace_back(material);
          for (const auto& renderTarget : material->getRenderTargetTextures()) {
            if (std::find(_renderTargets.begin(), _renderTargets.end(), renderTarget)
                == _renderTargets.end()) {
              _renderTargets.emplace_back(renderTarget);
            }
          }
        }
      }

      // Dispatch
      _renderingManager->dispatch(subMesh, mesh, material);
    }
  }
}

bool Scene::_isInIntermediateRendering() const
{
  return _intermediateRendering;
}

void Scene::setActiveMeshCandidateProvider(IActiveMeshCandidateProvider* provider)
{
  _activeMeshCandidateProvider = provider;
}

IActiveMeshCandidateProvider* Scene::getActiveMeshCandidateProvider() const
{
  return _activeMeshCandidateProvider;
}

Scene& Scene::freezeActiveMeshes(bool skipEvaluateActiveMeshes,
                                 const std::function<void()>& onSuccess,
                                 const std::function<void(const std::string& message)> onError)
{
  if (!_activeCamera) {
    if (onError) {
      onError("No active camera found");
    }
    return *this;
  }

  if (!_frustumPlanesSet) {
    setTransformMatrix(_activeCamera->getViewMatrix(), _activeCamera->getProjectionMatrix());
  }

  _evaluateActiveMeshes();
  _activeMeshesFrozen                 = true;
  _skipEvaluateActiveMeshesCompletely = skipEvaluateActiveMeshes;

  for (const auto& activeMesh : _activeMeshes) {
    activeMesh->_freeze();
  }
  if (onSuccess) {
    onSuccess();
  }

  return *this;
}

Scene& Scene::unfreezeActiveMeshes()
{
  for (const auto& mesh : meshes) {
    /* if (mesh->_internalAbstractMeshDataInfo) */ {
      mesh->_internalAbstractMeshDataInfo._isActive = false;
    }
  }

  for (const auto& activeMesh : _activeMeshes) {
    activeMesh->_unFreeze();
  }

  _activeMeshesFrozen = false;
  return *this;
}

void Scene::_evaluateActiveMeshes()
{
  if (_activeMeshesFrozen && !_activeMeshes.empty()) {

    if (!_skipEvaluateActiveMeshesCompletely) {
      for (const auto& mesh : _activeMeshes) {
        mesh->computeWorldMatrix();
      }
    }

    if (!_activeParticleSystems.empty()) {
      for (const auto& particleSystem : _activeParticleSystems) {
        particleSystem->animate();
      }
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

    if (!mesh->isReady() || !mesh->isEnabled() || mesh->scaling().lengthSquared() == 0.f) {
      continue;
    }

    mesh->computeWorldMatrix();

    // Intersections
    if (mesh->actionManager
        && mesh->actionManager->hasSpecificTriggers2(ActionManager::OnIntersectionEnterTrigger,
                                                     ActionManager::OnIntersectionExitTrigger)) {
      if (std::find(_meshesForIntersections.begin(), _meshesForIntersections.end(), mesh)
          == _meshesForIntersections.end()) {
        _meshesForIntersections.emplace_back(mesh);
      }
    }

    // Switch to current LOD
    auto meshLOD = mesh->getLOD(_activeCamera);
    if (!meshLOD) {
      continue;
    }

    mesh->_preActivate();

    if (mesh->isVisible && mesh->visibility() > 0.f
        && (mesh->alwaysSelectAsActiveMesh
            || ((mesh->layerMask & _activeCamera->layerMask) != 0
                && mesh->isInFrustum(_frustumPlanes)))) {
      _activeMeshes.emplace_back(mesh);
      _activeCamera->_activeMeshes.emplace_back(_activeMeshes.back());

      mesh->_activate(_renderId, false);
      if (meshLOD != mesh) {
        meshLOD->_activate(_renderId, false);
      }

      for (const auto& step : _preActiveMeshStage) {
        step.action(mesh);
      }

      _activeMesh(mesh, meshLOD);
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
    if (std::find(_activeSkeletons.begin(), _activeSkeletons.end(), mesh->skeleton())
        == _activeSkeletons.end()) {
      _activeSkeletons.emplace_back(mesh->skeleton());
      mesh->skeleton()->prepare();
    }

    if (!mesh->computeBonesUsingShaders()) {
      if (auto _mesh = static_cast<Mesh*>(mesh)) {
        if (std::find(_softwareSkinnedMeshes.begin(), _softwareSkinnedMeshes.end(), _mesh)
            == _softwareSkinnedMeshes.end()) {
          _softwareSkinnedMeshes.emplace_back(_mesh);
        }
      }
    }
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
  auto activeCamera_ = activeCamera();
  if (activeCamera_ && activeCamera_->_multiviewTexture) {
    activeCamera_->_multiviewTexture->_bindFrameBuffer();
  }
  else if (activeCamera_ && activeCamera_->outputRenderTarget) {
    const auto useMultiview = getEngine()->getCaps().multiview && activeCamera_->outputRenderTarget
                              && activeCamera_->outputRenderTarget->getViewCount() > 1;
    if (useMultiview) {
      activeCamera_->outputRenderTarget->_bindFrameBuffer();
    }
    else {
      const auto internalTexture = activeCamera_->outputRenderTarget->getInternalTexture();
      if (internalTexture) {
        getEngine()->bindFramebuffer(internalTexture);
      }
      else {
        BABYLON_LOG_ERROR("Scene", "Camera contains invalid customDefaultRenderTarget")
      }
    }
  }
  else {
    getEngine()->restoreDefaultFramebuffer(); // Restore back buffer if needed
  }
}

void Scene::updateAlternateTransformMatrix(Camera* alternateCamera)
{
  _setAlternateTransformMatrix(alternateCamera->getViewMatrix(),
                               alternateCamera->getProjectionMatrix());
}

void Scene::_renderForCamera(const CameraPtr& camera, const CameraPtr& rigParent)
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

  const auto useMultiview = getEngine()->getCaps().multiview && camera->outputRenderTarget
                            && camera->outputRenderTarget->getViewCount() > 1;
  if (useMultiview) {
    setTransformMatrix(
      camera->_rigCameras[0]->getViewMatrix(), camera->_rigCameras[0]->getProjectionMatrix(),
      camera->_rigCameras[1]->getViewMatrix(), camera->_rigCameras[1]->getProjectionMatrix());
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
    stl_util::concat_with_no_duplicates(_renderTargets, camera->customRenderTargets);
  }

  if (rigParent && !rigParent->customRenderTargets.empty()) {
    stl_util::concat_with_no_duplicates(_renderTargets, rigParent->customRenderTargets);
  }

  // Collects render targets from external components.
  for (const auto& step : _gatherActiveCameraRenderTargetsStage) {
    step.action(_renderTargets);
  }

  auto needRebind = false;
  if (renderTargetsEnabled) {
    _intermediateRendering = true;

    if (!_renderTargets.empty()) {
      Tools::StartPerformanceCounter("Render targets", !_renderTargets.empty());
      for (const auto& renderTarget : _renderTargets) {
        if (renderTarget->_shouldRender()) {
          ++_renderId;
          const auto hasSpecialRenderTargetCamera
            = renderTarget->activeCamera && renderTarget->activeCamera != _activeCamera;
          renderTarget->render(hasSpecialRenderTargetCamera, dumpNextRenderTargets);
          needRebind = true;
        }
      }
      Tools::EndPerformanceCounter("Render targets", !_renderTargets.empty());

      ++_renderId;
    }

    for (const auto& step : _cameraDrawRenderTargetStage) {
      needRebind = step.action(_activeCamera.get());
    }

    _intermediateRendering = false;

    // Need to bind if sub-camera has an outputRenderTarget eg. for webXR
    if (activeCamera() && activeCamera()->outputRenderTarget) {
      needRebind = true;
    }
  }

  // Restore framebuffer after rendering to targets
  if (needRebind && !prePass) {
    _bindFrameBuffer();
  }

  onAfterRenderTargetsRenderObservable.notifyObservers(this);

  // Prepare Frame
  if (postProcessManager && !camera->_multiviewTexture && !prePass) {
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

    // if the camera has an output render target, render the post process to the render target
    const auto texture
      = camera->outputRenderTarget ? camera->outputRenderTarget->getInternalTexture() : nullptr;
    postProcessManager->_finalizeFrame(camera->isIntermediate, texture);
  }

  // Reset some special arrays
  _renderTargets.clear();

  onAfterCameraRenderObservable.notifyObservers(_activeCamera.get());
}

void Scene::_processSubCameras(const CameraPtr& camera)
{
  if (camera->cameraRigMode == Camera::RIG_MODE_NONE
      || (camera->outputRenderTarget && camera->outputRenderTarget->getViewCount() > 1
          && getEngine()->getCaps().multiview)) {
    _renderForCamera(camera);
    onAfterRenderCameraObservable.notifyObservers(camera.get());
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

  _activeCamera = camera;
  setTransformMatrix(_activeCamera->getViewMatrix(), _activeCamera->getProjectionMatrix());
  onAfterRenderCameraObservable.notifyObservers(camera.get());
}

void Scene::_checkIntersections()
{
}

void Scene::animate()
{
  if (_engine->isDeterministicLockStep()) {
    auto iDeltaTime = (std::max(static_cast<float>(Scene::MinDeltaTime.count()),
                                std::min(_engine->getDeltaTime() * 1000.f,
                                         static_cast<float>(Scene::MaxDeltaTime.count())))
                       / 1000.f)
                      + _timeAccumulator;

    auto defaultFrameTime = _engine->getTimeStep();
    auto defaultFPS       = (1000.f / defaultFrameTime) / 1000.f;

    auto stepsTaken = 0u;

    auto maxSubSteps = _engine->getLockstepMaxSteps();

    // compute the amount of fixed steps we should have taken since the last step
    auto internalSteps = static_cast<unsigned int>(std::floor(iDeltaTime / defaultFrameTime));
    internalSteps      = std::min(internalSteps, maxSubSteps);

    while (iDeltaTime > 0.f && stepsTaken < internalSteps) {
      onBeforeStepObservable.notifyObservers(this);

      // Animations
      _animationRatio = defaultFrameTime * defaultFPS;
      _animate();
      onAfterAnimationsObservable.notifyObservers(this);

      // Physics
      if (physicsEnabled) {
        _advancePhysicsEngineStep(defaultFrameTime);
      }

      onAfterStepObservable.notifyObservers(this);
      ++_currentStepId;

      ++stepsTaken;
      iDeltaTime -= defaultFrameTime;
    }

    _timeAccumulator = iDeltaTime < 0.f ? 0.f : iDeltaTime;
  }
  else {
    // Animations
    const float iDeltaTime
      = useConstantAnimationDeltaTime ?
          16.f :
          std::max(Time::fpMillisecondsDuration<float>(Scene::MinDeltaTime),
                   std::min(_engine->getDeltaTime(),
                            Time::fpMillisecondsDuration<float>(Scene::MaxDeltaTime)));
    _animationRatio = iDeltaTime * (60.f / 1000.f);
    _animate();
    onAfterAnimationsObservable.notifyObservers(this);

    // Physics
    if (physicsEnabled) {
      _advancePhysicsEngineStep(iDeltaTime);
    }
  }
}

void Scene::render(bool updateCameras, bool ignoreAnimations)
{
  if (isDisposed()) {
    return;
  }

  if (onReadyObservable.hasObservers() && _executeWhenReadyTimeoutId == -1) {
    _checkIsReady();
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
    actionManager->processTrigger(ActionManager::OnEveryFrameTrigger);
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
  auto engine              = getEngine();
  auto currentActiveCamera = _activeCamera;
  if (renderTargetsEnabled) {
    Tools::StartPerformanceCounter("Custom render targets", !customRenderTargets.empty());
    _intermediateRendering = true;
    for (const auto& renderTarget : customRenderTargets) {
      if (renderTarget->_shouldRender()) {
        ++_renderId;

        _activeCamera = renderTarget->activeCamera ? renderTarget->activeCamera : _activeCamera;

        if (!_activeCamera) {
          BABYLON_LOG_ERROR("Scene", "Active camera not set")
          return;
        }

        // Viewport
        engine->setViewport(_activeCamera->viewport);

        // Camera
        updateTransformMatrix();

        renderTarget->render(currentActiveCamera != _activeCamera, dumpNextRenderTargets);
      }
    }
    Tools::EndPerformanceCounter("Custom render targets", !customRenderTargets.empty());
    _intermediateRendering = false;
    ++_renderId;
  }

  // Restore back buffer
  activeCamera = currentActiveCamera;
  if (_activeCamera && _activeCamera->cameraRigMode != Camera::RIG_MODE_CUSTOM && !prePass) {
    _bindFrameBuffer();
  }
  onAfterRenderTargetsRenderObservable.notifyObservers(this);

  for (const auto& step : _beforeClearStage) {
    step.action();
  }

  // Clear
  if ((autoClearDepthAndStencil || autoClear) && !prePass) {
    _engine->clear(clearColor, autoClear || forceWireframe() || forcePointsCloud(),
                   autoClearDepthAndStencil, autoClearDepthAndStencil);
  }

  // Collects render targets from external components.
  for (const auto& step : _gatherRenderTargetsStage) {
    step.action(_renderTargets);
  }

  // Multi-cameras?
  if (!activeCameras.empty()) {
    for (auto cameraIndex = 0u; cameraIndex < activeCameras.size(); ++cameraIndex) {
      if (cameraIndex > 0) {
        _engine->clear(std::nullopt, false, true, true);
      }

      _processSubCameras(activeCameras[cameraIndex]);
    }
  }
  else {
    if (!_activeCamera) {
      throw std::runtime_error("No camera defined");
    }

    _processSubCameras(_activeCamera);
  }

  // Intersection checks
  _checkIntersections();

  // Executes the after render stage actions.
  for (const auto& step : _afterRenderStage) {
    step.action();
  }

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

DepthRendererPtr Scene::enableDepthRenderer(const CameraPtr& camera, bool storeNonLinearDepth,
                                            bool force32bitsFloat)
{
  auto _camera = camera ? camera : _activeCamera;
  if (!_camera) {
    throw std::runtime_error("No camera available to enable depth renderer");
  }
  if (!stl_util::contains(_depthRenderer, _camera->id) || !_depthRenderer[_camera->id]) {
    const auto supportFullfloat = getEngine()->getCaps().textureFloatRender;
    auto textureType            = 0u;
    if (_engine->getCaps().textureHalfFloatRender && (!force32bitsFloat || !supportFullfloat)) {
      textureType = Constants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (supportFullfloat) {
      textureType = Constants::TEXTURETYPE_FLOAT;
    }
    else {
      textureType = Constants::TEXTURETYPE_UNSIGNED_BYTE;
    }
    _depthRenderer[_camera->id]
      = std::make_shared<DepthRenderer>(this, textureType, _camera, storeNonLinearDepth);
  }

  return _depthRenderer[_camera->id];
}

void Scene::disableDepthRenderer(const CameraPtr& camera)
{
  auto _camera = camera ? camera : _activeCamera;
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

  _geometryBufferRenderer = std::make_shared<GeometryBufferRenderer>(this, ratio);
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

PrePassRendererPtr& Scene::enablePrePassRenderer()
{
  if (_prePassRenderer) {
    return _prePassRenderer;
  }

  _prePassRenderer = std::make_shared<PrePassRenderer>(this);

  if (!_prePassRenderer->isSupported()) {
    _prePassRenderer = nullptr;
    BABYLON_LOG_ERROR("Scene",
                      "PrePassRenderer needs WebGL 2 support.\nMaybe you tried to use the "
                      "following features that need the PrePassRenderer :\nSubsurface Scattering");
  }

  return _prePassRenderer;
}

void Scene::disablePrePassRenderer()
{
  if (!_prePassRenderer) {
    return;
  }

  _prePassRenderer->dispose();
  _prePassRenderer = nullptr;
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
  _preActiveMeshStage.clear();
  _cameraDrawRenderTargetStage.clear();
  _beforeCameraDrawStage.clear();
  _beforeRenderingGroupDrawStage.clear();
  _beforeRenderingMeshStage.clear();
  _afterRenderingMeshStage.clear();
  _afterRenderingGroupDrawStage.clear();
  _afterCameraDrawStage.clear();
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
    _activeCamera = nullptr;
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
  onNewMaterialAddedObservable.clear();
  onMaterialRemovedObservable.clear();
  onNewTextureAddedObservable.clear();
  onTextureRemovedObservable.clear();
  onPrePointerObservable.clear();
  onPointerObservable.clear();
  onPreKeyboardObservable.clear();
  onKeyboardObservable.clear();

  detachControl();

  // Detach cameras
  auto canvas = _engine->getInputElement();
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
    removeTransformNode(transformNode);
  }

  // Release cameras
  for (const auto& camera : cameras) {
    camera->dispose();
  }

  // Release materials
  if (defaultMaterial()) {
    defaultMaterial()->dispose();
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

  if (_alternateSceneUbo) {
    _alternateSceneUbo->dispose();
  }

  // Post-processes
  postProcessManager->dispose();

  // Remove from engine
  _engine->scenes.erase(std::remove(_engine->scenes.begin(), _engine->scenes.end(), this),
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
    markAllMaterialsAsDirty(Material::AllDirtyFlag);
  }
}

SubSurfaceConfigurationPtr& Scene::get_subSurfaceConfiguration()
{
  return _subSurfaceConfiguration;
}

void Scene::set_subSurfaceConfiguration(const SubSurfaceConfigurationPtr& value)
{
  if (value) {
    if (enablePrePassRenderer()) {
      _subSurfaceConfiguration = value;
    }
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
MinMax
Scene::getWorldExtends(const std::function<bool(const AbstractMeshPtr& mesh)>& filterPredicate)
{
  Vector3 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max());
  Vector3 max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(),
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

    Vector3::CheckExtends(minBox, min, max);
    Vector3::CheckExtends(maxBox, min, max);
  }

  return {min, max};
}

Octree<AbstractMesh*>* Scene::createOrUpdateSelectionOctree(size_t maxCapacity, size_t maxDepth)
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
Ray Scene::createPickingRay(int x, int y, Matrix& world, const CameraPtr& camera,
                            bool cameraViewSpace)
{
  auto result = Ray::Zero();

  createPickingRayToRef(x, y, world, result, camera, cameraViewSpace);

  return result;
}

Scene& Scene::createPickingRayToRef(int x, int y, const std::optional<Matrix>& world, Ray& result,
                                    CameraPtr camera, bool cameraViewSpace)
{
  auto engine = _engine;

  if (!camera) {
    if (!_activeCamera) {
      return *this;
    }

    camera = _activeCamera;
  }

  auto& cameraViewport = camera->viewport;
  auto viewport = cameraViewport.toGlobal(engine->getRenderWidth(), engine->getRenderHeight());

  auto identity = Matrix::Identity();

  // Moving coordinates to local viewport world
  auto _x = static_cast<float>(x);
  _x /= static_cast<float>(_engine->getHardwareScalingLevel() - viewport.x);
  auto _y = static_cast<float>(y);
  _y /= static_cast<float>(_engine->getHardwareScalingLevel()
                           - (_engine->getRenderHeight() - viewport.y - viewport.height));

  auto _world = world.has_value() ? *world : identity;
  result.update(_x, _y, static_cast<float>(viewport.width), static_cast<float>(viewport.height),
                _world, cameraViewSpace ? identity : camera->getViewMatrix(),
                camera->getProjectionMatrix());
  return *this;
}

Ray Scene::createPickingRayInCameraSpace(int x, int y, const CameraPtr& camera)
{
  auto result = Ray::Zero();

  createPickingRayInCameraSpaceToRef(x, y, result, camera);

  return result;
}

Scene& Scene::createPickingRayInCameraSpaceToRef(int x, int y, Ray& result, CameraPtr camera)
{
  auto engine = _engine;

  if (!camera) {
    if (!_activeCamera) {
      BABYLON_LOG_ERROR("Scene", "Active camera not set")
      return *this;
    }

    camera = _activeCamera;
  }

  auto& cameraViewport = camera->viewport;
  auto viewport = cameraViewport.toGlobal(engine->getRenderWidth(), engine->getRenderHeight());
  auto identity = Matrix::Identity();

  // Moving coordinates to local viewport world
  auto _x = static_cast<float>(x);
  _x /= static_cast<float>(_engine->getHardwareScalingLevel() - viewport.x);
  auto _y = static_cast<float>(y);
  _y /= static_cast<float>(_engine->getHardwareScalingLevel()
                           - (_engine->getRenderHeight() - viewport.y - viewport.height));
  result.update(_x, _y, static_cast<float>(viewport.width), static_cast<float>(viewport.height),
                identity, identity, camera->getProjectionMatrix());
  return *this;
}

std::optional<PickingInfo> Scene::_internalPickForMesh(
  const std::optional<PickingInfo>& pickingInfo,
  const std::function<Ray(Matrix& world)>& rayFunction, const AbstractMeshPtr& mesh, Matrix& world,
  const std::optional<bool>& fastCheck, const std::optional<bool>& onlyBoundingInfo,
  const TrianglePickingPredicate& trianglePredicate, const std::optional<bool>& skipBoundingInfo)
{
  auto ray = rayFunction(world);

  auto result = mesh->intersects(ray, fastCheck, trianglePredicate, onlyBoundingInfo, world,
                                 skipBoundingInfo.value_or(false));
  if (/*!result || */ !result.hit) {
    return std::nullopt;
  }

  if (!fastCheck && /*pickingInfo != null && */ result.distance >= pickingInfo->distance) {
    return std::nullopt;
  }

  return result;
}

std::optional<PickingInfo>
Scene::_internalPick(const std::function<Ray(Matrix& world)>& rayFunction,
                     const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
                     const std::optional<bool>& iFastCheck,
                     const std::optional<bool>& onlyBoundingInfo,
                     const TrianglePickingPredicate& trianglePredicate)
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

    auto world = mesh->skeleton() && mesh->skeleton()->overrideMesh ?
                   mesh->skeleton()->overrideMesh->getWorldMatrix() :
                   mesh->getWorldMatrix();

    auto _mesh = std::static_pointer_cast<Mesh>(mesh);

    if (mesh->hasThinInstances() && _mesh && _mesh->thinInstanceEnablePicking) {
      // first check if the ray intersects the whole bounding box/sphere of the mesh
      auto result = _internalPickForMesh(pickingInfo, rayFunction, mesh, world, true, true,
                                         trianglePredicate);
      if (result) {
        if (onlyBoundingInfo.value_or(false)) {
          // the user only asked for a bounding info check so we can return
          return pickingInfo;
        }
        auto& tmpMatrix   = TmpVectors::MatrixArray[1];
        auto thinMatrices = _mesh->thinInstanceGetWorldMatrices();
        for (size_t index = 0; index < thinMatrices.size(); ++index) {
          auto& thinMatrix = thinMatrices[index];
          thinMatrix.multiplyToRef(world, tmpMatrix);
          auto iResult = _internalPickForMesh(pickingInfo, rayFunction, mesh, tmpMatrix, iFastCheck,
                                              onlyBoundingInfo, trianglePredicate, true);

          if (iResult) {
            pickingInfo                    = iResult;
            pickingInfo->thinInstanceIndex = static_cast<int>(index);

            if (iFastCheck.value_or(false)) {
              return pickingInfo;
            }
          }
        }
      }
    }
    else {
      const auto result = _internalPickForMesh(pickingInfo, rayFunction, mesh, world, iFastCheck,
                                               onlyBoundingInfo, trianglePredicate);

      if (result) {
        pickingInfo = result;

        if (iFastCheck.value_or(false)) {
          return pickingInfo;
        }
      }
    }
  }

  return pickingInfo.value_or(PickingInfo());
}

std::vector<std::optional<PickingInfo>>
Scene::_internalMultiPick(const std::function<Ray(Matrix& world)>& rayFunction,
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

    auto world = mesh->skeleton() && mesh->skeleton()->overrideMesh ?
                   mesh->skeleton()->overrideMesh->getWorldMatrix() :
                   mesh->getWorldMatrix();

    auto _mesh = std::static_pointer_cast<Mesh>(mesh);
    if (mesh->hasThinInstances() && _mesh && _mesh->thinInstanceEnablePicking) {
      auto result = _internalPickForMesh(std::nullopt, rayFunction, mesh, world, true, true,
                                         trianglePredicate);
      if (result) {
        auto& tmpMatrix   = TmpVectors::MatrixArray[1];
        auto thinMatrices = _mesh->thinInstanceGetWorldMatrices();
        for (size_t index = 0; index < thinMatrices.size(); ++index) {
          auto& thinMatrix = thinMatrices[index];
          thinMatrix.multiplyToRef(world, tmpMatrix);
          auto iResult = _internalPickForMesh(std::nullopt, rayFunction, mesh, tmpMatrix, false,
                                              false, trianglePredicate, true);

          if (iResult) {
            iResult->thinInstanceIndex = static_cast<int>(index);
            pickingInfos.emplace_back(iResult);
          }
        }
      }
    }
    else {
      auto result = _internalPickForMesh(std::nullopt, rayFunction, mesh, world, false, false,
                                         trianglePredicate);

      if (result) {
        pickingInfos.emplace_back(result);
      }
    }
  }

  return pickingInfos;
}

std::optional<PickingInfo>
Scene::_internalPickSprites(const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate,
                            bool fastCheck, CameraPtr camera)
{
  std::optional<PickingInfo> pickingInfo = std::nullopt;

  if (!camera) {
    if (!_activeCamera) {
      return std::nullopt;
    }
    camera = _activeCamera;
  }

  if (!spriteManagers.empty()) {
    for (const auto& spriteManager : spriteManagers) {
      if (!spriteManager->isPickable) {
        continue;
      }

      auto result = spriteManager->intersects(ray, camera, predicate, fastCheck);
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
Scene::pickWithBoundingInfo(int x, int y,
                            const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
                            bool fastCheck, const CameraPtr& camera)
{
  auto result = _internalPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      if (!_tempPickingRay) {
        _tempPickingRay = std::make_unique<Ray>(Ray::Zero());
      }

      createPickingRayToRef(x, y, world, *_tempPickingRay, camera ? camera : nullptr);
      return *_tempPickingRay;
    },
    predicate, fastCheck, true);
  if (result) {
    auto world  = Matrix::Identity();
    result->ray = createPickingRay(x, y, world, camera ? camera : nullptr);
  }
  return result;
}

std::optional<PickingInfo>
Scene::pick(int x, int y, const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
            bool fastCheck, const CameraPtr& camera,
            const TrianglePickingPredicate& trianglePredicate)
{
  auto result = _internalPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      createPickingRayToRef(x, y, world, *_tempPickingRay, camera);
      return *_tempPickingRay;
    },
    predicate, fastCheck, false, trianglePredicate);
  if (result) {
    auto _result     = *result;
    auto identityMat = Matrix::Identity();
    _result.ray      = createPickingRay(x, y, identityMat, camera ? camera : nullptr);
    result           = _result;
  }
  return result;
}

std::optional<PickingInfo> Scene::pickSprite(int x, int y,
                                             const std::function<bool(Sprite* sprite)>& predicate,
                                             bool fastCheck, const CameraPtr& camera)
{
  if (!_tempSpritePickingRay) {
    return std::nullopt;
  }

  createPickingRayInCameraSpaceToRef(x, y, *_tempPickingRay, camera);

  return _internalPickSprites(*_tempPickingRay, predicate, fastCheck, camera);
}

std::optional<PickingInfo>
Scene::pickSpriteWithRay(const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate,
                         bool fastCheck, CameraPtr& camera)
{
  if (!_tempSpritePickingRay) {
    return std::nullopt;
  }

  if (!camera) {
    if (!_activeCamera) {
      return std::nullopt;
    }
    camera = _activeCamera;
  }

  Ray::TransformToRef(ray, camera->getViewMatrix(), *_tempSpritePickingRay);

  return _internalPickSprites(*_tempSpritePickingRay, predicate, fastCheck, camera);
}

std::vector<PickingInfo> Scene::_internalMultiPickSprites(
  const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate, CameraPtr& camera)
{
  std::vector<PickingInfo> pickingInfos;

  if (!camera) {
    if (!activeCamera()) {
      return pickingInfos;
    }
    camera = activeCamera();
  }

  if (!spriteManagers.empty()) {
    for (const auto& spriteManager : spriteManagers) {
      if (!spriteManager->isPickable) {
        continue;
      }

      auto results = spriteManager->multiIntersects(ray, camera, predicate);

      if (!results.empty()) {
        pickingInfos = stl_util::concat(pickingInfos, results);
      }
    }
  }

  return pickingInfos;
}

std::vector<PickingInfo>
Scene::multiPickSprite(int x, int y, const std::function<bool(Sprite* sprite)>& predicate,
                       CameraPtr& camera)
{
  createPickingRayInCameraSpaceToRef(x, y, *_tempSpritePickingRay, camera);

  return _internalMultiPickSprites(*_tempSpritePickingRay, predicate, camera);
}

std::vector<PickingInfo>
Scene::multiPickSpriteWithRay(const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate,
                              CameraPtr& camera)
{
  std::vector<PickingInfo> pickingInfos;

  if (!_tempSpritePickingRay) {
    return pickingInfos;
  }

  if (!camera) {
    if (!activeCamera()) {
      return pickingInfos;
    }
    camera = activeCamera();
  }

  Ray::TransformToRef(ray, camera->getViewMatrix(), *_tempSpritePickingRay);

  return _internalMultiPickSprites(*_tempSpritePickingRay, predicate, camera);
}

std::optional<PickingInfo>
Scene::pickWithRay(const Ray& ray,
                   const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
                   bool fastCheck, const TrianglePickingPredicate& trianglePredicate)
{
  auto result = _internalPick(
    [this, &ray](Matrix& world) -> Ray {
      if (!_pickWithRayInverseMatrix) {
        _pickWithRayInverseMatrix = std::make_unique<Matrix>(Matrix::Identity());
      }
      world.invertToRef(*_pickWithRayInverseMatrix);

      if (!_cachedRayForTransform) {
        _cachedRayForTransform = std::make_unique<Ray>(Ray::Zero());
      }

      Ray::TransformToRef(ray, *_pickWithRayInverseMatrix, *_cachedRayForTransform);
      return *_cachedRayForTransform;
    },
    predicate, fastCheck, false, trianglePredicate);
  if (result) {
    auto _result = *result;
    _result.ray  = ray;
    result       = _result;
  }
  return result;
}

std::vector<std::optional<PickingInfo>>
Scene::multiPick(int x, int y, const std::function<bool(AbstractMesh* mesh)>& predicate,
                 const CameraPtr& camera)
{
  return _internalMultiPick(
    [this, x, y, &camera](Matrix& world) -> Ray { return createPickingRay(x, y, world, camera); },
    predicate);
}

std::vector<std::optional<PickingInfo>>
Scene::multiPickWithRay(const Ray& ray, const std::function<bool(AbstractMesh* mesh)>& predicate)
{
  return _internalMultiPick(
    [this, &ray](Matrix& world) -> Ray {
      if (!_pickWithRayInverseMatrix) {
        _pickWithRayInverseMatrix = std::make_unique<Matrix>(Matrix::Identity());
      }
      world.invertToRef(*_pickWithRayInverseMatrix);

      if (!_cachedRayForTransform) {
        _cachedRayForTransform = std::make_unique<Ray>(Ray::Zero());
      }

      Ray::TransformToRef(ray, *_pickWithRayInverseMatrix, *_cachedRayForTransform);
      return *_cachedRayForTransform;
    },
    predicate);
}

AbstractMeshPtr& Scene::getPointerOverMesh()
{
  return _pointerOverMesh;
}

void Scene::setPointerOverMesh(AbstractMesh* mesh, const std::optional<int>& /*pointerId*/)
{
  if (_pointerOverMesh.get() == mesh) {
    return;
  }
#if 0
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
#endif
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

bool Scene::enablePhysics(const std::optional<Vector3>& iGravity, IPhysicsEnginePlugin* plugin)
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

  _physicsEngine          = PhysicsEngine::New(iGravity, plugin);
  _physicsTimeAccumulator = 0.f;
  bool isInitialized      = _physicsEngine->isInitialized();
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
    auto subTime = _physicsEngine->getSubTimeStep();
    if (subTime > 0.f) {
      _physicsTimeAccumulator += step;
      while (_physicsTimeAccumulator > subTime) {
        onBeforePhysicsObservable.notifyObservers(this);
        _physicsEngine->_step(subTime / 1000.f);
        onAfterPhysicsObservable.notifyObservers(this);
        _physicsTimeAccumulator -= subTime;
      }
    }
    else {
      onBeforePhysicsObservable.notifyObservers(this);
      _physicsEngine->_step(step / 1000.f);
      onAfterPhysicsObservable.notifyObservers(this);
    }
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

  markAllMaterialsAsDirty(Material::TextureDirtyFlag);
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

void Scene::createDefaultCamera(bool createArcRotateCamera, bool replace, bool attachCameraControls)
{
  // Dispose existing camera in replace mode.
  if (replace) {
    if (_activeCamera) {
      _activeCamera->dispose();
      _activeCamera = nullptr;
    }
  }

  // Camera
  if (!_activeCamera) {
    auto worldExtends = getWorldExtends(
      [](const AbstractMeshPtr& mesh) -> bool { return mesh->isVisible && mesh->isEnabled(); });
    auto worldSize   = worldExtends.max.subtract(worldExtends.min);
    auto worldCenter = worldExtends.min.add(worldSize.scale(0.5f));

    TargetCameraPtr camera = nullptr;
    auto radius            = worldSize.length() * 1.5f;
    // empty scene scenario!
    if (std::isinf(radius)) {
      radius = 1.f;
      worldCenter.copyFromFloats(0.f, 0.f, 0.f);
    }
    if (createArcRotateCamera) {
      auto arcRotateCamera = ArcRotateCamera::New("default camera", -Math::PI_2, Math::PI_2, radius,
                                                  worldCenter, this);
      arcRotateCamera->lowerRadiusLimit = radius * 0.01f;
      arcRotateCamera->wheelPrecision   = 100.f / radius;
      camera                            = arcRotateCamera;
    }
    else {
      auto freeCamera
        = FreeCamera::New("default camera", Vector3(worldCenter.x, worldCenter.y, -radius), this);
      freeCamera->setTarget(worldCenter);
      camera = freeCamera;
    }
    camera->minZ  = radius * 0.01f;
    camera->maxZ  = radius * 1000.f;
    camera->speed = radius * 0.2f;
    _activeCamera = camera;

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

MeshPtr Scene::createDefaultSkybox(BaseTexturePtr iEnvironmentTexture, bool pbr, float scale,
                                   float blur, bool setGlobalEnvTexture)
{
  if (!iEnvironmentTexture) {
    BABYLON_LOG_WARN("Scene", "Can not create default skybox without environment texture.")
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
    hdrSkyboxMaterial->reflectionTexture = iEnvironmentTexture;
    if (hdrSkyboxMaterial->reflectionTexture()) {
      hdrSkyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
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
    skyboxMaterial->reflectionTexture = iEnvironmentTexture;
    if (skyboxMaterial->reflectionTexture()) {
      skyboxMaterial->reflectionTexture()->coordinatesMode = TextureConstants::SKYBOX_MODE;
    }
    skyboxMaterial->disableLighting = true;
    hdrSkybox->infiniteDistance     = true;
    hdrSkybox->material             = skyboxMaterial;
  }

  hdrSkybox->isPickable = false;
  return hdrSkybox;
}

std::unique_ptr<EnvironmentHelper>
Scene::createDefaultEnvironment(const std::optional<IEnvironmentHelperOptions>& options)
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

std::vector<TransformNode*> Scene::getTransformNodesByTags()
{
  return std::vector<TransformNode*>();
}

void Scene::setRenderingOrder(
  unsigned int renderingGroupId,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& transparentSortCompareFn)
{
  _renderingManager->setRenderingOrder(renderingGroupId, opaqueSortCompareFn,
                                       alphaTestSortCompareFn, transparentSortCompareFn);
}

void Scene::setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                              bool autoClearDepthStencil, bool depth, bool stencil)
{
  _renderingManager->setRenderingAutoClearDepthStencil(renderingGroupId, autoClearDepthStencil,
                                                       depth, stencil);
}

std::optional<IRenderingManagerAutoClearSetup> Scene::getAutoClearDepthStencilSetup(size_t index)
{
  return _renderingManager->getAutoClearDepthStencilSetup(index);
}

void Scene::markAllMaterialsAsDirty(unsigned int flag,
                                    const std::function<bool(Material* mat)>& predicate)
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

IFileRequest Scene::_loadFile(
  const std::string& /*url*/,
  const std::function<void(const std::variant<std::string, ArrayBufferView>& /*data*/,
                           const std::string&)>& /*onSuccess*/)
{
  return IFileRequest();
}

std::variant<std::string, ArrayBuffer>
Scene::_loadFileAsync(const std::string& /*url*/, const std::optional<bool>& /*useDatabase*/,
                      const std::optional<bool>& /*useArrayBuffer*/)
{
  return ArrayBuffer();
}

void Scene::_createMultiviewUbo()
{
  _multiviewSceneUbo = std::make_unique<UniformBuffer>(getEngine(), Float32Array(), true);
  _multiviewSceneUbo->addUniform("viewProjection", 16);
  _multiviewSceneUbo->addUniform("viewProjectionR", 16);
  _multiviewSceneUbo->addUniform("view", 16);
}

void Scene::_updateMultiviewUbo(std::optional<Matrix> viewR, std::optional<Matrix> projectionR)
{
  if (viewR && projectionR) {
    viewR->multiplyToRef(*projectionR, _transformMatrixR);
  }

  if (viewR && projectionR) {
    viewR->multiplyToRef(*projectionR, TmpVectors::MatrixArray[0]);
    Frustum::GetRightPlaneToRef(
      TmpVectors::MatrixArray[0],
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
    (camera->_rigPostProcess && camera->_rigPostProcess && camera->_rigPostProcess->width > 0) ?
      camera->_rigPostProcess->width / 2 :
      getEngine()->getRenderWidth(true) / 2,
    (camera->_rigPostProcess && camera->_rigPostProcess && camera->_rigPostProcess->height > 0) ?
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
    _activeCamera = camera->_rigCameras[index];
    engine->setViewport(_activeCamera->viewport);
    if (postProcessManager) {
      postProcessManager->_prepareFrame();
      postProcessManager->_finalizeFrame(_activeCamera->isIntermediate);
    }
  }
}

SubSurfaceConfigurationPtr Scene::enableSubSurfaceForPrePass()
{
  if (_subSurfaceConfiguration) {
    return _subSurfaceConfiguration;
  }

  const auto prePassRenderer = enablePrePassRenderer();
  if (prePassRenderer) {
    _subSurfaceConfiguration = std::make_shared<SubSurfaceConfiguration>(this);
    prePassRenderer->addEffectConfiguration(_subSurfaceConfiguration);
    return _subSurfaceConfiguration;
  }

  return nullptr;
}

void Scene::disableSubSurfaceForPrePass()
{
  if (!_subSurfaceConfiguration) {
    return;
  }

  _subSurfaceConfiguration->dispose();
  _subSurfaceConfiguration = nullptr;
}

} // end of namespace BABYLON
