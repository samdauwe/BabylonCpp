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
  bool _prepareFrame(InternalTexture* sourceTexture,
                     const std::vector<PostProcess*>& postProcesses = {});
  void directRender(const std::vector<PostProcess*>& postProcesses,
                    InternalTexture* targetTexture,
                    bool forceFullscreenViewport = false);
  void _finalizeFrame(bool doNotPresent,
                      InternalTexture* targetTexture = nullptr,
                      unsigned int faceIndex         = 0,
                      const std::vector<PostProcess*>& postProcesses
                      = std::vector<PostProcess*>());
  void dispose(bool doNotRecurse = false) override;

private:
  void _prepareBuffers();

private:
  Scene* _scene;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Float32Array _vertexDeclaration;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unordered_map<std::string, VertexBuffer*> _vertexBufferPtrs;

}; // end of class PostProcessManager

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
