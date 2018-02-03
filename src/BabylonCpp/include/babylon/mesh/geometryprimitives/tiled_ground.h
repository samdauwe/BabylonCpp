#ifndef BABYLON_MESH_PRIMITIVES_TILED_GROUND_H
#define BABYLON_MESH_PRIMITIVES_TILED_GROUND_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT TiledGround : public _Primitive {

public:
  template <typename... Ts>
  static TiledGround* New(Ts&&... args)
  {
    auto tiledGround = new TiledGround(::std::forward<Ts>(args)...);
    tiledGround->addToScene(static_cast<unique_ptr_t<Geometry>>(tiledGround));

    return tiledGround;
  }
  ~TiledGround() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;

protected:
  TiledGround(const string_t& id, Scene* scene, float xmin, float zmin,
              float xmax, float zmax, const ISize& subdivisions,
              const ISize& precision, bool canBeRegenerated,
              Mesh* mesh = nullptr);

private:
  float xmin;
  float zmin;
  float xmax;
  float zmax;
  ISize subdivisions;
  ISize precision;

}; // end of class TiledGround

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_TILED_GROUND_H
