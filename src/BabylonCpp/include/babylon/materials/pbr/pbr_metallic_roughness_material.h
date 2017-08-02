#ifndef BABYLON_MATERIALS_PBR_PBR_METALLIC_ROUGHNESS_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_METALLIC_ROUGHNESS_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/pbr/pbr_base_simple_material.h>

namespace BABYLON {

/**
 * The PBR material of BJS following the metal roughness convention.
 *
 * This fits to the PBR convention in the GLTF definition:
 * https://github.com/KhronosGroup/glTF/tree/2.0/specification/2.0
 */
class BABYLON_SHARED_EXPORT PBRMetallicRoughnessMaterial
  : public Internals::PBRBaseSimpleMaterial {

public:
  /**
   * @brief Instantiates a new PBRMetalRoughnessMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRMetallicRoughnessMaterial(const std::string& name, Scene* scene);
  ~PBRMetallicRoughnessMaterial();

  /**
   * @brief Return the currrent class name of the material.
   */
  const char* getClassName() const;

  /**
   * @brief Return the active textures of the material.
   */
  std::vector<BaseTexture*> getActiveTextures() const;

  bool hasTexture(BaseTexture* texture) const;

  PBRMetallicRoughnessMaterial* clone(const std::string& name);

  /**
   * @brief Serialize the material to a parsable JSON object.
   */
  Json::object serialize() const;

  /**
   * @brief Parses a JSON object correponding to the serialize function.
   */
  static PBRMetallicRoughnessMaterial*
  Parse(const Json::value& source, Scene* scene, const std::string& rootUrl);

public:
  /**
   * The base color has two different interpretations depending on the value of
   * metalness.
   * When the material is a metal, the base color is the specific measured
   * reflectance value at normal incidence (F0). For a non-metal the base color
   * represents the reflected diffuse color of the material.
   */
  Color3 baseColor;

  /**
   * Base texture of the metallic workflow. It contains both the baseColor
   * information in RGB as well as opacity information in the alpha channel.
   */
  BaseTexture* baseTexture;

  /**
   * Specifies the metallic scalar value of the material.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  float metallic;

  /**
   * Specifies the roughness scalar value of the material.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  float roughness;

  /**
   * Texture containing both the metallic value in the B channel and the
   * roughness value in the G channel to keep better precision.
   */
  BaseTexture* metallicRoughnessTexture;

}; // end of class PBRMetallicRoughnessMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_METALLIC_ROUGHNESS_MATERIAL_H
