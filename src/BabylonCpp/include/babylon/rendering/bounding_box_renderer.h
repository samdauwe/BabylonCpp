#ifndef BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
#define BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/iscene_component.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class AbstractMesh;
class BoundingBox;
class SubMesh;
FWD_CLASS_SPTR(BoundingBoxRenderer)
FWD_CLASS_SPTR(ShaderMaterial)
FWD_CLASS_SPTR(VertexBuffer)
FWD_CLASS_SPTR(WebGLDataBuffer)

/**
 * @brief Component responsible of rendering the bounding box of the meshes in a
 * scene. This is usually used through the mesh.showBoundingBox or the
 * scene.forceShowBoundingBoxes properties
 */
class BABYLON_SHARED_EXPORT BoundingBoxRenderer : public ISceneComponent {

public:
  /**
   * The component name helpfull to identify the component in the list of scene
   * components.
   */
  static constexpr const char* name = SceneComponentConstants::NAME_BOUNDINGBOXRENDERER;

public:
  template <typename... Ts>
  static BoundingBoxRendererPtr New(Ts&&... args)
  {
    auto renderer
      = std::shared_ptr<BoundingBoxRenderer>(new BoundingBoxRenderer(std::forward<Ts>(args)...));
    renderer->addToScene(renderer);

    return renderer;
  }
  ~BoundingBoxRenderer() override; // = default

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

  /**
   * @brief Hidden
   */
  void reset();

  /**
   * @brief Render the bounding boxes of a specific rendering group.
   * @param renderingGroupId defines the rendering group to render
   */
  void render(int renderingGroupId);

  /**
   * @brief In case of occlusion queries, we can render the occlusion bounding
   * box through this method.
   * @param mesh Define the mesh to render the occlusion bounding box for
   */
  void renderOcclusionBoundingBox(AbstractMesh* mesh);

  /**
   * @brief Dispose and release the resources attached to this renderer.
   */
  void dispose() override;

protected:
  /**
   * @brief Instantiates a new bounding box renderer in a scene.
   * @param scene the scene the  renderer renders in
   */
  BoundingBoxRenderer(Scene* scene);

private:
  void _evaluateSubMesh(AbstractMesh* mesh, SubMesh* subMesh);
  void _preActiveMesh(AbstractMesh* mesh);
  void _prepareResources();
  void _createIndexBuffer();

public:
  /**
   * Color of the bounding box lines placed in front of an object
   */
  Color3 frontColor;

  /**
   * Color of the bounding box lines placed behind an object
   */
  Color3 backColor;

  /**
   * Defines if the renderer should show the back lines or not
   */
  bool showBackLines;

  /**
   * Observable raised before rendering a bounding box
   */
  Observable<BoundingBox> onBeforeBoxRenderingObservable;

  /**
   * Observable raised after rendering a bounding box
   */
  Observable<BoundingBox> onAfterBoxRenderingObservable;

  /**
   * Observable raised after resources are created
   */
  Observable<BoundingBoxRenderer> onResourcesReadyObservable;

  /**
   * When false, no bounding boxes will be rendered
   */
  bool enabled;

  /**
   * Hidden
   */
  std::vector<BoundingBox> renderList;

private:
  ShaderMaterialPtr _colorShader;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  WebGLDataBufferPtr _indexBuffer;
  WebGLDataBufferPtr _fillIndexBuffer;
  IndicesArray _fillIndexData;

}; // end of class BoundingBoxRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_BOUNDING_BOX_RENDERER_H
