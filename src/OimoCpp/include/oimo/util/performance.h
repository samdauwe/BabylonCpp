#ifndef OIMO_UTIL_PERFORMANCE_H
#define OIMO_UTIL_PERFORMANCE_H

#include <array>
#include <chrono>
#include <string>

namespace OIMO {

class World;

/**
 * @brief A performance evaluator.
 */
class Performance {

public:
  using millisecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
  using milliseconds_t = std::chrono::milliseconds;
  using microsecond_t = std::chrono::duration<uint64_t, std::ratio<1, 1000000>>;
  using microseconds_t        = std::chrono::microseconds;
  using high_res_clock_t      = std::chrono::high_resolution_clock;
  using high_res_time_point_t = std::chrono::time_point<high_res_clock_t>;
  using system_clock_t        = std::chrono::system_clock;
  using system_time_point_t   = std::chrono::time_point<system_clock_t>;

public:
  static const milliseconds_t ONE_SECOND;

public:
  Performance(World* world);
  ~Performance();

  void setTime(unsigned int n = 0);
  void resetMax();
  void calcBroadPhase();
  void calcNarrowPhase();
  void calcEnd();
  void upfps();

public:
  World* parent;
  std::array<float, 13> infos;
  std::array<high_res_time_point_t, 2> f;
  std::array<high_res_time_point_t, 3> times;
  std::string broadPhase;
  std::string version;
  float fps, fpsTmp;
  unsigned int tt;
  unsigned int numRigidBodies;
  unsigned int numContacts;
  unsigned int numContactPoints;
  unsigned int numPairChecks;
  unsigned int numIslands;
  microsecond_t broadPhaseTime;
  microsecond_t narrowPhaseTime;
  microsecond_t solvingTime;
  microsecond_t totalTime;
  microsecond_t updateTime;
  microsecond_t maxBroadPhaseTime;
  microsecond_t maxNarrowPhaseTime;
  microsecond_t maxSolvingTime;
  microsecond_t maxTotalTime;
  microsecond_t maxUpdateTime;

}; // end of class Performance

} // end of namespace OIMO

#endif // end of OIMO_UTIL_PERFORMANCE_H
