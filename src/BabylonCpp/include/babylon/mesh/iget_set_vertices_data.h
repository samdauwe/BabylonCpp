#ifndef BABYLON_MESH_IGET_SET_VERTICES_DATA_H
#define BABYLON_MESH_IGET_SET_VERTICES_DATA_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

/**
 * @brief Define an interface for all classes that will get and set the data on
 * vertices.
 */
struct BABYLON_SHARED_EXPORT IGetSetVerticesData {
  virtual bool isVerticesDataPresent(unsigned int kind) = 0;
  virtual Float32Array getVerticesData(unsigned int kind,
                                       bool copyWhenShared = false,
                                       bool forceCopy      = false)
    = 0;
  virtual IndicesArray getIndices(bool copyWhenShared = false) = 0;
  virtual AbstractMesh*
  setVerticesData(unsigned int kind, const Float32Array& data,
                  bool updatable                 = false,
                  const Nullable<size_t>& stride = nullptr)
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
