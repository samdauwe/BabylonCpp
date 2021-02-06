#ifndef BABYLON_MATERIALS_PBR_PBR_SHEEN_CONFIGURATION_H
#define BABYLON_MATERIALS_PBR_PBR_SHEEN_CONFIGURATION_H

#include <functional>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/color3.h>

using json = nlohmann::json;

namespace BABYLON {

class BaseTexture;
class EffectFallbacks;
class IAnimatable;
struct MaterialDefines;
class Scene;
class SubMesh;
class UniformBuffer;
FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(IAnimatable)

/**
 * @brief Define the code related to the BRDF parameters of the pbr material.
 */
class BABYLON_SHARED_EXPORT PBRSheenConfiguration {

public:
  /**
   * @brief Instantiate a new instance of clear coat configuration.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to dirty
   */
  PBRSheenConfiguration(const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~PBRSheenConfiguration(); // = default

  /**
   * @brief Hidden
   */
  void _markAllSubMeshesAsTexturesDirty();

  /**
   * @brief Specifies that the submesh is ready to be used.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene the material belongs to.
   * @returns - boolean indicating that the submesh is ready or not.
   */
  bool isReadyForSubMesh(const MaterialDefines& defines, Scene* scene) const;

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene the material belongs to.
   */
  void prepareDefines(MaterialDefines& defines, Scene* scene);

  /**
   * @brief Binds the material data.
   * @param uniformBuffer defines the Uniform buffer to fill in.
   * @param scene defines the scene the material belongs to.
   * @param isFrozen defines whether the material is frozen or not.
   * @param subMesh the submesh to bind data for
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene, bool isFrozen,
                      SubMesh* subMesh = nullptr);

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
   * @returns "PBRSheenConfiguration"
   */
  std::string getClassName() const;

  /**
   * @brief Add fallbacks to the effect fallbacks list.
   * @param defines defines the Base texture to use.
   * @param fallbacks defines the current fallback list.
   * @param currentRank defines the current fallback rank.
   * @returns the new fallback rank.
   */
  static unsigned int AddFallbacks(const MaterialDefines& defines, EffectFallbacks& fallbacks,
                                   unsigned int currentRank);

  /**
   * @brief Add the required uniforms to the current list.
   * @param uniforms defines the current uniform list.
   */
  static void AddUniforms(std::vector<std::string>& uniforms);

  /**
   * @brief Add the required uniforms to the current buffer.
   * @param uniformBuffer defines the current uniform buffer.
   */
  static void PrepareUniformBuffer(UniformBuffer& uniformBuffer);

  /**
   * @brief Add the required samplers to the current list.
   * @param samplers defines the current sampler list.
   */
  static void AddSamplers(std::vector<std::string>& samplers);

  /**
   * @brief Makes a duplicate of the current configuration into another one.
   * @param sheenConfiguration define the config where to copy the info
   */
  void copyTo(PBRSheenConfiguration& sheenConfiguration);

  /**
   * @brief Serializes this BRDF configuration.
   * @returns - An object with the serialized config.
   */
  json serialize() const;

  /**
   * @brief Parses a sheen Configuration from a serialized object.
   * @param source - Serialized object.
   * @param scene Defines the scene we are parsing for
   * @param rootUrl Defines the rootUrl to load from
   */
  void parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);
  bool get_linkSheenWithAlbedo() const;
  void set_linkSheenWithAlbedo(bool value);
  BaseTexturePtr& get_texture();
  void set_texture(const BaseTexturePtr& value);
  bool get_useRoughnessFromMainTexture() const;
  void set_useRoughnessFromMainTexture(bool value);
  std::optional<float>& get_roughness();
  void set_roughness(const std::optional<float>& value);
  BaseTexturePtr& get_textureRoughness();
  void set_textureRoughness(const BaseTexturePtr& value);
  bool get_albedoScaling() const;
  void set_albedoScaling(bool value);

public:
  /**
   * Defines if the material uses sheen.
   */
  Property<PBRSheenConfiguration, bool> isEnabled;

  /**
   * Defines if the sheen is linked to the sheen color.
   */
  Property<PBRSheenConfiguration, bool> linkSheenWithAlbedo;

  /**
   * Defines the sheen intensity.
   */
  float intensity;

  /**
   * Defines the sheen color.
   */
  Color3 color;

  /**
   * Stores the sheen tint values in a texture.
   * rgb is tint
   * a is a intensity or roughness if the roughness property has been defined and
   * useRoughnessFromTexture is true (in that case, textureRoughness won't be used) If the roughness
   * property has been defined and useRoughnessFromTexture is false then the alpha channel is not
   * used to modulate roughness
   */
  Property<PBRSheenConfiguration, BaseTexturePtr> texture;

  /**
   * Indicates that the alpha channel of the texture property will be used for roughness.
   * Has no effect if the roughness (and texture!) property is not defined
   */
  Property<PBRSheenConfiguration, bool> useRoughnessFromMainTexture;

  /**
   * Defines the sheen roughness.
   * It is not taken into account if linkSheenWithAlbedo is true.
   * To stay backward compatible, material roughness is used instead if sheen roughness = null
   */
  Property<PBRSheenConfiguration, std::optional<float>> roughness;

  /**
   * Stores the sheen roughness in a texture.
   * alpha channel is the roughness. This texture won't be used if the texture property is not empty
   * and useRoughnessFromTexture is true
   */
  Property<PBRSheenConfiguration, BaseTexturePtr> textureRoughness;

  /**
   * If true, the sheen effect is layered above the base BRDF with the albedo-scaling technique.
   * It allows the strength of the sheen effect to not depend on the base color of the material,
   * making it easier to setup and tweak the effect
   */
  Property<PBRSheenConfiguration, bool> albedoScaling;

private:
  bool _isEnabled;
  bool _linkSheenWithAlbedo;
  BaseTexturePtr _texture;
  bool _useRoughnessFromMainTexture;
  std::optional<float> _roughness;
  BaseTexturePtr _textureRoughness;
  bool _albedoScaling;
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class PBRSheenConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_SHEEN_CONFIGURATION_H
