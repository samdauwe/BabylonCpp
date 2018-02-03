#ifndef BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H
#define BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/pbr/pbr_base_material.h>

namespace BABYLON {
namespace Internals {

/**
 * The Physically based simple base material of BJS.
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
  PBRBaseSimpleMaterial(const string_t& name, Scene* scene);
  ~PBRBaseSimpleMaterial() override;

  /**
   * @brief Returns the string "PBRBaseSimpleMaterial".
   */
  const char* getClassName() const override;

  /**
   * @brief Gets the current double sided mode.
   */
  bool doubleSided() const;

  /**
   * @brief If sets to true and backfaceCulling is false, normals will be
   * flipped on the backside.
   */
  void setDoubleSided(bool value);

  /**
   * @brief Return the active textures of the material.
   */
  vector_t<BaseTexture*> getActiveTextures() const override;

  bool hasTexture(BaseTexture* texture) const override;

protected:
  /**
   * Specifies wether or not the alpha value of the albedo texture should be
   * used.
   */
  bool _shouldUseAlphaFromAlbedoTexture() const;

public:
  /**
   * Number of Simultaneous lights allowed on the material.
   */
  unsigned int maxSimultaneousLights;

  /**
   * If sets to true, disables all the lights affecting the material.
   */
  bool disableLighting;

  /**
   * Environment Texture used in the material (this is use for both reflection
   * and environment lighting).
   */
  BaseTexture* environmentTexture;

  /**
   * If sets to true, x component of normal map value will invert (x = 1.0 - x).
   */
  bool invertNormalMapX;

  /**
   * If sets to true, y component of normal map value will invert (y = 1.0 - y).
   */
  bool invertNormalMapY;

  /**
   * Normal map used in the model.
   */
  BaseTexture* normalTexture;

  /**
   * Emissivie color used to self-illuminate the model.
   */
  Color3 emissiveColor;

  /**
   * Emissivie texture used to self-illuminate the model.
   */
  BaseTexture* emissiveTexture;

  /**
   * Occlusion Channel Strenght.
   */
  float occlusionStrength;

  /**
   * Occlusion Texture of the material (adding extra occlusion effects).
   */
  BaseTexture* occlusionTexture;

  /**
   * Defines the alpha limits in alpha test mode.
   */
  float alphaCutOff;

  BaseTexture* lightmapTexture;

  bool useLightmapAsShadowmap;

}; // end of class PBRBaseSimpleMaterial

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_PBR_BASE_SIMPLE_MATERIAL_H
