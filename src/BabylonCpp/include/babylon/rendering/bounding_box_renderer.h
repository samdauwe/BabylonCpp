#ifndef BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
#define BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoundingBoxRenderer : public IDisposable {

public:
  BoundingBoxRenderer(Scene* scene);
  virtual ~BoundingBoxRenderer();

  void _rebuild();
  void reset();
  void render();
  void renderOcclusionBoundingBox(AbstractMesh* mesh);
  void dispose(bool doNotRecurse = false) override;

private:
  void _prepareResources();
  void _createIndexBuffer();

public:
  Color3 frontColor;
  Color3 backColor;
  bool showBackLines;
  vector_t<BoundingBox> renderList;

private:
  Scene* _scene;
  ShaderMaterial* _colorShader;
  vector_t<unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unordered_map_t<string_t, VertexBuffer*> _vertexBuffersMap;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;

}; // end of class BoundingBoxRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
