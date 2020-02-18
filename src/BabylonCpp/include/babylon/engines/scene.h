#ifndef BABYLON_ENGINES_SCENE_H
#define BABYLON_ENGINES_SCENE_H

#include <nlohmann/json.hpp>
#include <regex>
#include <variant>

#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/culling/octrees/octree.h>
#include <babylon/engines/abstract_scene.h>
#include <babylon/engines/scene_options.h>
#include <babylon/engines/stage.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/events/pointer_info.h>
#include <babylon/events/pointer_info_pre.h>
#include <babylon/helpers/ienvironment_helper_options.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/plane.h>
#include <babylon/misc/ifile_request.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>
#include <babylon/misc/perf_counter.h>

using json = nlohmann::json;

namespace BABYLON {

class Animatable;
struct AnimationPropertiesOverride;
class Bone;
class BoundingBoxRenderer;
class ClickInfo;
class Collider;
class DebugLayer;
class DepthRenderer;
class Effect;
class Engine;
class EnvironmentHelper;
class GamepadManager;
class GeometryBufferRenderer;
struct IActiveMeshCandidateProvider;
class IAnimatable;
struct ICollisionCoordinator;
class ImageProcessingConfiguration;
class InternalTexture;
struct IPhysicsEngine;
struct IPhysicsEnginePlugin;
struct IRenderingManagerAutoClearSetup;
struct ISceneComponent;
struct ISceneSerializableComponent;
struct ISpriteManager;
class KeyboardInfo;
class KeyboardInfoPre;
class Mesh;
class Node;
class OutlineRenderer;
class PostProcess;
class PostProcessManager;
class PostProcessRenderPipelineManager;
class ProceduralTexture;
struct RenderingGroupInfo;
class RenderingManager;
class RuntimeAnimation;
class SimplificationQueue;
class SoundTrack;
class UniformBuffer;
using AnimatablePtr                   = std::shared_ptr<Animatable>;
using BoundingBoxRendererPtr          = std::shared_ptr<BoundingBoxRenderer>;
using BonePtr                         = std::shared_ptr<Bone>;
using EffectPtr                       = std::shared_ptr<Effect>;
using DepthRendererPtr                = std::shared_ptr<DepthRenderer>;
using GeometryBufferRendererPtr       = std::shared_ptr<GeometryBufferRenderer>;
using IAnimatablePtr                  = std::shared_ptr<IAnimatable>;
using IPhysicsEnginePtr               = std::shared_ptr<IPhysicsEngine>;
using ImageProcessingConfigurationPtr = std::shared_ptr<ImageProcessingConfiguration>;
using InternalTexturePtr              = std::shared_ptr<InternalTexture>;
using ISceneComponentPtr              = std::shared_ptr<ISceneComponent>;
using ISceneSerializableComponentPtr  = std::shared_ptr<ISceneSerializableComponent>;
using ISpriteManagerPtr               = std::shared_ptr<ISpriteManager>;
using NodePtr                         = std::shared_ptr<Node>;
using MeshPtr                         = std::shared_ptr<Mesh>;
using OutlineRendererPtr              = std::shared_ptr<OutlineRenderer>;
using PostProcessPtr                  = std::shared_ptr<PostProcess>;
using ProceduralTexturePtr            = std::shared_ptr<ProceduralTexture>;
using SimplificationQueuePtr          = std::shared_ptr<SimplificationQueue>;
using SoundTrackPtr                   = std::shared_ptr<SoundTrack>;
using SubMeshPtr                      = std::shared_ptr<SubMesh>;

/**
 * @brief Represents a scene to be rendered by the engine.
 * @see http://doc.babylonjs.com/features/scene
 */
class BABYLON_SHARED_EXPORT Scene : public AbstractScene, public IAnimatable {

public:
  static size_t _uniqueIdCounter;

  /** The fog is deactivated */
  static constexpr unsigned int FOGMODE_NONE = 0;
  /** The fog density is following an exponential function */
  static constexpr unsigned int FOGMODE_EXP = 1;
  /** The fog density is following an exponential function faster than
   * FOGMODE_EXP */
  static constexpr unsigned int FOGMODE_EXP2 = 2;
  /** The fog density is following a linear function. */
  static constexpr unsigned int FOGMODE_LINEAR = 3;

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

  /**
   * The distance in pixel that you have to move to prevent some events.
   */
  static unsigned int DragMovementThreshold;
  /**
   * Time in milliseconds to wait to raise long press events if button is still pressed.
   */
  static milliseconds_t LongPressDelay;
  /**
   * Time in milliseconds with two consecutive clicks will be considered as a double click.
   */
  static milliseconds_t DoubleClickDelay;
  /**
   * If you need to check double click without raising a single click at first click, enable this
   * flag.
   */
  static bool ExclusiveDoubleClickMode;

  template <typename... Ts>
  static std::unique_ptr<Scene> New(Ts&&... args)
  {
    std::unique_ptr<Scene> scene(new Scene(std::forward<Ts>(args)...));
    scene->postInitialize();
    return scene;
  }
  ~Scene() override; // = default

  void postInitialize();
  Type type() const override;

  /**
   * @brief Add a component to the scene.
   * Note that the component could be registered on th next frame if this is called after the
   * register component stage.
   * @param component Defines the component to add to the scene
   * Hidden
   */
  void _addComponent(const ISceneComponentPtr& component);

  /**
   * @brief Gets a component from the scene.
   * @param name defines the name of the component to retrieve
   * @returns the component or null if not present
   * Hidden
   */
  ISceneComponentPtr _getComponent(const std::string& name) const;

  /** Properties **/

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
   * @brief Hidden
   */
  void setMirroredCameraPosition(const Vector3& newPosition);

  /**
   * @brief Hidden
   */
  std::vector<AbstractMesh*> _getDefaultMeshCandidates();

  /**
   * @brief Hidden
   */
  std::vector<SubMesh*> _getDefaultSubMeshCandidates(AbstractMesh* mesh);

  /**
   * @brief Sets the default candidate providers for the scene.
   * This sets the getActiveMeshCandidates, getActiveSubMeshCandidates,
   * getIntersectingSubMeshCandidates and getCollidingSubMeshCandidates to their default function
   */
  void setDefaultCandidateProviders();

  /**
   * @brief Gets the cached material (ie. the latest rendered one).
   * @returns the cached material
   */
  Material* getCachedMaterial();

  /**
   * @brief Gets the cached effect (ie. the latest rendered one).
   * @returns the cached effect
   */
  EffectPtr& getCachedEffect();

  /**
   * @brief Gets the cached visibility state (ie. the latest rendered one).
   * @returns the cached visibility state
   */
  std::optional<float> getCachedVisibility();

  /**
   * @brief Gets a boolean indicating if the current material / effect / visibility must be bind
   * again.
   * @param material defines the current material
   * @param effect defines the current effect
   * @param visibility defines the current visibility state
   * @returns true if one parameter is not cached
   */
  bool isCachedMaterialInvalid(Material* material, const EffectPtr& effect, float visibility = 1.f);

  /**
   * @brief Gets the bounding box renderer associated with the scene.
   * @returns a BoundingBoxRenderer
   */
  BoundingBoxRendererPtr& getBoundingBoxRenderer();

  /**
   * @brief Gets the outline renderer associated with the scene.
   * @returns a OutlineRenderer
   */
  OutlineRendererPtr& getOutlineRenderer();

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
   * @brief Gets the total number of active indices rendered per frame (You can deduce the number of
   * rendered triangles by dividing this number by 3).
   * @returns the total number of active indices rendered per frame
   */
  size_t getActiveIndices() const;

  /**
   * @brief Gets the total number of active particles rendered per frame.
   * @returns the total number of active particles rendered per frame
   */
  size_t getActiveParticles() const;

  /**
   * @brief Gets the total number of active bones rendered per frame.
   * @returns the total number of active bones rendered per frame
   */
  size_t getActiveBones() const;

  /** Stats **/

  /**
   * @brief Gets the array of active meshes.
   * @returns an array of AbstractMesh
   */
  std::vector<AbstractMesh*>& getActiveMeshes();

  /**
   * @brief Gets the array of active meshes.
   * @returns an array of AbstractMesh
   */
  const std::vector<AbstractMesh*>& getActiveMeshes() const;

  /**
   * @brief Gets the animation ratio (which is 1.0 is the scene renders at 60fps and 2 if the scene
   * renders at 30fps, etc.).
   * @returns a number
   */
  float getAnimationRatio() const;

  /**
   * @brief Gets an unique Id for the current render phase.
   * @returns a number
   */
  int getRenderId() const;

  /**
   * @brief Gets an unique Id for the current frame.
   * @returns a number
   */
  int getFrameId() const;

  /**
   * @brief Call this function if you want to manually increment the render Id.
   */
  void incrementRenderId();

  /** Pointers handling **/

  /**
   * @brief Use this method to simulate a pointer move on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer event on
   * @param pointerEventInit pointer event state to be used when simulating the pointer event (eg.
   * pointer id for multitouch)
   * @returns the current scene
   */
  Scene& simulatePointerMove(std::optional<PickingInfo>& pickResult);

  /**
   * @brief Use this method to simulate a pointer down on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer event on
   * @param pointerEventInit pointer event state to be used when simulating the pointer event (eg.
   * pointer id for multitouch)
   * @returns the current scene
   */
  Scene& simulatePointerDown(std::optional<PickingInfo>& pickResult);

  /**
   * @brief Use this method to simulate a pointer up on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer event on
   * @param pointerEventInit pointer event state to be used when simulating the pointer event (eg.
   * pointer id for multitouch)
   * @returns the current scene
   */
  Scene& simulatePointerUp(std::optional<PickingInfo>& pickResult);

  /**
   * @brief Gets a boolean indicating if the current pointer event is captured (meaning that the
   * scene has already handled the pointer down)
   * @param pointerId defines the pointer id to use in a multi-touch scenario (0 by default)
   * @returns true if the pointer was captured
   */
  bool isPointerCaptured(int pointerId = 0);

  /**
   * @brief Hidden
   */
  bool _isPointerSwiping() const;

  /**
   * @brief Attach events to the canvas (To handle actionManagers triggers and raise onPointerMove,
   * onPointerDown and onPointerUp.
   * @param attachUp defines if you want to attach events to pointerup
   * @param attachDown defines if you want to attach events to pointerdown
   * @param attachMove defines if you want to attach events to pointermove
   */
  void attachControl(bool attachUp = true, bool attachDown = true, bool attachMove = true);

  /**
   * @brief Detaches all event handlers.
   */
  void detachControl();

  /**
   * @brief This function will check if the scene can be rendered (textures are loaded, shaders are
   * compiled) Delay loaded resources are not taking in account.
   * @return true if all required resources are ready
   */
  bool isReady();

  /**
   * @brief Resets all cached information relative to material (including effect and visibility).
   */
  void resetCachedMaterial();

  /**
   * @brief Registers a function to be called before every frame render.
   * @param func defines the function to register
   */
  void registerBeforeRender(const std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * Unregisters a function called before every frame render
   * @param func defines the function to unregister
   */
  void unregisterBeforeRender(const std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Registers a function to be called after every frame render.
   * @param func defines the function to register
   */
  void registerAfterRender(const std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Unregisters a function called after every frame render.
   * @param func defines the function to unregister
   */
  void unregisterAfterRender(const std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Hidden
   */
  void _addPendingData(Mesh* mesh);

  /**
   * @brief Hidden
   */
  void _addPendingData(const InternalTexturePtr& texure);

  /**
   * @brief Hidden
   */
  void _removePendingData(Mesh* mesh);

  /**
   * @brief Hidden
   */
  void _removePendingData(const InternalTexturePtr& texture);

  /**
   * @brief Returns the number of items waiting to be loaded.
   * @returns the number of items waiting to be loaded
   */
  void getWaitingItemsCount();

  /**
   * Registers a function to be executed when the scene is ready
   * @param {Function} func - the function to be executed
   */
  void executeWhenReady(const std::function<void(Scene* scene, EventState& es)>& func);

  /**
   * @brief Hidden
   */
  void _checkIsReady();

  /** Animations **/

  /**
   * @brief Gets all animations attached to the scene.
   */
  std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Will start the animation sequence of a given target.
   * @param target defines the target
   * @param from defines from which frame should animation start
   * @param to defines until which frame should animation run.
   * @param weight defines the weight to apply to the animation (1.0 by default)
   * @param loop defines if the animation loops
   * @param speedRatio defines the speed in which to run the animation (1.0 by default)
   * @param onAnimationEnd defines the function to be executed when the animation ends
   * @param animatable defines an animatable object. If not provided a new one will be created from
   * the given params
   * @param targetMask defines if the target should be animated if animations are present (this is
   * called recursively on descendant animatables regardless of return value)
   * @param onAnimationLoop defines the callback to call when an animation loops
   * @returns the animatable object created for this animation
   */
  AnimatablePtr
  beginWeightedAnimation(const IAnimatablePtr& target, float from, float to, float weight = 1.f,
                         bool loop = false, float speedRatio = 1.f,
                         const std::function<void()>& onAnimationEnd                = nullptr,
                         AnimatablePtr animatable                                   = nullptr,
                         const std::function<bool(IAnimatable* target)>& targetMask = nullptr,
                         const std::function<void()>& onAnimationLoop               = nullptr);

  /**
   * @brief Will start the animation sequence of a given target.
   * @param target defines the target
   * @param from defines from which frame should animation start
   * @param to defines until which frame should animation run.
   * @param loop defines if the animation loops
   * @param speedRatio defines the speed in which to run the animation (1.0 by default)
   * @param onAnimationEnd defines the function to be executed when the animation ends
   * @param animatable defines an animatable object. If not provided a new one will be created from
   * the given params
   * @param stopCurrent defines if the current animations must be stopped first (true by default)
   * @param targetMask defines if the target should be animate if animations are present (this is
   * called recursively on descendant animatables regardless of return value)
   * @param onAnimationLoop defines the callback to call when an animation loops
   * @returns the animatable object created for this animation
   */
  AnimatablePtr beginAnimation(const IAnimatablePtr& target, float from, float to,
                               bool loop = false, float speedRatio = 1.f,
                               const std::function<void()>& onAnimationEnd = nullptr,
                               AnimatablePtr animatable = nullptr, bool stopCurrent = true,
                               const std::function<bool(IAnimatable* target)>& targetMask = nullptr,
                               const std::function<void()>& onAnimationLoop = nullptr);

  /**
   * @brief Will start the animation sequence of a given target and its hierarchy.
   * @param target defines the target
   * @param directDescendantsOnly if true only direct descendants will be used, if false direct and
   * also indirect (children of children, an so on in a recursive manner) descendants will be used.
   * @param from defines from which frame should animation start
   * @param to defines until which frame should animation run.
   * @param loop defines if the animation loops
   * @param speedRatio defines the speed in which to run the animation (1.0 by default)
   * @param onAnimationEnd defines the function to be executed when the
   * animation ends
   * @param animatable defines an animatable object. If not provided a new one will be created from
   * the given params
   * @param stopCurrent defines if the current animations must be stopped first (true by default)
   * @param targetMask defines if the target should be animated if animations are present (this is
   * called recursively on descendant animatables regardless of return value)
   * @param onAnimationLoop defines the callback to call when an animation loops
   * @returns the list of created animatables
   */
  std::vector<AnimatablePtr>
  beginHierarchyAnimation(const NodePtr& target, bool directDescendantsOnly, float from, float to,
                          bool loop = false, float speedRatio = 1.f,
                          const std::function<void()>& onAnimationEnd = nullptr,
                          AnimatablePtr animatable = nullptr, bool stopCurrent = true,
                          const std::function<bool(IAnimatable* target)>& targetMask = nullptr,
                          const std::function<void()>& onAnimationLoop               = nullptr);

  /**
   * @brief Begin a new animation on a given node.
   * @param target defines the target where the animation will take place
   * @param animations defines the list of animations to start
   * @param from defines the initial value
   * @param to defines the final value
   * @param loop defines if you want animation to loop (off by default)
   * @param speedRatio defines the speed ratio to apply to all animations
   * @param onAnimationEnd defines the callback to call when an animation ends (will be called once
   * per node)
   * @param onAnimationLoop defines the callback to call when an animation loops
   * @returns the list of created animatables
   */
  AnimatablePtr beginDirectAnimation(const IAnimatablePtr& target,
                                     const std::vector<AnimationPtr>& animations, float from,
                                     float to, bool loop = false, float speedRatio = 1.f,
                                     const std::function<void()>& onAnimationEnd  = nullptr,
                                     const std::function<void()>& onAnimationLoop = nullptr);

  /**
   * @brief Begin a new animation on a given node and its hierarchy.
   * @param target defines the root node where the animation will take place
   * @param directDescendantsOnly if true only direct descendants will be used, if false direct and
   * also indirect (children of children, an so on in a recursive manner) descendants will be used.
   * @param animations defines the list of animations to start
   * @param from defines the initial value
   * @param to defines the final value
   * @param loop defines if you want animation to loop (off by default)
   * @param speedRatio defines the speed ratio to apply to all animations
   * @param onAnimationEnd defines the callback to call when an animation ends
   * (will be called once per node)
   * @param onAnimationLoop defines the callback to call when an animation loops
   * @returns the list of animatables created for all nodes
   */
  std::vector<AnimatablePtr>
  beginDirectHierarchyAnimation(const NodePtr& target, bool directDescendantsOnly,
                                const std::vector<AnimationPtr>& animations, float from, float to,
                                bool loop = false, float speedRatio = 1.f,
                                const std::function<void()>& onAnimationEnd  = nullptr,
                                const std::function<void()>& onAnimationLoop = nullptr);

  /**
   * @brief Gets the animatable associated with a specific target.
   * @param target defines the target of the animatable
   * @returns the required animatable if found
   */
  AnimatablePtr getAnimatableByTarget(const IAnimatablePtr& target);

  /**
   * @brief Gets all animatables associated with a given target.
   * @param target defines the target to look animatables for
   * @returns an array of Animatables
   */
  std::vector<AnimatablePtr> getAllAnimatablesByTarget(const IAnimatablePtr& target);
  std::vector<AnimatablePtr> getAllAnimatablesByTarget(IAnimatable* target);

  /**
   * @brief Will stop the animation of the given target.
   * @param target - the target
   * @param animationName - the name of the animation to stop (all animations will be stopped if
   * both this and targetMask are empty)
   * @param targetMask - a function that determines if the animation should be stopped based on its
   * target (all animations will be stopped if both this and animationName are empty)
   */
  void stopAnimation(const IAnimatablePtr& target, const std::string& animationName = "",
                     const std::function<bool(IAnimatable* target)>& targetMask = nullptr);

  /**
   * @brief Will stop the animation of the given target.
   * @param target - the target
   * @param animationName - the name of the animation to stop (all animations will be stopped if
   * both this and targetMask are empty)
   * @param targetMask - a function that determines if the animation should be stopped based on its
   * target (all animations will be stopped if both this and animationName are empty)
   */
  void stopAnimation(IAnimatable* target, const std::string& animationName = "",
                     const std::function<bool(IAnimatable* target)>& targetMask = nullptr);

  /**
   * @brief Stops and removes all animations that have been applied to the scene.
   */
  void stopAllAnimations();

  /**
   * @brief Hidden
   */
  void _registerTargetForLateAnimationBinding(RuntimeAnimation* runtimeAnimation,
                                              const AnimationValue& originalValue);

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
   * @param recursive if all child meshes should also be added to the scene
   */
  void addMesh(const AbstractMeshPtr& newMesh, bool recursive = false);

  /**
   * @brief Remove a mesh for the list of scene's meshes.
   * @param toRemove defines the mesh to remove
   * @param recursive if all child meshes should also be removed from the scene
   * @returns the index where the mesh was in the mesh list
   */
  int removeMesh(const AbstractMeshPtr& toRemove, bool recursive = false);
  int removeMesh(AbstractMesh* toRemove, bool recursive = false);

  /**
   * @brief Add a transform node to the list of scene's transform nodes.
   * @param newTransformNode defines the transform node to add
   */
  void addTransformNode(const TransformNodePtr& newTransformNode);

  /**
   * @brief Remove a transform node for the list of scene's transform nodes.
   * @param toRemove defines the transform node to remove
   * @returns the index where the transform node was in the transform node list
   */
  int removeTransformNode(const TransformNodePtr& toRemove);
  int removeTransformNode(TransformNode* toRemove);

  /**
   * @brief Remove a skeleton for the list of scene's skeletons.
   * @param toRemove defines the skeleton to remove
   * @returns the index where the skeleton was in the skeleton list
   */
  int removeSkeleton(const SkeletonPtr& toRemove);
  int removeSkeleton(Skeleton* toRemove);

  /**
   * @brief Remove a morph target for the list of scene's morph targets.
   * @param toRemove defines the morph target to remove
   * @returns the index where the morph target was in the morph target list
   */
  int removeMorphTargetManager(const MorphTargetManagerPtr& toRemove);

  /**
   * @brief Remove a light for the list of scene's lights.
   * @param toRemove defines the light to remove
   * @returns the index where the light was in the light list
   */
  int removeLight(const LightPtr& toRemove);

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
  int removeCamera(const CameraPtr& toRemove);
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
  int removeAnimation(const AnimationPtr& toRemove);

  /**
   * @brief Removes the given animation group from this scene.
   * @param toRemove The animation group to remove
   * @returns The index of the removed animation group
   */
  int removeAnimationGroup(const AnimationGroupPtr& toRemove);

  /**
   * @brief Removes the given multi-material from this scene.
   * @param toRemove The multi-material to remove
   * @returns The index of the removed multi-material
   */
  int removeMultiMaterial(const MultiMaterialPtr& toRemove);

  /**
   * @brief Removes the given material from this scene.
   * @param toRemove The material to remove
   * @returns The index of the removed material
   */
  int removeMaterial(const MaterialPtr& toRemove);
  int removeMaterial(Material* toRemove);

  /**
   * @brief Removes the given action manager from this scene.
   * @param toRemove The action manager to remove
   * @returns The index of the removed action manager
   */
  int removeActionManager(const AbstractActionManagerPtr& toRemove);
  int removeActionManager(AbstractActionManager* toRemove);

  /**
   * @brief Removes the given texture from this scene.
   * @param toRemove The texture to remove
   * @returns The index of the removed texture
   */
  int removeTexture(const BaseTexturePtr& toRemove);
  int removeTexture(BaseTexture* toRemove);

  /**
   * @brief Adds the given light to this scene.
   * @param newLight The light to add
   */
  void addLight(const LightPtr& newLight);

  /**
   * @brief Sorts the list list based on light priorities.
   */
  void sortLightsByPriority();

  /**
   * @brief Adds the given camera to this scene.
   * @param newCamera The camera to add
   */
  void addCamera(const CameraPtr& newCamera);

  /**
   * @brief Adds the given skeleton to this scene.
   * @param newSkeleton The skeleton to add
   */
  void addSkeleton(const SkeletonPtr& newSkeleton);

  /**
   * @brief Adds the given particle system to this scene.
   * @param newParticleSystem The particle system to add
   */
  void addParticleSystem(const IParticleSystemPtr& newParticleSystem);

  /**
   * @brief Adds the given animation to this scene.
   * @param newAnimation The animation to add
   */
  void addAnimation(const AnimationPtr& newAnimation);

  /**
   * @brief Adds the given animation group to this scene.
   * @param newAnimationGroup The animation group to add
   */
  void addAnimationGroup(const AnimationGroupPtr& newAnimationGroup);

  /**
   * @brief Adds the given multi-material to this scene.
   * @param newMultiMaterial The multi-material to add
   */
  void addMultiMaterial(const MultiMaterialPtr& newMultiMaterial);

  /**
   * @brief Adds the given material to this scene.
   * @param newMaterial The material to add
   */
  void addMaterial(const MaterialPtr& newMaterial);

  /**
   * @brief Adds the given morph target to this scene.
   * @param newMorphTargetManager The morph target to add
   */
  void addMorphTargetManager(const MorphTargetManagerPtr& newMorphTargetManager);

  /**
   * @brief Adds the given geometry to this scene.
   * @param newGeometry The geometry to add
   */
  void addGeometry(const GeometryPtr& newGeometry);

  /**
   * @brief Adds the given action manager to this scene.
   * @param newActionManager The action manager to add
   */
  void addActionManager(const AbstractActionManagerPtr& newActionManager);

  /**
   * @brief Adds the given texture to this scene.
   * @param newTexture The texture to add
   */
  void addTexture(const BaseTexturePtr& newTexture);

  /**
   * @brief Switch active camera.
   * @param newCamera defines the new active camera
   * @param attachControl defines if attachControl must be called for the new active camera
   * (default: true)
   */
  void switchActiveCamera(const CameraPtr& newCamera, bool attachControl = true);

  /**
   * @brief Sets the active camera of the scene using its ID.
   * @param id defines the camera's ID
   * @return the new active camera or null if none found.
   */
  CameraPtr setActiveCameraByID(const std::string& id);

  /**
   * @brief Sets the active camera of the scene using its name.
   * @param name defines the camera's name
   * @returns the new active camera or null if none found.
   */
  CameraPtr setActiveCameraByName(const std::string& name);

  /**
   * Gets an animation group using its name.
   * @param name defines the material's name
   * @return the animation group or null if none found.
   */
  AnimationGroupPtr getAnimationGroupByName(const std::string& name);

  /**
   * @brief Gets a material using its id.
   * @param id defines the material's ID
   * @return the material or null if none found.
   */
  MaterialPtr getMaterialByID(const std::string& id);

  /**
   * @brief Gets a the last added material using a given id.
   * @param id defines the material's ID
   * @return the last material with the given id or null if none found.
   */
  MaterialPtr getLastMaterialByID(const std::string& id);

  /**
   * @brief Gets a material using its unique id.
   * @param uniqueId defines the unique id to look for
   * @returns the material or null if not found
   */
  MaterialPtr getMaterialByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a material using its name.
   * @param name defines the material's name
   * @return the material or null if none found.
   */
  MaterialPtr getMaterialByName(const std::string& name);

  /**
   * @brief Gets a camera using its id.
   * @param id defines the id to look for
   * @returns the camera or null if not found
   */
  CameraPtr getCameraByID(const std::string& id);

  /**
   * @brief Gets a camera using its unique id.
   * @param uniqueId defines the unique id to look for
   * @returns the camera or null if not found
   */
  CameraPtr getCameraByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a camera using its name.
   * @param name defines the camera's name
   * @return the camera or null if none found.
   */
  CameraPtr getCameraByName(const std::string& name);

  /**
   * @brief Gets a bone using its id.
   * @param id defines the bone's id
   * @return the bone or null if not found
   */
  BonePtr getBoneByID(const std::string& id);

  /**
   * @brief Gets a bone with its auto-generated unique id.
   * @param uniqueId defines the unique id to search for
   * @return the found bone or null if not found at all.
   */
  BonePtr getBoneByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a bone using its id.
   * @param name defines the bone's name
   * @return the bone or null if not found
   */
  BonePtr getBoneByName(const std::string& name);

  /**
   * @brief Gets a light node using its name.
   * @param name defines the the light's name
   * @return the light or null if none found.
   */
  LightPtr getLightByName(const std::string& name);

  /**
   * @brief Gets a light node using its id.
   * @param id defines the light's id
   * @return the light or null if none found.
   */
  LightPtr getLightByID(const std::string& id);

  /**
   * @brief Gets a light node using its scene-generated unique ID.
   * @param uniqueId defines the light's unique id
   * @return the light or null if none found.
   */
  LightPtr getLightByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a particle system by id.
   * @param id defines the particle system id
   * @return the corresponding system or null if none found
   */
  IParticleSystemPtr getParticleSystemByID(const std::string& id);

  /**
   * @brief Gets a geometry using its ID
   * @param id defines the geometry's id.
   * @return the geometry or null if none found.
   */
  GeometryPtr getGeometryByID(const std::string& id);

  /**
   * @brief Add a new geometry to this scene.
   * @param geometry defines the geometry to be added to the scene.
   * @param force defines if the geometry must be pushed even if a geometry with this id already
   * exists
   * @return a boolean defining if the geometry was added or not
   */
  bool pushGeometry(const GeometryPtr& geometry, bool force = false);

  /**
   * @brief Removes an existing geometry.
   * @param geometry defines the geometry to be removed from the scene
   * @return a boolean defining if the geometry was removed or not
   */
  bool removeGeometry(const GeometryPtr& geometry);
  bool removeGeometry(Geometry* geometry);

  /**
   * @brief Gets the list of geometries attached to the scene.
   * @returns an array of Geometry
   */
  std::vector<GeometryPtr>& getGeometries();

  /**
   * @brief Gets the first added mesh found of a given ID.
   * @param id defines the id to search for
   * @return the mesh found or null if not found at all
   */
  AbstractMeshPtr getMeshByID(const std::string& id);

  /**
   * @brief Gets a list of meshes using their id.
   * @param id defines the id to search for
   * @returns a list of meshes
   */
  std::vector<AbstractMeshPtr> getMeshesByID(const std::string& id);

  /**
   * @brief Gets the first added transform node found of a given ID.
   * @param id defines the id to search for
   * @return the found transform node or null if not found at all.
   */
  TransformNodePtr getTransformNodeByID(const std::string& id);

  /**
   * @brief Gets a transform node with its auto-generated unique id.
   * @param uniqueId defines the unique id to search for
   * @return the found transform node or null if not found at all.
   */
  TransformNodePtr getTransformNodeByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a list of transform nodes using their id.
   * @param id defines the id to search for
   * @returns a list of transform nodes
   */
  std::vector<TransformNodePtr> getTransformNodesByID(const std::string& id);

  /**
   * @brief Gets a mesh with its auto-generated unique id.
   * @param uniqueId defines the unique id to search for
   * @return the found mesh or null if not found at all.
   */
  AbstractMeshPtr getMeshByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a the last added mesh using a given id.
   * @param id defines the id to search for
   * @return the found mesh or null if not found at all.
   */
  AbstractMeshPtr getLastMeshByID(const std::string& id);

  /**
   * @brief Hidden
   */
  std::vector<AbstractMeshPtr> getMeshes() const;

  /**
   * @brief Gets a the last added node (Mesh, Camera, Light) using a given id.
   * @param id defines the id to search for
   * @return the found node or null if not found at all
   */
  NodePtr getLastEntryByID(const std::string& id);

  /**
   * @brief Gets a node (Mesh, Camera, Light) using a given id.
   * @param id defines the id to search for
   * @return the found node or null if not found at all
   */
  NodePtr getNodeByID(const std::string& id);

  /**
   * @brief Gets a node (Mesh, Camera, Light) using a given name.
   * @param name defines the name to search for
   * @return the found node or null if not found at all.
   */
  NodePtr getNodeByName(const std::string& name);

  /**
   * @brief Gets a mesh using a given name.
   * @param name defines the name to search for
   * @return the found mesh or null if not found at all.
   */
  AbstractMeshPtr getMeshByName(const std::string& name);

  /**
   * @brief Gets a transform node using a given name.
   * @param name defines the name to search for
   * @return the found transform node or null if not found at all.
   */
  TransformNodePtr getTransformNodeByName(const std::string& name);

  /**
   * @brief Gets a sound using a given name.
   * @param name defines the name to search for
   * @return the found sound or null if not found at all.
   */
  SoundPtr getSoundByName(const std::string& name);

  /**
   * @brief Gets a skeleton using a given id (if many are found, this function will pick the last
   * one).
   * @param id defines the id to search for
   * @return the found skeleton or null if not found at all.
   */
  SkeletonPtr getLastSkeletonByID(const std::string& id);

  /**
   * @brief Gets a skeleton using a given id (if many are found, this function will pick the first
   * one).
   * @param id defines the id to search for
   * @return the found skeleton or null if not found at all.
   */
  SkeletonPtr getSkeletonById(const std::string& id);

  /**
   * @brief Gets a skeleton using its unique id.
   * @param uniqueId defines the unique id to look for
   * @return the found skeleton or null if not found at all.
   */
  SkeletonPtr getSkeletonByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a skeleton using a given name.
   * @param name defines the name to search for
   * @return the found skeleton or null if not found at all.
   */
  SkeletonPtr getSkeletonByName(const std::string& name);

  /**
   * @brief Gets a morph target manager  using a given id (if many are found, this function will
   * pick the last one).
   * @param id defines the id to search for
   * @return the found morph target manager or null if not found at all.
   */
  MorphTargetManagerPtr getMorphTargetManagerById(unsigned int id);

  /**
   * @brief Gets a texture using its unique id.
   * @param uniqueId defines the unique id to look for
   * @return the found texture or null if not found at all.
   */
  BaseTexturePtr getTextureByUniqueID(size_t uniqueId);

  /**
   * @brief Gets a boolean indicating if the given mesh is active.
   * @param mesh defines the mesh to look for
   * @returns true if the mesh is in the active list
   */
  bool isActiveMesh(const AbstractMeshPtr& mesh);

  /**
   * @brief Clear the processed materials smart array preventing retention point in material
   * dispose.
   */
  void freeProcessedMaterials();

  /**
   * @brief Clear the active meshes smart array preventing retention point in mesh dispose.
   */
  void freeActiveMeshes();

  /**
   * @brief Clear the info related to rendering groups preventing retention points during dispose.
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
   * @brief Use this function to stop evaluating active meshes. The current list will be keep alive
   * between frames.
   * @param skipEvaluateActiveMeshes defines an optional boolean indicating that the evaluate active
   * meshes step must be completely skipped
   * @returns the current scene
   */
  Scene& freezeActiveMeshes(bool skipEvaluateActiveMeshes);

  /**
   * @brief Use this function to restart evaluating active meshes on every frame.
   * @returns the current scene
   */
  Scene& unfreezeActiveMeshes();

  /**
   * @brief Update the transform matrix to update from the current active camera.
   * @param force defines a boolean used to force the update even if cache is up to date
   */
  void updateTransformMatrix(bool force = false);

  /**
   * @brief Defines an alternate camera (used mostly in VR-like scenario where two cameras can
   * render the same scene from a slightly different point of view).
   * @param alternateCamera defines the camera to use
   */
  void updateAlternateTransformMatrix(Camera* alternateCamera);

  /**
   * @brief Execute all animations (for a frame).
   */
  void animate();

  /**
   * @brief Render the scene.
   * @param updateCameras defines a boolean indicating if cameras must update according to their
   * inputs (true by default)
   * @param ignoreAnimations defines a boolean indicating if animations should not be executed
   * (false by default)
   */
  void render(bool updateCameras = true, bool ignoreAnimations = false);

  /** Rendering **/

  /**
   * @brief Creates a depth renderer a given camera which contains a depth map which can be used for
   * post processing.
   * @param camera The camera to create the depth renderer on (default: scene's active camera)
   * @param storeNonLinearDepth Defines whether the depth is stored linearly like in Babylon Shadows
   * or directly like glFragCoord.z
   * @returns the created depth renderer
   */
  DepthRendererPtr enableDepthRenderer(const CameraPtr& camera  = nullptr,
                                       bool storeNonLinearDepth = false);

  /**
   * @brief Disables a depth renderer for a given camera.
   * @param camera The camera to disable the depth renderer on (default: scene's active camera)
   */
  void disableDepthRenderer(const CameraPtr& camera = nullptr);

  /**
   * @brief Enables a GeometryBufferRender and associates it with the scene.
   * @param ratio defines the scaling ratio to apply to the renderer (1 by default which means same
   * resolution)
   * @returns the GeometryBufferRenderer
   */
  GeometryBufferRendererPtr& enableGeometryBufferRenderer(float ratio = 1.f);

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
   * @brief Call this function to reduce memory footprint of the scene.
   * Vertex buffers will not store CPU data anymore (this will prevent picking, collisions or
   * physics to work correctly)
   */
  void clearCachedVertexData();

  /**
   * @brief This function will remove the local cached buffer data from texture.
   * It will save memory but will prevent the texture from being rebuilt
   */
  void cleanCachedTextureBuffer();

  /** Octrees **/

  /**
   * @brief Get the world extend vectors with an optional filter.
   * @param filterPredicate the predicate - which meshes should be included when calculating the
   * world size
   * @returns {{ min: Vector3; max: Vector3 }} min and max vectors
   */
  MinMax getWorldExtends(const std::function<bool(const AbstractMeshPtr& mesh)>& filterPredicate
                         = nullptr);

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
   * @param world defines the world matrix to use if you want to pick in object space (instead of
   * world space)
   * @param camera defines the camera to use for the picking
   * @param cameraViewSpace defines if picking will be done in view space (false by default)
   * @returns a Ray
   */
  Ray createPickingRay(int x, int y, Matrix& world, const CameraPtr& camera = nullptr,
                       bool cameraViewSpace = false);

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param world defines the world matrix to use if you want to pick in object space (instead of
   * world space)
   * @param result defines the ray where to store the picking ray
   * @param camera defines the camera to use for the picking
   * @param cameraViewSpace defines if picking will be done in view space (false by default)
   * @returns the current scene
   */
  Scene& createPickingRayToRef(int x, int y, const std::optional<Matrix>& world, Ray& result,
                               CameraPtr camera, bool cameraViewSpace = false);

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param camera defines the camera to use for the picking
   * @returns a Ray
   */
  Ray createPickingRayInCameraSpace(int x, int y, const CameraPtr& camera = nullptr);

  /**
   * @brief Creates a ray that can be used to pick in the scene.
   * @param x defines the x coordinate of the origin (on-screen)
   * @param y defines the y coordinate of the origin (on-screen)
   * @param result defines the ray where to store the picking ray
   * @param camera defines the camera to use for the picking
   * @returns the current scene
   */
  Scene& createPickingRayInCameraSpaceToRef(int x, int y, Ray& result, CameraPtr camera = nullptr);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene.
   * @param x position on screen
   * @param y position on screen
   * @param predicate Predicate function used to determine eligible meshes. Can be set to null. In
   * this case, a mesh must be enabled, visible and with isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be set to null.
   * @param camera to use for computing the picking ray. Can be set to null. In this case, the
   * scene.activeCamera will be used
   * @returns a PickingInfo
   */
  std::optional<PickingInfo>
  pick(int x, int y, const std::function<bool(const AbstractMeshPtr& mesh)>& predicate = nullptr,
       bool fastCheck = false, const CameraPtr& camera = nullptr);

  /**
   * @brief Launch a ray to try to pick a sprite in the scene.
   * @param x position on screen
   * @param y position on screen
   * @param predicate Predicate function used to determine eligible sprites. Can be set to null. In
   * this case, a sprite must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be set to null.
   * @param camera camera to use for computing the picking ray. Can be set to null. In this case,
   * the scene.activeCamera will be used
   * @returns a PickingInfo
   */
  std::optional<PickingInfo>
  pickSprite(int x, int y, const std::function<bool(Sprite* sprite)>& predicate = nullptr,
             bool fastCheck = false, const CameraPtr& camera = nullptr);

  /**
   * @brief Use the given ray to pick a sprite in the scene.
   * @param ray The ray (in world space) to use to pick meshes
   * @param predicate Predicate function used to determine eligible sprites. Can be set to null. In
   * this case, a sprite must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be set to null.
   * @param camera camera to use. Can be set to null. In this case, the scene.activeCamera will be
   * used
   * @returns a PickingInfo
   */
  std::optional<PickingInfo> pickSpriteWithRay(const Ray& ray,
                                               const std::function<bool(Sprite* sprite)>& predicate,
                                               bool fastCheck, CameraPtr& camera);

  /**
   * @brief Hidden
   */
  std::vector<PickingInfo>
  _internalMultiPickSprites(const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate,
                            CameraPtr& camera);

  /**
   * @brief Launch a ray to try to pick sprites in the scene.
   * @param x position on screen
   * @param y position on screen
   * @param predicate Predicate function used to determine eligible sprites. Can be set to null. In
   * this case, a sprite must have isPickable set to true
   * @param camera camera to use for computing the picking ray. Can be set to null. In this case,
   * the scene.activeCamera will be used
   * @returns a PickingInfo array
   */
  std::vector<PickingInfo> multiPickSprite(int x, int y,
                                           const std::function<bool(Sprite* sprite)>& predicate,
                                           CameraPtr& camera);

  /**
   * @brief Use the given ray to pick sprites in the scene.
   * @param ray The ray (in world space) to use to pick meshes
   * @param predicate Predicate function used to determine eligible sprites. Can be set to null. In
   * this case, a sprite must have isPickable set to true
   * @param camera camera to use. Can be set to null. In this case, the scene.activeCamera will be
   * used
   * @returns a PickingInfo array
   */
  std::vector<PickingInfo>
  multiPickSpriteWithRay(const Ray& ray, const std::function<bool(Sprite* sprite)>& predicate,
                         CameraPtr& camera);

  /**
   * @brief Use the given ray to pick a mesh in the scene.
   * @param ray The ray to use to pick meshes
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must have isPickable set to true
   * @param fastCheck Launch a fast check only using the bounding boxes. Can be
   * set to null
   * @returns a PickingInfo
   */
  std::optional<PickingInfo>
  pickWithRay(const Ray& ray,
              const std::function<bool(const AbstractMeshPtr& mesh)>& predicate = nullptr,
              bool fastCheck                                                    = false);

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
  std::vector<std::optional<PickingInfo>>
  multiPick(int x, int y, const std::function<bool(AbstractMesh* mesh)>& predicate,
            const CameraPtr& camera = nullptr);

  /**
   * @brief Launch a ray to try to pick a mesh in the scene.
   * @param ray Ray to use
   * @param predicate Predicate function used to determine eligible meshes. Can
   * be set to null. In this case, a mesh must be enabled, visible and with
   * isPickable set to true
   * @returns an array of PickingInfo
   */
  std::vector<std::optional<PickingInfo>>
  multiPickWithRay(const Ray& ray, const std::function<bool(AbstractMesh* mesh)>& predicate);

  /**
   * @brief Force the value of meshUnderPointer.
   * @param mesh defines the mesh to use
   */
  void setPointerOverMesh(AbstractMesh* mesh);

  /**
   * @brief Gets the mesh under the pointer.
   * @returns a Mesh or null if no mesh is under the pointer
   */
  AbstractMeshPtr& getPointerOverMesh();

  /**
   * @brief Force the sprite under the pointer.
   * @param sprite defines the sprite to use
   */
  void setPointerOverSprite(const SpritePtr& sprite);

  /**
   * @brief Gets the sprite under the pointer.
   * @returns a Sprite or null if no sprite is under the pointer
   */
  SpritePtr& getPointerOverSprite();

  /** Physics **/

  /**
   * @brief Gets the current physics engine.
   * @returns a PhysicsEngine or null if none attached
   */
  IPhysicsEnginePtr& getPhysicsEngine();

  /**
   * @brief Enables physics to the current scene.
   * @param gravity defines the scene's gravity for the physics engine
   * @param plugin defines the physics engine to be used. defaults to OimoJS.
   * @return a boolean indicating if the physics engine was initialized
   */
  bool enablePhysics(const std::optional<Vector3>& gravity = std::nullopt,
                     IPhysicsEnginePlugin* plugin          = nullptr);

  /**
   * @brief Disables and disposes the physics engine associated with the scene.
   */
  void disablePhysicsEngine();

  /**
   * @brief Gets a boolean indicating if there is an active physics engine.
   * @returns a boolean indicating if there is an active physics engine
   */
  bool isPhysicsEnabled();

  /**
   * @brief Hidden
   */
  void _advancePhysicsEngineStep(float step);

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
   * @see http://doc.babylonjs.com/How_To/Fast_Build#create-default-light
   * @param replace has the default false, when true replaces the existing
   * lights in the scene with a hemispheric light
   */
  void createDefaultLight(bool replace = false);

  /**
   * @brief Creates a default camera for the scene.
   * @see http://doc.babylonjs.com/How_To/Fast_Build#create-default-camera
   * @param createArcRotateCamera has the default false which creates a free
   * camera, when true creates an arc rotate camera
   * @param replace has default false, when true replaces the active camera in
   * the scene
   * @param attachCameraControls has default false, when true attaches camera
   * controls to the canvas.
   */
  void createDefaultCamera(bool createArcRotateCamera = false, bool replace = false,
                           bool attachCameraControls = false);

  /**
   * @brief Creates a default camera and a default light.
   * @see
   * http://doc.babylonjs.com/how_to/Fast_Build#create-default-camera-or-light
   * @param createArcRotateCamera has the default false which creates a free
   * camera, when true creates an arc rotate camera
   * @param replace has the default false, when true replaces the active
   * camera/light in the scene
   * @param attachCameraControls has the default false, when true attaches
   * camera controls to the canvas.
   */
  void createDefaultCameraOrLight(bool createArcRotateCamera = false, bool replace = false,
                                  bool attachCameraControls = false);

  /**
   * @brief Creates a new sky box.
   * @see http://doc.babylonjs.com/how_to/Fast_Build#create-default-skybox
   * @param environmentTexture defines the texture to use as environment texture
   * @param pbr has default false which requires the StandardMaterial to be
   * used, when true PBRMaterial must be used
   * @param scale defines the overall scale of the skybox
   * @param blur is only available when pbr is true, default is 0, no blur,
   * maximum value is 1
   * @param setGlobalEnvTexture has default true indicating that
   * scene.environmentTexture must match the current skybox texture
   * @returns a new mesh holding the sky box
   */
  MeshPtr createDefaultSkybox(BaseTexturePtr environmentTexture = nullptr, bool pbr = false,
                              float scale = 1000.f, float blur = 0.f,
                              bool setGlobalEnvTexture = true);

  /**
   * Creates a new environment.
   * @see http://doc.babylonjs.com/How_To/Fast_Build#create-default-environment
   * @param options defines the options you can use to configure the environment
   * @returns the new EnvironmentHelper
   */
  std::unique_ptr<EnvironmentHelper>
  createDefaultEnvironment(const std::optional<IEnvironmentHelperOptions>& options = std::nullopt);

  /** Tags **/

  /**
   * @brief Get a list of meshes by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Mesh
   */
  std::vector<Mesh*> getMeshesByTags();

  /**
   * @brief Get a list of cameras by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Camera
   */
  std::vector<Camera*> getCamerasByTags();

  /**
   * @brief Get a list of lights by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Light
   */
  std::vector<Light*> getLightsByTags();

  /**
   * @brief Get a list of materials by tags.
   * @param tagsQuery defines the tags query to use
   * @param forEach defines a predicate used to filter results
   * @returns an array of Material
   */
  std::vector<Material*> getMaterialByTags();

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
    const std::function<int(const SubMesh* a, const SubMesh* b)>& opaqueSortCompareFn    = nullptr,
    const std::function<int(const SubMesh* a, const SubMesh* b)>& alphaTestSortCompareFn = nullptr,
    const std::function<int(const SubMesh* a, const SubMesh* b)>& transparentSortCompareFn
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
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId, bool autoClearDepthStencil,
                                         bool depth = true, bool stencil = true);

  /**
   * @brief Gets the current auto clear configuration for one rendering group of
   * the rendering manager.
   * @param index the rendering group index to get the information for
   * @returns The auto clear setup for the requested rendering group
   */
  std::optional<IRenderingManagerAutoClearSetup> getAutoClearDepthStencilSetup(size_t index);

  /**
   * @brief Will flag all materials as dirty to trigger new shader compilation.
   * @param flag defines the flag used to specify which material part must be
   * marked as dirty
   * @param predicate If not null, it will be used to specifiy if a material has
   * to be marked as dirty
   */
  void markAllMaterialsAsDirty(unsigned int flag,
                               const std::function<bool(Material* mat)>& predicate = nullptr);

  /**
   * @brief Hidden
   */
  IFileRequest
  _loadFile(const std::string& url,
            const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                                     const std::string& responseURL)>& onSuccess);

  /**
   * @brief Hidden
   */
  std::variant<std::string, ArrayBuffer>
  _loadFileAsync(const std::string& url, const std::optional<bool>& useDatabase = std::nullopt,
                 const std::optional<bool>& useArrayBuffer = std::nullopt);

  /**
   * @brief Hidden
   */
  void _createMultiviewUbo();

  /**
   * @brief Hidden
   */
  void _updateMultiviewUbo(std::optional<Matrix> viewR       = std::nullopt,
                           std::optional<Matrix> projectionR = std::nullopt);

  /**
   * @brief Hidden
   */
  void _renderMultiviewToSingleView(const CameraPtr& camera);

protected:
  /**
   * @brief Creates a new Scene.
   * @param engine defines the engine to use to render this scene
   */
  explicit Scene(Engine* engine, const std::optional<SceneOptions>& options = std::nullopt);

private:
  /**
   * @brief Registers the transient components if needed.
   */
  void _registerTransientComponents();

  void _updatePointerPosition(const PointerEvent& evt);
  void _createUbo();
  void _createAlternateUbo();
  // Pointers handling
  std::optional<PickingInfo>
  _pickSpriteButKeepRay(const std::optional<PickingInfo>& originalPointerInfo, int x, int y,
                        const std::function<bool(Sprite* sprite)>& predicate = nullptr,
                        bool fastCheck = false, const CameraPtr& camera = nullptr);
  void _setRayOnPointerInfo(PointerInfo& pointerInfo);
  Scene& _processPointerMove(std::optional<PickingInfo>& pickResult, const PointerEvent& evt);
  bool _checkPrePointerObservable(const std::optional<PickingInfo>& pickResult,
                                  const PointerEvent& evt, PointerEventTypes type);
  Scene& _processPointerDown(std::optional<PickingInfo>& pickResult, const PointerEvent& evt);
  Scene& _processPointerUp(std::optional<PickingInfo>& pickResult, const PointerEvent& evt,
                           const ClickInfo& clickInfo);
  void _animate();
  /**
   * @brief Hidden
   */
  AnimationValue
  _processLateAnimationBindingsForMatrices(float holderTotalWeight,
                                           std::vector<RuntimeAnimation*>& holderAnimations,
                                           Matrix& holderOriginalValue);
  /**
   * @brief Hidden
   */
  Quaternion _processLateAnimationBindingsForQuaternions(
    float holderTotalWeight, std::vector<RuntimeAnimation*>& holderAnimations,
    Quaternion& holderOriginalValue, Quaternion& refQuaternion);
  /**
   * @brief Hidden
   */
  void _processLateAnimationBindings();
  void _evaluateSubMesh(SubMesh* subMesh, AbstractMesh* mesh, AbstractMesh* initialMesh);
  void _evaluateActiveMeshes();
  void _activeMesh(AbstractMesh* sourceMesh, AbstractMesh* mesh);
  void _renderForCamera(const CameraPtr& camera, const CameraPtr& rigParent = nullptr);
  void _bindFrameBuffer();
  void _processSubCameras(const CameraPtr& camera);
  void _checkIntersections();
  /** Pointers handling **/
  void _onPointerMoveEvent(PointerEvent&& evt);
  void _onPointerDownEvent(PointerEvent&& evt);
  void _onPointerUpEvent(PointerEvent&& evt);
  void _onKeyDownEvent(KeyboardEvent&& evt);
  void _onKeyUpEvent(KeyboardEvent&& evt);
  /** Picking **/
  std::optional<PickingInfo>
  _internalPick(const std::function<Ray(Matrix& world)>& rayFunction,
                const std::function<bool(const AbstractMeshPtr& mesh)>& predicate, bool fastCheck);
  std::vector<std::optional<PickingInfo>>
  _internalMultiPick(const std::function<Ray(Matrix& world)>& rayFunction,
                     const std::function<bool(AbstractMesh* mesh)>& predicate);

  /**
   * @brief hidden
   */
  std::optional<PickingInfo>
  _internalPickSprites(const Ray& ray,
                       const std::function<bool(Sprite* sprite)>& predicate = nullptr,
                       bool fastCheck = false, CameraPtr camera = nullptr);
  /** Tags **/
  std::vector<std::string> _getByTags();

protected:
  /**
   * @brief Returns the texture used in all pbr material as the reflection
   * texture.
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to reference from here than from all the
   * materials.
   */
  BaseTexturePtr& get_environmentTexture() override;

  /**
   * @brief Sets the texture used in all pbr material as the reflection texture.
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to set here than in all the materials.
   */
  void set_environmentTexture(const BaseTexturePtr& value) override;

  /**
   * @brief Intensity of the environment in all pbr material.
   * This dims or reinforces the IBL lighting overall (reflection and diffuse).
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to reference from here than from all the
   * materials.
   */
  float get_environmentIntensity() const;

  /**
   * @brief Intensity of the environment in all pbr material.
   * This dims or reinforces the IBL lighting overall (reflection and diffuse).
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to set here than in all the materials.
   */
  void set_environmentIntensity(float value);

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
  ImageProcessingConfigurationPtr& get_imageProcessingConfiguration();

  /**
   * @brief Sets a boolean indicating if all rendering must be done in wireframe.
   */
  void set_forceWireframe(bool value);

  /**
   * @brief Gets a boolean indicating if all rendering must be done in wireframe.
   */
  bool get_forceWireframe() const;

  /**
   * @brief Sets a boolean indicating if we should skip the frustum clipping part of the active
   * meshes selection.
   */
  void set_skipFrustumClipping(bool value);

  /**
   * @brief Gets a boolean indicating if we should skip the frustum clipping part of the active
   * meshes selection.
   */
  bool get_skipFrustumClipping() const;

  /**
   * @brief Sets a boolean indicating if all rendering must be done in point
   * cloud.
   */
  void set_forcePointsCloud(bool value);

  /**
   * @brief Gets a boolean indicating if all rendering must be done in point
   * cloud.
   */
  bool get_forcePointsCloud() const;

  /**
   * Sets a boolean indicating if all bounding boxes must be rendered.
   */
  void set_forceShowBoundingBoxes(bool value);

  /**
   * Gets  a boolean indicating if all bounding boxes must be rendered.
   */
  bool get_forceShowBoundingBoxes() const;

  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverridePtr& get_animationPropertiesOverride() override;

  /**
   * @brief Sets the animation properties override.
   */
  void set_animationPropertiesOverride(const AnimationPropertiesOverridePtr& value) override;

  // Events
  /**
   * @brief Sets a function to be executed when this scene is disposed.
   */
  void set_onDispose(const std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed before rendering this scene.
   */
  void set_beforeRender(const std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed after rendering this scene.
   */
  void set_afterRender(const std::function<void(Scene* scene, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed before rendering a camera.
   */
  void set_beforeCameraRender(const std::function<void(Camera* camera, EventState& es)>& callback);

  /**
   * @brief Sets a function to be executed after rendering a camera.
   */
  void set_afterCameraRender(const std::function<void(Camera* camera, EventState& es)>& callback);

  // Gamepads

  /**
   * @brief Gets the gamepad manager associated with the scene.
   * @see http://doc.babylonjs.com/how_to/how_to_use_gamepads
   */
  std::unique_ptr<GamepadManager>& get_gamepadManager();

  // Pointers

  /**
   * @brief Gets the pointer coordinates without any translation (ie. straight
   * out of the pointer event).
   */
  Vector2& get_unTranslatedPointer();

  /**
   * @brief Gets a boolean indicating if the scene must use right-handed
   * coordinates system.
   */
  bool get_useRightHandedSystem() const;

  /**
   * @brief Sets a boolean indicating if the scene must use right-handed
   * coordinates system.
   */
  void set_useRightHandedSystem(bool value);

  /**
   * @brief Gets or sets a boolean indicating if fog is enabled on this scene.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  void set_fogEnabled(bool value);

  /**
   * @brief Gets or sets a boolean indicating if fog is enabled on this scene.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  bool get_fogEnabled() const;

  /**
   * @brief Sets the fog mode to use.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  void set_fogMode(unsigned int value);

  /**
   * @brief Gets the fog mode to use.
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  unsigned int get_fogMode() const;

  /**
   * @brief Sets a boolean indicating if shadows are enabled on this scene.
   */
  void set_shadowsEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if shadows are enabled on this scene.
   */
  bool get_shadowsEnabled() const;

  /**
   * @brief Sets a boolean indicating if lights are enabled on this scene.
   */
  void set_lightsEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if lights are enabled on this scene.
   */
  bool get_lightsEnabled() const;

  /**
   * @brief Gets the current active camera.
   */
  CameraPtr& get_activeCamera();

  /**
   * @brief Sets the current active camera.
   */
  void set_activeCamera(const CameraPtr& value);

  /**
   * @brief Gets the default material used on meshes when no material is
   * affected.
   */
  MaterialPtr& get_defaultMaterial();

  /**
   * @brief Sets the default material used on meshes when no material is
   * affected.
   */
  void set_defaultMaterial(const MaterialPtr& value);

  /**
   * @brief Sets a boolean indicating if textures are enabled on this scene.
   */
  void set_texturesEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if textures are enabled on this scene.
   */
  bool get_texturesEnabled() const;

  /**
   * @brief Sets a boolean indicating if skeletons are enabled on this scene.
   */
  void set_skeletonsEnabled(bool value);

  /**
   * @brief Gets a boolean indicating if skeletons are enabled on this scene.
   */
  bool get_skeletonsEnabled() const;

  /** Hidden */
  std::unique_ptr<ICollisionCoordinator>& get_collisionCoordinator();

  /**
   * @brief Gets the postprocess render pipeline manager.
   * @see http://doc.babylonjs.com/how_to/how_to_use_postprocessrenderpipeline
   * @see http://doc.babylonjs.com/how_to/using_default_rendering_pipeline
   */
  std::unique_ptr<PostProcessRenderPipelineManager>& get_postProcessRenderPipelineManager();

  /**
   * @brief Gets the main sound track played by the scene.
   * It cotains your primary collection of sounds.
   */
  SoundTrackPtr& get_mainSoundTrack();

  /**
   * Gets the simplification queue attached to the scene.
   * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
   */
  SimplificationQueuePtr& get_simplificationQueue();

  /**
   * Sets the simplification queue attached to the scene.
   * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
   */
  void set_simplificationQueue(const SimplificationQueuePtr& value);

  /**
   * @brief Hidden
   */
  bool get_isAlternateRenderingEnabled() const;

  /**
   * @brief Gets the list of frustum planes (built from the active camera).
   */
  std::array<Plane, 6>& get_frustumPlanes();

  /**
   * @brief Hidden (Backing field)
   */
  std::unordered_map<std::string, DepthRendererPtr>& get_depthRenderer();

  /**
   * @brief Gets the current geometry buffer associated to the scene.
   */
  GeometryBufferRendererPtr& get_geometryBufferRenderer();

  /**
   * @brief Sets the current geometry buffer for the scene.
   */
  void set_geometryBufferRenderer(const GeometryBufferRendererPtr& value);

  /**
   * @brief Gets the debug layer (aka Inspector) associated with the scene.
   * @see http://doc.babylonjs.com/features/playground_debuglayer
   */
  std::unique_ptr<DebugLayer>& get_debugLayer();

  /**
   * @brief Gets the octree used to boost mesh selection (picking).
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
   */
  Octree<AbstractMesh*>*& get_selectionOctree();

  /**
   * @brief Gets the mesh that is currently under the pointer.
   */
  AbstractMeshPtr& get_meshUnderPointer();

  /**
   * @brief Gets the current on-screen X position of the pointer.
   */
  int get_pointerX() const;

  /**
   * @brief Sets the current on-screen X position of the pointer.
   */
  void set_pointerX(int value);

  /**
   * @brief Gets the current on-screen Y position of the pointer.
   */
  int get_pointerY() const;

  /**
   * @brief Sets the current on-screen Y position of the pointer.
   */
  void set_pointerY(int value);

  /**
   * @brief Gets the performance counter for total vertices.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& get_totalVerticesPerfCounter();

  /**
   * @brief Gets the performance counter for active indices.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& get_totalActiveIndicesPerfCounter();

  /**
   * @brief Gets the performance counter for active particles.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& get_activeParticlesPerfCounter();

  /**
   * @brief Gets the performance counter for active bones.
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  PerfCounter& get_activeBonesPerfCounter();

  /**
   * @brief Returns a boolean indicating if the scene is still loading data.
   */
  bool get_isLoading() const;

  /**
   * @brief Return a unique id as a string which can serve as an identifier for
   * the scene.
   */
  std::string get_uid() const;

  /** Audio **/

  /**
   * @brief Gets if audio support is enabled.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  std::optional<bool>& get_audioEnabled();

  /**
   * @brief Sets if audio support is enabled.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  void set_audioEnabled(const std::optional<bool>& value);

  /**
   * @brief Gets if audio will be output to headphones.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  std::optional<bool>& get_headphone();

  /**
   * @brief Sets if audio will be output to headphones.
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  void set_headphone(const std::optional<bool>& value);

  /**
   * @brief Gets if the scene is already disposed.
   */
  bool get_isDisposed() const;

  /**
   * @brief Gets a boolean blocking all the calls to markAllMaterialsAsDirty
   * (ie. the materials won't be updated if they are out of sync)
   */
  bool get_blockMaterialDirtyMechanism() const;

  /**
   * @brief Sets a boolean blocking all the calls to markAllMaterialsAsDirty
   * (ie. the materials won't be updated if they are out of sync)
   */
  void set_blockMaterialDirtyMechanism(bool value);

public:
  // Members

  /**
   * Hidden
   */
  bool _blockEntityCollection;

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

  /**
   * This is use to store the default BRDF lookup for PBR materials in your
   * scene. It should only be one of the following (if not the default embedded
   * one):
   * * For uncorrelated BRDF (pbr.brdf.useEnergyConservation = false and
   * pbr.brdf.useSmithVisibilityHeightCorrelated = false) :
   * https://assets.babylonjs.com/environments/uncorrelatedBRDF.dds
   * * For correlated BRDF (pbr.brdf.useEnergyConservation = false and
   * pbr.brdf.useSmithVisibilityHeightCorrelated = true) :
   * https://assets.babylonjs.com/environments/correlatedBRDF.dds
   * * For correlated multi scattering BRDF (pbr.brdf.useEnergyConservation =
   * true and pbr.brdf.useSmithVisibilityHeightCorrelated = true) :
   * https://assets.babylonjs.com/environments/correlatedMSBRDF.dds The material
   * properties need to be setup according to the type of texture in use.
   */
  BaseTexturePtr environmentBRDFTexture;

  /**
   * Intensity of the environment in all pbr material.
   * This dims or reinforces the IBL lighting overall (reflection and diffuse).
   * As in the majority of the scene they are the same (exception for multi room
   * and so on), this is easier to set here than in all the materials.
   */
  Property<Scene, float> environmentIntensity;

  /**
   * Default image processing configuration used either in the rendering
   * Forward main pass or through the imageProcessingPostProcess if present.
   * As in the majority of the scene they are the same (exception for multi
   * camera), this is easier to reference from here than from all the materials
   * and post process.
   *
   * No setter as we it is a shared configuration, you can set the values
   * instead.
   */
  ReadOnlyProperty<Scene, ImageProcessingConfigurationPtr> imageProcessingConfiguration;

  // Events

  /**
   * An event triggered when the scene is disposed.
   */
  Observable<Scene> onDisposeObservable;

  /**
   * Sets a function to be executed when this scene is disposed
   */
  WriteOnlyProperty<Scene, std::function<void(Scene* scene, EventState& es)>> onDispose;

  /**
   * An event triggered before rendering the scene (right after animations and
   * physics)
   */
  Observable<Scene> onBeforeRenderObservable;

  /**
   * Sets a function to be executed before rendering this scene
   */
  WriteOnlyProperty<Scene, std::function<void(Scene* scene, EventState& es)>> beforeRender;

  /**
   * An event triggered after rendering the scene
   */
  Observable<Scene> onAfterRenderObservable;

  /**
   * Sets a function to be executed after rendering this scene
   */
  WriteOnlyProperty<Scene, std::function<void(Scene* scene, EventState& es)>> afterRender;

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
   * Sets a function to be executed before rendering a camera
   */
  WriteOnlyProperty<Scene, std::function<void(Camera* scene, EventState& es)>> beforeCameraRender;
  /**
   * An event triggered after rendering a camera
   */
  Observable<Camera> onAfterCameraRenderObservable;

  /**
   * Sets a function to be executed after rendering a camera
   */
  WriteOnlyProperty<Scene, std::function<void(Camera* scene, EventState& es)>> afterCameraRender;

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
   * Note: This event can be trigger more than once per frame (because sprites can be rendered by
   * render target textures as well)
   */
  Observable<Scene> onBeforeSpritesRenderingObservable;

  /**
   * An event triggered when sprites rendering is done
   * Note: This event can be trigger more than once per frame (because sprites can be rendered by
   * render target textures as well)
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
   * An event triggered when a material is created
   */
  Observable<Material> onNewMaterialAddedObservable;

  /**
   * An event triggered when a material is removed
   */
  Observable<Material> onMaterialRemovedObservable;

  /**
   * An event triggered when a texture is created
   */
  Observable<BaseTexture> onNewTextureAddedObservable;

  /**
   * An event triggered when a texture is removed
   */
  Observable<BaseTexture> onTextureRemovedObservable;

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
   * An event triggered when the activeCamera property is updated
   */
  Observable<Scene> onActiveCameraChanged;

  /**
   * This Observable will be triggered before rendering each renderingGroup of
   * each rendered camera. The RenderinGroupInfo class contains all the
   * information about the context in which the observable is called If you wish
   * to register an Observer only for a given set of renderingGroup, use the
   * mask with a combination of the renderingGroup index elevated to the power
   * of two (1 for renderingGroup 0, 2 for renderingrOup1, 4 for 2 and 8 for 3)
   */
  Observable<RenderingGroupInfo> onBeforeRenderingGroupObservable;

  /**
   * This Observable will be triggered after rendering each renderingGroup of
   * each rendered camera. The RenderinGroupInfo class contains all the
   * information about the context in which the observable is called If you wish
   * to register an Observer only for a given set of renderingGroup, use the
   * mask with a combination of the renderingGroup index elevated to the power
   * of two (1 for renderingGroup 0, 2 for renderingrOup1, 4 for 2 and 8 for 3)
   */
  Observable<RenderingGroupInfo> onAfterRenderingGroupObservable;

  /**
   * This Observable will when a mesh has been imported into the scene.
   */
  Observable<AbstractMesh> onMeshImportedObservable;

  /**
   * This Observable will when an animation file has been imported into the scene.
   */
  Observable<Scene> onAnimationFileImportedObservable;

  // Pointers

  /**
   * Gets or sets a predicate used to select candidate meshes for a pointer down
   * event
   */
  std::function<bool(const AbstractMeshPtr& mesh)> pointerDownPredicate;
  /**
   * Gets or sets a predicate used to select candidate meshes for a pointer up
   * event
   */
  std::function<bool(const AbstractMeshPtr& mesh)> pointerUpPredicate;
  /**
   * Gets or sets a predicate used to select candidate meshes for a pointer move
   * event
   */
  std::function<bool(const AbstractMeshPtr& mesh)> pointerMovePredicate;

  /** Deprecated. Use onPointerObservable instead */
  std::function<void(const PointerEvent& evt, const std::optional<PickingInfo>& pickInfo,
                     PointerEventTypes type)>
    onPointerMove;
  /** Deprecated. Use onPointerObservable instead */
  std::function<void(const PointerEvent& evt, const std::optional<PickingInfo>& pickInfo,
                     PointerEventTypes type)>
    onPointerDown;
  /** Deprecated. Use onPointerObservable instead */
  std::function<void(const PointerEvent& evt, const std::optional<PickingInfo>& pickInfo,
                     PointerEventTypes type)>
    onPointerUp;
  /** Deprecated. Use onPointerObservable instead */
  std::function<void(const PointerEvent& evt, const std::optional<PickingInfo>& pickInfo)>
    onPointerPick;

  /**
   * Gets a boolean indicating if all rendering must be done in wireframe.
   */
  Property<Scene, bool> forceWireframe;

  /**
   * Gets or sets a boolean indicating if we should skip the frustum clipping part of the active
   * meshes selection.
   */
  Property<Scene, bool> skipFrustumClipping;

  /**
   * Gets a boolean indicating if all rendering must be done in point cloud
   */
  Property<Scene, bool> forcePointsCloud;

  /**
   * Gets or sets the active clipplane 1
   */
  std::optional<Plane> clipPlane;

  /**
   * Gets or sets the active clipplane 2
   */
  std::optional<Plane> clipPlane2;

  /**
   * Gets or sets the active clipplane 3
   */
  std::optional<Plane> clipPlane3;

  /**
   * Gets or sets the active clipplane 4
   */
  std::optional<Plane> clipPlane4;

  /**
   * Gets or sets the active clipplane 5
   */
  std::optional<Plane> clipPlane5;

  /**
   * Gets or sets the active clipplane 6
   */
  std::optional<Plane> clipPlane6;

  /**
   * Gets or sets a boolean indicating if all bounding boxes must be rendered
   */
  Property<Scene, bool> forceShowBoundingBoxes;

  /**
   * Gets or sets a boolean indicating if animations are enabled
   */
  bool animationsEnabled;

  /**
   * Gets or sets a boolean indicating if a constant deltatime has to be used
   * This is mostly useful for testing purposes when you do not want the
   * animations to scale with the framerate
   */
  bool useConstantAnimationDeltaTime;

  /**
   * Gets the current delta time used by animation engine
   */
  float deltaTime;

  /**
   * Gets or sets a boolean indicating if the scene must keep the
   * meshUnderPointer property updated Please note that it requires to run a ray
   * cast through the scene on every frame
   */
  bool constantlyUpdateMeshUnderPointer;

  /**
   * Defines the HTML cursor to use when hovering over interactive elements
   */
  std::string hoverCursor;

  /**
   * Defines the HTML default cursor to use (empty by default)
   */
  std::string defaultCursor;

  /**
   * Defines wether cursors are handled by the scene.
   */
  bool doNotHandleCursors;

  /**
   * This is used to call preventDefault() on pointer down
   * in order to block unwanted artifacts like system double clicks
   */
  bool preventDefaultOnPointerDown;

  /**
   * This is used to call preventDefault() on pointer up
   * in order to block unwanted artifacts like system double clicks
   */
  bool preventDefaultOnPointerUp;

  // Metadata

  /**
   * Gets or sets user defined metadata
   */
  json metadata;

  /**
   * Gets the name of the plugin used to load this scene (null by default)
   */
  std::string loadingPluginName;

  /**
   * Use this array to add regular expressions used to disable offline support
   * for specific urls
   */
  std::vector<std::regex> disableOfflineSupportExceptionRules;

  // Gamepads

  /**
   * Gets the gamepad manager associated with the scene
   * @see http://doc.babylonjs.com/how_to/how_to_use_gamepads
   */
  ReadOnlyProperty<Scene, std::unique_ptr<GamepadManager>> gamepadManager;

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
   * Gets the pointer coordinates without any translation (ie. straight
   * out of the pointer event)
   */
  ReadOnlyProperty<Scene, Vector2> unTranslatedPointer;

  /**
   * Define this parameter if you are using multiple cameras and you want to
   * specify which one should be used for pointer position.
   */
  CameraPtr cameraToUseForPointers;

  // Mirror

  /** Hidden */
  std::unique_ptr<Vector3> _mirroredCameraPosition;

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
   * Gets a boolean indicating if the scene must use right-handed
   * coordinates system.
   */
  Property<Scene, bool> useRightHandedSystem;

  // Fog

  /**
   * Gets or sets a boolean indicating if fog is enabled on this scene
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   */
  Property<Scene, bool> fogEnabled;

  /**
   * Gets or sets the fog mode to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   * | mode | value |
   * | --- | --- |
   * | FOGMODE_NONE | 0 |
   * | FOGMODE_EXP | 1 |
   * | FOGMODE_EXP2 | 2 |
   * | FOGMODE_LINEAR | 3 |
   */
  Property<Scene, unsigned int> fogMode;

  /**
   * Gets or sets the fog color to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   * (Default is Color3(0.2, 0.2, 0.3))
   */
  Color3 fogColor;

  /**
   * Gets or sets the fog density to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   * (Default is 0.1)
   */
  float fogDensity;

  /**
   * Gets or sets the fog start distance to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   * (Default is 0)
   */
  float fogStart;

  /**
   * Gets or sets the fog end distance to use
   * @see http://doc.babylonjs.com/babylon101/environment#fog
   * (Default is 1000)
   */
  float fogEnd;

  // Lights

  /**
   * Gets a boolean indicating if shadows are enabled on this scene
   */
  Property<Scene, bool> shadowsEnabled;

  /**
   * Gets a boolean indicating if lights are enabled on this scene
   */
  Property<Scene, bool> lightsEnabled;

  // Cameras

  /**
   * All of the active cameras added to this scene
   */
  std::vector<CameraPtr> activeCameras;

  /**
   * @brief Hidden
   */
  CameraPtr _activeCamera;

  /**
   * The current active camera
   */
  Property<Scene, CameraPtr> activeCamera;

  // Materials

  /**
   * Gets the default material used on meshes when no material is
   * affected
   */
  Property<Scene, MaterialPtr> defaultMaterial;

  // Textures

  /**
   * Gets a boolean indicating if textures are enabled on this scene
   */
  Property<Scene, bool> texturesEnabled;

  // Particles

  /**
   * Gets or sets a boolean indicating if particles are enabled on this scene
   */
  bool particlesEnabled;

  // Sprites

  /**
   * Gets or sets a boolean indicating if sprites are enabled on this scene
   */
  bool spritesEnabled;

  std::function<bool(Sprite* sprite)> spritePredicate;

  /**
   * Hidden
   */
  SpritePtr _pointerOverSprite;

  /**
   * Hidden
   */
  SpritePtr _pickedDownSprite;

  /**
   * Hidden
   */
  std::optional<Ray> _tempSpritePickingRay;

  /**
   * All of the sprite managers added to this scene
   * @see http://doc.babylonjs.com/babylon101/sprites
   */
  std::vector<ISpriteManagerPtr> spriteManagers;

  std::vector<std::unique_ptr<HighlightLayer>> highlightLayers;

  // Skeletons

  /**
   * Gets a boolean indicating if skeletons are enabled on this scene
   */
  Property<Scene, bool> skeletonsEnabled;

  // Lens flares

  /**
   * Gets or sets a boolean indicating if lens flares are enabled on this scene
   */
  bool lensFlaresEnabled;

  // Collisions

  /**
   * Gets or sets a boolean indicating if collisions are enabled on this scene
   * @see
   * http://doc.babylonjs.com/babylon101/cameras,_mesh_collisions_and_gravity
   */
  bool collisionsEnabled;

  /** Hidden */
  ReadOnlyProperty<Scene, std::unique_ptr<ICollisionCoordinator>> collisionCoordinator;

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
  std::vector<PostProcessPtr> postProcesses;

  /**
   * Gets or sets a boolean indicating if postprocesses are enabled on this
   * scene
   */
  bool postProcessesEnabled;

  /**
   * Gets the current postprocess manager
   */
  std::unique_ptr<PostProcessManager> postProcessManager;

  /**
   * Gets the postprocess render pipeline manager
   * @see http://doc.babylonjs.com/how_to/how_to_use_postprocessrenderpipeline
   * @see http://doc.babylonjs.com/how_to/using_default_rendering_pipeline
   */
  ReadOnlyProperty<Scene, std::unique_ptr<PostProcessRenderPipelineManager>>
    postProcessRenderPipelineManager;

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
  std::vector<RenderTargetTexturePtr> customRenderTargets;

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
  std::vector<std::string> importedMeshesFiles;

  // Probes

  /**
   * Gets or sets a boolean indicating if probes are enabled on this scene
   */
  bool probesEnabled;

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
  AbstractActionManagerPtr actionManager;

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
  std::vector<ProceduralTexturePtr> proceduralTextures;

  // Sound Tracks

  /**
   * The list of sound tracks added to the scene
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  std::vector<SoundTrack*> soundTracks;

  /**
   * The main sound track played by the scene.
   * It cotains your primary collection of sounds.
   */
  ReadOnlyProperty<Scene, SoundTrackPtr> mainSoundTrack;

  // Simplification Queue

  /**
   * Gets or sets the simplification queue attached to the scene
   * @see http://doc.babylonjs.com/how_to/in-browser_mesh_simplification
   */
  Property<Scene, SimplificationQueuePtr> simplificationQueue;

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
  EffectPtr _cachedEffect;
  /** Hidden */
  std::optional<float> _cachedVisibility;
  /** Hidden */
  std::vector<IDisposable*> _toBeDisposed;

  /**
   * Gets or sets a boolean indicating that all submeshes of active meshes must
   * be rendered Use this boolean to avoid computing frustum clipping on
   * submeshes (This could help when you are CPU bound)
   */
  bool dispatchAllSubMeshesOfActiveMeshes;

  /** Hidden */
  std::vector<IParticleSystem*> _activeParticleSystems;

  /** Hidden */
  std::vector<AnimatablePtr> _activeAnimatables;

  /** Hidden */
  std::unique_ptr<Vector3> _forcedViewPosition;

  /**
   * Hidden
   */
  ReadOnlyProperty<Scene, bool> _isAlternateRenderingEnabled;

  /**
   * Gets the list of frustum planes (built from the active camera)
   */
  ReadOnlyProperty<Scene, std::array<Plane, 6>> frustumPlanes;

  /**
   * Gets or sets a boolean indicating if lights must be sorted by priority (off
   * by default) This is useful if there are more lights that the maximum
   * simulteanous authorized
   */
  bool requireLightSorting;

  /**
   * Hidden
   */
  const bool useMaterialMeshMap;

  /**
   * Hidden
   */
  const bool useClonedMeshhMap;

  /**
   * Backing store of defined scene components
   * Hidden
   */
  std::vector<ISceneComponentPtr> _components;

  /**
   * Backing store of defined scene components
   * Hidden
   */
  std::vector<ISceneSerializableComponentPtr> _serializableComponents;

  /**
   * Gets the depth renderer object.
   */
  ReadOnlyProperty<Scene, std::unordered_map<std::string, DepthRendererPtr>> depthRenderer;

  /**
   * Gets the current geometry buffer associated to the scene
   */
  Property<Scene, GeometryBufferRendererPtr> geometryBufferRenderer;

  /**
   * Gets the debug layer (aka Inspector) associated with the scene
   * @see http://doc.babylonjs.com/features/playground_debuglayer
   */
  ReadOnlyProperty<Scene, std::unique_ptr<DebugLayer>> debugLayer;

  /**
   * Gets the octree used to boost mesh selection (picking)
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene_with_octrees
   */
  ReadOnlyProperty<Scene, Octree<AbstractMesh*>*> selectionOctree;

  /**
   * Gets the mesh that is currently under the pointer
   */
  ReadOnlyProperty<Scene, AbstractMeshPtr> meshUnderPointer;

  /**
   * Gets the current on-screen X position of the pointer
   */
  Property<Scene, int> pointerX;

  /**
   * Gets the current on-screen Y position of the pointer
   */
  Property<Scene, int> pointerY;

  /**
   * Gets the performance counter for total vertices
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  ReadOnlyProperty<Scene, PerfCounter> totalVerticesPerfCounter;

  /**
   * Gets the performance counter for active indices
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  ReadOnlyProperty<Scene, PerfCounter> totalActiveIndicesPerfCounter;

  /**
   * Gets the performance counter for active particles
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  ReadOnlyProperty<Scene, PerfCounter> activeParticlesPerfCounter;

  /**
   * Gets the performance counter for active bones
   * @see http://doc.babylonjs.com/how_to/optimizing_your_scene#instrumentation
   */
  ReadOnlyProperty<Scene, PerfCounter> activeBonesPerfCounter;

  /**
   * Returns a boolean indicating if the scene is still loading data
   */
  ReadOnlyProperty<Scene, bool> isLoading;

  /**
   * Return a unique id as a string which can serve as an identifier for
   * the scene
   */
  ReadOnlyProperty<Scene, std::string> uid;

  /** Audio **/

  /**
   * Gets if audio support is enabled
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  Property<Scene, std::optional<bool>> audioEnabled;

  /**
   * Gets if audio will be output to headphones
   * @see http://doc.babylonjs.com/how_to/playing_sounds_and_music
   */
  Property<Scene, std::optional<bool>> headphone;

  /**
   * Gets if the scene is already disposed
   */
  ReadOnlyProperty<Scene, bool> isDisposed;

  /**
   * Hidden
   */
  bool _allowPostProcessClearColor;

  /**
   * Defines the actions happening before camera updates
   * Hidden
   */
  Stage<SimpleStageAction> _beforeCameraUpdateStage;

  /**
   * Defines the actions happening before clear the canvas
   * Hidden
   */
  Stage<SimpleStageAction> _beforeClearStage;

  /**
   * Defines the actions happening before camera updates.
   * Hidden
   */
  Stage<RenderTargetsStageAction> _gatherRenderTargetsStage;

  /**
   * Defines the actions happening for one camera in the frame.
   * Hidden
   */
  Stage<RenderTargetsStageAction> _gatherActiveCameraRenderTargetsStage;

  /**
   * Defines the actions happening during the per mesh ready checks
   * Hidden
   */
  Stage<MeshStageAction> _isReadyForMeshStage;

  /**
   * Defines the actions happening before evaluate active mesh checks
   * Hidden
   */
  Stage<SimpleStageAction> _beforeEvaluateActiveMeshStage;

  /**
   * Defines the actions happening during the evaluate sub mesh checks
   * Hidden
   */
  Stage<EvaluateSubMeshStageAction> _evaluateSubMeshStage;

  /**
   * Defines the actions happening during the active mesh stage
   * Hidden
   */
  Stage<ActiveMeshStageAction> _activeMeshStage;

  /**
   * Defines the actions happening during the per camera render target step
   * Hidden
   */
  Stage<CameraStageAction> _cameraDrawRenderTargetStage;

  /**
   * Defines the actions happening just before the active camera is drawing
   * Hidden
   */
  Stage<CameraStageAction> _beforeCameraDrawStage;

  /**
   * Defines the actions happening just before a render target is drawing.
   * Hidden
   */
  Stage<RenderTargetStageAction> _beforeRenderTargetDrawStage;

  /**
   * Defines the actions happening just before a rendering group is drawing
   * Hidden
   */
  Stage<RenderingGroupStageAction> _beforeRenderingGroupDrawStage;

  /**
   * Defines the actions happening just before a mesh is drawing.
   * Hidden
   */
  Stage<RenderingMeshStageAction> _beforeRenderingMeshStage;

  /**
   * Defines the actions happening just after a mesh has been drawn.
   * Hidden
   */
  Stage<RenderingMeshStageAction> _afterRenderingMeshStage;

  /**
   * Defines the actions happening just after a rendering group has been drawn
   * Hidden
   */
  Stage<RenderingGroupStageAction> _afterRenderingGroupDrawStage;

  /**
   * Defines the actions happening just after the active camera has been drawn
   * Hidden
   */
  Stage<CameraStageAction> _afterCameraDrawStage;

  /**
   * Defines the actions happening just after a render target has been drawn.
   * Hidden
   */
  Stage<RenderTargetStageAction> _afterRenderTargetDrawStage;

  /**
   * Defines the actions happening just after rendering all cameras and
   * computing intersections. Hidden
   */
  Stage<SimpleStageAction> _afterRenderStage;

  /**
   * Defines the actions happening when a pointer move event happens.
   * Hidden
   */
  Stage<PointerMoveStageAction> _pointerMoveStage;

  /**
   * Defines the actions happening when a pointer down event happens.
   * Hidden
   */
  Stage<PointerUpDownStageAction> _pointerDownStage;

  /**
   * Defines the actions happening when a pointer up event happens.
   * Hidden
   */
  Stage<PointerUpDownStageAction> _pointerUpStage;

  /**
   * Defines the actions happening when Geometries are rebuilding.
   * Hidden
   */
  Stage<SimpleStageAction> _rebuildGeometryStage;

  /**
   * Gets or sets a boolean blocking all the calls to markAllMaterialsAsDirty
   * (ie. the materials won't be updated if they are out of sync)
   */
  Property<Scene, bool> blockMaterialDirtyMechanism;

  /**
   * Lambda returning the list of potentially active meshes.
   */
  std::function<std::vector<AbstractMesh*>()> getActiveMeshCandidates;

  /**
   * Lambda returning the list of potentially active sub meshes.
   */
  std::function<std::vector<SubMesh*>(AbstractMesh* mesh)> getActiveSubMeshCandidates;

  /**
   * Lambda returning the list of potentially intersecting sub meshes.
   */
  std::function<std::vector<SubMesh*>(AbstractMesh* mesh, const Ray& localRay)>
    getIntersectingSubMeshCandidates;

  /**
   * Lambda returning the list of potentially colliding sub meshes.
   */
  std::function<std::vector<SubMesh*>(AbstractMesh* mesh, const Collider& collider)>
    getCollidingSubMeshCandidates;

  /**
   * Hidden (Backing field)
   */
  IPhysicsEnginePtr _physicsEngine;

  /**
   * Hidden
   */
  float _physicsTimeAccumulator;

  /**
   * User updatable function that will return a deterministic frame time when
   * engine is in deterministic lock step mode
   */
  std::function<float()> getDeterministicFrameTime;

  /**
   * Hidden
   */
  Matrix _transformMatrixR;

protected:
  /** Hidden */
  BaseTexturePtr _environmentTexture;
  /** Hidden */
  float _environmentIntensity;
  /** Hidden */
  ImageProcessingConfigurationPtr _imageProcessingConfiguration;

private:
  // Animations
  AnimationPropertiesOverridePtr _animationPropertiesOverride;
  // Events
  std::function<bool(Sprite* sprite)> _spritePredicate;
  Observer<Scene>::Ptr _onDisposeObserver;
  Observer<Scene>::Ptr _onBeforeRenderObserver;
  Observer<Scene>::Ptr _onAfterRenderObserver;
  Observer<Camera>::Ptr _onBeforeCameraRenderObserver;
  Observer<Camera>::Ptr _onAfterCameraRenderObserver;
  // Animations
  std::vector<std::string> _registeredForLateAnimationBindings;
  // Pointers
  std::function<void(PointerEvent&& evt)> _onPointerMove;
  std::function<void(PointerEvent&& evt)> _onPointerDown;
  std::function<void(PointerEvent&& evt)> _onPointerUp;
  // Gamepads
  std::unique_ptr<GamepadManager> _gamepadManager;
  // Click events
  std::function<void(
    Observable<PointerInfoPre>& obs1, Observable<PointerInfo>& obs2, const PointerEvent& evt,
    const std::function<void(const ClickInfo& clickInfo, std::optional<PickingInfo>& pickResult)>&
      cb)>
    _initClickEvent;
  std::function<AbstractActionManagerPtr(const AbstractActionManagerPtr& act,
                                         const ClickInfo& clickInfo)>
    _initActionManager;
  std::function<void(
    unsigned int btn, const ClickInfo& clickInfo,
    const std::function<void(const ClickInfo& clickInfo, const PointerInfo& pickResult)>& cb)>
    _delayedSimpleClick;
  milliseconds_t _delayedSimpleClickTimeout;
  milliseconds_t _previousDelayedSimpleClickTimeout;
  bool _meshPickProceed;
  MouseButtonType _previousButtonPressed;
  bool _previousHasSwiped;
  std::optional<PickingInfo> _currentPickResult;
  std::optional<PickingInfo> _previousPickResult;
  int _totalPointersPressed;
  bool _doubleClickOccured;
  int _pointerX;
  int _pointerY;
  int _unTranslatedPointerX;
  int _unTranslatedPointerY;
  Vector2 _startingPointerPosition;
  Vector2 _previousStartingPointerPosition;
  high_res_time_point_t _startingPointerTime;
  high_res_time_point_t _previousStartingPointerTime;
  std::unordered_map<int, bool> _pointerCaptures;
  // AbstractMesh* _meshUnderPointer;
  // Deterministic lockstep
  float _timeAccumulator;
  unsigned int _currentStepId;
  unsigned int _currentInternalStep;
  // Keyboard
  std::function<void(KeyboardEvent&& evt)> _onKeyDown;
  std::function<void(KeyboardEvent&& evt)> _onKeyUp;
  Observer<Engine>::Ptr _onCanvasFocusObserver;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  // Coordinates system
  bool _useRightHandedSystem;
  // Members
  bool _forceWireframe;
  bool _skipFrustumClipping;
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

  // Materials
  MaterialPtr _defaultMaterial;
  // Textures
  bool _texturesEnabled;
  // Skeletons
  bool _skeletonsEnabled;
  // Postprocesses
  std::unique_ptr<PostProcessRenderPipelineManager> _postProcessRenderPipelineManager;
  // Collisions
  /** Hidden */
  std::unique_ptr<ICollisionCoordinator> _collisionCoordinator;
  // Actions
  std::vector<AbstractMesh*> _meshesForIntersections;
  // Sound Tracks
  bool _hasAudioEngine;
  SoundTrackPtr _mainSoundTrack;
  // Render engine
  Engine* _engine;
  // Performance counters
  PerfCounter _totalVertices;
  float _animationRatio;
  std::optional<high_res_time_point_t> _animationTimeLast;
  int _animationTime;
  int _renderId;
  int _frameId;
  int _executeWhenReadyTimeoutId;
  bool _intermediateRendering;
  int _viewUpdateFlag;
  int _projectionUpdateFlag;
  int _alternateViewUpdateFlag;
  int _alternateProjectionUpdateFlag;
  std::vector<IFileRequest> _activeRequests;
  std::vector<std::string> _pendingData;
  bool _isDisposed;
  std::vector<AbstractMesh*> _activeMeshes;
  IActiveMeshCandidateProvider* _activeMeshCandidateProvider;
  bool _activeMeshesFrozen;
  bool _skipEvaluateActiveMeshesCompletely;
  std::vector<MaterialPtr> _processedMaterials;
  std::vector<RenderTargetTexturePtr> _renderTargets;
  std::vector<SkeletonPtr> _activeSkeletons;
  std::vector<Mesh*> _softwareSkinnedMeshes;
  std::unique_ptr<RenderingManager> _renderingManager;
  Matrix _transformMatrix;
  std::unique_ptr<UniformBuffer> _sceneUbo;
  std::unique_ptr<UniformBuffer> _alternateSceneUbo;
  std::unique_ptr<Matrix> _pickWithRayInverseMatrix;

  /**
   * An optional map from Geometry Id to Geometry index in the 'geometries'
   * array
   */
  std::unordered_map<std::string, size_t> geometriesById;

  /** Hidden (Backing field) */
  SimplificationQueuePtr _simplificationQueue;

  /** Hidden (Backing field) */
  BoundingBoxRendererPtr _boundingBoxRenderer;

  /** Hidden (Backing field) */
  bool _forceShowBoundingBoxes;

  /** Hidden */
  OutlineRendererPtr _outlineRenderer;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _alternateViewMatrix;
  Matrix _alternateProjectionMatrix;
  std::unique_ptr<Matrix> _alternateTransformMatrix;
  bool _useAlternateCameraConfiguration;
  bool _alternateRendering;
  bool _frustumPlanesSet;
  std::array<Plane, 6> _frustumPlanes;

  /** Hidden (Backing field) */
  Octree<AbstractMesh*>* _selectionOctree;

  Vector2 _unTranslatedPointer;
  AbstractMeshPtr _pointerOverMesh;
  std::unique_ptr<DebugLayer> _debugLayer;
  std::unordered_map<std::string, DepthRendererPtr> _depthRenderer;
  GeometryBufferRendererPtr _geometryBufferRenderer;
  AbstractMesh* _pickedDownMesh;
  AbstractMesh* _pickedUpMesh;
  std::string _uid;
  bool _blockMaterialDirtyMechanism;

  /**
   * List of components to register on the next registration step
   */
  std::vector<ISceneComponentPtr> _transientComponents;

  std::unique_ptr<Ray> _tempPickingRay;
  std::unique_ptr<Ray> _cachedRayForTransform;

  std::vector<AbstractMesh*> _defaultMeshCandidates;
  std::vector<SubMesh*> _defaultSubMeshCandidates;

  std::optional<bool> _audioEnabled;
  std::optional<bool> _headphone;

  /** Hidden */
  std::unique_ptr<UniformBuffer> _multiviewSceneUbo;

}; // end of class Scene

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_SCENE_H
