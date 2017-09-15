#ifndef BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
#define BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GeometryBufferRenderer : public IDisposable {

public:
  GeometryBufferRenderer(Scene* scene, float ratio = 1.f);
  virtual ~GeometryBufferRenderer();

  void setRenderList(const vector_t<Mesh*>& meshes);
  bool isSupported() const;
  bool enablePosition() const;
  void setEnablePosition(bool enable);
  bool isReady(SubMesh* subMesh, bool useInstances);
  MultiRenderTarget* getGBuffer();
  void dispose(bool doNotRecurse = false) override;

private:
  void renderSubMesh(SubMesh* subMesh);
  void _createRenderTargets();

private:
  Scene* _scene;
  unique_ptr_t<MultiRenderTarget> _multiRenderTarget;
  Effect* _effect;
  float _ratio;
  Matrix _viewMatrix;
  Matrix _projectionMatrix;
  Matrix _transformMatrix;
  Matrix _worldViewProjection;
  string_t _cachedDefines;
  bool _enablePosition;

}; // end of class GeometryBufferRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
