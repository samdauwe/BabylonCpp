#ifndef BABYLON_STL_UTIL_H
#define BABYLON_STL_UTIL_H

#include <bitset>
#include <cstring>
#include <functional>
#include <memory>
#include <set>
#include <sstream>
#include <unordered_map>

namespace BABYLON {
namespace stl_util {

// Determines if the given floating point number arg is a not-a-number (NaN)
// value.
#define isNan(a) (a != a)

// -- Implementation of make_unique function in C++11 --
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

using byte = unsigned char;

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

template <typename C, typename T>
std::vector<C> to_array(const std::vector<T> buffer, size_t byteOffset,
                        size_t byteLength)
{
  std::vector<C> bytes(byteLength);
  std::memcpy(bytes.data(), buffer.data() + byteOffset,
              bytes.size() * sizeof(C));
  return bytes;
}

template <typename C, typename T>
std::vector<C> to_array(const std::vector<T> buffer, size_t byteOffset)
{
  return to_array<C>(buffer, byteOffset,
                     (buffer.size() * sizeof(T)) / sizeof(C));
}

template <typename C, typename T>
std::vector<C> to_array(const std::vector<T> buffer)
{
  return to_array<C>(buffer, 0, (buffer.size() * sizeof(T)) / sizeof(C));
}

template <typename C, typename T>
std::vector<C> cast_array_elements(const std::vector<T> buffer)
{
  struct to_result_type {
    float operator()(T value)
    {
      return static_cast<C>(value);
    }
  };

  std::vector<C> result;
  std::transform(buffer.begin(), buffer.end(), back_inserter(result),
                 to_result_type());

  return result;
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
constexpr
  typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
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

// Finding of a minimum and maximum with variadic templates and C++11

template <typename... Ts>
auto min(Ts... ts) -> typename std::common_type<Ts...>::type
{
  return std::min({static_cast<typename std::common_type<Ts...>::type>(ts)...});
}

template <typename... Ts>
auto max(Ts... ts) -> typename std::common_type<Ts...>::type
{
  return std::max({static_cast<typename std::common_type<Ts...>::type>(ts)...});
}

// Three-way comparison

/**
 * @brief Combined Comparison (Spaceship) Operator.
 *
 * Operator (expr) <=> (expr) implementation, it returns 0 if both operands are
 * equal, 1 if the left is greater, and -1 if the right is greater. It uses
 * exactly the same comparison rules as used by our existing comparison
 * operators: <, <=, ==, >= and >.
 *
 * @param lhs Left hand side operand.
 * @param rhs Right hand side operand.
 * @return 0 if values on either side are equal.
 *         1 if value on the left is greater.
 *        -1 if the value on the right is greater.
 */
template <typename T,
          typename
          = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
constexpr int spaceship(const T& lhs, const T& rhs)
{
  return (lhs < rhs) ? -1 : (rhs < lhs) ? 1 : 0;
}

/**
 * @brief Combined Comparison (Spaceship) Operator for std::string.
 */
inline int spaceship(const std::string& lhs, const std::string& rhs)
{
  const int comparison = lhs.compare(rhs);
  return (comparison < 0) ? -1 : (comparison > 0) ? 1 : 0;
}

// Container functions

template <typename T, typename... Ts>
inline std::vector<T> to_vector(const T& t0, const Ts&... ts)
{
  return std::vector<T>{t0, ts...};
}

template <typename T>
inline std::vector<T*>
to_raw_ptr_vector(const std::vector<std::unique_ptr<T>>& c)
{
  std::vector<T*> result;
  result.reserve(c.size());
  std::transform(c.begin(), c.end(), std::back_inserter(result),
                 [](const std::unique_ptr<T>& ci) { return ci.get(); });
  return result;
}

template <typename T>
inline std::vector<T*>
to_raw_ptr_vector(const std::vector<std::shared_ptr<T>>& c)
{
  std::vector<T*> result;
  result.reserve(c.size());
  std::transform(c.begin(), c.end(), std::back_inserter(result),
                 [](const std::shared_ptr<T>& ci) { return ci.get(); });
  return result;
}

template <typename K, typename V>
inline std::unordered_map<K, V*>
to_raw_ptr_map(const std::unordered_map<K, std::unique_ptr<V>>& c)
{
  std::unordered_map<K, V*> result;
  result.reserve(c.size());
  for (const auto& ci : c) {
    result[ci.first] = ci.second.get();
  }
  return result;
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

// Sequence (i.e. std::vector or std::list)
template <template <typename, typename> class C, template <typename> class A,
          typename T, typename V>
constexpr bool contains(const C<T, A<T>>& c, const V& elem)
{
  return std::find(c.begin(), c.end(), T(elem)) != c.end();
}

// Set
template <typename C, typename A>
constexpr bool contains(const std::set<C>& c, const A& elem)
{
  return c.find(C(elem)) != c.end();
}

// Unordered Map
template <typename C, typename A, typename T>
constexpr bool contains(const std::unordered_map<C, T>& c, const A& elem)
{
  return c.find(C(elem)) != c.end();
}

template <typename C, typename T>
inline C& insert_at(C& c, size_t pIndex, const T& elem)
{
  c.insert(c.begin() + pIndex, elem);

  return c;
}

template <typename K, typename V>
inline std::vector<K> extract_keys(std::unordered_map<K, V> const& inputMap)
{
  std::vector<K> keys;
  keys.reserve(inputMap.size());
  for (auto const& element : inputMap) {
    keys.emplace_back(element.first);
  }
  return keys;
}

template <typename K, typename V>
inline std::vector<V> extract_values(std::unordered_map<K, V> const& inputMap)
{
  std::vector<V> values;
  values.reserve(inputMap.size());
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
 * @brief Removes duplicate elements from std::vector while maintaining order.
 * @param v The std::vector to make unique while maintaining order.
 * @returns The std::vector having no duplicate item.
 */
template <typename T>
std::vector<T> remove_duplicates(const std::vector<T>& v)
{
  std::set<T> tmpSet;
  std::vector<T> res; // Resultant vector

  std::remove_copy_if(v.begin(), v.end(), std::back_inserter(res),
                      [&tmpSet](T x) { return !tmpSet.insert(x).second; });

  return res;
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
inline int index_of_raw_ptr(C& c, T* elem)
{
  auto i = std::find_if(
    c.begin(), c.end(),
    [&elem](const std::shared_ptr<T>& _elem) { return _elem.get() == elem; });
  if (i != c.end()) {
    return static_cast<int>(i - c.begin());
  }
  return -1;
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
inline int index_of_ptr(const std::vector<std::shared_ptr<C>>& c, T* elem)
{
  auto i
    = std::find_if(c.begin(), c.end(), [elem](const std::shared_ptr<C>& cElem) {
        return cElem.get() == elem;
      });
  if (i != c.end()) {
    return static_cast<int>(i - c.begin());
  }
  return -1;
}

/**
 * @brief Filters a vector according to the specified predicate.
 * @param original Vector to filter.
 * @param pred Unary predicate which returns ​true for the required elements.
 * @return A vector, containing the filtered values.
 */
template <typename T, class UnaryPredicate>
inline std::vector<T> filter(const std::vector<T>& original,
                             UnaryPredicate pred)
{
  std::vector<T> filtered;
  std::copy_if(original.begin(), original.end(), std::back_inserter(filtered),
               pred);
  return filtered;
}

/**
 * @brief Applies an operation sequentially to the elements of the input vector
 * using the mapping function.
 * @param original Vector to map.
 * @param mappingFunction Unary mapping function.
 * @return A vector, containing the mapped values.
 */
template <typename T, class UnaryOperation>
inline std::vector<T> map(const std::vector<T>& original,
                          UnaryOperation mappingFunction)
{
  std::vector<T> mapped;
  std::transform(original.begin(), original.end(), std::back_inserter(mapped),
                 mappingFunction);
  return mapped;
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

template <typename C, typename P>
inline void remove_if(C& c, const P& p)
{
  c.erase(std::remove_if(std::begin(c), std::end(c), p), std::end(c));
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

} // end of namespace stl_util
} // end of namespace BABYLON

#endif // end of BABYLON_STL_UTIL_H
