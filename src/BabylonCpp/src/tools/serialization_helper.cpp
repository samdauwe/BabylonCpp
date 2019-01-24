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
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

CubeTexturePtr SerializationHelper::Parse(
  const std::function<CubeTexturePtr()>& creationFunction,
  const json& /*source*/, Scene* /*scene*/, const std::string& /*rootUrl*/)
{
  auto texture = creationFunction();

  if (!texture) {
    return nullptr;
  }

  return texture;
}

TexturePtr
SerializationHelper::Parse(const std::function<TexturePtr()>& creationFunction,
                           const json& source, Scene* /*scene*/,
                           const std::string& /*rootUrl*/)
{
  auto texture = creationFunction();

  if (!texture) {
    return nullptr;
  }

  // coordinatesIndex
  if (json_util::has_key(source, "coordinatesIndex")
      && !json_util::is_null(source["coordinatesIndex"])) {
    texture->coordinatesIndex
      = json_util::get_number<unsigned>(source, "coordinatesIndex");
  }
  // coordinatesMode
  if (json_util::has_key(source, "coordinatesMode")
      && !json_util::is_null(source["coordinatesMode"])) {
    texture->coordinatesMode
      = json_util::get_number<unsigned>(source, "coordinatesMode");
  }
  // hasAlpha
  if (json_util::has_key(source, "hasAlpha")
      && !json_util::is_null(source["hasAlpha"])) {
    texture->hasAlpha = json_util::get_bool(source, "hasAlpha");
  }
  // coordinatesMode
  if (json_util::has_key(source, "level")
      && !json_util::is_null(source["level"])) {
    texture->level = json_util::get_number<unsigned>(source, "level");
  }
  // uAng
  if (json_util::has_key(source, "uAng")
      && !json_util::is_null(source["uAng"])) {
    texture->uAng = json_util::get_number<float>(source, "uAng");
  }
  // uOffset
  if (json_util::has_key(source, "uOffset")
      && !json_util::is_null(source["uOffset"])) {
    texture->uOffset = json_util::get_number<float>(source, "uOffset");
  }
  // uScale
  if (json_util::has_key(source, "uScale")
      && !json_util::is_null(source["uScale"])) {
    texture->uScale = json_util::get_number<float>(source, "uScale");
  }
  // vAng
  if (json_util::has_key(source, "vAng")
      && !json_util::is_null(source["vAng"])) {
    texture->vAng = json_util::get_number<float>(source, "vAng");
  }
  // vOffset
  if (json_util::has_key(source, "vOffset")
      && !json_util::is_null(source["vOffset"])) {
    texture->vOffset = json_util::get_number<float>(source, "vOffset");
  }
  // vScale
  if (json_util::has_key(source, "vScale")
      && !json_util::is_null(source["vScale"])) {
    texture->vScale = json_util::get_number<float>(source, "vScale");
  }
  // wAng
  if (json_util::has_key(source, "wAng")
      && !json_util::is_null(source["wAng"])) {
    texture->wAng = json_util::get_number<float>(source, "wAng");
  }
  // wrapU
  if (json_util::has_key(source, "wrapU")
      && !json_util::is_null(source["wrapU"])) {
    texture->wrapU = json_util::get_number<unsigned>(source, "wrapU");
  }
  // wrapV
  if (json_util::has_key(source, "wrapV")
      && !json_util::is_null(source["wrapV"])) {
    texture->wrapV = json_util::get_number<unsigned>(source, "wrapV");
  }

  return texture;
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
    if (json_util::has_key(source, "applyGravity")
        && !json_util::is_null(source["applyGravity"])) {
      freeCamera->applyGravity = json_util::get_bool(source, "applyGravity");
    }
    // cameraRigMode
    if (json_util::has_key(source, "cameraRigMode")
        && !json_util::is_null(source["cameraRigMode"])) {
      freeCamera->cameraRigMode
        = json_util::get_number<unsigned>(source, "cameraRigMode");
    }
    // checkCollisions
    if (json_util::has_key(source, "checkCollisions")
        && !json_util::is_null(source["checkCollisions"])) {
      freeCamera->checkCollisions
        = json_util::get_bool(source, "checkCollisions");
    }
    // ellipsoid
    if (json_util::has_key(source, "ellipsoid")
        && !json_util::is_null(source["ellipsoid"])) {
      freeCamera->ellipsoid
        = Vector3::FromArray(json_util::get_array<float>(source, "ellipsoid"));
    }
    // fov
    if (json_util::has_key(source, "fov")
        && !json_util::is_null(source["fov"])) {
      freeCamera->fov = json_util::get_number<float>(source, "fov");
    }
    // id
    if (json_util::has_key(source, "id") && !json_util::is_null(source["id"])) {
      freeCamera->id = json_util::get_string(source, "id");
    }
    // inertia
    if (json_util::has_key(source, "inertia")
        && !json_util::is_null(source["inertia"])) {
      freeCamera->inertia = json_util::get_number<float>(source, "inertia");
    }
    // maxZ
    if (json_util::has_key(source, "maxZ")
        && !json_util::is_null(source["maxZ"])) {
      freeCamera->maxZ = json_util::get_number<float>(source, "maxZ");
    }
    // minZ
    if (json_util::has_key(source, "minZ")
        && !json_util::is_null(source["minZ"])) {
      freeCamera->minZ = json_util::get_number<float>(source, "minZ");
    }
    // position
    if (json_util::has_key(source, "position")
        && !json_util::is_null(source["position"])) {
      freeCamera->position
        = Vector3::FromArray(json_util::get_array<float>(source, "position"));
    }
    // rotation
    if (json_util::has_key(source, "rotation")
        && !json_util::is_null(source["rotation"])) {
      freeCamera->rotation = std::make_unique<Vector3>(
        Vector3::FromArray(json_util::get_array<float>(source, "rotation")));
    }
    // speed
    if (json_util::has_key(source, "speed")
        && !json_util::is_null(source["speed"])) {
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
      // diffuse
      if (json_util::has_key(source, "diffuse")
          && !json_util::is_null(source["diffuse"])) {
        pointLight->diffuse
          = Color3::FromArray(json_util::get_array<float>(source, "diffuse"));
      }
      // direction
      if (json_util::has_key(source, "direction")
          && !json_util::is_null(source["direction"])) {
        pointLight->direction = Vector3::FromArray(
          json_util::get_array<float>(source, "direction"));
      }
      // id
      if (json_util::has_key(source, "id")
          && !json_util::is_null(source["id"])) {
        pointLight->id = json_util::get_string(source, "id");
      }
      // intensity
      if (json_util::has_key(source, "intensity")
          && !json_util::is_null(source["intensity"])) {
        pointLight->intensity = json_util::get_number(source, "direction", 1.f);
      }
      // position
      if (json_util::has_key(source, "position")
          && !json_util::is_null(source["position"])) {
        pointLight->position
          = Vector3::FromArray(json_util::get_array<float>(source, "position"));
      }
      // specular
      if (json_util::has_key(source, "specular")
          && !json_util::is_null(source["specular"])) {
        pointLight->specular
          = Color3::FromArray(json_util::get_array<float>(source, "specular"));
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
      // direction
      if (json_util::has_key(source, "direction")
          && !json_util::is_null(source["direction"])) {
        hemisphericLight->direction = Vector3::FromArray(
          json_util::get_array<float>(source, "direction"));
      }
      // groundColor
      if (json_util::has_key(source, "groundColor")) {
        hemisphericLight->groundColor = Color3::FromArray(
          json_util::get_array<float>(source, "groundColor"));
      }
    } break;
    default:
      break;
  }

  return light;
}

StandardMaterialPtr SerializationHelper::Parse(
  const std::function<StandardMaterialPtr()>& creationFunction,
  const json& source, Scene* scene, const std::string& rootUrl)
{
  auto standardMaterial = creationFunction();

  if (!standardMaterial) {
    return nullptr;
  }

  // alpha
  if (json_util::has_key(source, "alpha")
      && !json_util::is_null(source["alpha"])) {
    standardMaterial->alpha = json_util::get_number<float>(source, "alpha");
  }
  // ambient
  if (json_util::has_key(source, "ambient")
      && !json_util::is_null(source["ambient"])) {
    standardMaterial->ambientColor
      = Color3::FromArray(json_util::get_array<float>(source, "ambient"));
  }
  // ambientTexture
  if (json_util::has_key(source, "ambientTexture")
      && !json_util::is_null(source["ambientTexture"])) {
    standardMaterial->ambientTexture
      = Texture::Parse(source["ambientTexture"], scene, rootUrl);
  }
  // backFaceCulling
  if (json_util::has_key(source, "backFaceCulling")
      && !json_util::is_null(source["backFaceCulling"])) {
    standardMaterial->backFaceCulling
      = json_util::get_bool(source, "backFaceCulling");
  }
  // checkReadyOnlyOnce
  if (json_util::has_key(source, "checkReadyOnlyOnce")
      && !json_util::is_null(source["checkReadyOnlyOnce"])) {
    standardMaterial->checkReadyOnlyOnce
      = json_util::get_bool(source, "checkReadyOnlyOnce");
  }
  // diffuse
  if (json_util::has_key(source, "diffuse")
      && !json_util::is_null(source["diffuse"])) {
    standardMaterial->diffuseColor
      = Color3::FromArray(json_util::get_array<float>(source, "diffuse"));
  }
  // diffuseTexture
  if (json_util::has_key(source, "diffuseTexture")
      && !json_util::is_null(source["diffuseTexture"])) {
    standardMaterial->diffuseTexture
      = Texture::Parse(source["diffuseTexture"], scene, rootUrl);
  }
  // disableLighting
  if (json_util::has_key(source, "disableLighting")
      && !json_util::is_null(source["disableLighting"])) {
    standardMaterial->disableLighting
      = json_util::get_bool(source, "disableLighting");
  }
  // emissive
  if (json_util::has_key(source, "emissive")
      && !json_util::is_null(source["emissive"])) {
    standardMaterial->emissiveColor
      = Color3::FromArray(json_util::get_array<float>(source, "emissive"));
  }
  // emissiveTexture
  if (json_util::has_key(source, "emissiveTexture")
      && !json_util::is_null(source["emissiveTexture"])) {
    standardMaterial->emissiveTexture
      = Texture::Parse(source["emissiveTexture"], scene, rootUrl);
  }
  // id
  if (json_util::has_key(source, "id") && !json_util::is_null(source["id"])) {
    standardMaterial->id = json_util::get_string(source, "id");
  }
  // indexOfRefraction
  if (json_util::has_key(source, "indexOfRefraction")
      && !json_util::is_null(source["indexOfRefraction"])) {
    standardMaterial->indexOfRefraction
      = json_util::get_number<float>(source, "indexOfRefraction", 0.98f);
  }
  // invertNormalMapX
  if (json_util::has_key(source, "invertNormalMapX")
      && !json_util::is_null(source["invertNormalMapX"])) {
    standardMaterial->invertNormalMapX
      = json_util::get_bool(source, "invertNormalMapX");
  }
  // invertNormalMapY
  if (json_util::has_key(source, "invertNormalMapY")
      && !json_util::is_null(source["invertNormalMapY"])) {
    standardMaterial->invertNormalMapY
      = json_util::get_bool(source, "invertNormalMapY");
  }
  // invertRefractionY
  if (json_util::has_key(source, "invertRefractionY")
      && !json_util::is_null(source["invertRefractionY"])) {
    standardMaterial->invertRefractionY
      = json_util::get_bool(source, "invertRefractionY", true);
  }
  // linkEmissiveWithDiffuse
  if (json_util::has_key(source, "linkEmissiveWithDiffuse")
      && !json_util::is_null(source["linkEmissiveWithDiffuse"])) {
    standardMaterial->linkEmissiveWithDiffuse
      = json_util::get_bool(source, "linkEmissiveWithDiffuse");
  }
  // maxSimultaneousLights
  if (json_util::has_key(source, "maxSimultaneousLights")
      && !json_util::is_null(source["maxSimultaneousLights"])) {
    standardMaterial->maxSimultaneousLights
      = json_util::get_number<unsigned>(source, "maxSimultaneousLights", 4);
  }
  // opacityTexture
  if (json_util::has_key(source, "opacityTexture")
      && !json_util::is_null(source["opacityTexture"])) {
    standardMaterial->opacityTexture
      = Texture::Parse(source["opacityTexture"], scene, rootUrl);
  }
  // parallaxScaleBias
  if (json_util::has_key(source, "parallaxScaleBias")
      && !json_util::is_null(source["parallaxScaleBias"])) {
    standardMaterial->parallaxScaleBias
      = json_util::get_number<float>(source, "parallaxScaleBias", 0.5f);
  }
  // reflectionTexture
  if (json_util::has_key(source, "reflectionTexture")
      && !json_util::is_null(source["reflectionTexture"])) {
    standardMaterial->reflectionTexture
      = Texture::Parse(source["reflectionTexture"], scene, rootUrl);
  }
  // roughness
  if (json_util::has_key(source, "roughness")
      && !json_util::is_null(source["roughness"])) {
    standardMaterial->roughness
      = json_util::get_number<float>(source, "roughness", 0.f);
  }
  // specular
  if (json_util::has_key(source, "specular")
      && !json_util::is_null(source["specular"])) {
    standardMaterial->specularColor
      = Color3::FromArray(json_util::get_array<float>(source, "specular"));
  }
  // specularPower
  if (json_util::has_key(source, "specularPower")
      && !json_util::is_null(source["specularPower"])) {
    standardMaterial->specularPower
      = json_util::get_number<float>(source, "specularPower", 64.f);
  }
  // specularTexture
  if (json_util::has_key(source, "specularTexture")
      && !json_util::is_null(source["specularTexture"])) {
    standardMaterial->reflectionTexture
      = Texture::Parse(source["specularTexture"], scene, rootUrl);
  }
  // useAlphaFromDiffuseTexture
  if (json_util::has_key(source, "useAlphaFromDiffuseTexture")
      && !json_util::is_null(source["useAlphaFromDiffuseTexture"])) {
    standardMaterial->useAlphaFromDiffuseTexture
      = json_util::get_bool(source, "useAlphaFromDiffuseTexture");
  }
  // useEmissiveAsIllumination
  if (json_util::has_key(source, "useEmissiveAsIllumination")
      && !json_util::is_null(source["useEmissiveAsIllumination"])) {
    standardMaterial->useEmissiveAsIllumination
      = json_util::get_bool(source, "useEmissiveAsIllumination");
  }
  // useGlossinessFromSpecularMapAlpha
  if (json_util::has_key(source, "useGlossinessFromSpecularMapAlpha")
      && !json_util::is_null(source["useGlossinessFromSpecularMapAlpha"])) {
    standardMaterial->useGlossinessFromSpecularMapAlpha
      = json_util::get_bool(source, "useGlossinessFromSpecularMapAlpha");
  }
  // useLightmapAsShadowmap
  if (json_util::has_key(source, "useLightmapAsShadowmap")
      && !json_util::is_null(source["useLightmapAsShadowmap"])) {
    standardMaterial->useLightmapAsShadowmap
      = json_util::get_bool(source, "useLightmapAsShadowmap", true);
  }
  // useParallax
  if (json_util::has_key(source, "useParallax")
      && !json_util::is_null(source["useParallax"])) {
    standardMaterial->useParallax = json_util::get_bool(source, "useParallax");
  }
  // useParallaxOcclusion
  if (json_util::has_key(source, "useParallaxOcclusion")
      && !json_util::is_null(source["useParallaxOcclusion"])) {
    standardMaterial->useParallaxOcclusion
      = json_util::get_bool(source, "useParallaxOcclusion");
  }
  // useReflectionFresnelFromSpecular
  if (json_util::has_key(source, "useReflectionFresnelFromSpecular")
      && !json_util::is_null(source["useReflectionFresnelFromSpecular"])) {
    standardMaterial->useReflectionFresnelFromSpecular
      = json_util::get_bool(source, "useReflectionFresnelFromSpecular");
  }
  // useReflectionOverAlpha
  if (json_util::has_key(source, "useReflectionOverAlpha")
      && !json_util::is_null(source["useReflectionOverAlpha"])) {
    standardMaterial->useReflectionOverAlpha
      = json_util::get_bool(source, "useReflectionOverAlpha");
  }
  // useSpecularOverAlpha
  if (json_util::has_key(source, "useSpecularOverAlpha")
      && !json_util::is_null(source["useSpecularOverAlpha"])) {
    standardMaterial->useSpecularOverAlpha
      = json_util::get_bool(source, "useSpecularOverAlpha");
  }

  return standardMaterial;
}

} // end of namespace BABYLON
