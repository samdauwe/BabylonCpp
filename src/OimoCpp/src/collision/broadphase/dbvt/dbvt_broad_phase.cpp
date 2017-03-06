#include <oimo/collision/broadphase/dbvt/dbvt_broad_phase.h>

#include <algorithm>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/dbvt/dbvt.h>
#include <oimo/collision/broadphase/dbvt/dbvt_node.h>
#include <oimo/collision/broadphase/dbvt/dbvt_proxy.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/shape.h>

namespace OIMO {

DBVTBroadPhase::DBVTBroadPhase()
    : BroadPhase{}
    , _tree{make_unique<DBVT>()}
    , _maxStack{256}
    , _numLeaves{0}
    , _maxLeaves{256}
{
  type = BroadPhase::Type::BR_BOUNDING_VOLUME_TREE;
  _stack.resize(_maxStack);
  _leaves.reserve(_maxLeaves);
}

DBVTBroadPhase::~DBVTBroadPhase()
{
}

std::unique_ptr<Proxy> DBVTBroadPhase::createProxy(Shape* shape)
{
  return make_unique<DBVTProxy>(shape);
}

void DBVTBroadPhase::addProxy(Proxy* proxy)
{
  auto _proxy = dynamic_cast<DBVTProxy*>(proxy);
  if (_proxy == nullptr) {
    return;
  }
  _tree->insertLeaf(_proxy->leaf.get());
  _leaves.emplace_back(_proxy->leaf.get());
  ++_numLeaves;
}

void DBVTBroadPhase::removeProxy(Proxy* proxy)
{
  auto _proxy = dynamic_cast<DBVTProxy*>(proxy);
  if (_proxy == nullptr) {
    return;
  }
  _tree->deleteLeaf(_proxy->leaf.get());
  auto it = std::find_if(
    _leaves.begin(), _leaves.end(),
    [_proxy](const DBVTNode* node) { return node == _proxy->leaf.get(); });
  if (it != _leaves.end()) {
    _leaves.erase(it);
    --_numLeaves;
  }
}

void DBVTBroadPhase::collectPairs()
{
  if (_numLeaves < 2) {
    return;
  }
  DBVTNode* leaf = nullptr;
  float margin   = 0.1f;
  for (unsigned int i = _numLeaves; i-- > 0;) {
    leaf = _leaves[i];
    if (leaf->proxy->aabb->intersectTestTwo(*leaf->aabb)) {
      leaf->aabb->copy(*leaf->proxy->aabb, margin);
      _tree->deleteLeaf(leaf);
      _tree->insertLeaf(leaf);
      _collide(leaf, _tree->root);
    }
  }
}

void DBVTBroadPhase::_collide(DBVTNode* node1, DBVTNode* node2)
{
  bool l1 = false, l2 = false;
  unsigned int stackCount = 2;
  DBVTNode *n1 = nullptr, *n2 = nullptr;
  Shape *s1 = nullptr, *s2 = nullptr;
  _stack[0] = node1;
  _stack[1] = node2;

  while (stackCount > 0) {

    n1 = _stack[--stackCount];
    n2 = _stack[--stackCount];
    l1 = (n1->proxy != nullptr);
    l2 = (n2->proxy != nullptr);

    ++numPairChecks;

    if (l1 && l2) {
      s1 = n1->proxy->shape;
      s2 = n2->proxy->shape;
      if (s1 == s2 || s1->aabb->intersectTest(*s2->aabb)
          || !isAvailablePair(s1, s2)) {
        continue;
      }
      addPair(s1, s2);
    }
    else {
      if (n1->aabb->intersectTest(*n2->aabb)) {
        continue;
      }
      if (stackCount + 4 >= _maxStack) { // expand the stack
        _maxStack *= 2;
        _stack.resize(_maxStack);
      }
      if (l2 || (!l1 && (n1->aabb->surfaceArea() > n2->aabb->surfaceArea()))) {
        _stack[stackCount++] = n1->child1;
        _stack[stackCount++] = n2;
        _stack[stackCount++] = n1->child2;
        _stack[stackCount++] = n2;
      }
      else {
        _stack[stackCount++] = n1;
        _stack[stackCount++] = n2->child1;
        _stack[stackCount++] = n1;
        _stack[stackCount++] = n2->child2;
      }
    }
  }
}

} // end of namespace OIMO
