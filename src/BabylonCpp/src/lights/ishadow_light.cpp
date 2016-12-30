#include <babylon/lights/ishadow_light.h>

#include <babylon/lights/shadows/shadow_generator.h>

namespace BABYLON {

IShadowLight::IShadowLight() : _shadowGenerator{nullptr}
{
}

IShadowLight::~IShadowLight()
{
}
}
