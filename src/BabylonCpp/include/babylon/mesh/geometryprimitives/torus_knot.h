#ifndef BABYLON_MESH_PRIMITIVES_TORUS_KNOT_H
#define BABYLON_MESH_PRIMITIVES_TORUS_KNOT_H

#include <babylon/babylon_global.h>

#include <babylon/mesh/geometryprimitives/_primitive.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace GeometryPrimitives {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT TorusKnot : public _Primitive {

public:
  template <typename... Ts>
  static TorusKnot* New(Ts&&... args)
  {
    auto torusKnot = new TorusKnot(std::forward<Ts>(args)...);
    torusKnot->addToScene(static_cast<std::unique_ptr<Geometry>>(torusKnot));

    return torusKnot;
  }
  ~TorusKnot();

  std::unique_ptr<VertexData> _regenerateVertexData() override;
  Geometry* copy(const std::string& id) override;

  // Statics
  static TorusKnot* Parse(const Json::value& parsedTorusKnot, Scene* scene);

protected:
  TorusKnot(const std::string& id, Scene* scene, float radius, float tube,
            unsigned int radialSegments, unsigned int tubularSegments, float p,
            float q, bool canBeRegenerated, Mesh* mesh,
            unsigned int side = Mesh::DEFAULTSIDE);

public:
  float radius;
  float tube;
  unsigned int radialSegments;
  unsigned int tubularSegments;
  float p;
  float q;
  unsigned int side;

}; // end of class TorusKnot

} // end of namespace GeometryPrimitives
} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVES_TORUS_KNOT_H
