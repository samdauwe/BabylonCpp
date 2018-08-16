#ifndef BABYLON_MESH_PRIMITIVE_GEOMETRIES_PRIMITIVE_GEOMETRY_H
#define BABYLON_MESH_PRIMITIVE_GEOMETRIES_PRIMITIVE_GEOMETRY_H

#include <babylon/babylon_global.h>
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
  _PrimitiveGeometry(const string_t& id, Scene* scene, bool canBeRegenerated,
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
  Geometry* asNewGeometry(const string_t& id);

  // overrides

  /**
   * @brief Hidden
   */
  void setAllVerticesData(VertexData* vertexData, bool updatable);

  AbstractMesh* setVerticesData(unsigned int kind, const Float32Array& data,
                                bool updatable = false,
                                const nullable_t<size_t>& stride
                                = nullopt_t) override;

  // to override
  virtual unique_ptr_t<VertexData> _regenerateVertexData() = 0;
  virtual Geometry* copy(const string_t& id)               = 0;
  virtual Json::object serialize() const;

private:
  bool _beingRegenerated;
  bool _canBeRegenerated;

}; // end of class _PrimitiveGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_PRIMITIVE_GEOMETRIES_PRIMITIVE_GEOMETRY_H
