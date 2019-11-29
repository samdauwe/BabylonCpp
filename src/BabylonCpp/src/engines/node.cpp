#include <babylon/engines/node.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_range.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/behavior.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/maths/matrix.h>
#include <babylon/meshes/abstract_mesh.h>

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
    auto r = constructorFunc(name, scene, options);
    r->_initCache();
    return r;
  };
}

Node::Node(const std::string& iName, Scene* scene, bool addToRootNodes)
    : name{iName}
    , id{iName}
    , doNotSerialize{false}
    , _isDisposed{false}
    , onReady{nullptr}
    , _currentRenderId{-1}
    , _childUpdateId{-1}
    , _worldMatrix{Matrix::Identity()}
    , _worldMatrixDeterminant{0.f}
    , _worldMatrixDeterminantIsDirty{true}
    , onDispose{this, &Node::set_onDispose}
    , behaviors{this, &Node::get_behaviors}
    , _isNode{true}
    , _isEnabled{true}
    , _isParentEnabled{true}
    , _isReady{true}
    , _parentUpdateId{-1}
    , _parentNode{nullptr}
    , _sceneRootNodesIndex{-1}
    , _animationPropertiesOverride{nullptr}
    , _onDisposeObserver{nullptr}
    , _addToRootNodes{addToRootNodes}
{
  state    = "";
  _scene   = scene ? scene : Engine::LastCreatedScene();
  uniqueId = _scene->getUniqueId();

  // We cannot call the virtual function _initCache() in the constructor
  // instead :
  // - we here call the base implementation (initCacheImpl()) which is not virtual
  // - Node::Construct() also calls _initCache()
  // - All the classes derived from Camera will call _initCache()
  //   via Camera::addToScene(), which is called by their named constructor "New()"
  //_initCache();
  initCacheImpl();
}

Node::~Node() = default;

void Node::addToRootNodes()
{
  if (_addToRootNodes) {
    addToSceneRootNodes();
  }
}

Type Node::type() const
{
  return Type::NODE;
}

bool Node::isDisposed() const
{
  return _isDisposed;
}

void Node::set_parent(Node* const& iParent)
{
  if (_parentNode == iParent) {
    return;
  }

  auto previousParentNode = _parentNode;

  // Remove self from list of children of parent
  if (_parentNode && !_parentNode->_children.empty()) {
    stl_util::remove_vector_elements_equal_sharedptr(_parentNode->_children, this);

    if (!iParent && !_isDisposed) {
      addToSceneRootNodes();
    }
  }

  // Store new parent
  _parentNode = iParent;

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
    _scene->rootNodes.emplace_back(shared_from_this());
  }
}

void Node::removeFromSceneRootNodes()
{
  if (_sceneRootNodesIndex != -1) {
    auto& rootNodes                                      = _scene->rootNodes;
    auto lastIdx                                         = rootNodes.size() - 1;
    rootNodes[static_cast<size_t>(_sceneRootNodesIndex)] = rootNodes[lastIdx];
    rootNodes[static_cast<size_t>(_sceneRootNodesIndex)]->_sceneRootNodesIndex
      = _sceneRootNodesIndex;
    _scene->rootNodes.pop_back();
    _sceneRootNodesIndex = -1;
  }
}

AnimationPropertiesOverridePtr& Node::get_animationPropertiesOverride()
{
  if (!_animationPropertiesOverride) {
    return _scene->animationPropertiesOverride;
  }
  return _animationPropertiesOverride;
}

void Node::set_animationPropertiesOverride(
  const AnimationPropertiesOverridePtr& value)
{
  _animationPropertiesOverride = value;
}

std::string Node::getClassName() const
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

float Node::_getWorldMatrixDeterminant()
{
  if (_worldMatrixDeterminantIsDirty) {
    _worldMatrixDeterminantIsDirty = false;
    _worldMatrixDeterminant        = _worldMatrix.determinant();
  }
  return _worldMatrixDeterminant;
}

Matrix& Node::worldMatrixFromCache()
{
  return _worldMatrix;
}

void Node::_initCache()
{
  initCacheImpl();
}

void Node::initCacheImpl()
{
  if (_cache.cache_inited)
    return;
  _cache.parent = nullptr;
  _cache.cache_inited = true;
}


void Node::updateCache(bool force)
{
  if (!force && isSynchronized()) {
    return;
  }

  _cache.parent = parent();

  _updateCache();
}

AbstractActionManagerPtr
Node::_getActionManagerForTrigger(const std::optional<unsigned int>& trigger,
                                  bool /*initialCall*/)
{
  if (!parent()) {
    return nullptr;
  }

  return parent()->_getActionManagerForTrigger(trigger, false);
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
    _parentUpdateId = _parentNode->_childUpdateId;
  }
}

bool Node::isSynchronizedWithParent() const
{
  if (!_parentNode) {
    return true;
  }

  if (_parentUpdateId != _parentNode->_childUpdateId) {
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
  if (!checkAncestors) {
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

std::vector<NodePtr>
Node::getChildren(const std::function<bool(const NodePtr& node)>& predicate,
                  bool directDescendantsOnly)
{
  return getDescendants(directDescendantsOnly, predicate);
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
    _ranges[iName] = std::make_shared<AnimationRange>(iName, from, to);
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

AnimationRangePtr Node::getAnimationRange(const std::string& iName)
{
  if (stl_util::contains(_ranges, iName)) {
    return _ranges[iName];
  }

  return nullptr;
}

std::vector<AnimationRangePtr> Node::getAnimationRanges()
{
  std::vector<AnimationRangePtr> animationRanges;
  for (const auto& item : _ranges) {
    animationRanges.emplace_back(item.second);
  }
  return animationRanges;
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
  for (const auto& [rangeName, range] : _ranges) {
    serializationRanges.emplace_back(
      AnimationRange(rangeName, range->from, range->to));
  }

  return serializationRanges;
}

Matrix& Node::computeWorldMatrix(bool /*force*/, bool /*useWasUpdatedFlag*/)
{
  return _worldMatrix;
}

void Node::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _isDisposed = true;

  if (!doNotRecurse) {
    auto nodes = getDescendants(true);
    for (const auto& node : nodes) {
      node->dispose(doNotRecurse, disposeMaterialAndTextures);
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
}

void Node::ParseAnimationRanges(Node& /*node*/, const json& /*parsedNode*/,
                                Scene* /*scene*/)
{
}

MinMax Node::getHierarchyBoundingVectors(
  bool includeDescendants,
  const std::function<bool(const AbstractMeshPtr& abstractMesh)>& predicate)
{
  // Ensures that all world matrix will be recomputed.
  getScene()->incrementRenderId();

  computeWorldMatrix(true);

  Vector3 min;
  Vector3 max;

  auto thisAbstractMesh = static_cast<AbstractMesh*>(this);
  if (thisAbstractMesh->getBoundingInfo()
      && !thisAbstractMesh->subMeshes.empty()) {
    // If this is an abstract mesh get its bounding info
    const auto& boundingInfo = *thisAbstractMesh->getBoundingInfo();
    min                      = boundingInfo.boundingBox.minimumWorld;
    max                      = boundingInfo.boundingBox.maximumWorld;
  }
  else {
    min = Vector3(std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max(),
                  std::numeric_limits<float>::max());
    max = Vector3(std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest(),
                  std::numeric_limits<float>::lowest());
  }

  if (includeDescendants) {
    auto descendants = getDescendants(false);

    for (const auto& descendant : descendants) {
      auto childMesh = std::static_pointer_cast<AbstractMesh>(descendant);
      childMesh->computeWorldMatrix(true);

      // Filters meshes based on custom predicate function.
      if (predicate && !predicate(childMesh)) {
        continue;
      }

      // make sure we have the needed params to get mix and max
      if (childMesh->getTotalVertices() == 0) {
        continue;
      }

      const auto& childBoundingInfo = *childMesh->getBoundingInfo();
      auto boundingBox              = childBoundingInfo.boundingBox;

      auto minBox = boundingBox.minimumWorld;
      auto maxBox = boundingBox.maximumWorld;

      Vector3::CheckExtends(minBox, min, max);
      Vector3::CheckExtends(maxBox, min, max);
    }
  }

  return {
    min, // min
    max  // max
  };
}

template void Node::_getDescendants<Node>(
  std::vector<NodePtr>& results, bool directDescendantsOnly,
  const std::function<bool(const NodePtr& node)>& predicate);
template void Node::_getDescendants<AbstractMesh>(
  std::vector<AbstractMeshPtr>& results, bool directDescendantsOnly,
  const std::function<bool(const NodePtr& node)>& predicate);
template void Node::_getDescendants<TransformNode>(
  std::vector<TransformNodePtr>& results, bool directDescendantsOnly,
  const std::function<bool(const NodePtr& node)>& predicate);

} // end of namespace BABYLON
