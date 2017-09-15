#ifndef BABYLON_MESH_PRIMITIVES_PLANE_H
#define BABYLON_MESH_PRIMITIVES_PLANE_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Plane : public _Primitive {

public:
  template <typename... Ts>
  static Plane* New(Ts&&... args)
  {
    auto plane = new Plane(::std::forward<Ts>(args)...);
    plane->addToScene(static_cast<unique_ptr_t<Geometry>>(plane));

    return plane;
  }
  ~Plane();

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;

  // Statics
  static Plane* Parse(const Json::value& parsedPlane, Scene* scene);

protected:
  Plane(const string_t& id, Scene* scene, float size, bool canBeRegenerated,
        Mesh* mesh, unsigned int side = Mesh::DEFAULTSIDE);

public:
  float size;
  unsigned int side;

}; // end of class Plane

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_PLANE_H
