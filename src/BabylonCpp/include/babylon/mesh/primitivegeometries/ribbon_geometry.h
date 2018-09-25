#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_RIBBON_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_RIBBON_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

class RibbonGeometry;
using RibbonGeometryPtr = std::shared_ptr<RibbonGeometry>;

/**
 * @brief Creates a ribbon geometry.
 * @see http://doc.babylonjs.com/how_to/ribbon_tutorial,
 * http://doc.babylonjs.com/resources/maths_make_ribbons
 */
class BABYLON_SHARED_EXPORT RibbonGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static RibbonGeometryPtr New(Ts&&... args)
  {
    auto mesh = std::shared_ptr<RibbonGeometry>(
      new RibbonGeometry(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~RibbonGeometry();

  /**
   * @brief Hidden
   */
  std::unique_ptr<VertexData> _regenerateVertexData();

  GeometryPtr copy(const std::string& id);

protected:
  /**
   * @brief Creates a ribbon geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param pathArray defines the array of paths to use
   * @param closeArray defines if the last path and the first path must be
   * joined
   * @param closePath defines if the last and first points of each path in your
   * pathArray must be joined
   * @param offset defines the offset between points
   * @param canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   * @param side defines if the created geometry is double sided or not (default
   * is BABYLON.Mesh.DEFAULTSIDE)
   */
  RibbonGeometry(const std::string& id, Scene* scene,
                 const std::vector<std::vector<Vector3>>& pathArray,
                 bool closeArray, bool closePath, int offset,
                 bool canBeRegenerated, Mesh* mesh,
                 unsigned int side = Mesh::DEFAULTSIDE());

public:
  /**
   * Defines the array of paths to use
   */
  std::vector<std::vector<Vector3>> pathArray;

  /**
   * Defines if the last and first points of each path in your pathArray must be
   * joined
   */
  bool closeArray;

  /**
   * Defines if the last and first points of each path in your pathArray must be
   * joined
   */
  bool closePath;

  /**
   * Defines the offset between points
   */
  int offset;

  /**
   * Defines if the created geometry is double sided or not (default is
   * BABYLON.Mesh.DEFAULTSIDE)
   */
  unsigned int side;

}; // end of class RibbonGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_RIBBON_GEOMETRY_H
