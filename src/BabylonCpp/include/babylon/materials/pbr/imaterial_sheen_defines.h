#ifndef BABYLON_MATERIALS_PBR_IMATERIAL_SHEEN_DEFINES_H
#define BABYLON_MATERIALS_PBR_IMATERIAL_SHEEN_DEFINES_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT IMaterialSheenDefines : public MaterialDefines {

  IMaterialSheenDefines();
  ~IMaterialSheenDefines() override;

  /**
   * @brief Converts the material define values to a string.
   * @returns - String of material define information.
   */
  virtual std::string toString() const override;

}; // end of struct IMaterialSheenDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_PBR_IMATERIAL_SHEEN_DEFINES_H
