#ifndef BABYLON_MATERIALS_PBR_PBR_METALLIC_ROUGHNESS_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_METALLIC_ROUGHNESS_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/pbr/pbr_base_simple_material.h>

namespace BABYLON {

class PBRMetallicRoughnessMaterial;
using PBRMetallicRoughnessMaterialPtr = std::shared_ptr<PBRMetallicRoughnessMaterial>;

/**
 * @brief The PBR material of BJS following the metal roughness convention.
 *
 * This fits to the PBR convention in the GLTF definition:
 * https://github.com/KhronosGroup/glTF/tree/2.0/specification/2.0
 */
class BABYLON_SHARED_EXPORT PBRMetallicRoughnessMaterial : public PBRBaseSimpleMaterial {

public:
  template <typename... Ts>
  static PBRMetallicRoughnessMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<PBRMetallicRoughnessMaterial>(
      new PBRMetallicRoughnessMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~PBRMetallicRoughnessMaterial() override; // = default

  /**
   * @brief Return the currrent class name of the material.
   */
  [[nodiscard]] std::string getClassName() const override;

  /**
   * @brief Gets a property.
   */
  AnimationValue getProperty(const std::vector<std::string>& targetPropertyPath) override;

  /**
   * @brief Sets a property.
   */
  void setProperty(const std::vector<std::string>& targetPropertyPath,
                   const AnimationValue& value) override;

  /**
   * @brief Makes a duplicate of the current material.
   * @param name - name to use for the new material.
   */
  [[nodiscard]] MaterialPtr clone(const std::string& name,
                                  bool cloneChildren = false) const override;

  /**
   * @brief Serialize the material to a parsable JSON object.
   */
  [[nodiscard]] json serialize() const;

  /**
   * @brief Parses a JSON object correponding to the serialize function.
   */
  static PBRMetallicRoughnessMaterial* Parse(const json& source, Scene* scene,
                                             const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new PBRMetalRoughnessMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRMetallicRoughnessMaterial(const std::string& name, Scene* scene);

  /**
   * @brief The base color has two different interpretations depending on the
   * value of metalness. When the material is a metal, the base color is the
   * specific measured reflectance value at normal incidence (F0). For a
   * non-metal the base color represents the reflected diffuse color of the
   * material.
   */
  Color3& get_baseColor();
  void set_baseColor(const Color3& value);

  /**
   * @brief Base texture of the metallic workflow. It contains both the
   * baseColor information in RGB as well as opacity information in the alpha
   * channel.
   */
  BaseTexturePtr& get_baseTexture();
  void set_baseTexture(const BaseTexturePtr& value);

  /**
   * @brief Specifies the metallic scalar value of the material.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  [[nodiscard]] float get_metallic() const;
  void set_metallic(float value);

  /**
   * @brief Specifies the roughness scalar value of the material.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  [[nodiscard]] float get_roughness() const;
  void set_roughness(float value);

  /**
   * @brief Texture containing both the metallic value in the B channel and the
   * roughness value in the G channel to keep better precision.
   */
  BaseTexturePtr& get_metallicRoughnessTexture();
  void set_metallicRoughnessTexture(const BaseTexturePtr& value);

public:
  /**
   * The base color has two different interpretations depending on the value of
   * metalness.
   * When the material is a metal, the base color is the specific measured
   * reflectance value at normal incidence (F0). For a non-metal the base color
   * represents the reflected diffuse color of the material.
   */
  Property<PBRMetallicRoughnessMaterial, Color3> baseColor;

  /**
   * Base texture of the metallic workflow. It contains both the baseColor
   * information in RGB as well as opacity information in the alpha channel.
   */
  Property<PBRMetallicRoughnessMaterial, BaseTexturePtr> baseTexture;

  /**
   * Specifies the metallic scalar value of the material.
   * Can also be used to scale the metalness values of the metallic texture.
   */
  Property<PBRMetallicRoughnessMaterial, float> metallic;

  /**
   * Specifies the roughness scalar value of the material.
   * Can also be used to scale the roughness values of the metallic texture.
   */
  Property<PBRMetallicRoughnessMaterial, float> roughness;

  /**
   * Texture containing both the metallic value in the B channel and the
   * roughness value in the G channel to keep better precision.
   */
  Property<PBRMetallicRoughnessMaterial, BaseTexturePtr> metallicRoughnessTexture;

}; // end of class PBRMetallicRoughnessMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_METALLIC_ROUGHNESS_MATERIAL_H
