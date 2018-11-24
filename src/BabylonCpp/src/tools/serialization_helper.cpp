#include <babylon/tools/serialization_helper.h>

#include <babylon/cameras/arc_follow_camera.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/json_util.h>
#include <babylon/lights/directional_light.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/lights/spot_light.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

BaseTexture* SerializationHelper::Parse(BaseTexture* baseTexture,
                                        const json& /*parsedBaseTexture*/,
                                        Scene* /*scene*/)
{
  return baseTexture;
}

CameraPtr SerializationHelper::Parse(const CameraPtr& camera,
                                     const json& parsedCamera, Scene* /*scene*/)
{
  if (!camera) {
    return nullptr;
  }

  std::string cameraClassName{camera->getClassName()};
  if (cameraClassName == "ArcRotateCamera") {
    /** ArcRotateCamera **/
    auto arcRotateCamera = std::static_pointer_cast<ArcRotateCamera>(camera);
    // alpha
    if (json_util::has_key(parsedCamera, "alpha")) {
      arcRotateCamera->alpha
        = json_util::get_number<float>(parsedCamera, "alpha", 0.f);
    }
    // beta
    if (json_util::has_key(parsedCamera, "beta")) {
      arcRotateCamera->beta
        = json_util::get_number<float>(parsedCamera, "beta", 0.f);
    }
    // radius
    if (json_util::has_key(parsedCamera, "radius")) {
      arcRotateCamera->radius
        = json_util::get_number<float>(parsedCamera, "radius", 0.f);
    }
    // target
    if (json_util::has_key(parsedCamera, "target")) {
      arcRotateCamera->setTarget(Vector3::FromArray(
        json_util::get_array<float>(parsedCamera, "target")));
    }
    // inertialAlphaOffset
    if (json_util::has_key(parsedCamera, "inertialAlphaOffset")) {
      arcRotateCamera->inertialAlphaOffset = json_util::get_number<float>(
        parsedCamera, "inertialAlphaOffset", 0.f);
    }
    // inertialBetaOffset
    if (json_util::has_key(parsedCamera, "inertialBetaOffset")) {
      arcRotateCamera->inertialBetaOffset
        = json_util::get_number<float>(parsedCamera, "inertialBetaOffset", 0.f);
    }
    // inertialRadiusOffset
    if (json_util::has_key(parsedCamera, "inertialRadiusOffset")) {
      arcRotateCamera->inertialRadiusOffset = json_util::get_number<float>(
        parsedCamera, "inertialRadiusOffset", 0.f);
    }
    // lowerAlphaLimit
    if (json_util::has_key(parsedCamera, "lowerAlphaLimit")) {
      arcRotateCamera->lowerAlphaLimit
        = json_util::get_number<float>(parsedCamera, "lowerAlphaLimit", 0.f);
    }
    // upperAlphaLimit
    if (json_util::has_key(parsedCamera, "upperAlphaLimit")) {
      arcRotateCamera->upperAlphaLimit
        = json_util::get_number<float>(parsedCamera, "upperAlphaLimit", 0.f);
    }
    // lowerBetaLimit
    if (json_util::has_key(parsedCamera, "lowerBetaLimit")) {
      arcRotateCamera->lowerBetaLimit
        = json_util::get_number<float>(parsedCamera, "lowerBetaLimit", 0.01f);
    }
    // upperBetaLimit
    if (json_util::has_key(parsedCamera, "upperBetaLimit")) {
      arcRotateCamera->upperBetaLimit = json_util::get_number<float>(
        parsedCamera, "upperBetaLimit", Math::PI);
    }
    // lowerRadiusLimit
    if (json_util::has_key(parsedCamera, "lowerRadiusLimit")) {
      arcRotateCamera->lowerRadiusLimit
        = json_util::get_number<float>(parsedCamera, "lowerRadiusLimit", 0.f);
    }
    // upperRadiusLimit
    if (json_util::has_key(parsedCamera, "upperRadiusLimit")) {
      arcRotateCamera->upperRadiusLimit
        = json_util::get_number<float>(parsedCamera, "upperRadiusLimit", 0.f);
    }
    // inertialPanningX
    if (json_util::has_key(parsedCamera, "inertialPanningX")) {
      arcRotateCamera->inertialPanningX
        = json_util::get_number<float>(parsedCamera, "inertialPanningX", 0.f);
    }
    // inertialPanningY
    if (json_util::has_key(parsedCamera, "inertialPanningY")) {
      arcRotateCamera->inertialPanningY
        = json_util::get_number<float>(parsedCamera, "inertialPanningY", 0.f);
    }
    // zoomOnFactor
    if (json_util::has_key(parsedCamera, "zoomOnFactor")) {
      arcRotateCamera->zoomOnFactor
        = json_util::get_number<float>(parsedCamera, "zoomOnFactor", 1.f);
    }
    // allowUpsideDown
    if (json_util::has_key(parsedCamera, "allowUpsideDown")) {
      arcRotateCamera->allowUpsideDown
        = json_util::get_bool(parsedCamera, "allowUpsideDown", true);
    }
  }
  else if ((cameraClassName == "FollowCamera")
           || (cameraClassName == "ArcFollowCamera")) {
    /** FollowCamera / ArcFollowCamera **/
    auto followCamera = std::static_pointer_cast<FollowCamera>(camera);
    // zoomOnFactor
    if (json_util::has_key(parsedCamera, "radius")) {
      followCamera->radius
        = json_util::get_number<float>(parsedCamera, "radius", 12.f);
    }
    // zoomOnFactor
    if (json_util::has_key(parsedCamera, "rotationOffset")) {
      followCamera->rotationOffset
        = json_util::get_number<float>(parsedCamera, "rotationOffset", 0.f);
    }
    // zoomOnFactor
    if (json_util::has_key(parsedCamera, "heightOffset")) {
      followCamera->heightOffset
        = json_util::get_number<float>(parsedCamera, "heightOffset", 4.f);
    }
    // zoomOnFactor
    if (json_util::has_key(parsedCamera, "cameraAcceleration")) {
      followCamera->cameraAcceleration = json_util::get_number<float>(
        parsedCamera, "cameraAcceleration", 0.05f);
    }
    // zoomOnFactor
    if (json_util::has_key(parsedCamera, "maxCameraSpeed")) {
      followCamera->maxCameraSpeed
        = json_util::get_number<float>(parsedCamera, "maxCameraSpeed", 20.f);
    }
  }
  else if (cameraClassName == "FreeCamera") {
    /** FreeCamera **/
    auto freeCamera = std::static_pointer_cast<FreeCamera>(camera);
    // ellipsoid
    if (json_util::has_key(parsedCamera, "ellipsoid")) {
      freeCamera->ellipsoid = Vector3::FromArray(
        json_util::get_array<float>(parsedCamera, "ellipsoid"));
    }
    // checkCollisions
    if (json_util::has_key(parsedCamera, "checkCollisions")) {
      freeCamera->checkCollisions
        = json_util::get_bool(parsedCamera, "checkCollisions");
    }
    // applyGravity
    if (json_util::has_key(parsedCamera, "applyGravity")) {
      freeCamera->applyGravity
        = json_util::get_bool(parsedCamera, "applyGravity");
    }
  }

  return camera;
}

LightPtr SerializationHelper::Parse(const LightPtr& light,
                                    const json& parsedLight, Scene* /*scene*/)
{
  if (!light) {
    return nullptr;
  }

  switch (light->getTypeID()) {
    case 0: {
      /** PointLight **/
      auto pointLight = std::static_pointer_cast<PointLight>(light);
      // position
      if (json_util::has_key(parsedLight, "position")) {
        pointLight->position = Vector3::FromArray(
          json_util::get_array<float>(parsedLight, "position"));
      }
    } break;
    case 1: {
      /** DirectionalLight **/
      auto directionalLight = std::static_pointer_cast<DirectionalLight>(light);
      // position
      if (json_util::has_key(parsedLight, "position")) {
        directionalLight->position = Vector3::FromArray(
          json_util::get_array<float>(parsedLight, "position"));
      }
      // direction
      if (json_util::has_key(parsedLight, "direction")) {
        directionalLight->direction = Vector3::FromArray(
          json_util::get_array<float>(parsedLight, "direction"));
      }
      // shadowOrthoScale
      if (json_util::has_key(parsedLight, "shadowOrthoScale")) {
        directionalLight->shadowOrthoScale
          = json_util::get_number(parsedLight, "shadowOrthoScale", 0.5f);
      }
      // autoUpdateExtends
      if (json_util::has_key(parsedLight, "autoUpdateExtends")) {
        directionalLight->autoUpdateExtends
          = json_util::get_bool(parsedLight, "autoUpdateExtends", true);
      }
    } break;
    case 2: {
      /** SpotLight **/
      auto spotLight = std::static_pointer_cast<SpotLight>(light);
      // position
      if (json_util::has_key(parsedLight, "position")) {
        spotLight->position = Vector3::FromArray(
          json_util::get_array<float>(parsedLight, "position"));
      }
      // direction
      if (json_util::has_key(parsedLight, "direction")) {
        spotLight->direction = Vector3::FromArray(
          json_util::get_array<float>(parsedLight, "direction"));
      }
      // angle
      if (json_util::has_key(parsedLight, "angle")) {
        spotLight->angle = json_util::get_number(parsedLight, "angle", 0.f);
      }
      // exponent
      if (json_util::has_key(parsedLight, "exponent")) {
        spotLight->exponent
          = json_util::get_number(parsedLight, "exponent", 0.f);
      }
    } break;
    case 3: {
      /** HemisphericLight **/
      auto hemisphericLight = std::static_pointer_cast<HemisphericLight>(light);
      // groundColor
      if (json_util::has_key(parsedLight, "groundColor")) {
        hemisphericLight->groundColor = Color3::FromArray(
          json_util::get_array<float>(parsedLight, "groundColor"));
      }
      // direction
      if (json_util::has_key(parsedLight, "direction")) {
        hemisphericLight->direction = Vector3::FromArray(
          json_util::get_array<float>(parsedLight, "direction"));
      }
    } break;
    default:
      break;
  }

  return light;
}

StandardMaterialPtr
SerializationHelper::Parse(const StandardMaterialPtr& standardMaterial,
                           const json& parsedMaterial, Scene* /*scene*/,
                           const std::string& /*rootUrl*/)
{
  if (!standardMaterial) {
    return nullptr;
  }

  // ambient
  if (json_util::has_key(parsedMaterial, "ambient")) {
    standardMaterial->ambientColor = Color3::FromArray(
      json_util::get_array<float>(parsedMaterial, "ambient"));
  }
  // diffuse
  if (json_util::has_key(parsedMaterial, "diffuse")) {
    standardMaterial->diffuseColor = Color3::FromArray(
      json_util::get_array<float>(parsedMaterial, "diffuse"));
  }
  // specular
  if (json_util::has_key(parsedMaterial, "specular")) {
    standardMaterial->specularColor = Color3::FromArray(
      json_util::get_array<float>(parsedMaterial, "specular"));
  }
  // emissive
  if (json_util::has_key(parsedMaterial, "emissive")) {
    standardMaterial->emissiveColor = Color3::FromArray(
      json_util::get_array<float>(parsedMaterial, "emissive"));
  }
  // specularPower
  if (json_util::has_key(parsedMaterial, "specularPower")) {
    standardMaterial->specularPower
      = json_util::get_number<float>(parsedMaterial, "specularPower", 64.f);
  }
  // useAlphaFromDiffuseTexture
  if (json_util::has_key(parsedMaterial, "useAlphaFromDiffuseTexture")) {
    standardMaterial->useAlphaFromDiffuseTexture
      = json_util::get_bool(parsedMaterial, "useAlphaFromDiffuseTexture");
  }
  // useEmissiveAsIllumination
  if (json_util::has_key(parsedMaterial, "useEmissiveAsIllumination")) {
    standardMaterial->useEmissiveAsIllumination
      = json_util::get_bool(parsedMaterial, "useEmissiveAsIllumination");
  }
  // linkEmissiveWithDiffuse
  if (json_util::has_key(parsedMaterial, "linkEmissiveWithDiffuse")) {
    standardMaterial->linkEmissiveWithDiffuse
      = json_util::get_bool(parsedMaterial, "linkEmissiveWithDiffuse");
  }
  // useReflectionFresnelFromSpecular
  if (json_util::has_key(parsedMaterial, "useReflectionFresnelFromSpecular")) {
    standardMaterial->useReflectionFresnelFromSpecular
      = json_util::get_bool(parsedMaterial, "useReflectionFresnelFromSpecular");
  }
  // useSpecularOverAlpha
  if (json_util::has_key(parsedMaterial, "useSpecularOverAlpha")) {
    standardMaterial->useSpecularOverAlpha
      = json_util::get_bool(parsedMaterial, "useSpecularOverAlpha");
  }
  // useReflectionOverAlpha
  if (json_util::has_key(parsedMaterial, "useReflectionOverAlpha")) {
    standardMaterial->useReflectionOverAlpha
      = json_util::get_bool(parsedMaterial, "useReflectionOverAlpha");
  }
  // disableLighting
  if (json_util::has_key(parsedMaterial, "disableLighting")) {
    standardMaterial->disableLighting
      = json_util::get_bool(parsedMaterial, "disableLighting");
  }
  // useParallax
  if (json_util::has_key(parsedMaterial, "useParallax")) {
    standardMaterial->useParallax
      = json_util::get_bool(parsedMaterial, "useParallax");
  }
  // useParallaxOcclusion
  if (json_util::has_key(parsedMaterial, "useParallaxOcclusion")) {
    standardMaterial->useParallaxOcclusion
      = json_util::get_bool(parsedMaterial, "useParallaxOcclusion");
  }
  // parallaxScaleBias
  if (json_util::has_key(parsedMaterial, "parallaxScaleBias")) {
    standardMaterial->parallaxScaleBias
      = json_util::get_number<float>(parsedMaterial, "parallaxScaleBias", 0.5f);
  }
  // roughness
  if (json_util::has_key(parsedMaterial, "roughness")) {
    standardMaterial->roughness
      = json_util::get_number<float>(parsedMaterial, "roughness", 0.f);
  }
  // indexOfRefraction
  if (json_util::has_key(parsedMaterial, "indexOfRefraction")) {
    standardMaterial->indexOfRefraction = json_util::get_number<float>(
      parsedMaterial, "indexOfRefraction", 0.98f);
  }
  // invertRefractionY
  if (json_util::has_key(parsedMaterial, "invertRefractionY")) {
    standardMaterial->invertRefractionY
      = json_util::get_bool(parsedMaterial, "invertRefractionY", true);
  }
  // useLightmapAsShadowmap
  if (json_util::has_key(parsedMaterial, "useLightmapAsShadowmap")) {
    standardMaterial->useLightmapAsShadowmap
      = json_util::get_bool(parsedMaterial, "useLightmapAsShadowmap", true);
  }
  // useGlossinessFromSpecularMapAlpha
  if (json_util::has_key(parsedMaterial, "useGlossinessFromSpecularMapAlpha")) {
    standardMaterial->useGlossinessFromSpecularMapAlpha = json_util::get_bool(
      parsedMaterial, "useGlossinessFromSpecularMapAlpha");
  }
  // maxSimultaneousLights
  if (json_util::has_key(parsedMaterial, "maxSimultaneousLights")) {
    standardMaterial->maxSimultaneousLights = json_util::get_number<unsigned>(
      parsedMaterial, "maxSimultaneousLights", 4);
  }
  // invertNormalMapX
  if (json_util::has_key(parsedMaterial, "invertNormalMapX")) {
    standardMaterial->invertNormalMapX
      = json_util::get_bool(parsedMaterial, "invertNormalMapX");
  }
  // invertNormalMapY
  if (json_util::has_key(parsedMaterial, "invertNormalMapY")) {
    standardMaterial->invertNormalMapY
      = json_util::get_bool(parsedMaterial, "invertNormalMapY");
  }

  return standardMaterial;
}

} // end of namespace BABYLON
