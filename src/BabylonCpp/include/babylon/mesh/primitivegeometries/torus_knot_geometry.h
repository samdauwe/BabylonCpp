#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_KNOT_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_KNOT_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

class TorusKnotGeometry;
using TorusKnotGeometryPtr = std::shared_ptr<TorusKnotGeometry>;

/**
 * @brief Creates a torus knot geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#torus-knot
 */
class BABYLON_SHARED_EXPORT TorusKnotGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static TorusKnotGeometryPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<TorusKnotGeometry>(
      new TorusKnotGeometry(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~TorusKnotGeometry() override;

  /**
   * @brief Hidden
   * @return
   */
  std::unique_ptr<VertexData> _regenerateVertexData() override;

  GeometryPtr copy(const std::string& id) override;
  json serialize() const override;

  // Statics
  static TorusKnotGeometryPtr Parse(const json& parsedTorusKnot, Scene* scene);

protected:
  /**
   * @brief Creates a torus knot geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param radius defines the radius of the torus knot
   * @param tube defines the thickness of the torus knot tube
   * @param radialSegments defines the number of radial segments
   * @param tubularSegments defines the number of tubular segments
   * @param p defines the first number of windings
   * @param q defines the second number of windings
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  TorusKnotGeometry(const std::string& id, Scene* scene, float radius,
                    float tube, unsigned int radialSegments,
                    unsigned int tubularSegments, float p, float q,
                    bool canBeRegenerated, Mesh* mesh = nullptr,
                    unsigned int side = Mesh::DEFAULTSIDE);

public:
  /**
   * Defines the radius of the torus knot
   */
  float radius;

  /**
   * Defines the thickness of the torus knot tube
   */
  float tube;

  /**
   * Defines the number of radial segments
   */
  unsigned int radialSegments;

  /**
   * Defines the number of tubular segments
   */
  unsigned int tubularSegments;

  /**
   * Defines the first number of windings
   */
  float p;

  /**
   * Defines the second number of windings
   */
  float q;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class TorusKnotGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_KNOT_GEOMETRY_H
