#ifndef BABYLON_MESHES_POLYGONMESH_INDEXED_VECTOR2_H
#define BABYLON_MESHES_POLYGONMESH_INDEXED_VECTOR2_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector2.h>

namespace BABYLON {

/**
 * @brief Vector2 wth index property.
 */
class BABYLON_SHARED_EXPORT IndexedVector2 : public Vector2 {

public:
  IndexedVector2();
  IndexedVector2(const Vector2& original, int index);
  IndexedVector2(const IndexedVector2& other);
  IndexedVector2(IndexedVector2&& other);
  IndexedVector2& operator=(const IndexedVector2& other);
  IndexedVector2& operator=(IndexedVector2&& other);
  ~IndexedVector2(); // = default

public:
  /**
   * Index of the vector2
   */
  int index;

}; // end of class IndexedVector2

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_POLYGONMESH_INDEXED_VECTOR2_H
