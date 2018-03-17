#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a new torus geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#torus
 */
class BABYLON_SHARED_EXPORT TorusGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static TorusGeometry* New(Ts&&... args)
  {
    auto torus = new TorusGeometry(::std::forward<Ts>(args)...);
    torus->addToScene(static_cast<unique_ptr_t<Geometry>>(torus));

    return torus;
  }
  ~TorusGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static TorusGeometry* Parse(const Json::value& parsedTorus, Scene* scene);

protected:
  /**
   * @brief Creates a new torus geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param diameter defines the diameter of the torus
   * @param thickness defines the thickness of the torus (ie. internal diameter)
   * @param tessellation defines the tesselation factor to apply to the torus
   * (number of segments along the circle)
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  TorusGeometry(const string_t& id, Scene* scene, float diameter,
                float thickness, unsigned int tessellation,
                bool canBeRegenerated, Mesh* mesh = nullptr,
                unsigned int side = Mesh::DEFAULTSIDE());

private:
  float diameter;
  float thickness;
  unsigned int tessellation;
  unsigned int side;

}; // end of class TorusGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_TORUS_GEOMETRY_H
