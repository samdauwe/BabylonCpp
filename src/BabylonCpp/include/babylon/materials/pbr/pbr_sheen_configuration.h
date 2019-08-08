#ifndef BABYLON_MATERIALS_PBR_PBR_SHEEN_CONFIGURATION_H
#define BABYLON_MATERIALS_PBR_PBR_SHEEN_CONFIGURATION_H

#include <functional>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/color3.h>

using json = nlohmann::json;

namespace BABYLON {

class BaseTexture;
class EffectFallbacks;
class IAnimatable;
struct MaterialDefines;
class Scene;
class UniformBuffer;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

/**
 * @brief Define the code related to the BRDF parameters of the pbr material.
 */
class BABYLON_SHARED_EXPORT PBRSheenConfiguration {

public:
  /**
   * @brief Instantiate a new istance of clear coat configuration.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to
   * dirty
   */
  PBRSheenConfiguration(
    const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~PBRSheenConfiguration();

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
   * @param isFrozen defines wether the material is frozen or not.
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene,
                      bool isFrozen);

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
   * @brief Get the current class name of the texture useful for serialization
   * or dynamic coding.
   * @returns "PBRSheenConfiguration"
   */
  const std::string getClassName() const;

  /**
   * @brief Add fallbacks to the effect fallbacks list.
   * @param defines defines the Base texture to use.
   * @param fallbacks defines the current fallback list.
   * @param currentRank defines the current fallback rank.
   * @returns the new fallback rank.
   */
  static unsigned int AddFallbacks(const MaterialDefines& defines,
                                   EffectFallbacks& fallbacks,
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
   * @brief Parses a Sheen Configuration from a serialized object.
   * @param source - Serialized object.
   */
  void parse(const json& source);

protected:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);
  bool get_linkSheenWithAlbedo() const;
  void set_linkSheenWithAlbedo(bool value);
  BaseTexturePtr& get_texture();
  void set_texture(const BaseTexturePtr& value);

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
   * a is a intensity
   */
  Property<PBRSheenConfiguration, BaseTexturePtr> texture;

private:
  bool _isEnabled;
  bool _linkSheenWithAlbedo;
  BaseTexturePtr _texture;
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class PBRSheenConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_SHEEN_CONFIGURATION_H
