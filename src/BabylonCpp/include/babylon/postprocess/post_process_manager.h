#ifndef BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
#define BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

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
   * Rebuilds the vertex buffers of the manager.
   */
  void _rebuild();

  /**
   * @brief Prepares a frame to be run through a post process.
   * @param sourceTexture The input texture to the post procesess. (default:
   * null)
   * @param postProcesses An array of post processes to be run. (default: null)
   * @returns True if the post processes were able to be run.
   */
  bool _prepareFrame(InternalTexture* sourceTexture              = nullptr,
                     const vector_t<PostProcess*>& postProcesses = {});

  /**
   * @brief Manually render a set of post processes to a texture.
   * @param postProcesses An array of post processes to be run.
   * @param targetTexture The target texture to render to.
   * @param forceFullscreenViewport force gl.viewport to be full screen eg.
   * 0,0,textureWidth,textureHeight
   */
  void directRender(const vector_t<PostProcess*>& postProcesses,
                    InternalTexture* targetTexture = nullptr,
                    bool forceFullscreenViewport   = false);

  /**
   * @brief Finalize the result of the output of the postprocesses.
   * @param doNotPresent If true the result will not be displayed to the screen.
   * @param targetTexture The target texture to render to.
   * @param faceIndex The index of the face to bind the target texture to.
   * @param postProcesses The array of post processes to render.
   * @param forceFullscreenViewport force gl.viewport to be full screen eg.
   * 0,0,textureWidth,textureHeight (default: false)
   */
  void _finalizeFrame(bool doNotPresent,
                      InternalTexture* targetTexture = nullptr,
                      unsigned int faceIndex         = 0,
                      const vector_t<PostProcess*>& postProcesses
                      = vector_t<PostProcess*>(),
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
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Float32Array _vertexDeclaration;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unordered_map_t<string_t, VertexBuffer*> _vertexBufferPtrs;

}; // end of class PostProcessManager

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_POST_PROCESS_MANANGER_H
