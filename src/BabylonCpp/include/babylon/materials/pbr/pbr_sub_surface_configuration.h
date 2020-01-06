#ifndef BABYLON_MATERIALS_PBR_PBR_SUB_SURFACE_CONFIGURATION_H
#define BABYLON_MATERIALS_PBR_PBR_SUB_SURFACE_CONFIGURATION_H

#include <functional>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/color3.h>

using json = nlohmann::json;

namespace BABYLON {

class IAnimatable;
class BaseTexture;
class Effect;
class EffectFallbacks;
class Engine;
struct MaterialDefines;
class RenderTargetTexture;
class Scene;
class UniformBuffer;
using IAnimatablePtr         = std::shared_ptr<IAnimatable>;
using BaseTexturePtr         = std::shared_ptr<BaseTexture>;
using RenderTargetTexturePtr = std::shared_ptr<RenderTargetTexture>;

/**
 * @brief Define the code related to the sub surface parameters of the pbr material.
 */
class BABYLON_SHARED_EXPORT PBRSubSurfaceConfiguration {

public:
  /**
   * @brief Instantiate a new istance of sub surface configuration.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to dirty
   */
  PBRSubSurfaceConfiguration(const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~PBRSubSurfaceConfiguration(); // = default

  /**
   * @brief Hidden
   */
  void _markAllSubMeshesAsTexturesDirty();

  /**
   * @brief Gets wehter the submesh is ready to be used or not.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene the material belongs to.
   * @returns - boolean indicating that the submesh is ready or not.
   */
  bool isReadyForSubMesh(const MaterialDefines& defines, Scene* scene) const;

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
   * @param isFrozen defines wether the material is frozen or not.
   * @param lodBasedMicrosurface defines wether the material relies on lod based microsurface or
   * not.
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene, Engine* engine, bool isFrozen,
                      bool lodBasedMicrosurface);

  /**
   * @brief Unbinds the material from the mesh.
   * @param activeEffect defines the effect that should be unbound from.
   * @returns true if unbound, otherwise false
   */
  bool unbind(Effect* activeEffect);

  /**
   * @brief Returns true if alpha blending should be disabled.
   */
  [[nodiscard]] bool disableAlphaBlending() const;

  /**
   * @brief Fills the list of render target textures.
   * @param renderTargets the list of render targets to update
   */
  void fillRenderTargetTextures(std::vector<RenderTargetTexturePtr>& renderTargets);

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param texture - Base texture to use.
   * @returns - Boolean specifying if a texture is used in the material.
   */
  [[nodiscard]] bool hasTexture(const BaseTexturePtr& texture) const;

  /**
   * @brief Gets a boolean indicating that current material needs to register RTT.
   * @returns true if this uses a render target otherwise false.
   */
  [[nodiscard]] bool hasRenderTargetTextures() const;

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
   * @returns "PBRSubSurfaceConfiguration"
   */
  [[nodiscard]] std::string getClassName() const;

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
   * @param configuration define the config where to copy the info
   */
  void copyTo(PBRSubSurfaceConfiguration& configuration);

  /**
   * @brief Serializes this Sub Surface configuration.
   * @returns - An object with the serialized config.
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parses a Sub Surface Configuration from a serialized object.
   * @param source - Serialized object.
   * @param scene Defines the scene we are parsing for
   * @param rootUrl Defines the rootUrl to load from
   */
  void parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  [[nodiscard]] bool get_isRefractionEnabled() const;
  void set_isRefractionEnabled(bool value);
  [[nodiscard]] bool get_isTranslucencyEnabled() const;
  void set_isTranslucencyEnabled(bool value);
  BaseTexturePtr& get_thicknessTexture();
  void set_thicknessTexture(const BaseTexturePtr& value);
  BaseTexturePtr& get_refractionTexture();
  void set_refractionTexture(const BaseTexturePtr& value);
  [[nodiscard]] float get_indexOfRefraction() const;
  void set_indexOfRefraction(float value);
  [[nodiscard]] bool get_invertRefractionY() const;
  void set_invertRefractionY(bool value);
  [[nodiscard]] bool get_linkRefractionWithTransparency() const;
  void set_linkRefractionWithTransparency(bool value);
  [[nodiscard]] bool get_useMaskFromThicknessTexture() const;
  void set_useMaskFromThicknessTexture(bool value);

private:
  /**
   * @brief Returns the texture used for refraction or null if none is used.
   * @param scene defines the scene the material belongs to.
   * @returns - Refraction texture if present.  If no refraction texture and refraction is linked
   * with transparency, returns environment texture. Otherwise, returns null.
   */
  BaseTexturePtr _getRefractionTexture(Scene* scene) const;

public:
  /**
   * Defines if the refraction is enabled in the material.
   */
  Property<PBRSubSurfaceConfiguration, bool> isRefractionEnabled;

  /**
   * Defines if the translucency is enabled in the material.
   */
  Property<PBRSubSurfaceConfiguration, bool> isTranslucencyEnabled;

  /**
   * Defines the refraction intensity of the material.
   * The refraction when enabled replaces the Diffuse part of the material.
   * The intensity helps transitionning between diffuse and refraction.
   */
  float refractionIntensity;

  /**
   * Defines the translucency intensity of the material.
   * When translucency has been enabled, this defines how much of the "translucency" is addded to
   * the diffuse part of the material.
   */
  float translucencyIntensity;

  /**
   * Defines the scattering intensity of the material.
   * When scattering has been enabled, this defines how much of the "scattered light" is addded to
   * the diffuse part of the material.
   */
  float scatteringIntensity;

  /**
   * Stores the average thickness of a mesh in a texture (The texture is holding the values
   * linearly). The red channel of the texture should contain the thickness remapped between 0
   * and 1. 0 would mean minimumThickness 1 would mean maximumThickness The other channels might be
   * use as a mask to vary the different effects intensity.
   */
  Property<PBRSubSurfaceConfiguration, BaseTexturePtr> thicknessTexture;

  /**
   * Defines the texture to use for refraction.
   */
  Property<PBRSubSurfaceConfiguration, BaseTexturePtr> refractionTexture;

  /**
   * Defines the index of refraction used in the material.
   * https://en.wikipedia.org/wiki/List_of_refractive_indices
   */
  Property<PBRSubSurfaceConfiguration, float> indexOfRefraction;

  /**
   * Controls if refraction needs to be inverted on Y. This could be useful for procedural texture.
   */
  Property<PBRSubSurfaceConfiguration, bool> invertRefractionY;

  /**
   * This parameters will make the material used its opacity to control how much it is refracting
   * aginst not. Materials half opaque for instance using refraction could benefit from this
   * control.
   */
  Property<PBRSubSurfaceConfiguration, bool> linkRefractionWithTransparency;

  /**
   * Defines the minimum thickness stored in the thickness map.
   * If no thickness map is defined, this value will be used to simulate thickness.
   */
  float minimumThickness;

  /**
   * Defines the maximum thickness stored in the thickness map.
   */
  float maximumThickness;

  /**
   * Defines the volume tint of the material.
   * This is used for both translucency and scattering.
   */
  Color3 tintColor;

  /**
   * Defines the distance at which the tint color should be found in the media.
   * This is used for refraction only.
   */
  float tintColorAtDistance;

  /**
   * Defines how far each channel transmit through the media.
   * It is defined as a color to simplify it selection.
   */
  Color3 diffusionDistance;

  /**
   * Stores the intensity of the different subsurface effects in the thickness texture.
   * * the green channel is the translucency intensity.
   * * the blue channel is the scattering intensity.
   * * the alpha channel is the refraction intensity.
   */
  Property<PBRSubSurfaceConfiguration, bool> useMaskFromThicknessTexture;

private:
  bool _isRefractionEnabled;
  bool _isTranslucencyEnabled;
  bool _isScatteringEnabled;
  BaseTexturePtr _thicknessTexture;
  BaseTexturePtr _refractionTexture;
  float _indexOfRefraction;
  bool _invertRefractionY;
  bool _linkRefractionWithTransparency;
  bool _useMaskFromThicknessTexture;

  /** Hidden */
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class PBRSubSurfaceConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_SUB_SURFACE_CONFIGURATION_H
