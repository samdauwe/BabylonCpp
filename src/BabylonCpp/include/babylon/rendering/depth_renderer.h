#ifndef BABYLON_RENDERING_DEPTH_RENDERER_H
#define BABYLON_RENDERING_DEPTH_RENDERER_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT DepthRenderer : public IDisposable {

public:
  DepthRenderer(Scene* scene, unsigned int type = Engine::TEXTURETYPE_FLOAT);
  virtual ~DepthRenderer();

  bool isReady(SubMesh* subMesh, bool useInstances);
  RenderTargetTexture* getDepthMap();
  void dispose(bool doNotRecurse = false) override;

private:
  Scene* _scene;
  std::unique_ptr<RenderTargetTexture> _depthMap;
  Effect* _effect;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _transformMatrix;
  Matrix _worldViewProjection;
  std::string _cachedDefines;

}; // end of class DepthRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_DEPTH_RENDERER_H
