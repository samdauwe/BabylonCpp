#include <babylon/materials/draw_wrapper.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/materials/effect.h>

namespace BABYLON {

bool DrawWrapper::IsWrapper(const EffectPtr& iEffect)
{
  return iEffect->getPipelineContext() == nullptr;
}

bool DrawWrapper::IsWrapper(const DrawWrapperPtr& /*effect*/)
{
  return true;
}

EffectPtr const& DrawWrapper::GetEffect(const EffectPtr& iEffect)
{
  return iEffect;
}

EffectPtr const& DrawWrapper::GetEffect(const DrawWrapperPtr& iEffect)
{
  return iEffect->effect;
}

DrawWrapper::DrawWrapper(ThinEngine* engine, bool createMaterialContext)
    : materialContext{nullptr}, drawContext{nullptr}
{
  effect      = nullptr;
  defines     = std::nullopt;
  drawContext = engine->createDrawContext();
  if (createMaterialContext) {
    materialContext = engine->createMaterialContext();
  }
}

DrawWrapper::~DrawWrapper() = default;

void DrawWrapper::setEffect(const EffectPtr& iEffect, const std::optional<std::string>& iDefines)
{
  effect = iEffect;
  if (iDefines.has_value()) {
    defines = *iDefines;
  }
}

void DrawWrapper::setEffect(const EffectPtr& iEffect,
                            const std::optional<MaterialDefines>& iDefines)
{
  effect = iEffect;
  if (iDefines.has_value()) {
    defines = *iDefines;
  }
}

} // end of namespace BABYLON
