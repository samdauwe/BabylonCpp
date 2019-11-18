#ifndef BABYLON_MATERIALS_PBR_IMATERIAL_CLEAR_COAT_DEFINES_H
#define BABYLON_MATERIALS_PBR_IMATERIAL_CLEAR_COAT_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT IMaterialClearCoatDefines : public MaterialDefines {

  IMaterialClearCoatDefines();
  ~IMaterialClearCoatDefines() override; // = default

  /**
   * @brief Converts the material define values to a string.
   * @returns - String of material define information.
   */
  [[nodiscard]] std::string toString() const override;

}; // end of struct IMaterialClearCoatDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_IMATERIAL_CLEAR_COAT_DEFINES_H
