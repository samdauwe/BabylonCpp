#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_CYLINDER_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_CYLINDER_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a new cylinder geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#cylinder-or-cone
 */
class BABYLON_SHARED_EXPORT CylinderGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static CylinderGeometry* New(Ts&&... args)
  {
    auto cylinder = new CylinderGeometry(::std::forward<Ts>(args)...);
    cylinder->addToScene(static_cast<unique_ptr_t<Geometry>>(cylinder));

    return cylinder;
  }
  ~CylinderGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static CylinderGeometry* Parse(const Json::value& parsedCylinder,
                                 Scene* scene);

protected:
  /**
   * @brief Creates a new cylinder geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param height defines the height of the cylinder
   * @param diameterTop defines the diameter of the cylinder's top cap
   * @param diameterBottom defines the diameter of the cylinder's bottom cap
   * @param tessellation defines the tessellation factor to apply to the
   * cylinder (number of radial sides)
   * @param subdivisions defines the number of subdivisions to apply to the
   * cylinder (number of rings) (1 by default)
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  CylinderGeometry(const string_t& id, Scene* scene, float height,
                   float diameterTop, float diameterBottom,
                   unsigned int tessellation, unsigned int subdivisions,
                   bool canBeRegenerated, Mesh* mesh = nullptr,
                   unsigned int side = Mesh::DEFAULTSIDE());

public:
  float height;
  float diameterTop;
  float diameterBottom;
  unsigned int tessellation;
  unsigned int subdivisions;
  unsigned int side;

}; // end of class CylinderGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_CYLINDER_GEOMETRY_H
