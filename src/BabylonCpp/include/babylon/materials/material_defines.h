#ifndef BABYLON_MATERIALS_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_MATERIAL_DEFINES_H

#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/materials/imaterial_defines.h>

namespace BABYLON {

/**
 * @brief Manages the defines for the Material.
 */
struct BABYLON_SHARED_EXPORT MaterialDefines : public IMaterialDefines {

  MaterialDefines();
  MaterialDefines(const MaterialDefines& other);
  MaterialDefines(MaterialDefines&& other);
  MaterialDefines& operator=(const MaterialDefines& other);
  MaterialDefines& operator=(MaterialDefines&& other);
  ~MaterialDefines() override; // = default

  bool operator[](const std::string& define) const;
  bool operator==(const MaterialDefines& rhs) const;
  bool operator!=(const MaterialDefines& rhs) const;
  friend std::ostream& operator<<(std::ostream& os, const MaterialDefines& materialDefines);

  /**
   * @brief Specifies if the material needs to be re-calculated.
   */
  [[nodiscard]] bool isDirty() const override;

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
   * @brief Marks the material to indicate the lights need to be re-calculated
   * @param disposed Defines whether the light is dirty due to dispose or not
   */
  void markAsLightDirty(bool disposed = false) override;

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
   * @brief Marks the prepass state as changed.
   */
  void markAsPrePassDirty() override;

  /**
   * @brief Rebuilds the material defines.
   */
  void rebuild() override;

  /**
   * @brief Specifies if two material defines are equal.
   * @param other - A material define instance to compare to.
   * @returns - Boolean indicating if the material defines are equal (true) or
   * not (false).
   */
  [[nodiscard]] bool isEqual(const MaterialDefines& other) const override;

  /**
   * @brief Clones this instance's defines to another instance.
   * @param other - material defines to clone values to.
   */
  void cloneTo(MaterialDefines& other) override;

  /**
   * @brief Resets the material define values.
   */
  void reset() override;

  /**
   * @brief Converts the material define values to a string.
   * @returns String of material define information.
   */
  [[nodiscard]] std::string toString() const override;

  // Properties
  std::unordered_map<std::string, bool> boolDef;
  std::unordered_map<std::string, int> intDef;
  std::unordered_map<std::string, float> floatDef;
  std::unordered_map<std::string, std::string> stringDef;

  bool _isDirty;
  /** Hidden */
  int _renderId;

  /** Hidden */
  bool _areLightsDirty;
  /** Hidden */
  bool _areLightsDisposed;
  /** Hidden */
  bool _areAttributesDirty;
  /** Hidden */
  bool _areTexturesDirty;
  /** Hidden */
  bool _areFresnelDirty;
  /** Hidden */
  bool _areMiscDirty;
  /** @hidden */
  bool _arePrePassDirty;
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
