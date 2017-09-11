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

// Fix for namespace uses when using stl
#if _MSC_VER && !__INTEL_COMPILER
#include <babylon/babylon_stl_mscv.h>
#endif

namespace BABYLON {

// Common types
using millisecond_t = std::chrono::duration<std::uint64_t, std::ratio<1, 1000>>;
using milliseconds_t = std::chrono::milliseconds;
using microsecond_t
  = std::chrono::duration<std::uint64_t, std::ratio<1, 1000000>>;
using microseconds_t        = std::chrono::microseconds;
using high_res_clock_t      = std::chrono::high_resolution_clock;
using high_res_time_point_t = std::chrono::time_point<high_res_clock_t>;
using system_clock_t        = std::chrono::system_clock;
using system_time_point_t   = std::chrono::time_point<system_clock_t>;

// byte type
using byte = unsigned char;

// -- Array types -- //
// 8-bit twos complement signed integer
using Int8Array = std::vector<std::int8_t>;
// 8-bit unsigned integer
using Uint8Array = std::vector<std::uint8_t>;
// 16-bit twos complement signed integer
using Int16Array = std::vector<std::int16_t>;
// 16-bit unsigned integer
using Uint16Array = std::vector<std::uint16_t>;
// 32-bit twos complement signed integer
using Int32Array = std::vector<std::int32_t>;
// 32-bit unsigned integer
using Uint32Array = std::vector<std::uint32_t>;
// 32-bit IEEE floating point number
using Float32Array = std::vector<std::float_t>;
// 64-bit IEEE floating point number
using Float64Array = std::vector<std::double_t>;

// Indices array alias
using IndicesArray = std::vector<std::uint32_t>;

} // end of namespace BABYLON

#endif // end of BABYLON_STL_H
