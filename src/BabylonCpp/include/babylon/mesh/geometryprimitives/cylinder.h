#ifndef BABYLON_MESH_PRIMITIVES_CYLINDER_H
#define BABYLON_MESH_PRIMITIVES_CYLINDER_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Cylinder : public _Primitive {

public:
  template <typename... Ts>
  static Cylinder* New(Ts&&... args)
  {
    auto cylinder = new Cylinder(::std::forward<Ts>(args)...);
    cylinder->addToScene(static_cast<std::unique_ptr<Geometry>>(cylinder));

    return cylinder;
  }
  ~Cylinder();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;

  // Statics
  static Cylinder* Parse(const Json::value& parsedCylinder, Scene* scene);

protected:
  Cylinder(const string_t& id, Scene* scene, float height, float diameterTop,
           float diameterBottom, unsigned int tessellation,
           unsigned int subdivisions, bool canBeRegenerated, Mesh* mesh,
           unsigned int side = Mesh::DEFAULTSIDE);

public:
  float height;
  float diameterTop;
  float diameterBottom;
  unsigned int tessellation;
  unsigned int subdivisions;
  unsigned int side;

}; // end of class Cylinder

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_CYLINDER_H
