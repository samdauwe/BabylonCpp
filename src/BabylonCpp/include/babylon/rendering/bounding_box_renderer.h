#ifndef BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
#define BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/engine/iscene_component.h>
#include <babylon/engine/scene_component_constants.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class AbstractMesh;
class BoundingBox;
class BoundingBoxRenderer;
class ShaderMaterial;
class SubMesh;
class VertexBuffer;
using BoundingBoxRendererPtr = std::shared_ptr<BoundingBoxRenderer>;
using ShaderMaterialPtr      = std::shared_ptr<ShaderMaterial>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

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
    auto renderer = std::shared_ptr<BoundingBoxRenderer>(
      new BoundingBoxRenderer(std::forward<Ts>(args)...));
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
  std::vector<BoundingBox> renderList;

private:
  ShaderMaterialPtr _colorShader;
  std::vector<std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unordered_map<std::string, VertexBuffer*> _vertexBuffersMap;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;

}; // end of class BoundingBoxRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
