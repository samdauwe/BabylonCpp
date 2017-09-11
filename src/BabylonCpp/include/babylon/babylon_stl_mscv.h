#ifndef BABYLON_STL_MSCV_H
#define BABYLON_STL_MSCV_H

namespace BABYLON {
namespace std {

/** Type support **/
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
using numeric_limits = ::std::numeric_limits<T>;

/** Numerics library **/

/** Compile time rational arithmetic **/
template <::std::intmax_t Num, ::std::intmax_t Denom = 1>
using ratio = ::std::ratio<Num, Denom>;

/** Date and time utilities **/
namespace chrono {
template <class Rep, class Period = ::std::ratio<1>>
using duration = ::std::chrono::duration<Rep, Period>;

template <class Clock, class Duration = typename Clock::duration>
using time_point = ::std::chrono::time_point<Clock, Duration>;

using milliseconds = ::std::chrono::milliseconds;
using microseconds = ::std::chrono::microseconds;

using high_resolution_clock = ::std::chrono::high_resolution_clock;
using system_clock          = ::std::chrono::system_clock;
} // end of namespace chrono

/** Containers library **/
template <class T, std::size_t N>
using array = ::std::array<T, N>;

template <class T, class Allocator = ::std::allocator<T>>
using vector = ::std::vector<T, Allocator>;

template <class Key,                          // unordered_map::key_type
          class T,                            // unordered_map::mapped_type
          class Hash  = ::std::hash<Key>,     // unordered_map::hasher
          class Pred  = ::std::equal_to<Key>, // unordered_map::key_equal
          class Alloc = ::std::allocator<
            ::std::pair<const Key, T>> // unordered_map::allocator_type
          >
using unordered_map = ::std::unordered_map<Key, T, Hash, Pred, Alloc>;

/** Dynamic memory management **/
template <class T, class Deleter = ::std::default_delete<T>>
using unique_ptr = ::std::unique_ptr<T, Deleter>;

template <class T>
using shared_ptr = ::std::shared_ptr<T>;

/** Input/output library **/
using ostream       = ::std::basic_ostream<char>;
using ostringstream = ::std::basic_ostringstream<char>;

/** Strings library **/
template <class charT,
          class traits = ::std::char_traits<charT>, // basic_string::traits_type
          class Alloc  = ::std::allocator<charT> // basic_string::allocator_type
          >
using basic_string = ::std::basic_string<charT, traits, Alloc>;

using string = ::std::basic_string<char>;

/**  Utilities library **/
template <std::size_t N>
using bitset = ::std::bitset<N>;

} // end of namespace std
} // end of namespace BABYLON

#endif // end of BABYLON_STL_MSCV_H