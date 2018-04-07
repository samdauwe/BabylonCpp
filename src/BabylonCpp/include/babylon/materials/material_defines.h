#ifndef BABYLON_MATERIALS_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/imaterial_defines.h>

namespace BABYLON {

/**
 * @brief Manages the defines for the Material.
 */
struct BABYLON_SHARED_EXPORT MaterialDefines : public IMaterialDefines {

  MaterialDefines();
  virtual ~MaterialDefines();

  bool operator[](unsigned int define) const;
  bool operator==(const MaterialDefines& rhs) const;
  bool operator!=(const MaterialDefines& rhs) const;
  void resizeLights(unsigned int lightIndex);
  friend std::ostream& operator<<(std::ostream& os,
                                  const MaterialDefines& materialDefines);

  /**
   * @brief Specifies if the material needs to be re-calculated.
   */
  bool isDirty() const override;

  /**
   * @brief Marks the material to indicate that it has been re-calculated.
   */
  void markAsProcessed() override;

  /**
   * @brief Marks the material to indicate that it needs to be re-calculated.
   */
  void markAsUnprocessed() override;

  /**
   * @brief Marks the material to indicate all of its defines need to be
   * re-calculated.
   */
  void markAllAsDirty() override;

  /**
   * @brief Marks the material to indicate that image processing needs to be
   * re-calculated.
   */
  void markAsImageProcessingDirty() override;

  /**
   * @brief Marks the material to indicate the lights need to be re-calculated.
   */
  void markAsLightDirty() override;

  /**
   * @brief Marks the attribute state as changed.
   */
  void markAsAttributesDirty() override;

  /**
   * @brief Marks the texture state as changed.
   */
  void markAsTexturesDirty() override;

  /**
   * @brief Marks the fresnel state as changed.
   */
  void markAsFresnelDirty() override;

  /**
   * @brief Marks the misc state as changed.
   */
  void markAsMiscDirty() override;

  /**
   * @brief Rebuilds the material defines.
   */
  virtual void rebuild() override;

  /**
   * @brief Specifies if two material defines are equal.
   * @param other - A material define instance to compare to.
   * @returns - Boolean indicating if the material defines are equal (true) or
   * not (false).
   */
  virtual bool isEqual(const MaterialDefines& other) const override;

  /**
   * @brief Clones this instance's defines to another instance.
   * @param other - material defines to clone values to.
   */
  virtual void cloneTo(MaterialDefines& other) override;

  /**
   * @brief Resets the material define values.
   */
  virtual void reset() override;

  /**
   * @brief Converts the material define values to a string.
   * @returns - String of material define information.
   */
  virtual string_t toString() const override;

  // Properties
  vector_t<bool> defines;
  vector_t<string_t> _keys;

  unsigned int NUM_BONE_INFLUENCERS;
  unsigned int BonesPerMesh;
  unsigned int NUM_MORPH_INFLUENCERS;

  /**
   * The direct UV channel to use.
   */
  unsigned int DIFFUSEDIRECTUV;
  unsigned int AMBIENTDIRECTUV;
  unsigned int OPACITYDIRECTUV;
  unsigned int EMISSIVEDIRECTUV;
  unsigned int SPECULARDIRECTUV;
  unsigned int BUMPDIRECTUV;
  unsigned int LIGHTMAPDIRECTUV;

  // PBR Material
  unsigned int ALBEDODIRECTUV;
  unsigned int REFLECTIVITYDIRECTUV;
  unsigned int MICROSURFACEMAPDIRECTUV;
  string_t ALPHATESTVALUE;

  vector_t<bool> lights;
  vector_t<bool> pointlights;
  vector_t<bool> dirlights;
  vector_t<bool> hemilights;
  vector_t<bool> spotlights;
  vector_t<bool> shadows;
  vector_t<bool> shadowesms;
  vector_t<bool> shadowcloseesms;
  vector_t<bool> shadowpcsss;
  vector_t<bool> shadowpcfs;
  vector_t<bool> shadowpoissons;
  vector_t<bool> shadowcubes;
  vector_t<bool> projectedLightTexture;
  vector_t<bool> shadowlowqualities;
  vector_t<bool> shadowmediumqualities;
  vector_t<bool> shadowhighqualities;

  bool TANGENT;
  bool SHADOWS;
  bool LIGHTMAPEXCLUDED;
  vector_t<bool> lightmapexcluded;
  vector_t<bool> lightmapnospecular;

  bool USERIGHTHANDEDSYSTEM;

  bool _isDirty;
  int _renderId;

  bool _areLightsDirty;
  bool _areAttributesDirty;
  bool _areTexturesDirty;
  bool _areFresnelDirty;
  bool _areMiscDirty;
  bool _areImageProcessingDirty;

  bool _normals;
  bool _uvs;

  bool _needNormals;
  bool _needUVs;

}; // end of struct MaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_DEFINES_H
