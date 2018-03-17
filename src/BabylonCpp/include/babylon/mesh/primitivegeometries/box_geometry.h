#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_BOX_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_BOX_GEOMETRY_H

#include <babylon/babylon_global.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitivegeometries/_primitive_geometry.h>

namespace BABYLON {

/**
 * @brief Creates a box geometry.
 * @see http://doc.babylonjs.com/how_to/set_shapes#box
 */
class BABYLON_SHARED_EXPORT BoxGeometry : public _PrimitiveGeometry {

public:
  template <typename... Ts>
  static BoxGeometry* New(Ts&&... args)
  {
    auto box = new BoxGeometry(::std::forward<Ts>(args)...);
    box->addToScene(static_cast<unique_ptr_t<Geometry>>(box));

    return box;
  }
  ~BoxGeometry() override;

  unique_ptr_t<VertexData> _regenerateVertexData() override;
  Geometry* copy(const string_t& id) override;
  Json::object serialize() const override;

  // Statics
  static BoxGeometry* Parse(const Json::value& parsedBox, Scene* scene);

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
  BoxGeometry(const string_t& id, Scene* scene, float size,
              bool canBeRegenerated, Mesh* mesh = nullptr,
              unsigned int side = Mesh::DEFAULTSIDE());

public:
  float size;
  unsigned int side;

}; // end of class BoxGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_BOX_GEOMETRY_H
