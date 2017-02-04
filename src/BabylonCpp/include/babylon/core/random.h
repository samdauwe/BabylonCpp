#ifndef BABYLON_CORE_RANDOM_H
#define BABYLON_CORE_RANDOM_H

#include <babylon/babylon_stl.h>

namespace BABYLON {
namespace Math {

// Random number generation
template <typename T>
inline T randomNumber(T min, T max)
{
  if (std_util::almost_equal(min, max)) {
    return min;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<T> dis(min, max);
  return dis(gen);
}

template <typename T>
inline std::vector<T> randomList(T min, T max, size_t nbrOfElements = 1)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<T> dis(min, max);
  std::vector<T> result;
  const size_t cnt = (nbrOfElements < 1) ? 1 : nbrOfElements;
  for (size_t n = 0; n < cnt; ++n) {
    result.emplace_back(dis(gen));
  }
  return result;
}

inline float random()
{
  return randomNumber(0.f, 1.f);
}

} // end of namespace Random
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_RANDOM_H
