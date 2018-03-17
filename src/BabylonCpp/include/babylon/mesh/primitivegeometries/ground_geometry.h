#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_GROUND_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_GROUND_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a new ground geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#ground
 */
class BABYLON_SHARED_EXPORT GroundGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static GroundGeometry* New(Ts&&... args)
  {
    auto ground = new GroundGeometry(::std::forward<Ts>(args)...);
    ground->addToScene(static_cast<unique_ptr_t<Geometry>>(ground));

    return ground;
  }
  ~GroundGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static GroundGeometry* Parse(const Json::value& parsedGround, Scene* scene);

protected:
  /**
   * @brief Creates a new ground geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param width defines the width of the ground
   * @param height defines the height of the ground
   * @param subdivisions defines the subdivisions to apply to the ground
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   */
  GroundGeometry(const string_t& id, Scene* scene, unsigned int width,
                 unsigned int height, unsigned int subdivisions,
                 bool canBeRegenerated, Mesh* mesh = nullptr);

public:
  unsigned int width;
  unsigned int height;
  unsigned int subdivisions;

}; // end of class GroundGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_GROUND_GEOMETRY_H
