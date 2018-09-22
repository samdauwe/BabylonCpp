#include <babylon/lights/ishadow_light.h>

#include <babylon/lights/shadows/shadow_generator.h>

namespace BABYLON {

IShadowLight::IShadowLight(const std::string& iName, Scene* scene)
    : Light{iName, scene}
    , position{this, &IShadowLight::get_position, &IShadowLight::set_position}
    , direction{this, &IShadowLight::get_direction,
                &IShadowLight::set_direction}
{
}

IShadowLight::~IShadowLight()
{
}

} // end of namespace BABYLON
