#ifndef BABYLON_MATERIALS_THIN_MATERIAL_HELPER_H
#define BABYLON_MATERIALS_THIN_MATERIAL_HELPER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

struct IClipPlanesHolder;
FWD_CLASS_SPTR(Effect)

/**
 * @brief "Static Class" containing a few commonly used helper while dealing with material for
 * rendering purpose.
 *
 * It is complementary with MaterialHelper but provides completely independent functions (for tree
 * shaking sake)
 *
 * This works by convention in BabylonJS but is meant to be use only with shader following the in
 * place naming rules and conventions.
 */
struct BABYLON_SHARED_EXPORT ThinMaterialHelper {

  /**
   * @brief Binds the clip plane information from the holder to the effect.
   * @param effect The effect we are binding the data to
   * @param holder The entity containing the clip plane information
   */
  static void BindClipPlane(const EffectPtr& effect, const IClipPlanesHolder& holder);

}; // end of struct ThinMaterialHelper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_THIN_MATERIAL_HELPER_H
