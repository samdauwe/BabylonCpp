#include <babylon/tools/serialization_helper.h>

#include <babylon/core/json.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>

namespace BABYLON {

Light* SerializationHelper::Parse(Light* light, const Json::value& parsedLight,
                                  Scene* /*scene*/)
{
  if (!light) {
    return nullptr;
  }

  switch (light->getTypeID()) {
    case 0: {
      /** PointLight **/
      auto pointLight = dynamic_cast<PointLight*>(light);
      // position
      if (parsedLight.contains("position")) {
        pointLight->position
          = Vector3::FromArray(Json::ToArray<float>(parsedLight, "position"));
      }
    } break;
    case 1: {
      /** DirectionalLight **/
      auto directionalLight = dynamic_cast<DirectionalLight*>(light);
      // position
      if (parsedLight.contains("position")) {
        directionalLight->position
          = Vector3::FromArray(Json::ToArray<float>(parsedLight, "position"));
      }
      // direction
      if (parsedLight.contains("direction")) {
        directionalLight->direction
          = Vector3::FromArray(Json::ToArray<float>(parsedLight, "direction"));
      }
      // shadowOrthoScale
      if (parsedLight.contains("shadowOrthoScale")) {
        directionalLight->shadowOrthoScale
          = Json::GetNumber(parsedLight, "shadowOrthoScale", 0.5f);
      }
      // autoUpdateExtends
      if (parsedLight.contains("autoUpdateExtends")) {
        directionalLight->autoUpdateExtends
          = Json::GetBool(parsedLight, "autoUpdateExtends", true);
      }
    } break;
    case 2: {
      /** SpotLight **/
      auto spotLight = dynamic_cast<SpotLight*>(light);
      // position
      if (parsedLight.contains("position")) {
        spotLight->position
          = Vector3::FromArray(Json::ToArray<float>(parsedLight, "position"));
      }
      // direction
      if (parsedLight.contains("direction")) {
        spotLight->direction
          = Vector3::FromArray(Json::ToArray<float>(parsedLight, "direction"));
      }
      // angle
      if (parsedLight.contains("angle")) {
        spotLight->angle = Json::GetNumber(parsedLight, "angle", 0.f);
      }
      // exponent
      if (parsedLight.contains("exponent")) {
        spotLight->exponent = Json::GetNumber(parsedLight, "exponent", 0.f);
      }
    } break;
    case 3: {
      /** HemisphericLight **/
      auto hemisphericLight = dynamic_cast<HemisphericLight*>(light);
      // groundColor
      if (parsedLight.contains("groundColor")) {
        hemisphericLight->groundColor
          = Color3::FromArray(Json::ToArray<float>(parsedLight, "groundColor"));
      }
      // direction
      if (parsedLight.contains("direction")) {
        hemisphericLight->direction
          = Vector3::FromArray(Json::ToArray<float>(parsedLight, "direction"));
      }
    } break;
    default:
      break;
  }

  return light;
}

} // end of namespace BABYLON
