#include <babylon/mesh/polygonmesh/indexed_vector2.h>

namespace BABYLON {

IndexedVector2::IndexedVector2() : Vector2(0.f, 0.f), index{0}
{
}

IndexedVector2::IndexedVector2(const Vector2& original, int _index)
    : Vector2(original.x, original.y), index{_index}
{
}

IndexedVector2::IndexedVector2(const IndexedVector2& other)
    : Vector2(other), index{other.index}
{
}

IndexedVector2::IndexedVector2(IndexedVector2&& other)
    : Vector2(::std::move(other)), index{::std::move(other.index)}
{
}

IndexedVector2& IndexedVector2::operator=(const IndexedVector2& other)
{
  if (&other != this) {
    Vector2::operator=(other);
    index            = other.index;
  }

  return *this;
}

IndexedVector2& IndexedVector2::operator=(IndexedVector2&& other)
{
  if (&other != this) {
    Vector2::operator=(::std::move(other));
    index            = ::std::move(other.index);
  }

  return *this;
}

IndexedVector2::~IndexedVector2()
{
}

} // end of namespace BABYLON
