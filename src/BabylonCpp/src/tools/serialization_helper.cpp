#include <babylon/tools/serialization_helper.h>

#include <babylon/cameras/arc_follow_camera.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/json.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

BaseTexture*
SerializationHelper::Parse(BaseTexture* baseTexture,
                           const Json::value& /*parsedBaseTexture*/,
                           Scene* /*scene*/)
{
  return baseTexture;
}

Camera* SerializationHelper::Parse(Camera* camera,
                                   const Json::value& parsedCamera,
                                   Scene* /*scene*/)
{
  if (!camera) {
    return nullptr;
  }

  string_t cameraClassName{camera->getClassName()};
  if (cameraClassName == "ArcRotateCamera") {
    /** ArcRotateCamera **/
    auto arcRotateCamera = dynamic_cast<ArcRotateCamera*>(camera);
    // alpha
    if (parsedCamera.contains("alpha")) {
      arcRotateCamera->alpha
        = Json::GetNumber<float>(parsedCamera, "alpha", 0.f);
    }
    // beta
    if (parsedCamera.contains("beta")) {
      arcRotateCamera->beta = Json::GetNumber<float>(parsedCamera, "beta", 0.f);
    }
    // radius
    if (parsedCamera.contains("radius")) {
      arcRotateCamera->radius
        = Json::GetNumber<float>(parsedCamera, "radius", 0.f);
    }
    // target
    if (parsedCamera.contains("target")) {
      arcRotateCamera->setTarget(
        Vector3::FromArray(Json::ToArray<float>(parsedCamera, "target")));
    }
    // inertialAlphaOffset
    if (parsedCamera.contains("inertialAlphaOffset")) {
      arcRotateCamera->inertialAlphaOffset
        = Json::GetNumber<float>(parsedCamera, "inertialAlphaOffset", 0.f);
    }
    // inertialBetaOffset
    if (parsedCamera.contains("inertialBetaOffset")) {
      arcRotateCamera->inertialBetaOffset
        = Json::GetNumber<float>(parsedCamera, "inertialBetaOffset", 0.f);
    }
    // inertialRadiusOffset
    if (parsedCamera.contains("inertialRadiusOffset")) {
      arcRotateCamera->inertialRadiusOffset
        = Json::GetNumber<float>(parsedCamera, "inertialRadiusOffset", 0.f);
    }
    // lowerAlphaLimit
    if (parsedCamera.contains("lowerAlphaLimit")) {
      arcRotateCamera->lowerAlphaLimit
        = Json::GetNumber<float>(parsedCamera, "lowerAlphaLimit", 0.f);
    }
    // upperAlphaLimit
    if (parsedCamera.contains("upperAlphaLimit")) {
      arcRotateCamera->upperAlphaLimit
        = Json::GetNumber<float>(parsedCamera, "upperAlphaLimit", 0.f);
    }
    // lowerBetaLimit
    if (parsedCamera.contains("lowerBetaLimit")) {
      arcRotateCamera->lowerBetaLimit
        = Json::GetNumber<float>(parsedCamera, "lowerBetaLimit", 0.01f);
    }
    // upperBetaLimit
    if (parsedCamera.contains("upperBetaLimit")) {
      arcRotateCamera->upperBetaLimit
        = Json::GetNumber<float>(parsedCamera, "upperBetaLimit", Math::PI);
    }
    // lowerRadiusLimit
    if (parsedCamera.contains("lowerRadiusLimit")) {
      arcRotateCamera->lowerRadiusLimit
        = Json::GetNumber<float>(parsedCamera, "lowerRadiusLimit", 0.f);
    }
    // upperRadiusLimit
    if (parsedCamera.contains("upperRadiusLimit")) {
      arcRotateCamera->upperRadiusLimit
        = Json::GetNumber<float>(parsedCamera, "upperRadiusLimit", 0.f);
    }
    // inertialPanningX
    if (parsedCamera.contains("inertialPanningX")) {
      arcRotateCamera->inertialPanningX
        = Json::GetNumber<float>(parsedCamera, "inertialPanningX", 0.f);
    }
    // inertialPanningY
    if (parsedCamera.contains("inertialPanningY")) {
      arcRotateCamera->inertialPanningY
        = Json::GetNumber<float>(parsedCamera, "inertialPanningY", 0.f);
    }
    // zoomOnFactor
    if (parsedCamera.contains("zoomOnFactor")) {
      arcRotateCamera->zoomOnFactor
        = Json::GetNumber<float>(parsedCamera, "zoomOnFactor", 1.f);
    }
    // allowUpsideDown
    if (parsedCamera.contains("allowUpsideDown")) {
      arcRotateCamera->allowUpsideDown
        = Json::GetBool(parsedCamera, "allowUpsideDown", true);
    }
  }
  else if ((cameraClassName == "FollowCamera")
           || (cameraClassName == "ArcFollowCamera")) {
    /** FollowCamera / ArcFollowCamera **/
    auto followCamera = dynamic_cast<FollowCamera*>(camera);
    // zoomOnFactor
    if (parsedCamera.contains("radius")) {
      followCamera->radius
        = Json::GetNumber<float>(parsedCamera, "radius", 12.f);
    }
    // zoomOnFactor
    if (parsedCamera.contains("rotationOffset")) {
      followCamera->rotationOffset
        = Json::GetNumber<float>(parsedCamera, "rotationOffset", 0.f);
    }
    // zoomOnFactor
    if (parsedCamera.contains("heightOffset")) {
      followCamera->heightOffset
        = Json::GetNumber<float>(parsedCamera, "heightOffset", 4.f);
    }
    // zoomOnFactor
    if (parsedCamera.contains("cameraAcceleration")) {
      followCamera->cameraAcceleration
        = Json::GetNumber<float>(parsedCamera, "cameraAcceleration", 0.05f);
    }
    // zoomOnFactor
    if (parsedCamera.contains("maxCameraSpeed")) {
      followCamera->maxCameraSpeed
        = Json::GetNumber<float>(parsedCamera, "maxCameraSpeed", 20.f);
    }
  }
  else if (cameraClassName == "FreeCamera") {
    /** FreeCamera **/
    auto freeCamera = dynamic_cast<FreeCamera*>(camera);
    // ellipsoid
    if (parsedCamera.contains("ellipsoid")) {
      freeCamera->ellipsoid
        = Vector3::FromArray(Json::ToArray<float>(parsedCamera, "ellipsoid"));
    }
    // checkCollisions
    if (parsedCamera.contains("checkCollisions")) {
      freeCamera->checkCollisions
        = Json::GetBool(parsedCamera, "checkCollisions");
    }
    // applyGravity
    if (parsedCamera.contains("applyGravity")) {
      freeCamera->applyGravity = Json::GetBool(parsedCamera, "applyGravity");
    }
  }

  return camera;
}

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
        pointLight->setPosition(
          Vector3::FromArray(Json::ToArray<float>(parsedLight, "position")));
      }
    } break;
    case 1: {
      /** DirectionalLight **/
      auto directionalLight = dynamic_cast<DirectionalLight*>(light);
      // position
      if (parsedLight.contains("position")) {
        directionalLight->setPosition(
          Vector3::FromArray(Json::ToArray<float>(parsedLight, "position")));
      }
      // direction
      if (parsedLight.contains("direction")) {
        directionalLight->setDirection(
          Vector3::FromArray(Json::ToArray<float>(parsedLight, "direction")));
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
        spotLight->setPosition(
          Vector3::FromArray(Json::ToArray<float>(parsedLight, "position")));
      }
      // direction
      if (parsedLight.contains("direction")) {
        spotLight->setDirection(
          Vector3::FromArray(Json::ToArray<float>(parsedLight, "direction")));
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

StandardMaterial* SerializationHelper::Parse(StandardMaterial* standardMaterial,
                                             const Json::value& parsedMaterial,
                                             Scene* /*scene*/,
                                             const string_t& /*rootUrl*/)
{
  if (!standardMaterial) {
    return nullptr;
  }

  // ambient
  if (parsedMaterial.contains("ambient")) {
    standardMaterial->ambientColor
      = Color3::FromArray(Json::ToArray<float>(parsedMaterial, "ambient"));
  }
  // diffuse
  if (parsedMaterial.contains("diffuse")) {
    standardMaterial->diffuseColor
      = Color3::FromArray(Json::ToArray<float>(parsedMaterial, "diffuse"));
  }
  // specular
  if (parsedMaterial.contains("specular")) {
    standardMaterial->specularColor
      = Color3::FromArray(Json::ToArray<float>(parsedMaterial, "specular"));
  }
  // emissive
  if (parsedMaterial.contains("emissive")) {
    standardMaterial->emissiveColor
      = Color3::FromArray(Json::ToArray<float>(parsedMaterial, "emissive"));
  }
  // specularPower
  if (parsedMaterial.contains("specularPower")) {
    standardMaterial->specularPower
      = Json::GetNumber<float>(parsedMaterial, "specularPower", 64.f);
  }
  // useAlphaFromDiffuseTexture
  if (parsedMaterial.contains("useAlphaFromDiffuseTexture")) {
    standardMaterial->setUseAlphaFromDiffuseTexture(
      Json::GetBool(parsedMaterial, "useAlphaFromDiffuseTexture"));
  }
  // useEmissiveAsIllumination
  if (parsedMaterial.contains("useEmissiveAsIllumination")) {
    standardMaterial->setUseEmissiveAsIllumination(
      Json::GetBool(parsedMaterial, "useEmissiveAsIllumination"));
  }
  // linkEmissiveWithDiffuse
  if (parsedMaterial.contains("linkEmissiveWithDiffuse")) {
    standardMaterial->setLinkEmissiveWithDiffuse(
      Json::GetBool(parsedMaterial, "linkEmissiveWithDiffuse"));
  }
  // useReflectionFresnelFromSpecular
  if (parsedMaterial.contains("useReflectionFresnelFromSpecular")) {
    standardMaterial->setUseReflectionFresnelFromSpecular(
      Json::GetBool(parsedMaterial, "useReflectionFresnelFromSpecular"));
  }
  // useSpecularOverAlpha
  if (parsedMaterial.contains("useSpecularOverAlpha")) {
    standardMaterial->setUseSpecularOverAlpha(
      Json::GetBool(parsedMaterial, "useSpecularOverAlpha"));
  }
  // useReflectionOverAlpha
  if (parsedMaterial.contains("useReflectionOverAlpha")) {
    standardMaterial->setUseReflectionOverAlpha(
      Json::GetBool(parsedMaterial, "useReflectionOverAlpha"));
  }
  // disableLighting
  if (parsedMaterial.contains("disableLighting")) {
    standardMaterial->setDisableLighting(
      Json::GetBool(parsedMaterial, "disableLighting"));
  }
  // useParallax
  if (parsedMaterial.contains("useParallax")) {
    standardMaterial->setUseParallax(
      Json::GetBool(parsedMaterial, "useParallax"));
  }
  // useParallaxOcclusion
  if (parsedMaterial.contains("useParallaxOcclusion")) {
    standardMaterial->setUseParallaxOcclusion(
      Json::GetBool(parsedMaterial, "useParallaxOcclusion"));
  }
  // parallaxScaleBias
  if (parsedMaterial.contains("parallaxScaleBias")) {
    standardMaterial->parallaxScaleBias
      = Json::GetNumber<float>(parsedMaterial, "parallaxScaleBias", 0.5f);
  }
  // roughness
  if (parsedMaterial.contains("roughness")) {
    standardMaterial->setRoughness(
      Json::GetNumber<float>(parsedMaterial, "roughness", 0.f));
  }
  // indexOfRefraction
  if (parsedMaterial.contains("indexOfRefraction")) {
    standardMaterial->indexOfRefraction
      = Json::GetNumber<float>(parsedMaterial, "indexOfRefraction", 0.98f);
  }
  // invertRefractionY
  if (parsedMaterial.contains("invertRefractionY")) {
    standardMaterial->invertRefractionY
      = Json::GetBool(parsedMaterial, "invertRefractionY", true);
  }
  // useLightmapAsShadowmap
  if (parsedMaterial.contains("useLightmapAsShadowmap")) {
    standardMaterial->setUseLightmapAsShadowmap(
      Json::GetBool(parsedMaterial, "useLightmapAsShadowmap", true));
  }
  // useGlossinessFromSpecularMapAlpha
  if (parsedMaterial.contains("useGlossinessFromSpecularMapAlpha")) {
    standardMaterial->setUseGlossinessFromSpecularMapAlpha(
      Json::GetBool(parsedMaterial, "useGlossinessFromSpecularMapAlpha"));
  }
  // maxSimultaneousLights
  if (parsedMaterial.contains("maxSimultaneousLights")) {
    standardMaterial->setMaxSimultaneousLights(
      Json::GetNumber<unsigned>(parsedMaterial, "maxSimultaneousLights", 4));
  }
  // invertNormalMapX
  if (parsedMaterial.contains("invertNormalMapX")) {
    standardMaterial->setInvertNormalMapX(
      Json::GetBool(parsedMaterial, "invertNormalMapX"));
  }
  // invertNormalMapY
  if (parsedMaterial.contains("invertNormalMapY")) {
    standardMaterial->setInvertNormalMapY(
      Json::GetBool(parsedMaterial, "invertNormalMapY"));
  }

  return standardMaterial;
}

} // end of namespace BABYLON
