#ifndef BABYLON_MESH_BUFFER_H
#define BABYLON_MESH_BUFFER_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

/**
 * @brief Buffer representation.
 */
class BABYLON_SHARED_EXPORT Buffer : public IDisposable {

public:
  Buffer(Engine* engine, const Float32Array& data, bool updatable, int stride,
         bool postponeInternalCreation = false, bool instanced = false);
  Buffer(Mesh* mesh, const Float32Array& data, bool updatable, int stride,
         bool postponeInternalCreation = false, bool instanced = false);
  virtual ~Buffer();

  /**
   * @param Create a new {BABYLON.VertexBuffer} based on the current buffer.
   * @param kind defines the vertex buffer kind (position, normal, etc.)
   * @param offset defines offset in the buffer (0 by default)
   * @param size defines the size in floats of attributes (position is 3 for
   * instance)
   * @param stride defines the stride size in floats in the buffer (the offset
   * to apply to reach next value when data is interleaved)
   * @param instanced defines if the vertex buffer contains indexed data
   * @returns the new vertex buffer
   */
  unique_ptr_t<VertexBuffer>
  createVertexBuffer(unsigned int kind, int offset, int size,
                     Nullable<int> stride     = nullptr,
                     Nullable<bool> instanced = nullptr);

  // Properties
  bool isUpdatable() const;
  Float32Array& getData();
  GL::IGLBuffer* getBuffer();
  int getStrideSize() const;
  bool getIsInstanced() const;
  unsigned int instanceDivisor() const;
  void setInstanceDivisor(unsigned int value);

  // Methods
  void _rebuild();
  GL::IGLBuffer* create(Float32Array data = {});
  GL::IGLBuffer* update(const Float32Array& data);
  GL::IGLBuffer* updateDirectly(const Float32Array& data, int offset);
  GL::IGLBuffer* updateDirectly(const Float32Array& data, int offset,
                                size_t vertexCount);
  void dispose(bool doNotRecurse = false) override;

private:
  Engine* _engine;
  unique_ptr_t<GL::IGLBuffer> _buffer;
  Float32Array _data;
  bool _updatable;
  int _strideSize;
  bool _instanced;
  unsigned int _instanceDivisor;

}; // end of class Buffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_BUFFER_H
