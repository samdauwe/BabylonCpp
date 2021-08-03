#include <babylon/materials/uniform_buffer_effect_common_accessor.h>

#include <babylon/materials/effect.h>
#include <babylon/materials/uniform_buffer.h>

namespace BABYLON {

UniformBufferEffectCommonAccessor::UniformBufferEffectCommonAccessor()
{
}

UniformBufferEffectCommonAccessor::~UniformBufferEffectCommonAccessor() = default;

bool UniformBufferEffectCommonAccessor::_isUbo(
  const std::variant<UniformBuffer*, Effect*>& /*uboOrEffect*/) const
{
  return false;
}

} // end of namespace BABYLON
