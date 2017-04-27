#include <babylon/lights/ishadow_light.h>

#include <babylon/lights/shadows/shadow_generator.h>

namespace BABYLON {

IShadowLight::IShadowLight(const std::string& iName, Scene* scene)
    : Light{iName, scene}, _shadowGenerator{nullptr}
{
}

IShadowLight::~IShadowLight()
{
}

} // end of namespace BABYLON
