#ifndef BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
#define BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H

#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class InternalTexture;
class PostProcess;
class Scene;
class VertexBuffer;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief PostProcessManager is used to manage one or more post processes or
 * post process pipelines See
 * https://doc.babylonjs.com/how_to/how_to_use_postprocesses
 */
class BABYLON_SHARED_EXPORT PostProcessManager {

public:
  /**
   * @brief Creates a new instance PostProcess.
   * @param scene The scene that the post process is associated with.
   */
  PostProcessManager(Scene* scene);
  virtual ~PostProcessManager();

  /** Methods **/
  /**
   * @brief Rebuilds the vertex buffers of the manager.
   * Hidden
   */
  void _rebuild();

  /**
   * @brief Prepares a frame to be run through a post process.
   * @param sourceTexture The input texture to the post procesess. (default:
   * null)
   * @param postProcesses An array of post processes to be run. (default: null)
   * @returns True if the post processes were able to be run.
   * Hidden
   */
  bool _prepareFrame(const InternalTexturePtr& sourceTexture        = nullptr,
                     const std::vector<PostProcess*>& postProcesses = {});

  /**
   * @brief Manually render a set of post processes to a texture.
   * @param postProcesses An array of post processes to be run.
   * @param targetTexture The target texture to render to.
   * @param forceFullscreenViewport force gl.viewport to be full screen eg.
   * 0,0,textureWidth,textureHeight
   * @param faceIndex defines the face to render to if a cubemap is defined as
   * the target
   * @param lodLevel defines which lod of the texture to render to
   */
  void directRender(const std::vector<PostProcess*>& postProcesses,
                    const InternalTexturePtr& targetTexture = nullptr,
                    bool forceFullscreenViewport            = false,
                    unsigned int faceIndex = 0, int lodLevel = 0);

  /**
   * @brief Finalize the result of the output of the postprocesses.
   * @param doNotPresent If true the result will not be displayed to the screen.
   * @param targetTexture The target texture to render to.
   * @param faceIndex The index of the face to bind the target texture to.
   * @param postProcesses The array of post processes to render.
   * @param forceFullscreenViewport force gl.viewport to be full screen eg.
   * 0,0,textureWidth,textureHeight (default: false)
   * Hidden
   */
  void _finalizeFrame(bool doNotPresent,
                      const InternalTexturePtr& targetTexture = nullptr,
                      unsigned int faceIndex                  = 0,
                      const std::vector<PostProcess*>& postProcesses
                      = std::vector<PostProcess*>(),
                      bool forceFullscreenViewport = false);

  /**
   * @brief Disposes of the post process manager.
   */
  void dispose();

private:
  void _prepareBuffers();
  void _buildIndexBuffer();

private:
  Scene* _scene;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Float32Array _vertexDeclaration;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unordered_map<std::string, VertexBuffer*> _vertexBufferPtrs;

}; // end of class PostProcessManager

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
