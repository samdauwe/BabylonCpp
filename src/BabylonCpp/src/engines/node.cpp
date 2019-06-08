#include <babylon/engines/node.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_range.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/math/matrix.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

std::unordered_map<std::string, NodeConstructor> Node::_NodeConstructors;

void Node::AddNodeConstructor(const std::string& type,
                              const NodeConstructor& constructorFunc)
{
  _NodeConstructors[type] = constructorFunc;
}

std::function<NodePtr()> Node::Construct(const std::string& type,
                                         const std::string& name, Scene* scene,
                                         const std::optional<json>& options)
{
  if (!stl_util::contains(_NodeConstructors, type)) {
    return nullptr;
  }

  auto& constructorFunc = _NodeConstructors[type];

  return [name, scene, options, constructorFunc]() {
    return constructorFunc(name, scene, options);
  };
}

Node::Node(const std::string& iName, Scene* scene, bool /*addToRootNodes*/)
    : name{iName}
    , id{iName}
    , doNotSerialize{false}
    , _isDisposed{false}
    , _currentRenderId{-1}
    , _childUpdateId{-1}
    , _worldMatrix{Matrix::Identity()}
    , _worldMatrixDeterminant{0.f}
    , _worldMatrixDeterminantIsDirty{true}
    , animationPropertiesOverride{this, &Node::get_animationPropertiesOverride,
                                  &Node::set_animationPropertiesOverride}
    , onDispose{this, &Node::set_onDispose}
    , behaviors{this, &Node::get_behaviors}
    , _childRenderId{-1}
    , _isEnabled{true}
    , _isParentEnabled{true}
    , _isReady{true}
    , _parentRenderId{-1}
    , _parentNode{nullptr}
    , _sceneRootNodesIndex{-1}
    , _animationPropertiesOverride{nullptr}
    , _onDisposeObserver{nullptr}
{
  state    = "";
  _scene   = scene ? scene : Engine::LastCreatedScene();
  uniqueId = _scene->getUniqueId();
  _initCache();

  addToSceneRootNodes();
}

Node::~Node()
{
}

Type Node::type() const
{
  return Type::NODE;
}

bool Node::isDisposed() const
{
  return _isDisposed;
}

void Node::set_parent(Node* const& parent)
{
  if (_parentNode == parent) {
    return;
  }

  auto previousParentNode = _parentNode;

  // Remove self from list of children of parent
  if (_parentNode && !_parentNode->_children.empty()) {
    _parentNode->_children.erase(std::remove_if(_parentNode->_children.begin(),
                                                _parentNode->_children.end(),
                                                [this](const NodePtr& node) {
                                                  return node.get() == this;
                                                }),
                                 _parentNode->_children.end());

    if (!parent) {
      addToSceneRootNodes();
    }
  }

  // Store new parent
  _parentNode = parent;

  // Add as child to new parent
  if (_parentNode) {
    _parentNode->_children.emplace_back(shared_from_this());

    if (!previousParentNode) {
      removeFromSceneRootNodes();
    }
  }

  // Enabled state
  _syncParentEnabledState();
}

Node*& Node::get_parent()
{
  return _parentNode;
}

void Node::addToSceneRootNodes()
{
  if (_sceneRootNodesIndex == -1) {
    _sceneRootNodesIndex = static_cast<int>(_scene->rootNodes.size());
    _scene->rootNodes.emplace_back(this);
  }
}

void Node::removeFromSceneRootNodes()
{
  if (_sceneRootNodesIndex != -1) {
    auto rootNodes                                       = _scene->rootNodes;
    auto lastIdx                                         = rootNodes.size() - 1;
    rootNodes[static_cast<size_t>(_sceneRootNodesIndex)] = rootNodes[lastIdx];
    rootNodes[static_cast<size_t>(_sceneRootNodesIndex)]->_sceneRootNodesIndex
      = _sceneRootNodesIndex;
    _scene->rootNodes.pop_back();
    _sceneRootNodesIndex = -1;
  }
}

AnimationPropertiesOverride*& Node::get_animationPropertiesOverride()
{
  if (!_animationPropertiesOverride) {
    return _scene->animationPropertiesOverride;
  }
  return _animationPropertiesOverride;
}

void Node::set_animationPropertiesOverride(
  AnimationPropertiesOverride* const& value)
{
  _animationPropertiesOverride = value;
}

const std::string Node::getClassName() const
{
  return "Node";
}

void Node::set_onDispose(
  const std::function<void(Node* node, EventState& es)>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

Scene* Node::getScene() const
{
  return _scene;
}

Engine* Node::getEngine()
{
  return _scene->getEngine();
}

NodePtr Node::getAsNodePtr()
{
  return shared_from_this();
}

Node& Node::addBehavior(Behavior<Node>* behavior, bool attachImmediately)
{
  auto it = std::find(_behaviors.begin(), _behaviors.end(), behavior);

  if (it != _behaviors.end()) {
    return *this;
  }

  behavior->init();

  if (_scene->isLoading() && !attachImmediately) {
    // We defer the attach when the scene will be loaded
    _scene->onDataLoadedObservable.addOnce(
      [this, &behavior](Scene* /*scene*/, EventState /*es*/) {
        behavior->attach(shared_from_this());
      });
  }
  else {
    behavior->attach(shared_from_this());
  }
  _behaviors.emplace_back(behavior);

  return *this;
}

Node& Node::removeBehavior(Behavior<Node>* behavior)
{
  auto it = std::find(_behaviors.begin(), _behaviors.end(), behavior);

  if (it == _behaviors.end()) {
    return *this;
  }

  (*it)->detach();
  _behaviors.erase(it);

  return *this;
}

std::vector<Behavior<Node>*>& Node::get_behaviors()
{
  return _behaviors;
}

Behavior<Node>* Node::getBehaviorByName(const std::string& iName)
{
  auto it = std::find_if(_behaviors.begin(), _behaviors.end(),
                         [&iName](const Behavior<Node>* behavior) {
                           return behavior->name == iName;
                         });

  return (it != _behaviors.end() ? *it : nullptr);
}

Matrix& Node::getWorldMatrix()
{
  if (_currentRenderId != _scene->getRenderId()) {
    computeWorldMatrix();
  }
  return _worldMatrix;
}

float Node::_getWorldMatrixDeterminant() const
{
  return _worldMatrixDeterminant;
}

Matrix& Node::worldMatrixFromCache()
{
  return _worldMatrix;
}

void Node::_initCache()
{
  _cache.parent = nullptr;
}

void Node::updateCache(bool force)
{
  if (!force && isSynchronized()) {
    return;
  }

  _cache.parent = parent();

  _updateCache();
}

void Node::_updateCache(bool /*ignoreParentClass*/)
{
}

bool Node::_isSynchronized()
{
  return true;
}

void Node::_markSyncedWithParent()
{
  if (_parentNode) {
    _parentRenderId = _parentNode->_childRenderId;
  }
}

bool Node::isSynchronizedWithParent() const
{
  if (!_parentNode) {
    return true;
  }

  if (_parentRenderId != _parentNode->_childRenderId) {
    return false;
  }

  return _parentNode->isSynchronized();
}

bool Node::isSynchronized()
{
  if (_cache.parent != _parentNode) {
    _cache.parent = _parentNode;
    return false;
  }

  if (_parentNode && !isSynchronizedWithParent()) {
    return false;
  }

  return _isSynchronized();
}

bool Node::isReady(bool /*completeCheck*/, bool /*forceInstanceSupport*/)
{
  return _isReady;
}

bool Node::isEnabled(bool checkAncestors)
{
  if (checkAncestors == false) {
    return _isEnabled;
  }

  if (!_isEnabled) {
    return false;
  }

  return _isParentEnabled;
}

void Node::_syncParentEnabledState()
{
  _isParentEnabled = _parentNode ? _parentNode->isEnabled() : true;

  if (!_children.empty()) {
    for (auto& c : _children) {
      c->_syncParentEnabledState(); // Force children to update accordingly
    }
  }
}

void Node::setEnabled(bool value)
{
  _isEnabled = value;

  _syncParentEnabledState();
}

bool Node::isDescendantOf(const Node* ancestor)
{
  if (parent()) {
    if (parent() == ancestor) {
      return true;
    }

    return parent()->isDescendantOf(ancestor);
  }
  return false;
}

template <typename T>
void Node::_getDescendants(
  std::vector<std::shared_ptr<T>>& results, bool directDescendantsOnly,
  const std::function<bool(const NodePtr& node)>& predicate)
{
  if (_children.empty()) {
    return;
  }

  for (auto& item : _children) {
    if (!predicate || predicate(item)) {
      if (auto m = std::static_pointer_cast<T>(item)) {
        results.emplace_back(m);
      }
    }

    if (!directDescendantsOnly) {
      item->_getDescendants(results, false, predicate);
    }
  }
}

std::vector<NodePtr>
Node::getDescendants(bool directDescendantsOnly,
                     const std::function<bool(const NodePtr& node)>& predicate)
{
  std::vector<NodePtr> results;
  _getDescendants(results, directDescendantsOnly, predicate);

  return results;
}

std::vector<AbstractMeshPtr>
Node::getChildMeshes(bool directDescendantsOnly,
                     const std::function<bool(const NodePtr& node)>& predicate)
{
  std::vector<AbstractMeshPtr> results;
  _getDescendants(results, directDescendantsOnly,
                  [&predicate](const NodePtr& node) {
                    return ((!predicate || predicate(node))
                            && (std::static_pointer_cast<AbstractMesh>(node)));
                  });
  return results;
}

std::vector<TransformNodePtr> Node::getChildTransformNodes(
  bool directDescendantsOnly,
  const std::function<bool(const NodePtr& node)>& predicate)
{
  std::vector<TransformNodePtr> results;
  _getDescendants(results, directDescendantsOnly,
                  [&predicate](const NodePtr& node) {
                    return ((!predicate || predicate(node))
                            && (std::static_pointer_cast<TransformNode>(node)));
                  });
  return results;
}

std::vector<NodePtr>
Node::getChildren(const std::function<bool(const NodePtr& node)>& predicate)
{
  return getDescendants(true, predicate);
}

void Node::_setReady(bool iState)
{
  if (iState == _isReady) {
    return;
  }

  if (!iState) {
    _isReady = false;
    return;
  }

  _isReady = true;
  if (onReady) {
    onReady(this);
  }
  _isReady = true;
}

std::vector<AnimationPtr> Node::getAnimations()
{
  return animations;
}

AnimationPtr Node::getAnimationByName(const std::string& iName)
{
  auto it = std::find_if(animations.begin(), animations.end(),
                         [&iName](const AnimationPtr& animation) {
                           return animation->name == iName;
                         });

  return (it == animations.end() ? nullptr : *it);
}

void Node::createAnimationRange(const std::string& iName, float from, float to)
{
  // check name not already in use
  if (!stl_util::contains(_ranges, iName)) {
    _ranges[iName] = std::make_unique<AnimationRange>(iName, from, to);
    for (auto& animation : animations) {
      if (animation) {
        animation->createRange(iName, from, to);
      }
    }
  }
}

void Node::deleteAnimationRange(const std::string& iName, bool deleteFrames)
{
  for (auto& animation : animations) {
    if (animation) {
      animation->deleteRange(iName, deleteFrames);
    }
  }

  _ranges.erase(iName);
}

AnimationRange* Node::getAnimationRange(const std::string& iName)
{
  if (stl_util::contains(_ranges, iName)) {
    return _ranges[iName].get();
  }

  return nullptr;
}

AnimatablePtr Node::beginAnimation(const std::string& iName, bool loop,
                                   float speedRatio,
                                   std::function<void()> onAnimationEnd)
{
  auto range = getAnimationRange(iName);

  if (!range) {
    return nullptr;
  }

  auto _this = std::static_pointer_cast<IAnimatable>(shared_from_this());

  return _scene->beginAnimation(_this, range->from, range->to, loop, speedRatio,
                                onAnimationEnd);
}

std::vector<AnimationRange> Node::serializeAnimationRanges()
{
  std::vector<AnimationRange> serializationRanges;
  for (auto& item : _ranges) {
    serializationRanges.emplace_back(
      AnimationRange(item.first, item.second->from, item.second->to));
  }

  return serializationRanges;
}

Matrix& Node::computeWorldMatrix(bool /*force*/, bool /*useWasUpdatedFlag*/)
{
  return _worldMatrix;
}

void Node::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  if (!doNotRecurse) {
    auto nodes = getDescendants(true);
    for (auto& node : nodes) {
      node->dispose(doNotRecurse, disposeMaterialAndTextures);
    }
  }
  else {
    auto transformNodes = getChildTransformNodes(true);
    for (auto& transformNode : transformNodes) {
      transformNode->setParent(nullptr);
      transformNode->computeWorldMatrix(true);
    }
  }

  if (!parent()) {
    removeFromSceneRootNodes();
  }
  else {
    parent = nullptr;
  }

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();

  // Behaviors
  for (auto& behavior : _behaviors) {
    behavior->detach();
  }

  _behaviors.clear();
  _isDisposed = true;
}

void Node::ParseAnimationRanges(Node& /*node*/, const json& /*parsedNode*/,
                                Scene* /*scene*/)
{
}

template void Node::_getDescendants<Node>(
  std::vector<NodePtr>& results, bool directDescendantsOnly = false,
  const std::function<bool(const NodePtr& node)>& predicate = nullptr);
template void Node::_getDescendants<AbstractMesh>(
  std::vector<AbstractMeshPtr>& results, bool directDescendantsOnly = false,
  const std::function<bool(const NodePtr& node)>& predicate = nullptr);
template void Node::_getDescendants<TransformNode>(
  std::vector<TransformNodePtr>& results, bool directDescendantsOnly = false,
  const std::function<bool(const NodePtr& node)>& predicate = nullptr);

} // end of namespace BABYLON
