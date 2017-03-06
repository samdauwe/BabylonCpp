#ifndef OIMO_COLLISION_BROADPHASE_SAP_SAP_BROAD_PHASE_H
#define OIMO_COLLISION_BROADPHASE_SAP_SAP_BROAD_PHASE_H

#include <vector>

#include <oimo/collision/broadphase/broad_phase.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class Proxy;
class SAPAxis;
class Shape;

/**
* @brief A broad-phase collision detection algorithm using sweep and prune.
*/
class SAPBroadPhase : public BroadPhase {

public:
  SAPBroadPhase();
  ~SAPBroadPhase();

  std::unique_ptr<Proxy> createProxy(Shape* shape) override;
  void addProxy(Proxy* proxy) override;
  void removeProxy(Proxy* proxy) override;
  void collectPairs() override;

private:
  unsigned int _numElementsD;
  unsigned int _numElementsS;
  // Dynamic proxies
  std::vector<SAPAxis> _axesD;
  // static or sleeping proxies
  std::vector<SAPAxis> _axesS;
  // Indices
  unsigned int _index1;
  unsigned int _index2;

}; // end of class SAPBroadPhase

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_SAP_SAP_BROAD_PHASE_H
