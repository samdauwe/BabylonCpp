#ifndef BABYLON_CORE_uuid_H
#define BABYLON_CORE_uuid_H

#include <babylon/babylon_stl.h>

namespace BABYLON {

/**
 * @brief A universally unique identifier (uuid).
 */
class uuid {

public:
  using value_type      = uint8_t;
  using reference       = value_type&;
  using const_reference = value_type const&;
  using iterator        = value_type*;
  using const_iterator  = value_type const*;
  using size_type       = size_t;

  static uuid random();
  static uuid nil();

  uuid() = default;

  reference operator[](size_t i);
  const_reference operator[](size_t i) const;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  size_type size() const;

  void swap(uuid& other);

  friend bool operator==(uuid const& x, uuid const& y);
  friend bool operator<(uuid const& x, uuid const& y);

private:
  std::array<value_type, 16> id_;
};

} // end of namespace BABYLON

// TODO: express in terms of hashable concept. This means simply hashing the
// bytes of the entire std::array.
namespace std {

template <>
struct hash<BABYLON::uuid> {
  size_t operator()(BABYLON::uuid const& u) const
  {
    size_t x = 0;
    for (auto& byte : u)
      x ^= static_cast<size_t>(byte) + 0x9e3779b9 + (x << 6) + (x >> 2);
    return x;
  }
};

} // end of namespace std

#endif // end of BABYLON_CORE_uuid_H
