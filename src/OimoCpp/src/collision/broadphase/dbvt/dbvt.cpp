#include <oimo/collision/broadphase/dbvt/dbvt.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/dbvt/dbvt_node.h>

namespace OIMO {

DBVT::DBVT() : root{nullptr}, _numFreeNodes{0}, _aabb{make_unique<AABB>()}
{
}

DBVT::~DBVT()
{
}

void DBVT::moveLeaf(DBVTNode* leaf)
{
  deleteLeaf(leaf);
  insertLeaf(leaf);
}

void DBVT::insertLeaf(DBVTNode* leaf)
{
  if (root == nullptr) {
    root = leaf;
    return;
  }
  auto lb      = leaf->aabb.get();
  auto sibling = root;
  DBVTNode *c1 = nullptr, *c2 = nullptr;
  DBVTNode *oldParent = nullptr, *newParent = nullptr;
  AABB *b = nullptr, *c1b = nullptr, *c2b = nullptr;
  float oldArea = 0.f, newArea = 0.f;
  float creatingCost = 0.f, incrementalCost = 0.f;
  float discendingCost1 = 0.f, discendingCost2 = 0.f;
  while (sibling->proxy == nullptr) {
    // descend the node to search the best pair
    c1      = sibling->child1;
    c2      = sibling->child2;
    b       = sibling->aabb.get();
    c1b     = c1->aabb.get();
    c2b     = c2->aabb.get();
    oldArea = b->surfaceArea();
    _aabb->combine(*lb, *b);
    newArea      = _aabb->surfaceArea();
    creatingCost = newArea * 2;
    incrementalCost
      = (newArea - oldArea) * 2; // cost of creating a new pair with the node
    discendingCost1 = incrementalCost;
    _aabb->combine(*lb, *c1b);
    if (c1->proxy != nullptr) {
      // leaf cost = area(combined aabb)
      discendingCost1 += _aabb->surfaceArea();
    }
    else {
      // node cost = area(combined aabb) - area(old aabb)
      discendingCost1 += _aabb->surfaceArea() - c1b->surfaceArea();
    }
    discendingCost2 = incrementalCost;
    _aabb->combine(*lb, *c2b);
    if (c2->proxy != nullptr) {
      // leaf cost = area(combined aabb)
      discendingCost2 += _aabb->surfaceArea();
    }
    else {
      // node cost = area(combined aabb) - area(old aabb)
      discendingCost2 += _aabb->surfaceArea() - c2b->surfaceArea();
    }
    if (discendingCost1 < discendingCost2) {
      if (creatingCost < discendingCost1) {
        break; // stop descending
      }
      else {
        sibling = c1; // descend into first child
      }
    }
    else {
      if (creatingCost < discendingCost2) {
        break; // stop descending
      }
      else {
        sibling = c2; // descend into second child
      }
    }
  }
  oldParent = sibling->parent;
  if (_numFreeNodes > 0) {
    newParent = _freeNodes[--_numFreeNodes];
  }
  else {
    _freeList.emplace_back(make_unique<DBVTNode>());
    newParent = _freeList.back().get();
  }

  newParent->parent = oldParent;
  newParent->child1 = leaf;
  newParent->child2 = sibling;
  newParent->aabb->combine(*leaf->aabb, *sibling->aabb);
  newParent->height = sibling->height + 1;
  sibling->parent   = newParent;
  leaf->parent      = newParent;
  if (sibling == root) {
    // replace root
    root = newParent;
  }
  else {
    // replace child
    if (oldParent->child1 == sibling) {
      oldParent->child1 = newParent;
    }
    else {
      oldParent->child2 = newParent;
    }
  }
  // update whole tree
  do {
    newParent = _balance(newParent);
    _fix(newParent);
    newParent = newParent->parent;
  } while (newParent != nullptr);
}

int DBVT::getBalance(DBVTNode* node) const
{
  if (node->proxy != nullptr) {
    return 0;
  }
  return node->child1->height - node->child2->height;
}

void DBVT::deleteLeaf(DBVTNode* leaf)
{
  if (leaf == root) {
    root = nullptr;
    return;
  }
  auto parent       = leaf->parent;
  DBVTNode* sibling = nullptr;
  if (parent->child1 == leaf) {
    sibling = parent->child2;
  }
  else {
    sibling = parent->child1;
  }
  if (parent == root) {
    root            = sibling;
    sibling->parent = nullptr;
    return;
  }
  auto grandParent = parent->parent;
  sibling->parent  = grandParent;
  if (grandParent->child1 == parent) {
    grandParent->child1 = sibling;
  }
  else {
    grandParent->child2 = sibling;
  }
  if (_numFreeNodes < 16384) {
    _freeNodes[_numFreeNodes++] = parent;
  }
  do {
    grandParent = _balance(grandParent);
    _fix(grandParent);
    grandParent = grandParent->parent;
  } while (grandParent != nullptr);
}

DBVTNode* DBVT::_balance(DBVTNode* node)
{
  auto nh = node->height;
  if (nh < 2) {
    return node;
  }
  auto p       = node->parent;
  auto l       = node->child1;
  auto r       = node->child2;
  auto lh      = l->height;
  auto rh      = r->height;
  auto balance = lh - rh;
  int t; // for bit operation

  /**
   *          [ N ]
   *         /     \
   *    [ L ]       [ R ]
   *     / \         / \
   * [L-L] [L-R] [R-L] [R-R]
   */

  // Is the tree balanced?
  if (balance > 1) {
    auto ll  = l->child1;
    auto lr  = l->child2;
    auto llh = ll->height;
    auto lrh = lr->height;

    // Is L-L higher than L-R?
    if (llh > lrh) {
      // set N to L-R
      l->child2    = node;
      node->parent = l;

      /**
       *          [ L ]
       *         /     \
       *    [L-L]       [ N ]
       *     / \         / \
       * [...] [...] [ L ] [ R ]
       */

      // set L-R
      node->child1 = lr;
      lr->parent   = node;

      /**
       *          [ L ]
       *         /     \
       *    [L-L]       [ N ]
       *     / \         / \
       * [...] [...] [L-R] [ R ]
       */

      // fix bounds and heights
      node->aabb->combine(*lr->aabb, *r->aabb);
      t            = lrh - rh;
      node->height = lrh - (t & t >> 31) + 1;
      l->aabb->combine(*ll->aabb, *node->aabb);
      t         = llh - nh;
      l->height = llh - (t & t >> 31) + 1;
    }
    else {
      // set N to L-L
      l->child1    = node;
      node->parent = l;

      /**
       *          [ L ]
       *         /     \
       *    [ N ]       [L-R]
       *     / \         / \
       * [ L ] [ R ] [...] [...]
       */

      // set L-L
      node->child1 = ll;
      ll->parent   = node;

      /**
       *          [ L ]
       *         /     \
       *    [ N ]       [L-R]
       *     / \         / \
       * [L-L] [ R ] [...] [...]
       */

      // fix bounds and heights
      node->aabb->combine(*ll->aabb, *r->aabb);
      t            = llh - rh;
      node->height = llh - (t & t >> 31) + 1;

      l->aabb->combine(*node->aabb, *lr->aabb);
      t         = nh - lrh;
      l->height = nh - (t & t >> 31) + 1;
    }
    // set new parent of L
    if (p != nullptr) {
      if (p->child1 == node) {
        p->child1 = l;
      }
      else {
        p->child2 = l;
      }
    }
    else {
      this->root = l;
    }
    l->parent = p;
    return l;
  }
  else if (balance < -1) {
    auto rl  = r->child1;
    auto rr  = r->child2;
    auto rlh = rl->height;
    auto rrh = rr->height;

    // Is R-L higher than R-R?
    if (rlh > rrh) {
      // set N to R-R
      r->child2    = node;
      node->parent = r;

      /**
       *          [ R ]
       *         /     \
       *    [R-L]       [ N ]
       *     / \         / \
       * [...] [...] [ L ] [ R ]
       */

      // set R-R
      node->child2 = rr;
      rr->parent   = node;

      /**
       *          [ R ]
       *         /     \
       *    [R-L]       [ N ]
       *     / \         / \
       * [...] [...] [ L ] [R-R]
       */

      // fix bounds and heights
      node->aabb->combine(*l->aabb, *rr->aabb);
      t            = lh - rrh;
      node->height = lh - (t & t >> 31) + 1;
      r->aabb->combine(*rl->aabb, *node->aabb);
      t         = rlh - nh;
      r->height = rlh - (t & t >> 31) + 1;
    }
    else {
      // set N to R-L
      r->child1    = node;
      node->parent = r;

      /**
       *          [ R ]
       *         /     \
       *    [ N ]       [R-R]
       *     / \         / \
       * [ L ] [ R ] [...] [...]
       */

      // set R-L
      node->child2 = rl;
      rl->parent   = node;

      /**
       *          [ R ]
       *         /     \
       *    [ N ]       [R-R]
       *     / \         / \
       * [ L ] [R-L] [...] [...]
       */

      // fix bounds and heights
      node->aabb->combine(*l->aabb, *rl->aabb);
      t            = lh - rlh;
      node->height = lh - (t & t >> 31) + 1;
      r->aabb->combine(*node->aabb, *rr->aabb);
      t         = nh - rrh;
      r->height = nh - (t & t >> 31) + 1;
    }
    // set new parent of R
    if (p != nullptr) {
      if (p->child1 == node) {
        p->child1 = r;
      }
      else {
        p->child2 = r;
      }
    }
    else {
      root = r;
    }
    r->parent = p;
    return r;
  }
  return node;
}

void DBVT::_fix(DBVTNode* node)
{
  auto c1 = node->child1;
  auto c2 = node->child2;
  node->aabb->combine(*c1->aabb, *c2->aabb);
  node->height = c1->height < c2->height ? c2->height + 1 : c1->height + 1;
}

} // end of namespace OIMO
