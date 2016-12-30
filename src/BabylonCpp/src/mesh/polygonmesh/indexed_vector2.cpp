#include <babylon/mesh/polygonmesh/indexed_vector2.h>

namespace BABYLON {

IndexedVector2::IndexedVector2() : Vector2(0.f, 0.f), index{0}
{
}

IndexedVector2::IndexedVector2(const Vector2& original, int _index)
    : Vector2(original.x, original.y), index{_index}
{
}

IndexedVector2::~IndexedVector2()
{
}

} // end of namespace BABYLON
