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
   * @brief Constructor
   * @param {string} name - the name and id to be given to this node
   * @param {BABYLON.Scene} the scene this node will be added to
   */
  Node(const string_t& name, Scene* scene = nullptr);
  virtual ~Node() override;

  virtual IReflect::Type type() const override;
  void setParent(Node* parent);
  virtual const char* getClassName() const;
  Node* parent() const override;
  void setOnDispose(
    const ::std::function<void(Node* node, EventState& es)>& callback);
  virtual Scene* getScene();
  Engine* getEngine();

  // Behaviors
  Node& addBehavior(Behavior<Node>* behavior);
  Node& removeBehavior(Behavior<Node>* behavior);
  vector_t<Behavior<Node>*>& behaviors();
  const vector_t<Behavior<Node>*>& behaviors() const;
  Behavior<Node>* getBehaviorByName(const string_t& name);

  virtual Matrix* getWorldMatrix() override;
  virtual void _initCache();
  virtual void updateCache(bool force = false);
  virtual void _updateCache(bool ignoreParentClass = false);
  virtual bool _isSynchronized();
  void _markSyncedWithParent();
  bool isSynchronizedWithParent();
  bool isSynchronized(bool updateCache = false);
  bool hasNewParent(bool update = false);

  /**
   * @brief Is this node ready to be used/rendered
   * @return {boolean} is it ready
   */
  bool isReady() const;

  /**
   * @brief Is this node enabled.
   * If the node has a parent, all ancestors will be checked and false will be
   * returned if any are false (not enabled), otherwise will return true.
   * @param {boolean} [checkAncestors=true] - Indicates if this method should
   * check the ancestors. The default is to check the ancestors. If set to
   * false, the method will return the value of this node without checking
   * ancestors.
   * @return {boolean} whether this node (and its parent) is enabled.
   * @see setEnabled
   */
  bool isEnabled(bool checkAncestors = true);

  /**
   * @brief Set the enabled state of this node.
   * @param {boolean} value - the new enabled state
   * @see isEnabled
   */
  void setEnabled(bool value);

  /**
   * @brief Is this node a descendant of the given node.
   * The function will iterate up the hierarchy until the ancestor was found or
   * no more parents defined.
   * @param {BABYLON.Node} ancestor - The parent node to inspect
   * @see parent
   */
  bool isDescendantOf(const Node* ancestor);

  /**
   * @brief Evaluate the list of children and determine if they should be
   * considered as descendants considering the given criterias
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
  void
  _getDescendants(vector_t<Node*>& results, bool directDescendantsOnly = false,
                  const ::std::function<bool(Node* node)>& predicate = nullptr);
  void _getDescendants(vector_t<AbstractMesh*>& results,
                       bool directDescendantsOnly = false,
                       const ::std::function<bool(Node* node)>& predicate
                       = nullptr);
  void _getDescendants(vector_t<TransformNode*>& results,
                       bool directDescendantsOnly = false,
                       const ::std::function<bool(Node* node)>& predicate
                       = nullptr);

  /**
   * @brief Will return all nodes that have this node as ascendant.
   * @param {boolean} directDescendantsOnly if true only direct descendants of
   * 'this' will be considered, if false direct and also indirect (children of
   * children, an so on in a recursive manner) descendants of 'this' will be
   * considered.
   * @param predicate: an optional predicate that will be called on every
   * evaluated children, the predicate must return true for a given child to be
   * part of the result, otherwise it will be ignored.
   * @return {BABYLON.Node[]} all children nodes of all types.
   */
  vector_t<Node*>
  getDescendants(bool directDescendantsOnly                         = false,
                 const ::std::function<bool(Node* node)>& predicate = nullptr);

  /**
   * @brief Get all child-meshes of this node.
   */
  virtual vector_t<AbstractMesh*>
  getChildMeshes(bool directDescendantsOnly                         = false,
                 const ::std::function<bool(Node* node)>& predicate = nullptr);

  /**
   * @brief Get all child-transformNodes of this node.
   */
  virtual vector_t<TransformNode*>
  getChildTransformNodes(bool directDescendantsOnly = false,
                         const ::std::function<bool(Node* node)>& predicate
                         = nullptr);

  /**
   * Get all direct children of this node.
   */
  vector_t<Node*> getChildren(const ::std::function<bool(Node* node)>& predicate
                              = nullptr);

  void _setReady(bool state);
  virtual vector_t<Animation*> getAnimations() override;
  Animation* getAnimationByName(const string_t& name);
  void createAnimationRange(const string_t& name, float from, float to);
  void deleteAnimationRange(const string_t& name, bool deleteFrames = true);
  AnimationRange* getAnimationRange(const string_t& name);

  /**
   * @brief Will start the animation sequence.
   * @param name defines the range frames for animation sequence
   * @param loop defines if the animation should loop (false by default)
   * @param speedRatio defines the speed factor in which to run the animation (1
   * by default)
   * @param onAnimationEnd defines a function to be executed when the animation
   * ended (undefined by default)
   * @returns the {BABYLON.Animatable} object created for this animation. If
   * range does not exist, it will return null
   */
  Animatable* beginAnimation(const string_t& name, bool loop = false,
                             float speedRatio                       = 1.f,
                             ::std::function<void()> onAnimationEnd = nullptr);

  vector_t<AnimationRange> serializeAnimationRanges();
  virtual Matrix& computeWorldMatrix(bool force = false);
  virtual void dispose(bool doNotRecurse = false) override;
  static void ParseAnimationRanges(Node* node, const Json::value& parsedNode,
                                   Scene* scene);

public:
  string_t name;
  string_t id;
  size_t uniqueId;

  Json::object metadata;
  bool doNotSerialize;

  vector_t<Animation*> animations;

  ::std::function<void(Node* node)> onReady;

  int _currentRenderId;
  string_t parentId;
  string_t _waitingParentId;
  NodeCache _cache;

  /**
   * An event triggered when the mesh is disposed.
   */
  Observable<Node> onDisposeObservable;

private:
  unordered_map_t<string_t, unique_ptr_t<AnimationRange>> _ranges;
  bool _isEnabled;
  bool _isReady;
  int _parentRenderId;
  Scene* _scene;
  Node* _parentNode;
  vector_t<Node*> _children;
  unique_ptr_t<Matrix> _worldMatrix;
  Observer<Node>::Ptr _onDisposeObserver;

  // Behaviors
  vector_t<Behavior<Node>*> _behaviors;

}; // end of class Node

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_NODE_H
