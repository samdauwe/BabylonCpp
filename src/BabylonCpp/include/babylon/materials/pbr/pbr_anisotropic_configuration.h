#ifndef BABYLON_MATERIALS_PBR_PBR_ANISOTROPIC_CONFIGURATION_H
#define BABYLON_MATERIALS_PBR_PBR_ANISOTROPIC_CONFIGURATION_H

#include <functional>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/maths/vector2.h>

using json = nlohmann::json;

namespace BABYLON {

class AbstractMesh;
class IAnimatable;
class BaseTexture;
class EffectFallbacks;
struct MaterialDefines;
class Scene;
class UniformBuffer;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using IAnimatablePtr  = std::shared_ptr<IAnimatable>;
using BaseTexturePtr  = std::shared_ptr<BaseTexture>;

/**
 * @brief Define the code related to the anisotropic parameters of the pbr
 * material.
 */
class BABYLON_SHARED_EXPORT PBRAnisotropicConfiguration {

public:
  /**
   * @brief Instantiate a new istance of anisotropy configuration.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to
   * dirty
   */
  PBRAnisotropicConfiguration(const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~PBRAnisotropicConfiguration(); // = default

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
   * @param mesh the mesh we are preparing the defines for.
   * @param scene defines the scene the material belongs to.
   */
  void prepareDefines(MaterialDefines& defines, AbstractMesh& mesh, Scene* scene);

  /**
   * @brief Binds the material data.
   * @param uniformBuffer defines the Uniform buffer to fill in.
   * @param scene defines the scene the material belongs to.
   * @param isFrozen defines wether the material is frozen or not.
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene, bool isFrozen);

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param texture - Base texture to use.
   * @returns - Boolean specifying if a texture is used in the material.
   */
  [[nodiscard]] bool hasTexture(const BaseTexturePtr& texture) const;

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
   * @returns "PBRAnisotropicConfiguration"
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
   * @param anisotropicConfiguration define the config where to copy the info
   */
  void copyTo(PBRAnisotropicConfiguration& anisotropicConfiguration);

  /**
   * @brief Serializes this anisotropy configuration.
   * @returns - An object with the serialized config.
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parses a anisotropy Configuration from a serialized object.
   * @param source - Serialized object.
   * @param scene Defines the scene we are parsing for
   * @param rootUrl Defines the rootUrl to load from
   */
  void parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  [[nodiscard]] bool get_isEnabled() const;
  void set_isEnabled(bool value);
  BaseTexturePtr& get_texture();
  void set_texture(const BaseTexturePtr& value);

public:
  /**
   * Defines if the anisotropy is enabled in the material.
   */
  Property<PBRAnisotropicConfiguration, bool> isEnabled;

  /**
   * Defines the anisotropy strength (between 0 and 1) it defaults to 1.
   */
  float intensity;

  /**
   * Defines if the effect is along the tangents, bitangents or in between.
   * By default, the effect is "strectching" the highlights along the tangents.
   */
  Vector2 direction;

  /**
   * Stores the anisotropy values in a texture.
   * rg is direction (like normal from -1 to 1)
   * b is a intensity
   */
  Property<PBRAnisotropicConfiguration, BaseTexturePtr> texture;

private:
  bool _isEnabled;
  BaseTexturePtr _texture;

  /** Hidden */
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class PBRAnisotropicConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_ANISOTROPIC_CONFIGURATION_H
