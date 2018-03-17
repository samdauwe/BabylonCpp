#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_TILED_GROUND_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_TILED_GROUND_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a tiled ground geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#tiled-ground
 */
class BABYLON_SHARED_EXPORT TiledGroundGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static TiledGroundGeometry* New(Ts&&... args)
  {
    auto tiledGround = new TiledGroundGeometry(::std::forward<Ts>(args)...);
    tiledGround->addToScene(static_cast<unique_ptr_t<Geometry>>(tiledGround));

    return tiledGround;
  }
  ~TiledGroundGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;

protected:
  /**
   * @brief Creates a tiled ground geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param xmin defines the minimum value on X axis
   * @param zmin defines the minimum value on Z axis
   * @param xmax defines the maximum value on X axis
   * @param zmax defines the maximum value on Z axis
   * @param subdivisions defines the subdivisions to apply to the ground (number
   * of subdivisions (tiles) on the height and the width of the map)
   * @param precision defines the precision to use when computing the tiles
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   */
  TiledGroundGeometry(const string_t& id, Scene* scene, float xmin, float zmin,
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

}; // end of class TiledGroundGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_TILED_GROUND_GEOMETRY_H
