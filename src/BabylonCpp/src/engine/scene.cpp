#include <babylon/engine/scene.h>

#include <babylon/actions/action_event.h>
#include <babylon/actions/action_manager.h>
#include <babylon/animations/animatable.h>
#include <babylon/animations/animation_group.h>
#include <babylon/animations/runtime_animation.h>
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
#include <babylon/engine/iactive_mesh_candidate_provider.h>
#include <babylon/engine/iscene_component.h>
#include <babylon/engine/iscene_serializable_component.h>
#include <babylon/events/keyboard_event_types.h>
#include <babylon/events/keyboard_info_pre.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/gamepad/gamepad_manager.h>
#include <babylon/gamepad/gamepad_system_scene_component.h>
#include <babylon/helpers/environment_helper.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/glow_layer.h>
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
#include <babylon/mesh/buffer.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/mesh_simplification_scene_component.h>
#include <babylon/mesh/simplification/simplification_queue.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/physics/physics_engine.h>
#include <babylon/postprocess/post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager.h>
#include <babylon/postprocess/renderpipeline/post_process_render_pipeline_manager_scene_component.h>
#include <babylon/probes/reflection_probe.h>
#include <babylon/rendering/bounding_box_renderer.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/rendering/geometry_buffer_renderer.h>
#include <babylon/rendering/irendering_manager_auto_clear_setup.h>
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
    : AbstractScene{}
    , autoClear{true}
    , autoClearDepthAndStencil{true}
    , clearColor{Color4(0.2f, 0.2f, 0.3f, 1.f)}
    , ambientColor{Color3(0.f, 0.f, 0.f)}
    , _environmentBRDFTexture{nullptr}
    , environmentTexture{this, &Scene::get_environmentTexture,
                         &Scene::set_environmentTexture}
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
    , animationPropertiesOverride{this, &Scene::get_animationPropertiesOverride,
                                  &Scene::set_animationPropertiesOverride}
    , useConstantAnimationDeltaTime{false}
    , constantlyUpdateMeshUnderPointer{false}
    , hoverCursor{"pointer"}
    , defaultCursor{""}
    , preventDefaultOnPointerDown{true}
    , gamepadManager{this, &Scene::get_gamepadManager}
    , unTranslatedPointer{this, &Scene::get_unTranslatedPointer}
    , cameraToUseForPointers(nullptr)
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
    , activeCamera{nullptr}
    , defaultMaterial{this, &Scene::get_defaultMaterial,
                      &Scene::set_defaultMaterial}
    , texturesEnabled{this, &Scene::get_texturesEnabled,
                      &Scene::set_texturesEnabled}
    , particlesEnabled{true}
    , spritesEnabled{true}
    , skeletonsEnabled{this, &Scene::get_skeletonsEnabled,
                       &Scene::set_skeletonsEnabled}
    , lensFlaresEnabled{true}
    , collisionsEnabled{true}
    , collisionCoordinator{nullptr}
    , gravity{Vector3(0.f, -9.807f, 0.f)}
    , postProcessesEnabled{true}
    , postProcessManager{nullptr}
    , postProcessRenderPipelineManager{this,
                                       &Scene::
                                         get_postProcessRenderPipelineManager}
    , renderTargetsEnabled{true}
    , dumpNextRenderTargets{false}
    , probesEnabled{true}
    , actionManager{nullptr}
    , proceduralTexturesEnabled{true}
    , mainSoundTrack{this, &Scene::get_mainSoundTrack}
    , simplificationQueue{this, &Scene::get_simplificationQueue,
                          &Scene::set_simplificationQueue}
    , animationTimeScale{1}
    , _cachedMaterial{nullptr}
    , _cachedEffect{nullptr}
    , _cachedVisibility{0.f}
    , dispatchAllSubMeshesOfActiveMeshes{false}
    , _forcedViewPosition{nullptr}
    , _isAlternateRenderingEnabled{this,
                                   &Scene::get_isAlternateRenderingEnabled}
    , frustumPlanes{this, &Scene::get_frustumPlanes}
    , requireLightSorting{false}
    , depthRenderer{this, &Scene::get_depthRenderer}
    , geometryBufferRenderer{this, &Scene::get_geometryBufferRenderer,
                             &Scene::set_geometryBufferRenderer}
    , debugLayer{this, &Scene::get_debugLayer}
    , workerCollisions{this, &Scene::get_workerCollisions,
                       &Scene::set_workerCollisions}
    , selectionOctree{this, &Scene::get_selectionOctree}
    , meshUnderPointer{this, &Scene::get_meshUnderPointer}
    , pointerX{this, &Scene::get_pointerX}
    , pointerY{this, &Scene::get_pointerY}
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
    , _allowPostProcessClear{true}
    , blockMaterialDirtyMechanism{false}
    , _environmentTexture{nullptr}
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
    , _animationRatio{1.f}
    , _animationTimeLastSet{false}
    , _renderId{0}
    , _executeWhenReadyTimeoutId{-1}
    , _intermediateRendering{false}
    , _viewUpdateFlag{-1}
    , _projectionUpdateFlag{-1}
    , _alternateViewUpdateFlag{-1}
    , _alternateProjectionUpdateFlag{-1}
    , _isDisposed{false}
    , _activeMeshCandidateProvider{nullptr}
    , _activeMeshesFrozen{false}
    , _renderingManager{nullptr}
    , _physicsEngine{nullptr}
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
    , _selectionOctree{nullptr}
    , _pointerOverMesh{nullptr}
    , _pointerOverSprite{nullptr}
    , _debugLayer{nullptr}
    , _geometryBufferRenderer{nullptr}
    , _pickedDownMesh{nullptr}
    , _pickedUpMesh{nullptr}
    , _pickedDownSprite{nullptr}
    , _uid{Tools::RandomId()}
    , _tempPickingRay{std::make_unique<Ray>(Ray::Zero())}
    , _cachedRayForTransform{nullptr}
{
  engine->scenes.emplace_back(this);

  _renderingManager = std::make_unique<RenderingManager>(this);

  // if (PostProcessManager) {
  {
    postProcessManager = std::make_unique<PostProcessManager>(this);
  }

  attachControl();

  // Collision coordinator initialization.
  workerCollisions = false;

  // Uniform Buffer
  _createUbo();

  // Default Image processing definition.
  // if (ImageProcessingConfiguration)
  {
    _imageProcessingConfiguration
      = std::make_shared<ImageProcessingConfiguration>();
  }
}

Scene::~Scene()
{
}

IReflect::Type Scene::type() const
{
  return IReflect::Type::SCENE;
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

ISceneComponentPtr Scene::_getComponent(const std::string& name)
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
  _unTranslatedPointer = Vector2(static_cast<float>(_unTranslatedPointerX),
                                 static_cast<float>(_unTranslatedPointerY));

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
  markAllMaterialsAsDirty(Material::TextureDirtyFlag());
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag());
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag());
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag());
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag());
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
  markAllMaterialsAsDirty(Material::MiscDirtyFlag());
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
  markAllMaterialsAsDirty(Material::LightDirtyFlag());
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
  markAllMaterialsAsDirty(Material::LightDirtyFlag());
}

bool Scene::get_lightsEnabled() const
{
  return _lightsEnabled;
}

void Scene::set_texturesEnabled(bool value)
{
  if (_texturesEnabled == value) {
    return;
  }
  _texturesEnabled = value;
  markAllMaterialsAsDirty(Material::TextureDirtyFlag());
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
  markAllMaterialsAsDirty(Material::AttributesDirtyFlag());
}

bool Scene::get_skeletonsEnabled() const
{
  return _skeletonsEnabled;
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

std::unique_ptr<SoundTrack>& Scene::get_mainSoundTrack()
{
  if (!_mainSoundTrack) {
    _mainSoundTrack = std::make_unique<SoundTrack>(this, true);
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

std::unordered_map<std::string, std::unique_ptr<DepthRenderer>>&
Scene::get_depthRenderer()
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

void Scene::set_workerCollisions(bool enabled)
{
  _workerCollisions = enabled;
  if (collisionCoordinator) {
    collisionCoordinator->destroy();
    collisionCoordinator.reset(nullptr);
  }

  if (enabled) {
    collisionCoordinator = std::make_unique<CollisionCoordinatorWorker>();
  }
  else {
    collisionCoordinator = std::make_unique<CollisionCoordinatorLegacy>();
  }

  collisionCoordinator->init(this);
}

bool Scene::get_workerCollisions() const
{
  return _workerCollisions;
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

AbstractMesh*& Scene::get_meshUnderPointer()
{
  return _pointerOverMesh;
}

int Scene::get_pointerX() const
{
  return _pointerX;
}

int Scene::get_pointerY() const
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

std::optional<float> Scene::getCachedVisibility()
{
  return _cachedVisibility;
}

bool Scene::isCachedMaterialInvalid(Material* material, Effect* effect,
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

std::vector<AbstractMeshPtr>& Scene::getActiveMeshes()
{
  return _activeMeshes;
}

const std::vector<AbstractMeshPtr>& Scene::getActiveMeshes() const
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
  _sceneUbo = std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _sceneUbo->addUniform("viewProjection", 16);
  _sceneUbo->addUniform("view", 16);
}

void Scene::_createAlternateUbo()
{
  _alternateSceneUbo
    = std::make_unique<UniformBuffer>(_engine, Float32Array(), true);
  _alternateSceneUbo->addUniform("viewProjection", 16);
  _alternateSceneUbo->addUniform("view", 16);
}

std::optional<PickingInfo> Scene::_pickSpriteButKeepRay(
  const std::optional<PickingInfo>& originalPointerInfo, int x, int y,
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
        pointerInfo.pickInfo.ray = createPickingRay(
          pointerInfo.mouseWheelEvent.offsetX,
          pointerInfo.mouseWheelEvent.offsetY, &identityMatrix, activeCamera);
      }
      else if (pointerInfo.type == PointerEventTypes::POINTERMOVE) {
        pointerInfo.pickInfo.ray = createPickingRay(
          pointerInfo.pointerEvent.offsetX, pointerInfo.pointerEvent.offsetY,
          &identityMatrix, activeCamera);
      }
    }
  }
}

Scene& Scene::simulatePointerMove(std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointermove");

  if (_checkPrePointerObservable(pickResult, evt,
                                 PointerEventTypes::POINTERMOVE)) {
    return *this;
  }
  return _processPointerMove(pickResult, evt);
}

Scene& Scene::_processPointerMove(std::optional<PickingInfo>& pickResult,
                                  const PointerEvent& evt)
{
  auto canvas = _engine->getRenderingCanvas();

  if (!canvas) {
    return *this;
  }

  if (pickResult && (*pickResult).hit && (*pickResult).pickedMesh) {
    setPointerOverSprite(nullptr);

    setPointerOverMesh((*pickResult).pickedMesh);

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
    pickResult = _pickSpriteButKeepRay(pickResult, _unTranslatedPointerX,
                                       _unTranslatedPointerY, _spritePredicate,
                                       false, cameraToUseForPointers);

    if (pickResult && (*pickResult).hit && (*pickResult).pickedSprite) {
      setPointerOverSprite((*pickResult).pickedSprite);
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
    auto type = (evt.type == EventType::MOUSE_WHEEL
                 || evt.type == EventType::DOM_MOUSE_SCROLL) ?
                  PointerEventTypes::POINTERWHEEL :
                  PointerEventTypes::POINTERMOVE;

    if (onPointerMove) {
      onPointerMove(evt, pickResult, type);
    }

    if (onPointerObservable.hasObservers()) {
      auto type = evt.type == EventType::MOUSE_WHEEL
                      || evt.type == EventType::DOM_MOUSE_SCROLL ?
                    PointerEventTypes::POINTERWHEEL :
                    PointerEventTypes::POINTERMOVE;

      if (type == PointerEventTypes::POINTERWHEEL) {
        PointerInfo pi(type, *static_cast<MouseWheelEvent const*>(&evt),
                       *pickResult);
        _setRayOnPointerInfo(pi);
        onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
      }
      else {
        PointerInfo pi(type, evt, *pickResult);
        _setRayOnPointerInfo(pi);
        onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
      }
    }
  }

  return *this;
}

bool Scene::_checkPrePointerObservable(
  const std::optional<PickingInfo>& pickResult, const PointerEvent& evt,
  PointerEventTypes type)
{
  PointerInfoPre pi(type, evt, _unTranslatedPointerX, _unTranslatedPointerY);
  if (pickResult) {
    pi.ray = (*pickResult).ray;
  }
  onPrePointerObservable.notifyObservers(&pi, static_cast<int>(type));
  if (pi.skipOnPointerObservable) {
    return true;
  }
  else {
    return false;
  }
}

Scene& Scene::simulatePointerDown(const std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointerdown");

  if (_checkPrePointerObservable(pickResult, evt,
                                 PointerEventTypes::POINTERDOWN)) {
    return *this;
  }

  return _processPointerDown(pickResult, evt);
}

Scene& Scene::_processPointerDown(const std::optional<PickingInfo>& pickResult,
                                  const PointerEvent& evt)
{
  if (pickResult && (*pickResult).hit && (*pickResult).pickedMesh) {
    _pickedDownMesh    = (*pickResult).pickedMesh;
    auto actionManager = _pickedDownMesh->actionManager;
    if (actionManager) {
      if (actionManager->hasPickTriggers()) {
        actionManager->processTrigger(
          ActionManager::OnPickDownTrigger(),
          ActionEvent::CreateNew(_pickedDownMesh, evt));
        switch (evt.button) {
          case MouseButtonType::LEFT:
            actionManager->processTrigger(
              ActionManager::OnLeftPickTrigger(),
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          case MouseButtonType::MIDDLE:
            actionManager->processTrigger(
              ActionManager::OnCenterPickTrigger(),
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          case MouseButtonType::RIGHT:
            actionManager->processTrigger(
              ActionManager::OnRightPickTrigger(),
              ActionEvent::CreateNew(_pickedDownMesh, evt));
            break;
          default:
            break;
        }
      }

      if (actionManager->hasSpecificTrigger(
            ActionManager::OnLongPressTrigger())) {
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

Scene& Scene::simulatePointerUp(const std::optional<PickingInfo>& pickResult)
{
  PointerEvent evt("pointerup");
  ClickInfo clickInfo;
  clickInfo.singleClick = true;
  clickInfo.ignore      = true;

  if (_checkPrePointerObservable(pickResult, evt,
                                 PointerEventTypes::POINTERUP)) {
    return *this;
  }

  return _processPointerUp(pickResult, evt, clickInfo);
}

Scene& Scene::_processPointerUp(const std::optional<PickingInfo>& pickResult,
                                const PointerEvent& evt,
                                const ClickInfo& clickInfo)
{
  if (pickResult && pickResult && (*pickResult).pickedMesh) {
    const auto& _pickResult = *pickResult;
    _pickedUpMesh           = _pickResult.pickedMesh;
    if (_pickedDownMesh == _pickedUpMesh) {
      if (onPointerPick) {
        onPointerPick(evt, pickResult);
      }
      if (clickInfo.singleClick() && !clickInfo.ignore()
          && onPointerObservable.hasObservers()) {
        auto type = PointerEventTypes::POINTERPICK;
        PointerInfo pi(type, evt, *pickResult);
        _setRayOnPointerInfo(pi);
        onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
      }
    }
    if (_pickedUpMesh->actionManager) {
      if (clickInfo.ignore()) {
        _pickedUpMesh->actionManager->processTrigger(
          ActionManager::OnPickUpTrigger(),
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
      if (!clickInfo.hasSwiped() && !clickInfo.ignore()
          && clickInfo.singleClick()) {
        _pickedUpMesh->actionManager->processTrigger(
          ActionManager::OnPickTrigger(),
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
      if (clickInfo.doubleClick() && !clickInfo.ignore()
          && _pickedUpMesh->actionManager->hasSpecificTrigger(
               ActionManager::OnDoublePickTrigger())) {
        _pickedUpMesh->actionManager->processTrigger(
          ActionManager::OnDoublePickTrigger(),
          ActionEvent::CreateNew(_pickedUpMesh, evt));
      }
    }
  }
  if (_pickedDownMesh && _pickedDownMesh->actionManager
      && _pickedDownMesh->actionManager->hasSpecificTrigger(
           ActionManager::OnPickOutTrigger())
      && _pickedDownMesh != _pickedUpMesh) {
    _pickedDownMesh->actionManager->processTrigger(
      ActionManager::OnPickOutTrigger(),
      ActionEvent::CreateNew(_pickedDownMesh, evt));
  }

  auto type = PointerEventTypes::POINTERUP;
  if (onPointerObservable.hasObservers()) {
    if (!clickInfo.ignore()) {
      if (!clickInfo.hasSwiped()) {
        if (clickInfo.singleClick()
            && onPointerObservable.hasSpecificMask(
                 static_cast<int>(PointerEventTypes::POINTERTAP))) {
          auto type = PointerEventTypes::POINTERTAP;
          PointerInfo pi(type, evt, *pickResult);
          _setRayOnPointerInfo(pi);
          onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
        }
        if (clickInfo.doubleClick()
            && onPointerObservable.hasSpecificMask(
                 static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))) {
          auto type = PointerEventTypes::POINTERDOUBLETAP;
          PointerInfo pi(type, evt, *pickResult);
          _setRayOnPointerInfo(pi);
          onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
        }
      }
    }
    else {
      auto type = PointerEventTypes::POINTERUP;
      PointerInfo pi(type, evt, *pickResult);
      _setRayOnPointerInfo(pi);
      onPointerObservable.notifyObservers(&pi, static_cast<int>(type));
    }
  }

  if (onPointerUp) {
    onPointerUp(evt, pickResult, type);
  }

  return *this;
}

bool Scene::isPointerCaptured(int pointerId)
{
  return stl_util::contains(_pointerCaptures, pointerId)
         && _pointerCaptures[pointerId];
}

void Scene::attachControl(bool attachUp, bool attachDown, bool attachMove)
{
  _initActionManager
    = [this](ActionManager* act,
             const ClickInfo & /*clickInfo*/) -> ActionManager* {
    if (!_meshPickProceed) {
      auto pickResult
        = pick(_unTranslatedPointerX, _unTranslatedPointerY,
               pointerDownPredicate, false, cameraToUseForPointers);
      if (pickResult) {
        _currentPickResult = *pickResult;
      }
      else {
        _currentPickResult = std::nullopt;
      }
      if (pickResult) {
        const auto _pickResult = *pickResult;
        act                    = (_pickResult.hit && _pickResult.pickedMesh) ?
                _pickResult.pickedMesh->actionManager :
                nullptr;
      }
      _meshPickProceed = true;
    }
    return act;
  };

  _initClickEvent = [this](Observable<PointerInfoPre>& obs1,
                           Observable<PointerInfo>& obs2,
                           const PointerEvent& evt,
                           const std::function<void(
                             const ClickInfo& clickInfo,
                             std::optional<PickingInfo>& pickResult)>& cb) {
    ClickInfo clickInfo;
    _currentPickResult = std::nullopt;
    ActionManager* act = nullptr;

    auto checkPicking
      = obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERPICK))
        || obs2.hasSpecificMask(
             static_cast<int>(PointerEventTypes::POINTERPICK))
        || obs1.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERTAP))
        || obs2.hasSpecificMask(static_cast<int>(PointerEventTypes::POINTERTAP))
        || obs1.hasSpecificMask(
             static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
        || obs2.hasSpecificMask(
             static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));
    if (!checkPicking && ActionManager::HasPickTriggers()) {
      act = _initActionManager(act, clickInfo);
      if (act) {
        checkPicking = act->hasPickTriggers();
      }
    }
    if (checkPicking) {
      auto btn            = evt.button;
      clickInfo.hasSwiped = std::abs(_startingPointerPosition.x - _pointerX)
                              > Scene::DragMovementThreshold
                            || std::abs(_startingPointerPosition.y - _pointerY)
                                 > Scene::DragMovementThreshold;

      if (!clickInfo.hasSwiped) {
        auto checkSingleClickImmediately = !Scene::ExclusiveDoubleClickMode;

        if (!checkSingleClickImmediately) {
          checkSingleClickImmediately
            = !obs1.hasSpecificMask(
                static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
              && !obs2.hasSpecificMask(
                   static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));

          if (checkSingleClickImmediately
              && !ActionManager::HasSpecificTrigger(
                   ActionManager::OnDoublePickTrigger())) {
            act = _initActionManager(act, clickInfo);
            if (act) {
              checkSingleClickImmediately = !act->hasSpecificTrigger(
                ActionManager::OnDoublePickTrigger());
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
          = obs1.hasSpecificMask(
              static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))
            || obs2.hasSpecificMask(
                 static_cast<int>(PointerEventTypes::POINTERDOUBLETAP));
        if (!checkDoubleClick
            && ActionManager::HasSpecificTrigger(
                 ActionManager::OnDoublePickTrigger())) {
          act = _initActionManager(act, clickInfo);
          if (act) {
            checkDoubleClick
              = act->hasSpecificTrigger(ActionManager::OnDoublePickTrigger());
          }
        }
        if (checkDoubleClick) {
          // two successive keys pressed are equal, double click delay is not
          // over and double click has not just occurred
          if (btn == _previousButtonPressed
              && Time::fpTimeSince<long, std::milli>(
                   _previousStartingPointerTime)
                   < Scene::DoubleClickDelay.count()
              && !_doubleClickOccured) {
            // pointer has not moved for 2 clicks, it's a double click
            if (!clickInfo.hasSwiped
                && std::abs(_previousStartingPointerPosition.x
                            - _startingPointerPosition.x)
                     < Scene::DragMovementThreshold
                && std::abs(_previousStartingPointerPosition.y
                            - _startingPointerPosition.y)
                     < Scene::DragMovementThreshold) {
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

  _onPointerMove
    = [this](PointerEvent&& evt) { _onPointerMoveEvent(std::move(evt)); };

  _onPointerDown
    = [this](PointerEvent&& evt) { _onPointerDownEvent(std::move(evt)); };

  _onPointerUp
    = [this](PointerEvent&& evt) { _onPointerUpEvent(std::move(evt)); };

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
  if (_checkPrePointerObservable(std::nullopt, evt,
                                 (evt.type == EventType::MOUSE_WHEEL
                                  || evt.type == EventType::DOM_MOUSE_SCROLL) ?
                                   PointerEventTypes::POINTERWHEEL :
                                   PointerEventTypes::POINTERMOVE)) {
    return;
  }

  if (!cameraToUseForPointers && !activeCamera) {
    return;
  }

  if (!pointerMovePredicate) {
    pointerMovePredicate = [this](const AbstractMeshPtr& mesh) -> bool {
      return mesh->isPickable && mesh->isVisible && mesh->isReady()
             && mesh->isEnabled()
             && (mesh->enablePointerMoveEvents
                 || constantlyUpdateMeshUnderPointer
                 || mesh->actionManager != nullptr);
    };
  }

  // Meshes
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY,
                         pointerMovePredicate, false, cameraToUseForPointers);

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
  if (_checkPrePointerObservable(std::nullopt, evt,
                                 PointerEventTypes::POINTERDOWN)) {
    return;
  }

  if (!cameraToUseForPointers && !activeCamera) {
    return;
  }

  _pointerCaptures[evt.pointerId] = true;
  _startingPointerPosition.x      = _pointerX;
  _startingPointerPosition.y      = _pointerY;
  _startingPointerTime            = Time::highresTimepointNow();

  if (!pointerDownPredicate) {
    pointerDownPredicate = [](const AbstractMeshPtr& mesh) -> bool {
      return mesh->isPickable && mesh->isVisible && mesh->isReady()
             && mesh->isEnabled();
    };
  }

  // Meshes
  _pickedDownMesh = nullptr;
  auto pickResult = pick(_unTranslatedPointerX, _unTranslatedPointerY,
                         pointerDownPredicate, false, cameraToUseForPointers);

  _processPointerDown(pickResult, evt);

  // Sprites
  _pickedDownSprite = nullptr;
  if (!spriteManagers.empty()) {
    pickResult = pickSprite(_unTranslatedPointerX, _unTranslatedPointerY,
                            _spritePredicate, false, cameraToUseForPointers);

    if (pickResult && (*pickResult).hit && (*pickResult).pickedSprite) {
      const auto& _pickResult = *pickResult;
      if (_pickResult.pickedSprite->actionManager) {
        _pickedDownSprite = _pickResult.pickedSprite;
        switch (evt.button) {
          case MouseButtonType::LEFT:
            _pickedDownSprite->actionManager->processTrigger(
              ActionManager::OnLeftPickTrigger(),
              ActionEvent::CreateNewFromSprite(_pickedDownSprite, this, evt));
            break;
          case MouseButtonType::MIDDLE:
            _pickedDownSprite->actionManager->processTrigger(
              ActionManager::OnCenterPickTrigger(),
              ActionEvent::CreateNewFromSprite(_pickedDownSprite, this, evt));
            break;
          case MouseButtonType::RIGHT:
            _pickedDownSprite->actionManager->processTrigger(
              ActionManager::OnRightPickTrigger(),
              ActionEvent::CreateNewFromSprite(_pickedDownSprite, this, evt));
            break;
          default:
            break;
        }
        if (_pickedDownSprite->actionManager) {
          _pickedDownSprite->actionManager->processTrigger(
            ActionManager::OnPickDownTrigger(),
            ActionEvent::CreateNewFromSprite(_pickedDownSprite, this, evt));
        }
      }
    }
  }
}

void Scene::_onPointerUpEvent(PointerEvent&& evt)
{
  if (_totalPointersPressed == 0) { // We are attaching the pointer up to
                                    // windows because of a bug in FF
    return; // So we need to test it the pointer down was pressed before.
  }

  --_totalPointersPressed;
  _pickedUpMesh    = nullptr;
  _meshPickProceed = false;

  _updatePointerPosition(evt);
  _initClickEvent(
    onPrePointerObservable, onPointerObservable, evt,
    [this, evt](const ClickInfo& clickInfo,
                std::optional<PickingInfo>& pickResult) {
      // PreObservable support
      if (onPrePointerObservable.hasObservers()) {
        if (!clickInfo.ignore) {
          if (!clickInfo.hasSwiped) {
            if (clickInfo.singleClick
                && onPrePointerObservable.hasSpecificMask(
                     static_cast<int>(PointerEventTypes::POINTERTAP))) {
              if (_checkPrePointerObservable(std::nullopt, evt,
                                             PointerEventTypes::POINTERTAP)) {
                return;
              }
            }
            if (clickInfo.doubleClick
                && onPrePointerObservable.hasSpecificMask(
                     static_cast<int>(PointerEventTypes::POINTERDOUBLETAP))) {
              if (_checkPrePointerObservable(
                    std::nullopt, evt, PointerEventTypes::POINTERDOUBLETAP)) {
                return;
              }
            }
          }
        }
        else {
          if (_checkPrePointerObservable(std::nullopt, evt,
                                         PointerEventTypes::POINTERUP)) {
            return;
          }
        }
      }

      if (!cameraToUseForPointers && !activeCamera) {
        return;
      }

      _pointerCaptures[evt.pointerId] = false;

      if (!pointerUpPredicate) {
        pointerUpPredicate = [](const AbstractMeshPtr& mesh) -> bool {
          return mesh->isPickable && mesh->isVisible && mesh->isReady()
                 && mesh->isEnabled();
        };
      }

      // Meshes
      if (!_meshPickProceed
          && (ActionManager::HasTriggers()
              || onPointerObservable.hasObservers())) {
        _initActionManager(nullptr, clickInfo);
      }
      if (!pickResult) {
        pickResult = *_currentPickResult;
      }

      _processPointerUp(pickResult, evt, clickInfo);

      // Sprites
      if (!clickInfo.ignore) {
        if (!spriteManagers.empty()) {
          auto spritePickResult
            = pickSprite(_unTranslatedPointerX, _unTranslatedPointerY,
                         _spritePredicate, false, cameraToUseForPointers);

          if (spritePickResult) {
            const auto& _spritePickResult = *spritePickResult;
            if (_spritePickResult.hit && _spritePickResult.pickedSprite) {
              if (_spritePickResult.pickedSprite->actionManager) {
                _spritePickResult.pickedSprite->actionManager->processTrigger(
                  ActionManager::OnPickUpTrigger(),
                  ActionEvent::CreateNewFromSprite(
                    _spritePickResult.pickedSprite, this, evt));
                if (_spritePickResult.pickedSprite->actionManager) {
                  if (std::abs(_startingPointerPosition.x - _pointerX)
                        < Scene::DragMovementThreshold
                      && std::abs(_startingPointerPosition.y - _pointerY)
                           < Scene::DragMovementThreshold) {
                    _spritePickResult.pickedSprite->actionManager
                      ->processTrigger(
                        ActionManager::OnPickTrigger(),
                        ActionEvent::CreateNewFromSprite(
                          _spritePickResult.pickedSprite, this, evt));
                  }
                }
              }
            }
            if (_pickedDownSprite && _pickedDownSprite->actionManager
                && _pickedDownSprite != _spritePickResult.pickedSprite) {
              _pickedDownSprite->actionManager->processTrigger(
                ActionManager::OnPickOutTrigger(),
                ActionEvent::CreateNewFromSprite(_pickedDownSprite, this, evt));
            }
          }
        }
      }

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
    actionManager->processTrigger(ActionManager::OnKeyDownTrigger(),
                                  ActionEvent::CreateNewFromScene(this, evt));
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
    actionManager->processTrigger(ActionManager::OnKeyUpTrigger(),
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
  for (const auto& geometry : geometries) {
    if (geometry->delayLoadState == EngineConstants::DELAYLOADSTATE_LOADING) {
      return false;
    }
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
      = mesh->getClassName() == std::string("InstancedMesh")
        || (engine->getCaps().instancedArrays && _mesh->instances.size() > 0);

    // Is Ready For Mesh
    for (const auto& step : _isReadyForMeshStage) {
      if (!step.action(mesh.get(), hardwareInstancedRendering)) {
        return false;
      }
    }
  }

  // Post-processes
  if (!activeCameras.empty()) {
    for (auto& camera : activeCameras) {
      if (!camera->isReady(true)) {
        return false;
      }
    }
  }
  else if (activeCamera) {
    if (!activeCamera->isReady(true)) {
      return false;
    }
  }

  // Particles
  for (auto& particleSystem : particleSystems) {
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

std::vector<AnimationPtr> Scene::getAnimations()
{
  return std::vector<AnimationPtr>();
}

AnimatablePtr Scene::beginWeightedAnimation(
  const IAnimatablePtr& target, int from, int to, float weight, bool loop,
  float speedRatio, const std::function<void()>& onAnimationEnd,
  AnimatablePtr animatable,
  const std::function<bool(IAnimatable* target)>& targetMask)
{
  auto returnedAnimatable
    = beginAnimation(target, from, to, loop, speedRatio, onAnimationEnd,
                     animatable, false, targetMask);
  returnedAnimatable->weight = weight;

  return returnedAnimatable;
}

AnimatablePtr Scene::beginAnimation(
  const IAnimatablePtr& target, int from, int to, bool loop, float speedRatio,
  const std::function<void()>& onAnimationEnd, AnimatablePtr animatable,
  bool stopCurrent, const std::function<bool(IAnimatable* target)>& targetMask)
{
  if (from > to && speedRatio > 0.f) {
    speedRatio *= -1.f;
  }

  if (stopCurrent) {
    stopAnimation(target, "", targetMask);
  }

  if (!animatable) {
    animatable = Animatable::New(this, target, from, to, loop, speedRatio,
                                 onAnimationEnd);
  }

  auto shouldRunTargetAnimations = targetMask ? targetMask(target.get()) : true;

  // Local animations
  auto _animations = target->getAnimations();
  if (!_animations.empty() && shouldRunTargetAnimations) {
    animatable->appendAnimations(target, _animations);
  }

  // Children animations
  std::vector<IAnimatablePtr> animatables;
  if (auto s = std::dynamic_pointer_cast<Skeleton>(target)) {
    animatables = s->getAnimatables();
  }
  else if (auto s = std::dynamic_pointer_cast<StandardMaterial>(target)) {
    animatables = s->getAnimatables();
  }
  else if (auto m = std::dynamic_pointer_cast<Mesh>(target)) {
    animatables = m->getAnimatables();
  }

  if (!animatables.empty()) {
    for (auto& childAnimatable : animatables) {
      beginAnimation(childAnimatable, from, to, loop, speedRatio,
                     onAnimationEnd, animatable, stopCurrent, targetMask);
    }
  }

  animatable->reset();

  return animatable;
}

AnimatablePtr
Scene::beginDirectAnimation(const IAnimatablePtr& target,
                            const std::vector<AnimationPtr>& _animations,
                            float from, float to, bool loop, float speedRatio,
                            const std::function<void()>& onAnimationEnd)
{
  return Animatable::New(this, target, from, to, loop, speedRatio,
                         onAnimationEnd, _animations);
}

std::vector<AnimatablePtr> Scene::beginDirectHierarchyAnimation(
  const NodePtr& target, bool directDescendantsOnly,
  const std::vector<AnimationPtr>& animations, int from, int to, bool loop,
  float speedRatio, const std::function<void()>& onAnimationEnd)
{
  auto children = target->getDescendants(directDescendantsOnly);

  std::vector<AnimatablePtr> result;
  result.emplace_back(beginDirectAnimation(target, animations, from, to, loop,
                                           speedRatio, onAnimationEnd));
  for (auto& child : children) {
    result.emplace_back(beginDirectAnimation(child, animations, from, to, loop,
                                             speedRatio, onAnimationEnd));
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

  for (auto& animatable : animatables) {
    animatable->stop(animationName, targetMask);
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

  for (auto& group : animationGroups) {
    group->stop();
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
  auto deltaTime = useConstantAnimationDeltaTime ?
                     16.f :
                     Time::fpTimeSince<size_t, std::milli>(_animationTimeLast)
                       * animationTimeScale;
  _animationTime += static_cast<int>(deltaTime);
  _animationTimeLast = now;
  for (auto& activeAnimatable : _activeAnimatables) {
    activeAnimatable->_animate(std::chrono::milliseconds(_animationTime));
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
      .matrixData.decompose(finalScaling, finalQuaternion, finalPosition);
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
    auto scale              = runtimeAnimation->weight / normalizer;
    auto& currentPosition   = Tmp::Vector3Array[2];
    auto& currentScaling    = Tmp::Vector3Array[3];
    auto& currentQuaternion = Tmp::QuaternionArray[1];

    (*runtimeAnimation->currentValue())
      .matrixData.decompose(currentScaling, currentQuaternion, currentPosition);
    currentScaling.scaleAndAddToRef(scale, finalScaling);
    currentQuaternion.scaleAndAddToRef(scale, finalQuaternion);
    currentPosition.scaleAndAddToRef(scale, finalPosition);
  }

  auto workValue = *originalAnimation->_workValue;
  Matrix::ComposeToRef(finalScaling, finalQuaternion, finalPosition,
                       workValue.matrixData);
  originalAnimation->_workValue = workValue;
  return (*originalAnimation->_workValue);
}

Quaternion Scene::_processLateAnimationBindingsForQuaternions(
  float holderTotalWeight, std::vector<RuntimeAnimation*>& holderAnimations,
  Quaternion& holderOriginalValue, Quaternion& refQuaternion)
{
  auto& originalAnimation = holderAnimations[0];
  auto& originalValue     = holderOriginalValue;

  if (holderAnimations.size() == 1) {
    Quaternion::SlerpToRef(originalValue,
                           (*originalAnimation->currentValue()).quaternionData,
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
        (*holderAnimations[0]->currentValue()).quaternionData,
        (*holderAnimations[1]->currentValue()).quaternionData,
        holderAnimations[1]->weight / holderTotalWeight, refQuaternion);
      return refQuaternion;
    }
    quaternions.clear();
    weights.clear();

    normalizer = holderTotalWeight;
  }
  for (auto& runtimeAnimation : holderAnimations) {
    quaternions.emplace_back(
      (*runtimeAnimation->currentValue()).quaternionData);
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
  if (!_frustumPlanesSet) {
    _frustumPlanes    = Frustum::GetPlanes(_transformMatrix);
    _frustumPlanesSet = true;
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
    _alternateTransformMatrix = std::make_unique<Matrix>(Matrix::Zero());
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

void Scene::addMesh(const AbstractMeshPtr& newMesh, bool recursive)
{
  meshes.emplace_back(newMesh);

  // notify the collision coordinator
  if (collisionCoordinator) {
    collisionCoordinator->onMeshAdded(newMesh.get());
  }
  newMesh->_resyncLightSources();

  onNewMeshAddedObservable.notifyObservers(newMesh.get());

  if (recursive) {
    // for (auto& m : newMesh->getChildMeshes()) {
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
  int index = static_cast<int>(it - meshes.begin());
  if (it != meshes.end()) {
    // Remove from the scene if mesh found
    meshes.erase(it);
  }

  onMeshRemovedObservable.notifyObservers(toRemove);

  if (recursive) {
    for (auto& m : toRemove->getChildMeshes()) {
      removeMesh(m);
    }
  }

  return index;
}

void Scene::addTransformNode(const TransformNodePtr& newTransformNode)
{
  transformNodes.emplace_back(newTransformNode);

  onNewTransformNodeAddedObservable.notifyObservers(newTransformNode.get());
}

int Scene::removeTransformNode(const TransformNodePtr& toRemove)
{
  return removeTransformNode(toRemove.get());
}

int Scene::removeTransformNode(TransformNode* toRemove)
{
  auto it   = std::find_if(transformNodes.begin(), transformNodes.end(),
                         [toRemove](const TransformNodePtr& transformNode) {
                           return transformNode.get() == toRemove;
                         });
  int index = static_cast<int>(it - transformNodes.begin());
  if (it != transformNodes.end()) {
    // Remove from the scene if found
    transformNodes.erase(it);
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
  auto it   = std::find_if(skeletons.begin(), skeletons.end(),
                         [toRemove](const SkeletonPtr& skeleton) {
                           return skeleton.get() == toRemove;
                         });
  int index = static_cast<int>(it - skeletons.begin());
  if (it != skeletons.end()) {
    // Remove from the scene if found
    skeletons.erase(it);
  }

  return index;
}

int Scene::removeMorphTargetManager(const MorphTargetManagerPtr& toRemove)
{
  auto it   = std::find(morphTargetManagers.begin(), morphTargetManagers.end(),
                      toRemove);
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
  auto it = std::find_if(
    lights.begin(), lights.end(),
    [&toRemove](const LightPtr& light) { return light.get() == toRemove; });
  int index = static_cast<int>(it - lights.begin());
  if (it != lights.end()) {
    // Remove from meshes
    for (auto& mesh : meshes) {
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
  int index = static_cast<int>(it1 - cameras.begin());
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
  if (activeCamera.get() == toRemove) {
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
  auto it = std::find(animationGroups.begin(), animationGroups.end(), toRemove);
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
  auto it   = std::find(materials.begin(), materials.end(), toRemove);
  int index = static_cast<int>(it - materials.begin());
  if (it != materials.end()) {
    materials.erase(it);
  }
  return index;
}

int Scene::removeActionManager(const ActionManagerPtr& toRemove)
{
  return removeActionManager(toRemove.get());
}

int Scene::removeActionManager(ActionManager* toRemove)
{
  auto it   = std::find_if(actionManagers.begin(), actionManagers.end(),
                         [toRemove](const ActionManagerPtr& actionManager) {
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
  auto it   = std::find_if(textures.begin(), textures.end(),
                         [toRemove](const BaseTexturePtr& texture) {
                           return texture.get() == toRemove;
                         });
  int index = static_cast<int>(it - textures.begin());
  if (it != textures.end()) {
    textures.erase(it);
  }
  return index;
}

void Scene::addLight(const LightPtr& newLight)
{
  lights.emplace_back(newLight);
  sortLightsByPriority();

  // Add light to all meshes (To support if the light is removed and then
  // readded)
  for (auto& mesh : meshes) {
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
  materials.emplace_back(newMaterial);
}

void Scene::addMorphTargetManager(
  const MorphTargetManagerPtr& newMorphTargetManager)
{
  morphTargetManagers.emplace_back(newMorphTargetManager);
}

void Scene::addGeometry(const GeometryPtr& newGeometry)
{
  geometries.emplace_back(std::move(newGeometry));
}

void Scene::addActionManager(
  const std::shared_ptr<ActionManager>& newActionManager)
{
  actionManagers.emplace_back(newActionManager);
}

void Scene::addTexture(const BaseTexturePtr& newTexture)
{
  textures.emplace_back(newTexture);
}

void Scene::switchActiveCamera(const CameraPtr& newCamera, bool attachControl)
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

CameraPtr Scene::getCameraByUniqueID(unsigned int uniqueId)
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
  for (auto& skeleton : skeletons) {
    for (auto& bone : skeleton->bones) {
      if (bone->id == id) {
        return bone;
      }
    }
  }

  return nullptr;
}

BonePtr Scene::getBoneByName(const std::string& name)
{
  for (auto& skeleton : skeletons) {
    for (auto& bone : skeleton->bones) {
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

LightPtr Scene::getLightByUniqueID(unsigned int uniqueId)
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

bool Scene::pushGeometry(const GeometryPtr& geometry, bool force)
{
  if (!force && getGeometryByID(geometry->id)) {
    return false;
  }

  geometries.emplace_back(geometry);

  // Notify the collision coordinator
  if (collisionCoordinator) {
    collisionCoordinator->onGeometryAdded(geometry.get());
  }

  onNewGeometryAddedObservable.notifyObservers(geometry.get());

  return true;
}

bool Scene::removeGeometry(const GeometryPtr& geometry)
{
  return removeGeometry(geometry.get());
}

bool Scene::removeGeometry(Geometry* geometry)
{
  auto it = std::find_if(geometries.begin(), geometries.end(),
                         [geometry](const GeometryPtr& _geometry) {
                           return _geometry.get() == geometry;
                         });
  if (it != geometries.end()) {
    geometries.erase(it);

    // notify the collision coordinator
    if (collisionCoordinator) {
      collisionCoordinator->onGeometryDeleted(geometry);
    }

    onGeometryRemovedObservable.notifyObservers(geometry);

    return true;
  }
  return false;
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

AbstractMeshPtr Scene::getMeshByUniqueID(unsigned int uniqueId)
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

SoundPtr Scene::getSoundByName(const std::string& /*name*/)
{
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

MorphTargetManagerPtr Scene::getMorphTargetManagerById(unsigned int id)
{
  auto it
    = std::find_if(morphTargetManagers.begin(), morphTargetManagers.end(),
                   [&id](const MorphTargetManagerPtr& morphTargetManager) {
                     return morphTargetManager->uniqueId() == id;
                   });

  return (it == morphTargetManagers.end()) ? nullptr : *it;
}

bool Scene::isActiveMesh(const AbstractMeshPtr& mesh)
{
  return std::find(_activeMeshes.begin(), _activeMeshes.end(), mesh)
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
  if (activeCamera && !activeCamera->_activeMeshes.empty()) {
    activeCamera->_activeMeshes.clear();
  }
  if (!activeCameras.empty()) {
    for (auto& activeCamera : activeCameras) {
      if (activeCamera && !activeCamera->_activeMeshes.empty()) {
        activeCamera->_activeMeshes.clear();
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
    for (auto& texture : textures) {
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

void Scene::_evaluateSubMesh(const SubMeshPtr& subMesh, AbstractMesh* mesh)
{
  if (dispatchAllSubMeshesOfActiveMeshes || mesh->alwaysSelectAsActiveMesh
      || mesh->subMeshes.size() == 1 || subMesh->isInFrustum(_frustumPlanes)) {

    for (const auto& step : _evaluateSubMeshStage) {
      step.action(mesh, subMesh.get());
    }

    auto material = subMesh->getMaterial();
    if (material) {
      // Render targets
      if (material->getRenderTargetTextures) {
        if (std::find(_processedMaterials.begin(), _processedMaterials.end(),
                      material)
            == _processedMaterials.end()) {
          _processedMaterials.emplace_back(material);
          for (auto& renderTarget : material->getRenderTargetTextures()) {
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
  if (!activeCamera) {
    return *this;
  }

  if (!_frustumPlanesSet) {
    setTransformMatrix(activeCamera->getViewMatrix(),
                       activeCamera->getProjectionMatrix());
  }

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

  for (const auto& step : _beforeEvaluateActiveMeshStage) {
    step.action();
  }

  // Meshes
  std::vector<AbstractMeshPtr> _meshes;
  bool checkIsEnabled = true;

  // Determine mesh candidates
  if (_activeMeshCandidateProvider) {
    // Use _activeMeshCandidateProvider
    _meshes        = _activeMeshCandidateProvider->getMeshes(this);
    checkIsEnabled = _activeMeshCandidateProvider->checksIsEnabled == false;
  }
  else if (_selectionOctree) {
    // Octree
    // _meshes = _selectionOctree->select(_frustumPlanes);
  }
  else {
    // Full scene traversal
    _meshes = getMeshes();
  }

  // Check each mesh
  for (auto& mesh : _meshes) {
    if (mesh->isBlocked()) {
      continue;
    }

    _totalVertices.addCount(mesh->getTotalVertices(), false);

    if (!mesh->isReady() | (checkIsEnabled && !mesh->isEnabled())) {
      continue;
    }

    mesh->computeWorldMatrix();

    // Intersections
    if (mesh->actionManager
        && mesh->actionManager->hasSpecificTriggers2(
             ActionManager::OnIntersectionEnterTrigger(),
             ActionManager::OnIntersectionExitTrigger())) {
      if (std::find(_meshesForIntersections.begin(),
                    _meshesForIntersections.end(), mesh)
          == _meshesForIntersections.end()) {
        _meshesForIntersections.emplace_back(mesh);
      }
    }

    // Switch to current LOD
    auto meshLOD = mesh->getLOD(activeCamera);

    if (!meshLOD) {
      continue;
    }

    mesh->_preActivate();

    if (mesh->isVisible && mesh->visibility > 0
        && (mesh->alwaysSelectAsActiveMesh
            || ((mesh->layerMask & activeCamera->layerMask) != 0
                && mesh->isInFrustum(_frustumPlanes)))) {
      _activeMeshes.emplace_back(mesh);
      activeCamera->_activeMeshes.emplace_back(_activeMeshes.back());

      mesh->_activate(_renderId);
      if (meshLOD != mesh.get()) {
        meshLOD->_activate(_renderId);
      }

      _activeMesh(mesh, meshLOD);
    }
  }

  // Particle systems
  if (particlesEnabled) {
    onBeforeParticlesRenderingObservable.notifyObservers(this);
    for (auto& particleSystem : particleSystems) {
      if (!particleSystem->isStarted()) {
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

void Scene::_activeMesh(const AbstractMeshPtr& sourceMesh, AbstractMesh* mesh)
{
  if (skeletonsEnabled() && mesh->skeleton()) {
    if (std::find(_activeSkeletons.begin(), _activeSkeletons.end(),
                  mesh->skeleton())
        == _activeSkeletons.end()) {
      _activeSkeletons.emplace_back(mesh->skeleton());
      mesh->skeleton()->prepare();
    }

    if (!mesh->computeBonesUsingShaders()) {
      auto _mesh = static_cast<Mesh*>(mesh);
      if (_mesh) {
        if (std::find(_softwareSkinnedMeshes.begin(),
                      _softwareSkinnedMeshes.end(), _mesh)
            == _softwareSkinnedMeshes.end()) {
          _softwareSkinnedMeshes.emplace_back(_mesh);
        }
      }
    }
  }

  for (const auto& step : _activeMeshStage) {
    step.action(sourceMesh.get(), mesh);
  }

  if (mesh && !mesh->subMeshes.empty()) {
    // Submeshes Octrees
    std::vector<SubMeshPtr> subMeshes;

    if (mesh->_submeshesOctree && mesh->useOctreeForRenderingSelection) {
      // subMeshes = mesh->_submeshesOctree->select(_frustumPlanes);
    }
    else {
      subMeshes = mesh->subMeshes;
    }

    for (auto& subMesh : subMeshes) {
      _evaluateSubMesh(subMesh, mesh);
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

void Scene::_renderForCamera(const CameraPtr& camera,
                             const CameraPtr& rigParent)
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

  // Viewport
  engine->setViewport(activeCamera->viewport);

  // Camera
  resetCachedMaterial();
  ++_renderId;
  updateTransformMatrix();

  if (camera->_alternateCamera) {
    updateAlternateTransformMatrix(camera->_alternateCamera);
    _alternateRendering = true;
  }

  onBeforeCameraRenderObservable.notifyObservers(activeCamera.get());

  // Meshes
  _evaluateActiveMeshes();

  // Software skinning
  for (auto& mesh : _softwareSkinnedMeshes) {
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

  if (renderTargetsEnabled) {
    _intermediateRendering = true;

    if (!_renderTargets.empty()) {
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

      ++_renderId;
    }

    for (const auto& step : _cameraDrawRenderTargetStage) {
      step.action(activeCamera.get());
    }

    _intermediateRendering = false;

    engine->restoreDefaultFramebuffer(); // Restore back buffer
  }

  onAfterRenderTargetsRenderObservable.notifyObservers(this);

  // Prepare Frame
  if (postProcessManager) {
    postProcessManager->_prepareFrame(nullptr);
  }

  // Before Camera Draw
  for (const auto& step : _beforeCameraDrawStage) {
    step.action(activeCamera.get());
  }

  // Render
  onBeforeDrawPhaseObservable.notifyObservers(this);
  _renderingManager->render(nullptr, {}, true, true);
  onAfterDrawPhaseObservable.notifyObservers(this);

  // After Camera Draw
  for (const auto& step : _afterCameraDrawStage) {
    step.action(activeCamera.get());
  }

  // Finalize frame
  if (postProcessManager) {
    postProcessManager->_finalizeFrame(camera->isIntermediate);
  }

  // Reset some special arrays
  _renderTargets.clear();

  _alternateRendering = false;

  onAfterCameraRenderObservable.notifyObservers(activeCamera.get());
}

void Scene::_processSubCameras(const CameraPtr& camera)
{
  if (camera->cameraRigMode == Camera::RIG_MODE_NONE()) {
    _renderForCamera(camera);
    return;
  }

  // rig cameras
  for (auto& rigCamera : camera->_rigCameras) {
    _renderForCamera(rigCamera, camera);
  }

  activeCamera = camera;
  setTransformMatrix(activeCamera->getViewMatrix(),
                     activeCamera->getProjectionMatrix());
}

void Scene::_checkIntersections()
{
}

void Scene::render(bool updateCameras)
{
  if (isDisposed()) {
    return;
  }

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
    actionManager->processTrigger(ActionManager::OnEveryFrameTrigger());
  }

  if (_engine->isDeterministicLockStep()) {
    auto deltaTime
      = (std::max(static_cast<float>(Scene::MinDeltaTime.count()),
                  std::min(_engine->getDeltaTime() * 1000.f,
                           static_cast<float>(Scene::MaxDeltaTime.count())))
         / 1000.f)
        + _timeAccumulator;

    auto defaultFPS = (60.f / 1000.f);

    auto defaultFrameTime = 1000.f / 60.f; // frame time in MS

    if (_physicsEngine) {
      defaultFrameTime = _physicsEngine->getTimeStep() * 1000.f;
    }

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
      if (_physicsEngine) {
        onBeforePhysicsObservable.notifyObservers(this);
        _physicsEngine->_step(defaultFrameTime / 1000.f);
        onAfterPhysicsObservable.notifyObservers(this);
      }

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
    if (_physicsEngine) {
      onBeforePhysicsObservable.notifyObservers(this);
      _physicsEngine->_step(deltaTime / 1000.f);
      onAfterPhysicsObservable.notifyObservers(this);
    }
  }

  // Before camera update steps
  for (const auto& step : _beforeCameraUpdateStage) {
    step.action();
  }

  // Update Cameras
  if (updateCameras) {
    if (!activeCameras.empty()) {
      for (auto& camera : activeCameras) {
        camera->update();
        if (camera->cameraRigMode != Camera::RIG_MODE_NONE()) {
          // Rig cameras
          for (auto& rigCamera : camera->_rigCameras) {
            rigCamera->update();
          }
        }
      }
    }
    else if (activeCamera) {
      activeCamera->update();
      if (activeCamera->cameraRigMode != Camera::RIG_MODE_NONE()) {
        // rig cameras
        for (auto& rigCamera : activeCamera->_rigCameras) {
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

  onAfterRenderTargetsRenderObservable.notifyObservers(this);
  activeCamera = currentActiveCamera;

  // Procedural textures
  if (proceduralTexturesEnabled) {
    Tools::StartPerformanceCounter("Procedural textures",
                                   !proceduralTextures.empty());
    for (auto& proceduralTexture : proceduralTextures) {
      if (proceduralTexture->_shouldRender()) {
        proceduralTexture->render();
      }
    }
    Tools::EndPerformanceCounter("Procedural textures",
                                 !proceduralTextures.empty());
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

  // Shadows
  if (shadowsEnabled()) {
    for (auto& light : lights) {
      auto shadowGenerator = light->getShadowGenerator();
      if (light->isEnabled() && light->shadowEnabled && shadowGenerator) {
        auto shadowMap  = shadowGenerator->getShadowMap();
        auto& _textures = shadowGenerator->getShadowMap()->getScene()->textures;
        auto it         = std::find_if(_textures.begin(), _textures.end(),
                               [&shadowMap](const BaseTexturePtr& texture) {
                                 return texture == shadowMap;
                               });
        if (it != _textures.end()) {
          _renderTargets.emplace_back(shadowMap);
        }
      }
    }
  }

  // Collects render targets from external components.
  for (auto& step : _gatherRenderTargetsStage) {
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

bool Scene::get_audioEnabled() const
{
  return _audioEnabled;
}

void Scene::set_audioEnabled(bool value)
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

bool Scene::get_headphone() const
{
  return _headphone;
}

void Scene::set_headphone(bool value)
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

DepthRenderer* Scene::enableDepthRenderer(const CameraPtr& camera)
{
  auto _camera = camera ? camera : activeCamera;
  if (!_camera) {
    throw std::runtime_error("No camera available to enable depth renderer");
  }
  if (!stl_util::contains(_depthRenderer, _camera->id)
      || !_depthRenderer[_camera->id]) {
    unsigned int textureType = 0;
    if (_engine->getCaps().textureHalfFloatRender) {
      textureType = EngineConstants::TEXTURETYPE_HALF_FLOAT;
    }
    else if (_engine->getCaps().textureFloatRender) {
      textureType = EngineConstants::TEXTURETYPE_FLOAT;
    }
    else {
      throw std::runtime_error(
        "Depth renderer does not support int texture type");
    }
    _depthRenderer[_camera->id]
      = std::make_unique<DepthRenderer>(this, textureType, _camera);
  }

  return _depthRenderer[_camera->id].get();
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
  _beforeRenderingGroupDrawStage.clear();
  _afterRenderingGroupDrawStage.clear();
  _afterCameraDrawStage.clear();
  _beforeCameraUpdateStage.clear();
  _gatherRenderTargetsStage.clear();
  _rebuildGeometryStage.clear();
  for (const auto& component : _components) {
    component->dispose();
  }

  importedMeshesFiles.clear();

  stopAllAnimations();

  resetCachedMaterial();

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
  _registeredForLateAnimationBindings.clear();

  _meshesForIntersections.clear();
  _toBeDisposed.clear();

  // Abort active requests
  for (auto& request : _activeRequests) {
    if (request.abort) {
      request.abort();
    }
  }

  // Debug layer
  if (_debugLayer) {
    _debugLayer->hide();
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
  onBeforeSpritesRenderingObservable.clear();
  onAfterSpritesRenderingObservable.clear();
  onBeforeDrawPhaseObservable.clear();
  onAfterDrawPhaseObservable.clear();
  onBeforePhysicsObservable.clear();
  onAfterPhysicsObservable.clear();
  onBeforeAnimationsObservable.clear();
  onAfterAnimationsObservable.clear();
  onDataLoadedObservable.clear();
  onBeforeRenderingGroupObservable.clear();
  onAfterRenderingGroupObservable.clear();

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

  // Release animation groups
  for (auto& animationGroup : animationGroups) {
    animationGroup->dispose();
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

  // Physics
  if (_physicsEngine) {
    disablePhysicsEngine();
  }

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

void Scene::disposeSounds()
{
}

void Scene::clearCachedVertexData()
{
  for (auto& abstractmesh : meshes) {
    auto mesh = static_cast<Mesh*>(abstractmesh.get());
    if (mesh) {
      auto geometry = mesh->geometry();

      if (geometry) {
        geometry->_indices.clear();

        for (auto& vb : geometry->_vertexBuffers) {
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
  for (auto& baseTexture : textures) {
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
    for (auto& mesh : meshes) {
      if (filterPredicate(mesh)) {
        filteredMeshes.emplace_back(mesh);
      }
    }
  }
  else {
    filteredMeshes = meshes;
  }

  for (auto& mesh : filteredMeshes) {
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

Octree<AbstractMesh*>*
  Scene::createOrUpdateSelectionOctree(size_t /*maxCapacity*/,
                                       size_t /*maxDepth*/)
{
  if (!_selectionOctree) {
#if 0
    _selectionOctree = new Octree<AbstractMeshPtr>(
      [](const AbstractMeshPtr& entry, OctreeBlock<AbstractMeshPtr>& block) {
        Octree<AbstractMeshPtr>::CreationFuncForMeshes(entry, block);
      },
      maxCapacity, maxDepth);
#else
    return nullptr;
#endif
  }

  auto worldExtends = getWorldExtends();

  // Update octree
  auto _meshes = getMeshes();
  // _selectionOctree->update(worldExtends.min, worldExtends.max, _meshes);

  return _selectionOctree;
}

/** Picking **/
Ray Scene::createPickingRay(int x, int y, Matrix* world,
                            const CameraPtr& camera, bool cameraViewSpace)
{
  auto result = Ray::Zero();

  createPickingRayToRef(x, y, world, result, camera, cameraViewSpace);

  return result;
}

Scene& Scene::createPickingRayToRef(int x, int y, Matrix* world, Ray& result,
                                    CameraPtr camera, bool cameraViewSpace)
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

std::optional<PickingInfo> Scene::_internalPick(
  const std::function<Ray(Matrix& world)>& rayFunction,
  const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
  bool fastCheck)
{
  std::optional<PickingInfo> pickingInfo = std::nullopt;

  for (auto& mesh : meshes) {
    if (predicate) {
      if (!predicate(mesh)) {
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
  const std::function<bool(AbstractMesh* mesh)>& predicate)
{
  std::vector<std::optional<PickingInfo>> pickingInfos;

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
    if (!activeCamera) {
      return std::nullopt;
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
            bool fastCheck, const CameraPtr& camera)
{
  auto result = _internalPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      createPickingRayToRef(x, y, &world, *_tempPickingRay, camera);
      return *_tempPickingRay;
    },
    predicate, fastCheck);
  if (result) {
    auto _result     = *result;
    auto identityMat = Matrix::Identity();
    _result.ray
      = createPickingRay(x, y, &identityMat, camera ? camera : nullptr);
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

std::optional<PickingInfo> Scene::pickWithRay(
  const Ray& ray,
  const std::function<bool(const AbstractMeshPtr& mesh)>& predicate,
  bool fastCheck)
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
    predicate, fastCheck);
  if (result) {
    auto _result = *result;
    _result.ray  = ray;
    result       = _result;
  }
  return result;
}

std::vector<std::optional<PickingInfo>>
Scene::multiPick(int x, int y,
                 const std::function<bool(AbstractMesh* mesh)>& predicate,
                 const CameraPtr& camera)
{
  return _internalMultiPick(
    [this, x, y, &camera](Matrix& world) -> Ray {
      return createPickingRay(x, y, &world, camera);
    },
    predicate);
}

std::vector<std::optional<PickingInfo>> Scene::multiPickWithRay(
  const Ray& ray, const std::function<bool(AbstractMesh* mesh)>& predicate)
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
    predicate);
}

AbstractMesh* Scene::getPointerOverMesh()
{
  return _pointerOverMesh;
}

void Scene::setPointerOverMesh(AbstractMesh* mesh)
{
  if (_pointerOverMesh == mesh) {
    return;
  }

  if (_pointerOverMesh && _pointerOverMesh->actionManager) {
    _pointerOverMesh->actionManager->processTrigger(
      ActionManager::OnPointerOutTrigger(),
      ActionEvent::CreateNew(_pointerOverMesh));
  }

  _pointerOverMesh = mesh;
  if (_pointerOverMesh && _pointerOverMesh->actionManager) {
    _pointerOverMesh->actionManager->processTrigger(
      ActionManager::OnPointerOverTrigger(),
      ActionEvent::CreateNew(_pointerOverMesh));
  }
}

void Scene::setPointerOverSprite(Sprite* sprite)
{
  if (_pointerOverSprite == sprite) {
    return;
  }

  if (_pointerOverSprite && _pointerOverSprite->actionManager) {
    Event evt;
    _pointerOverSprite->actionManager->processTrigger(
      ActionManager::OnPointerOutTrigger(),
      ActionEvent::CreateNewFromSprite(_pointerOverSprite, this, evt));
  }

  _pointerOverSprite = sprite;
  if (_pointerOverSprite && _pointerOverSprite->actionManager) {
    Event evt;
    _pointerOverSprite->actionManager->processTrigger(
      ActionManager::OnPointerOverTrigger(),
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

  _physicsEngine     = std::make_unique<PhysicsEngine>(iGravity, plugin);
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
  for (auto& geometry : geometries) {
    geometry->_rebuild();
  }

  for (auto& mesh : meshes) {
    mesh->_rebuild();
  }

  if (postProcessManager) {
    postProcessManager->_rebuild();
  }

  for (const auto& component : _components) {
    component->rebuild();
  }

  for (auto& system : particleSystems) {
    system->rebuild();
  }

  for (auto& step : _rebuildGeometryStage) {
    step.action();
  }
}
void Scene::_rebuildTextures()
{
  for (auto& texture : textures) {
    texture->_rebuild();
  }

  markAllMaterialsAsDirty(Material::TextureDirtyFlag());
}

void Scene::createDefaultLight(bool replace)
{
  // Dispose existing light in replace mode.
  if (replace) {
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
}

void Scene::createDefaultCamera(bool createArcRotateCamera, bool replace,
                                bool attachCameraControls)
{
  // Dispose existing camera in replace mode.
  if (replace) {
    if (activeCamera) {
      activeCamera->dispose();
      activeCamera = nullptr;
    }
  }

  // Camera
  if (!activeCamera) {
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
      "Scene", "Can not create default skybox without environment texture.");
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

  return hdrSkybox;
}

std::unique_ptr<EnvironmentHelper>
Scene::createDefaultEnvironment(const IEnvironmentHelperOptions& options)
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
  const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
    opaqueSortCompareFn,
  const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
    alphaTestSortCompareFn,
  const std::function<int(const SubMeshPtr& a, const SubMeshPtr& b)>&
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
  if (blockMaterialDirtyMechanism) {
    return;
  }

  for (auto& material : materials) {
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
  const std::function<void(std::variant<std::string, ArrayBuffer>&,
                           const std::string&)>& /*onSuccess*/)
{
  return IFileRequest();
}

} // end of namespace BABYLON
