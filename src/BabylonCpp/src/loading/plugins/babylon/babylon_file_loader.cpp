#include <babylon/loading/plugins/babylon/babylon_file_loader.h>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/animation.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>
#include <babylon/engine/scene.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/geometry_primitives.h>
#include <babylon/mesh/mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

BabylonFileLoader::BabylonFileLoader()
{
  extensions = ".babylon";
}

BabylonFileLoader::~BabylonFileLoader()
{
}

Material* BabylonFileLoader::parseMaterialById(const std::string& id,
                                               const Json::value& parsedData,
                                               Scene* scene,
                                               const std::string& rootUrl) const
{
  if (parsedData.contains("materials")
      && (parsedData.get("materials").is<Json::array>())) {
    for (const auto& parsedMaterial :
         parsedData.get("materials").get<Json::array>()) {
      if (parsedMaterial.contains("id")) {
        const std::string parsedMaterialId
          = parsedMaterial.get("id").get<std::string>();
        if (parsedMaterialId == id) {
          return Material::Parse(parsedMaterial, scene, rootUrl);
        }
      }
    }
  }
  return nullptr;
}

bool BabylonFileLoader::isDescendantOf(const Json::value& mesh,
                                       const std::vector<std::string>& names,
                                       std::vector<std::string>& hierarchyIds)
{
  const std::string meshName
    = mesh.contains("name") ? mesh.get("name").get<std::string>() : "";
  for (auto& name : names) {
    if (meshName == name) {
      hierarchyIds.emplace_back(mesh.get("id").get<std::string>());
      return true;
    }
  }
  if (mesh.contains("parentId")) {
    const std::string meshParentId = mesh.get("parentId").get<std::string>();
    if (std_util::contains(hierarchyIds, meshParentId)) {
      hierarchyIds.emplace_back(mesh.get("id").get<std::string>());
      return true;
    }
  }
  return false;
}

std::string BabylonFileLoader::logOperation(const std::string& operation) const
{
  return operation + " of unknown";
}

std::string BabylonFileLoader::logOperation(const std::string& operation,
                                            const Json::value& producer) const
{
  return operation + " of " + producer.get("file").get<std::string>() + " from "
         + producer.get("name").get<std::string>() + " version: "
         + producer.get("version").get<std::string>() + ", exporter version: "
         + producer.get("exporter_version").get<std::string>();
}

bool BabylonFileLoader::importMesh(
  const std::vector<std::string>& meshesNames, Scene* scene,
  const std::string& data, const std::string& rootUrl,
  std::vector<AbstractMesh*>& meshes,
  std::vector<ParticleSystem*>& particleSystems,
  std::vector<Skeleton*>& skeletons)
{
  std::string log = "importMesh has failed JSON parse";
  Json::value parsedData;
  const char* _data = data.c_str();
  std::string err;
  err = picojson::parse(parsedData, _data, _data + strlen(_data));
  if (!err.empty()) {
    BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", log.c_str());
    return false;
  }
  std::ostringstream logStream;

  bool fullDetails = SceneLoader::LoggingLevel == SceneLoader::DETAILED_LOGGING;

  std::vector<std::string> loadedSkeletonsIds;
  std::vector<std::string> loadedMaterialsIds;
  std::vector<std::string> hierarchyIds;

  Json::array parsedMeshes = parsedData.get("meshes").get<Json::array>();
  for (auto& parsedMesh : parsedMeshes) {
    if (meshesNames.empty()
        || isDescendantOf(parsedMesh, meshesNames, hierarchyIds)) {

      // Id
      const std::string parsedMeshId = Json::GetString(parsedMesh, "id", "");

      // Geometry ?
      if (parsedMesh.contains("geometryId")) {
        const std::string parsedMeshGeometryId
          = Json::GetString(parsedMesh, "geometryId", "");
        // Does the file contain geometries?
        if (parsedData.contains("geometries")) {
          // find the correct geometry and add it to the scene
          bool found = false;
          const std::array<std::string, 8> geometryTypes{
            {"boxes", "spheres", "cylinders", "toruses", "grounds", "planes",
             "torusKnots", "vertexData"}};
          for (const auto& geometryType : geometryTypes) {
            if (found || !parsedData.get("geometries").contains(geometryType)
                || !(parsedData.get("geometries")
                       .get(geometryType)
                       .is<Json::array>())) {
              break;
            }
            else {
              for (const auto& parsedGeometryData : parsedData.get("geometries")
                                                      .get(geometryType)
                                                      .get<Json::array>()) {
                const std::string parsedGeometryDataId
                  = Json::GetString(parsedGeometryData, "id", "");
                if ((!parsedGeometryDataId.empty())
                    && (parsedGeometryDataId == parsedMeshGeometryId)) {
                  if (geometryType == "boxes") {
                    GeometryPrimitives::Box::Parse(parsedGeometryData, scene);
                  }
                  else if (geometryType == "spheres") {
                    GeometryPrimitives::Sphere::Parse(parsedGeometryData,
                                                      scene);
                  }
                  else if (geometryType == "cylinders") {
                    GeometryPrimitives::Cylinder::Parse(parsedGeometryData,
                                                        scene);
                  }
                  else if (geometryType == "toruses") {
                    GeometryPrimitives::Torus::Parse(parsedGeometryData, scene);
                  }
                  else if (geometryType == "grounds") {
                    GeometryPrimitives::Ground::Parse(parsedGeometryData,
                                                      scene);
                  }
                  else if (geometryType == "planes") {
                    GeometryPrimitives::Plane::Parse(parsedGeometryData, scene);
                  }
                  else if (geometryType == "torusKnots") {
                    GeometryPrimitives::TorusKnot::Parse(parsedGeometryData,
                                                         scene);
                  }
                  else if (geometryType == "vertexData") {
                    Geometry::Parse(parsedGeometryData, scene, rootUrl);
                  }
                  found = true;
                }
              }
            }
          }
          if (!found) {
            BABYLON_LOGF_WARN("BabylonFileLoader",
                              "Geometry not found for mesh %s",
                              parsedMeshId.c_str());
          }
        }
      }

      // Material ?
      if (parsedMesh.contains("materialId")) {
        const std::string parsedMeshMaterialId
          = Json::GetString(parsedMesh, "materialId", "");
        bool materialFound
          = std_util::contains(loadedMaterialsIds, parsedMeshMaterialId);
        if (!parsedMeshMaterialId.empty() && !materialFound
            && parsedData.contains("multiMaterials")
            && parsedData.get("multiMaterials").is<Json::array>()) {
          for (const auto& parsedMultiMaterial :
               parsedData.get("multiMaterials").get<Json::array>()) {
            const std::string parsedMultiMaterialId
              = Json::GetString(parsedMultiMaterial, "id", "");
            if ((!parsedMultiMaterialId.empty())
                && (parsedMultiMaterialId == parsedMeshMaterialId)) {
              if (parsedMultiMaterial.contains("materials")
                  && parsedMultiMaterial.get("materials").is<Json::array>()) {
                for (const auto& subMatId :
                     parsedMultiMaterial.get("materials").get<Json::array>()) {
                  loadedMaterialsIds.emplace_back(subMatId.get<std::string>());
                  Material* mat = parseMaterialById(subMatId.get<std::string>(),
                                                    parsedData, scene, rootUrl);
                  logStream << "\n\tMaterial " << mat->toString(fullDetails);
                }
              }
              loadedMaterialsIds.emplace_back(parsedMultiMaterialId);
              Material* mmat
                = Material::ParseMultiMaterial(parsedMultiMaterial, scene);
              materialFound = true;
              logStream << "\n\tMulti-Material " << mmat->toString(fullDetails);
              break;
            }
          }
        }

        if (!materialFound) {
          loadedMaterialsIds.emplace_back(parsedMeshMaterialId);
          Material* mat = parseMaterialById(parsedMeshMaterialId, parsedData,
                                            scene, rootUrl);
          if (!mat) {
            BABYLON_LOGF_WARN("BabylonFileLoader",
                              "Material not found for mesh %s",
                              parsedMeshId.c_str());
          }
          else {
            logStream << "\n\tMaterial " << mat->toString(fullDetails);
          }
        }
      }

      // Skeleton ?
      if (parsedMesh.contains("skeletonId")) {
        const std::string parsedMeshSkeletonId
          = Json::GetString(parsedMesh, "skeletonId", "");
        bool skeletonAlreadyLoaded
          = std_util::contains(loadedSkeletonsIds, parsedMeshSkeletonId);
        if (!parsedMeshSkeletonId.empty() && !skeletonAlreadyLoaded
            && parsedData.contains("skeletons")
            && parsedData.get("skeletons").is<Json::array>()) {
          for (const auto& parsedSkeleton :
               parsedData.get("skeletons").get<Json::array>()) {
            const std::string parsedSkeletonId
              = Json::GetString(parsedSkeleton, "id", "");
            if ((!parsedSkeletonId.empty())
                && (parsedSkeletonId == parsedMeshSkeletonId)) {
              Skeleton* skeleton = Skeleton::Parse(parsedSkeleton, scene);
              skeletons.emplace_back(skeleton);
              loadedSkeletonsIds.emplace_back(parsedSkeletonId);
              logStream << "\n\tSkeleton " << skeleton->toString(fullDetails);
            }
          }
        }
      }

      Mesh* mesh = Mesh::Parse(parsedMesh, scene, rootUrl);
      meshes.emplace_back(mesh);
      log += "\n\tMesh " + mesh->toString(fullDetails);
    }
  }

  // Connecting parents
  for (auto& currentMesh : scene->meshes) {
    if (!currentMesh->_waitingParentId.empty()) {
      currentMesh->setParent(
        scene->getLastEntryByID(currentMesh->_waitingParentId));
      currentMesh->_waitingParentId = "";
    }
  }

  // freeze and compute world matrix application
  for (auto& currentMesh : scene->meshes) {
    if (currentMesh->_waitingFreezeWorldMatrix) {
      currentMesh->freezeWorldMatrix();
      currentMesh->_waitingFreezeWorldMatrix = false;
    }
    else {
      currentMesh->computeWorldMatrix(true);
    }
  }

  // Particles ?
  if (parsedData.contains("particleSystems")
      && parsedData.get("particleSystems").is<Json::array>()) {
    for (const auto& parsedParticleSystem :
         parsedData.get("particleSystems").get<Json::array>()) {
      const std::string parsedParticleSystemEmitterId
        = Json::GetString(parsedParticleSystem, "emitterId", "");
      if (!parsedParticleSystemEmitterId.empty()
          && std_util::contains(hierarchyIds, parsedParticleSystemEmitterId)) {
        particleSystems.emplace_back(
          ParticleSystem::Parse(parsedParticleSystem, scene, rootUrl));
      }
    }
  }

  log = logStream.str();
  if (!log.empty() && SceneLoader::LoggingLevel != SceneLoader::NO_LOGGING) {
    std::string msg = parsedData.contains("producer") ?
                        logOperation("importMesh", parsedData.get("producer")) :
                        logOperation("importMesh");
    std::string logStr
      = SceneLoader::LoggingLevel != SceneLoader::MINIMAL_LOGGING ? log : "";
    BABYLON_LOGF_INFO("BabylonFileLoader", "%s%s", msg.c_str(), logStr.c_str());
  }

  return true;
}

bool BabylonFileLoader::load(Scene* scene, const std::string& data,
                             const std::string& rootUrl)
{
  std::string parseLog = "importScene has failed JSON parse";
  Json::value parsedData;
  const char* _data = data.c_str();
  std::string err;
  err = picojson::parse(parsedData, _data, _data + strlen(_data));
  if (!err.empty()) {
    BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", parseLog.c_str());
    return false;
  }
  std::ostringstream log;

  bool fullDetails = SceneLoader::LoggingLevel == SceneLoader::DETAILED_LOGGING;

  // Scene
  scene->useDelayedTextureLoading
    = Json::GetBool(parsedData, "useDelayedTextureLoading", false)
      && !SceneLoader::ForceFullSceneLoadingForIncremental;
  scene->autoClear = Json::GetBool(parsedData, "autoClear", true);
  scene->clearColor
    = Color3::FromArray(Json::ToArray<float>(parsedData, "clearColor"));
  scene->ambientColor
    = Color3::FromArray(Json::ToArray<float>(parsedData, "ambientColor"));
  if (parsedData.contains("gravity")) {
    scene->gravity
      = Vector3::FromArray(Json::ToArray<float>(parsedData, "gravity"));
  }

  // Fog
  unsigned int fogMode
    = Json::GetNumber(parsedData, "fogMode", Scene::FOGMODE_NONE);
  if (fogMode != Scene::FOGMODE_NONE) {
    scene->fogMode = fogMode;
    scene->fogColor
      = Color3::FromArray(Json::ToArray<float>(parsedData, "fogColor"));
    scene->fogStart   = Json::GetNumber(parsedData, "fogStart", 0.f);
    scene->fogEnd     = Json::GetNumber(parsedData, "fogEnd", 1000.f);
    scene->fogDensity = Json::GetNumber(parsedData, "fogDensity", 0.1f);
    log << "\tFog mode for scene:  ";
    switch (fogMode) {
      case Scene::FOGMODE_EXP:
        log << "exp\n";
        break;
      case Scene::FOGMODE_EXP2:
        log << "exp2\n";
        break;
      case Scene::FOGMODE_LINEAR:
        log << "linear\n";
        break;
      default:
        log << "unknown\n";
    }
  }

  // Physics
  bool physicsEnabled = Json::GetBool(parsedData, "physicsEnabled", false);
  if (physicsEnabled) {
  }

  // Collisions, if defined. otherwise, default is true
  scene->collisionsEnabled = Json::GetBool(parsedData, "physicsEnabled", true);
  scene->setWorkerCollisions(
    Json::GetBool(parsedData, "workerCollisions", false));

  // Lights
  unsigned int index = 0;
  if (parsedData.contains("lights")
      && (parsedData.get("lights").is<Json::array>())) {
    for (const auto& parsedLight :
         parsedData.get("lights").get<Json::array>()) {
      Light* light = Light::Parse(parsedLight, scene);
      log << (index == 0 ? "\n\tLights:" : "");
      log << "\n\t\t" << light->toString(fullDetails);
      ++index;
    }
  }

  // Animations
  /*index = 0;
  if (parsedData.contains("animations")
      && (parsedData.get("animations").is<Json::array>())) {
    for (const auto& parsedAnimation :
         parsedData.get("animations").get<Json::array>()) {
      Animation* animation = Animation::Parse(parsedAnimation);
      scene->animations.emplace_back(animation);
      log << (index == 0 ? "\n\tAnimations:" : "");
      log << "\n\t\t" << animation->toString(fullDetails);
      ++index;
    }
  }

  if (Json::GetBool(parsedData, "autoAnimate", false)) {
    scene->beginAnimation(scene,
                          Tools::GetNumber(parsedData, "autoAnimateFrom", 0),
                          Tools::GetNumber(parsedData, "autoAnimateTo", 0),
                          Json::GetBool(parsedData, "autoAnimateLoop", false),
                          Tools::GetNumber(parsedData, "autoAnimateFrom", 1.f));
  }*/

  // Materials
  index = 0;
  if (parsedData.contains("materials")
      && (parsedData.get("materials").is<Json::array>())) {
    for (const auto& parsedMaterial :
         parsedData.get("materials").get<Json::array>()) {
      Material* mat = Material::Parse(parsedMaterial, scene, rootUrl);
      log << (index == 0 ? "\n\tMaterials:" : "");
      log << "\n\t\t" << mat->toString(fullDetails);
      ++index;
    }
  }

  // Multi materials
  index = 0;
  if (parsedData.contains("multiMaterials")
      && (parsedData.get("multiMaterials").is<Json::array>())) {
    for (const auto& parsedMultiMaterial :
         parsedData.get("multiMaterials").get<Json::array>()) {
      MultiMaterial* mmat
        = Material::ParseMultiMaterial(parsedMultiMaterial, scene);
      log << (index == 0 ? "\n\tMultiMaterials:" : "");
      log << "\n\t\t" << mmat->toString(fullDetails);
      ++index;
    }
  }

  // Skeletons
  index = 0;
  if (parsedData.contains("skeletons")
      && (parsedData.get("skeletons").is<Json::array>())) {
    for (const auto& parsedSkeleton :
         parsedData.get("skeletons").get<Json::array>()) {
      Skeleton* skeleton = Skeleton::Parse(parsedSkeleton, scene);
      log << (index == 0 ? "\n\tSkeletons:" : "");
      log << "\n\t\t" << skeleton->toString(fullDetails);
      ++index;
    }
  }

  if (parsedData.contains("geometries")) {
    const Json::value& geometries = parsedData.get("geometries");

    // Boxes
    if (geometries.contains("boxes")
        && (geometries.get("boxes").is<Json::array>())) {
      for (const auto& parsedBox : geometries.get("boxes").get<Json::array>()) {
        GeometryPrimitives::Box::Parse(parsedBox, scene);
      }
    }

    // Spheres
    if (geometries.contains("spheres")
        && (geometries.get("spheres").is<Json::array>())) {
      for (const auto& parsedSphere :
           geometries.get("spheres").get<Json::array>()) {
        GeometryPrimitives::Sphere::Parse(parsedSphere, scene);
      }
    }

    // Cylinders
    if (geometries.contains("cylinders")
        && (geometries.get("cylinders").is<Json::array>())) {
      for (const auto& parsedCylinder :
           geometries.get("cylinders").get<Json::array>()) {
        GeometryPrimitives::Cylinder::Parse(parsedCylinder, scene);
      }
    }

    // Toruses
    if (geometries.contains("toruses")
        && (geometries.get("toruses").is<Json::array>())) {
      for (const auto& parsedTorus :
           geometries.get("toruses").get<Json::array>()) {
        GeometryPrimitives::Torus::Parse(parsedTorus, scene);
      }
    }

    // Grounds
    if (geometries.contains("grounds")
        && (geometries.get("grounds").is<Json::array>())) {
      for (const auto& parsedGround :
           geometries.get("grounds").get<Json::array>()) {
        GeometryPrimitives::Ground::Parse(parsedGround, scene);
      }
    }

    // Planes
    if (geometries.contains("planes")
        && (geometries.get("planes").is<Json::array>())) {
      for (const auto& parsedPlane :
           geometries.get("planes").get<Json::array>()) {
        GeometryPrimitives::Plane::Parse(parsedPlane, scene);
      }
    }

    // TorusKnots
    if (geometries.contains("torusKnots")
        && (geometries.get("torusKnots").is<Json::array>())) {
      for (const auto& parsedTorusKnot :
           geometries.get("torusKnots").get<Json::array>()) {
        GeometryPrimitives::TorusKnot::Parse(parsedTorusKnot, scene);
      }
    }

    // VertexData
    if (geometries.contains("vertexData")
        && (geometries.get("vertexData").is<Json::array>())) {
      for (const auto& parsedVertexData :
           geometries.get("vertexData").get<Json::array>()) {
        Geometry::Parse(parsedVertexData, scene, rootUrl);
      }
    }
  }

  // Meshes
  index = 0;
  if (parsedData.contains("meshes")
      && (parsedData.get("meshes").is<Json::array>())) {
    for (const auto& parsedMesh : parsedData.get("meshes").get<Json::array>()) {
      Mesh* mesh = Mesh::Parse(parsedMesh, scene, rootUrl);
      log << (index == 0 ? "\n\tMeshes:" : "");
      log << "\n\t\t" << mesh->toString(fullDetails);
      ++index;
    }
  }

  // Cameras
  index = 0;
  if (parsedData.contains("cameras")
      && (parsedData.get("cameras").is<Json::array>())) {
    for (const auto& parsedCamera :
         parsedData.get("cameras").get<Json::array>()) {
      Camera* camera = Camera::Parse(parsedCamera, scene);
      log << (index == 0 ? "\n\tCameras:" : "");
      log << "\n\t\t" << camera->toString(fullDetails);
      ++index;
    }
  }
  if (parsedData.contains("activeCameraID")) {
    scene->setActiveCameraByID(
      Json::GetString(parsedData, "activeCameraID", ""));
  }

  // Browsing all the graph to connect the dots
  for (auto& camera : scene->cameras) {
    if (!camera->_waitingParentId.empty()) {
      camera->setParent(scene->getLastEntryByID(camera->_waitingParentId));
      camera->_waitingParentId.clear();
    }
  }

  for (auto& light : scene->lights) {
    if (!light->_waitingParentId.empty()) {
      light->setParent(scene->getLastEntryByID(light->_waitingParentId));
      light->_waitingParentId.clear();
    }
  }

  // Sounds

  // Connect parents & children and parse actions
  for (auto& mesh : scene->meshes) {
    if (!mesh->_waitingParentId.empty()) {
      mesh->setParent(scene->getLastEntryByID(mesh->_waitingParentId));
      mesh->_waitingParentId.clear();
    }
    if (!mesh->_waitingActions.empty()) {
      ActionManager::Parse(mesh->_waitingActions, mesh.get(), scene);
      mesh->_waitingActions.clear();
    }
  }

  // Freeze world matrix application
  for (auto& currentMesh : scene->meshes) {
    if (currentMesh->_waitingFreezeWorldMatrix) {
      currentMesh->freezeWorldMatrix();
      currentMesh->_waitingFreezeWorldMatrix = false;
    }
    else {
      currentMesh->computeWorldMatrix(true);
    }
  }

  // Particles Systems
  if (parsedData.contains("particleSystems")
      && (parsedData.get("particleSystems").is<Json::array>())) {
    for (const auto& parsedParticleSystem :
         parsedData.get("particleSystems").get<Json::array>()) {
      ParticleSystem::Parse(parsedParticleSystem, scene, rootUrl);
    }
  }

  // Lens flares
  if (parsedData.contains("lensFlareSystems")
      && (parsedData.get("lensFlareSystems").is<Json::array>())) {
    for (const auto& parsedLensFlareSystem :
         parsedData.get("lensFlareSystems").get<Json::array>()) {
      LensFlareSystem::Parse(parsedLensFlareSystem, scene, rootUrl);
    }
  }

  // Shadows
  if (parsedData.contains("shadowGenerators")
      && (parsedData.get("shadowGenerators").is<Json::array>())) {
    for (const auto& parsedShadowGenerator :
         parsedData.get("shadowGenerators").get<Json::array>()) {
      ShadowGenerator::Parse(parsedShadowGenerator, scene);
    }
  }

  // Actions (scene)
  if (parsedData.contains("actions")
      && (parsedData.get("actions").is<Json::array>())) {
    ActionManager::Parse(parsedData.get("actions").get<Json::array>(), nullptr,
                         scene);
  }

  const std::string _log = log.str();
  if (!_log.empty() && SceneLoader::LoggingLevel != SceneLoader::NO_LOGGING) {
    std::string msg
      = parsedData.contains("producer") ?
          logOperation("importScene", parsedData.get("producer")) :
          logOperation("importScene");
    std::string logStr
      = SceneLoader::LoggingLevel != SceneLoader::MINIMAL_LOGGING ? _log : "";
    BABYLON_LOGF_INFO("BabylonFileLoader", "%s%s", msg.c_str(), logStr.c_str());
  }

  // Finish
  return true;
}

} // end of namespace BABYLON
