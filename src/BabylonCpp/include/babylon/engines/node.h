#ifndef BABYLON_ENGINES_NODE_H
#define BABYLON_ENGINES_NODE_H

#include <functional>
#include <nlohmann/json.hpp>

#include <babylon/animations/animation_range.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_api.h>
#include <babylon/behaviors/ibehavior_aware.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class Animatable;
struct AnimationPropertiesOverride;
class Engine;
class Node;
class Scene;
class TransformNode;
using AbstractMeshPtr  = std::shared_ptr<AbstractMesh>;
using AnimatablePtr    = std::shared_ptr<Animatable>;
using NodePtr          = std::shared_ptr<Node>;
using TransformNodePtr = std::shared_ptr<TransformNode>;

/**
 * Defines how a node can be built from a string name.
 */
using NodeConstructor = std::function<NodePtr(
  const std::string& name, Scene* scene, const std::optional<json>& options)>;

/**
 * @brief Node is the basic class for all scene objects (Mesh, Light, Camera.)
 */
class BABYLON_SHARED_EXPORT Node : public std::enable_shared_from_this<Node>,
                                   public IAnimatable,
                                   public IBehaviorAware<Node>,
                                   public IDisposable {

  friend class UtilityLayerRenderer;

private:
  static std::unordered_map<std::string, NodeConstructor> _NodeConstructors;

public:
  /**
   * @brief Add a new node constructor.
   * @param type defines the type name of the node to construct
   * @param constructorFunc defines the constructor function
   */
  static void AddNodeConstructor(const std::string& type,
                                 const NodeConstructor& constructorFunc);

  /**
   * @brief Returns a node constructor based on type name.
   * @param type defines the type name
   * @param name defines the new node name
   * @param scene defines the hosting scene
   * @param options defines optional options to transmit to constructors
   * @returns the new constructor or null
   */
  static std::function<NodePtr()>
  Construct(const std::string& type, const std::string& name, Scene* scene,
            const std::optional<json>& options = std::nullopt);

public:
  /**
   * @brief Creates a new Node
   * @param name the name and id to be given to this node
   * @param scene the scene this node will be added to
   */
  Node(const std::string& name, Scene* scene = nullptr);
  virtual ~Node() override;

  template <typename Derived>
  std::shared_ptr<Derived> shared_from_base()
  {
    return std::static_pointer_cast<Derived>(shared_from_this());
  }

  virtual Type type() const override;

  /**
   * @brief Gets a boolean indicating if the node has been disposed.
   * @returns true if the node was disposed
   */
  bool isDisposed() const;

  /**
   * @brief Gets a string idenfifying the name of the class.
   * @returns "Node" string
   */
  virtual const std::string getClassName() const;

  /**
   * @brief Gets the engine of the node
   * @returns a Engine
   */
  virtual Scene* getScene() const;

  /**
   * @brief Gets the engine of the node.
   * @returns a {BABYLON.Engine}
   */
  Engine* getEngine();

  /**
   * @brief Gets the shared pointer to this object.
   * @return the shared pointer to this object
   */
  NodePtr getAsNodePtr();

  // Behaviors
  /**
   * @brief Attach a behavior to the node.
   * @see http://doc.babylonjs.com/features/behaviour
   * @param behavior defines the behavior to attach
   * @param attachImmediately defines that the behavior must be attached even if
   * the scene is still loading
   * @returns the current Node
   */
  Node& addBehavior(Behavior<Node>* behavior,
                    bool attachImmediately = false) override;

  /**
   * @brief Remove an attached behavior.
   * @see http://doc.babylonjs.com/features/behaviour
   * @param behavior defines the behavior to attach
   * @returns the current Node
   */
  Node& removeBehavior(Behavior<Node>* behavior) override;

  /**
   * @brief Gets an attached behavior by name.
   * @param name defines the name of the behavior to look for
   * @see http://doc.babylonjs.com/features/behaviour
   * @returns null if behavior was not found else the requested behavior
   */
  Behavior<Node>* getBehaviorByName(const std::string& name) override;

  /**
   * @brief Returns the latest update of the World matrix
   * @returns a Matrix
   */
  virtual Matrix& getWorldMatrix() override;

  /**
   * @brief Hidden
   */
  virtual float _getWorldMatrixDeterminant() const;

  /**
   * @brief Returns directly the latest state of the mesh World matrix.
   * A Matrix is returned.
   */
  Matrix& worldMatrixFromCache();

  /**
   * @brief Hidden
   */
  virtual void _initCache();

  /**
   * @brief Hidden
   */
  virtual void updateCache(bool force = false);

  /**
   * @brief Hidden
   */
  virtual void _updateCache(bool ignoreParentClass = false);

  /**
   * @brief Hidden
   */
  virtual bool _isSynchronized();

  /**
   * @brief Hidden
   */
  void _markSyncedWithParent();

  /**
   * @brief Hidden
   */
  bool isSynchronizedWithParent() const;

  /**
   * @brief Hidden
   */
  bool isSynchronized();

  /**
   * @brief Is this node ready to be used/rendered.
   * @param completeCheck defines if a complete check (including materials and
   * lights) has to be done (false by default)
   * @return true if the node is ready
   */
  virtual bool isReady(bool completeCheck        = false,
                       bool forceInstanceSupport = false);

  /**
   * @brief Is this node enabled?
   * If the node has a parent, all ancestors will be checked and false will be
   * returned if any are false (not enabled), otherwise will return true
   * @param checkAncestors indicates if this method should check the ancestors.
   * The default is to check the ancestors. If set to false, the method will
   * return the value of this node without checking ancestors
   * @return whether this node (and its parent) is enabled
   */
  bool isEnabled(bool checkAncestors = true);

  /**
   * @brief Set the enabled state of this node.
   * @param value defines the new enabled state
   */
  void setEnabled(bool value);

  /**
   * @brief Is this node a descendant of the given node?
   * The function will iterate up the hierarchy until the ancestor was found or
   * no more parents defined
   * @param ancestor defines the parent node to inspect
   * @returns a boolean indicating if this node is a descendant of the given
   * node
   */
  bool isDescendantOf(const Node* ancestor);

  /**
   * @brief Evaluate the list of children and determine if they should be
   * considered as descendants considering the given criterias.
   * @param {BABYLON.Node[]} results the result array containing the nodes
   * matching the given criterias
   * @param {boolean} directDescendantsOnly if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered.
   * @param predicate: an optional predicate that will be called on every
   * evaluated children, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored.
   */
  template <typename T>
  void _getDescendants(std::vector<std::shared_ptr<T>>& results,
                       bool directDescendantsOnly = false,
                       const std::function<bool(const NodePtr& node)>& predicate
                       = nullptr);

  /**
   * @brief Will return all nodes that have this node as ascendant.
   * @param directDescendantsOnly defines if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @return all children nodes of all types
   */
  std::vector<NodePtr>
  getDescendants(bool directDescendantsOnly = false,
                 const std::function<bool(const NodePtr& node)>& predicate
                 = nullptr);

  /**
   * @brief Get all child-meshes of this node.
   * @param directDescendantsOnly defines if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of AbstractMesh
   */
  virtual std::vector<AbstractMeshPtr>
  getChildMeshes(bool directDescendantsOnly = false,
                 const std::function<bool(const NodePtr& node)>& predicate
                 = nullptr);

  /**
   * @brief Get all child-transformNodes of this node.
   * @param directDescendantsOnly defines if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of TransformNode
   */
  virtual std::vector<TransformNodePtr> getChildTransformNodes(
    bool directDescendantsOnly                                = false,
    const std::function<bool(const NodePtr& node)>& predicate = nullptr);

  /**
   * @brief Get all direct children of this node.
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of Node
   */
  std::vector<NodePtr>
  getChildren(const std::function<bool(const NodePtr& node)>& predicate
              = nullptr);

  /**
   * @brief Hidden
   */
  void _setReady(bool state);

  /**
   * @brief Hidden
   */
  virtual std::vector<AnimationPtr> getAnimations() override;

  /**
   * @brief Get an animation by name.
   * @param name defines the name of the animation to look for
   * @returns null if not found else the requested animation
   */
  AnimationPtr getAnimationByName(const std::string& name);

  /**
   * @brief Creates an animation range for this node.
   * @param name defines the name of the range
   * @param from defines the starting key
   * @param to defines the end key
   */
  void createAnimationRange(const std::string& name, float from, float to);

  /**
   * @brief Delete a specific animation range.
   * @param name defines the name of the range to delete
   * @param deleteFrames defines if animation frames from the range must be
   * deleted as well
   */
  void deleteAnimationRange(const std::string& name, bool deleteFrames = true);

  /**
   * @brief Get an animation range by name.
   * @param name defines the name of the animation range to look for
   * @returns null if not found else the requested animation range
   */
  AnimationRange* getAnimationRange(const std::string& name);

  /**
   * @brief Will start the animation sequence.
   * @param name defines the range frames for animation sequence
   * @param loop defines if the animation should loop (false by default)
   * @param speedRatio defines the speed factor in which to run the animation (1
   * by default)
   * @param onAnimationEnd defines a function to be executed when the animation
   * ended (undefined by default)
   * @returns the object created for this animation. If range does not exist, it
   * will return null
   */
  AnimatablePtr beginAnimation(const std::string& name, bool loop = false,
                               float speedRatio                     = 1.f,
                               std::function<void()> onAnimationEnd = nullptr);

  /**
   * @brief Serialize animation ranges into a JSON compatible object.
   * @returns serialization object
   */
  std::vector<AnimationRange> serializeAnimationRanges();

  /**
   * @brief Computes the world matrix of the node.
   * @param force defines if the cache version should be invalidated forcing the
   * world matrix to be created from scratch
   * @returns the world matrix
   */
  virtual Matrix& computeWorldMatrix(bool force             = false,
                                     bool useWasUpdatedFlag = false);

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Parse animation range data from a serialization object and store
   * them into a given node.
   * @param node defines where to store the animation ranges
   * @param parsedNode defines the serialization object to read data from
   * @param scene defines the hosting scene
   */
  static void ParseAnimationRanges(Node& node, const json& parsedNode,
                                   Scene* scene);

protected:
  /**
   * @brief Sets the parent of the node.
   */
  void set_parent(Node* const& parent) override;

  /**
   * @brief Gets the parent of the node.
   */
  Node*& get_parent() override;

  /**
   * @brief Gets the animation properties override.
   */
  AnimationPropertiesOverride*& get_animationPropertiesOverride();

  /**
   * @brief Sets the animation properties override.
   */
  void
  set_animationPropertiesOverride(AnimationPropertiesOverride* const& value);

  /**
   * @brief Sets a callback that will be raised when the node will be disposed.
   */
  void set_onDispose(
    const std::function<void(Node* node, EventState& es)>& callback);

  /**
   * @brief Gets the list of attached behaviors.
   * @see http://doc.babylonjs.com/features/behaviour
   */
  std::vector<Behavior<Node>*>& get_behaviors();

  /**
   * @brief Hidden
   */
  virtual void _syncParentEnabledState();

private:
  void addToSceneRootNodes();
  void removeFromSceneRootNodes();

public:
  /**
   * Gets or sets the name of the node
   */
  std::string name;

  /**
   * Gets or sets the id of the node
   */
  std::string id;

  /**
   * Gets or sets the unique id of the node
   */
  size_t uniqueId;

  /**
   * Gets or sets an object used to store user defined information for the node
   */
  json metadata;

  /**
   * Gets or sets a boolean used to define if the node must be serialized
   */
  bool doNotSerialize;

  bool _isDisposed;

  /**
   * Gets a list of Animations associated with the node.
   */
  std::vector<AnimationPtr> animations;

  /**
   * Callback raised when the node is ready to be used
   */
  std::function<void(Node* node)> onReady;

  int _currentRenderId;
  std::string parentId;
  std::string _waitingParentId;

  /** Hidden */
  Scene* _scene;

  /** Hidden */
  NodeCache _cache;

  /** Hidden */
  Matrix _worldMatrix;
  /** Hidden */
  float _worldMatrixDeterminant;

  /**
   * Animation properties override.
   */
  Property<Node, AnimationPropertiesOverride*> animationPropertiesOverride;

  /**
   * An event triggered when the mesh is disposed.
   */
  Observable<Node> onDisposeObservable;

  /**
   * Callback that will be raised when the node will be disposed.
   */
  WriteOnlyProperty<Node, std::function<void(Node* node, EventState& es)>>
    onDispose;

  /**
   * List of attached behaviors
   */
  ReadOnlyProperty<Node, std::vector<Behavior<Node>*>> behaviors;

protected:
  std::unordered_map<std::string, std::unique_ptr<AnimationRange>> _ranges;
  int _childRenderId;

private:
  bool _isEnabled;
  bool _isParentEnabled;
  bool _isReady;
  int _parentRenderId;
  Node* _parentNode;
  std::vector<NodePtr> _children;
  int _sceneRootNodesIndex;
  AnimationPropertiesOverride* _animationPropertiesOverride;
  Observer<Node>::Ptr _onDisposeObserver;

  // Behaviors
  std::vector<Behavior<Node>*> _behaviors;

}; // end of class Node

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_NODE_H
