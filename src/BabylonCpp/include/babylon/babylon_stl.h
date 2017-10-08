#ifndef BABYLON_STL_H
#define BABYLON_STL_H

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

// Algorithm
#include <algorithm>

// Bitset
#include <bitset>

// C Standard General Utilities Library
#include <stdlib.h>

// Condition variable
#include <condition_variable>

// Containers library
#include <array>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Dynamic memory management
#include <memory>
#include <new>

// Error handling
#include <exception>

// Input/output library
#include <fstream>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <sstream>

// Numerics library
#include <cmath>
#include <numeric>

// Strings library
#include <string>

// Function objects
#include <functional>

// Pseudo-random number generation
#include <random>

// Regular expressions library
#include <regex>

// Thread support
#include <future>
#include <mutex>
#include <thread>

// Time functions
#include <chrono>
#include <ctime>

// Type Traits
#include <type_traits>

// Typedefs for types such as size_t, NULL and others
#include <cstddef>

// Utilities library
#include <utility>

// Function objects
#include <babylon/core/delegates/delegate.h>

namespace BABYLON {

// -- Type support -- //
// Basic types
using int8_t   = ::std::int8_t;
using uint8_t  = ::std::uint8_t;
using int16_t  = ::std::int16_t;
using uint16_t = ::std::uint16_t;
using int32_t  = ::std::int32_t;
using uint32_t = ::std::uint32_t;
using uint64_t = ::std::uint64_t;
using float_t  = ::std::float_t;
using double_t = ::std::double_t;
using size_t   = ::std::size_t;

template <class T>
using numeric_limits_t = ::std::numeric_limits<T>;

// Time types
using millisecond_t
  = ::std::chrono::duration<std::uint64_t, std::ratio<1, 1000>>;
using milliseconds_t = ::std::chrono::milliseconds;
using microsecond_t
  = std::chrono::duration<std::uint64_t, std::ratio<1, 1000000>>;
using microseconds_t        = ::std::chrono::microseconds;
using high_res_clock_t      = ::std::chrono::high_resolution_clock;
using high_res_time_point_t = ::std::chrono::time_point<high_res_clock_t>;
using system_clock_t        = ::std::chrono::system_clock;
using system_time_point_t   = ::std::chrono::time_point<system_clock_t>;

// Byte type
using byte = unsigned char;

// -- Array types -- //
// 8-bit twos complement signed integer
using Int8Array = ::std::vector<int8_t>;
// 8-bit unsigned integer
using Uint8Array = ::std::vector<uint8_t>;
// 16-bit twos complement signed integer
using Int16Array = ::std::vector<int16_t>;
// 16-bit unsigned integer
using Uint16Array = ::std::vector<uint16_t>;
// 32-bit twos complement signed integer
using Int32Array = ::std::vector<int32_t>;
// 32-bit unsigned integer
using Uint32Array = ::std::vector<uint32_t>;
// 32-bit IEEE floating point number
using Float32Array = ::std::vector<float_t>;
// 64-bit IEEE floating point number
using Float64Array = ::std::vector<double_t>;

// Indices array alias
using IndicesArray = ::std::vector<std::uint32_t>;

// ArrayBuffer
using ArrayBuffer = ::std::vector<uint8_t>;

// -- Compile time rational arithmetic -- //
template <::std::intmax_t Num, ::std::intmax_t Denom = 1>
using ratio_t = ::std::ratio<Num, Denom>;

// -- Containers library -- //
template <class T, ::std::size_t N>
using array_t = ::std::array<T, N>;

template <class Key,                        // map::key_type
          class T,                          // map::mapped_type
          class Compare = ::std::less<Key>, // map::key_compare
          class Alloc
          = ::std::allocator<::std::pair<const Key, T>> // map::allocator_type
          >
using map_t = ::std::map<Key, T, Compare, Alloc>;

template <class T, class Container = ::std::deque<T>>
using queue_t = ::std::queue<T, Container>;

template <class Key,                         // unordered_map::key_type
          class T,                           // unordered_map::mapped_type
          class Hash = ::std::hash<Key>,     // unordered_map::hasher
          class Pred = ::std::equal_to<Key>, // unordered_map::key_equal
          class Alloc
          = ::std::allocator<::std::pair<const Key,
                                         T>> // unordered_map::allocator_type
          >
using unordered_map_t = ::std::unordered_map<Key, T, Hash, Pred, Alloc>;

template <class T, class Allocator = ::std::allocator<T>>
using vector_t = ::std::vector<T, Allocator>;

// -- Date and time utilities -- //
namespace chrono {
template <class Rep, class Period = ::std::ratio<1>>
using duration_t = ::std::chrono::duration<Rep, Period>;

template <class Clock, class Duration = typename Clock::duration>
using time_point_t = ::std::chrono::time_point<Clock, Duration>;

using milliseconds_t = ::std::chrono::milliseconds;
using microseconds_t = ::std::chrono::microseconds;

using high_resolution_clock = ::std::chrono::high_resolution_clock;
using system_clock_t        = ::std::chrono::system_clock;
} // end of namespace chrono

// -- Dynamic memory management -- //
template <class T, class Deleter = ::std::default_delete<T>>
using unique_ptr_t = ::std::unique_ptr<T, Deleter>;

template <class T>
using shared_ptr_t = ::std::shared_ptr<T>;

// -- Input/output library -- //
using ostream_t       = ::std::basic_ostream<char>;
using ostringstream_t = ::std::basic_ostringstream<char>;

// -- Strings library -- //
template <class charT,
          class traits = ::std::char_traits<charT>, // basic_string::traits_type
          class Alloc  = ::std::allocator<charT> // basic_string::allocator_type
          >
using basic_string_t = ::std::basic_string<charT, traits, Alloc>;

using string_t = ::std::basic_string<char>;

// -- Thread support library -- //
using condition_variable_t = ::std::condition_variable;
using mutex_t              = ::std::mutex;
using thread_t             = ::std::thread;

// -- Utilities library -- //
template <size_t N>
using bitset_t = ::std::bitset<N>;

using nullptr_t = ::std::nullptr_t;

template <class T1, class T2>
using pair_t = ::std::pair<T1, T2>;

// Function objects
template <typename T>
using delegate_t = SA::delegate<T>;
template <typename T>
using multicast_delegate_t = SA::multicast_delegate<T>;

const delegate_t<void()> nullptr_void_deletegate_t;

template <class Function>
struct Comparator {
  bool operator()(const Function& f1, const Function& f2) const
  {
    auto ptr1 = f1.template target<Function>();
    auto ptr2 = f2.template target<Function>();
    return ptr1 < ptr2;
  }
};

} // end of namespace BABYLON

#endif // end of BABYLON_STL_H
