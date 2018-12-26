#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_SPHERE_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_SPHERE_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

class SphereGeometry;
using SphereGeometryPtr = std::shared_ptr<SphereGeometry>;

/**
 * @brief Creates a sphere geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#sphere
 */
class BABYLON_SHARED_EXPORT SphereGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static SphereGeometryPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<SphereGeometry>(
      new SphereGeometry(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~SphereGeometry() override;

  /**
   * @brief Hidden
   * @return
   */
  std::unique_ptr<VertexData> _regenerateVertexData() override;

  GeometryPtr copy(const std::string& id) override;
  json serialize() const override;

  // Statics
  static SphereGeometryPtr Parse(const json& parsedSphere, Scene* scene);

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
  SphereGeometry(const std::string& id, Scene* scene, unsigned int segments,
                 float diameter, bool canBeRegenerated, Mesh* mesh = nullptr,
                 unsigned int side = Mesh::DEFAULTSIDE);

public:
  /**
   * Defines the number of segments to use to create the sphere
   */
  unsigned int segments;

  /**
   * Defines the diameter of the sphere
   */
  float diameter;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class SphereGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_SPHERE_GEOMETRY_H
