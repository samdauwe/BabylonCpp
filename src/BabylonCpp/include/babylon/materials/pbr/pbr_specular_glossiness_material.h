#ifndef BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/pbr/pbr_base_simple_material.h>

namespace BABYLON {

class PBRSpecularGlossinessMaterial;
using PBRSpecularGlossinessMaterialPtr
  = std::shared_ptr<PBRSpecularGlossinessMaterial>;

/**
 * @brief The PBR material of BJS following the metal roughness convention.
 *
 * This fits to the PBR convention in the GLTF definition:
 * https://github.com/KhronosGroup/glTF/tree/2.0/specification/2.0
 */
class BABYLON_SHARED_EXPORT PBRSpecularGlossinessMaterial
    : public PBRBaseSimpleMaterial {

public:
  template <typename... Ts>
  static PBRSpecularGlossinessMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<PBRSpecularGlossinessMaterial>(
      new PBRSpecularGlossinessMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~PBRSpecularGlossinessMaterial() override;

  /**
   * @brief Return the currrent class name of the material.
   */
  const std::string getClassName() const override;

  /**
   * @brief Return the active textures of the material.
   */
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  /**
   * @brief Checks to see if a texture is used in the material.
   * @param texture - Base texture to use.
   * @returns - Boolean specifying if a texture is used in the material.
   */
  bool hasTexture(const BaseTexturePtr& texture) const override;

  /**
   * @brief Makes a duplicate of the current material.
   * @param name - name to use for the new material.
   */
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;

  /**
   * @brief Serialize the material to a parsable JSON object.
   */
  json serialize() const;

  /**
   * @brief Parses a JSON object correponding to the serialize function.
   */
  static PBRSpecularGlossinessMaterial* Parse(const json& source, Scene* scene,
                                              const std::string& rootUrl);

protected:
  /**
   * @brief Instantiates a new PBRMetalRoughnessMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRSpecularGlossinessMaterial(const std::string& name, Scene* scene);

  /**
   * @brief Specifies the diffuse color of the material.
   */
  Color3& get_diffuseColor();
  void set_diffuseColor(const Color3& value);

  /**
   * @brief Specifies the diffuse texture of the material. This can also
   * contains the opcity value in its alpha channel.
   */
  BaseTexturePtr& get_diffuseTexture();
  void set_diffuseTexture(const BaseTexturePtr& value);

  /**
   * @brief Specifies the specular color of the material. This indicates how
   * reflective is the material (none to mirror).
   */
  Color3& get_specularColor();
  void set_specularColor(const Color3& value);

  /**
   * @brief Specifies the glossiness of the material. This indicates "how sharp
   * is the reflection".
   */
  float get_glossiness() const;
  void set_glossiness(float value);

  /**
   * @brief Specifies both the specular color RGB and the glossiness A of the
   * material per pixels.
   */
  BaseTexturePtr& get_specularGlossinessTexture();
  void set_specularGlossinessTexture(const BaseTexturePtr& value);

public:
  /**
   * Specifies the diffuse color of the material.
   */
  Property<PBRSpecularGlossinessMaterial, Color3> diffuseColor;

  /**
   * Specifies the diffuse texture of the material. This can also contains the
   * opcity value in its alpha channel.
   */
  Property<PBRSpecularGlossinessMaterial, BaseTexturePtr> diffuseTexture;

  /**
   * Specifies the specular color of the material. This indicates how reflective
   * is the material (none to mirror).
   */
  Property<PBRSpecularGlossinessMaterial, Color3> specularColor;

  /**
   * Specifies the glossiness of the material. This indicates "how sharp is the
   * reflection".
   */
  Property<PBRSpecularGlossinessMaterial, float> glossiness;

  /**
   * Specifies both the specular color RGB and the glossiness A of the material
   * per pixels.
   */
  Property<PBRSpecularGlossinessMaterial, BaseTexturePtr>
    specularGlossinessTexture;

}; // end of class PBRSpecularGlossinessMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H
