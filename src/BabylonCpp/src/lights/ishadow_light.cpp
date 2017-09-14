#include <babylon/lights/ishadow_light.h>

#include <babylon/lights/shadows/shadow_generator.h>

namespace BABYLON {

IShadowLight::IShadowLight(const string_t& iName, Scene* scene)
    : Light{iName, scene}
{
}

IShadowLight::~IShadowLight()
{
}

} // end of namespace BABYLON
