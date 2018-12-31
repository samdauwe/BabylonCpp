#include <babylon/loading/plugins/babylon/babylon_file_loader.h>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_group.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/asset_container.h>
#include <babylon/engine/scene.h>
#include <babylon/engine/scene_component_constants.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/mesh/geometry.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/primitive_geometries.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

BabylonFileLoader::BabylonFileLoader()
{
  name = "babylon.js";
  extensions.mapping.emplace(std::make_pair(".babylon", false));
  canDirectLoad = [](const std::string& data) {
    // We consider that the producer string is filled
    if (String::endsWith(data, "babylon")) {
      return true;
    }

    return false;
  };
}

BabylonFileLoader::~BabylonFileLoader()
{
}

MaterialPtr
BabylonFileLoader::parseMaterialById(const std::string& id,
                                     const json& parsedData, Scene* scene,
                                     const std::string& rootUrl) const
{
  for (const auto& parsedMaterial :
       json_util::get_array<json>(parsedData, "materials")) {
    if (json_util::get_string(parsedData, "id") == id) {
      return Material::Parse(parsedMaterial, scene, rootUrl);
    }
  }
  return nullptr;
}

bool BabylonFileLoader::isDescendantOf(
  const json& mesh, const std::vector<std::string>& names,
  std::vector<std::string>& hierarchyIds) const
{
  for (const auto& name : names) {
    if (json_util::get_string(mesh, "name") == name) {
      hierarchyIds.emplace_back(json_util::get_string(mesh, "id"));
      return true;
    }
  }
  if (json_util::has_key(mesh, "parentId")
      && stl_util::contains(hierarchyIds,
                            json_util::get_string(mesh, "parentId"))) {
    hierarchyIds.emplace_back(json_util::get_string(mesh, "id"));
    return true;
  }
  return false;
}

std::string BabylonFileLoader::logOperation(const std::string& operation) const
{
  return operation + " of unknown";
}

std::string BabylonFileLoader::logOperation(const std::string& operation,
                                            const json& producer) const
{
  return operation + " of " + json_util::get_string(producer, "file") + " from "
         + json_util::get_string(producer, "name") + " version: "
         + json_util::get_string(producer, "version") + ", exporter version: "
         + json_util::get_string(producer, "exporter_version");
}

bool BabylonFileLoader::importMesh(
  const std::vector<std::string>& meshesNames, Scene* scene,
  const std::string& data, const std::string& rootUrl,
  std::vector<AbstractMeshPtr>& meshes,
  std::vector<IParticleSystemPtr>& /*particleSystems*/,
  std::vector<SkeletonPtr>& skeletons,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError) const
{
  // Entire method running in try block, so ALWAYS logs as far as it got, only
  // actually writes details when SceneLoader.debugLogging = true (default), or
  // exception encountered. Everything stored in var log instead of writing
  // separate lines to support only writing in exception, and avoid problems
  // with multiple concurrent .babylon loads.
  std::ostringstream log;
  log << "importMesh has failed JSON parse";
  json parsedData;
  try {
    parsedData = json::parse(data);

    log.str(" ");
    log.clear();

    auto fullDetails
      = SceneLoader::LoggingLevel() == SceneLoader::DETAILED_LOGGING;

    std::vector<size_t> loadedSkeletonsIds;
    std::vector<std::string> loadedMaterialsIds;
    std::vector<std::string> hierarchyIds;

    for (const auto& parsedMesh :
         json_util::get_array<json>(parsedData, "meshes")) {
      if (meshesNames.empty()
          || isDescendantOf(parsedMesh, meshesNames, hierarchyIds)) {

        // Id
        const std::string parsedMeshId
          = json_util::get_string(parsedMesh, "id");

        // Geometry ?
        if (json_util::has_key(parsedMesh, "geometryId")
            && !json_util::is_null(parsedMesh["geometryId"])) {
          const auto parsedMeshGeometryId
            = json_util::get_string(parsedMesh, "geometryId");
          // Does the file contain geometries?
          if (json_util::has_key(parsedMesh, "geometries")
              && !json_util::is_null(parsedMesh["geometries"])) {
            auto geometries = parsedData["geometries"];
            // find the correct geometry and add it to the scene
            bool found = false;
            const std::array<std::string, 8> geometryTypes{
              {"boxes", "spheres", "cylinders", "toruses", "grounds", "planes",
               "torusKnots", "vertexData"}};
            for (const auto& geometryType : geometryTypes) {
              if (found || !json_util::has_key(geometries, geometryType)
                  || !(geometries[geometryType].is_array())) {
                break;
              }
              else {
                for (const auto& parsedGeometryData :
                     json_util::get_array<json>(geometries, geometryType)) {
                  const std::string parsedGeometryDataId
                    = json_util::get_string(parsedGeometryData, "id");
                  if (!parsedGeometryDataId.empty()
                      && (parsedGeometryDataId == parsedMeshGeometryId)) {
                    if (geometryType == "boxes") {
                      BoxGeometry::Parse(parsedGeometryData, scene);
                    }
                    else if (geometryType == "spheres") {
                      SphereGeometry::Parse(parsedGeometryData, scene);
                    }
                    else if (geometryType == "cylinders") {
                      CylinderGeometry::Parse(parsedGeometryData, scene);
                    }
                    else if (geometryType == "toruses") {
                      TorusGeometry::Parse(parsedGeometryData, scene);
                    }
                    else if (geometryType == "grounds") {
                      GroundGeometry::Parse(parsedGeometryData, scene);
                    }
                    else if (geometryType == "planes") {
                      PlaneGeometry::Parse(parsedGeometryData, scene);
                    }
                    else if (geometryType == "torusKnots") {
                      TorusKnotGeometry::Parse(parsedGeometryData, scene);
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
        if (json_util::has_key(parsedMesh, "materialId")) {
          const auto parsedMeshMaterialId
            = json_util::get_string(parsedMesh, "materialId");
          auto materialFound
            = stl_util::contains(loadedMaterialsIds, parsedMeshMaterialId);
          if (!parsedMeshMaterialId.empty() && !materialFound
              && json_util::has_key(parsedData, "multiMaterials")
              && parsedData["multiMaterials"].is_array()) {
            for (const auto& parsedMultiMaterial :
                 json_util::get_array<json>(parsedData, "multiMaterials")) {
              const auto parsedMultiMaterialId
                = json_util::get_string(parsedMultiMaterial, "id");
              if ((!parsedMultiMaterialId.empty())
                  && (parsedMultiMaterialId == parsedMeshMaterialId)) {
                if (json_util::has_key(parsedMultiMaterial, "materials")
                    && parsedMultiMaterial["materials"].is_array()) {
                  for (const auto& subMatId : json_util::get_array<json>(
                         parsedMultiMaterial, "materials")) {
                    loadedMaterialsIds.emplace_back(
                      subMatId.get<std::string>());
                    auto mat = parseMaterialById(subMatId.get<std::string>(),
                                                 parsedData, scene, rootUrl);
                    log << "\n\tMaterial " << mat->toString(fullDetails);
                  }
                }
                loadedMaterialsIds.emplace_back(parsedMultiMaterialId);
                auto mmat
                  = Material::ParseMultiMaterial(parsedMultiMaterial, scene);
                materialFound = true;
                log << "\n\tMulti-Material " << mmat->toString(fullDetails);
                break;
              }
            }
          }

          if (!materialFound) {
            loadedMaterialsIds.emplace_back(parsedMeshMaterialId);
            auto mat = parseMaterialById(parsedMeshMaterialId, parsedData,
                                         scene, rootUrl);
            if (!mat) {
              BABYLON_LOGF_WARN("BabylonFileLoader",
                                "Material not found for mesh %s",
                                parsedMeshId.c_str());
            }
            else {
              log << "\n\tMaterial " << mat->toString(fullDetails);
            }
          }
        }

        // Skeleton ?
        if (json_util::has_key(parsedMesh, "skeletonId")) {
          const auto parsedMeshSkeletonId
            = json_util::get_number<int>(parsedMesh, "skeletonId", -1);
          if ((parsedMeshSkeletonId > -1)
              && json_util::has_key(parsedData, "skeletons")
              && parsedData["skeletons"].is_array()
              && !stl_util::contains(loadedSkeletonsIds,
                                     parsedMeshSkeletonId)) {
            for (const auto& parsedSkeleton :
                 json_util::get_array<json>(parsedData, "skeletons")) {
              const auto parsedSkeletonId
                = json_util::get_number<int>(parsedSkeleton, "id", -1);
              if (parsedSkeletonId == parsedMeshSkeletonId) {
                auto skeleton = Skeleton::Parse(parsedSkeleton, scene);
                skeletons.emplace_back(skeleton);
                loadedSkeletonsIds.emplace_back(
                  static_cast<size_t>(parsedSkeletonId));
                log << "\n\tSkeleton " << skeleton->toString(fullDetails);
              }
            }
          }
        }

        // Morph targets ?
        if (json_util::has_key(parsedData, "morphTargetManagers")
            && parsedData["morphTargetManagers"].is_array()) {
          for (const auto& managerData :
               json_util::get_array<json>(parsedData, "morphTargetManagers")) {
            MorphTargetManager::Parse(managerData, scene);
          }
        }

        auto mesh = Mesh::Parse(parsedMesh, scene, rootUrl);
        meshes.emplace_back(mesh);
        log << "\n\tMesh " << mesh->toString(fullDetails);
      }
    }

    // Connecting parents
    for (auto& currentMesh : scene->meshes) {
      if (!currentMesh->_waitingParentId.empty()) {
        std::static_pointer_cast<Node>(currentMesh)->parent
          = scene->getLastEntryByID(currentMesh->_waitingParentId).get();
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

    // Particles
    if (json_util::has_key(parsedData, "particleSystems")
        && parsedData["particleSystems"].is_array()) {
      auto parser = AbstractScene::GetIndividualParser(
        SceneComponentConstants::NAME_PARTICLESYSTEM);
      if (parser.has_value()) {

        for (const auto& parsedParticleSystem :
             json_util::get_array<json>(parsedData, "particleSystems")) {
          const auto parsedParticleSystemEmitterId
            = json_util::get_string(parsedParticleSystem, "emitterId");
          if (!parsedParticleSystemEmitterId.empty()
              && stl_util::contains(hierarchyIds,
                                    parsedParticleSystemEmitterId)) {
            // TODO FIXME
            // particleSystems.emplace_back(
            //  parser.value()(parsedParticleSystem, scene, rootUrl));
          }
        }
      }
    }

    finally("importMesh", log, parsedData);

    return true;
  }
  catch (const std::exception& err) {
    auto msg = json_util::has_key(parsedData, "producer") ?
                 logOperation("importMesh",
                              json_util::get_string(parsedData, "producer")) :
                 logOperation("importMesh", "Unknown");
    msg += log.str();
    if (onError) {
      onError(msg, err.what());
    }
    else {
      BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", msg.c_str());
    }
  }
  catch (...) {
  }

  finally("importMesh", log, parsedData);

  return false;
}

bool BabylonFileLoader::load(
  Scene* scene, const std::string& data, const std::string& rootUrl,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError) const
{
  // Entire method running in try block, so ALWAYS logs as far as it got, only
  // actually writes details when SceneLoader.debugLogging = true (default), or
  // exception encountered. Everything stored in var log instead of writing
  // separate lines to support only writing in exception, and avoid problems
  // with multiple concurrent .babylon loads.
  std::ostringstream log;
  log << "importMesh has failed JSON parse";
  json parsedData;
  try {
    parsedData = json::parse(data);

    log.str(" ");
    log.clear();

    // Scene
    if (json_util::has_key(parsedData, "useDelayedTextureLoading")
        && !json_util::is_null(parsedData["useDelayedTextureLoading"])) {
      scene->useDelayedTextureLoading
        = json_util::get_bool(parsedData, "useDelayedTextureLoading")
          && !SceneLoader::ForceFullSceneLoadingForIncremental();
    }
    if (json_util::has_key(parsedData, "autoClear")
        && !json_util::is_null(parsedData["autoClear"])) {
      scene->autoClear = json_util::get_bool(parsedData, "autoClear", true);
    }
    if (json_util::has_key(parsedData, "clearColor")
        && !json_util::is_null(parsedData["clearColor"])) {
      scene->clearColor = Color4::FromArray(
        json_util::get_array<float>(parsedData, "clearColor"));
    }
    if (json_util::has_key(parsedData, "ambientColor")
        && !json_util::is_null(parsedData["ambientColor"])) {
      scene->ambientColor = Color3::FromArray(
        json_util::get_array<float>(parsedData, "ambientColor"));
    }
    if (json_util::has_key(parsedData, "gravity")
        && !json_util::is_null(parsedData["gravity"])) {
      scene->gravity = Vector3::FromArray(
        json_util::get_array<float>(parsedData, "gravity"));
    }

    // Fog
    if (json_util::has_key(parsedData, "fogMode")
        && (json_util::get_number<unsigned>(parsedData, "fogMode") != 0u)) {
      scene->fogMode  = json_util::get_number<unsigned>(parsedData, "fogMode");
      scene->fogColor = Color3::FromArray(
        json_util::get_array<float>(parsedData, "fogColor"));
      scene->fogStart   = json_util::get_number(parsedData, "fogStart", 0.f);
      scene->fogEnd     = json_util::get_number(parsedData, "fogEnd", 1000.f);
      scene->fogDensity = json_util::get_number(parsedData, "fogDensity", 0.1f);
      log << "\tFog mode for scene:  ";
      switch (scene->fogMode) {
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
    if (json_util::get_bool(parsedData, "physicsEnabled")) {
    }

    // Metadata
    if (json_util::has_key(parsedData, "metadata")
        && !json_util::is_null(parsedData["metadata"])) {
      scene->metadata = parsedData["metadata"];
    }

    // Collisions, if defined. otherwise, default is true
    if (json_util::has_key(parsedData, "collisionsEnabled")
        && !json_util::is_null(parsedData["collisionsEnabled"])) {
      scene->collisionsEnabled
        = json_util::get_bool(parsedData, "collisionsEnabled", true);
    }
    scene->workerCollisions
      = json_util::get_bool(parsedData, "workerCollisions");

    if (json_util::get_bool(parsedData, "autoAnimate", false)) {
#if 0
      scene->beginAnimation(
        scene, json_util::get_number(parsedData, "autoAnimateFrom", 0),
        json_util::get_number(parsedData, "autoAnimateTo", 0),
        json_util::get_bool(parsedData, "autoAnimateLoop"),
        json_util::get_number(parsedData, "autoAnimateFrom", 1.f));
#endif
    }

    // Collisions, if defined. otherwise, default is true
    if (json_util::has_key(parsedData, "activeCameraID")
        && !json_util::is_null(parsedData["activeCameraID"])) {
      scene->setActiveCameraByID(
        json_util::get_string(parsedData, "activeCameraID"));
    }

    // Environment texture
    if (json_util::has_key(parsedData, "environmentTexture")
        && !json_util::is_null(parsedData["environmentTexture"])) {
      if (json_util::has_key(parsedData, "environmentTextureType")
          && json_util::get_string(parsedData, "environmentTextureType")
               == "BABYLON.HDRCubeTexture") {
        size_t hdrSize
          = (json_util::has_key(parsedData, "environmentTextureSize")) ?
              json_util::get_number<size_t>(parsedData,
                                            "environmentTextureSize") :
              128;
        auto hdrTexture = HDRCubeTexture::New(
          rootUrl + json_util::get_string(parsedData, "environmentTexture"),
          scene, hdrSize);
        if (json_util::has_key(parsedData, "environmentTextureRotationY")) {
          hdrTexture->rotationY = json_util::get_number<float>(
            parsedData, "environmentTextureRotationY");
        }
        scene->environmentTexture = hdrTexture;
      }
      else {
        auto cubeTexture = CubeTexture::CreateFromPrefilteredData(
          rootUrl + json_util::get_string(parsedData, "environmentTexture"),
          scene);
        if (json_util::has_key(parsedData, "environmentTextureRotationY")) {
          cubeTexture->rotationY = json_util::get_number<float>(
            parsedData, "environmentTextureRotationY");
        }
        scene->environmentTexture = cubeTexture;
      }
      if (json_util::has_key(parsedData, "createDefaultSkybox")
          && json_util::get_bool(parsedData, "createDefaultSkybox")) {
        auto skyboxScale
          = (scene->activeCamera != nullptr) ?
              (scene->activeCamera->maxZ - scene->activeCamera->minZ) / 2.f :
              1000.f;
        float skyboxBlurLevel
          = json_util::get_number<float>(parsedData, "skyboxBlurLevel", 0.f);
        scene->createDefaultSkybox(nullptr, true, skyboxScale, skyboxBlurLevel);
      }
    }

    // Finish
    finally("importScene", log, parsedData);

    return true;
  }
  catch (const std::exception& err) {
    auto msg = json_util::has_key(parsedData, "producer") ?
                 logOperation("importScene",
                              json_util::get_string(parsedData, "producer")) :
                 logOperation("importScene", "Unknown");
    msg += log.str();
    if (onError) {
      onError(msg, err.what());
    }
    else {
      BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", msg.c_str());
    }
  }
  catch (...) {
  }

  finally("importScene", log, parsedData);

  return false;
}

AssetContainerPtr BabylonFileLoader::loadAssetContainer(
  Scene* scene, const std::string& data, const std::string& rootUrl,
  const std::function<void(const std::string& message,
                           const std::string& exception)>& onError,
  bool addToScene) const
{
  auto container = AssetContainer::New(scene);

  // Entire method running in try block, so ALWAYS logs as far as it got, only
  // actually writes details when SceneLoader.debugLogging = true (default), or
  // exception encountered. Everything stored in var log instead of writing
  // separate lines to support only writing in exception, and avoid problems
  // with multiple concurrent .babylon loads.
  std::ostringstream log;
  log << "importMesh has failed JSON parse";
  json parsedData;
  try {
    parsedData = json::parse(data);

    log.str(" ");
    log.clear();

    auto fullDetails
      = SceneLoader::LoggingLevel() == SceneLoader::DETAILED_LOGGING;

    // Lights
    unsigned int index = 0;
    for (const auto& parsedLight :
         json_util::get_array<json>(parsedData, "lights")) {
      auto light = Light::Parse(parsedLight, scene);
      if (light) {
        container->lights.emplace_back(light);
        log << (index == 0 ? "\n\tLights:" : "");
        log << "\n\t\t" << light->toString(fullDetails);
      }
      ++index;
    }

    // Animations
    index = 0;
    for (const auto& parsedAnimation :
         json_util::get_array<json>(parsedData, "animations")) {
      auto animation = Animation::Parse(parsedAnimation);
      scene->animations.emplace_back(animation);
      container->animations.emplace_back(animation);
      log << (index == 0 ? "\n\tAnimations:" : "");
      log << "\n\t\t" << animation->toString(fullDetails);
      ++index;
    }

    // Materials
    index = 0;
    for (const auto& parsedMaterial :
         json_util::get_array<json>(parsedData, "materials")) {
      auto mat = Material::Parse(parsedMaterial, scene, rootUrl);
      container->materials.emplace_back(mat);
      log << (index == 0 ? "\n\tMaterials:" : "");
      log << "\n\t\t" << mat->toString(fullDetails);
      ++index;
    }

    // Multi materials
    index = 0;
    for (const auto& parsedMultiMaterial :
         json_util::get_array<json>(parsedData, "multiMaterials")) {
      auto mmat = Material::ParseMultiMaterial(parsedMultiMaterial, scene);
      container->multiMaterials.emplace_back(mmat);
      log << (index == 0 ? "\n\tMultiMaterials:" : "");
      log << "\n\t\t" << mmat->toString(fullDetails);
      ++index;
    }

    // Morph targets
    for (const auto& managerData :
         json_util::get_array<json>(parsedData, "morphTargetManagers")) {
      container->morphTargetManagers.emplace_back(
        MorphTargetManager::Parse(managerData, scene));
    }

    // Skeletons
    index = 0;
    for (const auto& parsedSkeleton :
         json_util::get_array<json>(parsedData, "skeletons")) {
      auto skeleton = Skeleton::Parse(parsedSkeleton, scene);
      container->skeletons.emplace_back(skeleton);
      log << (index == 0 ? "\n\tSkeletons:" : "");
      log << "\n\t\t" << skeleton->toString(fullDetails);
      ++index;
    }

    if (json_util::has_key(parsedData, "geometries")
        && !json_util::is_null(parsedData["geometries"])) {
      const auto geometries = parsedData["geometries"];
      std::vector<GeometryPtr> addedGeometry;

      // Boxes
      for (const auto& parsedBox :
           json_util::get_array<json>(geometries, "boxes")) {
        addedGeometry.emplace_back(BoxGeometry::Parse(parsedBox, scene));
      }

      // Spheres
      for (const auto& parsedSphere :
           json_util::get_array<json>(geometries, "spheres")) {
        addedGeometry.emplace_back(SphereGeometry::Parse(parsedSphere, scene));
      }

      // Cylinders
      for (const auto& parsedCylinder :
           json_util::get_array<json>(geometries, "cylinders")) {
        addedGeometry.emplace_back(
          CylinderGeometry::Parse(parsedCylinder, scene));
      }

      // Toruses
      for (const auto& parsedTorus :
           json_util::get_array<json>(geometries, "toruses")) {
        addedGeometry.emplace_back(TorusGeometry::Parse(parsedTorus, scene));
      }

      // Grounds
      for (const auto& parsedGround :
           json_util::get_array<json>(geometries, "grounds")) {
        addedGeometry.emplace_back(GroundGeometry::Parse(parsedGround, scene));
      }

      // Planes
      for (const auto& parsedPlane :
           json_util::get_array<json>(geometries, "planes")) {
        addedGeometry.emplace_back(PlaneGeometry::Parse(parsedPlane, scene));
      }

      // TorusKnots
      for (const auto& parsedTorusKnot :
           json_util::get_array<json>(geometries, "torusKnots")) {
        addedGeometry.emplace_back(
          TorusKnotGeometry::Parse(parsedTorusKnot, scene));
      }

      // VertexData
      for (const auto& parsedVertexData :
           json_util::get_array<json>(geometries, "vertexData")) {
        addedGeometry.emplace_back(
          Geometry::Parse(parsedVertexData, scene, rootUrl));
      }

      for (const auto& g : addedGeometry) {
        if (g) {
          container->geometries.emplace_back(g);
        }
      }
    }

    // Transform nodes
    for (const auto& parsedTransformNode :
         json_util::get_array<json>(parsedData, "transformNodes")) {
      auto node = TransformNode::Parse(parsedTransformNode, scene, rootUrl);
      container->transformNodes.emplace_back(node);
    }

    // Meshes
    index = 0;
    for (const auto& parsedMesh :
         json_util::get_array<json>(parsedData, "meshes")) {
      auto mesh = Mesh::Parse(parsedMesh, scene, rootUrl);
      container->meshes.emplace_back(mesh);
      log << (index == 0 ? "\n\tMeshes:" : "");
      log << "\n\t\t" << mesh->toString(fullDetails);
      ++index;
    }

    // Cameras
    index = 0;
    for (const auto& parsedCamera :
         json_util::get_array<json>(parsedData, "cameras")) {
      auto camera = Camera::Parse(parsedCamera, scene);
      container->cameras.emplace_back(camera);
      log << (index == 0 ? "\n\tCameras:" : "");
      log << "\n\t\t" << camera->toString(fullDetails);
      ++index;
    }

    // Animation Groups
    index = 0;
    for (const auto& parsedAnimationGroup :
         json_util::get_array<json>(parsedData, "animationGroups")) {
      auto animationGroup = AnimationGroup::Parse(parsedAnimationGroup, scene);
      container->animationGroups.emplace_back(animationGroup);
      log << (index == 0 ? "\n\tAnimationGroups:" : "");
      log << "\n\t\t" + animationGroup->toString(fullDetails);
      ++index;
    }

    // Browsing all the graph to connect the dots
    for (auto& camera : scene->cameras) {
      if (!camera->_waitingParentId.empty()) {
        camera->parent
          = scene->getLastEntryByID(camera->_waitingParentId).get();
        camera->_waitingParentId.clear();
      }
    }

    for (auto& light : scene->lights) {
      if (!light->_waitingParentId.empty()) {
        light->parent = scene->getLastEntryByID(light->_waitingParentId).get();
        light->_waitingParentId.clear();
      }
    }

    // Connect parents & children and parse actions
    for (auto& transformNode : scene->transformNodes) {
      if (!transformNode->_waitingParentId.empty()) {
        static_cast<Node*>(transformNode.get())->parent
          = scene->getLastEntryByID(transformNode->_waitingParentId).get();
        transformNode->_waitingParentId.clear();
      }
    }
    for (auto& mesh : scene->meshes) {
      if (!mesh->_waitingParentId.empty()) {
        static_cast<Node*>(mesh.get())->parent
          = scene->getLastEntryByID(mesh->_waitingParentId).get();
        mesh->_waitingParentId.clear();
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

    // Lights exclusions / inclusions
    for (auto& light : scene->lights) {
      // Excluded check
      if (!light->_excludedMeshesIds.empty()) {
        for (const auto& excludedMeshesId : light->_excludedMeshesIds) {
          auto excludedMesh = scene->getMeshByID(excludedMeshesId);

          if (excludedMesh) {
            light->excludedMeshes().emplace_back(excludedMesh);
          }
        }

        light->_excludedMeshesIds.clear();
      }

      // Included check
      if (!light->_includedOnlyMeshesIds.empty()) {
        for (const auto& includedOnlyMeshesId : light->_includedOnlyMeshesIds) {
          auto includedOnlyMesh = scene->getMeshByID(includedOnlyMeshesId);

          if (includedOnlyMesh) {
            light->includedOnlyMeshes().emplace_back(includedOnlyMesh);
          }
        }

        light->_includedOnlyMeshesIds.clear();
      }
    }

    AbstractScene::Parse(parsedData, scene, container, rootUrl);

    // Actions (scene) Done last as it can access other objects.
    for (auto& mesh : scene->meshes) {
      if (!mesh->_waitingActions.empty()) {
        ActionManager::Parse(mesh->_waitingActions, mesh, scene);
        mesh->_waitingActions.clear();
      }
    }
    if (json_util::has_key(parsedData, "actions")
        && (parsedData["actions"].is_array())) {
      ActionManager::Parse(json_util::get_array<json>(parsedData, "actions"),
                           nullptr, scene);
    }

    if (!addToScene) {
      container->removeAllFromScene();
    }
  }
  catch (const std::exception& err) {
    auto msg = json_util::has_key(parsedData, "producer") ?
                 logOperation("loadAssets",
                              json_util::get_string(parsedData, "producer")) :
                 logOperation("loadAssets", "Unknown");
    msg += log.str();
    if (onError) {
      onError(msg, err.what());
    }
    else {
      BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", msg.c_str());
    }
  }
  catch (...) {
  }

  finally("loadAssets", log, parsedData);

  return container;
}

void BabylonFileLoader::finally(const std::string& producer,
                                const std::ostringstream& log,
                                const json& parsedData) const
{
  const auto _log = log.str();
  if (!_log.empty() && SceneLoader::LoggingLevel() != SceneLoader::NO_LOGGING) {
    const auto msg
      = json_util::has_key(parsedData, "producer") ?
          logOperation(producer, json_util::get_string(parsedData, "producer",
                                                       "Unknown")) :
          logOperation(producer);
    const auto logStr
      = SceneLoader::LoggingLevel() != SceneLoader::MINIMAL_LOGGING ? _log : "";
    BABYLON_LOGF_INFO("BabylonFileLoader", "%s%s", msg.c_str(), logStr.c_str());
  }
}

} // end of namespace BABYLON
