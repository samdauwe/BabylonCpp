#ifndef BABYLON_ENGINES_EXTENSIONS_DYNAMIC_BUFFER_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_DYNAMIC_BUFFER_EXTENSION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class ThinEngine;
class WebGLDataBuffer;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT DynamicBufferExtension {

public:
  DynamicBufferExtension(ThinEngine* engine);
  ~DynamicBufferExtension();

  /**
   * @brief Update a dynamic index buffer.
   * @param indexBuffer defines the target index buffer
   * @param indices defines the data to update
   * @param offset defines the offset in the target index buffer where update should start
   */
  void updateDynamicIndexBuffer(const WebGLDataBufferPtr& indexBuffer, const IndicesArray& indices,
                                int offset = 0);

  /**
   * @brief Updates a dynamic vertex buffer.
   * @param vertexBuffer the vertex buffer to update
   * @param data the data used to update the vertex buffer
   * @param byteOffset the byte offset of the data
   * @param byteLength the byte length of the data
   */
  void updateDynamicVertexBuffer(const WebGLDataBufferPtr& vertexBuffer, const Float32Array& data,
                                 int byteOffset = -1, int byteLength = -1);

private:
  ThinEngine* _this;

}; // end of class DynamicBufferExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_DYNAMIC_BUFFER_EXTENSION_H
