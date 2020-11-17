#ifndef BABYLON_MATERIALS_PBR_PBR_CLEAR_COAT_CONFIGURATION_H
#define BABYLON_MATERIALS_PBR_PBR_CLEAR_COAT_CONFIGURATION_H

#include <functional>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/color3.h>

using json = nlohmann::json;

namespace BABYLON {

class Effect;
class EffectFallbacks;
class Engine;
struct IMaterialClearCoatDefines;
struct MaterialDefines;
class Scene;
class SubMesh;
class UniformBuffer;
FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(IAnimatable)
FWD_CLASS_SPTR(RenderTargetTexture)

/**
 * @brief Define the code related to the clear coat parameters of the pbr material.
 */
class BABYLON_SHARED_EXPORT PBRClearCoatConfiguration {

public:
  /**
   * This defaults to 1.5 corresponding to a 0.04 f0 or a 4% reflectance at normal incidence
   * The default fits with a polyurethane material.
   * @hidden
   */
  static constexpr float _DefaultIndexOfRefraction = 1.5f;

public:
  /**
   * @brief Instantiate a new istance of clear coat configuration.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to dirty
   */
  PBRClearCoatConfiguration(const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~PBRClearCoatConfiguration(); // = default

  /**
   * @brief Hidden
   */
  void _markAllSubMeshesAsTexturesDirty();

  /**
   * @brief Gets wehter the submesh is ready to be used or not.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene the material belongs to.
   * @param engine defines the engine the material belongs to.
   * @param disableBumpMap defines wether the material disables bump or not.
   * @returns - boolean indicating that the submesh is ready or not.
   */
  bool isReadyForSubMesh(const MaterialDefines& defines, Scene* scene, Engine* engine,
                         bool disableBumpMap) const;

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene to the material belongs to.
   */
  void prepareDefines(MaterialDefines& defines, Scene* scene);

  /**
   * @brief Binds the material data.
   * @param uniformBuffer defines the Uniform buffer to fill in.
   * @param scene defines the scene the material belongs to.
   * @param engine defines the engine the material belongs to.
   * @param disableBumpMap defines wether the material disables bump or not.
   * @param isFrozen defines wether the material is frozen or not.
   * @param invertNormalMapX If sets to true, x component of normal map value will be inverted (x
   * = 1.0 - x).
   * @param invertNormalMapY If sets to true, y component of normal map value will be inverted (y
   * = 1.0 - y).
   * @param subMesh the submesh to bind data for
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene, Engine* engine,
                      bool disableBumpMap, bool isFrozen, bool invertNormalMapX,
                      bool invertNormalMapY, SubMesh* subMesh = nullptr);

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param texture - Base texture to use.
   * @returns - Boolean specifying if a texture is used in the material.
   */
  bool hasTexture(const BaseTexturePtr& texture) const;

  /**
   * @brief Returns an array of the actively used textures.
   * @param activeTextures Array of BaseTextures
   */
  void getActiveTextures(std::vector<BaseTexturePtr>& activeTextures);

  /**
   * @brief Returns the animatable textures.
   * @param animatables Array of animatable textures.
   */
  void getAnimatables(std::vector<IAnimatablePtr>& animatables);

  /**
   * @brief Disposes the resources of the material.
   * @param forceDisposeTextures - Forces the disposal of all textures.
   */
  void dispose(bool forceDisposeTextures = false);

  /**
   * @brief Get the current class name of the texture useful for serialization or dynamic coding.
   * @returns "PBRClearCoatConfiguration"
   */
  std::string getClassName() const;

  /**
   * @brief Add fallbacks to the effect fallbacks list.
   * @param defines defines the Base texture to use.
   * @param fallbacks defines the current fallback list.
   * @param currentRank defines the current fallback rank.
   * @returns the new fallback rank.
   */
  static unsigned int AddFallbacks(const IMaterialClearCoatDefines& defines,
                                   EffectFallbacks& fallbacks, unsigned int currentRank);

  /**
   * @brief Add the required uniforms to the current list.
   * @param uniforms defines the current uniform list.
   */
  static void AddUniforms(std::vector<std::string>& uniforms);

  /**
   * @brief Add the required samplers to the current list.
   * @param samplers defines the current sampler list.
   */
  static void AddSamplers(std::vector<std::string>& samplers);

  /**
   * @brief Add the required uniforms to the current buffer.
   * @param uniformBuffer defines the current uniform buffer.
   */
  static void PrepareUniformBuffer(UniformBuffer& uniformBuffer);

  /**
   * @brief Makes a duplicate of the current configuration into another one.
   * @param clearCoatConfiguration define the config where to copy the info
   */
  void copyTo(PBRClearCoatConfiguration& configuration);

  /**
   * @brief Serializes this clear coat configuration.
   * @returns - An object with the serialized config.
   */
  json serialize() const;

  /**
   * @brief Parses a Clear Coat Configuration from a serialized object.
   * @param source - Serialized object.
   * @param scene Defines the scene we are parsing for
   * @param rootUrl Defines the rootUrl to load from
   */
  void parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);
  float get_indexOfRefraction() const;
  void set_indexOfRefraction(float value);
  BaseTexturePtr& get_texture();
  void set_texture(const BaseTexturePtr& value);
  bool get_useRoughnessFromMainTexture() const;
  void set_useRoughnessFromMainTexture(bool value);
  BaseTexturePtr& get_textureRoughness();
  void set_textureRoughness(const BaseTexturePtr& value);
  bool get_remapF0OnInterfaceChange() const;
  void set_remapF0OnInterfaceChange(bool value);
  BaseTexturePtr& get_bumpTexture();
  void set_bumpTexture(const BaseTexturePtr& value);
  bool get_isTintEnabled() const;
  void set_isTintEnabled(bool value);
  BaseTexturePtr& get_tintTexture();
  void set_tintTexture(const BaseTexturePtr& value);

public:
  /**
   * Defines if the clear coat is enabled in the material.
   */
  Property<PBRClearCoatConfiguration, bool> isEnabled;

  /**
   * Defines the clear coat layer strength (between 0 and 1) it defaults to 1.
   */
  float intensity;

  /**
   * Defines the clear coat layer roughness.
   */
  float roughness;

  /**
   * Defines the index of refraction of the clear coat.
   * This defaults to 1.5 corresponding to a 0.04 f0 or a 4% reflectance at normal incidence The
   * default fits with a polyurethane material. Changing the default value is more performance
   * intensive.
   */
  Property<PBRClearCoatConfiguration, float> indexOfRefraction;

  /**
   * Stores the clear coat values in a texture (red channel is intensity and green channel is
   * roughness) If useRoughnessFromMainTexture is false, the green channel of texture is not used
   * and the green channel of textureRoughness is used instead if textureRoughness is not empty,
   * else no texture roughness is used
   */
  Property<PBRClearCoatConfiguration, BaseTexturePtr> texture;

  /**
   * Indicates that the green channel of the texture property will be used for roughness (default:
   * true) If false, the green channel from textureRoughness is used for roughness
   */
  Property<PBRClearCoatConfiguration, bool> useRoughnessFromMainTexture;

  /**
   * Stores the clear coat roughness in a texture (green channel)
   * Not used if useRoughnessFromMainTexture is true
   */
  Property<PBRClearCoatConfiguration, BaseTexturePtr> textureRoughness;

  /**
   * Defines if the F0 value should be remapped to account for the interface change in the material.
   */
  Property<PBRClearCoatConfiguration, bool> remapF0OnInterfaceChange;

  /**
   * Define the clear coat specific bump texture.
   */
  Property<PBRClearCoatConfiguration, BaseTexturePtr> bumpTexture;

  /**
   * Defines if the clear coat tint is enabled in the material.
   */
  Property<PBRClearCoatConfiguration, bool> isTintEnabled;

  /**
   * Defines the clear coat tint of the material.
   * This is only use if tint is enabled
   */
  Color3 tintColor;

  /**
   * Defines the distance at which the tint color should be found in the clear coat media.
   * This is only use if tint is enabled
   */
  float tintColorAtDistance;

  /**
   * Defines the clear coat layer thickness.
   * This is only use if tint is enabled
   */
  float tintThickness;

  /**
   * Stores the clear tint values in a texture.
   * rgb is tint
   * a is a thickness factor
   */
  Property<PBRClearCoatConfiguration, BaseTexturePtr> tintTexture;

private:
  bool _isEnabled;
  float _indexOfRefraction;
  BaseTexturePtr _texture;
  bool _useRoughnessFromMainTexture;
  BaseTexturePtr _textureRoughness;
  bool _remapF0OnInterfaceChange;
  BaseTexturePtr _bumpTexture;
  bool _isTintEnabled;
  BaseTexturePtr _tintTexture;

  /** Hidden */
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class PBRClearCoatConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_CLEAR_COAT_CONFIGURATION_H
