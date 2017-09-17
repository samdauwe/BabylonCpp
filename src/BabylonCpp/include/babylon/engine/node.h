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
   * Constructor
   * @param {string} name - the name and id to be given to this node
   * @param {BABYLON.Scene} the scene this node will be added to
   */
  Node(const string_t& name, Scene* scene);
  virtual ~Node();

  virtual IReflect::Type type() const override;
  void setParent(Node* parent);
  virtual const char* getClassName() const;
  Node* parent() const override;
  void setOnDispose(
    const ::std::function<void(Node* node, const EventState& es)>& callback);
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
   * Is this node ready to be used/rendered
   * @return {boolean} is it ready
   */
  bool isReady() const;
  /**
   * Is this node enabled.
   * If the node has a parent and is enabled, the parent will be inspected as
   * well.
   * @return {boolean} whether this node (and its parent) is enabled.
   * @see setEnabled
   */
  bool isEnabled();
  /**
   * Set the enabled state of this node.
   * @param {boolean} value - the new enabled state
   * @see isEnabled
   */
  void setEnabled(bool value);
  /**
   * Is this node a descendant of the given node.
   * The function will iterate up the hierarchy until the ancestor was found or
   * no more parents defined.
   * @param {BABYLON.Node} ancestor - The parent node to inspect
   * @see parent
   */
  bool isDescendantOf(const Node* ancestor);
  /**
   * Evaluate the list of children and determine if they should be considered as
   * descendants considering the given criterias
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
  /**
   * Will return all nodes that have this node as ascendant.
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
   * Get all child-meshes of this node.
   */
  virtual vector_t<AbstractMesh*>
  getChildMeshes(bool directDecendantsOnly,
                 const ::std::function<bool(Node* node)>& predicate);
  void _setReady(bool state);
  virtual vector_t<Animation*> getAnimations() override;
  Animation* getAnimationByName(const string_t& name);
  void createAnimationRange(const string_t& name, int from, int to);
  void deleteAnimationRange(const string_t& name, bool deleteFrames = true);
  AnimationRange* getAnimationRange(const string_t& name);
  Animatable* beginAnimation(const string_t& name, bool loop = false,
                             float speedRatio                       = 1.f,
                             ::std::function<void()> onAnimationEnd = nullptr);
  vector_t<AnimationRange> serializeAnimationRanges();
  virtual void dispose(bool doNotRecurse = false) override;
  static void ParseAnimationRanges(Node* node, const Json::value& parsedNode,
                                   Scene* scene);

public:
  string_t name;
  string_t id;
  unsigned int uniqueId;
  string_t state;

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
  int _childrenFlag;
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
