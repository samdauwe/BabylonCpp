#ifndef BABYLON_ENGINES_EXTENSIONS_UNIFORM_BUFFER_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_UNIFORM_BUFFER_EXTENSION_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class IPipelineContext;
class ThinEngine;
class WebGLDataBuffer;
using IPipelineContextPtr = std::shared_ptr<IPipelineContext>;
using WebGLDataBufferPtr  = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT UniformBufferExtension {

public:
  UniformBufferExtension(ThinEngine* engine);
  ~UniformBufferExtension();

  /**
   * @brief Create an uniform buffer.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param elements defines the content of the uniform buffer
   * @returns the webGL uniform buffer
   */
  WebGLDataBufferPtr createUniformBuffer(const Float32Array& elements);

  /**
   * @brief Create a dynamic uniform buffer.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param elements defines the content of the uniform buffer
   * @returns the webGL uniform buffer
   */
  WebGLDataBufferPtr createDynamicUniformBuffer(const Float32Array& elements);

  /**
   * @brief Update an existing uniform buffer.
   * @see https://doc.babylonjs.com/features/webgl2#uniform-buffer-objets
   * @param uniformBuffer defines the target uniform buffer
   * @param elements defines the content to update
   * @param offset defines the offset in the uniform buffer where update should start
   * @param count defines the size of the data to update
   */
  void updateUniformBuffer(const WebGLDataBufferPtr& uniformBuffer, const Float32Array& elements,
                           int offset = -1, int count = -1);

  /**
   * @brief Bind an uniform buffer to the current webGL context
   * @param buffer defines the buffer to bind
   */
  void bindUniformBuffer(const WebGLDataBufferPtr& buffer);

  /**
   * @brief Bind a buffer to the current webGL context at a given location.
   * @param buffer defines the buffer to bind
   * @param location defines the index where to bind the buffer
   * @param name Name of the uniform variable to bind
   */
  void bindUniformBufferBase(const WebGLDataBufferPtr& buffer, unsigned int location,
                             const std::string& name);

  /**
   * @brief Bind a specific block at a given index in a specific shader program.
   * @param pipelineContext defines the pipeline context to use
   * @param blockName defines the block name
   * @param index defines the index where to bind the block
   */
  void bindUniformBlock(const IPipelineContextPtr& pipelineContext, const std::string& blockName,
                        unsigned int index);

private:
  ThinEngine* _this;

}; // end of class UniformBufferExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_UNIFORM_BUFFER_EXTENSION_H
