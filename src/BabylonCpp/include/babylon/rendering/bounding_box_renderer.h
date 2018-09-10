#ifndef BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
#define BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H

#include <babylon/babylon_global.h>
#include <babylon/engine/iscene_component.h>
#include <babylon/engine/scene_component_constants.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT BoundingBoxRenderer : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name
    = SceneComponentConstants::NAME_BOUNDINGBOXRENDERER;

public:
  template <typename... Ts>
  static BoundingBoxRendererPtr New(Ts&&... args)
  {
    auto renderer = shared_ptr_t<BoundingBoxRenderer>(
      new BoundingBoxRenderer(::std::forward<Ts>(args)...));
    renderer->addToScene(renderer);

    return renderer;
  }
  virtual ~BoundingBoxRenderer();

  void addToScene(const BoundingBoxRendererPtr& newBoundingBoxRenderer);

  /**
   * @brief Registers the component in a given scene.
   */
  void _register() override;

  /**
   * @brief Rebuilds the elements related to this component in case of
   * context lost for instance.
   */
  void rebuild() override;
  void reset();
  void render();
  void renderOcclusionBoundingBox(AbstractMesh* mesh);

  /**
   * @brief Disposes the component and the associated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Creates a new instance of the component for the given scene.
   * @param scene Defines the scene to register the component in
   */
  BoundingBoxRenderer(Scene* scene);

private:
  void _evaluateSubMesh(AbstractMesh* mesh, SubMesh* subMesh);
  void _activeMesh(AbstractMesh* sourceMesh, AbstractMesh* mesh);
  void _prepareResources();
  void _createIndexBuffer();

public:
  Color3 frontColor;
  Color3 backColor;
  bool showBackLines;
  vector_t<BoundingBox> renderList;

private:
  ShaderMaterialPtr _colorShader;
  vector_t<unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unordered_map_t<string_t, VertexBuffer*> _vertexBuffersMap;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;

}; // end of class BoundingBoxRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
