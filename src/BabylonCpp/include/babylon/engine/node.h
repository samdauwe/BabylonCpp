#ifndef BABYLON_ENGINE_NODE_H
#define BABYLON_ENGINE_NODE_H

#include <babylon/animations/animation_range.h>
#include <babylon/animations/ianimatable.h>
#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

/**
 * @brief Node is the basic class for all scene objects (Mesh, Light, Camera,
 * etc.).
 */
class BABYLON_SHARED_EXPORT Node : public IAnimatable, public IDisposable {

public:
  /**
   * @brief Creates a new Node.
   * @param {string} name - the name and id to be given to this node
   * @param {BABYLON.Scene} the scene this node will be added to
   */
  Node(const string_t& name, Scene* scene = nullptr);
  virtual ~Node() override;

  virtual IReflect::Type type() const override;

  /**
   * @brief Gets a boolean indicating if the node has been disposed.
   * @returns true if the node was disposed
   */
  bool isDisposed() const;

  /**
   * @brief Gets a string idenfifying the name of the class.
   * @returns "Node" string
   */
  virtual const string_t getClassName() const;

  /**
   * @brief Gets the scene of the node.
   * @returns a {BABYLON.Scene}
   */
  virtual Scene* getScene();

  /**
   * @brief Gets the engine of the node.
   * @returns a {BABYLON.Engine}
   */
  Engine* getEngine();

  // Behaviors
  /**
   * @brief Attach a behavior to the node.
   * @see http://doc.babylonjs.com/features/behaviour
   * @param behavior defines the behavior to attach
   * @returns the current Node
   */
  Node& addBehavior(Behavior<Node>* behavior);

  /**
   * @brief Remove an attached behavior.
   * @see http://doc.babylonjs.com/features/behaviour
   * @param behavior defines the behavior to attach
   * @returns the current Node
   */
  Node& removeBehavior(Behavior<Node>* behavior);

  /**
   * @brief Gets an attached behavior by name.
   * @param name defines the name of the behavior to look for
   * @see http://doc.babylonjs.com/features/behaviour
   * @returns null if behavior was not found else the requested behavior
   */
  Behavior<Node>* getBehaviorByName(const string_t& name);

  /**
   * @brief Returns the world matrix of the node.
   * @returns a matrix containing the node's world matrix
   */
  virtual Matrix* getWorldMatrix() override;

  /**
   * @brief Hidden
   */
  virtual float _getWorldMatrixDeterminant() const;

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
  bool isSynchronized(bool updateCache = false);

  /**
   * @brief Hidden
   */
  bool hasNewParent(bool update = false);

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
  void
  _getDescendants(vector_t<T*>& results, bool directDescendantsOnly = false,
                  const ::std::function<bool(Node* node)>& predicate = nullptr);

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
  vector_t<Node*>
  getDescendants(bool directDescendantsOnly                         = false,
                 const ::std::function<bool(Node* node)>& predicate = nullptr);

  /**
   * @brief Get all child-meshes of this node.
   * @param directDescendantsOnly defines if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of {BABYLON.AbstractMesh}
   */
  virtual vector_t<AbstractMesh*>
  getChildMeshes(bool directDescendantsOnly                         = false,
                 const ::std::function<bool(Node* node)>& predicate = nullptr);

  /**
   * @brief Get all child-transformNodes of this node.
   * @param directDescendantsOnly defines if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of {BABYLON.TransformNode}
   */
  virtual vector_t<TransformNode*>
  getChildTransformNodes(bool directDescendantsOnly = false,
                         const ::std::function<bool(Node* node)>& predicate
                         = nullptr);

  /**
   * @brief Get all direct children of this node.
   * @param predicate defines an optional predicate that will be called on every
   * evaluated child, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored
   * @returns an array of {BABYLON.Node}
   */
  vector_t<Node*> getChildren(const ::std::function<bool(Node* node)>& predicate
                              = nullptr);

  /**
   * @brief Hidden
   */
  void _setReady(bool state);

  /**
   * @brief Hidden
   */
  virtual vector_t<Animation*> getAnimations() override;

  /**
   * @brief Get an animation by name.
   * @param name defines the name of the animation to look for
   * @returns null if not found else the requested animation
   */
  Animation* getAnimationByName(const string_t& name);

  /**
   * @brief Creates an animation range for this node.
   * @param name defines the name of the range
   * @param from defines the starting key
   * @param to defines the end key
   */
  void createAnimationRange(const string_t& name, float from, float to);

  /**
   * @brief Delete a specific animation range.
   * @param name defines the name of the range to delete
   * @param deleteFrames defines if animation frames from the range must be
   * deleted as well
   */
  void deleteAnimationRange(const string_t& name, bool deleteFrames = true);

  /**
   * @brief Get an animation range by name.
   * @param name defines the name of the animation range to look for
   * @returns null if not found else the requested animation range
   */
  AnimationRange* getAnimationRange(const string_t& name);

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
  Animatable* beginAnimation(const string_t& name, bool loop = false,
                             float speedRatio                       = 1.f,
                             ::std::function<void()> onAnimationEnd = nullptr);

  /**
   * @brief Serialize animation ranges into a JSON compatible object.
   * @returns serialization object
   */
  vector_t<AnimationRange> serializeAnimationRanges();

  /**
   * @brief Computes the world matrix of the node.
   * @param force defines if the cache version should be invalidated forcing the
   * world matrix to be created from scratch
   * @returns the world matrix
   */
  virtual Matrix& computeWorldMatrix(bool force = false);

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
  static void ParseAnimationRanges(Node* node, const Json::value& parsedNode,
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
    const ::std::function<void(Node* node, EventState& es)>& callback);

  /**
   * @brief Gets the list of attached behaviors.
   * @see http://doc.babylonjs.com/features/behaviour
   */
  vector_t<Behavior<Node>*>& get_behaviors();

public:
  /**
   * Gets or sets the name of the node
   */
  string_t name;

  /**
   * Gets or sets the id of the node
   */
  string_t id;

  /**
   * Gets or sets the unique id of the node
   */
  size_t uniqueId;

  /**
   * Gets or sets an object used to store user defined information for the node
   */
  Json::object metadata;

  /**
   * Gets or sets a boolean used to define if the node must be serialized
   */
  bool doNotSerialize;

  bool _isDisposed;

  /**
   * Gets a list of Animations associated with the node.
   */
  vector_t<Animation*> animations;

  /**
   * Callback raised when the node is ready to be used
   */
  ::std::function<void(Node* node)> onReady;

  int _currentRenderId;
  string_t parentId;
  string_t _waitingParentId;
  NodeCache _cache;

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
  WriteOnlyProperty<Node, ::std::function<void(Node* node, EventState& es)>>
    onDispose;

  /**
   * List of attached behaviors
   */
  ReadOnlyProperty<Node, vector_t<Behavior<Node>*>> behaviors;

protected:
  int _childRenderId;

private:
  unordered_map_t<string_t, unique_ptr_t<AnimationRange>> _ranges;
  bool _isEnabled;
  bool _isReady;
  int _parentRenderId;
  Scene* _scene;
  Node* _parentNode;
  vector_t<Node*> _children;
  AnimationPropertiesOverride* _animationPropertiesOverride;
  unique_ptr_t<Matrix> _worldMatrix;
  Observer<Node>::Ptr _onDisposeObserver;

  // Behaviors
  vector_t<Behavior<Node>*> _behaviors;

}; // end of class Node

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_NODE_H
