#ifndef BABYLON_MESH_PRIMITIVES_SPHERE_H
#define BABYLON_MESH_PRIMITIVES_SPHERE_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Sphere : public _Primitive {

public:
  template <typename... Ts>
  static Sphere* New(Ts&&... args)
  {
    auto sphere = new Sphere(::std::forward<Ts>(args)...);
    sphere->addToScene(static_cast<std::unique_ptr<Geometry>>(sphere));

    return sphere;
  }
  ~Sphere();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;

  // Statics
  static Sphere* Parse(const Json::value& parsedSphere, Scene* scene);

protected:
  Sphere(const string_t& id, Scene* scene, unsigned int segments,
         float diameter, bool canBeRegenerated, Mesh* mesh,
         unsigned int side = Mesh::DEFAULTSIDE);

public:
  unsigned int segments;
  float diameter;
  unsigned int side;

}; // end of class Sphere

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_SPHERE_H
