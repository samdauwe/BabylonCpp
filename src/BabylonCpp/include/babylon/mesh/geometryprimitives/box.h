#ifndef BABYLON_MESH_GEOMETRY_PRIMITIVES_BOX_H
#define BABYLON_MESH_GEOMETRY_PRIMITIVES_BOX_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT Box : public _Primitive {

public:
  template <typename... Ts>
  static Box* New(Ts&&... args)
  {
    auto box = new Box(::std::forward<Ts>(args)...);
    box->addToScene(static_cast<std::unique_ptr<Geometry>>(box));

    return box;
  }
  ~Box();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const std::string& id) override;

  // Statics
  static Box* Parse(const Json::value& parsedBox, Scene* scene);

protected:
  Box(const std::string& id, Scene* scene, float size, bool canBeRegenerated,
      Mesh* mesh, unsigned int side = Mesh::DEFAULTSIDE);

public:
  float size;
  unsigned int side;

}; // end of class Box

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GEOMETRY_PRIMITIVES_BOX_H
