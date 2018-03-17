#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_SPHERE_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_SPHERE_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a sphere geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#sphere
 */
class BABYLON_SHARED_EXPORT SphereGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static SphereGeometry* New(Ts&&... args)
  {
    auto sphere = new SphereGeometry(::std::forward<Ts>(args)...);
    sphere->addToScene(static_cast<unique_ptr_t<Geometry>>(sphere));

    return sphere;
  }
  ~SphereGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static SphereGeometry* Parse(const Json::value& parsedSphere, Scene* scene);

protected:
  /**
   * @brief Create a new sphere geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param segments defines the number of segments to use to create the sphere
   * @param diameter defines the diameter of the sphere
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  SphereGeometry(const string_t& id, Scene* scene, unsigned int segments,
                 float diameter, bool canBeRegenerated, Mesh* mesh = nullptr,
                 unsigned int side = Mesh::DEFAULTSIDE());

public:
  unsigned int segments;
  float diameter;
  unsigned int side;

}; // end of class SphereGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_SPHERE_GEOMETRY_H
