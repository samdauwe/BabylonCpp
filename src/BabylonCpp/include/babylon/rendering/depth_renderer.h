#ifndef BABYLON_RENDERING_DEPTH_RENDERER_H
#define BABYLON_RENDERING_DEPTH_RENDERER_H

#include <babylon/babylon_api.h>
#include <babylon/engines/engine_constants.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

class Camera;
class Effect;
class RenderTargetTexture;
class Scene;
class SubMesh;
using CameraPtr              = std::shared_ptr<Camera>;
using EffectPtr              = std::shared_ptr<Effect>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief This represents a depth renderer in Babylon.
 * A depth renderer will render to it's depth map every frame which can be
 * displayed or used in post processing
 */
class BABYLON_SHARED_EXPORT DepthRenderer {

public:
  /**
   * @brief Instantiates a depth renderer.
   * @param scene The scene the renderer belongs to
   * @param type The texture type of the depth map (default:
   * Engine.TEXTURETYPE_FLOAT)
   * @param camera The camera to be used to render the depth map (default:
   * scene's active camera)
   */
  DepthRenderer(Scene* scene,
                unsigned int type       = EngineConstants::TEXTURETYPE_FLOAT,
                const CameraPtr& camera = nullptr);
  virtual ~DepthRenderer();

  /**
   * @brief Creates the depth rendering effect and checks if the effect is
   * ready.
   * @param subMesh The submesh to be used to render the depth map of
   * @param useInstances If multiple world instances should be used
   * @returns if the depth renderer is ready to render the depth map
   */
  bool isReady(SubMesh* subMesh, bool useInstances);

  /**
   * @brief Gets the texture which the depth map will be written to.
   * @returns The depth map texture
   */
  RenderTargetTexturePtr& getDepthMap();

  /**
   * @brief Disposes of the depth renderer.
   */
  void dispose();

public:
  /**
   * Specifies that the depth renderer will only be used within
   * the camera it is created for.
   * This can help forcing its rendering during the camera processing.
   */
  bool useOnlyInActiveCamera;

private:
  Scene* _scene;
  RenderTargetTexturePtr _depthMap;
  EffectPtr _effect;
  std::string _cachedDefines;
  CameraPtr _camera;

}; // end of class DepthRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_DEPTH_RENDERER_H
