#ifndef OIMO_COLLISION_BROADPHASE_BROAD_PHASE_H
#define OIMO_COLLISION_BROADPHASE_BROAD_PHASE_H

// -- Disable warnings -- //
// 'identifier' : class 'type' needs to have dll-interface to be used by clients
// of class 'type2'
#pragma warning(disable : 4251)

#include <memory>
#include <vector>

#include <oimo/collision/broadphase/pair.h>
#include <oimo/oimo_api.h>

namespace OIMO {

class Proxy;
class Shape;

/**
 * @brief The broad-phase is used for collecting all possible pairs for
 * collision.
 */
class OIMO_SHARED_EXPORT BroadPhase {

public:
  // BroadPhase type
  enum class Type : unsigned int {
    // Brute force broad-phase algorithm.
    BR_BRUTE_FORCE = 0,
    // Sweep and prune broad-phase algorithm.
    BR_SWEEP_AND_PRUNE = 1,
    // Dynamic bounding volume tree broad-phase algorithm.
    BR_BOUNDING_VOLUME_TREE = 2,
    // Unknown broad-phase algorithm.
    BR_NULL = 3
  }; // end of enum class Type

public:
  BroadPhase();
  virtual ~BroadPhase();

  /**
   * Create a new proxy.
   * @param   shape
   * @return
   */
  virtual std::unique_ptr<Proxy> createProxy(Shape* shape) = 0;

  /**
   * Add the proxy into the broad-phase.
   * @param   proxy
   */
  virtual void addProxy(Proxy* proxy) = 0;

  /**
   * Remove the proxy from the broad-phase.
   * @param   proxy
   */
  virtual void removeProxy(Proxy* proxy) = 0;

  /**
   * Returns whether the pair is available or not.
   * @param   s1
   * @param   s2
   * @return
   */
  bool isAvailablePair(Shape* s1, Shape* s2);

  // Detect overlapping pairs.
  void detectPairs();

  /**
   * Remove the proxy from the broad-phase.
   * @param   proxy
   */
  virtual void collectPairs() = 0;

  void addPair(Shape* s1, Shape* s2);

public:
  BroadPhase::Type type;
  // The number of pair checks.
  int numPairChecks;
  // The number of pairs.
  unsigned int numPairs;
  std::vector<Pair> pairs;

}; // end of class BroadPhase

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_BROAD_PHASE_H
