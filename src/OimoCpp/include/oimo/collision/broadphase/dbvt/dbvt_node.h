#ifndef OIMO_COLLISION_BROADPHASE_DBVT_DBVT_NODE_H
#define OIMO_COLLISION_BROADPHASE_DBVT_DBVT_NODE_H

#include <oimo/collision/broadphase/proxy.h>

namespace OIMO {

class AABB;
struct DBVTNode;
class DBVTProxy;

/**
 * @brief A node of the dynamic bounding volume tree.
 */
struct DBVTNode {

  DBVTNode();
  ~DBVTNode();

  // The first child node of this node.
  DBVTNode* child1;
  // The second child node of this node.
  DBVTNode* child2;
  //  The parent node of this tree.
  DBVTNode* parent;
  // The proxy of this node. This has no value if this node is not leaf.
  DBVTProxy* proxy;
  // The maximum distance from leaf nodes.
  int height;
  // The AABB of this node.
  AABB* aabb;

}; // end of struct DBVTNode

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_DBVT_DBVT_NODE_H
