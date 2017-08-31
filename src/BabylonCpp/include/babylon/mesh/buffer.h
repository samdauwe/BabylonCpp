#ifndef BABYLON_MESH_BUFFER_H
#define BABYLON_MESH_BUFFER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

/**
* @brief
*/
class BABYLON_SHARED_EXPORT Buffer : public IDisposable {

public:
  Buffer(Engine* engine, const Float32Array& data, bool updatable, int stride,
         bool postponeInternalCreation = false, bool instanced = false);
  Buffer(Mesh* mesh, const Float32Array& data, bool updatable, int stride,
         bool postponeInternalCreation = false, bool instanced = false);
  virtual ~Buffer();

  std::unique_ptr<VertexBuffer> createVertexBuffer(unsigned int kind,
                                                   int offset, int size);
  std::unique_ptr<VertexBuffer>
  createVertexBuffer(unsigned int kind, int offset, int size, int stride);

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
  std::unique_ptr<GL::IGLBuffer> _buffer;
  Float32Array _data;
  bool _updatable;
  int _strideSize;
  bool _instanced;
  unsigned int _instanceDivisor;

}; // end of class Buffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_BUFFER_H
