#ifndef OIMO_COLLISION_BROADPHASE_DBVT_DBVT_BROAD_PHASE_H
#define OIMO_COLLISION_BROADPHASE_DBVT_DBVT_BROAD_PHASE_H

#include <vector>

#include <oimo/collision/broadphase/broad_phase.h>

namespace OIMO {

class DBVT;
struct DBVTNode;
class Proxy;
class Shape;

/**
 * @brief A broad-phase algorithm using dynamic bounding volume tree.
 */
class DBVTBroadPhase : public BroadPhase {

public:
  DBVTBroadPhase();
  ~DBVTBroadPhase();

  Proxy* createProxy(Shape* shape) override;
  void addProxy(Proxy* proxy) override;
  void removeProxy(Proxy* proxy) override;
  void collectPairs() override;

private:
  void _collide(DBVTNode* node1, DBVTNode* node2);

private:
  DBVT* _tree;
  std::vector<DBVTNode*> _stack;
  unsigned int _maxStack;
  std::vector<DBVTNode*> _leaves;
  unsigned int _numLeaves;
  unsigned int _maxLeaves;

}; // end of class DBVTBroadPhase

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_DBVT_DBVT_BROAD_PHASE_H
