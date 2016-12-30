#ifndef OIMO_UTIL_PERFORMANCE_H
#define OIMO_UTIL_PERFORMANCE_H

#include <array>
#include <string>

namespace OIMO {

class World;

/**
 * @brief Performance statistics container.
 */
class Performance {

public:
  Performance(World* world);
  ~Performance();

  void upfps();

private:
  float _updatingTime() const;

public:
  std::string version;
  std::string broadPhase;
  float fps;
  unsigned int numRigidBodies;
  unsigned int numContacts;
  unsigned int numContactPoints;
  unsigned int numPairChecks;
  unsigned int numIslands;
  time_t broadPhaseTime;
  time_t narrowPhaseTime;
  time_t solvingTime;
  time_t totalTime;
  time_t updatingTime;

private:
  // World* _parent;
  // std::array<float, 3> _f;

}; // end of class Performance

} // end of namespace OIMO

#endif // end of OIMO_UTIL_PERFORMANCE_H
