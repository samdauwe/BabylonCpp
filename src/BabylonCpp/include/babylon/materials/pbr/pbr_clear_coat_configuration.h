#ifndef BABYLON_MATERIALS_PBR_PBR_CLEAR_COAT_CONFIGURATION_H
#define BABYLON_MATERIALS_PBR_PBR_CLEAR_COAT_CONFIGURATION_H

#include <functional>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/math/color3.h>

using json = nlohmann::json;

namespace BABYLON {

class IAnimatable;
class BaseTexture;
class Effect;
class EffectFallbacks;
class Engine;
struct IMaterialClearCoatDefines;
class RenderTargetTexture;
class Scene;
class UniformBuffer;
using IAnimatablePtr         = std::shared_ptr<IAnimatable>;
using BaseTexturePtr         = std::shared_ptr<BaseTexture>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief Define the code related to the clear coat parameters of the pbr
 * material.
 */
class BABYLON_SHARED_EXPORT PBRClearCoatConfiguration {

public:
  /**
   * @brief Instantiate a new istance of clear coat configuration.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to
   * dirty
   */
  PBRClearCoatConfiguration(
    const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~PBRClearCoatConfiguration();

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
  bool isReadyForSubMesh(const IMaterialClearCoatDefines& defines, Scene* scene,
                         Engine* engine, bool disableBumpMap) const;

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene to the material belongs to.
   */
  void prepareDefines(IMaterialClearCoatDefines& defines, Scene* scene);

  /**
   * @brief Binds the material data.
   * @param uniformBuffer defines the Uniform buffer to fill in.
   * @param scene defines the scene the material belongs to.
   * @param engine defines the engine the material belongs to.
   * @param disableBumpMap defines wether the material disables bump or not.
   * @param isFrozen defines wether the material is frozen or not.
   * @param invertNormalMapX If sets to true, x component of normal map value
   * will be inverted (x = 1.0 - x).
   * @param invertNormalMapY If sets to true, y component of normal map value
   * will be inverted (y = 1.0 - y).
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene,
                      Engine* engine, bool disableBumpMap, bool isFrozen,
                      bool invertNormalMapX, bool invertNormalMapY);

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
   * @brief  Disposes the resources of the material.
   * @param forceDisposeTextures - Forces the disposal of all textures.
   */
  void dispose(bool forceDisposeTextures = false);

  /**
   * @brief Get the current class name of the texture useful for serialization
   * or dynamic coding.
   * @returns "PBRClearCoatConfiguration"
   */
  const std::string getClassName() const;

  /**
   * @briefAdd fallbacks to the effect fallbacks list.
   * @param defines defines the Base texture to use.
   * @param fallbacks defines the current fallback list.
   * @param currentRank defines the current fallback rank.
   * @returns the new fallback rank.
   */
  static unsigned int AddFallbacks(const IMaterialClearCoatDefines& defines,
                                   EffectFallbacks& fallbacks,
                                   unsigned int currentRank);

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
   */
  void parse(const json& source);

protected:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);
  float get_indiceOfRefraction() const;
  void set_indiceOfRefraction(float value);
  BaseTexturePtr& get_texture();
  void set_texture(const BaseTexturePtr& value);
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
   * Defines the indice of refraction of the clear coat.
   * This defaults to 1.5 corresponding to a 0.04 f0 or a 4% reflectance at
   * normal incidence The default fits with a polyurethane material. Changing
   * the default value is more performance intensive.
   */
  Property<PBRClearCoatConfiguration, float> indiceOfRefraction;

  /**
   * Stores the clear coat values in a texture.
   */
  Property<PBRClearCoatConfiguration, BaseTexturePtr> texture;

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
   * Defines the distance at which the tint color should be found in the
   * clear coat media.
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
  static constexpr float _DefaultIndiceOfRefraction = 1.5f;

  bool _isEnabled;
  float _indiceOfRefraction;
  BaseTexturePtr _texture;
  BaseTexturePtr _bumpTexture;
  bool _isTintEnabled;
  BaseTexturePtr _tintTexture;

  /** Hidden */
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class PBRClearCoatConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_CLEAR_COAT_CONFIGURATION_H
