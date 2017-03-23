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

namespace std_util {

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

// Bitsets

/**
 * Float (IEEE754 Single precision 32-bit) to bitset conversion.
 *
 * x xxxxxxxx xxxxxxxxxxxxxxxxxxxxxxx
 * |     |              |- Mantisse(23 bits)
 * |     |- Exponent (8 bits)
 * |- Sign (1 bit)
 */
inline std::bitset<32> to_bitset(float number)
{
  std::bitset<32> result;
  if (sizeof(number) != 4) {
    return result;
  }

  char* bits = reinterpret_cast<char*>(&number);
  for (std::size_t n = 0; n < sizeof(number); ++n) {
    std::bitset<8> octet(static_cast<size_t>(bits[n]));
    for (unsigned int i = 0; i < 8; ++i) {
      result[n * 8 + i] = octet[i];
    }
  }
  return result;
}

inline std::string to_hex_string(const std::string& bit_string)
{
  std::ostringstream oss;
  oss << "0x" << std::hex;
  for (size_t i = 0, idx = 0, ul = bit_string.size() / 4; i < ul;
       ++i, idx += 4) {
    std::bitset<4> nibble(bit_string.substr(idx, idx + 4));
    oss << nibble.to_ulong();
  }
  return oss.str();
}

inline unsigned int hex_string_to_uint(const std::string& hex_string)
{
  return static_cast<unsigned int>(std::stoul(hex_string, nullptr, 16));
}

inline float hex_to_float(unsigned int hex_value)
{
  union IntFloat {
    uint32_t i;
    float f;
  };
  union IntFloat val;
  val.i = hex_value;
  return val.f;
}

inline float hex_to_float(const std::string& hex_string)
{
  return hex_to_float(hex_string_to_uint(hex_string));
}

// byte conversion

template <typename T>
std::array<byte, sizeof(T)> to_bytes(const T& object)
{
  std::array<byte, sizeof(T)> bytes;

  const byte* begin = reinterpret_cast<const byte*>(std::addressof(object));
  const byte* end   = begin + sizeof(T);
  std::copy(begin, end, std::begin(bytes));

  return bytes;
}

template <typename T>
T& from_bytes(const std::array<byte, sizeof(T)>& bytes, T& object)
{
  // http://en.cppreference.com/w/cpp/types/is_trivially_copyable
  static_assert(std::is_trivially_copyable<T>::value,
                "not a TriviallyCopyable type");

  byte* begin_object = reinterpret_cast<byte*>(std::addressof(object));
  std::copy(std::begin(bytes), std::end(bytes), begin_object);

  return object;
}

// -- String helper functions --

// Compares two strings in compile time constant fashion
constexpr int c_strcmp(char const* lhs, char const* rhs)
{
  return (('\0' == lhs[0]) && ('\0' == rhs[0])) ?
           0 :
           (lhs[0] != rhs[0]) ? (lhs[0] - rhs[0]) : c_strcmp(lhs + 1, rhs + 1);
}

// -- Helper Templates --

// Floating point comparison

template <class T>
inline typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp = 4)
{
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::abs(x - y) < std::numeric_limits<T>::epsilon() * std::abs(x + y)
                             * static_cast<T>(ulp)
         // unless the result is subnormal
         || std::abs(x - y) < std::numeric_limits<T>::min();
}

// Used for lambda comparison

template <typename T, typename... U>
size_t get_address(std::function<T(U...)> f)
{
  typedef T(fnType)(U...);
  fnType** fnPointer = f.template target<fnType*>();
  return reinterpret_cast<size_t>(*fnPointer);
}

// Container functions

template <typename T, typename... Ts>
inline std::vector<T> to_vector(const T& t0, const Ts&... ts)
{
  return std::vector<T>{t0, ts...};
}

/**
 * The concat() method is used to join two or more arrays.
 * Note: This method does not change the existing arrays, but returns a new
 * array, containing the values of the joined arrays.
 * @param a Required. The first array to be joined.
 * @param b Required. The first array to be joined.
 * @return An Array object, representing the joined array.
 */
template <typename C>
inline C& concat(C& a, const C& b)
{
  a.insert(a.end(), b.begin(), b.end());
  return a;
}

template <typename C>
inline C& concat_with_no_duplicates(C& a, const C& b)
{
  // Concatenate the lists
  concat(a, b);

  // Remove duplicates
  std::sort(a.begin(), a.end());
  a.erase(std::unique(a.begin(), a.end()), a.end());

  return a;
}

template <typename C, typename T>
inline bool contains(const C& c, const T& elem)
{
  return std::find(c.begin(), c.end(), elem) != c.end();
}

template <typename C, typename T>
inline C& insert_at(C& c, size_t pIndex, const T& elem)
{
  c.insert(c.begin() + pIndex, elem);

  return c;
}

template <typename T>
inline bool contains(const std::unordered_map<std::string, T>& c,
                     const std::string& elem)
{
  return c.find(elem) != c.end();
}

template <typename T>
inline bool contains(const std::unordered_map<std::string, T>& c,
                     const char* elem)
{
  return contains(c, std::string(elem));
}

template <typename K, typename V>
inline std::vector<K> extract_keys(std::unordered_map<K, V> const& inputMap)
{
  std::vector<K> keys;
  for (auto const& element : inputMap) {
    keys.emplace_back(element.first);
  }
  return keys;
}

template <typename K, typename V>
inline std::vector<V> extract_values(std::unordered_map<K, V> const& inputMap)
{
  std::vector<V> values;
  for (auto const& element : inputMap) {
    values.emplace_back(element.second);
  }
  return values;
}

template <typename C, typename T>
inline bool erase(C& c, const T& elem)
{
  auto i = std::find(c.begin(), c.end(), elem);
  if (i != c.end()) {
    c.erase(i);
    return true;
  }
  return false;
}

template <typename C, typename FwdIt, typename P>
void erase_if(C& container, FwdIt it, const FwdIt end, P predicate)
{
  while (it != end) {
    if (predicate(*it)) {
      container.erase(it++);
    }
    else {
      ++it;
    }
  }
}

template <typename C, typename P>
void erase_if(C& container, P predicate)
{
  erase_if(container, container.begin(), container.end(), predicate);
}

/**
 * Searches the array for the specified item, and returns its position.
 * Returns -1 if the item is not found.
 * If the item is present more than once, the indexOf method returns the
 * position of the first occurence.
 * @param c Required. The container to search in.
 * @param elem Required. The item to search for.
 * @return A Number, representing the position of the specified item, otherwise
 * -1
 */
template <typename C, typename T>
inline int index_of(C& c, const T& elem)
{
  auto i = std::find(c.begin(), c.end(), elem);
  if (i != c.end()) {
    return static_cast<int>(i - c.begin());
  }
  return -1;
}

/**
 * Returns the selected elements in an array, as a new array object.
 * The slice() method selects the elements starting at the given start argument,
 * and ends at, but does not include, the given end argument.
 * Note: The original array will not be changed.
 * @param start Optional. An integer that specifies where to start the selection
 * (The first element has an index of 0). Use negative numbers to select from
 * the end of an array. If omitted, it acts like 0.
 * @param end   Optional. An integer that specifies where to end the
 * selection. If omitted, all elements from the start position and to the end of
 * the array will be selected. Use negative numbers to select from the end of an
 * array.
 * @return A new Array, containing the selected elements.
 */
template <typename T>
inline std::vector<T> slice(const std::vector<T>& v, int start = 0,
                            int end = -1)
{
  if (end < 0) {
    return std::vector<T>(v.begin() + start, v.end());
  }

  if (start > end) {
    return std::vector<T>(v.begin() + end, v.begin() + start);
  }

  return std::vector<T>(v.begin() + (start < 0 ? 0 : start), v.begin() + end);
}

/**
 * Selects the elements starting at the given start argument, and ends at, but
 * does not include, the given end argument.The original array will not be
 * changed by the selected array.
 * @param start Optional. An integer that specifies where to start the selection
 * (The first element has an index of 0). Use negative numbers to select from
 * the end of an array. If omitted, it acts like 0.
 * @param end   Optional. An integer that specifies where to end the
 * selection. If omitted, all elements from the start position and to the end of
 * the array will be selected. Use negative numbers to select from the end of an
 * array.
 * @return Nothing.
 */
template <typename T>
inline void slice_in_place(std::vector<T>& v, int start = 0, int end = -1)
{
  if (end < 0) {
    std::vector<T>(v.begin() + start, v.end()).swap(v);
  }
  else if (start > end) {
    std::vector<T>(v.begin() + end, v.begin() + start).swap(v);
  }
  else {
    std::vector<T>(v.begin() + (start < 0 ? 0 : start), v.begin() + end)
      .swap(v);
  }
}

/**
 * Adds/removes items to/from an array, and returns the removed item(s).
 * @param index  Required. An integer that specifies at what position to
 * add/remove items, Use negative values to specify the position from the end of
 * the array.
 * @param howmany  Required. The number of items to be removed. If set to 0, no
 * items will be removed.
 * @param itemsToAdd  Optional. The new item(s) to be added to the array
 * @return A new Array, containing the removed items (if any)
 */
template <typename T>
inline std::vector<T> splice(std::vector<T>& v, int index = 0, int howmany = 0,
                             const std::vector<T>& itemsToAdd
                             = std::vector<T>())
{
  if (index < 0) {
    const auto start = std::max(v.begin(), v.end() + index);
    const auto end   = std::min(v.end(), start + howmany);
    std::vector<T> removedItems(start, end);
    v.erase(start, end);
    v.insert(end - static_cast<int>(removedItems.size()), itemsToAdd.begin(),
             itemsToAdd.end());
    return removedItems;
  }
  else {
    const auto start = std::min(v.end(), v.begin() + index);
    const auto end   = std::min(v.end(), start + howmany);
    std::vector<T> removedItems(start, end);
    v.erase(start, end);
    v.insert(end - static_cast<int>(removedItems.size()), itemsToAdd.begin(),
             itemsToAdd.end());
    return removedItems;
  }
}

template <typename T>
inline void remove(std::vector<T>& vec, size_t pos)
{
  vec.erase(vec.begin() + pos);
}

template <typename C, typename T>
inline bool push_unique(C& c, T&& elem)
{
  if (!contains(c, elem)) {
    c.push_back(std::move(elem));
    return true;
  }
  return false;
}

class range {
public:
  class iterator {
    friend class range;

  public:
    std::size_t operator*() const
    {
      return i_;
    }
    const iterator& operator++()
    {
      ++i_;
      return *this;
    }
    iterator operator++(int)
    {
      iterator copy(*this);
      ++i_;
      return copy;
    }

    bool operator==(const iterator& other) const
    {
      return i_ == other.i_;
    }
    bool operator!=(const iterator& other) const
    {
      return i_ != other.i_;
    }

  protected:
    iterator(std::size_t start) : i_(start)
    {
    }

  private:
    std::size_t i_;
  };

  iterator begin() const
  {
    return begin_;
  }
  iterator end() const
  {
    return end_;
  }
  range(std::size_t begin, std::size_t end) : begin_(begin), end_(end)
  {
  }

private:
  iterator begin_;
  iterator end_;
};

} // end of namespace std_util

// Bugfix for Visual Studio 2013 which does not handle well
// std::packaged_task<void()>.
// Ref: workarounds at
// http://connect.microsoft.com/VisualStudio/feedback/details/791185/std-packaged-task-t-where-t-is-void-or-a-reference-class-are-not-movable

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))                  \
  && !defined(__MINGW32__)
namespace std {

template <class... _ArgTypes>
class packaged_task<void(_ArgTypes...)> {
  promise<void> _my_promise;
  function<void(_ArgTypes...)> _my_func;

public:
  packaged_task()
  {
  }

  template <class _Fty2>
  explicit packaged_task(_Fty2&& _Fnarg) : _my_func(_Fnarg)
  {
  }

  packaged_task(packaged_task&& _Other)
      : _my_promise(move(_Other._my_promise)), _my_func(move(_Other._my_func))
  {
  }

  packaged_task& operator=(packaged_task&& _Other)
  {
    _my_promise = move(_Other._my_promise);
    _my_func    = move(_Other._my_func);
    return (*this);
  }

  packaged_task(const packaged_task&) = delete;
  packaged_task& operator=(const packaged_task&) = delete;

  ~packaged_task()
  {
  }

  void swap(packaged_task& _Other)
  {
    swap(_my_promise, _Other._my_promise);
    swap(_my_func, _Other._my_func);
  }

  explicit operator bool() const
  {
    return _my_func != false;
  }

  bool valid() const
  {
    return _my_func != false;
  }

  future<void> get_future()
  {
    return _my_promise.get_future();
  }

  void operator()(_ArgTypes... _Args)
  {
    _my_func(forward<_ArgTypes>(_Args)...);
    _my_promise.set_value();
  }

  void reset()
  {
    _my_promise.swap(promise<void>());
    _my_func.swap(function<void(_ArgTypes...)>());
  }
};

};     // namespace std
#endif // defined(WIN32) ...

} // end of namespace BABYLON

#endif // end of BABYLON_STL_H
