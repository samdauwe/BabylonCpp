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

  bool operator[](const string_t& define) const;
  bool operator==(const MaterialDefines& rhs) const;
  bool operator!=(const MaterialDefines& rhs) const;
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
  unordered_map_t<string_t, bool> boolDef;
  unordered_map_t<string_t, unsigned int> intDef;
  unordered_map_t<string_t, float> floatDef;
  unordered_map_t<string_t, string_t> stringDef;

  bool _isDirty;
  /** Hidden */
  int _renderId;

  /** Hidden */
  bool _areLightsDirty;
  /** Hidden */
  bool _areAttributesDirty;
  /** Hidden */
  bool _areTexturesDirty;
  /** Hidden */
  bool _areFresnelDirty;
  /** Hidden */
  bool _areMiscDirty;
  /** Hidden */
  bool _areImageProcessingDirty;

  /** Hidden */
  bool _normals;
  /** Hidden */
  bool _uvs;

  /** Hidden */
  bool _needNormals;
  /** Hidden */
  bool _needUVs;

}; // end of struct MaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_DEFINES_H
