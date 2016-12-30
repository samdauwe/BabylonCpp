#ifndef BABYLON_MESH_POLYGONMESH_INDEXED_VECTOR2_H
#define BABYLON_MESH_POLYGONMESH_INDEXED_VECTOR2_H

#include <babylon/babylon_global.h>

#include <babylon/math/vector2.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT IndexedVector2 : public Vector2 {

public:
  IndexedVector2();
  IndexedVector2(const Vector2& original, int index);
  ~IndexedVector2();

public:
  int index;

}; // end of class IndexedVector2

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_POLYGONMESH_INDEXED_VECTOR2_H
