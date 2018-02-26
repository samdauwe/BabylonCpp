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
    box->addToScene(static_cast<unique_ptr_t<Geometry>>(box));

    return box;
  }
  ~Box() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;

  // Statics
  static Box* Parse(const Json::value& parsedBox, Scene* scene);

protected:
  Box(const string_t& id, Scene* scene, float size, bool canBeRegenerated,
      Mesh* mesh = nullptr, unsigned int side = Mesh::DEFAULTSIDE());

public:
  float size;
  unsigned int side;

}; // end of class Box

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_GEOMETRY_PRIMITIVES_BOX_H
