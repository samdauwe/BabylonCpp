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
  bool _prepareFrame(GL::IGLTexture* sourceTexture);
  void directRender(const std::vector<PostProcess*>& postProcesses,
                    GL::IGLTexture* targetTexture);
  void _finalizeFrame(bool doNotPresent,
                      GL::IGLTexture* targetTexture = nullptr,
                      int faceIndex                 = 0,
                      const std::vector<PostProcess*>& postProcesses
                      = std::vector<PostProcess*>());
  void dispose(bool doNotRecurse = false) override;

private:
  void _prepareBuffers();

private:
  Scene* _scene;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  std::vector<float> _vertexDeclaration;
  int _vertexStrideSize;
  std::unique_ptr<GL::IGLBuffer> _vertexBuffer;

}; // end of class PostProcessManager

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
