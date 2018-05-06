#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_RIBBON_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_RIBBON_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a ribbon geometry.
 * @see http://doc.babylonjs.com/how_to/ribbon_tutorial,
 * http://doc.babylonjs.com/resources/maths_make_ribbons
 */
class BABYLON_SHARED_EXPORT RibbonGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static RibbonGeometry* New(Ts&&... args)
  {
    auto ribbon = new RibbonGeometry(::std::forward<Ts>(args)...);
    ribbon->addToScene(static_cast<unique_ptr_t<Geometry>>(ribbon));

    return ribbon;
  }
  ~RibbonGeometry();

  /**
   * @brief Hidden
   */
  unique_ptr_t<VertexData> _regenerateVertexData();

  Geometry* copy(const string_t& id);

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
  RibbonGeometry(const string_t& id, Scene* scene,
                 const vector_t<vector_t<Vector3>>& pathArray, bool closeArray,
                 bool closePath, int offset, bool canBeRegenerated, Mesh* mesh,
                 unsigned int side = Mesh::DEFAULTSIDE());

public:
  vector_t<vector_t<Vector3>> pathArray;
  bool closeArray;
  bool closePath;
  int offset;
  unsigned int side;

}; // end of class RibbonGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_RIBBON_GEOMETRY_H
