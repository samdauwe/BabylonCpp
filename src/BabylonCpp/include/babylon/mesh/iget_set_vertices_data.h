#ifndef BABYLON_MESH_IGET_SET_VERTICES_DATA_H
#define BABYLON_MESH_IGET_SET_VERTICES_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class AbstractMesh;

/**
 * @brief Define an interface for all classes that will get and set the data on
 * vertices.
 */
struct BABYLON_SHARED_EXPORT IGetSetVerticesData {
  virtual bool isVerticesDataPresent(unsigned int kind) const = 0;
  virtual Float32Array getVerticesData(unsigned int kind,
                                       bool copyWhenShared = false,
                                       bool forceCopy      = false)
    = 0;
  /**
   * @brief Returns an array of integers or a typed array (Int32Array,
   * Uint32Array, Uint16Array) populated with the mesh indices.
   * @param copyWhenShared If true (default false) and and if the mesh geometry
   * is shared among some other meshes, the returned array is a copy of the
   * internal one.
   * @param forceCopy defines a boolean indicating that the returned array must
   * be cloned upon returning it
   * @returns the indices array or an empty array if the mesh has no geometry
   */
  virtual IndicesArray getIndices(bool copyWhenShared = false,
                                  bool forceCopy      = false)
    = 0;
  virtual AbstractMesh*
  setVerticesData(unsigned int kind, const Float32Array& data,
                  bool updatable                      = false,
                  const std::optional<size_t>& stride = std::nullopt)
    = 0;
  virtual AbstractMesh*
  updateVerticesData(unsigned int kind, const Float32Array& data,
                     bool updateExtends = false, bool makeItUnique = false)
    = 0;
  virtual AbstractMesh* setIndices(const IndicesArray& indices,
                                   size_t totalVertices = 0,
                                   bool updatable       = false)
    = 0;
}; // end of struct IGetSetVerticesData

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_IGET_SET_VERTICES_DATA_H
