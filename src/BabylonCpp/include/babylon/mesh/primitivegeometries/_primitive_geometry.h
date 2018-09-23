#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_PRIMITIVE_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_PRIMITIVE_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/mesh/geometry.h>

namespace BABYLON {

/**
 * @brief Abstract class used to provide common services for all typed
 * geometries.
 * Hidden
 */
class BABYLON_SHARED_EXPORT _PrimitiveGeometry : public Geometry {

public:
  /**
   * @brief Creates a new typed geometry.
   * @param id defines the unique ID of the geometry
   * @param scene defines the hosting scene
   * @param _canBeRegenerated defines if the geometry supports being regenerated
   * with new parameters (false by default)
   * @param mesh defines the hosting mesh (can be null)
   */
  _PrimitiveGeometry(const std::string& id, Scene* scene, bool canBeRegenerated,
                     Mesh* mesh);
  ~_PrimitiveGeometry() override;

  /**
   * @brief Gets a value indicating if the geometry supports being regenerated
   * with new parameters (false by default).
   * @returns true if the geometry can be regenerated
   */
  bool canBeRegenerated() const;

  /**
   * @brief If the geometry supports regeneration., the function will recreates
   * the geometry with updated parameter values.
   */
  void regenerate();

  /**
   * @brief Clone the geometry.
   * @param id defines the unique ID of the new geometry
   * @returns the new geometry
   */
  GeometryPtr asNewGeometry(const std::string& id);

  // overrides

  /**
   * @brief Hidden
   */
  void setAllVerticesData(VertexData* vertexData, bool updatable);

  AbstractMesh* setVerticesData(unsigned int kind, const Float32Array& data,
                                bool updatable = false,
                                const std::optional<size_t>& stride
                                = std::nullopt) override;

  // to override
  virtual std::unique_ptr<VertexData> _regenerateVertexData() = 0;
  virtual GeometryPtr copy(const std::string& id)             = 0;
  virtual Json::object serialize() const;

private:
  bool _beingRegenerated;
  bool _canBeRegenerated;

}; // end of class _PrimitiveGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_PRIMITIVE_GEOMETRY_H
