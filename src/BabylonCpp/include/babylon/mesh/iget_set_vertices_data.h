#ifndef BABYLON_MESH_IGET_SET_VERTICES_DATA_H
#define BABYLON_MESH_IGET_SET_VERTICES_DATA_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
struct BABYLON_SHARED_EXPORT IGetSetVerticesData {
  virtual bool isVerticesDataPresent(unsigned int kind) = 0;
  virtual Float32Array getVerticesData(unsigned int kind,
                                       bool copyWhenShared = false)
    = 0;
  virtual IndicesArray getIndices(bool copyWhenShared = false) = 0;
  virtual void setVerticesData(unsigned int kind, const Float32Array& data,
                               bool updatable = false, int stride = -1)
    = 0;
  virtual void updateVerticesData(unsigned int kind, const Float32Array& data,
                                  bool updateExtends = false,
                                  bool makeItUnique  = false)
    = 0;
  virtual void setIndices(const IndicesArray& indices, int totalVertices = -1)
    = 0;
}; // end of struct IGetSetVerticesData

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_IGET_SET_VERTICES_DATA_H
