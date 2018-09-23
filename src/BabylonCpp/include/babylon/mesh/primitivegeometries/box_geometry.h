#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_BOX_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_BOX_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

class BoxGeometry;
using BoxGeometryPtr = std::shared_ptr<BoxGeometry>;

/**
 * @brief Creates a box geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#box
 */
class BABYLON_SHARED_EXPORT BoxGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static BoxGeometryPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<BoxGeometry>(
      new BoxGeometry(::std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~BoxGeometry() override;

  /**
   * @brief Hidden
   * @return
   */
  std::unique_ptr<VertexData> _regenerateVertexData() override;

  GeometryPtr copy(const std::string& id) override;
  Json::object serialize() const override;

  // Statics
  static BoxGeometryPtr Parse(const Json::value& parsedBox, Scene* scene);

protected:
  /**
   * @brief Creates a box geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param size defines the zise of the box (width, height and depth are the
   * same)
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  BoxGeometry(const std::string& id, Scene* scene, float size,
              bool canBeRegenerated, Mesh* mesh = nullptr,
              unsigned int side = Mesh::DEFAULTSIDE());

public:
  /**
   * Defines the zise of the box (width, height and depth are the same)
   */
  float size;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class BoxGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_BOX_GEOMETRY_H
