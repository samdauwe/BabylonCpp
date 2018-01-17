#ifndef BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
#define BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT PostProcessManager : public IDisposable {

public:
  PostProcessManager(Scene* scene);
  virtual ~PostProcessManager();

  /** Methods **/
  void _rebuild();
  bool _prepareFrame(InternalTexture* sourceTexture              = nullptr,
                     const vector_t<PostProcess*>& postProcesses = {});
  void directRender(const vector_t<PostProcess*>& postProcesses,
                    InternalTexture* targetTexture = nullptr,
                    bool forceFullscreenViewport   = false);
  void _finalizeFrame(bool doNotPresent,
                      InternalTexture* targetTexture = nullptr,
                      unsigned int faceIndex         = 0,
                      const vector_t<PostProcess*>& postProcesses
                      = vector_t<PostProcess*>(),
                      bool forceFullscreenViewport = false);
  void dispose(bool doNotRecurse = false) override;

private:
  void _prepareBuffers();
  void _buildIndexBuffer();

private:
  Scene* _scene;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Float32Array _vertexDeclaration;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unordered_map_t<string_t, VertexBuffer*> _vertexBufferPtrs;

}; // end of class PostProcessManager

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
