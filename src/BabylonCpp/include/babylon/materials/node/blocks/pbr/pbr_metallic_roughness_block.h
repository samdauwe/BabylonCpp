#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_PBR_METALLIC_ROUGHNESS_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_PBR_METALLIC_ROUGHNESS_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class BaseTexture;
class Light;
class PBRMetallicRoughnessBlock;
using BaseTexturePtr               = std::shared_ptr<BaseTexture>;
using LightPtr                     = std::shared_ptr<Light>;
using PBRMetallicRoughnessBlockPtr = std::shared_ptr<PBRMetallicRoughnessBlock>;

/**
 * @brief Block used to implement the PBR metallic/roughness model
 */
class BABYLON_SHARED_EXPORT PBRMetallicRoughnessBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static PBRMetallicRoughnessBlockPtr New(Ts&&... args)
  {
    auto pbrMetallicRoughnessBlock = std::shared_ptr<PBRMetallicRoughnessBlock>(
      new PBRMetallicRoughnessBlock(std::forward<Ts>(args)...));
    PBRMetallicRoughnessBlock::RegisterConnections(pbrMetallicRoughnessBlock);

    return pbrMetallicRoughnessBlock;
  }
  ~PBRMetallicRoughnessBlock() override; // = default

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const PBRMetallicRoughnessBlockPtr& pbrMetallicRoughnessBlock);

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& material) override;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false,
                      SubMesh* subMesh = nullptr) override;

  /**
   * @brief Add uniforms, samplers and uniform buffers at compilation time.
   * @param state defines the state to update
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param uniformBuffers defines the list of uniform buffer names
   */
  void updateUniformsAndSamples(NodeMaterialBuildState& state, const NodeMaterialPtr& nodeMaterial,
                                const NodeMaterialDefines& defines,
                                std::vector<std::string>& uniformBuffers) override;

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

  /**
   * @brief Gets the code corresponding to the albedo/opacity module.
   * @returns the shader code
   */
  std::string getAlbedoOpacityCode() const;

  /**
   * @brief Serializes this block in a JSON representation.
   * @returns the serialized block object
   */
  json serialize() const override;

  /**
   * @brief Hidden
   */
  void _deserialize(const json& serializationObject, Scene* scene,
                    const std::string& rootUrl) override;

protected:
  /**
   * @brief Creates a new PBRMetallicRoughnessBlock.
   * @param name defines the block name
   */
  PBRMetallicRoughnessBlock(const std::string& name);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the world normal input component.
   */
  NodeMaterialConnectionPointPtr& get_worldNormal();

  /**
   * @brief Gets the perturbed normal input component.
   */
  NodeMaterialConnectionPointPtr& get_perturbedNormal();

  /**
   * @brief Gets the camera position input component.
   */
  NodeMaterialConnectionPointPtr& get_cameraPosition();

  /**
   * @brief Gets the base color input component.
   */
  NodeMaterialConnectionPointPtr& get_baseColor();

  /**
   * @brief Gets the opacity texture input component.
   */
  NodeMaterialConnectionPointPtr& get_opacityTexture();

  /**
   * @brief Gets the ambient color input component.
   */
  NodeMaterialConnectionPointPtr& get_ambientColor();

  /**
   * @brief Gets the reflectivity object parameters.
   */
  NodeMaterialConnectionPointPtr& get_reflectivity();

  /**
   * @brief Gets the ambient occlusion object parameters.
   */
  NodeMaterialConnectionPointPtr& get_ambientOcclusion();

  /**
   * @brief Gets the reflection object parameters.
   */
  NodeMaterialConnectionPointPtr& get_reflection();

  /**
   * @brief Gets the sheen object parameters.
   */
  NodeMaterialConnectionPointPtr& get_sheen();

  /**
   * @brief Gets the clear coat object parameters.
   */
  NodeMaterialConnectionPointPtr& get_clearcoat();

  /**
   * @brief Gets the sub surface object parameters.
   */
  NodeMaterialConnectionPointPtr& get_subsurface();

  /**
   * @brief Gets the anisotropy object parameters.
   */
  NodeMaterialConnectionPointPtr& get_anisotropy();

  /**
   * @brief Gets the ambient output component.
   */
  NodeMaterialConnectionPointPtr& get_ambient();

  /**
   * @brief Gets the diffuse output component.
   */
  NodeMaterialConnectionPointPtr& get_diffuse();

  /**
   * @brief Gets the specular output component.
   */
  NodeMaterialConnectionPointPtr& get_specular();

  /**
   * @brief Gets the sheen output component.
   */
  NodeMaterialConnectionPointPtr& get_sheenDir();

  /**
   * @brief Gets the clear coat output component.
   */
  NodeMaterialConnectionPointPtr& get_clearcoatDir();

  /**
   * @brief Gets the indirect diffuse output component.
   */
  NodeMaterialConnectionPointPtr& get_diffuseIndirect();

  /**
   * @brief Gets the indirect specular output component.
   */
  NodeMaterialConnectionPointPtr& get_specularIndirect();

  /**
   * @brief Gets the indirect sheen output component.
   */
  NodeMaterialConnectionPointPtr& get_sheenIndirect();

  /**
   * @brief Gets the indirect clear coat output component.
   */
  NodeMaterialConnectionPointPtr& get_clearcoatIndirect();

  /**
   * @brief Gets the refraction output component.
   */
  NodeMaterialConnectionPointPtr& get_refraction();

  /**
   * @brief Gets the global lighting output component.
   */
  NodeMaterialConnectionPointPtr& get_lighting();

  /**
   * @brief Gets the shadow output component.
   */
  NodeMaterialConnectionPointPtr& get_shadow();

  /**
   * @brief Gets the alpha output component.
   */
  NodeMaterialConnectionPointPtr& get_alpha();

  /**
   * @brief Hidden
   */
  PBRMetallicRoughnessBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

private:
  void _injectVertexCode(NodeMaterialBuildState& state);

public:
  /**
   * Gets or sets the light associated with this block
   */
  LightPtr light;

  // clang-format off

  /**
   * Intensity of the direct lights e.g. the four lights available in your scene.
   * This impacts both the direct diffuse and specular highlights.
   */
  // @editableInPropertyPage("Direct lights", PropertyTypeForEdition.Float, "INTENSITY", { min: 0, max: 1, "notifiers": { "update": true }})
  float directIntensity;

  /**
   * Intensity of the environment e.g. how much the environment will light the object
   * either through harmonics for rough material or through the refelction for shiny ones.
   */
  // @editableInPropertyPage("Environment lights", PropertyTypeForEdition.Float, "INTENSITY", { min: 0, max: 1, "notifiers": { "update": true }})
  float environmentIntensity;

  /**
   * This is a special control allowing the reduction of the specular highlights coming from the
   * four lights of the scene. Those highlights may not be needed in full environment lighting.
   */
  // @editableInPropertyPage("Specular highlights", PropertyTypeForEdition.Float, "INTENSITY", { min: 0, max: 1, "notifiers": { "update": true }})
  float specularIntensity;

  /**
   * Defines the  falloff type used in this material.
   * It by default is Physical.
   */
  // @editableInPropertyPage("Light falloff", PropertyTypeForEdition.List, "LIGHTING & COLORS", { "notifiers": { "update": true }, "options": [
  //    { label: "Physical", value: PBRBaseMaterial.LIGHTFALLOFF_PHYSICAL },
  //    { label: "GLTF", value: PBRBaseMaterial.LIGHTFALLOFF_GLTF },
  //    { label: "Standard", value: PBRBaseMaterial.LIGHTFALLOFF_STANDARD },
  //]})
  unsigned int lightFalloff;

  /**
   * Specifies that the alpha is coming form the albedo channel alpha channel for alpha blending.
   */
  // @editableInPropertyPage("Alpha from albedo", PropertyTypeForEdition.Boolean, "TRANSPARENCY", { "notifiers": { "update": true }})
  bool useAlphaFromAlbedoTexture;

  /**
   * Specifies that alpha test should be used
   */
  // @editableInPropertyPage("Alpha Testing", PropertyTypeForEdition.Boolean, "TRANSPARENCY")
  bool useAlphaTest;

  /**
   * Defines the alpha limits in alpha test mode.
   */
  // @editableInPropertyPage("Alpha CutOff", PropertyTypeForEdition.Float, "TRANSPARENCY", { min: 0, max: 1, "notifiers": { "update": true }})
  float alphaTestCutoff;

  /**
   * Specifies that alpha blending should be used
   */
  // @editableInPropertyPage("Alpha blending", PropertyTypeForEdition.Boolean, "TRANSPARENCY")
  bool useAlphaBlending;

  /**
   * Defines if the alpha value should be determined via the rgb values.
   * If true the luminance of the pixel might be used to find the corresponding alpha value.
   */
  // @editableInPropertyPage("Get alpha from opacity texture RGB", PropertyTypeForEdition.Boolean, "TRANSPARENCY", { "notifiers": { "update": true }})
  bool opacityRGB;

  /**
   * Specifies that the material will keeps the reflection highlights over a transparent surface (only the most luminous ones).
   * A car glass is a good exemple of that. When the street lights reflects on it you can not see what is behind.
   */
  // @editableInPropertyPage("Radiance over alpha", PropertyTypeForEdition.Boolean, "RENDERING", { "notifiers": { "update": true }})
  bool useRadianceOverAlpha;

  /**
   * Specifies that the material will keeps the specular highlights over a transparent surface (only the most luminous ones).
   * A car glass is a good exemple of that. When sun reflects on it you can not see what is behind.
   */
  // @editableInPropertyPage("Specular over alpha", PropertyTypeForEdition.Boolean, "RENDERING", { "notifiers": { "update": true }})
  bool useSpecularOverAlpha;

  /**
   * Enables specular anti aliasing in the PBR shader.
   * It will both interacts on the Geometry for analytical and IBL lighting.
   * It also prefilter the roughness map based on the bump values.
   */
  // @editableInPropertyPage("Specular anti-aliasing", PropertyTypeForEdition.Boolean, "RENDERING", { "notifiers": { "update": true }})
  bool enableSpecularAntiAliasing;

  /**
   * Enables realtime filtering on the texture.
   */
  // @editableInPropertyPage("Realtime filtering", PropertyTypeForEdition.Boolean, "RENDERING", { "notifiers": { "update": true }})
  bool realTimeFiltering;

  /**
   * Quality switch for realtime filtering
   */
  // @editableInPropertyPage("Realtime filtering quality", PropertyTypeForEdition.List, "RENDERING", { "notifiers": { "update": true }, "options": [
  //     { label: "Low", value: Constants.TEXTURE_FILTERING_QUALITY_LOW },
  //     { label: "Medium", value: Constants.TEXTURE_FILTERING_QUALITY_MEDIUM },
  //     { label: "High", value: Constants.TEXTURE_FILTERING_QUALITY_HIGH },
  // ]})
  unsigned int realTimeFilteringQuality;

  /**
   * Defines if the material uses energy conservation.
   */
  // @editableInPropertyPage("Energy Conservation", PropertyTypeForEdition.Boolean, "ADVANCED", { "notifiers": { "update": true }})
  bool useEnergyConservation;

  /**
   * This parameters will enable/disable radiance occlusion by preventing the radiance to lit
   * too much the area relying on ambient texture to define their ambient occlusion.
   */
  // @editableInPropertyPage("Radiance occlusion", PropertyTypeForEdition.Boolean, "ADVANCED", { "notifiers": { "update": true }})
  bool useRadianceOcclusion;

  /**
   * This parameters will enable/disable Horizon occlusion to prevent normal maps to look shiny when the normal
   * makes the reflect vector face the model (under horizon).
   */
  // @editableInPropertyPage("Horizon occlusion", PropertyTypeForEdition.Boolean, "ADVANCED", { "notifiers": { "update": true }})
  bool useHorizonOcclusion;

  /**
   * If set to true, no lighting calculations will be applied.
   */
  // @editableInPropertyPage("Unlit", PropertyTypeForEdition.Boolean, "ADVANCED", { "notifiers": { "update": true }})
  bool unlit;

  /**
   * Force normal to face away from face.
   */
  // @editableInPropertyPage("Force normal forward", PropertyTypeForEdition.Boolean, "ADVANCED", { "notifiers": { "update": true }})
  bool forceNormalForward;

  /**
   * Defines the material debug mode.
   * It helps seeing only some components of the material while troubleshooting.
   */
  // @editableInPropertyPage("Debug mode", PropertyTypeForEdition.List, "DEBUG", { "notifiers": { "update": true }, "options": [
  //     { label: "None", value: 0 },
  //     // Geometry
  //     { label: "Normalized position", value: 1 },
  //     { label: "Normals", value: 2 },
  //     { label: "Tangents", value: 3 },
  //     { label: "Bitangents", value: 4 },
  //     { label: "Bump Normals", value: 5 },
  //     //{ label: "UV1", value: 6 },
  //     //{ label: "UV2", value: 7 },
  //     { label: "ClearCoat Normals", value: 8 },
  //     { label: "ClearCoat Tangents", value: 9 },
  //     { label: "ClearCoat Bitangents", value: 10 },
  //     { label: "Anisotropic Normals", value: 11 },
  //     { label: "Anisotropic Tangents", value: 12 },
  //     { label: "Anisotropic Bitangents", value: 13 },
  //     // Maps
  //     { label: "Albedo Map", value: 20 },
  //     { label: "Ambient Map", value: 21 },
  //     { label: "Opacity Map", value: 22 },
  //     //{ label: "Emissive Map", value: 23 },
  //     //{ label: "Light Map", value: 24 },
  //     { label: "Metallic Map", value: 25 },
  //     { label: "Reflectivity Map", value: 26 },
  //     { label: "ClearCoat Map", value: 27 },
  //     { label: "ClearCoat Tint Map", value: 28 },
  //     { label: "Sheen Map", value: 29 },
  //     { label: "Anisotropic Map", value: 30 },
  //     { label: "Thickness Map", value: 31 },
  //     // Env
  //     { label: "Env Refraction", value: 40 },
  //     { label: "Env Reflection", value: 41 },
  //     { label: "Env Clear Coat", value: 42 },
  //     // Lighting
  //     { label: "Direct Diffuse", value: 50 },
  //     { label: "Direct Specular", value: 51 },
  //     { label: "Direct Clear Coat", value: 52 },
  //     { label: "Direct Sheen", value: 53 },
  //     { label: "Env Irradiance", value: 54 },
  //     // Lighting Params
  //     { label: "Surface Albedo", value: 60 },
  //     { label: "Reflectance 0", value: 61 },
  //     { label: "Metallic", value: 62 },
  //     { label: "Metallic F0", value: 71 },
  //     { label: "Roughness", value: 63 },
  //     { label: "AlphaG", value: 64 },
  //     { label: "NdotV", value: 65 },
  //     { label: "ClearCoat Color", value: 66 },
  //     { label: "ClearCoat Roughness", value: 67 },
  //     { label: "ClearCoat NdotV", value: 68 },
  //     { label: "Transmittance", value: 69 },
  //     { label: "Refraction Transmittance", value: 70 },
  //     // Misc
  //     { label: "SEO", value: 80 },
  //     { label: "EHO", value: 81 },
  //     { label: "Energy Factor", value: 82 },
  //     { label: "Specular Reflectance", value: 83 },
  //     { label: "Clear Coat Reflectance", value: 84 },
  //     { label: "Sheen Reflectance", value: 85 },
  //     { label: "Luminance Over Alpha", value: 86 },
  //     { label: "Alpha", value: 87 },
  // ]})
  unsigned int debugMode;

  /**
   * Specify from where on screen the debug mode should start.
   * The value goes from -1 (full screen) to 1 (not visible)
   * It helps with side by side comparison against the final render
   * This defaults to 0
   */
  // @editableInPropertyPage("Split position", PropertyTypeForEdition.Float, "DEBUG", { min: -1, max: 1, "notifiers": { "update": true }})
  float debugLimit;

  /**
   * As the default viewing range might not be enough (if the ambient is really small for instance)
   * You can use the factor to better multiply the final value.
   */
  // @editableInPropertyPage("Output factor", PropertyTypeForEdition.Float, "DEBUG", { min: 0, max: 5, "notifiers": { "update": true }})
  float debugFactor;

  // clang-format on

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the world normal input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> worldNormal;

  /**
   * Gets the perturbed normal input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> perturbedNormal;

  /**
   * Gets the camera position input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> cameraPosition;

  /**
   * Gets the base color input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> baseColor;

  /**
   * Gets the opacity texture input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> opacityTexture;

  /**
   * Gets the ambient color input component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> ambientColor;

  /**
   * Gets the reflectivity object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> reflectivity;

  /**
   * Gets the ambient occlusion object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> ambientOcclusion;

  /**
   * Gets the reflection object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> reflection;

  /**
   * Gets the sheen object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> sheen;

  /**
   * Gets the clear coat object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> clearcoat;

  /**
   * Gets the sub surface object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> subsurface;

  /**
   * Gets the anisotropy object parameters
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> anisotropy;

  /**
   * Gets the ambient output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> ambient;

  /**
   * Gets the diffuse output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> diffuse;

  /**
   * Gets the specular output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> specular;

  /**
   * Gets the sheen output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> sheenDir;

  /**
   * Gets the clear coat output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> clearcoatDir;

  /**
   * Gets the indirect diffuse output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> diffuseIndirect;

  /**
   * Gets the indirect specular output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> specularIndirect;

  /**
   * Gets the indirect sheen output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> sheenIndirect;

  /**
   * Gets the indirect clear coat output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> clearcoatIndirect;

  /**
   * Gets the refraction output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> refraction;

  /**
   * Gets the global lighting output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> lighting;

  /**
   * Gets the shadow output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> shadow;

  /**
   * Gets the alpha output component
   */
  ReadOnlyProperty<PBRMetallicRoughnessBlock, NodeMaterialConnectionPointPtr> alpha;

private:
  unsigned int _lightId;
  Scene* _scene;
  BaseTexturePtr _environmentBRDFTexture;
  std::string _environmentBrdfSamplerName;
  std::string _vNormalWName;
  std::string _invertNormalName;

}; // end of class PBRMetallicRoughnessBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_PBR_METALLIC_ROUGHNESS_BLOCK_H
