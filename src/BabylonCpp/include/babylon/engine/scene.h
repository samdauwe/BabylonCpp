#ifndef BABYLON_ENGINE_SCENE_H
#define BABYLON_ENGINE_SCENE_H

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/core/variant.h>
#include <babylon/culling/octrees/octree.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/events/pointer_info.h>
#include <babylon/events/pointer_info_pre.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>
#include <babylon/tools/ifile_request.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>
#include <babylon/tools/perf_counter.h>

namespace BABYLON {

/**
 * @brief Represents a scene to be rendered by the engine.
 * @see http://doc.babylonjs.com/features/scene
 */
class BABYLON_SHARED_EXPORT Scene : public IAnimatable {

private:
  // Statics
  static constexpr unsigned int _FOGMODE_NONE   = 0;
  static constexpr unsigned int _FOGMODE_EXP    = 1;
  static constexpr unsigned int _FOGMODE_EXP2   = 2;
  static constexpr unsigned int _FOGMODE_LINEAR = 3;

public:
  static size_t _uniqueIdCounter;

  /**
   * Gets or sets the minimum deltatime when deterministic lock step is enabled
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  static microseconds_t MinDeltaTime;

  /**
   * Gets or sets the maximum deltatime when deterministic lock step is enabled
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   */
  static microseconds_t MaxDeltaTime;

  /** The fog is deactivated */
  static constexpr unsigned int FOGMODE_NONE()
  {
    return Scene::_FOGMODE_NONE;
  }

  /** The fog density is following an exponential function */
  static constexpr unsigned int FOGMODE_EXP()
  {
    return Scene::_FOGMODE_EXP;
  }

  /** The fog density is following an exponential function faster than
   * FOGMODE_EXP */
  static constexpr unsigned int FOGMODE_EXP2()
  {
    return Scene::_FOGMODE_EXP2;
  }

  /** The fog density is following a linear function. */
  static constexpr unsigned int FOGMODE_LINEAR()
  {
    return Scene::_FOGMODE_LINEAR;
  }

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
  virtual ~Scene() override;

  IReflect::Type type() const override;

  // Events
  /**
   * @brief Sets a function to be executed when this scene is disposed.
   */
  void setOnDispose(
    const ::std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed before rendering this scene.
   */
  void setBeforeRender(
    const ::std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed after rendering this scene.
   */
  void setAfterRender(
    const ::std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed before rendering a camera.
   */
  void setBeforeCameraRender(
    const ::std::function<void(Camera* camera, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed after rendering a camera.
   */
  void setAfterCameraRender(
    const ::std::function<void(Camera* camera, EventState& es)>& callback);

  // Gamepads

  /**
   * @brief Gets the gamepad manager associated with the scene.
   * @see http://doc.babylonjs.com/how_to/how_to_use_gamepads
   */
  GamepadManager& gamepadManager();

  // Pointers

  /**
   * @brief Gets the pointer coordinates without any translation (ie. straight
   * out of the pointer event).
   */
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

  /**
   * @brief Gets a boolean indicating if the scene must use right-handed
   * coordinates system.
   */
  bool useRightHandedSystem() const;

  /**
   * @brief Sets a boolean indicating if the scene must use right-handed
   * coordinates system.
   */
  void setUseRightHandedSystem(bool value);

  /**
   * @brief Sets the step Id used by deterministic lock step.
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @param newStepId defines the step Id
   */
  void setStepId(unsigned int newStepId);

  /**
   * @brief Gets the step Id used by deterministic lock step.
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns the step Id
   */
  unsigned int getStepId() const;

  /**
   * @brief Gets the internal step used by deterministic lock step.
   * @see http://doc.babylonjs.com/babylon101/animations#deterministic-lockstep
   * @returns the internal step
   */
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

  /**
   * @brief Gets a boolean indicating if all rendering must be done in
   * wireframe.
   */
  bool forceWireframe() const;

  /**
   * @brief Sets a boolean indicating if all rendering must be done in
   * wireframe.
   */
  void setForceWireframe(bool value);

  /**
   * @brief Gets a boolean indicating if all rendering must be done in point
   * cloud.
   */
  bool forcePointsCloud() const;

  /**
   * @brief Sets a boolean indicating if all rendering must be done in point
   * cloud.
   */
  void setForcePointsCloud(bool value);

  /**
   * @brief Gets or sets a boolean indicating if fog is enabled on this scene.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  bool fogEnabled() const;

  /**
   * @brief Gets or sets a boolean indicating if fog is enabled on this scene.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  void setFogEnabled(bool value);

  /**
   * @brief Gets the fog mode to use.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  bool fogMode() const;

  /**
   * @brief Sets the fog mode to use.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  void setFogMode(unsigned int value);

  /**
   * @brief Gets a boolean indicating if shadows are enabled on this scene.
   */
  bool shadowsEnabled() const;

  /**
   * @brief Sets a boolean indicating if shadows are enabled on this scene.
   */
  void setShadowsEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if lights are enabled on this scene.
   */
  bool lightsEnabled() const;

  /**
   * @brief Sets a boolean indicating if lights are enabled on this scene.
   */
  void setLightsEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if textures are enabled on this scene.
   */
  bool texturesEnabled() const;

  /**
   * @brief Sets a boolean indicating if textures are enabled on this scene.
   */
  void setTexturesEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if skeletons are enabled on this scene.
   */
  bool skeletonsEnabled() const;

  /**
   * @brief Sets a boolean indicating if skeletons are enabled on this scene.
   */
  void setSkeletonsEnabled(bool value);

  /**
   * @brief Gets the postprocess render pipeline manager.
   * @see http://doc.babylonjs.com/how_to/how_to_use_postprocessrenderpipeline
   * @see http://doc.babylonjs.com/how_to/using_default_rendering_pipeline
   */
  PostProcessRenderPipelineManager* postProcessRenderPipelineManager();

  /**
   * @brief Gets the main soundtrack associated with the scene.
   */
  SoundTrack* mainSoundTrack();

  /**
   * @brief Gets the current geometry buffer associated to the scene.
   */
  shared_ptr_t<GeometryBufferRenderer>& geometryBufferRenderer();

  /**
   * @brief Sets the current geometry buffer for the scene.
   */
  void setGeometryBufferRenderer(
    const shared_ptr_t<GeometryBufferRenderer>& geometryBufferRenderer);

  Plane* clipPlane();
  void setClipPlane(const Plane& plane);
  void resetClipPlane();
  void setMirroredCameraPosition(const Vector3& newPosition);

  /**
   * @brief Gets the default material used on meshes when no material is
   * affected.
   */
  Material* defaultMaterial();

  /**
   * @brief Sets the default material used on meshes when no material is
   * affected.
   */
  void setDefaultMaterial(Material* value);

  /**
   * @brief Hidden
   */
  bool _isAlternateRenderingEnabled() const;

  /**
   * @brief Gets the list of frustum planes (built from the active camera).
   */
  array_t<Plane, 6>& frustumPlanes();

  /**
   * @brief Gets the list of frustum planes (built from the active camera).
   */
  const array_t<Plane, 6>& frustumPlanes() const;

  /**
   * @brief Gets the debug layer associated with the scene.
   * @see http://doc.babylonjs.com/features/playground_debuglayer
   */
  DebugLayer* debugLayer();

  /**
   * @brief Sets a boolean indicating if collisions are processed on a web
   * worker.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#web-worker-based-collision-system-since-21
   */
  void setWorkerCollisions(bool enabled);

  /**
   * @brief Gets a boolean indicating if collisions are processed on a web
   * worker.
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity#web-worker-based-collision-system-since-21
   */
  bool workerCollisions() const;

  /**
   * @brief Gets the octree used to boost mesh selection (picking).
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
   */
  Octree<AbstractMesh*>* selectionOctree();

  /**
   * @brief Gets the mesh that is currently under the pointer.
   */
  AbstractMesh* meshUnderPointer();

  /**
   * @brief Gets the current on-screen X position of the pointer.
   */
  int pointerX();

  /**
   * @brief Gets the current on-screen Y position of the pointer.
   */
  int pointerY();

  /**
   * @brief Gets the cached material (ie. the latest rendered one).
   * @returns the cached material
   */
  Material* getCachedMaterial();

  /**
   * @brief Gets the cached effect (ie. the latest rendered one).
   * @returns the cached effect
   */
  Effect* getCachedEffect();

  /**
   * @brief Gets the cached visibility state (ie. the latest rendered one).
   * @returns the cached visibility state
   */
  float getCachedVisibility();

  /**
   * @brief Gets a boolean indicating if the current material / effect /
   * visibility must be bind again.
   * @param material defines the current material
   * @param effect defines the current effect
   * @param visibility defines the current visibility state
   * @returns true if one parameter is not cached
   */
  bool isCachedMaterialInvalid(Material* material, Effect* effect,
                               float visibility = 1.f);

  /**
   * @brief Gets the bounding box renderer associated with the scene.
   * @returns a BoundingBoxRenderer
   */
  BoundingBoxRenderer* getBoundingBoxRenderer();

  /**
   * @brief Gets the outline renderer associated with the scene.
   * @returns a OutlineRenderer
   */
  OutlineRenderer* getOutlineRenderer();

  /**
   * @brief Gets the engine associated with the scene.
   * @returns an Engine
   */
  Engine* getEngine();

  /**
   * @brief Gets the total number of vertices rendered per frame.
   * @returns the total number of vertices rendered per frame
   */
  size_t getTotalVertices() const;

  /**
   * @brief Gets the performance counter for total vertices.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& totalVerticesPerfCounter();

  /**
   * @brief Gets the total number of active indices rendered per frame (You can
   * deduce the number of rendered triangles by dividing this number by 3).
   * @returns the total number of active indices rendered per frame
   */
  size_t getActiveIndices() const;

  /**
   * @brief Gets the performance counter for active indices.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& totalActiveIndicesPerfCounter();

  /**
   * @brief Gets the total number of active particles rendered per frame.
   * @returns the total number of active particles rendered per frame
   */
  size_t getActiveParticles() const;

  /**
   * @brief Gets the performance counter for active particles.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& activeParticlesPerfCounter();

  /**
   * @brief Gets the total number of active bones rendered per frame.
   * @returns the total number of active bones rendered per frame
   */
  size_t getActiveBones() const;

  /**
   * @brief Gets the performance counter for active bones.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& activeBonesPerfCounter();

  /** Stats **/

  /**
   * @brief Gets the array of active meshes.
   * @returns an array of AbstractMesh
   */
  vector_t<AbstractMesh*>& getActiveMeshes();

  /**
   * @brief Gets the array of active meshes.
   * @returns an array of AbstractMesh
   */
  const vector_t<AbstractMesh*>& getActiveMeshes() const;

  /**
   * @brief Gets the animation ratio (which is 1.0 is the scene renders at 60fps
   * and 2 if the scene renders at 30fps, etc.).
   * @returns a number
   */
  float getAnimationRatio() const;

  /**
   * @brief Gets an unique Id for the current frame.
   * @returns a number
   */
  int getRenderId() const;

  /**
   * @brief Call this function if you want to manually increment the render Id.
   */
  void incrementRenderId();

  /** Pointers handling **/

  /**
   * @brief Use this method to simulate a pointer move on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer
   * event on
   * @param pointerEventInit pointer event state to be used when simulating the
   * pointer event (eg. pointer id for multitouch)
   * @returns the current scene
   */
  Scene& simulatePointerMove(const PickingInfo* pickResult);

  /**
   * @brief Use this method to simulate a pointer down on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer
   * event on
   * @param pointerEventInit pointer event state to be used when simulating the
   * pointer event (eg. pointer id for multitouch)
   * @returns the current scene
   */
  Scene& simulatePointerDown(const PickingInfo* pickResult);

  /**
   * @brief Use this method to simulate a pointer up on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer
   * event on
   * @param pointerEventInit pointer event state to be used when simulating the
   * pointer event (eg. pointer id for multitouch)
   * @returns the current scene
   */
  Scene& simulatePointerUp(const PickingInfo* pickResult);

  /**
   * @brief Attach events to the canvas (To handle actionManagers triggers and
   * raise onPointerMove, onPointerDown and onPointerUp.
   * @param attachUp defines if you want to attach events to pointerup
   * @param attachDown defines if you want to attach events to pointerdown
   * @param attachMove defines if you want to attach events to pointermove
   */
  void attachControl(bool attachUp = true, bool attachDown = true,
                     bool attachMove = true);

  /**
   * @brief Detaches all event handlers.
   */
  void detachControl();

  /**
   * @brief This function will check if the scene can be rendered (textures are.
   * loaded, shaders are compiled) Delay loaded resources are not taking in
   * account.
   * @return true if all required resources are ready
   */
  bool isReady();

  /**
   * @brief Resets all cached information relative to material (including effect
   * and visibility).
   */
  void resetCachedMaterial();

  /**
   * @brief Registers a function to be called before every frame render.
   * @param func defines the function to register
   */
  void registerBeforeRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * Unregisters a function called before every frame render
   * @param func defines the function to unregister
   */
  void unregisterBeforeRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Registers a function to be called after every frame render.
   * @param func defines the function to register
   */
  void registerAfterRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Unregisters a function called after every frame render.
   * @param func defines the function to unregister
   */
  void unregisterAfterRender(
    const ::std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Hidden
   */
  void _addPendingData(Mesh* mesh);

  /**
   * @brief Hidden
   */
  void _addPendingData(InternalTexture* texure);

  /**
   * @brief Hidden
   */
  void _removePendingData(InternalTexture* texture);

  /**
   * @brief Returns the number of items waiting to be loaded.
   * @returns the number of items waiting to be loaded
   */
  void getWaitingItemsCount();

  /**
   * @brief Returns a boolean indicating if the scene is still loading data.
   */
  bool isLoading() const;

  /**
   * Registers a function to be executed when the scene is ready
   * @param {Function} func - the function to be executed
   */
  void executeWhenReady(
    const ::std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Hidden
   */
  void _checkIsReady();

  /** Animations **/

  /**
   * @brief Gets all animations attached to the scene.
   */
  vector_t<Animation*> getAnimations() override;

  /**
   * @brief Will start the animation sequence of a given target.
   * @param target defines the target
   * @param from defines from which frame should animation start
   * @param to defines until which frame should animation run.
   * @param weight defines the weight to apply to the animation (1.0 by default)
   * @param loop defines if the animation loops
   * @param speedRatio defines the speed in which to run the animation (1.0 by
   * default)
   * @param onAnimationEnd defines the function to be executed when the
   * animation ends
   * @param animatable defines an animatable object. If not provided a new one
   * will be created from the given params
   * @returns the animatable object created for this animation
   */
  Animatable* beginWeightedAnimation(
    IAnimatable* target, int from, int to, float weight = 1.f,
    bool loop = false, float speedRatio = 1.f,
    const ::std::function<void()>& onAnimationEnd = nullptr,
    Animatable* animatable                        = nullptr);

  /**
   * @brief Will start the animation sequence of a given target.
   * @param target defines the target
   * @param from defines from which frame should animation start
   * @param to defines until which frame should animation run.
   * @param loop defines if the animation loops
   * @param speedRatio defines the speed in which to run the animation (1.0 by
   * default)
   * @param onAnimationEnd defines the function to be executed when the
   * animation ends
   * @param animatable defines an animatable object. If not provided a new one
   * will be created from the given params
   * @param stopCurrent defines if the current animations must be stopped first
   * (true by default)
   * @returns the animatable object created for this animation
   */
  Animatable*
  beginAnimation(IAnimatable* target, int from, int to, bool loop = false,
                 float speedRatio                              = 1.f,
                 const ::std::function<void()>& onAnimationEnd = nullptr,
                 Animatable* animatable = nullptr, bool stopCurrent = true);

  /**
   * @brief Begin a new animation on a given node.
   * @param target defines the target where the animation will take place
   * @param animations defines the list of animations to start
   * @param from defines the initial value
   * @param to defines the final value
   * @param loop defines if you want animation to loop (off by default)
   * @param speedRatio defines the speed ratio to apply to all animations
   * @param onAnimationEnd defines the callback to call when an animation ends
   * (will be called once per node)
   * @returns the list of created animatables
   */
  Animatable*
  beginDirectAnimation(IAnimatable* target,
                       const vector_t<Animation*>& animations, int from, int to,
                       bool loop = false, float speedRatio = 1.f,
                       const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Begin a new animation on a given node and its hierarchy.
   * @param target defines the root node where the animation will take place
   * @param directDescendantsOnly if true only direct descendants will be used,
   * if false direct and also indirect (children of children, an so on in a
   * recursive manner) descendants will be used.
   * @param animations defines the list of animations to start
   * @param from defines the initial value
   * @param to defines the final value
   * @param loop defines if you want animation to loop (off by default)
   * @param speedRatio defines the speed ratio to apply to all animations
   * @param onAnimationEnd defines the callback to call when an animation ends
   * (will be called once per node)
   * @returns the list of animatables created for all nodes
   */
  vector_t<Animatable*> beginDirectHierarchyAnimation(
    Node* target, bool directDescendantsOnly,
    const vector_t<Animation*>& animations, int from, int to, bool loop = false,
    float speedRatio                              = 1.f,
    const ::std::function<void()>& onAnimationEnd = nullptr);

  /**
   * @brief Gets the animatable associated with a specific target.
   * @param target defines the target of the animatable
   * @returns the required animatable if found
   */
  Animatable* getAnimatableByTarget(IAnimatable* target);

  /**
   * @brief Gets all animatables associated with a given target.
   * @param target defines the target to look animatables for
   * @returns an array of Animatables
   */
  vector_t<Animatable*> getAllAnimatablesByTarget(IAnimatable* target);

  /**
   * @brief Gets all animatable attached to the scene.
   */
  vector_t<Animatable*>& animatables();

  /**
   * @brief Will stop the animation of the given target.
   * @param target - the target
   * @param animationName - the name of the animation to stop (all animations
   * will be stopped is empty)
   */
  void stopAnimation(IAnimatable* target, const string_t& animationName = "");

  /**
   * @brief Stops and removes all animations that have been applied to the scene
   */
  void stopAllAnimations();

  /**
   * @brief Hidden
   */
  void
  _registerTargetForLateAnimationBinding(RuntimeAnimation* runtimeAnimation);

  /** Matrix **/

  /**
   * @brief Hidden
   */
  void _switchToAlternateCameraConfiguration(bool active);

  /**
   * @brief Gets the current view matrix.
   * @returns a Matrix
   */
  Matrix getViewMatrix();

  /**
   * @brief Gets the current projection matrix.
   * @returns a Matrix
   */
  Matrix& getProjectionMatrix();

  /**
   * @brief Gets the current transform matrix.
   * @returns a Matrix made of View * Projection
   */
  const Matrix& getProjectionMatrix() const;

  /**
   * @brief Gets the current transform matrix.
   * @returns a Matrix made of View * Projection
   */
  Matrix getTransformMatrix();

  /**
   * @brief Sets the current transform matrix.
   * @param view defines the View matrix to use
   * @param projection defines the Projection matrix to use
   */
  void setTransformMatrix(Matrix& view, Matrix& projection);

  /**
   * @brief Hidden
   */
  void _setAlternateTransformMatrix(Matrix& view, Matrix& projection);

  /**
   * @brief Gets the uniform buffer used to store scene data.
   * @returns a UniformBuffer
   */
  UniformBuffer* getSceneUniformBuffer();

  /**
   * @brief Gets an unique (relatively to the current scene) Id.
   * @returns an unique number for the scene
   */
  size_t getUniqueId();

  /**
   * @brief Add a mesh to the list of scene's meshes.
   * @param newMesh defines the mesh to add
   */
  void addMesh(unique_ptr_t<AbstractMesh>&& newMesh);

  /**
   * @brief Remove a mesh for the list of scene's meshes.
   * @param toRemove defines the mesh to remove
   * @returns the index where the mesh was in the mesh list
   */
  int removeMesh(AbstractMesh* toRemove);

  /**
   * @brief Add a transform node to the list of scene's transform nodes.
   * @param newTransformNode defines the transform node to add
   */
  void addTransformNode(TransformNode* newTransformNode);

  /**
   * @brief Remove a transform node for the list of scene's transform nodes.
   * @param toRemove defines the transform node to remove
   * @returns the index where the transform node was in the transform node list
   */
  int removeTransformNode(TransformNode* toRemove);

  /**
   * @brief Remove a skeleton for the list of scene's skeletons.
   * @param toRemove defines the skeleton to remove
   * @returns the index where the skeleton was in the skeleton list
   */
  int removeSkeleton(Skeleton* toRemove);

  /**
   * @brief Remove a morph target for the list of scene's morph targets.
   * @param toRemove defines the morph target to remove
   * @returns the index where the morph target was in the morph target list
   */
  int removeMorphTargetManager(MorphTargetManager* toRemove);

  /**
   * @brief Remove a light for the list of scene's lights.
   * @param toRemove defines the light to remove
   * @returns the index where the light was in the light list
   */
  int removeLight(Light* toRemove);

  /**
   * @brief Remove a camera for the list of scene's cameras.
   * @param toRemove defines the camera to remove
   * @returns the index where the camera was in the camera list
   */
  int removeCamera(Camera* toRemove);

  /**
   * @brief Remove a particle system for the list of scene's particle systems.
   * @param toRemove defines the particle system to remove
   * @returns the index where the particle system was in the particle system
   * list
   */
  int removeParticleSystem(IParticleSystem* toRemove);

  /**
   * @brief Remove a animation for the list of scene's animations.
   * @param toRemove defines the animation to remove
   * @returns the index where the animation was in the animation list
   */
  int removeAnimation(Animation* toRemove);

  /**
   * @brief Removes the given animation group from this scene.
   * @param toRemove The animation group to remove
   * @returns The index of the removed animation group
   */
  int removeAnimationGroup(AnimationGroup* toRemove);

  /**
   * @brief Removes the given multi-material from this scene.
   * @param toRemove The multi-material to remove
   * @returns The index of the removed multi-material
   */
  int removeMultiMaterial(MultiMaterial* toRemove);

  /**
   * @brief Removes the given material from this scene.
   * @param toRemove The material to remove
   * @returns The index of the removed material
   */
  int removeMaterial(Material* toRemove);

  /**
   * @brief Removes the given lens flare system from this scene.
   * @param toRemove The lens flare system to remove
   * @returns The index of the removed lens flare system
   */
  int removeLensFlareSystem(LensFlareSystem* toRemove);

  /**
   * @brief Removes the given action manager from this scene.
   * @param toRemove The action manager to remove
   * @returns The index of the removed action manager
   */
  int removeActionManager(ActionManager* toRemove);

  /**
   * @brief Removes the given effect layer from this scene.
   * @param toRemove defines the effect layer to remove
   * @returns the index of the removed effect layer
   */
  int removeEffectLayer(EffectLayer* toRemove);

  /**
   * @brief Removes the given texture from this scene.
   * @param toRemove The texture to remove
   * @returns The index of the removed texture
   */
  int removeTexture(BaseTexture* toRemove);

  /**
   * @brief Adds the given light to this scene.
   * @param newLight The light to add
   */
  void addLight(unique_ptr_t<Light>&& newLight);

  /**
   * @brief Sorts the list list based on light priorities.
   */
  void sortLightsByPriority();

  /**
   * @brief Adds the given camera to this scene.
   * @param newCamera The camera to add
   */
  void addCamera(unique_ptr_t<Camera>&& newCamera);

  /**
   * @brief Adds the given skeleton to this scene.
   * @param newSkeleton The skeleton to add
   */
  void addSkeleton(unique_ptr_t<Skeleton>&& newSkeleton);

  /**
   * @brief Adds the given particle system to this scene.
   * @param newParticleSystem The particle system to add
   */
  void addParticleSystem(unique_ptr_t<ParticleSystem>&& newParticleSystem);

  /**
   * @brief Adds the given animation to this scene.
   * @param newAnimation The animation to add
   */
  void addAnimation(Animation* newAnimation);

  /**
   * @brief Adds the given animation group to this scene.
   * @param newAnimationGroup The animation group to add
   */
  void addAnimationGroup(unique_ptr_t<AnimationGroup>&& newAnimationGroup);

  /**
   * @brief Adds the given multi-material to this scene.
   * @param newMultiMaterial The multi-material to add
   */
  void addMultiMaterial(unique_ptr_t<MultiMaterial>&& newMultiMaterial);

  /**
   * @brief Adds the given material to this scene.
   * @param newMaterial The material to add
   */
  void addMaterial(unique_ptr_t<Material>&& newMaterial);

  /**
   * @brief Adds the given morph target to this scene.
   * @param newMorphTargetManager The morph target to add
   */
  void addMorphTargetManager(
    unique_ptr_t<MorphTargetManager>&& newMorphTargetManager);

  /**
   * @brief Adds the given geometry to this scene.
   * @param newGeometry The geometry to add
   */
  void addGeometry(unique_ptr_t<Geometry>&& newGeometry);

  /**
   * @brief Adds the given lens flare system to this scene.
   * @param newLensFlareSystem The lens flare system to add
   */
  void addLensFlareSystem(unique_ptr_t<LensFlareSystem>&& newLensFlareSystem);

  /**
   * @brief Adds the given effect layer to this scene.
   * @param newEffectLayer defines the effect layer to add
   */
  void addEffectLayer(unique_ptr_t<EffectLayer>&& newEffectLayer);

  /**
   * @brief Adds the given action manager to this scene.
   * @param newActionManager The action manager to add
   */
  void addActionManager(unique_ptr_t<ActionManager>&& newActionManager);

  /**
   * @brief Adds the given texture to this scene.
   * @param newTexture The texture to add
   */
  void addTexture(unique_ptr_t<BaseTexture>&& newTexture);

  /**
   * @brief Switch active camera.
   * @param newCamera defines the new active camera
   * @param attachControl defines if attachControl must be called for the new
   * active camera (default: true)
   */
  void switchActiveCamera(Camera* newCamera, bool attachControl = true);

  /**
   * @brief Sets the active camera of the scene using its ID.
   * @param id defines the camera's ID
   * @return the new active camera or null if none found.
   */
  Camera* setActiveCameraByID(const string_t& id);

  /**
   * @brief Sets the active camera of the scene using its name.
   * @param name defines the camera's name
   * @returns the new active camera or null if none found.
   */
  Camera* setActiveCameraByName(const string_t& name);

  /**
   * Gets an animation group using its name.
   * @param name defines the material's name
   * @return the animation group or null if none found.
   */
  AnimationGroup* getAnimationGroupByName(const string_t& name);

  /**
   * @brief Gets a material using its id.
   * @param id defines the material's ID
   * @return the material or null if none found.
   */
  Material* getMaterialByID(const string_t& id);

  /**
   * @brief Gets a material using its name.
   * @param name defines the material's name
   * @return the material or null if none found.
   */
  Material* getMaterialByName(const string_t& name);

  /**
   * @brief Gets a lens flare system using its name.
   * @param name defines the name to look for
   * @returns the lens flare system or null if not found
   */
  LensFlareSystem* getLensFlareSystemByName(const string_t& name);

  /**
   * @brief Gets a lens flare system using its id.
   * @param id defines the id to look for
   * @returns the lens flare system or null if not found
   */
  LensFlareSystem* getLensFlareSystemByID(const string_t& id);

  /**
   * @brief Hidden
   */
  vector_t<Camera*> getCameras() const;

  /**
   * @brief Gets a camera using its id.
   * @param id defines the id to look for
   * @returns the camera or null if not found
   */
  Camera* getCameraByID(const string_t& id);

  /**
   * Gets a camera using its unique id.
   * @param uniqueId defines the unique id to look for
   * @returns the camera or null if not found
   */
  Camera* getCameraByUniqueID(unsigned int uniqueId);

  /**
   * @brief Gets a camera using its name.
   * @param name defines the camera's name
   * @return the camera or null if none found.
   */
  Camera* getCameraByName(const string_t& name);

  /**
   * @brief Gets a bone using its id.
   * @param id defines the bone's id
   * @return the bone or null if not found
   */
  Bone* getBoneByID(const string_t& id);

  /**
   * @brief Gets a bone using its id.
   * @param name defines the bone's name
   * @return the bone or null if not found
   */
  Bone* getBoneByName(const string_t& name);

  /**
   * @brief Gets a light node using its name.
   * @param name defines the the light's name
   * @return the light or null if none found.
   */
  Light* getLightByName(const string_t& name);

  /**
   * @brief Gets a light node using its id.
   * @param id defines the light's id
   * @return the light or null if none found.
   */
  Light* getLightByID(const string_t& id);

  /**
   * @brief Gets a light node using its scene-generated unique ID.
   * @param uniqueId defines the light's unique id
   * @return the light or null if none found.
   */
  Light* getLightByUniqueID(unsigned int uniqueId);

  /**
   * @brief Gets a particle system by id.
   * @param id defines the particle system id
   * @return the corresponding system or null if none found
   */
  IParticleSystem* getParticleSystemByID(const string_t& id);

  /**
   * @brief Gets a geometry using its ID
   * @param id defines the geometry's id.
   * @return the geometry or null if none found.
   */
  Geometry* getGeometryByID(const string_t& id);

  /**
   * @brief Add a new geometry to this scene.
   * @param geometry defines the geometry to be added to the scene.
   * @param force defines if the geometry must be pushed even if a geometry with
   * this id already exists
   * @return a boolean defining if the geometry was added or not
   */
  bool pushGeometry(unique_ptr_t<Geometry>&& geometry, bool force = false);

  /**
   * @brief Removes an existing geometry.
   * @param geometry defines the geometry to be removed from the scene
   * @return a boolean defining if the geometry was removed or not
   */
  bool removeGeometry(Geometry* geometry);

  /**
   * @brief Gets the list of geometries attached to the scene.
   * @returns an array of Geometry
   */
  vector_t<unique_ptr_t<Geometry>>& getGeometries();

  /**
   * @brief Gets the first added mesh found of a given ID.
   * @param id defines the id to search for
   * @return the mesh found or null if not found at all
   */
  AbstractMesh* getMeshByID(const string_t& id);

  /**
   * @brief Gets a list of meshes using their id.
   * @param id defines the id to search for
   * @returns a list of meshes
   */
  vector_t<AbstractMesh*> getMeshesByID(const string_t& id);

  /**
   * @brief Gets the first added transform node found of a given ID.
   * @param id defines the id to search for
   * @return the found transform node or null if not found at all.
   */
  TransformNode* getTransformNodeByID(const string_t& id);

  /**
   * @brief Gets a list of transform nodes using their id.
   * @param id defines the id to search for
   * @returns a list of transform nodes
   */
  vector_t<TransformNode*> getTransformNodesByID(const string_t& id);

  /**
   * @brief Gets a mesh with its auto-generated unique id.
   * @param uniqueId defines the unique id to search for
   * @return the found mesh or null if not found at all.
   */
  AbstractMesh* getMeshByUniqueID(unsigned int uniqueId);

  /**
   * @brief Gets a the last added mesh using a given id.
   * @param id defines the id to search for
   * @return the found mesh or null if not found at all.
   */
  AbstractMesh* getLastMeshByID(const string_t& id);

  /**
   * @brief Hidden
   */
  vector_t<AbstractMesh*> getMeshes() const;

  /**
   * @brief Gets a the last added node (Mesh, Camera, Light) using a given id.
   * @param id defines the id to search for
   * @return the found node or null if not found at all
   */
  Node* getLastEntryByID(const string_t& id);

  /**
   * @brief Gets a node (Mesh, Camera, Light) using a given id.
   * @param id defines the id to search for
   * @return the found node or null if not found at all
   */
  Node* getNodeByID(const string_t& id);

  /**
   * @brief Gets a node (Mesh, Camera, Light) using a given name.
   * @param name defines the name to search for
   * @return the found node or null if not found at all.
   */
  Node* getNodeByName(const string_t& name);

  /**
   * @brief Gets a mesh using a given name.
   * @param name defines the name to search for
   * @return the found mesh or null if not found at all.
   */
  AbstractMesh* getMeshByName(const string_t& name);

  /**
   * @brief Gets a transform node using a given name.
   * @param name defines the name to search for
   * @return the found transform node or null if not found at all.
   */
  TransformNode* getTransformNodeByName(const string_t& name);

  /**
   * @brief Gets a sound using a given name.
   * @param name defines the name to search for
   * @return the found sound or null if not found at all.
   */
  Sound* getSoundByName(const string_t& name);

  /**
   * @brief Gets a skeleton using a given id (if many are found, this function
   * will pick the last one).
   * @param id defines the id to search for
   * @return the found skeleton or null if not found at all.
   */
  Skeleton* getLastSkeletonByID(const string_t& id);

  /**
   * @brief Gets a skeleton using a given id (if many are found, this function
   * will pick the first one).
   * @param id defines the id to search for
   * @return the found skeleton or null if not found at all.
   */
  Skeleton* getSkeletonById(const string_t& id);

  /**
   * @brief Gets a skeleton using a given name.
   * @param name defines the name to search for
   * @return the found skeleton or null if not found at all.
   */
  Skeleton* getSkeletonByName(const string_t& name);

  /**
   * @brief Gets a morph target manager  using a given id (if many are found,
   * this function will pick the last one).
   * @param id defines the id to search for
   * @return the found morph target manager or null if not found at all.
   */
  MorphTargetManager* getMorphTargetManagerById(unsigned int id);

  /**
   * @brief Gets a boolean indicating if the given mesh is active.
   * @param mesh defines the mesh to look for
   * @returns true if the mesh is in the active list
   */
  bool isActiveMesh(AbstractMesh* mesh);

  /**
   * @brief Return a the first highlight layer of the scene with a given name.
   * @param name The name of the highlight layer to look for.
   * @return The highlight layer if found otherwise null.
   */
  HighlightLayer* getHighlightLayerByName(const string_t& name);

  /**
   * @brief Return a the first highlight layer of the scene with a given name.
   * @param name The name of the highlight layer to look for.
   * @return The highlight layer if found otherwise null.
   */
  GlowLayer* getGlowLayerByName(const string_t& name);

  /**
   * @brief Return a unique id as a string which can serve as an identifier for
   * the scene.
   */
  string_t uid();

  /**
   * @brief Clear the processed materials smart array preventing retention point
   * in material dispose.
   */
  void freeProcessedMaterials();

  /**
   * @brief Clear the active meshes smart array preventing retention point in
   * mesh dispose.
   */
  void freeActiveMeshes();

  /**
   * @brief Clear the info related to rendering groups preventing retention
   * points during dispose.
   */
  void freeRenderingGroups();

  /**
   * @brief Hidden
   */
  bool _isInIntermediateRendering() const;

  /**
   * @brief Defines the current active mesh candidate provider.
   * @param provider defines the provider to use
   */
  void setActiveMeshCandidateProvider(IActiveMeshCandidateProvider* provider);

  /**
   * @brief Gets the current active mesh candidate provider.
   * @returns the current active mesh candidate provider
   */
  IActiveMeshCandidateProvider* getActiveMeshCandidateProvider() const;

  /**
   * @brief Use this function to stop evaluating active meshes. The current list
   * will be keep alive between frames.
   * @returns the current scene
   */
  Scene& freezeActiveMeshes();

  /**
   * @brief Use this function to restart evaluating active meshes on every
   * frame.
   * @returns the current scene
   */
  Scene& unfreezeActiveMeshes();

  /**
   * @brief Update the transform matrix to update from the current active
   * camera.
   * @param force defines a boolean used to force the update even if cache is up
   * to date
   */
  void updateTransformMatrix(bool force = false);

  /**
   * @brief Defines an alternate camera (used mostly in VR-like scenario where
   * two cameras can render the same scene from a slightly different point of
   * view).
   * @param alternateCamera defines the camera to use
   */
  void updateAlternateTransformMatrix(Camera* alternateCamera);

  /**
   * @brief Render the scene.
   */
  void render();

  /** Audio **/

  /**
   * @brief Gets if audio support is enabled.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  bool audioEnabled() const;

  /**
   * @brief Sets if audio support is enabled.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  void setAudioEnabled(bool value);

  /**
   * @brief Gets if audio will be output to headphones.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  bool headphone() const;

  /**
   * @brief Sets if audio will be output to headphones.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  void setHeadphone(bool value);

  /** Rendering **/

  /**
   * @brief Creates a depth renderer a given camera which contains a depth map
   * which can be used for post processing.
   * @param camera The camera to create the depth renderer on (default: scene's
   * active camera)
   * @returns the created depth renderer
   */
  DepthRenderer* enableDepthRenderer(Camera* camera = nullptr);

  /**
   * @brief Disables a depth renderer for a given camera.
   * @param camera The camera to disable the depth renderer on (default: scene's
   * active camera)
   */
  void disableDepthRenderer(Camera* camera = nullptr);

  /**
   * @brief Enables a GeometryBufferRender and associates it with the scene.
   * @param ratio defines the scaling ratio to apply to the renderer (1 by
   * default which means same resolution)
   * @returns the GeometryBufferRenderer
   */
  GeometryBufferRenderer* enableGeometryBufferRenderer(float ratio = 1.f);

  /**
   * @brief Disables the GeometryBufferRender associated with the scene.
   */
  void disableGeometryBufferRenderer();

  /**
   * @brief Freeze all materials.
   * A frozen material will not be updatable but should be faster to render
   */
  void freezeMaterials();

  /**
   * @brief Unfreeze all materials.
   * A frozen material will not be updatable but should be faster to render
   */
  void unfreezeMaterials();

  /**
   * @brief Releases all held ressources.
   */
  void dispose();

  /**
   * @brief Gets if the scene is already disposed.
   */
  bool isDisposed() const;

  /**
   *  @brief Releases sounds & soundtracks.
   */
  void disposeSounds();

  /** Octrees **/

  /**
   * @brief Get the world extend vectors with an optional filter.
   * @param filterPredicate the predicate - which meshes should be included when
   * calculating the world size
   * @returns {{ min: Vector3; max: Vector3 }} min and max vectors
   */
  MinMax getWorldExtends(
    const ::std::function<bool(AbstractMesh* Mesh)>& filterPredicate = nullptr);

  /**
   * @brief Creates or updates the octree used to boost selection (picking).
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
   * @param maxCapacity defines the maximum capacity per leaf
   * @param maxDepth defines the maximum depth of the octree
   * @returns an octree of AbstractMesh
   */
  Octree<AbstractMesh*>* createOrUpdateSelectionOctree(size_t maxCapacity = 64,
                                                       size_t maxDepth    = 2);

  /** Picking **/

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param world defines the world matrix to use if you want to pick in object
   * space (instead of world space)
   * @param camera defines the camera to use for the picking
   * @param cameraViewSpace defines if picking will be done in view space (false
   * by default)
   * @returns a Ray
   */
  Ray createPickingRay(int x, int y, Matrix* world, Camera* camera = nullptr,
                       bool cameraViewSpace = false);

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param world defines the world matrix to use if you want to pick in object
   * space (instead of world space)
   * @param result defines the ray where to store the picking ray
   * @param camera defines the camera to use for the picking
   * @param cameraViewSpace defines if picking will be done in view space (false
   * by default)
   * @returns the current scene
   */
  Scene& createPickingRayToRef(int x, int y, Matrix* world, Ray& result,
                               Camera* camera, bool cameraViewSpace = false);

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param camera defines the camera to use for the picking
   * @returns a Ray
   */
  Ray createPickingRayInCameraSpace(int x, int y, Camera* camera = nullptr);

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param result defines the ray where to store the picking ray
   * @param camera defines the camera to use for the picking
   * @returns the current scene
   */
  Scene& createPickingRayInCameraSpaceToRef(int x, int y, Ray& result,
                                            Camera* camera = nullptr);

  /** @brief Launch a ray to try to pick a mesh in the scene.
   * @param x position on screen
   * @param y position on screen
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be
   * set to null.
   * @param camera to use for computing the picking ray. Can be set to null. In
   * this case, the scene.activeCamera will be used
   * @returns a PickingInfo
   */
  PickingInfo* pick(int x, int y,
                    const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                    bool fastCheck = false, Camera* camera = nullptr);

  /** @brief Launch a ray to try to pick a sprite in the scene.
   * @param x position on screen
   * @param y position on screen
   * @param predicate Predicate function used to determine eligible sprites. Can
   * be set to null. In this case, a sprite must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be
   * set to null.
   * @param camera camera to use for computing the picking ray. Can be set to
   * null. In this case, the scene.activeCamera will be used
   * @returns a PickingInfo
   */
  PickingInfo*
  pickSprite(int x, int y,
             const ::std::function<bool(Sprite* sprite)>& predicate,
             bool fastCheck = false, Camera* camera = nullptr);

  /** @brief Use the given ray to pick a mesh in the scene.
   * @param ray The ray to use to pick meshes
   * @param predicate Predicate function used to determine eligible sprites. Can
   * be set to null. In this case, a sprite must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be
   * set to null
   * @returns a PickingInfo
   */
  PickingInfo*
  pickWithRay(const Ray& ray,
              const ::std::function<bool(AbstractMesh* mesh)>& predicate,
              bool fastCheck = false);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene.
   * @param x X position on screen
   * @param y Y position on screen
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true
   * @param camera camera to use for computing the picking ray. Can be set to
   * null. In this case, the scene.activeCamera will be used
   * @returns an array of PickingInfo
   */
  vector_t<PickingInfo*>
  multiPick(int x, int y,
            const ::std::function<bool(AbstractMesh* mesh)>& predicate,
            Camera* camera = nullptr);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene.
   * @param ray Ray to use
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true
   * @returns an array of PickingInfo
   */
  vector_t<PickingInfo*>
  multiPickWithRay(const Ray& ray,
                   const ::std::function<bool(AbstractMesh* mesh)>& predicate);

  /**
   * @brief Force the value of meshUnderPointer.
   * @param mesh defines the mesh to use
   */
  void setPointerOverMesh(AbstractMesh* mesh);

  /**
   * @brief Gets the mesh under the pointer.
   * @returns a Mesh or null if no mesh is under the pointer
   */
  AbstractMesh* getPointerOverMesh();

  /**
   * @brief Force the sprite under the pointer.
   * @param sprite defines the sprite to use
   */
  void setPointerOverSprite(Sprite* sprite);

  /**
   * @brief Gets the sprite under the pointer.
   * @returns a Sprite or null if no sprite is under the pointer
   */
  Sprite* getPointerOverSprite() const;

  /** Physics **/

  /**
   * @brief Gets the current physics engine.
   * @returns a PhysicsEngine or null if none attached
   */
  PhysicsEngine* getPhysicsEngine();

  /**
   * @brief Enables physics to the current scene.
   * @param gravity defines the scene's gravity for the physics engine
   * @param plugin defines the physics engine to be used. defaults to OimoJS.
   * @return a boolean indicating if the physics engine was initialized
   */
  bool enablePhysics(const Vector3& gravity, IPhysicsEnginePlugin* plugin);

  /**
   * @brief Disables and disposes the physics engine associated with the scene.
   */
  void disablePhysicsEngine();

  /**
   * @brief Gets a boolean indicating if there is an active physics engine.
   * @returns a boolean indicating if there is an active physics engine
   */
  bool isPhysicsEnabled();

  /** Misc. **/

  /**
   * @brief Hidden
   */
  void _rebuildGeometries();

  /**
   * @brief Hidden
   */
  void _rebuildTextures();

  /**
   * @brief Creates a default light for the scene.
   * @param replace Whether to replace the existing lights in the scene.
   */
  void createDefaultLight(bool replace);

  /**
   * @brief Creates a default camera for the scene.
   * @param createArcRotateCamera Whether to create an arc rotate or a free
   * camera.
   * @param replace Whether to replace the existing active camera in the scene.
   * @param attachCameraControls Whether to attach camera controls to the
   * canvas.
   */
  void createDefaultCamera(bool createArcRotateCamera = false,
                           bool replace               = false,
                           bool attachCameraControls  = false);

  /**
   * @brief Creates a default camera and a default light.
   * @param createArcRotateCamera defines that the camera will be an
   * ArcRotateCamera
   * @param replace defines if the camera and/or light will replace the existing
   * ones
   * @param attachCameraControls defines if attachControl will be called on the
   * new camera
   */
  void createDefaultCameraOrLight(bool createArcRotateCamera = false,
                                  bool replace               = false,
                                  bool attachCameraControls  = false);

  /**
   * @brief Creates a new sky box.
   * @see http://doc.babylonjs.com/babylon101/environment#skybox
   * @param environmentTexture defines the texture to use as environment texture
   * @param pbr defines if PBRMaterial must be used instead of StandardMaterial
   * @param scale defines the overall scale of the skybox
   * @param blur defines if blurring must be applied to the environment texture
   * (works only with pbr === true)
   * @param setGlobalEnvTexture defines a boolean indicating that
   * scene.environmentTexture must match the current skybox texture (true by
   * default)
   * @returns a new mesh holding the sky box
   */
  Mesh* createDefaultSkybox(BaseTexture* environmentTexture = nullptr,
                            bool pbr = false, float scale = 1000.f,
                            float blur = 0.f, bool setGlobalEnvTexture = true);

  /**
   * @brief Creates a new environment.
   * @see http://doc.babylonjs.com/babylon101/environment#skybox
   * @param options defines the options you can use to configure the environment
   * @returns the new EnvironmentHelper
   */
  unique_ptr_t<EnvironmentHelper>
  createDefaultEnvironment(const IEnvironmentHelperOptions& options);

  /** Tags **/

  /**
   * @brief Get a list of meshes by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Mesh
   */
  vector_t<Mesh*> getMeshesByTags();

  /**
   * @brief Get a list of cameras by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Camera
   */
  vector_t<Camera*> getCamerasByTags();

  /**
   * @brief Get a list of lights by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Light
   */
  vector_t<Light*> getLightsByTags();

  /**
   * @brief Get a list of materials by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Material
   */
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
   * @brief Will flag all materials as dirty to trigger new shader compilation.
   * @param flag defines the flag used to specify which material part must be
   * marked as dirty
   * @param predicate If not null, it will be used to specifiy if a material has
   * to be marked as dirty
   */
  void
  markAllMaterialsAsDirty(unsigned int flag,
                          const ::std::function<bool(Material* mat)>& predicate
                          = nullptr);

  /**
   * @brief Hidden
   */
  IFileRequest _loadFile(
    const string_t& url,
    const ::std::function<void(Variant<string_t, ArrayBuffer>& data,
                               const string_t& responseURL)>& onSuccess);

protected:
  /**
   * @brief Creates a new Scene.
   * @param engine defines the engine to use to render this scene
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
  AnimationValue _processLateAnimationBindingsForMatrices(
    float holderTotalWeight, vector_t<RuntimeAnimation*>& holderAnimations,
    Matrix& originalValue);
  void _processLateAnimationBindings();
  void _evaluateSubMesh(SubMesh* subMesh, AbstractMesh* mesh);
  void _evaluateActiveMeshes();
  void _activeMesh(AbstractMesh* sourceMesh, AbstractMesh* mesh);
  void _renderForCamera(Camera* camera, Camera* rigParent = nullptr);
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
  void _switchAudioModeForHeadphones();
  void _switchAudioModeForNormalSpeakers();
  /** Picking **/
  PickingInfo*
  _internalPick(const ::std::function<Ray(Matrix& world)>& rayFunction,
                const ::std::function<bool(AbstractMesh* mesh)>& predicate,
                bool fastCheck);
  vector_t<PickingInfo*> _internalMultiPick(
    const ::std::function<Ray(Matrix& world)>& rayFunction,
    const ::std::function<bool(AbstractMesh* mesh)>& predicate);
  PickingInfo*
  _internalPickSprites(const Ray& ray,
                       const ::std::function<bool(Sprite* sprite)>& predicate,
                       bool fastCheck, Camera* camera);
  /** Tags **/
  vector_t<string_t> _getByTags();

public:
  // Members

  /**
   * Gets or sets a boolean that indicates if the scene must clear the render
   * buffer before rendering a frame
   */
  bool autoClear;

  /**
   * Gets or sets a boolean that indicates if the scene must clear the depth and
   * stencil buffers before rendering a frame
   */
  bool autoClearDepthAndStencil;

  /**
   * Defines the color used to clear the render buffer (Default is (0.2, 0.2,
   * 0.3, 1.0))
   */
  Color4 clearColor;

  /**
   * Defines the color used to simulate the ambient color (Default is (0, 0, 0))
   */
  Color3 ambientColor;

  /** Hidden */
  BaseTexture* _environmentBRDFTexture;

  // Events

  /**
   * An event triggered when the scene is disposed.
   */
  Observable<Scene> onDisposeObservable;
  /**
   * An event triggered before rendering the scene (right after animations and
   * physics)
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
   * An event triggered when SceneLoader.Append or SceneLoader.Load or
   * SceneLoader.ImportMesh were successfully executed
   */
  Observable<Scene> onDataLoadedObservable;

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
  Observable<Scene> onBeforeRenderTargetsRenderObservable;

  /**
   * An event triggered when render targets were rendered.
   * Can happen multiple times per frame.
   */
  Observable<Scene> onAfterRenderTargetsRenderObservable;

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

  /**
   * Gets a list of Animations associated with the scene
   */
  vector_t<Animation*> animations;

  // Pointers

  /**
   * Gets or sets a predicate used to select candidate meshes for a pointer down
   * event
   */
  ::std::function<bool(AbstractMesh* Mesh)> pointerDownPredicate;
  /**
   * Gets or sets a predicate used to select candidate meshes for a pointer up
   * event
   */
  ::std::function<bool(AbstractMesh* Mesh)> pointerUpPredicate;
  /**
   * Gets or sets a predicate used to select candidate meshes for a pointer move
   * event
   */
  ::std::function<bool(AbstractMesh* Mesh)> pointerMovePredicate;

  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo,
                       PointerEventTypes type)>
    onPointerMove;
  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo,
                       PointerEventTypes type)>
    onPointerDown;
  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo,
                       PointerEventTypes type)>
    onPointerUp;
  /** Deprecated. Use onPointerObservable instead */
  ::std::function<void(const PointerEvent& evt, const PickingInfo* pickInfo)>
    onPointerPick;

  /**
   * Gets or sets a boolean indicating if all bounding boxes must be rendered
   */
  bool forceShowBoundingBoxes;

  /**
   * Gets or sets the active clipplane
   */
  unique_ptr_t<Plane> _clipPlane;

  /**
   * Gets or sets a boolean indicating if animations are enabled
   */
  bool animationsEnabled;

  /**
   * Animation properties override.
   */
  Property<Scene, AnimationPropertiesOverride*> animationPropertiesOverride;

  /**
   * Gets or sets a boolean indicating if a constant deltatime has to be used
   * This is mostly useful for testing purposes when you do not want the
   * animations to scale with the framerate
   */
  bool useConstantAnimationDeltaTime;

  /**
   * Gets or sets a boolean indicating if the scene must keep the
   * meshUnderPointer property updated Please note that it requires to run a ray
   * cast through the scene on every frame
   */
  bool constantlyUpdateMeshUnderPointer;

  /**
   * Defines the HTML cursor to use when hovering over interactive elements
   */
  string_t hoverCursor;

  /**
   * Defines the HTML default cursor to use (empty by default)
   */
  string_t defaultCursor;

  /**
   * This is used to call preventDefault() on pointer down
   * in order to block unwanted artifacts like system double clicks
   */
  bool preventDefaultOnPointerDown;

  // Metadata

  /**
   * Gets or sets user defined metadata
   */
  Json::object metadata;

  /**
   * Gets the name of the plugin used to load this scene (null by default)
   */
  string_t loadingPluginName;

  /**
   * Use this array to add regular expressions used to disable offline support
   * for specific urls
   */
  vector_t<::std::regex> disableOfflineSupportExceptionRules;

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

  /** Hidden */
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

  /**
   * Gets or sets the fog color to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  Color3 fogColor;

  /**
   * Gets or sets the fog density to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  float fogDensity;

  /**
   * Gets or sets the fog start distance to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  float fogStart;

  /**
   * Gets or sets the fog end distance to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  float fogEnd;

  // Lights
  /**
   * All of the lights added to this scene
   * @see http://doc.babylonjs.com/babylon101/lights
   */
  vector_t<unique_ptr_t<Light>> lights;

  // Cameras
  /**
   * All of the cameras added to this scene
   * @see http://doc.babylonjs.com/babylon101/cameras
   */
  vector_t<unique_ptr_t<Camera>> cameras;
  /**
   * All of the active cameras added to this scene
   */
  vector_t<Camera*> activeCameras;

  /**
   * The current active camera
   */
  Camera* activeCamera;

  // Meshes

  /**
   * All of the tranform nodes added to this scene.
   * @see http://doc.babylonjs.com/how_to/transformnode
   */
  vector_t<TransformNode*> transformNodes;

  /**
   * All of the (abstract) meshes added to this scene.
   */
  vector_t<unique_ptr_t<AbstractMesh>> meshes;

  /**
   * All of the animation groups added to this scene
   * @see http://doc.babylonjs.com/how_to/group
   */
  vector_t<unique_ptr_t<AnimationGroup>> animationGroups;

  // Geometries

  /**
   * All of the materials added to this scene
   * @see http://doc.babylonjs.com/babylon101/materials
   */
  vector_t<unique_ptr_t<Material>> materials;

  /**
   * All of the multi-materials added to this scene
   * @see http://doc.babylonjs.com/how_to/multi_materials
   */
  vector_t<unique_ptr_t<MultiMaterial>> multiMaterials;

  // Textures

  /**
   * All of the textures added to this scene
   */
  vector_t<unique_ptr_t<BaseTexture>> textures;

  // Particles

  /**
   * Gets or sets a boolean indicating if particles are enabled on this scene
   */
  bool particlesEnabled;

  /**
   * All of the particle systems added to this scene
   * @see http://doc.babylonjs.com/babylon101/particles
   */
  vector_t<unique_ptr_t<IParticleSystem>> particleSystems;

  // Sprites

  /**
   * Gets or sets a boolean indicating if sprites are enabled on this scene
   */
  bool spritesEnabled;

  /**
   * All of the sprite managers added to this scene
   * @see http://doc.babylonjs.com/babylon101/sprites
   */
  vector_t<unique_ptr_t<SpriteManager>> spriteManagers;
  ::std::function<bool(Sprite* sprite)> spritePredicate;

  /**
   * The list of layers (background and foreground) of the scene
   */
  vector_t<Layer*> layers;

  /**
   * The list of effect layers (highlights/glow) added to the scene
   * @see http://doc.babylonjs.com/how_to/highlight_layer
   * @see http://doc.babylonjs.com/how_to/glow_layer
   */
  vector_t<unique_ptr_t<EffectLayer>> effectLayers;

  vector_t<unique_ptr_t<HighlightLayer>> highlightLayers;

  // Skeletons

  /**
   * The list of skeletons added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_bones_and_skeletons
   */
  vector_t<unique_ptr_t<Skeleton>> skeletons;

  // Morph targets

  /**
   * The list of morph target managers added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_dynamically_morph_a_mesh
   */
  vector_t<unique_ptr_t<MorphTargetManager>> morphTargetManagers;

  // Lens flares

  /**
   * Gets or sets a boolean indicating if lens flares are enabled on this scene
   */
  bool lensFlaresEnabled;

  /**
   * The list of lens flare system added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_lens_flares
   */
  vector_t<unique_ptr_t<LensFlareSystem>> lensFlareSystems;

  // Collisions

  /**
   * Gets or sets a boolean indicating if collisions are enabled on this scene
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  bool collisionsEnabled;

  /** Hidden */
  unique_ptr_t<ICollisionCoordinator> collisionCoordinator;

  /**
   * Defines the gravity applied to this scene (used only for collisions)
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  Vector3 gravity;
  // Postprocesses

  /**
   * The list of postprocesses added to the scene
   */
  vector_t<PostProcess*> postProcesses;

  /**
   * Gets or sets a boolean indicating if postprocesses are enabled on this
   * scene
   */
  bool postProcessesEnabled;

  /**
   * Gets the current postprocess manager
   */
  unique_ptr_t<PostProcessManager> postProcessManager;

  // Customs render targets

  /**
   * Gets or sets a boolean indicating if render targets are enabled on this
   * scene
   */
  bool renderTargetsEnabled;

  /**
   * Gets or sets a boolean indicating if next render targets must be dumped as
   * image for debugging purposes We recommend not using it and instead rely on
   * Spector.js: http://spector.babylonjs.com
   */
  bool dumpNextRenderTargets;

  /**
   * The list of user defined render targets added to the scene
   */
  vector_t<RenderTargetTexture*> customRenderTargets;

  // Delay loading

  /**
   * Defines if texture loading must be delayed
   * If true, textures will only be loaded when they need to be rendered
   */
  bool useDelayedTextureLoading;

  // Imported meshes

  /**
   * Gets the list of meshes imported to the scene through SceneLoader
   */
  vector_t<string_t> importedMeshesFiles;

  // Probes

  /**
   * Gets or sets a boolean indicating if probes are enabled on this scene
   */
  bool probesEnabled;

  /**
   * The list of reflection probes added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_reflection_probes
   */
  vector_t<unique_ptr_t<ReflectionProbe>> reflectionProbes;

  // Database

  /**
   * Hidden
   */
  // public database; //ANY

  // Actions

  /**
   * Gets or sets the action manager associated with the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_actions
   */
  ActionManager* actionManager;

  /** Hidden */
  vector_t<unique_ptr_t<ActionManager>> _actionManagers;

  // Procedural textures

  /**
   * Gets or sets a boolean indicating if procedural textures are enabled on
   * this scene
   */
  bool proceduralTexturesEnabled;

  /**
   * The list of procedural textures added to the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_procedural_textures
   */
  vector_t<unique_ptr_t<ProceduralTexture>> proceduralTextures;

  // Sound Tracks

  /**
   * The list of sound tracks added to the scene
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  vector_t<SoundTrack*> soundTracks;

  // Simplification Queue

  /**
   * Gets or sets the simplification queue attached to the scene
   * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
   */
  unique_ptr_t<SimplificationQueue> simplificationQueue;

  // Performance counters

  /** Hidden */
  PerfCounter _activeIndices;
  /** Hidden */
  PerfCounter _activeParticles;
  /** Hidden */
  PerfCounter _activeBones;

  /**
   * Gets or sets a general scale for animation speed
   * @see https://www.babylonjs-playground.com/#IBU2W7#3
   */
  size_t animationTimeScale;

  /** Hidden */
  Material* _cachedMaterial;
  /** Hidden */
  Effect* _cachedEffect;
  /** Hidden */
  Nullable<float> _cachedVisibility;
  /** Hidden */
  vector_t<IDisposable*> _toBeDisposed;

  /**
   * Gets or sets a boolean indicating that all submeshes of active meshes must
   * be rendered Use this boolean to avoid computing frustum clipping on
   * submeshes (This could help when you are CPU bound)
   */
  bool dispatchAllSubMeshesOfActiveMeshes;

  /** Hidden */
  vector_t<IParticleSystem*> _activeParticleSystems;

  /** Hidden */
  vector_t<Animatable*> _activeAnimatables;

  /** Hidden */
  unique_ptr_t<Vector3> _forcedViewPosition;

  /**
   * Gets or sets a boolean indicating if lights must be sorted by priority (off
   * by default) This is useful if there are more lights that the maximum
   * simulteanous authorized
   */
  bool requireLightSorting;

protected:
  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverride*& get_animationPropertiesOverride();

  /**
   * @brief Sets the animation properties override.
   */
  void
  set_animationPropertiesOverride(AnimationPropertiesOverride* const& value);

protected:
  /** Hidden */
  BaseTexture* _environmentTexture;
  /** Hidden */
  unique_ptr_t<ImageProcessingConfiguration> _imageProcessingConfiguration;

private:
  // Animations
  AnimationPropertiesOverride* _animationPropertiesOverride;
  // Events
  ::std::function<bool(Sprite* sprite)> _spritePredicate;
  Observer<Scene>::Ptr _onDisposeObserver;
  Observer<Scene>::Ptr _onBeforeRenderObserver;
  Observer<Scene>::Ptr _onAfterRenderObserver;
  Observer<Camera>::Ptr _onBeforeCameraRenderObserver;
  Observer<Camera>::Ptr _onAfterCameraRenderObserver;
  // Animations
  vector_t<string_t> _registeredForLateAnimationBindings;
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
  bool _totalPointersPressed;
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
  float _timeAccumulator;
  unsigned int _currentStepId;
  unsigned int _currentInternalStep;
  // Keyboard
  ::std::function<void(KeyboardEvent&& evt)> _onKeyDown;
  ::std::function<void(KeyboardEvent&& evt)> _onKeyUp;
  Observer<Engine>::Ptr _onCanvasFocusObserver;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  // Coordinates system
  bool _useRightHandedSystem;
  // Members
  bool _forceWireframe;
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
  PerfCounter _totalVertices;
  float _animationRatio;
  bool _animationTimeLastSet;
  high_res_time_point_t _animationTimeLast;
  int _animationTime;
  int _renderId;
  int _executeWhenReadyTimeoutId;
  bool _intermediateRendering;
  int _viewUpdateFlag;
  int _projectionUpdateFlag;
  int _alternateViewUpdateFlag;
  int _alternateProjectionUpdateFlag;
  vector_t<IFileRequest> _activeRequests;
  vector_t<string_t> _pendingData;
  bool _isDisposed;
  vector_t<AbstractMesh*> _activeMeshes;
  IActiveMeshCandidateProvider* _activeMeshCandidateProvider;
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
  unique_ptr_t<Matrix> _pickWithRayInverseMatrix;
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
  unordered_map_t<string_t, unique_ptr_t<DepthRenderer>> _depthRenderer;
  shared_ptr_t<GeometryBufferRenderer> _geometryBufferRenderer;
  AbstractMesh* _pickedDownMesh;
  AbstractMesh* _pickedUpMesh;
  Sprite* _pickedDownSprite;
  string_t _uid;
  unique_ptr_t<Ray> _tempPickingRay;
  unique_ptr_t<Ray> _cachedRayForTransform;

}; // end of class Scene

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_SCENE_H
