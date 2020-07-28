#ifndef BABYLON_MATERIALS_DETAIL_MAP_CONFIGURATION_H
#define BABYLON_MATERIALS_DETAIL_MAP_CONFIGURATION_H

#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

using json = nlohmann::json;

namespace BABYLON {

class IAnimatable;
struct IMaterialDetailMapDefines;
class BaseTexture;
class Scene;
class UniformBuffer;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;
using IAnimatablePtr = std::shared_ptr<IAnimatable>;

// clang-format off
/**
 * @brief Define the code related to the detail map parameters of a material.
 *
 * Inspired from:
 *   Unity: https://docs.unity3d.com/Packages/com.unity.render-pipelines.high-definition@9.0/manual/Mask-Map-and-Detail-Map.html and https://docs.unity3d.com/Manual/StandardShaderMaterialParameterDetail.html
 *   Unreal: https://docs.unrealengine.com/en-US/Engine/Rendering/Materials/HowTo/DetailTexturing/index.html
 *   Cryengine: https://docs.cryengine.com/display/SDKDOC2/Detail+Maps
 */
// clang-format on
class BABYLON_SHARED_EXPORT DetailMapConfiguration {

public:
  /**
   * @brief Instantiate a new detail map.
   * @param markAllSubMeshesAsTexturesDirty Callback to flag the material to dirty
   */
  DetailMapConfiguration(const std::function<void()>& markAllSubMeshesAsTexturesDirty);
  ~DetailMapConfiguration(); // = default

  /**
   * @brief Hidden
   */
  void _markAllSubMeshesAsTexturesDirty();

  /**
   * @brief Gets whether the submesh is ready to be used or not.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene the material belongs to.
   * @returns - boolean indicating that the submesh is ready or not.
   */
  bool isReadyForSubMesh(IMaterialDetailMapDefines& defines, Scene* scene);

  /**
   * @brief Update the defines for detail map usage.
   * @param defines the list of "defines" to update.
   * @param scene defines the scene the material belongs to.
   */
  void prepareDefines(IMaterialDetailMapDefines& defines, Scene* scene);

  /**
   * @brief Binds the material data.
   * @param uniformBuffer defines the Uniform buffer to fill in.
   * @param scene defines the scene the material belongs to.
   * @param isFrozen defines whether the material is frozen or not.
   */
  void bindForSubMesh(UniformBuffer& uniformBuffer, Scene* scene, bool isFrozen);

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
   * @brief Get the current class name useful for serialization or dynamic coding.
   * @returns "DetailMap"
   */
  std::string getClassName() const;

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
   * @brief Makes a duplicate of the current instance into another one.
   * @param detailMap define the instance where to copy the info
   */
  void copyTo(DetailMapConfiguration& detailMap);

  /**
   * @brief Serializes this detail map instance.
   * @returns - An object with the serialized instance.
   */
  json serialize() const;

  /**
   * @brief Parses a detail map setting from a serialized object.
   * @param source - Serialized object.
   * @param scene Defines the scene we are parsing for
   * @param rootUrl Defines the rootUrl to load from
   */
  void parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Gets the detail texture of the material.
   */
  BaseTexturePtr& get_texture();

  /**
   * @brief Sets the detail texture of the material.
   */
  void set_texture(const BaseTexturePtr& value);

  /**
   * @brief Gets the method used to blend the bump and detail normals together.
   */
  unsigned int get_normalBlendMethod() const;

  /**
   * @brief Sets method used to blend the bump and detail normals together.
   */
  void set_normalBlendMethod(unsigned int value);

  /**
   * @brief Gets the state of the detail map on this material.
   */
  bool get_isEnabled() const;

  /**
   * @brief Sets the state of the detail map on this material.
   */
  void set_isEnabled(bool value);

public:
  /**
   * The detail texture of the material.
   */
  Property<DetailMapConfiguration, BaseTexturePtr> texture;

  /**
   * Defines how strongly the detail diffuse/albedo channel is blended with the regular
   * diffuse/albedo texture Bigger values mean stronger blending
   */
  float diffuseBlendLevel;

  /**
   * Defines how strongly the detail roughness channel is blended with the regular roughness value
   * Bigger values mean stronger blending. Only used with PBR materials
   */
  float roughnessBlendLevel;

  /**
   * Defines how strong the bump effect from the detail map is
   * Bigger values mean stronger effect
   */
  float bumpLevel;

  /**
   * The method used to blend the bump and detail normals together
   */
  Property<DetailMapConfiguration, unsigned int> normalBlendMethod;

  /**
   * Enable or disable the detail map on this material
   */
  Property<DetailMapConfiguration, bool> isEnabled;

private:
  BaseTexturePtr _texture;
  unsigned int _normalBlendMethod;
  bool _isEnabled;

  /** @hidden */
  std::function<void()> _internalMarkAllSubMeshesAsTexturesDirty;

}; // end of class DetailMapConfiguration

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_DETAIL_MAP_CONFIGURATION_H
