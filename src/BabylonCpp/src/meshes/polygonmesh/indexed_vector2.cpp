#include <babylon/meshes/polygonmesh/indexed_vector2.h>

namespace BABYLON {

IndexedVector2::IndexedVector2() : Vector2(0.f, 0.f), index{0}
{
}

IndexedVector2::IndexedVector2(const Vector2& original, int _index)
    : Vector2(original.x, original.y), index{_index}
{
}

IndexedVector2::IndexedVector2(const IndexedVector2& other) = default;
IndexedVector2::IndexedVector2(IndexedVector2&& other) = default;
IndexedVector2& IndexedVector2::operator=(const IndexedVector2& other) = default;
IndexedVector2& IndexedVector2::operator=(IndexedVector2&& other) = default;
IndexedVector2::~IndexedVector2() = default;

} // end of namespace BABYLON
