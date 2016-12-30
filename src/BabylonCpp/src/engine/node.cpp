#include <babylon/engine/node.h>

#include <babylon/animations/animation.h>
#include <babylon/animations/animation_range.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>
#include <babylon/math/matrix.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Node::Node(const std::string& _name, Scene* scene)
    : name{_name}
    , id{_name}
    , state{""}
    , doNotSerialize{false}
    , _currentRenderId{-1}
    , _childrenFlag{-1}
    , _isEnabled{true}
    , _isReady{true}
    , _parentRenderId{-1}
    , _scene{scene}
    , _parentNode{nullptr}
    , _worldMatrix{std_util::make_unique<Matrix>(Matrix::Identity())}
{
  _initCache();
}

Node::~Node()
{
}

Node::Type Node::type() const
{
  return Node::Type::NODE;
}

void Node::setParent(Node* parent)
{
  if (_parentNode == parent) {
    return;
  }

  if (_parentNode) {
    _parentNode->_children.erase(std::remove(_parentNode->_children.begin(),
                                             _parentNode->_children.end(),
                                             this),
                                 _parentNode->_children.end());
  }

  _parentNode = parent;

  if (_parentNode) {
    _parentNode->_children.emplace_back(this);
  }
}

Node* Node::parent() const
{
  return _parentNode;
}

void Node::setOnDispose(const std::function<void()>& callback)
{
  if (_onDisposeObserver) {
    onDisposeObservable.remove(_onDisposeObserver);
  }
  _onDisposeObserver = onDisposeObservable.add(callback);
}

Scene* Node::getScene()
{
  return _scene;
}

Engine* Node::getEngine()
{
  return _scene->getEngine();
}

Matrix* Node::getWorldMatrix()
{
  return _worldMatrix.get();
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

// override it in derived class if you add new variables to the cache
bool Node::_isSynchronized()
{
  return true;
}

void Node::_markSyncedWithParent()
{
  _parentRenderId = parent()->_currentRenderId;
}

bool Node::isSynchronizedWithParent()
{
  if (!parent()) {
    return true;
  }

  if (_parentRenderId != _parentNode->_currentRenderId) {
    return false;
  }

  return parent()->isSynchronized();
}

bool Node::isSynchronized(bool _updateCache)
{
  bool check = hasNewParent();

  check = check ? check : !isSynchronizedWithParent();

  check = check ? check : !_isSynchronized();

  if (_updateCache) {
    updateCache(true);
  }

  return !check;
}

bool Node::hasNewParent(bool update)
{
  if (_cache.parent == parent()) {
    return false;
  }

  if (update) {
    _cache.parent = parent();
  }

  return true;
}

/**
 * Is this node ready to be used/rendered
 * @return {boolean} is it ready
 */
bool Node::isReady() const
{
  return _isReady;
}

bool Node::isEnabled()
{
  if (!_isEnabled) {
    return false;
  }

  if (parent()) {
    return parent()->isEnabled();
  }

  return true;
}

void Node::setEnabled(bool value)
{
  _isEnabled = value;
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

void Node::_getDescendants(std::vector<Node*>& results,
                           bool directDescendantsOnly,
                           const std::function<bool(Node* node)>& predicate)
{
  if (_children.empty()) {
    return;
  }

  for (auto& item : _children) {
    if (!predicate || predicate(item)) {
      results.emplace_back(item);
    }

    if (!directDescendantsOnly) {
      item->_getDescendants(results, false, predicate);
    }
  }
}

void Node::_getDescendants(std::vector<AbstractMesh*>& results,
                           bool directDescendantsOnly,
                           const std::function<bool(Node* node)>& predicate)
{
  if (_children.empty()) {
    return;
  }

  for (auto& item : _children) {
    if (!predicate || predicate(item)) {
      if (AbstractMesh* m = dynamic_cast<AbstractMesh*>(item)) {
        results.emplace_back(m);
      }
    }

    if (!directDescendantsOnly) {
      item->_getDescendants(results, false, predicate);
    }
  }
}

std::vector<Node*>
Node::getDescendants(bool directDescendantsOnly,
                     const std::function<bool(Node* node)>& predicate)
{
  std::vector<Node*> results;
  _getDescendants(results, directDescendantsOnly, predicate);

  return results;
}

std::vector<Node*>
Node::getChildren(const std::function<bool(Node* node)>& predicate)
{
  return getDescendants(true, predicate);
}

std::vector<AbstractMesh*>
Node::getChildMeshes(bool directDecendantsOnly,
                     const std::function<bool(Node* node)>& predicate)
{
  std::vector<AbstractMesh*> results;
  _getDescendants(results, directDecendantsOnly, [&predicate](Node* node) {
    return ((!predicate || predicate(node))
            && (dynamic_cast<AbstractMesh*>(node)));
  });
  return results;
}

void Node::_setReady(bool _state)
{
  if (_state == _isReady) {
    return;
  }

  if (!_state) {
    _isReady = false;
    return;
  }

  _isReady = true;
  if (onReady) {
    onReady(this);
  }
}

std::vector<Animation*> Node::getAnimations()
{
  return animations;
}

Animation* Node::getAnimationByName(const std::string& iName)
{
  auto it = std::find_if(
    animations.begin(), animations.end(),
    [&iName](Animation* animation) { return animation->name == iName; });

  return (it != animations.end() ? *it : nullptr);
}

void Node::createAnimationRange(const std::string& iName, int from, int to)
{
  // check name not already in use
  if (!std_util::contains(_ranges, iName)) {
    _ranges[iName] = std_util::make_unique<AnimationRange>(iName, from, to);
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
  if (std_util::contains(_ranges, iName)) {
    return _ranges[iName].get();
  }

  return nullptr;
}

Animatable* Node::beginAnimation(const std::string& iName, bool loop,
                                 float speedRatio,
                                 std::function<void()> onAnimationEnd)
{
  AnimationRange* range = getAnimationRange(iName);

  if (!range) {
    return nullptr;
  }

  return _scene->beginAnimation(this, range->from, range->to, loop, speedRatio,
                                onAnimationEnd);
}

std::vector<AnimationRange> Node::serializeAnimationRanges()
{
  std::vector<AnimationRange> serializationRanges;
  /*for (auto& item : _ranges) {
    serializationRanges.emplace_back(
      AnimationRange(item.first, item.second->from, item.second->to));
  }*/

  return serializationRanges;
}

void Node::dispose(bool /*doNotRecurse*/)
{
  setParent(nullptr);

  // Callback
  onDisposeObservable.notifyObservers(this);
  onDisposeObservable.clear();
}

void Node::ParseAnimationRanges(Node* node, const Json::value& parsedNode,
                                Scene* /*scene*/)
{
  if (parsedNode.contains("ranges")
      && (parsedNode.get("ranges").is<Json::array>())) {
    for (auto& range : parsedNode.get("ranges").get<Json::array>()) {
      node->createAnimationRange(Json::GetString(range, "name", ""),
                                 Json::GetNumber(range, "from", 0),
                                 Json::GetNumber(range, "to", 0));
    }
  }
}

std::string Node::TypeToString(Type type)
{
  switch (type) {
    case Type::BONE:
      return "Bone";
    case Type::CAMERA:
      return "Camera";
    case Type::FOLLOWCAMERA:
      return "FollowCamera";
    case Type::FREECAMERA:
      return "FreeCamera";
    case Type::TARGETCAMERA:
      return "TargetCamera";
    case Type::LIGHT:
      return "Light";
    case Type::DIRECTIONALLIGHT:
      return "DirectionalLight";
    case Type::HEMISPHERICLIGHT:
      return "HemisphericLight";
    case Type::POINTLIGHT:
      return "PointLight";
    case Type::SPOTLIGHT:
      return "SpotLight";
    case Type::ABSTRACTMESH:
      return "AbstractMesh";
    case Type::GROUNDMESH:
      return "GroundMesh";
    case Type::INSTANCEDMESH:
      return "InstancedMesh";
    case Type::LINESMESH:
      return "LinesMesh";
    case Type::MESH:
      return "Mesh";
    case Type::NODE:
    default:
      return "Node";
  }
}

} // end of namespace BABYLON
