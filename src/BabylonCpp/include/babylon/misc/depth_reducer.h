#ifndef BABYLON_MISC_DEPTH_REDUCER_H
#define BABYLON_MISC_DEPTH_REDUCER_H

#include <babylon/babylon_api.h>
#include <babylon/misc/min_max_reducer.h>

namespace BABYLON {

class DepthRenderer;
using DepthRendererPtr = std::shared_ptr<DepthRenderer>;

/**
 * @brief This class is a small wrapper around the MinMaxReducer class to compute the min/max values
 * of a depth texture.
 */
class BABYLON_SHARED_EXPORT DepthReducer : public MinMaxReducer {

public:
  /**
   * @brief Creates a depth reducer.
   * @param camera The camera used to render the depth texture
   */
  DepthReducer(const CameraPtr& camera);
  ~DepthReducer() override; // = default

  /**
   * @brief Sets the depth renderer to use to generate the depth map
   * @param depthRenderer The depth renderer to use. If not provided, a new one will be created
   * automatically
   * @param type The texture type of the depth map (default: TEXTURETYPE_HALF_FLOAT)
   * @param forceFullscreenViewport Forces the post processes used for the reduction to be applied
   * without taking into account viewport (defaults to true)
   */
  void setDepthRenderer(DepthRendererPtr depthRenderer = nullptr,
                        unsigned int type              = Constants::TEXTURETYPE_HALF_FLOAT,
                        bool forceFullscreenViewport   = true);

  /**
   * @brief Hidden
   */
  void setSourceTexture(const RenderTargetTexturePtr& sourceTexture, bool depthRedux,
                        unsigned int type            = Constants::TEXTURETYPE_HALF_FLOAT,
                        bool forceFullscreenViewport = true) override;

  /**
   * @brief Activates the reduction computation.
   * When activated, the observers registered in onAfterReductionPerformed are
   * called after the compuation is performed
   */
  void activate() override;

  /**
   * @brief Deactivates the reduction computation.
   */
  void deactivate() override;

  /**
   * @brief Disposes the depth reducer.
   * @param disposeAll true to dispose all the resources. You should always call this function with
   * true as the parameter (or without any parameter as it is the default one). This flag is meant
   * to be used internally.
   */
  void dispose(bool disposeAll = true) override;

protected:
  /**
   * @brief Gets the depth renderer used for the computation.
   * Note that the result is null if you provide your own renderer when calling setDepthRenderer.
   */
  DepthRendererPtr& get_depthRenderer();

public:
  /**
   * Gets the depth renderer used for the computation.
   * Note that the result is null if you provide your own renderer when calling setDepthRenderer.
   */
  ReadOnlyProperty<DepthReducer, DepthRendererPtr> depthRenderer;

private:
  DepthRendererPtr _depthRenderer;
  std::string _depthRendererId;

}; // end of class DepthReducer

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_DEPTH_REDUCER_H
