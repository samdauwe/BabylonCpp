#ifndef OIMO_COLLISION_BROADPHASE_BRUTE_FORCE_BROAD_PHASE_H
#define OIMO_COLLISION_BROADPHASE_BRUTE_FORCE_BROAD_PHASE_H

#include <vector>

#include <oimo/collision/broadphase/broad_phase.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class Proxy;
class Shape;

/**
 * @brief A broad-phase algorithm with brute-force search.
 * This always checks for all possible pairs.
 */
class BruteForceBroadPhase : public BroadPhase {

public:
  BruteForceBroadPhase();
  ~BruteForceBroadPhase();

  std::unique_ptr<Proxy> createProxy(Shape* shape) override;
  void addProxy(Proxy* proxy) override;
  void removeProxy(Proxy* proxy) override;
  void collectPairs() override;

  BroadPhase::Type type() const;

private:
  BroadPhase::Type _type;
  std::vector<Proxy*> _proxies;

}; // end of class BruteForceBroadPhase

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_BRUTE_FORCE_BROAD_PHASE_H
