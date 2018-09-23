#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_PLANE_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_PLANE_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

class PlaneGeometry;
using PlaneGeometryPtr = std::shared_ptr<PlaneGeometry>;

/**
 * @brief Creates a plane geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#plane
 */
class BABYLON_SHARED_EXPORT PlaneGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static PlaneGeometryPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<PlaneGeometry>(
      new PlaneGeometry(::std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~PlaneGeometry() override;

  /**
   * @brief Hidden
   * @return
   */
  std::unique_ptr<VertexData> _regenerateVertexData() override;

  GeometryPtr copy(const std::string& id) override;
  Json::object serialize() const override;

  // Statics
  static PlaneGeometryPtr Parse(const Json::value& parsedPlane, Scene* scene);

protected:
  /**
   * @brief Creates a plane geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param size defines the size of the plane (width === height)
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  PlaneGeometry(const std::string& id, Scene* scene, float size,
                bool canBeRegenerated, Mesh* mesh = nullptr,
                unsigned int side = Mesh::DEFAULTSIDE());

public:
  /**
   * Defines the size of the plane (width === height)
   */
  float size;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class PlaneGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_PLANE_GEOMETRY_H
