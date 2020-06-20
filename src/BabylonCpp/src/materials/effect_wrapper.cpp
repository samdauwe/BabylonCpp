#include <babylon/materials/effect_wrapper.h>

#include <babylon/materials/effect.h>
#include <babylon/materials/effect_wrapper_creation_options.h>
#include <babylon/materials/ieffect_creation_options.h>

namespace BABYLON {

EffectWrapper::EffectWrapper(EffectWrapperCreationOptions& /*creationOptions*/)
{
  // TODO Implement
}

EffectWrapper::~EffectWrapper() = default;

void EffectWrapper::dispose()
{
  effect->dispose();
}

} // end of namespace BABYLON
