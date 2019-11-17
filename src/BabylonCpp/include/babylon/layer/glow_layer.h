#ifndef BABYLON_LAYER_GLOW_LAYER_H
#define BABYLON_LAYER_GLOW_LAYER_H

#include <babylon/babylon_api.h>
#include <babylon/layer/effect_layer.h>
#include <babylon/layer/iglow_layer_options.h>

namespace BABYLON {

class AbstractMesh;
class BlurPostProcess;
class GlowLayer;
class Material;
class Mesh;
class SubMesh;
class Texture;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;
using BlurPostProcessPtr = std::shared_ptr<BlurPostProcess>;
using GlowLayerPtr       = std::shared_ptr<GlowLayer>;
using MaterialPtr        = std::shared_ptr<Material>;
using MeshPtr            = std::shared_ptr<Mesh>;
using SubMeshPtr         = std::shared_ptr<SubMesh>;
using TexturePtr         = std::shared_ptr<Texture>;

/**
 * @brief The glow layer Helps adding a glow effect around the emissive parts of
 * a mesh.
 *
 * Once instantiated in a scene, simply use the pushMesh or removeMesh method to
 * add or remove glowy meshes to your scene.
 *
 * Documentation: https://doc.babylonjs.com/how_to/glow_layer
 */
class BABYLON_SHARED_EXPORT GlowLayer : public EffectLayer {

public:
  /**
   * Effect Name of the layer.
   */
  static constexpr const char* EffectName = "GlowLayer";

  /**
   * The default blur kernel size used for the glow.
   */
  static constexpr unsigned int DefaultBlurKernelSize = 32;

  /**
   * The default texture size ratio used for the glow.
   */
  static constexpr float DefaultTextureRatio = 0.5f;

public:
  template <typename... Ts>
  static GlowLayerPtr New(Ts&&... args)
  {
    auto layer = std::shared_ptr<GlowLayer>(new GlowLayer(std::forward<Ts>(args)...));
    layer->addToScene(layer);

    return layer;
  }
  ~GlowLayer() override; // = default

  /**
   * @brief Get the effect name of the layer.
   * @return The effect name
   */
  [[nodiscard]] std::string getEffectName() const override;

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
   * @brief Returns wether or nood the layer needs stencil enabled during the
   * mesh rendering.
   */
  [[nodiscard]] bool needStencil() const override;

  /**
   * @brief Add a mesh in the exclusion list to prevent it to impact or being
   * impacted by the glow layer.
   * @param mesh The mesh to exclude from the glow layer
   */
  void addExcludedMesh(Mesh* mesh);

  /**
   * @brief Remove a mesh from the exclusion list to let it impact or being
   * impacted by the glow layer.
   * @param mesh The mesh to remove
   */
  void removeExcludedMesh(Mesh* mesh);

  /**
   * @brief Add a mesh in the inclusion list to impact or being impacted by the
   * glow layer.
   * @param mesh The mesh to include in the glow layer
   */
  void addIncludedOnlyMesh(Mesh* mesh);

  /**
   * @brief Remove a mesh from the Inclusion list to prevent it to impact or
   * being impacted by the glow layer.
   * @param mesh The mesh to remove
   */
  void removeIncludedOnlyMesh(Mesh* mesh);

  /**
   * @brief Determine if a given mesh will be used in the glow layer
   * @param mesh The mesh to test
   * @returns true if the mesh will be highlighted by the current glow layer
   */
  bool hasMesh(AbstractMesh* mesh) const override;

  /**
   * @brief Free any resources and references associated to a mesh.
   * Internal use
   * @param mesh The mesh to free.
   * Hidden
   */
  void _disposeMesh(Mesh* mesh) override;

  /**
   * @brief Gets the class name of the effect layer.
   * @returns the string with the class name of the effect layer
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Serializes this glow layer.
   * @returns a serialized glow layer object
   */
  [[nodiscard]] json serialize() const override;

  /**
   * @brief Creates a Glow Layer from parsed glow layer data.
   * @param parsedGlowLayer defines glow layer data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing the glow layer information
   * @returns a parsed Glow Layer
   */
  static GlowLayer* Parse(const json& parsedGlowLayer, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new glow Layer and references it to the scene.
   * @param name The name of the layer
   * @param scene The scene to use the layer in
   * @param options Sets of none mandatory options to use with the layer (see
   * IGlowLayerOptions for more information)
   */
  GlowLayer(const std::string& name, Scene* scene);
  GlowLayer(const std::string& name, Scene* scene, const IGlowLayerOptions& options);

  /**
   * @brief Sets the kernel size of the blur.
   */
  void set_blurKernelSize(float value);

  /**
   * @brief Gets the kernel size of the blur.
   */
  [[nodiscard]] float get_blurKernelSize() const;

  /**
   * @brief Sets the glow intensity.
   */
  void set_intensity(float value);

  /**
   * @brief Gets the glow intensity.
   */
  [[nodiscard]] float get_intensity() const;

  /**
   * @brief Create the merge effect. This is the shader use to blit the
   * information back to the main canvas at the end of the scene rendering.
   */
  EffectPtr _createMergeEffect() override;

  /**
   * @brief Creates the render target textures and post processes used in the
   * glow layer.
   */
  void _createTextureAndPostProcesses() override;

  /**
   * @brief Returns true if the mesh can be rendered, otherwise false.
   * @param mesh The mesh to render
   * @param material The material used on the mesh
   * @returns true if it can be rendered otherwise false
   */
  [[nodiscard]] bool _canRenderMesh(const AbstractMeshPtr& mesh,
                                    const MaterialPtr& material) const override;

  /**
   * @brief Implementation specific of rendering the generating effect on the
   * main canvas.
   * @param effect The effect used to render through
   */
  void _internalRender(const EffectPtr& effect) override;

  /**
   * @brief Sets the required values for both the emissive texture and and the
   * main color.
   */
  void _setEmissiveTextureAndColor(const MeshPtr& mesh, SubMesh* subMesh,
                                   const MaterialPtr& material) override;

  /**
   * @brief Returns true if the mesh should render, otherwise false.
   * @param mesh The mesh to render
   * @returns true if it should render otherwise false
   */
  bool _shouldRenderMesh(AbstractMesh* mesh) const override;

  /**
   * @brief Adds specific effects defines.
   * @param defines The defines to add specifics to.
   */
  void _addCustomEffectDefines(std::vector<std::string>& defines) override;

public:
  /**
   * Callback used to let the user override the color selection on a per mesh
   * basis
   */
  std::function<void(const MeshPtr& mesh, SubMesh* subMesh, Material* material, Color4& result)>
    customEmissiveColorSelector;
  /**
   * Callback used to let the user override the texture selection on a per mesh
   * basis
   */
  std::function<TexturePtr(const MeshPtr& mesh, SubMesh* subMesh, const MaterialPtr& material)>
    customEmissiveTextureSelector;

  Property<GlowLayer, float> blurKernelSize;
  Property<GlowLayer, float> intensity;

private:
  IGlowLayerOptions _options;
  float _intensity;
  BlurPostProcessPtr _horizontalBlurPostprocess1;
  BlurPostProcessPtr _verticalBlurPostprocess1;
  BlurPostProcessPtr _horizontalBlurPostprocess2;
  BlurPostProcessPtr _verticalBlurPostprocess2;
  RenderTargetTexturePtr _blurTexture1;
  RenderTargetTexturePtr _blurTexture2;
  std::vector<PostProcessPtr> _postProcesses1;
  std::vector<PostProcessPtr> _postProcesses2;

  Uint64Array _includedOnlyMeshes;
  Uint64Array _excludedMeshes;

}; // end of struct GlowLayer

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_GLOW_LAYER_H
