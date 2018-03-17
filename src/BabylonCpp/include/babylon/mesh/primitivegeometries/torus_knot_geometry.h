#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_KNOT_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_KNOT_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a torus knot geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#torus-knot
 */
class BABYLON_SHARED_EXPORT TorusKnotGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static TorusKnotGeometry* New(Ts&&... args)
  {
    auto torusKnot = new TorusKnotGeometry(::std::forward<Ts>(args)...);
    torusKnot->addToScene(static_cast<unique_ptr_t<Geometry>>(torusKnot));

    return torusKnot;
  }
  ~TorusKnotGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static TorusKnotGeometry* Parse(const Json::value& parsedTorusKnot,
                                  Scene* scene);

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
  TorusKnotGeometry(const string_t& id, Scene* scene, float radius, float tube,
                    unsigned int radialSegments, unsigned int tubularSegments,
                    float p, float q, bool canBeRegenerated,
                    Mesh* mesh        = nullptr,
                    unsigned int side = Mesh::DEFAULTSIDE());

public:
  float radius;
  float tube;
  unsigned int radialSegments;
  unsigned int tubularSegments;
  float p;
  float q;
  unsigned int side;

}; // end of class TorusKnotGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_KNOT_GEOMETRY_H
