#ifndef BABYLON_LAYER_HIGHLIGHT_LAYER_H
#define BABYLON_LAYER_HIGHLIGHT_LAYER_H

#include <babylon/babylon_api.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/ihighlight_layer_excluded_mesh.h>
#include <babylon/layer/ihighlight_layer_mesh.h>
#include <babylon/layer/ihighlight_layer_options.h>
#include <babylon/math/color4.h>
#include <babylon/math/isize.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class GlowBlurPostProcess;
class HighlightLayer;
class PassPostProcess;
using GlowBlurPostProcessPtr = std::shared_ptr<GlowBlurPostProcess>;
using HighlightLayerPtr      = std::shared_ptr<HighlightLayer>;
using PassPostProcessPtr     = std::shared_ptr<PassPostProcess>;

/**
 * @brief The highlight layer Helps adding a glow effect around a mesh.
 *
 * Once instantiated in a scene, simply use the pushMesh or removeMesh method to
 * add or remove
 * glowy meshes to your scene.
 *
 * !!! THIS REQUIRES AN ACTIVE STENCIL BUFFER ON THE CANVAS !!!
 */
class BABYLON_SHARED_EXPORT HighlightLayer : public EffectLayer {

public:
  /**
   * Effect Name of the highlight layer.
   */
  static constexpr const char* EffectName = "HighlightLayer";

  /**
   * The neutral color used during the preparation of the glow effect.
   * This is black by default as the blend operation is a blend operation.
   */
  static Color4 NeutralColor;

  /**
   * Stencil value used for glowing meshes.
   */
  static int GlowingMeshStencilReference;

  /**
   * Stencil value used for the other meshes in the scene.
   */
  static int NormalMeshStencilReference;

public:
  template <typename... Ts>
  static HighlightLayerPtr New(Ts&&... args)
  {
    auto layer = std::shared_ptr<HighlightLayer>(
      new HighlightLayer(std::forward<Ts>(args)...));
    layer->addToScene(layer);

    return layer;
  }
  ~HighlightLayer() override;

  /**
   * @brief Get the effect name of the layer.
   * @return The effect name
   */
  std::string getEffectName() const override;

  /**
   * @brief Returns wether or nood the layer needs stencil enabled during the
   * mesh rendering.
   */
  bool needStencil() const override;

  /**
   * @brief Checks for the readiness of the element composing the layer.
   * @param subMesh the mesh to check for
   * @param useInstances specify wether or not to use instances to render the
   * mesh
   * @param emissiveTexture the associated emissive texture used to generate the
   * glow
   * @return true if ready otherwise, false
   */
  bool isReady(SubMesh* subMesh, bool useInstances) override;

  /**
   * @brief Returns true if the layer contains information to display, otherwise
   * false.
   */
  bool shouldRender() const override;

  /**
   * @brief Add a mesh in the exclusion list to prevent it to impact or being
   * impacted by the highlight layer.
   * @param mesh The mesh to exclude from the highlight layer
   */
  void addExcludedMesh(Mesh* mesh);

  /**
   * @brief Remove a mesh from the exclusion list to let it impact or being
   * impacted by the highlight layer.
   * @param mesh The mesh to highlight
   */
  void removeExcludedMesh(Mesh* mesh);

  /**
   * @brief Determine if a given mesh will be highlighted by the current
   * HighlightLayer
   * @param mesh mesh to test
   * @returns true if the mesh will be highlighted by the current HighlightLayer
   */
  bool hasMesh(AbstractMesh* mesh) const override;

  /**
   * @brief Add a mesh in the highlight layer in order to make it glow with the
   * chosen color.
   * @param mesh The mesh to highlight
   * @param color The color of the highlight
   * @param glowEmissiveOnly Extract the glow from the emissive texture
   */
  void addMesh(const MeshPtr& mesh, const Color3& color,
               bool glowEmissiveOnly = false);

  /**
   * @brief Remove a mesh from the highlight layer in order to make it stop
   * glowing.
   * @param mesh The mesh to highlight
   */
  void removeMesh(Mesh* mesh);

  /**
   * Free any resources and references associated to a mesh.
   * Internal use
   * @param mesh The mesh to free.
   * Hidden
   */
  void _disposeMesh(Mesh* mesh) override;

  /**
   * @brief Dispose the highlight layer and free resources.
   */
  void dispose();

  /**
   * @brief Gets the class name of the effect layer.
   * @returns the string with the class name of the effect layer
   */
  std::string getClassName() const override;

  /**
   * @brief Serializes this Highlight layer.
   * @returns a serialized Highlight layer object
   */
  json serialize() const override;

  /**
   * @brief Creates a Highlight layer from parsed Highlight layer data.
   * @param parsedHightlightLayer defines the Highlight layer data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing the Highlight layer
   * information
   * @returns a parsed Highlight layer
   */
  static HighlightLayer* Parse(const json& parsedHightlightLayer, Scene* scene,
                               const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new highlight Layer and references it to the scene..
   * @param name The name of the layer
   * @param scene The scene to use the layer in
   * @param options Sets of none mandatory options to use with the layer (see
   * IHighlightLayerOptions for more information)
   */
  HighlightLayer(const std::string& name, Scene* scene);
  HighlightLayer(const std::string& name, Scene* scene,
                 const IHighlightLayerOptions& options);

  /**
   * @brief Create the merge effect. This is the shader use to blit the
   * information back to the main canvas at the end of the scene rendering.
   */
  EffectPtr _createMergeEffect() override;

  /**
   * @brief Creates the render target textures and post processes used in the
   * highlight layer.
   */
  void _createTextureAndPostProcesses() override;

  /**
   * @brief Implementation specific of rendering the generating effect on the
   * main canvas.
   * @param effect The effect used to render through
   */
  void _internalRender(const EffectPtr& effect) override;

  /**
   * @brief Returns true if the mesh should render, otherwise false.
   * @param mesh The mesh to render
   * @returns true if it should render otherwise false
   */
  bool _shouldRenderMesh(const MeshPtr& mesh) const override;

  /**
   * @brief Sets the required values for both the emissive texture and and the
   * main color.
   */
  void _setEmissiveTextureAndColor(const MeshPtr& mesh, SubMesh* subMesh,
                                   const MaterialPtr& material) override;

private:
  /**
   * @brief Specifies the horizontal size of the blur.
   */
  void set_blurHorizontalSize(float value);

  /**
   * @brief Specifies the vertical size of the blur.
   */
  void set_blurVerticalSize(float value);

  /**
   * @brief Gets the horizontal size of the blur.
   */
  float get_blurHorizontalSize() const;

  /**
   * @brief Gets the vertical size of the blur.
   */
  float get_blurVerticalSize() const;

  /**
   * @brief Force the stencil to the normal expected value for none glowing
   * parts
   */
  void _defaultStencilReference(Mesh* mesh);

public:
  /**
   * Specifies whether or not the inner glow is ACTIVE in the layer.
   */
  bool innerGlow;

  /**
   * Specifies whether or not the outer glow is ACTIVE in the layer.
   */
  bool outerGlow;

  /**
   * Specifies the horizontal size of the blur.
   */
  Property<HighlightLayer, float> blurHorizontalSize;

  /**
   * Specifies the vertical size of the blur.
   */
  Property<HighlightLayer, float> blurVerticalSize;

  /**
   * An event triggered when the highlight layer is being blurred.
   */
  Observable<HighlightLayer> onBeforeBlurObservable;

  /**
   * An event triggered when the highlight layer has been blurred.
   */
  Observable<HighlightLayer> onAfterBlurObservable;

private:
  IHighlightLayerOptions _options;
  unsigned int _instanceGlowingMeshStencilReference;
  PassPostProcessPtr _downSamplePostprocess;
  GlowBlurPostProcessPtr _horizontalBlurPostprocess;
  GlowBlurPostProcessPtr _verticalBlurPostprocess;
  RenderTargetTexturePtr _blurTexture;
  std::unordered_map<size_t, IHighlightLayerMesh> _meshes;
  std::unordered_map<size_t, IHighlightLayerExcludedMesh> _excludedMeshes;

}; // end of struct HighlightLayer

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_HIGHLIGHT_LAYER_H
