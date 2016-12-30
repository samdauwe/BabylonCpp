#ifndef BABYLON_LAYER_HIGHLIGHT_LAYER_H
#define BABYLON_LAYER_HIGHLIGHT_LAYER_H

#include <babylon/babylon_global.h>
#include <babylon/layer/ihighlight_layer_excluded_mesh.h>
#include <babylon/layer/ihighlight_layer_mesh.h>
#include <babylon/layer/ihighlight_layer_options.h>
#include <babylon/math/color4.h>
#include <babylon/math/isize.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief The highlight layer Helps adding a glow effect around a mesh.
 *
 * Once instantiated in a scene, simply use the pushMesh or removeMesh method to
 * add or remove
 * glowy meshes to your scene.
 *
 * !!! THIS REQUIRES AN ACTIVE STENCIL BUFFER ON THE CANVAS !!!
 */
class BABYLON_SHARED_EXPORT HighlightLayer {

public:
  /**
   * The neutral color used during the preparation of the glow effect.
   * This is black by default as the blend operation is a blend operation.
   */
  static Color4 neutralColor;

  /**
   * Stencil value used for glowing meshes.
   */
  static int glowingMeshStencilReference;

  /**
   * Stencil value used for the other meshes in the scene.
   */
  static int normalMeshStencilReference;

public:
  template <typename... Ts>
  static HighlightLayer* New(Ts&&... args);
  ~HighlightLayer();

  /**
   * Specifies the horizontal size of the blur.
   */
  void setBlurHorizontalSize(float value);

  /**
   * Specifies the vertical size of the blur.
   */
  void setBlurVerticalSize(float value);

  /**
   * Gets the horizontal size of the blur.
   */
  float blurHorizontalSize() const;

  /**
   * Gets the vertical size of the blur.
   */
  float blurVerticalSize() const;

  /**
   * Gets the camera attached to the layer.
   */
  Camera* camera() const;

  /**
   * Gets the render target texture.
   */
  RenderTargetTexture* mainTexture() const;

  /**
   * Renders the glowing part of the scene by blending the blurred glowing
   * meshes on top of the rendered scene.
   */
  void render();

  /**
   * Add a mesh in the exclusion list to prevent it to impact or being impacted
   * by the highlight layer.
   * @param mesh The mesh to exclude from the highlight layer
   */
  void addExcludedMesh(Mesh* mesh);

  /**
    * Remove a mesh from the exclusion list to let it impact or being impacted
   * by the highlight layer.
    * @param mesh The mesh to highlight
    */
  void removeExcludedMesh(Mesh* mesh);

  /**
   * Add a mesh in the highlight layer in order to make it glow with the
   * chosen color.
   * @param mesh The mesh to highlight
   * @param color The color of the highlight
   * @param glowEmissiveOnly Extract the glow from the emissive texture
   */
  void addMesh(Mesh* mesh, const Color3& color, bool glowEmissiveOnly = false);

  /**
   * Remove a mesh from the highlight layer in order to make it stop glowing.
   * @param mesh The mesh to highlight
   */
  void removeMesh(Mesh* mesh);

  /**
   * Returns true if the layer contains information to display, otherwise false.
   */
  bool shouldRender() const;

  /**
   * Dispose the highlight layer and free resources.
   */
  void dispose();

protected:
  /**
   * Instantiates a new highlight Layer and references it to the scene..
   * @param name The name of the layer
   * @param scene The scene to use the layer in
   * @param options Sets of none mandatory options to use with the layer (see
   * IHighlightLayerOptions for more information)
   */
  HighlightLayer(const std::string& name, Scene* scene);
  HighlightLayer(const std::string& name, Scene* scene,
                 const IHighlightLayerOptions& options);

private:
  /**
   * Creates the render target textures and post processes used in the
   * highlight
   * layer.
   */
  void createTextureAndPostProcesses();

  /**
   * Custom render function.
   */
  void renderSubMesh(SubMesh* subMesh);

  /**
   * Checks for the readiness of the element composing the layer.
   * @param subMesh the mesh to check for
   * @param useInstances specify wether or not to use instances to render the
   * mesh
   * @param emissiveTexture the associated emissive texture used to generate the
   * glow
   * @return true if ready otherwise, false
   */
  bool isReady(SubMesh* subMesh, bool useInstances,
               BaseTexture* emissiveTexture);

  /**
   * Sets the main texture desired size which is the closest power of two
   * of the engine canvas size.
   */
  void setMainTextureSize();

  /**
   * Force the stencil to the normal expected value for none glowing parts
   */
  void defaultStencilReference(Mesh* mesh);

  /**
   * Dispose only the render target textures and post process.
   */
  void disposeTextureAndPostProcesses();

public:
  /**
   * An event triggered when the highlight layer has been disposed.
   */
  Observable<HighlightLayer> onDisposeObservable;

  /**
   * An event triggered when the highlight layer is about rendering the main
   * texture with the glowy parts.
   */
  Observable<HighlightLayer> onBeforeRenderMainTextureObservable;

  /**
   * An event triggered when the highlight layer is being blurred.
   */
  Observable<HighlightLayer> onBeforeBlurObservable;

  /**
   * An event triggered when the highlight layer has been blurred.
   */
  Observable<HighlightLayer> onAfterBlurObservable;

  /**
   * An event triggered when the glowing blurred texture is being merged in the
   * scene.
   */
  Observable<HighlightLayer> onBeforeComposeObservable;

  /**
   * An event triggered when the glowing blurred texture has been merged in the
   * scene.
   */
  Observable<HighlightLayer> onAfterComposeObservable;

  /**
   * An event triggered when the highlight layer changes its size.
   */
  Observable<HighlightLayer> onSizeChangedObservable;

public:
  std::string name;

  /**
   * Specifies whether or not the inner glow is ACTIVE in the layer.
   */
  bool innerGlow;

  /**
   * Specifies whether or not the outer glow is ACTIVE in the layer.
   */
  bool outerGlow;

  /**
   * Specifies wether the highlight layer is enabled or not.
   */
  bool isEnabled;

private:
  Scene* _scene;
  Engine* _engine;
  IHighlightLayerOptions _options;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  std::unique_ptr<PassPostProcess> _downSamplePostprocess;
  std::unique_ptr<GlowBlurPostProcess> _horizontalBlurPostprocess;
  std::unique_ptr<GlowBlurPostProcess> _verticalBlurPostprocess;
  std::string _cachedDefines;
  Effect* _glowMapGenerationEffect;
  Effect* _glowMapMergeEffect;
  std::unique_ptr<RenderTargetTexture> _blurTexture;
  std::unique_ptr<RenderTargetTexture> _mainTexture;
  ISize _mainTextureDesiredSize;
  std::unordered_map<std::string, IHighlightLayerMesh> _meshes;
  int _maxSize;
  bool _shouldRender;
  int _instanceGlowingMeshStencilReference;
  std::unordered_map<std::string, IHighlightLayerExcludedMesh> _excludedMeshes;

}; // end of struct HighlightLayer

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_HIGHLIGHT_LAYER_H
