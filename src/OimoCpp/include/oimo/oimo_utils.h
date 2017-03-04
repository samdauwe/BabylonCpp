#ifndef OIMO_UTILS_H
#define OIMO_UTILS_H

// Time functions
#include <chrono>
#include <cmath>
#include <ctime>
#include <memory>

namespace OIMO {

// -- Implementation of std::make_unique function in C++11 --
template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args)
{
  static_assert(
    std::extent<T>::value == 0,
    "make_unique<T[N]>() is forbidden, please use make_unique<T[]>(),");
  typedef typename std::remove_extent<T>::type U;
  return std::unique_ptr<T>(
    new U[sizeof...(Args)]{std::forward<Args>(args)...});
}

template <typename T, typename... Args>
inline std::unique_ptr<T> make_unique(Args&&... args)
{
  return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}

// Comparison function

inline bool floats_are_equal(float x, float y, int ulp = 4)
{
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::abs(x - y) < std::numeric_limits<float>::epsilon()
                             * std::abs(x + y) * static_cast<float>(ulp)
         // unless the result is subnormal
         || std::abs(x - y) < std::numeric_limits<float>::min();
}

// Math functions

inline float clamp(float x)
{
  return std::min(std::max(x, -1.f), 1.f);
}

inline float clamp(float x, float a, float b)
{
  return (x < a) ? a : ((x > b) ? b : x);
}

// Time functions

// Common types
using millisecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
using milliseconds_t = std::chrono::milliseconds;
using microsecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000000>>;
using microseconds_t = std::chrono::microseconds;
using high_res_clock_t      = std::chrono::high_resolution_clock;
using high_res_time_point_t = std::chrono::time_point<high_res_clock_t>;
using system_clock_t        = std::chrono::system_clock;
using system_time_point_t   = std::chrono::time_point<system_clock_t>;

inline microseconds_t
microseconds_since(const high_res_time_point_t& high_res_timepoint)
{
  return (std::chrono::duration_cast<microseconds_t>(
    std::chrono::high_resolution_clock::now() - high_res_timepoint));
}

} // end of namespace OIMO

#endif // end of OIMO_UTILS_H
