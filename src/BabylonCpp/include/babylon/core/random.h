#ifndef BABYLON_CORE_RANDOM_H
#define BABYLON_CORE_RANDOM_H

#include <babylon/babylon_stl.h>
#include <babylon/babylon_stl_util.h>

namespace BABYLON {
namespace Math {

// -- Random number generation, uniformly distributed --
template <typename T>
inline T randomNumber(T min, T max)
{
  if (stl_util::almost_equal(min, max)) {
    return min;
  }

  std::random_device rd;
  std::mt19937 gen(static_cast<int>(rd()));
  std::uniform_real_distribution<T> dis(min, max);
  return dis(gen);
}

template <typename T>
inline std::vector<T> randomList(T min, T max, size_t nbrOfElements = 1)
{
  std::random_device rd;
  std::mt19937 gen(static_cast<int>(rd()));
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

// -- Permuted Congruential Generator --
constexpr auto seed()
{
  std::uint64_t shifted = 0;

  for (const auto c : __TIME__) {
    shifted <<= 8;
    shifted |= c;
  }

  return shifted;
}

struct PCG {
  struct pcg32_random_t {
    std::uint64_t state = 0;
    std::uint64_t inc   = seed();
  };
  pcg32_random_t rng;
  typedef std::uint32_t result_type;

  constexpr result_type operator()()
  {
    return pcg32_random_r();
  }

  static result_type constexpr min()
  {
    return std::numeric_limits<result_type>::min();
  }

  static result_type constexpr max()
  {
    return std::numeric_limits<result_type>::min();
  }

private:
  constexpr std::uint32_t pcg32_random_r()
  {
    std::uint64_t oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    std::uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    std::uint32_t rot        = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
  }
};

template <typename T, typename Gen>
constexpr auto distribution(Gen& g, T min, T max)
{
  const auto range          = max - min + 1;
  const auto bias_remainder = std::numeric_limits<T>::max() % range;
  const auto unbiased_max = std::numeric_limits<T>::max() - bias_remainder - 1;

  auto r = g();
  for (; r > unbiased_max; r = g())
    ;

  return (r % range) + min;
}

} // end of namespace Random
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_RANDOM_H
