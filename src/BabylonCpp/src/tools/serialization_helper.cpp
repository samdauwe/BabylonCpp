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

CameraPtr
SerializationHelper::Parse(const std::function<CameraPtr()>& creationFunction,
                           const json& source, Scene* /*scene*/,
                           const std::string& /*rootUrl*/)
{
  auto camera = creationFunction();

  if (!camera) {
    return nullptr;
  }

  std::string cameraClassName{camera->getClassName()};
  if (cameraClassName == "ArcRotateCamera") {
    /** ArcRotateCamera **/
    auto arcRotateCamera = std::static_pointer_cast<ArcRotateCamera>(camera);
    // alpha
    if (json_util::has_key(source, "alpha")) {
      arcRotateCamera->alpha
        = json_util::get_number<float>(source, "alpha", 0.f);
    }
    // beta
    if (json_util::has_key(source, "beta")) {
      arcRotateCamera->beta = json_util::get_number<float>(source, "beta", 0.f);
    }
    // radius
    if (json_util::has_key(source, "radius")) {
      arcRotateCamera->radius
        = json_util::get_number<float>(source, "radius", 0.f);
    }
    // target
    if (json_util::has_key(source, "target")) {
      arcRotateCamera->setTarget(
        Vector3::FromArray(json_util::get_array<float>(source, "target")));
    }
    // inertialAlphaOffset
    if (json_util::has_key(source, "inertialAlphaOffset")) {
      arcRotateCamera->inertialAlphaOffset
        = json_util::get_number<float>(source, "inertialAlphaOffset", 0.f);
    }
    // inertialBetaOffset
    if (json_util::has_key(source, "inertialBetaOffset")) {
      arcRotateCamera->inertialBetaOffset
        = json_util::get_number<float>(source, "inertialBetaOffset", 0.f);
    }
    // inertialRadiusOffset
    if (json_util::has_key(source, "inertialRadiusOffset")) {
      arcRotateCamera->inertialRadiusOffset
        = json_util::get_number<float>(source, "inertialRadiusOffset", 0.f);
    }
    // lowerAlphaLimit
    if (json_util::has_key(source, "lowerAlphaLimit")) {
      arcRotateCamera->lowerAlphaLimit
        = json_util::get_number<float>(source, "lowerAlphaLimit", 0.f);
    }
    // upperAlphaLimit
    if (json_util::has_key(source, "upperAlphaLimit")) {
      arcRotateCamera->upperAlphaLimit
        = json_util::get_number<float>(source, "upperAlphaLimit", 0.f);
    }
    // lowerBetaLimit
    if (json_util::has_key(source, "lowerBetaLimit")) {
      arcRotateCamera->lowerBetaLimit
        = json_util::get_number<float>(source, "lowerBetaLimit", 0.01f);
    }
    // upperBetaLimit
    if (json_util::has_key(source, "upperBetaLimit")) {
      arcRotateCamera->upperBetaLimit
        = json_util::get_number<float>(source, "upperBetaLimit", Math::PI);
    }
    // lowerRadiusLimit
    if (json_util::has_key(source, "lowerRadiusLimit")) {
      arcRotateCamera->lowerRadiusLimit
        = json_util::get_number<float>(source, "lowerRadiusLimit", 0.f);
    }
    // upperRadiusLimit
    if (json_util::has_key(source, "upperRadiusLimit")) {
      arcRotateCamera->upperRadiusLimit
        = json_util::get_number<float>(source, "upperRadiusLimit", 0.f);
    }
    // inertialPanningX
    if (json_util::has_key(source, "inertialPanningX")) {
      arcRotateCamera->inertialPanningX
        = json_util::get_number<float>(source, "inertialPanningX", 0.f);
    }
    // inertialPanningY
    if (json_util::has_key(source, "inertialPanningY")) {
      arcRotateCamera->inertialPanningY
        = json_util::get_number<float>(source, "inertialPanningY", 0.f);
    }
    // zoomOnFactor
    if (json_util::has_key(source, "zoomOnFactor")) {
      arcRotateCamera->zoomOnFactor
        = json_util::get_number<float>(source, "zoomOnFactor", 1.f);
    }
    // allowUpsideDown
    if (json_util::has_key(source, "allowUpsideDown")) {
      arcRotateCamera->allowUpsideDown
        = json_util::get_bool(source, "allowUpsideDown", true);
    }
  }
  else if ((cameraClassName == "FollowCamera")
           || (cameraClassName == "ArcFollowCamera")) {
    /** FollowCamera / ArcFollowCamera **/
    auto followCamera = std::static_pointer_cast<FollowCamera>(camera);
    // zoomOnFactor
    if (json_util::has_key(source, "radius")) {
      followCamera->radius
        = json_util::get_number<float>(source, "radius", 12.f);
    }
    // zoomOnFactor
    if (json_util::has_key(source, "rotationOffset")) {
      followCamera->rotationOffset
        = json_util::get_number<float>(source, "rotationOffset", 0.f);
    }
    // zoomOnFactor
    if (json_util::has_key(source, "heightOffset")) {
      followCamera->heightOffset
        = json_util::get_number<float>(source, "heightOffset", 4.f);
    }
    // zoomOnFactor
    if (json_util::has_key(source, "cameraAcceleration")) {
      followCamera->cameraAcceleration
        = json_util::get_number<float>(source, "cameraAcceleration", 0.05f);
    }
    // zoomOnFactor
    if (json_util::has_key(source, "maxCameraSpeed")) {
      followCamera->maxCameraSpeed
        = json_util::get_number<float>(source, "maxCameraSpeed", 20.f);
    }
  }
  else if (cameraClassName == "FreeCamera") {
    /** FreeCamera **/
    auto freeCamera = std::static_pointer_cast<FreeCamera>(camera);
    // applyGravity
    if (json_util::has_key(source, "applyGravity")) {
      freeCamera->applyGravity = json_util::get_bool(source, "applyGravity");
    }
    // cameraRigMode
    if (json_util::has_key(source, "cameraRigMode")) {
      freeCamera->cameraRigMode
        = json_util::get_number<unsigned>(source, "cameraRigMode");
    }
    // checkCollisions
    if (json_util::has_key(source, "checkCollisions")) {
      freeCamera->checkCollisions
        = json_util::get_bool(source, "checkCollisions");
    }
    // ellipsoid
    if (json_util::has_key(source, "ellipsoid")) {
      freeCamera->ellipsoid
        = Vector3::FromArray(json_util::get_array<float>(source, "ellipsoid"));
    }
    // fov
    if (json_util::has_key(source, "fov")) {
      freeCamera->fov = json_util::get_number<float>(source, "fov");
    }
    // fov
    if (json_util::has_key(source, "inertia")) {
      freeCamera->inertia = json_util::get_number<float>(source, "inertia");
    }
    // maxZ
    if (json_util::has_key(source, "maxZ")) {
      freeCamera->maxZ = json_util::get_number<float>(source, "maxZ");
    }
    // minZ
    if (json_util::has_key(source, "minZ")) {
      freeCamera->minZ = json_util::get_number<float>(source, "minZ");
    }
    // position
    if (json_util::has_key(source, "position")) {
      freeCamera->position
        = Vector3::FromArray(json_util::get_array<float>(source, "position"));
    }
    // speed
    if (json_util::has_key(source, "speed")) {
      freeCamera->speed = json_util::get_number<float>(source, "speed");
    }
  }

  return camera;
}

LightPtr
SerializationHelper::Parse(const std::function<LightPtr()>& creationFunction,
                           const json& source, Scene* /*scene*/,
                           const std::string& /*rootUrl*/)
{
  auto light = creationFunction();

  if (!light) {
    return nullptr;
  }

  switch (light->getTypeID()) {
    case 0: {
      /** PointLight **/
      auto pointLight = std::static_pointer_cast<PointLight>(light);
      // position
      if (json_util::has_key(source, "position")) {
        pointLight->position
          = Vector3::FromArray(json_util::get_array<float>(source, "position"));
      }
    } break;
    case 1: {
      /** DirectionalLight **/
      auto directionalLight = std::static_pointer_cast<DirectionalLight>(light);
      // position
      if (json_util::has_key(source, "position")) {
        directionalLight->position
          = Vector3::FromArray(json_util::get_array<float>(source, "position"));
      }
      // direction
      if (json_util::has_key(source, "direction")) {
        directionalLight->direction = Vector3::FromArray(
          json_util::get_array<float>(source, "direction"));
      }
      // shadowOrthoScale
      if (json_util::has_key(source, "shadowOrthoScale")) {
        directionalLight->shadowOrthoScale
          = json_util::get_number(source, "shadowOrthoScale", 0.5f);
      }
      // autoUpdateExtends
      if (json_util::has_key(source, "autoUpdateExtends")) {
        directionalLight->autoUpdateExtends
          = json_util::get_bool(source, "autoUpdateExtends", true);
      }
    } break;
    case 2: {
      /** SpotLight **/
      auto spotLight = std::static_pointer_cast<SpotLight>(light);
      // position
      if (json_util::has_key(source, "position")) {
        spotLight->position
          = Vector3::FromArray(json_util::get_array<float>(source, "position"));
      }
      // direction
      if (json_util::has_key(source, "direction")) {
        spotLight->direction = Vector3::FromArray(
          json_util::get_array<float>(source, "direction"));
      }
      // angle
      if (json_util::has_key(source, "angle")) {
        spotLight->angle = json_util::get_number(source, "angle", 0.f);
      }
      // exponent
      if (json_util::has_key(source, "exponent")) {
        spotLight->exponent = json_util::get_number(source, "exponent", 0.f);
      }
    } break;
    case 3: {
      /** HemisphericLight **/
      auto hemisphericLight = std::static_pointer_cast<HemisphericLight>(light);
      // groundColor
      if (json_util::has_key(source, "groundColor")) {
        hemisphericLight->groundColor = Color3::FromArray(
          json_util::get_array<float>(source, "groundColor"));
      }
      // direction
      if (json_util::has_key(source, "direction")) {
        hemisphericLight->direction = Vector3::FromArray(
          json_util::get_array<float>(source, "direction"));
      }
    } break;
    default:
      break;
  }

  return light;
}

StandardMaterialPtr SerializationHelper::Parse(
  const std::function<StandardMaterialPtr()>& creationFunction,
  const json& source, Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  auto standardMaterial = creationFunction();

  if (!standardMaterial) {
    return nullptr;
  }

  // alpha
  if (json_util::has_key(source, "alpha")) {
    standardMaterial->alpha = json_util::get_number<float>(source, "alpha");
  }
  // ambient
  if (json_util::has_key(source, "ambient")) {
    standardMaterial->ambientColor
      = Color3::FromArray(json_util::get_array<float>(source, "ambient"));
  }
  // backFaceCulling
  if (json_util::has_key(source, "backFaceCulling")) {
    standardMaterial->backFaceCulling
      = json_util::get_bool(source, "backFaceCulling");
  }
  // diffuse
  if (json_util::has_key(source, "diffuse")) {
    standardMaterial->diffuseColor
      = Color3::FromArray(json_util::get_array<float>(source, "diffuse"));
  }
  // emissive
  if (json_util::has_key(source, "emissive")) {
    standardMaterial->emissiveColor
      = Color3::FromArray(json_util::get_array<float>(source, "emissive"));
  }
  // specular
  if (json_util::has_key(source, "specular")) {
    standardMaterial->specularColor
      = Color3::FromArray(json_util::get_array<float>(source, "specular"));
  }
  // specularPower
  if (json_util::has_key(source, "specularPower")) {
    standardMaterial->specularPower
      = json_util::get_number<float>(source, "specularPower", 64.f);
  }
  // useAlphaFromDiffuseTexture
  if (json_util::has_key(source, "useAlphaFromDiffuseTexture")) {
    standardMaterial->useAlphaFromDiffuseTexture
      = json_util::get_bool(source, "useAlphaFromDiffuseTexture");
  }
  // useEmissiveAsIllumination
  if (json_util::has_key(source, "useEmissiveAsIllumination")) {
    standardMaterial->useEmissiveAsIllumination
      = json_util::get_bool(source, "useEmissiveAsIllumination");
  }
  // linkEmissiveWithDiffuse
  if (json_util::has_key(source, "linkEmissiveWithDiffuse")) {
    standardMaterial->linkEmissiveWithDiffuse
      = json_util::get_bool(source, "linkEmissiveWithDiffuse");
  }
  // useReflectionFresnelFromSpecular
  if (json_util::has_key(source, "useReflectionFresnelFromSpecular")) {
    standardMaterial->useReflectionFresnelFromSpecular
      = json_util::get_bool(source, "useReflectionFresnelFromSpecular");
  }
  // useSpecularOverAlpha
  if (json_util::has_key(source, "useSpecularOverAlpha")) {
    standardMaterial->useSpecularOverAlpha
      = json_util::get_bool(source, "useSpecularOverAlpha");
  }
  // useReflectionOverAlpha
  if (json_util::has_key(source, "useReflectionOverAlpha")) {
    standardMaterial->useReflectionOverAlpha
      = json_util::get_bool(source, "useReflectionOverAlpha");
  }
  // disableLighting
  if (json_util::has_key(source, "disableLighting")) {
    standardMaterial->disableLighting
      = json_util::get_bool(source, "disableLighting");
  }
  // useParallax
  if (json_util::has_key(source, "useParallax")) {
    standardMaterial->useParallax = json_util::get_bool(source, "useParallax");
  }
  // useParallaxOcclusion
  if (json_util::has_key(source, "useParallaxOcclusion")) {
    standardMaterial->useParallaxOcclusion
      = json_util::get_bool(source, "useParallaxOcclusion");
  }
  // parallaxScaleBias
  if (json_util::has_key(source, "parallaxScaleBias")) {
    standardMaterial->parallaxScaleBias
      = json_util::get_number<float>(source, "parallaxScaleBias", 0.5f);
  }
  // roughness
  if (json_util::has_key(source, "roughness")) {
    standardMaterial->roughness
      = json_util::get_number<float>(source, "roughness", 0.f);
  }
  // indexOfRefraction
  if (json_util::has_key(source, "indexOfRefraction")) {
    standardMaterial->indexOfRefraction
      = json_util::get_number<float>(source, "indexOfRefraction", 0.98f);
  }
  // invertRefractionY
  if (json_util::has_key(source, "invertRefractionY")) {
    standardMaterial->invertRefractionY
      = json_util::get_bool(source, "invertRefractionY", true);
  }
  // useLightmapAsShadowmap
  if (json_util::has_key(source, "useLightmapAsShadowmap")) {
    standardMaterial->useLightmapAsShadowmap
      = json_util::get_bool(source, "useLightmapAsShadowmap", true);
  }
  // useGlossinessFromSpecularMapAlpha
  if (json_util::has_key(source, "useGlossinessFromSpecularMapAlpha")) {
    standardMaterial->useGlossinessFromSpecularMapAlpha
      = json_util::get_bool(source, "useGlossinessFromSpecularMapAlpha");
  }
  // maxSimultaneousLights
  if (json_util::has_key(source, "maxSimultaneousLights")) {
    standardMaterial->maxSimultaneousLights
      = json_util::get_number<unsigned>(source, "maxSimultaneousLights", 4);
  }
  // invertNormalMapX
  if (json_util::has_key(source, "invertNormalMapX")) {
    standardMaterial->invertNormalMapX
      = json_util::get_bool(source, "invertNormalMapX");
  }
  // invertNormalMapY
  if (json_util::has_key(source, "invertNormalMapY")) {
    standardMaterial->invertNormalMapY
      = json_util::get_bool(source, "invertNormalMapY");
  }

  return standardMaterial;
}

} // end of namespace BABYLON
