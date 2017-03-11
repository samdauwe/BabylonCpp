#include <oimo/util/performance.h>

#include <oimo/dynamics/world.h>
#include <oimo/oimo_version.h>

namespace OIMO {

const Performance::milliseconds_t Performance::ONE_SECOND
  = milliseconds_t(1000);

Performance::Performance(World* world)
    : parent{world}
    , broadPhase{world->broadPhaseType}
    , version{OIMOCPP_VERSION}
    , fps{0.f}
    , fpsTmp{0.f}
    , tt{0}
    , numRigidBodies{0}
    , numContacts{0}
    , numContactPoints{0}
    , numPairChecks{0}
    , numIslands{0}
    , broadPhaseTime{microsecond_t(0)}
    , narrowPhaseTime{microsecond_t(0)}
    , solvingTime{microsecond_t(0)}
    , totalTime{microsecond_t(0)}
    , updateTime{microsecond_t(0)}
    , maxBroadPhaseTime{microsecond_t(0)}
    , maxNarrowPhaseTime{microsecond_t(0)}
    , maxSolvingTime{microsecond_t(0)}
    , maxTotalTime{microsecond_t(0)}
    , maxUpdateTime{microsecond_t(0)}
{
}

Performance::~Performance()
{
}

void Performance::setTime(unsigned int n)
{
  times[n] = high_res_clock_t::now();
}

void Performance::resetMax()
{
  maxBroadPhaseTime  = microsecond_t(0);
  maxNarrowPhaseTime = microsecond_t(0);
  maxSolvingTime     = microsecond_t(0);
  maxTotalTime       = microsecond_t(0);
  maxUpdateTime      = microsecond_t(0);
}

void Performance::calcBroadPhase()
{
  setTime(2);
  broadPhaseTime
    = std::chrono::duration_cast<microseconds_t>(times[2] - times[1]);
}

void Performance::calcNarrowPhase()
{
  setTime(3);
  narrowPhaseTime
    = std::chrono::duration_cast<microseconds_t>(times[3] - times[2]);
}

void Performance::calcEnd()
{
  setTime(2);
  solvingTime = std::chrono::duration_cast<microseconds_t>(times[2] - times[1]);
  totalTime   = std::chrono::duration_cast<microseconds_t>(times[2] - times[0]);
  updateTime  = totalTime - (broadPhaseTime + narrowPhaseTime + solvingTime);

  if (tt == 100) {
    resetMax();
  }

  if (tt > 100) {
    if (broadPhaseTime > maxBroadPhaseTime) {
      maxBroadPhaseTime = broadPhaseTime;
    }
    if (narrowPhaseTime > maxNarrowPhaseTime) {
      maxNarrowPhaseTime = narrowPhaseTime;
    }
    if (solvingTime > maxSolvingTime) {
      maxSolvingTime = solvingTime;
    }
    if (totalTime > maxTotalTime) {
      maxTotalTime = totalTime;
    }
    if (updateTime > maxUpdateTime) {
      maxUpdateTime = updateTime;
    }
  }

  upfps();

  ++tt;
  if (tt > 500) {
    tt = 0;
  };
}

void Performance::upfps()
{
  f[1]      = std::chrono::high_resolution_clock::now();
  auto diff = std::chrono::duration_cast<milliseconds_t>(f[1] - f[0]);
  if (diff > ONE_SECOND) {
    f[0]   = f[1];
    fps    = fpsTmp;
    fpsTmp = 0;
  }
  ++fpsTmp;
}

} // end of namespace OIMO
