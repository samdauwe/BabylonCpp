#ifndef BABYLON_ENGINE_SCENE_H
#define BABYLON_ENGINE_SCENE_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/culling/octrees/octree.h>
#include <babylon/events/pointer_info.h>
#include <babylon/events/pointer_info_pre.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>
#include <babylon/tools/perf_counter.h>

namespace BABYLON {

/**
 * @brief Represents a scene to be rendered by the engine.
 */
class BABYLON_SHARED_EXPORT Scene : public IAnimatable, public IDisposable {

public:
  // Statics
  /**
   * The fog is deactivated
   */
  static constexpr unsigned int FOGMODE_NONE = 0;
  /**
   * The fog density is following an exponential function
   */
  static constexpr unsigned int FOGMODE_EXP = 1;
  /**
   * The fog density is following an exponential function faster than
   * FOGMODE_EXP
   */
  static constexpr unsigned int FOGMODE_EXP2 = 2;
  /**
   * The fog density is following a linear function.
   */
  static constexpr unsigned int FOGMODE_LINEAR = 3;

  static microseconds_t MinDeltaTime;
  static microseconds_t MaxDeltaTime;

  /**
   * The distance in pixel that you have to move to prevent some events.
   */
  static unsigned int DragMovementThreshold;
  /**
   * Time in milliseconds to wait to raise long press events if button is still
   * pressed.
   */
  static milliseconds_t LongPressDelay;
  /**
   * Time in milliseconds with two consecutive clicks will be considered as a
   * double click.
   */
  static milliseconds_t DoubleClickDelay;
  /**
   * If you need to check double click without raising a single click at first
   * click, enable this flag.
   */
  static bool ExclusiveDoubleClickMode;

  template <typename... Ts>
  static unique_ptr_t<Scene> New(Ts&&... args)
  {
    unique_ptr_t<Scene> scene(new Scene(::std::forward<Ts>(args)...));
    return scene;
  }
  virtual ~Scene();

  IReflect::Type type() const override;

  // Events
  /**
   * @brief A function to be executed when this scene is disposed.
   */
  void setOnDispose(
    const ::std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief A function to be executed before rendering this scene.
   */
  void setBeforeRender(
    const ::std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief A function to be executed after rendering this scene.
   */
  void setAfterRender(
    const ::std::function<void(Scene* scene, EventState& es)>& callback);

  void setBeforeCameraRender(
    const ::std::function<void(Camera* camera, EventState& es)>& callback);
  void setAfterCameraRender(
    const ::std::function<void(Camera* camera, EventState& es)>& callback);

  // Gamepads
  GamepadManager& gamepadManager();

  // Pointers
  Vector2 unTranslatedPointer() const;

  /** Properties **/

  /**
   * @brief Returns the texture used in all pbr material as the reflection
   * texture.
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to reference from here than from all the
   * materials.
   */
  BaseTexture* environmentTexture();

  /**
   * @brief Sets the texture used in all pbr material as the reflection texture.
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to set here than in all the materials.
   */
  void setEnvironmentTexture(BaseTexture* value);

  bool useRightHandedSystem() const;
  void setUseRightHandedSystem(bool value);
  void setStepId(unsigned int newStepId);
  unsigned int getStepId() const;
  unsigned int getInternalStep() const;

  /**
   * @brief Default image processing configuration used either in the rendering
   * Forward main pass or through the imageProcessingPostProcess if present.
   * As in the majority of the scene they are the same (exception for multi
   * camera), this is easier to reference from here than from all the materials
   * and post process.
   *
   * No setter as we it is a shared configuration, you can set the values
   * instead.
   */
  ImageProcessingConfiguration* imageProcessingConfiguration();

  bool forcePointsCloud() const;
  void setForcePointsCloud(bool value);
  bool fogEnabled() const;
  void setFogEnabled(bool value);
  bool fogMode() const;
  void setFogMode(unsigned int value);
  bool shadowsEnabled() const;
  void setShadowsEnabled(bool value);
  bool lightsEnabled() const;
  void setLightsEnabled(bool value);
  bool texturesEnabled() const;
  void setTexturesEnabled(bool value);
  bool skeletonsEnabled() const;
  void setSkeletonsEnabled(bool value);
  PostProcessRenderPipelineManager* postProcessRenderPipelineManager();
  SoundTrack* mainSoundTrack();
  Plane* clipPlane();
  void setClipPlane(const Plane& plane);
  void resetClipPlane();
  void setMirroredCameraPosition(const Vector3& newPosition);

  /**
   * @brief Returns the default material used on meshes when no material is
   * affected.
   */
  Material* defaultMaterial();

  /**
   * @brief Sets the default material used on meshes when no material is
   * affected.
   */
  void setDefaultMaterial(Material* value);

  bool _isAlternateRenderingEnabled() const;
  array_t<Plane, 6>& frustumPlanes();
  const array_t<Plane, 6>& frustumPlanes() const;
  DebugLayer* debugLayer();
  void setWorkerCollisions(bool enabled);
  bool workerCollisions() const;
  Octree<AbstractMesh*>* selectionOctree();

  /**
   * @brief The mesh that is currently under the pointer.
   * @return {BABYLON.AbstractMesh} mesh under the pointer/mouse cursor or null
   * if none.
   */
  AbstractMesh* meshUnderPointer();

  /**
   * @brief Current on-screen X position of the pointer
   * @return {number} X position of the pointer
   */

  int pointerX();

  /**
   * @brief Current on-screen Y position of the pointer
   * @return {number} Y position of the pointer
   */
  int pointerY();

  Material* getCachedMaterial();
  Effect* getCachedEffect();
  float getCachedVisibility();
  bool isCachedMaterialInvalid(Material* material, Effect* effect,
                               float visibility = 1.f);
  BoundingBoxRenderer* getBoundingBoxRenderer();
  OutlineRenderer* getOutlineRenderer();
  Engine* getEngine();
  size_t getTotalVertices() const;
  PerfCounter& totalVerticesPerfCounter();
  size_t getActiveIndices() const;
  PerfCounter& totalActiveIndicesPerfCounter();
  size_t getActiveParticles() const;
  PerfCounter& activeParticlesPerfCounter();
  size_t getActiveBones() const;
  PerfCounter& activeBonesPerfCounter();
  /** Stats **/
  microsecond_t getInterFramePerfCounter() const;
  PerfCounter& interFramePerfCounter();
  microsecond_t getLastFrameDuration() const;
  PerfCounter& lastFramePerfCounter();
  microsecond_t getEvaluateActiveMeshesDuration() const;
  PerfCounter& evaluateActiveMeshesDurationPerfCounter();
  vector_t<Mesh*>& getActiveMeshes();
  microsecond_t getRenderTargetsDuration() const;
  microsecond_t getRenderDuration() const;
  PerfCounter& renderDurationPerfCounter();
  microsecond_t getParticlesDuration() const;
  PerfCounter& particlesDurationPerfCounter();
  microsecond_t getSpritesDuration() const;
  PerfCounter& spriteDuractionPerfCounter();
  float getAnimationRatio() const;
  int getRenderId() const;
  void incrementRenderId();

  /** Pointers handling **/

  /**
   * @brief Use this method to simulate a pointer move on a mesh
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   */
  Scene& simulatePointerMove(const PickingInfo* pickResult);
  /**
   * @brief Use this method to simulate a pointer down on a mesh
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   */
  Scene& simulatePointerDown(const PickingInfo* pickResult);

  /**
   * @brief Use this method to simulate a pointer up on a mesh
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   */
  Scene& simulatePointerUp(const PickingInfo* pickResult);

  /**
   * @brief Attach events to the canvas (To handle actionManagers triggers and
   * raise onPointerMove, onPointerDown and onPointerUp
   * @param attachUp defines if you want to attach events to pointerup
   * @param attachDown defines if you want to attach events to pointerdown
   * @param attachMove defines if you want to attach events to pointermove
   */
  void attachControl(bool attachUp = true, bool attachDown = true,
                     bool attachMove = true);
  void detachControl();
  /** Ready **/
  bool isReady();
  void resetCachedMaterial();
  void registerBeforeRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);
  void unregisterBeforeRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);
  void registerAfterRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);
  void unregisterAfterRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);
  void _addPendingData(Mesh* mesh);
  void _addPendingData(InternalTexture* texure);
  void _removePendingData(InternalTexture* texture);
  void getWaitingItemsCount();

  /**
   * @brief Registers a function to be executed when the scene is ready.
   * @param {Function} func - the function to be executed.
   */
  void executeWhenReady(
    const ::std::function<void(Scene* scene, EventState& es)>& func);
  void _checkIsReady();

  /** Animations **/
  vector_t<Animation*> getAnimations() override;

  /**
   * @brief Will start the animation sequence of a given target.
   * @param target - the target
   * @param {number} from - from which frame should animation start
   * @param {number} to - till which frame should animation run.
   * @param {boolean} [loop] - should the animation loop
   * @param {number} [speedRatio] - the speed in which to run the animation
   * @param {Function} [onAnimationEnd] function to be executed when the
   * animation ended.
   * @param {BABYLON.Animatable} [animatable] an animatable object. If not
   * provided a new one will be created from the given params.
   * @return {BABYLON.Animatable} the animatable object created for this
   * animation
   * @returns {BABYLON.Animatable} the animatable object created for this
   * animation
   * See BABYLON.Animatable
   * @see http://doc.babylonjs.com/page.php?p=22081
   */
  Animatable* beginAnimation(IAnimatable* target, int from, int to,
                             bool loop = false, float speedRatio = 1.f,
                             const ::std::function<void()>& onAnimationEnd
                             = nullptr,
                             Animatable* animatable = nullptr);
  Animatable*
  beginDirectAnimation(IAnimatable* target,
                       const vector_t<Animation*>& animations, int from, int to,
                       bool loop = false, float speedRatio = 1.f,
                       const ::std::function<void()>& onAnimationEnd = nullptr);
  Animatable* getAnimatableByTarget(IAnimatable* target);
  vector_t<Animatable*>& animatables();

  /**
   * @brief Will stop the animation of the given target.
   * @param target - the target
   * @param animationName - the name of the animation to stop (all animations
   * will be stopped is empty)
   */
  void stopAnimation(IAnimatable* target, const string_t& animationName = "");

  /** Matrix **/
  void _switchToAlternateCameraConfiguration(bool active);
  Matrix getViewMatrix();
  Matrix& getProjectionMatrix();
  const Matrix& getProjectionMatrix() const;
  Matrix getTransformMatrix();
  void setTransformMatrix(Matrix& view, Matrix& projection);
  void _setAlternateTransformMatrix(Matrix& view, Matrix& projection);
  UniformBuffer* getSceneUniformBuffer();

  /** Methods **/
  unsigned int getUniqueId();
  void addMesh(unique_ptr_t<AbstractMesh>&& newMesh);
  int removeMesh(AbstractMesh* toRemove);
  void addTransformNode(TransformNode* newTransformNode);
  int removeTransformNode(TransformNode* toRemove);
  int removeSkeleton(Skeleton* toRemove);
  int removeMorphTargetManager(MorphTargetManager* toRemove);
  int removeLight(Light* toRemove);
  int removeCamera(Camera* toRemove);
  void addLight(unique_ptr_t<Light>&& newLight);
  void sortLightsByPriority();
  void addCamera(unique_ptr_t<Camera>&& newCamera);

  /**
   * @brief Switch active camera.
   * @param {Camera} newCamera - new active camera
   * @param {boolean} attachControl - call attachControl for the new active
   * camera (default: true)
   */
  void switchActiveCamera(Camera* newCamera, bool attachControl = true);

  /**
   * @brief Sets the active camera of the scene using its ID.
   * @param {string} id - the camera's ID
   * @return {BABYLON.Camera|null} the new active camera or null if none found.
   * @see activeCamera
   */
  Camera* setActiveCameraByID(const string_t& id);

  /**
   * @brief Sets the active camera of the scene using its name.
   * @param {string} name - the camera's name
   * @return {BABYLON.Camera|null} the new active camera or null if none found.
   * @see activeCamera
   */
  Camera* setActiveCameraByName(const string_t& name);

  /**
   * @brief Get a material using its id.
   * @param {string} the material's ID
   * @return {BABYLON.Material|null} the material or null if none found.
   */
  Material* getMaterialByID(const string_t& id);

  /**
   * @brief Get a material using its name.
   * @param {string} the material's name
   * @return {BABYLON.Material|null} the material or null if none found.
   */
  Material* getMaterialByName(const string_t& name);

  LensFlareSystem* getLensFlareSystemByName(const string_t& name);
  LensFlareSystem* getLensFlareSystemByID(const string_t& id);
  vector_t<Camera*> getCameras() const;
  Camera* getCameraByID(const string_t& id);
  Camera* getCameraByUniqueID(unsigned int uniqueId);

  /**
   * @brief Get a camera using its name.
   * @param {string} the camera's name
   * @return {BABYLON.Camera|null} the camera or null if none found.
   */
  Camera* getCameraByName(const string_t& name);

  /**
   * @brief Get a bone using its id.
   * @param {string} the bone's id
   * @return {BABYLON.Bone|null} the bone or null if not found
   */
  Bone* getBoneByID(const string_t& id);

  /**
   * @brief Get a bone using its id.
   * @param {string} the bone's name
   * @return {BABYLON.Bone|null} the bone or null if not found
   */
  Bone* getBoneByName(const string_t& name);

  /**
   * @brief Get a light node using its name.
   * @param {string} the light's name
   * @return {BABYLON.Light|null} the light or null if none found.
   */
  Light* getLightByName(const string_t& name);

  /**
   * @brief Get a light node using its ID.
   * @param {string} the light's id
   * @return {BABYLON.Light|null} the light or null if none found.
   */
  Light* getLightByID(const string_t& id);

  /**
   * @brief Get a light node using its scene-generated unique ID.
   * @param {number} the light's unique id
   * @return {BABYLON.Light|null} the light or null if none found.
   */
  Light* getLightByUniqueID(unsigned int uniqueId);

  /**
   * @brief Get a particle system by id.
   * @param id {number} the particle system id
   * @return {BABYLON.IParticleSystem|null} the corresponding system or null if
   * none found.
   */
  IParticleSystem* getParticleSystemByID(const string_t& id);

  /**
   * @brief Get a geometry using its ID.
   * @param {string} the geometry's id
   * @return {BABYLON.Geometry|null} the geometry or null if none found.
   */
  Geometry* getGeometryByID(const string_t& id);

  /**
   * @brief Add a new geometry to this scene.
   * @param {BABYLON.Geometry} geometry - the geometry to be added to the scene.
   * @param {boolean} [force] - force addition, even if a geometry with this ID
   * already exists
   * @return {boolean} was the geometry added or not
   */
  bool pushGeometry(unique_ptr_t<Geometry>&& geometry, bool force = false);

  /**
   * @brief Removes an existing geometry.
   * @param {BABYLON.Geometry} geometry - the geometry to be removed from the
   * scene.
   * @return {boolean} was the geometry removed or not
   */
  bool removeGeometry(Geometry* geometry);

  vector_t<unique_ptr_t<Geometry>>& getGeometries();

  /**
   * @brief Get the first added mesh found of a given ID.
   * @param {string} id - the id to search for
   * @return {BABYLON.AbstractMesh|null} the mesh found or null if not found at
   * all.
   */
  AbstractMesh* getMeshByID(const string_t& id);

  /**
   * @brief Get the all added meshes found of a given ID.
   * @param {string} id - the id to search for
   * @return {BABYLON.AbstractMesh[]|[]]} the meshes found or an empty list if
   * not found at all.
   */
  vector_t<AbstractMesh*> getMeshesByID(const string_t& id);

  /**
   * @brief Get a mesh with its auto-generated unique id.
   * @param {number} uniqueId - the unique id to search for
   * @return {BABYLON.AbstractMesh|null} the mesh found or null if not found at
   * all.
   */
  AbstractMesh* getMeshByUniqueID(unsigned int uniqueId);

  /**
   * @brief Get a the last added mesh found of a given ID.
   * @param {string} id - the id to search for
   * @return {BABYLON.AbstractMesh|null} the mesh found or null if not found at
   * all.
   */
  AbstractMesh* getLastMeshByID(const string_t& id);

  vector_t<AbstractMesh*> getMeshes() const;

  /**
   * @brief Get a the last added node (Mesh, Camera, Light) found of a given ID.
   * @param {string} id - the id to search for
   * @return {BABYLON.Node|null} the node found or null if not found at all.
   */
  Node* getLastEntryByID(const string_t& id);

  Node* getNodeByID(const string_t& id);
  Node* getNodeByName(const string_t& name);
  AbstractMesh* getMeshByName(const string_t& name);
  Sound* getSoundByName(const string_t& name);
  Skeleton* getLastSkeletonByID(const string_t& id);
  Skeleton* getSkeletonById(const string_t& id);
  Skeleton* getSkeletonByName(const string_t& name);
  MorphTargetManager* getMorphTargetManagerById(unsigned int id);
  bool isActiveMesh(Mesh* mesh);

  /**
   * @brief Return a the first highlight layer of the scene with a given name.
   * @param name The name of the highlight layer to look for.
   * @return The highlight layer if found otherwise null.
   */
  HighlightLayer* getHighlightLayerByName(const string_t& name);

  /**
   * @brief Return a unique id as a string which can serve as an identifier for
   * the scene.
   */
  string_t uid();

  bool _isInIntermediateRendering() const;

  /**
   * @brief Use this function to stop evaluating active meshes. The current list
   * will be keep alive between frames
   */
  Scene& freezeActiveMeshes();

  /**
   * @brief Use this function to restart evaluating active meshes on every frame
   */
  Scene& unfreezeActiveMeshes();

  void updateTransformMatrix(bool force = false);
  void updateAlternateTransformMatrix(Camera* alternateCamera);
  void render();

  /** Audio **/
  bool audioEnabled() const;
  void setAudioEnabled(bool value);
  bool headphone() const;
  void setHeadphone(bool value);
  void _switchAudioModeForHeadphones();
  void _switchAudioModeForNormalSpeakers();

  /** Rendering **/
  DepthRenderer* enableDepthRenderer();
  void disableDepthRenderer();
  GeometryBufferRenderer* enableGeometryBufferRenderer(float ratio = 1.f);
  void disableGeometryBufferRenderer();
  void freezeMaterials();
  void unfreezeMaterials();
  void dispose(bool doNotRecurse = false) override;
  bool isDisposed() const;

  /** Release sounds & sounds tracks **/
  void disposeSounds();

  /** Octrees **/
  MinMax getWorldExtends();
  Octree<AbstractMesh*>* createOrUpdateSelectionOctree(size_t maxCapacity = 64,
                                                       size_t maxDepth    = 2);

  /** Picking **/
  unique_ptr_t<Ray> createPickingRay(int x, int y, Matrix* world,
                                     Camera* camera,
                                     bool cameraViewSpace = false);
  unique_ptr_t<Ray> createPickingRayInCameraSpace(int x, int y, Camera* camera);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene
   * @param x X position on screen
   * @param y Y position on screen
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true.
   * @param fastCheck Launch a fast check only using the bounding boxes.
   * @param camera camera to use for computing the picking ray. Can be set to
   * nullptr. In this case, the scene.activeCamera will be used.
   * @return picking info object
   */
  PickingInfo* pick(int x, int y,
                    const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                    bool fastCheck = false, Camera* camera = nullptr);

  /**
   * @brief Launch a ray to try to pick a sprite in the scene
   * @param x X position on screen
   * @param y Y position on screen
   * @param predicate Predicate function used to determine eligible sprites. Can
   * be set to null. In this case, a sprite must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes.
   * @param camera camera to use for computing the picking ray. Can be set to
   * null. In this case, the scene.activeCamera will be used
   * @return picking info object
   */
  PickingInfo*
  pickSprite(int x, int y,
             const ::std::function<bool(Sprite* sprite)>& predicate,
             bool fastCheck = false, Camera* camera = nullptr);

  /**
   * @brief Use the given ray to pick a mesh in the scene
   * @param ray The ray to use to pick meshes
   * @param predicate Predicate function used to determine eligible sprites. Can
   * be set to null. In this case, a sprite must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be
   * set to null.
   */
  PickingInfo* pickWithRay(const Ray& ray,
                           const ::std::function<bool(Mesh* mesh)>& predicate,
                           bool fastCheck = false);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene
   * @param x X position on screen
   * @param y Y position on screen
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true
   * @param camera camera to use for computing the picking ray. Can be set to
   * null. In this case, the scene.activeCamera will be used
   * @return list with picking info objects
   */
  vector_t<PickingInfo*>
  multiPick(int x, int y,
            const ::std::function<bool(AbstractMesh* mesh)>& predicate,
            Camera* camera);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene
   * @param ray Ray to use
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true
   * @return list with picking info objects
   */
  vector_t<PickingInfo*>
  multiPickWithRay(const Ray& ray,
                   const ::std::function<bool(Mesh* mesh)>& predicate);

  AbstractMesh* getPointerOverMesh();
  void setPointerOverMesh(AbstractMesh* mesh);
  void setPointerOverSprite(Sprite* sprite);
  Sprite* getPointerOverSprite() const;

  /** Physics **/
  PhysicsEngine* getPhysicsEngine();

  /**
   * Enables physics to the current scene
   * @param {BABYLON.Vector3} [gravity] - the scene's gravity for the physics
   * engine
   * @param {BABYLON.IPhysicsEnginePlugin} [plugin] - The physics engine to be
   * used. defaults to OimoJS.
   * @return {boolean} was the physics engine initialized
   */
  bool enablePhysics(const Vector3& gravity, IPhysicsEnginePlugin* plugin);

  void disablePhysicsEngine();
  bool isPhysicsEnabled();

  /** Misc. **/
  void _rebuildGeometries();
  void _rebuildTextures();
  void createDefaultCameraOrLight(bool createArcRotateCamera = false,
                                  bool replace               = false,
                                  bool attachCameraControls  = false);
  Mesh* createDefaultSkybox(BaseTexture* environmentTexture = nullptr,
                            bool pbr = false, float scale = 1000.f,
                            float blur = 0.f);

  /** Tags **/
  vector_t<Mesh*> getMeshesByTags();
  vector_t<Camera*> getCamerasByTags();
  vector_t<Light*> getLightsByTags();
  vector_t<Material*> getMaterialByTags();

  /**
   * @brief Overrides the default sort function applied in the renderging group
   * to prepare the meshes.
   * This allowed control for front to back rendering or reversly depending of
   * the special needs.
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param opaqueSortCompareFn The opaque queue comparison function use to
   * sort.
   * @param alphaTestSortCompareFn The alpha test queue comparison function use
   * to sort.
   * @param transparentSortCompareFn The transparent queue comparison function
   * use to sort.
   */
  void setRenderingOrder(
    unsigned int renderingGroupId,
    const ::std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn
    = nullptr,
    const ::std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn
    = nullptr,
    const ::std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn
    = nullptr);

  /**
   * @brief Specifies whether or not the stencil and depth buffer are cleared
   * between two rendering groups.
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param autoClearDepthStencil Automatically clears depth and stencil between
   * groups if true.
   * @param depth Automatically clears depth between groups if true and
   * autoClear is true.
   * @param stencil Automatically clears stencil between groups if true and
   * autoClear is true.
   */
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                         bool autoClearDepthStencil,
                                         bool depth   = true,
                                         bool stencil = true);

  /**
   * @brief Will flag all materials as dirty to trigger new shader compilation
   * @param predicate If not null, it will be used to specifiy if a material has
   * to be marked as dirty
   */
  void
  markAllMaterialsAsDirty(unsigned int flag,
                          const ::std::function<bool(Material* mat)>& predicate
                          = nullptr);

protected:
  /**
   * @brief Constructor.
   * @param {BABYLON.Engine} engine - the engine to be used to render this
   * scene.
   */
  explicit Scene(Engine* engine);

private:
  void _updatePointerPosition(const PointerEvent evt);
  void _createUbo();
  void _createAlternateUbo();
  Scene& _processPointerMove(const PickingInfo* pickResult,
                             const PointerEvent& evt);
  Scene& _processPointerDown(const PickingInfo* pickResult,
                             const PointerEvent& evt);
  Scene& _processPointerUp(const PickingInfo* pickResult,
                           const PointerEvent& evt, const ClickInfo& clickInfo);
  void _animate();
  void _evaluateSubMesh(SubMesh* subMesh, AbstractMesh* mesh);
  void _evaluateActiveMeshes();
  void _activeMesh(AbstractMesh* mesh);
  void _renderForCamera(Camera* camera);
  void _processSubCameras(Camera* camera);
  void _checkIntersections();
  void _updateAudioParameters();
  /** Pointers handling **/
  void _onPointerMoveEvent(PointerEvent&& evt);
  void _onPointerDownEvent(PointerEvent&& evt);
  void _onPointerUpEvent(PointerEvent&& evt);
  void _onKeyDownEvent(Event&& evt);
  void _onKeyUpEvent(Event&& evt);
  /** Audio **/
  void _disableAudio();
  void _enableAudio();
  // void _switchAudioModeForHeadphones();
  // void _switchAudioModeForNormalSpeakers();
  /** Picking **/
  PickingInfo*
  _internalPick(const ::std::function<Ray(const Matrix& world)>& rayFunction,
                const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                bool fastCheck);
  vector_t<PickingInfo*> _internalMultiPick(
    const ::std::function<Ray(const Matrix& world)>& rayFunction,
    const ::std::function<bool(AbstractMesh* mesh)>& predicate);
  PickingInfo*
  _internalPickSprites(const Ray& ray,
                       const ::std::function<bool(Sprite* sprite)>& predicate,
                       bool fastCheck, Camera* camera);
  /** Tags **/
  vector_t<string_t> _getByTags();

public:
  // Members
  bool autoClear;
  bool autoClearDepthAndStencil;
  Color4 clearColor;
  Color3 ambientColor;
  BaseTexture* _environmentBRDFTexture;
  // Events
  /**
   * An event triggered when the scene is disposed.
   */
  Observable<Scene> onDisposeObservable;
  /**
   * An event triggered before rendering the scene
   */
  Observable<Scene> onBeforeRenderObservable;
  /**
   * An event triggered after rendering the scene
   */
  Observable<Scene> onAfterRenderObservable;

  /**
   * An event triggered before animating the scene
   */
  Observable<Scene> onBeforeAnimationsObservable;

  /**
   * An event triggered after animations processing
   */
  Observable<Scene> onAfterAnimationsObservable;

  /**
   * An event triggered before draw calls are ready to be sent
   */
  Observable<Scene> onBeforeDrawPhaseObservable;

  /**
   * An event triggered after draw calls have been sent
   */
  Observable<Scene> onAfterDrawPhaseObservable;

  /**
   * An event triggered when physic simulation is about to be run
   */
  Observable<Scene> onBeforePhysicsObservable;

  /**
   * An event triggered when physic simulation has been done
   */
  Observable<Scene> onAfterPhysicsObservable;

  /**
   * An event triggered after rendering the scene
   */
  Observable<Scene> onReadyObservable;
  /**
   * An event triggered before rendering a camera
   */
  Observable<Camera> onBeforeCameraRenderObservable;
  /**
   * An event triggered after rendering a camera
   */
  Observable<Camera> onAfterCameraRenderObservable;

  /**
   * An event triggered when active meshes evaluation is about to start
   */
  Observable<Scene> onBeforeActiveMeshesEvaluationObservable;

  /**
   * An event triggered when active meshes evaluation is done
   */
  Observable<Scene> onAfterActiveMeshesEvaluationObservable;

  /**
   * An event triggered when particles rendering is about to start
   * Note: This event can be trigger more than once per frame (because particles
   * can be rendered by render target textures as well)
   */
  Observable<Scene> onBeforeParticlesRenderingObservable;

  /**
   * An event triggered when particles rendering is done
   * Note: This event can be trigger more than once per frame (because particles
   * can be rendered by render target textures as well)
  */
  Observable<Scene> onAfterParticlesRenderingObservable;

  /**
   * An event triggered when sprites rendering is about to start
   * Note: This event can be trigger more than once per frame (because sprites
   * can be rendered by render target textures as well)
   */
  Observable<Scene> onBeforeSpritesRenderingObservable;

  /**
   * An event triggered when sprites rendering is done
   * Note: This event can be trigger more than once per frame (because sprites
   * can be rendered by render target textures as well)
   */
  Observable<Scene> onAfterSpritesRenderingObservable;

  /**
   * An event triggered when a camera is created
   */
  Observable<Camera> onNewCameraAddedObservable;

  /**
   * An event triggered when a camera is removed
   */
  Observable<Camera> onCameraRemovedObservable;

  /**
   * An event triggered when a light is created
   */
  Observable<Light> onNewLightAddedObservable;

  /**
   * An event triggered when a light is removed
   */
  Observable<Light> onLightRemovedObservable;

  /**
   * An event triggered when a geometry is created
   */
  Observable<Geometry> onNewGeometryAddedObservable;

  /**
   * An event triggered when a geometry is removed
   */
  Observable<Geometry> onGeometryRemovedObservable;

  /**
   * An event triggered when a transform node is created
   */
  Observable<TransformNode> onNewTransformNodeAddedObservable;

  /**
   * An event triggered when a transform node is removed
   */
  Observable<TransformNode> onTransformNodeRemovedObservable;

  /**
   * An event triggered when a mesh is created
   */
  Observable<AbstractMesh> onNewMeshAddedObservable;

  /**
   * An event triggered when a mesh is removed
   */
  Observable<AbstractMesh> onMeshRemovedObservable;

  /**
   * An event triggered when render targets are about to be rendered
   * Can happen multiple times per frame.
   */
  Observable<Scene> OnBeforeRenderTargetsRenderObservable;

  /**
   * An event triggered when render targets were rendered.
   * Can happen multiple times per frame.
   */
  Observable<Scene> OnAfterRenderTargetsRenderObservable;

  /**
   * An event triggered before calculating deterministic simulation step
   */
  Observable<Scene> onBeforeStepObservable;

  /**
   * An event triggered after calculating deterministic simulation step
   */
  Observable<Scene> onAfterStepObservable;

  /**
   * This Observable will be triggered for each stage of each renderingGroup of
   * each rendered camera. The RenderinGroupInfo class contains all the
   * information about the context in which the observable is called If you wish
   * to register an Observer only for a given set of renderingGroup, use the
   * mask with a combination of the renderingGroup index elevated to the power
   * of two (1 for renderingGroup 0, 2 for renderingrOup1, 4 for 2 and 8 for 3)
   */
  Observable<RenderingGroupInfo> onRenderingGroupObservable;

  // Animations
  vector_t<Animation*> animations;

  // Pointers
  ::std::function<bool(AbstractMesh* Mesh)> pointerDownPredicate;
  ::std::function<bool(AbstractMesh* Mesh)> pointerUpPredicate;
  ::std::function<bool(AbstractMesh* Mesh)> pointerMovePredicate;

  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo)>
    onPointerMove;
  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo)>
    onPointerDown;
  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo)>
    onPointerUp;
  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo)>
    onPointerPick;

  bool forceWireframe;
  bool forceShowBoundingBoxes;
  unique_ptr_t<Plane> _clipPlane;
  bool animationsEnabled;
  bool constantlyUpdateMeshUnderPointer;

  string_t hoverCursor;
  string_t defaultCursor;

  // Metadata
  Json::object metadata;
  string_t loadingPluginName;

  // Pointers

  /**
   * This observable event is triggered when any ponter event is triggered. It
   * is registered during Scene.attachControl() and it is called BEFORE the 3D
   * engine process anything (mesh/sprite picking for instance).
   * You have the possibility to skip the process and the call to
   * onPointerObservable by setting PointerInfoPre.skipOnPointerObservable to
   * true
   */
  Observable<PointerInfoPre> onPrePointerObservable;

  /**
   * Observable event triggered each time an input event is received from the
   * rendering canvas
   */
  Observable<PointerInfo> onPointerObservable;

  /**
   * Define this parameter if you are using multiple cameras and you want to
   * specify which one should be used for pointer position.
   */
  Camera* cameraToUseForPointers;

  // Mirror
  unique_ptr_t<Vector3> _mirroredCameraPosition;

  // Keyboard

  /**
   * This observable event is triggered when any keyboard event si raised and
   * registered during Scene.attachControl()
   * You have the possibility to skip the process and the call to
   * onKeyboardObservable by setting KeyboardInfoPre.skipOnPointerObservable to
   * true
   */
  Observable<KeyboardInfoPre> onPreKeyboardObservable;

  /**
   * Observable event triggered each time an keyboard event is received from the
   * hosting window
   */
  Observable<KeyboardInfo> onKeyboardObservable;

  Color3 fogColor;
  float fogDensity;
  float fogStart;
  float fogEnd;
  // Lights
  /**
   * All of the lights added to this scene.
   * @see BABYLON.Light
   */
  vector_t<unique_ptr_t<Light>> lights;
  // Cameras
  /**
   * All of the cameras added to this scene.
   */

  vector_t<unique_ptr_t<Camera>> cameras;
  /**
   * All of the active cameras added to this scene.
   */
  vector_t<Camera*> activeCameras;

  /**
   * The current active camera
   */
  Camera* activeCamera;

  // Meshes
  /**
   * All of the tranform nodes added to this scene.
   */
  vector_t<TransformNode*> transformNodes;
  /**
   * All of the (abstract) meshes added to this scene.
   * @see BABYLON.AbstractMesh
   */
  vector_t<unique_ptr_t<AbstractMesh>> meshes;
  // Geometries
  vector_t<unique_ptr_t<Material>> materials;
  vector_t<unique_ptr_t<MultiMaterial>> multiMaterials;
  // Textures
  vector_t<unique_ptr_t<BaseTexture>> textures;
  // Particles
  bool particlesEnabled;
  vector_t<unique_ptr_t<IParticleSystem>> particleSystems;
  // Sprites
  bool spritesEnabled;
  vector_t<unique_ptr_t<SpriteManager>> spriteManagers;
  ::std::function<bool(Sprite* sprite)> spritePredicate;
  // Layers
  vector_t<Layer*> layers;
  vector_t<unique_ptr_t<HighlightLayer>> highlightLayers;
  // Skeletons
  vector_t<unique_ptr_t<Skeleton>> skeletons;
  // Morph targets
  vector_t<unique_ptr_t<MorphTargetManager>> morphTargetManagers;
  // Lens flares
  bool lensFlaresEnabled;
  vector_t<unique_ptr_t<LensFlareSystem>> lensFlareSystems;
  // Collisions
  bool collisionsEnabled;
  unique_ptr_t<ICollisionCoordinator> collisionCoordinator;
  /** Defines the gravity applied to this scene */
  Vector3 gravity;
  // Postprocesses
  vector_t<PostProcess*> postProcesses;
  bool postProcessesEnabled;
  unique_ptr_t<PostProcessManager> postProcessManager;
  // Customs render targets
  bool renderTargetsEnabled;
  bool dumpNextRenderTargets;
  vector_t<RenderTargetTexture*> customRenderTargets;
  // Delay loading
  bool useDelayedTextureLoading;
  // Imported meshes
  vector_t<string_t> importedMeshesFiles;
  // Probes
  bool probesEnabled;
  vector_t<unique_ptr_t<ReflectionProbe>> reflectionProbes;
  // Database
  // public database; //ANY
  // Actions
  /**
   * This scene's action manager
   */
  ActionManager* actionManager;
  vector_t<unique_ptr_t<ActionManager>> _actionManagers;
  // Procedural textures
  bool proceduralTexturesEnabled;
  vector_t<unique_ptr_t<ProceduralTexture>> _proceduralTextures;
  // Sound Tracks
  vector_t<SoundTrack*> soundTracks;
  // Simplification Queue
  unique_ptr_t<SimplificationQueue> simplificationQueue;
  // Performance counters
  PerfCounter _activeIndices;
  PerfCounter _activeParticles;
  PerfCounter _particlesDuration;
  PerfCounter _spritesDuration;
  PerfCounter _activeBones;
  Material* _cachedMaterial;
  Effect* _cachedEffect;
  float _cachedVisibility;
  vector_t<IDisposable*> _toBeDisposed;
  vector_t<IParticleSystem*> _activeParticleSystems;
  vector_t<Animatable*> _activeAnimatables;
  bool requireLightSorting;

protected:
  BaseTexture* _environmentTexture;
  unique_ptr_t<ImageProcessingConfiguration> _imageProcessingConfiguration;

private:
  // Events
  ::std::function<bool(Sprite* sprite)> _spritePredicate;
  Observer<Scene>::Ptr _onDisposeObserver;
  Observer<Scene>::Ptr _onBeforeRenderObserver;
  Observer<Scene>::Ptr _onAfterRenderObserver;
  Observer<Camera>::Ptr _onBeforeCameraRenderObserver;
  Observer<Camera>::Ptr _onAfterCameraRenderObserver;
  // Pointers
  ::std::function<void(PointerEvent&& evt)> _onPointerMove;
  ::std::function<void(PointerEvent&& evt)> _onPointerDown;
  ::std::function<void(PointerEvent&& evt)> _onPointerUp;
  // Gamepads
  unique_ptr_t<GamepadManager> _gamepadManager;
  // Click events
  ::std::function<void(
    Observable<PointerInfoPre>& obs1, Observable<PointerInfo>& obs2,
    PointerEvent&& evt,
    const ::std::function<void(const ClickInfo& clickInfo,
                               const PointerInfo& pickResult)>& cb)>
    _initClickEvent;
  ::std::function<ActionManager*(ActionManager* act,
                                 const ClickInfo& clickInfo)>
    _initActionManager;
  ::std::function<void(
    unsigned int btn, const ClickInfo& clickInfo,
    const ::std::function<void(const ClickInfo& clickInfo,
                               const PointerInfo& pickResult)>& cb)>
    _delayedSimpleClick;
  milliseconds_t _delayedSimpleClickTimeout;
  milliseconds_t _previousDelayedSimpleClickTimeout;
  bool _meshPickProceed;
  bool _previousButtonPressed;
  bool _previousHasSwiped;
  unique_ptr_t<PickingInfo> _currentPickResult;
  unique_ptr_t<PickingInfo> _previousPickResult;
  bool _isButtonPressed;
  bool _doubleClickOccured;
  int _pointerX;
  int _pointerY;
  int _unTranslatedPointerX;
  int _unTranslatedPointerY;
  Vector2 _startingPointerPosition;
  Vector2 _previousStartingPointerPosition;
  high_res_time_point_t _startingPointerTime;
  high_res_time_point_t _previousStartingPointerTime;
  // AbstractMesh* _meshUnderPointer;
  ::std::function<void()> beforeRender;
  ::std::function<void()> afterRender;
  // Deterministic lockstep
  unsigned int _timeAccumulator;
  unsigned int _currentStepId;
  unsigned int _currentInternalStep;
  // Keyboard
  ::std::function<void(Event&& evt)> _onKeyDown;
  ::std::function<void(Event&& evt)> _onKeyUp;
  Observer<Engine>::Ptr _onCanvasFocusObserver;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  // Coordinate system
  bool _useRightHandedSystem;
  // Members
  bool _forcePointsCloud;
  // Fog
  /**
   * is fog enabled on this scene.
   */
  bool _fogEnabled;
  unsigned int _fogMode;
  // Lights
  /**
   * is shadow enabled on this scene.
   */
  bool _shadowsEnabled;
  /**
   * is light enabled on this scene.
   */
  bool _lightsEnabled;
  // Geometries
  vector_t<unique_ptr_t<Geometry>> _geometries;
  // Materials
  Material* _defaultMaterial;
  // Textures
  bool _texturesEnabled;
  // Skeletons
  bool _skeletonsEnabled;
  // Postprocesses
  unique_ptr_t<PostProcessRenderPipelineManager>
    _postProcessRenderPipelineManager;
  // Collisions
  bool _workerCollisions;
  // Actions
  vector_t<AbstractMesh*> _meshesForIntersections;
  // Sound Tracks
  bool _hasAudioEngine;
  unique_ptr_t<SoundTrack> _mainSoundTrack;
  bool _audioEnabled;
  bool _headphone;
  // Render engine
  Engine* _engine;
  // Performance counters
  PerfCounter _totalMeshesCounter;
  PerfCounter _totalLightsCounter;
  PerfCounter _totalMaterialsCounter;
  PerfCounter _totalTexturesCounter;
  PerfCounter _totalVertices;
  PerfCounter _interFrameDuration;
  PerfCounter _lastFrameDuration;
  PerfCounter _evaluateActiveMeshesDuration;
  PerfCounter _renderTargetsDuration;
  PerfCounter _renderDuration;
  float _animationRatio;
  bool _animationTimeLastSet;
  high_res_time_point_t _animationTimeLast;
  int _animationTime;
  size_t animationTimeScale;
  int _renderId;
  int _executeWhenReadyTimeoutId;
  bool _intermediateRendering;
  int _viewUpdateFlag;
  int _projectionUpdateFlag;
  int _alternateViewUpdateFlag;
  int _alternateProjectionUpdateFlag;
  vector_t<string_t> _pendingData;
  vector_t<Mesh*> _activeMeshes;
  bool _activeMeshesFrozen;
  vector_t<Material*> _processedMaterials;
  vector_t<RenderTargetTexture*> _renderTargets;
  vector_t<Skeleton*> _activeSkeletons;
  vector_t<Mesh*> _softwareSkinnedMeshes;
  unique_ptr_t<RenderingManager> _renderingManager;
  unique_ptr_t<PhysicsEngine> _physicsEngine;
  Matrix _transformMatrix;
  unique_ptr_t<UniformBuffer> _sceneUbo;
  unique_ptr_t<UniformBuffer> _alternateSceneUbo;
  Matrix _pickWithRayInverseMatrix;
  unique_ptr_t<BoundingBoxRenderer> _boundingBoxRenderer;
  unique_ptr_t<OutlineRenderer> _outlineRenderer;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _alternateViewMatrix;
  Matrix _alternateProjectionMatrix;
  unique_ptr_t<Matrix> _alternateTransformMatrix;
  bool _useAlternateCameraConfiguration;
  bool _alternateRendering;
  bool _frustumPlanesSet;
  array_t<Plane, 6> _frustumPlanes;
  Octree<AbstractMesh*>* _selectionOctree;
  AbstractMesh* _pointerOverMesh;
  Sprite* _pointerOverSprite;
  unique_ptr_t<DebugLayer> _debugLayer;
  unique_ptr_t<DepthRenderer> _depthRenderer;
  unique_ptr_t<GeometryBufferRenderer> _geometryBufferRenderer;
  unsigned int _uniqueIdCounter;
  AbstractMesh* _pickedDownMesh;
  AbstractMesh* _pickedUpMesh;
  Sprite* _pickedDownSprite;
  string_t _uid;

}; // end of class Scene

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_SCENE_H
