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

namespace BABYLON {

// Common types
using millisecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
using milliseconds_t = std::chrono::milliseconds;
using microsecond_t  = std::chrono::duration<uint64_t, std::ratio<1, 1000000>>;
using microseconds_t = std::chrono::microseconds;
using high_res_clock_t      = std::chrono::high_resolution_clock;
using high_res_time_point_t = std::chrono::time_point<high_res_clock_t>;
using system_clock_t        = std::chrono::system_clock;
using system_time_point_t   = std::chrono::time_point<system_clock_t>;

// byte type
using byte = unsigned char;

// 32 bit (4 byte) floating point number array
using Float32Array = std::vector<std::float_t>;
using Int8Array    = std::vector<std::int8_t>;
using Int16Array   = std::vector<std::int16_t>;
using Int32Array   = std::vector<std::int32_t>;
using Uint8Array   = std::vector<std::uint8_t>;
using Uint16Array  = std::vector<std::uint16_t>;
using Uint32Array  = std::vector<std::uint32_t>;

// Indices array alias
using IndicesArray = Uint32Array;

} // end of namespace BABYLON

#endif // end of BABYLON_STL_H
