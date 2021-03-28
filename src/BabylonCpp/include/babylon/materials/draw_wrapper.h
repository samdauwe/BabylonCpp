#ifndef BABYLON_MATERIALS_DRAW_WRAPPER_H
#define BABYLON_MATERIALS_DRAW_WRAPPER_H

#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/material_defines.h>

namespace BABYLON {

class ThinEngine;
FWD_STRUCT_SPTR(DrawWrapper)
FWD_CLASS_SPTR(Effect)
FWD_STRUCT_SPTR(IDrawContext)
FWD_STRUCT_SPTR(IMaterialContext)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT DrawWrapper {

  static bool IsWrapper(const EffectPtr& effect);
  static bool IsWrapper(const DrawWrapperPtr& effect);
  static EffectPtr const& GetEffect(const EffectPtr& effect);
  static EffectPtr const& GetEffect(const DrawWrapperPtr& effect);

  DrawWrapper(ThinEngine* engine, bool createMaterialContext = true);
  ~DrawWrapper(); // = default

  void setEffect(const EffectPtr& effect, const std::optional<std::string>& defines = std::nullopt);
  void setEffect(const EffectPtr& effect,
                 const std::optional<MaterialDefines>& defines = std::nullopt);

  EffectPtr effect;
  std::optional<std::variant<std::string, MaterialDefines>> defines;
  IMaterialContextPtr materialContext;
  IDrawContextPtr drawContext;

}; // end of struct DrawWrapper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_DRAW_WRAPPER_H
