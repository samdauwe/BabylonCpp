#ifndef BABYLON_MESH_PRIMITIVES_RIBBON_H
#define BABYLON_MESH_PRIMITIVES_RIBBON_H

#include <babylon/babylon_global.h>

#include <babylon/math/vector3.h>
#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Ribbon : public _Primitive {

public:
  template <typename... Ts>
  static Ribbon* New(Ts&&... args)
  {
    auto ribbon = new Ribbon(::std::forward<Ts>(args)...);
    ribbon->addToScene(static_cast<unique_ptr_t<Geometry>>(ribbon));

    return ribbon;
  }
  ~Ribbon();

  unique_ptr_t<VertexData> _regenerateVertexData();
  Geometry* copy(const string_t& id);

protected:
  Ribbon(const string_t& id, Scene* scene,
         const vector_t<vector_t<Vector3>>& pathArray, bool closeArray,
         bool closePath, int offset, bool canBeRegenerated, Mesh* mesh,
         unsigned int side = Mesh::DEFAULTSIDE());

public:
  vector_t<vector_t<Vector3>> pathArray;
  bool closeArray;
  bool closePath;
  int offset;
  unsigned int side;

}; // end of class Ribbon

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_RIBBON_H
