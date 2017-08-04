#ifndef BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/pbr/pbr_base_simple_material.h>

namespace BABYLON {

/**
 * The PBR material of BJS following the metal roughness convention.
 *
 * This fits to the PBR convention in the GLTF definition:
 * https://github.com/KhronosGroup/glTF/tree/2.0/specification/2.0
 */
class BABYLON_SHARED_EXPORT PBRSpecularGlossinessMaterial
  : public Internals::PBRBaseSimpleMaterial {

public:
  /**
   * @brief Instantiates a new PBRMetalRoughnessMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRSpecularGlossinessMaterial(const std::string& name, Scene* scene);
  ~PBRSpecularGlossinessMaterial();

  /**
   * @brief Return the currrent class name of the material.
   */
  const char* getClassName() const;

  /**
   * @brief Return the active textures of the material.
   */
  std::vector<BaseTexture*> getActiveTextures() const;

  bool hasTexture(BaseTexture* texture) const;

  PBRSpecularGlossinessMaterial* clone(const std::string& name);

  /**
   * @brief Serialize the material to a parsable JSON object.
   */
  Json::object serialize() const;

  /**
   * @brief Parses a JSON object correponding to the serialize function.
   */
  static PBRSpecularGlossinessMaterial*
  Parse(const Json::value& source, Scene* scene, const std::string& rootUrl);

public:
  /**
   * Specifies the diffuse color of the material.
   */
  Color3 diffuseColor;

  /**
   * Specifies the diffuse texture of the material. This can also contains the
   * opcity value in its alpha channel.
   */
  BaseTexture* diffuseTexture;

  /**
   * Specifies the specular color of the material. This indicates how reflective
   * is the material (none to mirror).
   */
  Color3 specularColor;

  /**
   * Specifies the glossiness of the material. This indicates "how sharp is the
   * reflection".
   */
  float glossiness;

  /**
   * Specifies both the specular color RGB and the glossiness A of the material
   * per pixels.
   */
  BaseTexture* specularGlossinessTexture;

}; // end of class PBRSpecularGlossinessMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_SPECULAR_GLOSSINESS_MATERIAL_H
