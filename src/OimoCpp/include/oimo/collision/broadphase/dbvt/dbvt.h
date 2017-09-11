#ifndef OIMO_COLLISION_BROADPHASE_DBVT_DBVT_H
#define OIMO_COLLISION_BROADPHASE_DBVT_DBVT_H

#include <array>
#include <vector>

#include <oimo/oimo_utils.h>

namespace OIMO {

class AABB;
struct DBVTNode;

/**
 * @brief A dynamic bounding volume tree for the broad-phase algorithm.
 */
class DBVT {

public:
  DBVT();
  ~DBVT();

  /**
   * Move a leaf.
   * @param   leaf
   */
  void moveLeaf(DBVTNode* leaf);

  /**
   * Insert a leaf to the tree.
   * @param   node
   */
  void insertLeaf(DBVTNode* leaf);

  int getBalance(DBVTNode* node) const;

  /**
   * Delete a leaf from the tree.
   * @param   node
   */
  void deleteLeaf(DBVTNode* leaf);

private:
  DBVTNode* _balance(DBVTNode* node);
  void _fix(DBVTNode* node);

public:
  // The root of the tree.
  DBVTNode* root;

private:
  std::vector<std::unique_ptr<DBVTNode>> _freeList;
  std::array<DBVTNode*, 16384> _freeNodes;
  unsigned int _numFreeNodes;
  std::unique_ptr<AABB> _aabb;

}; // end of class DBVTBroadPhase

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_DBVT_DBVT_H
