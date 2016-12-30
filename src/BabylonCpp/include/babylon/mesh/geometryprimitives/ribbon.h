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
    auto ribbon = new Ribbon(std::forward<Ts>(args)...);
    ribbon->addToScene(static_cast<std::unique_ptr<Geometry>>(ribbon));

    return ribbon;
  }
  ~Ribbon();

  std::unique_ptr<VertexData> _regenerateVertexData();
  Geometry* copy(const std::string& id);

protected:
  Ribbon(const std::string& id, Scene* scene,
         const std::vector<std::vector<Vector3>>& pathArray, bool closeArray,
         bool closePath, int offset, bool canBeRegenerated, Mesh* mesh,
         unsigned int side = Mesh::DEFAULTSIDE);

public:
  std::vector<std::vector<Vector3>> pathArray;
  bool closeArray;
  bool closePath;
  int offset;
  unsigned int side;

}; // end of class Ribbon

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_RIBBON_H
