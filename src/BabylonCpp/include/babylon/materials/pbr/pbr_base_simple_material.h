#ifndef BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/pbr/pbr_base_material.h>

namespace BABYLON {

/**
 * @brief The Physically based simple base material of BJS.
 *
 * This enables better naming and convention enforcements on top of the
 * pbrMaterial.
 * It is used as the base class for both the specGloss and metalRough
 * conventions.
 */
class BABYLON_SHARED_EXPORT PBRBaseSimpleMaterial : public PBRBaseMaterial {

public:
  /**
   * @brief Instantiates a new PBRMaterial instance.
   *
   * @param name The material name
   * @param scene The scene the material will be use in.
   */
  PBRBaseSimpleMaterial(const std::string& name, Scene* scene);
  ~PBRBaseSimpleMaterial() override;

  /**
   * @brief Returns the string "PBRBaseSimpleMaterial".
   */
  const std::string getClassName() const override;

  /**
   * @brief Return the active textures of the material.
   */
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  bool hasTexture(const BaseTexturePtr& texture) const override;

public:
  /**
   * Number of Simultaneous lights allowed on the material.
   */
  Property<PBRBaseSimpleMaterial, unsigned int> maxSimultaneousLights;

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  Property<PBRBaseSimpleMaterial, bool> disableLighting;

  /**
   * Environment Texture used in the material (this is use for both reflection
   * and environment lighting).
   */
  Property<PBRBaseSimpleMaterial, BaseTexturePtr> environmentTexture;

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  Property<PBRBaseSimpleMaterial, bool> invertNormalMapX;

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  Property<PBRBaseSimpleMaterial, bool> invertNormalMapY;

  /**
   * Normal map used in the model.
   */
  Property<PBRBaseSimpleMaterial, BaseTexturePtr> normalTexture;

  /**
   * Emissivie color used to self-illuminate the model.
   */
  Property<PBRBaseSimpleMaterial, Color3> emissiveColor;

  /**
   * Emissivie texture used to self-illuminate the model.
   */
  Property<PBRBaseSimpleMaterial, BaseTexturePtr> emissiveTexture;

  /**
   * Occlusion Channel Strenght.
   */
  Property<PBRBaseSimpleMaterial, float> occlusionStrength;

  /**
   * Occlusion Texture of the material (adding extra occlusion effects).
   */
  Property<PBRBaseSimpleMaterial, BaseTexturePtr> occlusionTexture;

  /**
   * Defines the alpha limits in alpha test mode.
   */
  Property<PBRBaseSimpleMaterial, float> alphaCutOff;

  /**
   * Defines the current double sided mode.
   */
  Property<PBRBaseSimpleMaterial, bool> doubleSided;

  /**
   * Stores the pre-calculated light information of a mesh in a texture.
   */
  Property<PBRBaseSimpleMaterial, BaseTexturePtr> lightmapTexture;

  /**
   * If true, the light map contains occlusion information instead of lighting
   * info.
   */
  Property<PBRBaseSimpleMaterial, bool> useLightmapAsShadowmap;

protected:
  /**
   * Specifies wether or not the alpha value of the albedo texture should be
   * used.
   */
  bool _shouldUseAlphaFromAlbedoTexture() const;

  /**
   * Number of Simultaneous lights allowed on the material.
   */
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  bool get_disableLighting() const;
  void set_disableLighting(bool value);

  /**
   * Environment Texture used in the material (this is use for both reflection
   * and environment lighting).
   */
  BaseTexturePtr& get_environmentTexture();
  void set_environmentTexture(const BaseTexturePtr& value);

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  bool get_invertNormalMapX() const;
  void set_invertNormalMapX(bool value);

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  bool get_invertNormalMapY() const;
  void set_invertNormalMapY(bool value);

  /**
   * Normal map used in the model.
   */
  BaseTexturePtr& get_normalTexture();
  void set_normalTexture(const BaseTexturePtr& value);

  /**
   * Emissivie color used to self-illuminate the model.
   */
  Color3& get_emissiveColor();
  void set_emissiveColor(const Color3& value);

  /**
   * Emissivie texture used to self-illuminate the model.
   */
  BaseTexturePtr& get_emissiveTexture();
  void set_emissiveTexture(const BaseTexturePtr& value);

  /**
   * Occlusion Channel Strenght.
   */
  float get_occlusionStrength() const;
  void set_occlusionStrength(float value);

  /**
   * Occlusion Texture of the material (adding extra occlusion effects).
   */
  BaseTexturePtr& get_occlusionTexture();
  void set_occlusionTexture(const BaseTexturePtr& value);

  /**
   * Defines the alpha limits in alpha test mode.
   */
  float get_alphaCutOff() const;
  void set_alphaCutOff(float value);

  /**
   * @brief Gets the current double sided mode.
   */
  bool get_doubleSided() const;

  /**
   * @brief If sets to true and backfaceCulling is false, normals will be
   * flipped on the backside.
   */
  void set_doubleSided(bool value);

  /**
   * Stores the pre-calculated light information of a mesh in a texture.
   */
  BaseTexturePtr& get_lightmapTexture();
  void set_lightmapTexture(const BaseTexturePtr& value);

  /**
   * If true, the light map contains occlusion information instead of lighting
   * info.
   */
  bool get_useLightmapAsShadowmap() const;
  void set_useLightmapAsShadowmap(bool value);

}; // end of class PBRBaseSimpleMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H
