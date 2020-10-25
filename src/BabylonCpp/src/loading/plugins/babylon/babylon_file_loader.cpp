#include <babylon/loading/plugins/babylon/babylon_file_loader.h>

#include <babylon/actions/action_manager.h>
#include <babylon/animations/animation.h>
#include <babylon/animations/animation_group.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/bones/bone.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json_util.h>
#include <babylon/core/logging.h>
#include <babylon/engines/asset_container.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/lensflares/lens_flare_system.h>
#include <babylon/lights/light.h>
#include <babylon/lights/shadows/shadow_generator.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/material.h>
#include <babylon/materials/multi_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/meshes/geometry.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/string_tools.h>
#include <babylon/misc/tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/particles/particle_system.h>
#include <babylon/postprocesses/post_process.h>
#include <babylon/probes/reflection_probe.h>

namespace BABYLON {

BabylonFileLoader::BabylonFileLoader()
{
  name          = "babylon.js";
  extensions    = ".babylon";
  canDirectLoad = [](const std::string& data) {
    // We consider that the producer string is filled
    return StringTools::endsWith(data, "babylon");
  };
}

BabylonFileLoader::~BabylonFileLoader() = default;

MaterialPtr BabylonFileLoader::parseMaterialById(const std::string& id, const json& parsedData,
                                                 Scene* scene, const std::string& rootUrl) const
{
  for (const auto& parsedMaterial : json_util::get_array<json>(parsedData, "materials")) {
    if (json_util::get_string(parsedMaterial, "id") == id) {
      return Material::Parse(parsedMaterial, scene, rootUrl);
    }
  }
  return nullptr;
}

bool BabylonFileLoader::isDescendantOf(const json& mesh, const std::vector<std::string>& names,
                                       std::vector<std::string>& hierarchyIds) const
{
  for (const auto& iName : names) {
    if (json_util::get_string(mesh, "name") == iName) {
      hierarchyIds.emplace_back(json_util::get_string(mesh, "id"));
      return true;
    }
  }
  if (json_util::has_key(mesh, "parentId")
      && stl_util::contains(hierarchyIds, json_util::get_string(mesh, "parentId"))) {
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
         + json_util::get_string(producer, "name")
         + " version: " + json_util::get_string(producer, "version")
         + ", exporter version: " + json_util::get_string(producer, "exporter_version");
}

void BabylonFileLoader::loadDetailLevels(Scene* scene, const AbstractMeshPtr& mesh) const
{
  auto mastermesh = std::static_pointer_cast<Mesh>(mesh);

  // Every value specified in the ids array of the lod data points to another mesh which should be
  // used as the lower LOD level. The distances (or coverages) array values specified are used along
  // with the lod mesh ids as a hint to determine the switching threshold for the various LODs.
  if (mesh->_waitingData.lods.has_value()) {
    if (!mesh->_waitingData.lods->ids.empty()) {
      const auto& lodmeshes = mesh->_waitingData.lods->ids;
      const auto wasenabled = mastermesh->isEnabled(false);
      if (!mesh->_waitingData.lods->distances.empty()) {
        const auto& distances = mesh->_waitingData.lods->distances;
        if (distances.size() >= lodmeshes.size()) {
          const auto culling = (distances.size() > lodmeshes.size()) ? distances.back() : 0.f;
          mastermesh->setEnabled(false);
          for (size_t index = 0; index < lodmeshes.size(); index++) {
            const auto& lodid = lodmeshes[index];
            auto lodmesh      = std::static_pointer_cast<Mesh>(scene->getMeshByID(lodid));
            if (lodmesh != nullptr) {
              mastermesh->addLODLevel(distances[index], lodmesh);
            }
          }
          if (culling > 0.f) {
            mastermesh->addLODLevel(culling, nullptr);
          }
          if (wasenabled) {
            mastermesh->setEnabled(true);
          }
        }
        else {
          BABYLON_LOGF_WARN("BabylonFileLoader", "Invalid level of detail distances for %s",
                            mesh->name.c_str())
        }
      }
    }
    mesh->_waitingData.lods = std::nullopt;
  }
}

bool BabylonFileLoader::importMesh(
  const std::vector<std::string>& meshesNames, Scene* scene, const std::string& data,
  const std::string& rootUrl, std::vector<AbstractMeshPtr>& meshes,
  std::vector<IParticleSystemPtr>& /*particleSystems*/, std::vector<SkeletonPtr>& skeletons,
  const std::function<void(const std::string& message, const std::string& exception)>& onError)
  const
{
  // Entire method running in try block, so ALWAYS logs as far as it got, only actually writes
  // details when SceneLoader.debugLogging = true (default), or exception encountered. Everything
  // stored in var log instead of writing separate lines to support only writing in exception, and
  // avoid problems with multiple concurrent .babylon loads.
  std::ostringstream log;
  log << "importMesh has failed JSON parse";
  json parsedData;
  try {
    parsedData = json::parse(data);

    log.str(" ");
    log.clear();

    auto fullDetails = SceneLoader::LoggingLevel() == SceneLoader::DETAILED_LOGGING;

    std::vector<size_t> loadedSkeletonsIds;
    std::vector<std::string> loadedMaterialsIds;
    std::vector<std::string> hierarchyIds;

    for (const auto& parsedMesh : json_util::get_array<json>(parsedData, "meshes")) {
      if (meshesNames.empty() || isDescendantOf(parsedMesh, meshesNames, hierarchyIds)) {

        // Id
        const std::string parsedMeshId = json_util::get_string(parsedMesh, "id");

        // Geometry ?
        if (json_util::has_valid_key_value(parsedMesh, "geometryId")) {
          const auto parsedMeshGeometryId = json_util::get_string(parsedMesh, "geometryId");
          // Does the file contain geometries?
          if (json_util::has_valid_key_value(parsedMesh, "geometries")) {
            auto geometries = parsedData["geometries"];
            // find the correct geometry and add it to the scene
            bool found = false;
            const std::array<std::string, 8> geometryTypes{{"boxes", "spheres", "cylinders",
                                                            "toruses", "grounds", "planes",
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
                    if (geometryType == "vertexData") {
                      Geometry::Parse(parsedGeometryData, scene, rootUrl);
                    }
                    found = true;
                  }
                }
              }
            }
            if (!found) {
              BABYLON_LOGF_WARN("BabylonFileLoader", "Geometry not found for mesh %s",
                                parsedMeshId.c_str())
            }
          }
        }

        // Material ?
        if (json_util::has_key(parsedMesh, "materialId")) {
          const auto parsedMeshMaterialId = json_util::get_string(parsedMesh, "materialId");
          auto materialFound = stl_util::contains(loadedMaterialsIds, parsedMeshMaterialId);
          if (!parsedMeshMaterialId.empty() && !materialFound
              && json_util::has_key(parsedData, "multiMaterials")
              && parsedData["multiMaterials"].is_array()) {
            for (const auto& parsedMultiMaterial :
                 json_util::get_array<json>(parsedData, "multiMaterials")) {
              const auto parsedMultiMaterialId = json_util::get_string(parsedMultiMaterial, "id");
              if ((!parsedMultiMaterialId.empty())
                  && (parsedMultiMaterialId == parsedMeshMaterialId)) {
                if (json_util::has_key(parsedMultiMaterial, "materials")
                    && parsedMultiMaterial["materials"].is_array()) {
                  for (const auto& subMatId :
                       json_util::get_array<json>(parsedMultiMaterial, "materials")) {
                    loadedMaterialsIds.emplace_back(subMatId.get<std::string>());
                    auto mat
                      = parseMaterialById(subMatId.get<std::string>(), parsedData, scene, rootUrl);
                    if (mat) {
                      log << "\n\tMaterial " << mat->toString(fullDetails);
                    }
                  }
                }
                loadedMaterialsIds.emplace_back(parsedMultiMaterialId);
                auto mmat = MultiMaterial::ParseMultiMaterial(parsedMultiMaterial, scene);
                if (mmat) {
                  materialFound = true;
                  log << "\n\tMulti-Material " << mmat->toString(fullDetails);
                }
                break;
              }
            }
          }

          if (!materialFound && !parsedMeshMaterialId.empty()) {
            loadedMaterialsIds.emplace_back(parsedMeshMaterialId);
            auto mat = parseMaterialById(parsedMeshMaterialId, parsedData, scene, rootUrl);
            if (!mat) {
              BABYLON_LOGF_WARN("BabylonFileLoader", "Material not found for mesh %s",
                                parsedMeshId.c_str())
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
          if ((parsedMeshSkeletonId > -1) && json_util::has_key(parsedData, "skeletons")
              && parsedData["skeletons"].is_array()
              && !stl_util::contains(loadedSkeletonsIds, parsedMeshSkeletonId)) {
            for (const auto& parsedSkeleton : json_util::get_array<json>(parsedData, "skeletons")) {
              const auto parsedSkeletonId = json_util::get_number<int>(parsedSkeleton, "id", -1);
              if (parsedSkeletonId == parsedMeshSkeletonId) {
                auto skeleton = Skeleton::Parse(parsedSkeleton, scene);
                skeletons.emplace_back(skeleton);
                loadedSkeletonsIds.emplace_back(static_cast<size_t>(parsedSkeletonId));
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

    // Connecting parents and lods
    for (const auto& currentMesh : scene->meshes) {
      if (!currentMesh->_waitingParentId.empty()) {
        std::static_pointer_cast<Node>(currentMesh)->parent
          = scene->getLastEntryByID(currentMesh->_waitingParentId).get();
        currentMesh->_waitingParentId = "";
      }
      if (currentMesh->_waitingData.lods.has_value()) {
        loadDetailLevels(scene, currentMesh);
      }
    }

    // link skeleton transform nodes
    for (const auto& skeleton : scene->skeletons) {
      if (skeleton->_hasWaitingData.has_value()) {
        if (!skeleton->bones.empty()) {
          for (const auto& bone : skeleton->bones) {
            if (!bone->_waitingTransformNodeId.empty()) {
              auto linkTransformNode = std::static_pointer_cast<TransformNode>(
                scene->getLastEntryByID(bone->_waitingTransformNodeId));
              if (linkTransformNode) {
                bone->linkTransformNode(linkTransformNode);
              }
              bone->_waitingTransformNodeId.clear();
            }
          }
        }

        if (!skeleton->_waitingOverrideMeshId.empty()) {
          skeleton->overrideMesh           = scene->getMeshByID(skeleton->_waitingOverrideMeshId);
          skeleton->_waitingOverrideMeshId = "";
        }
        skeleton->_hasWaitingData = std::nullopt;
      }
    }

    // freeze and compute world matrix application
    for (const auto& currentMesh : scene->meshes) {
      if (currentMesh->_waitingData.freezeWorldMatrix.has_value()
          && *currentMesh->_waitingData.freezeWorldMatrix) {
        currentMesh->freezeWorldMatrix();
        currentMesh->_waitingData.freezeWorldMatrix = std::nullopt;
      }
      else {
        currentMesh->computeWorldMatrix(true);
      }
    }

    // Particles
    if (json_util::has_key(parsedData, "particleSystems")
        && parsedData["particleSystems"].is_array()) {
      auto parser
        = AbstractScene::GetIndividualParser(SceneComponentConstants::NAME_PARTICLESYSTEM);
      if (parser.has_value()) {

        for (const auto& parsedParticleSystem :
             json_util::get_array<json>(parsedData, "particleSystems")) {
          const auto parsedParticleSystemEmitterId
            = json_util::get_string(parsedParticleSystem, "emitterId");
          if (!parsedParticleSystemEmitterId.empty()
              && stl_util::contains(hierarchyIds, parsedParticleSystemEmitterId)) {
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
                 logOperation("importMesh", json_util::get_string(parsedData, "producer")) :
                 logOperation("importMesh", "Unknown");
    msg += log.str();
    if (onError) {
      onError(msg, err.what());
    }
    else {
      BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", msg.c_str())
    }
  }
  catch (...) {
  }

  finally("importMesh", log, parsedData);

  return false;
}

bool BabylonFileLoader::load(Scene* scene, const std::string& data, const std::string& rootUrl,
                             const std::function<void(const std::string& message,
                                                      const std::string& exception)>& onError) const
{
  // Entire method running in try block, so ALWAYS logs as far as it got, only actually writes
  // details when SceneLoader.debugLogging = true (default), or exception encountered. Everything
  // stored in var log instead of writing separate lines to support only writing in exception, and
  // avoid problems with multiple concurrent .babylon loads.
  std::ostringstream log;
  log << "importMesh has failed JSON parse";
  json parsedData;
  try {
    parsedData = json::parse(data);

    log.str(" ");
    log.clear();

    // Scene
    if (json_util::has_valid_key_value(parsedData, "useDelayedTextureLoading")) {
      scene->useDelayedTextureLoading = json_util::get_bool(parsedData, "useDelayedTextureLoading")
                                        && !SceneLoader::ForceFullSceneLoadingForIncremental();
    }
    if (json_util::has_valid_key_value(parsedData, "autoClear")) {
      scene->autoClear = json_util::get_bool(parsedData, "autoClear", true);
    }
    if (json_util::has_valid_key_value(parsedData, "clearColor")) {
      scene->clearColor = Color4::FromArray(json_util::get_array<float>(parsedData, "clearColor"));
    }
    if (json_util::has_valid_key_value(parsedData, "ambientColor")) {
      scene->ambientColor
        = Color3::FromArray(json_util::get_array<float>(parsedData, "ambientColor"));
    }
    if (json_util::has_valid_key_value(parsedData, "gravity")) {
      scene->gravity = Vector3::FromArray(json_util::get_array<float>(parsedData, "gravity"));
    }

    // Fog
    if (json_util::has_key(parsedData, "fogMode")
        && (json_util::get_number<unsigned>(parsedData, "fogMode") != 0u)) {
      scene->fogMode    = json_util::get_number<unsigned>(parsedData, "fogMode");
      scene->fogColor   = Color3::FromArray(json_util::get_array<float>(parsedData, "fogColor"));
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
    if (json_util::has_valid_key_value(parsedData, "metadata")) {
      scene->metadata = parsedData["metadata"];
    }

    // Collisions, if defined. otherwise, default is true
    if (json_util::has_valid_key_value(parsedData, "collisionsEnabled")) {
      scene->collisionsEnabled = json_util::get_bool(parsedData, "collisionsEnabled", true);
    }

    auto container = loadAssetContainer(scene, data, rootUrl, onError, true);
    if (!container) {
      return false;
    }

    if (json_util::get_bool(parsedData, "autoAnimate", false)) {
#if 0
      scene->beginAnimation(
        scene, json_util::get_number(parsedData, "autoAnimateFrom", 0),
        json_util::get_number(parsedData, "autoAnimateTo", 0),
        json_util::get_bool(parsedData, "autoAnimateLoop"),
        json_util::get_number(parsedData, "autoAnimateFrom", 1.f));
#endif
    }

    if (json_util::has_valid_key_value(parsedData, "activeCameraID")) {
      scene->setActiveCameraByID(json_util::get_string(parsedData, "activeCameraID"));
    }

    // Finish
    finally("importScene", log, parsedData);

    return true;
  }
  catch (const std::exception& err) {
    auto msg = json_util::has_key(parsedData, "producer") ?
                 logOperation("importScene", json_util::get_string(parsedData, "producer")) :
                 logOperation("importScene", "Unknown");
    msg += log.str();
    if (onError) {
      onError(msg, err.what());
    }
    else {
      BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", msg.c_str())
    }
  }
  catch (...) {
  }

  finally("importScene", log, parsedData);

  return false;
}

AssetContainerPtr BabylonFileLoader::loadAssetContainer(
  Scene* scene, const std::string& data, const std::string& rootUrl,
  const std::function<void(const std::string& message, const std::string& exception)>& onError,
  bool addToScene) const
{
  auto container = AssetContainer::New(scene);

  // Entire method running in try block, so ALWAYS logs as far as it got, only actually writes
  // details when SceneLoader.debugLogging = true (default), or exception encountered. Everything
  // stored in var log instead of writing separate lines to support only writing in exception, and
  // avoid problems with multiple concurrent .babylon loads.
  std::ostringstream log;
  log << "importMesh has failed JSON parse";
  json parsedData;
  try {
    parsedData = json::parse(data);

    log.str(" ");
    log.clear();

    auto fullDetails = SceneLoader::LoggingLevel() == SceneLoader::DETAILED_LOGGING;

    // Environment texture
    if (json_util::has_valid_key_value(parsedData, "environmentTexture")) {
      const auto environmentTextureStr = json_util::get_string(parsedData, "environmentTexture");
      // PBR needed for both HDR texture (gamma space) & a sky box
      auto isPBR = json_util::get_bool(parsedData, "isPBR", true);
      if (json_util::has_valid_key_value(parsedData, "environmentTextureType")
          && json_util::get_string(parsedData, "environmentTextureType")
               == "BABYLON.HDRCubeTexture") {
        const auto hdrSize
          = json_util::get_number<size_t>(parsedData, "environmentTextureSize", 128);
        const std::regex re("https?://", std::regex::optimize);
        const auto regexMatch = StringTools::regexMatch(environmentTextureStr, re);
        auto hdrTexture
          = HDRCubeTexture::New((!regexMatch.empty() ? "" : rootUrl) + environmentTextureStr, scene,
                                hdrSize, true, !isPBR);
        if (json_util::has_valid_key_value(parsedData, "environmentTextureRotationY")) {
          hdrTexture->rotationY
            = json_util::get_number<float>(parsedData, "environmentTextureRotationY");
        }
        scene->environmentTexture = hdrTexture;
      }
      else {
        if (StringTools::endsWith(environmentTextureStr, ".env")) {
          const std::regex re("https?://", std::regex::optimize);
          const auto regexMatch = StringTools::regexMatch(environmentTextureStr, re);
          auto compressedTexture
            = CubeTexture::New((!regexMatch.empty() ? "" : rootUrl) + environmentTextureStr, scene);
          if (json_util::has_valid_key_value(parsedData, "environmentTextureRotationY")) {
            compressedTexture->rotationY
              = json_util::get_number<float>(parsedData, "environmentTextureRotationY");
          }
          scene->environmentTexture = compressedTexture;
        }
        else {
          const std::regex re("https?://", std::regex::optimize);
          const auto regexMatch = StringTools::regexMatch(environmentTextureStr, re);
          auto cubeTexture      = CubeTexture::CreateFromPrefilteredData(
            (!regexMatch.empty() ? "" : rootUrl) + environmentTextureStr, scene);
          if (json_util::has_valid_key_value(parsedData, "environmentTextureRotationY")) {
            cubeTexture->rotationY
              = json_util::get_number<float>(parsedData, "environmentTextureRotationY");
          }
          scene->environmentTexture = cubeTexture;
        }
      }
      if (json_util::get_bool(parsedData, "createDefaultSkybox")) {
        const auto skyboxScale
          = (scene->activeCamera() != nullptr) ?
              (scene->activeCamera()->maxZ - scene->activeCamera()->minZ) / 2.f :
              1000.f;
        const auto skyboxBlurLevel
          = json_util::get_number<float>(parsedData, "skyboxBlurLevel", 0.f);
        scene->createDefaultSkybox(scene->environmentTexture, isPBR, skyboxScale, skyboxBlurLevel);
      }
      container->environmentTexture = scene->environmentTexture();
    }

    // Environment Intensity
    if (json_util::has_valid_key_value(parsedData, "environmentIntensity")) {
      scene->environmentIntensity
        = json_util::get_number<float>(parsedData, "environmentIntensity", 0.f);
    }

    // Lights
    unsigned int index = 0;
    for (const auto& parsedLight : json_util::get_array<json>(parsedData, "lights")) {
      auto light = Light::Parse(parsedLight, scene);
      if (light) {
        container->lights.emplace_back(light);
        log << (index == 0 ? "\n\tLights:" : "");
        log << "\n\t\t" << light->toString(fullDetails);
      }
      ++index;
    }

    // Reflection probes
    index = 0;
    for (const auto& parsedReflectionProbe :
         json_util::get_array<json>(parsedData, "reflectionProbes")) {
      auto reflectionProbe = ReflectionProbe::Parse(parsedReflectionProbe, scene, rootUrl);
      if (reflectionProbe) {
        container->reflectionProbes.emplace_back(reflectionProbe);
        log << (index == 0 ? "\n\tReflection Probes:" : "");
        log << "\n\t\t" << reflectionProbe->toString(fullDetails);
      }
    }

    // Animations
    index = 0;
    for (const auto& parsedAnimation : json_util::get_array<json>(parsedData, "animations")) {
      auto animation = Animation::Parse(parsedAnimation);
      scene->animations.emplace_back(animation);
      container->animations.emplace_back(animation);
      log << (index == 0 ? "\n\tAnimations:" : "");
      log << "\n\t\t" << animation->toString(fullDetails);
      ++index;
    }

    // Materials
    index = 0;
    for (const auto& parsedMaterial : json_util::get_array<json>(parsedData, "materials")) {
      auto mat = Material::Parse(parsedMaterial, scene, rootUrl);
      if (mat) {
        container->materials.emplace_back(mat);
        log << (index == 0 ? "\n\tMaterials:" : "");
        log << "\n\t\t" << mat->toString(fullDetails);

        // Textures
        for (const auto& t : mat->getActiveTextures()) {
          if (!stl_util::contains(container->textures, t)) {
            container->textures.emplace_back(t);
          }
        }
      }
      ++index;
    }

    // Multi materials
    index = 0;
    for (const auto& parsedMultiMaterial :
         json_util::get_array<json>(parsedData, "multiMaterials")) {
      auto mmat = MultiMaterial::ParseMultiMaterial(parsedMultiMaterial, scene);
      if (mmat) {
        container->multiMaterials.emplace_back(mmat);
        log << (index == 0 ? "\n\tMultiMaterials:" : "");
        log << "\n\t\t" << mmat->toString(fullDetails);

        // Textures
        for (const auto& t : mmat->getActiveTextures()) {
          if (!stl_util::contains(container->textures, t)) {
            container->textures.emplace_back(t);
          }
        }
      }
      ++index;
    }

    // Morph targets
    for (const auto& managerData : json_util::get_array<json>(parsedData, "morphTargetManagers")) {
      container->morphTargetManagers.emplace_back(MorphTargetManager::Parse(managerData, scene));
    }

    // Skeletons
    index = 0;
    for (const auto& parsedSkeleton : json_util::get_array<json>(parsedData, "skeletons")) {
      auto skeleton = Skeleton::Parse(parsedSkeleton, scene);
      container->skeletons.emplace_back(skeleton);
      log << (index == 0 ? "\n\tSkeletons:" : "");
      log << "\n\t\t" << skeleton->toString(fullDetails);
      ++index;
    }

    // Geometries
    if (json_util::has_valid_key_value(parsedData, "geometries")) {
      const auto& geometries = parsedData["geometries"];
      std::vector<GeometryPtr> addedGeometry;

      // VertexData
      for (const auto& parsedVertexData : json_util::get_array<json>(geometries, "vertexData")) {
        addedGeometry.emplace_back(Geometry::Parse(parsedVertexData, scene, rootUrl));
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
    for (const auto& parsedMesh : json_util::get_array<json>(parsedData, "meshes")) {
      auto mesh = Mesh::Parse(parsedMesh, scene, rootUrl);
      container->meshes.emplace_back(mesh);
      if (mesh->hasInstances()) {
        for (const auto& instance : mesh->instances) {
          container->meshes.emplace_back(instance);
        }
      }
      log << (index == 0 ? "\n\tMeshes:" : "");
      log << "\n\t\t" << mesh->toString(fullDetails);
      ++index;
    }

    // Cameras
    index = 0;
    for (const auto& parsedCamera : json_util::get_array<json>(parsedData, "cameras")) {
      auto camera = Camera::Parse(parsedCamera, scene);
      container->cameras.emplace_back(camera);
      log << (index == 0 ? "\n\tCameras:" : "");
      log << "\n\t\t" << camera->toString(fullDetails);
      ++index;
    }

    // Postprocesses
    index = 0;
    for (const auto& parsedPostProcess : json_util::get_array<json>(parsedData, "postProcesses")) {
      auto postProcess = PostProcess::Parse(parsedPostProcess, scene, rootUrl);
      if (postProcess) {
        container->postProcesses.emplace_back(postProcess);
        log << (index == 0 ? "\n\tPostprocesses:" : "");
        log << "\n\t\t" + postProcess->toString();
        ++index;
      }
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
    for (const auto& camera : scene->cameras) {
      if (!camera->_waitingParentId.empty()) {
        camera->parent = scene->getLastEntryByID(camera->_waitingParentId).get();
        camera->_waitingParentId.clear();
      }
    }

    for (const auto& light : scene->lights) {
      if (!light->_waitingParentId.empty()) {
        light->parent = scene->getLastEntryByID(light->_waitingParentId).get();
        light->_waitingParentId.clear();
      }
    }

    // Connect parents & children and parse actions and lods
    for (const auto& transformNode : scene->transformNodes) {
      if (!transformNode->_waitingParentId.empty()) {
        static_cast<Node*>(transformNode.get())->parent
          = scene->getLastEntryByID(transformNode->_waitingParentId).get();
        transformNode->_waitingParentId.clear();
      }
    }
    for (const auto& mesh : scene->meshes) {
      if (!mesh->_waitingParentId.empty()) {
        static_cast<Node*>(mesh.get())->parent
          = scene->getLastEntryByID(mesh->_waitingParentId).get();
        mesh->_waitingParentId.clear();
      }
      if (mesh->_waitingData.lods.has_value()) {
        loadDetailLevels(scene, mesh);
      }
    }

    // link skeleton transform nodes
    for (const auto& skeleton : scene->skeletons) {
      if (skeleton->_hasWaitingData.has_value()) {
        if (!skeleton->bones.empty()) {
          for (const auto& bone : skeleton->bones) {
            if (!bone->_waitingTransformNodeId.empty()) {
              auto linkTransformNode = std::static_pointer_cast<TransformNode>(
                scene->getLastEntryByID(bone->_waitingTransformNodeId));
              if (linkTransformNode) {
                bone->linkTransformNode(linkTransformNode);
              }
              bone->_waitingTransformNodeId.clear();
            }
          }
        }

        if (!skeleton->_waitingOverrideMeshId.empty()) {
          skeleton->overrideMesh           = scene->getMeshByID(skeleton->_waitingOverrideMeshId);
          skeleton->_waitingOverrideMeshId = "";
        }
        skeleton->_hasWaitingData = std::nullopt;
      }
    }

    // freeze and compute world matrix application
    for (const auto& currentMesh : scene->meshes) {
      if (currentMesh->_waitingData.freezeWorldMatrix.has_value()
          && *currentMesh->_waitingData.freezeWorldMatrix) {
        currentMesh->freezeWorldMatrix();
        currentMesh->_waitingData.freezeWorldMatrix = std::nullopt;
      }
      else {
        currentMesh->computeWorldMatrix(true);
      }
    }

    // Lights exclusions / inclusions
    for (const auto& light : scene->lights) {
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
    for (const auto& mesh : scene->meshes) {
      if (!mesh->_waitingData.actions.empty()) {
        ActionManager::Parse(mesh->_waitingData.actions, mesh, scene);
        mesh->_waitingData.actions.clear();
      }
    }
    if (json_util::has_key(parsedData, "actions") && (parsedData["actions"].is_array())) {
      ActionManager::Parse(json_util::get_array<json>(parsedData, "actions"), nullptr, scene);
    }
  }
  catch (const std::exception& err) {
    auto msg = json_util::has_key(parsedData, "producer") ?
                 logOperation("loadAssets", json_util::get_string(parsedData, "producer")) :
                 logOperation("loadAssets", "Unknown");
    msg += log.str();
    if (onError) {
      onError(msg, err.what());
    }
    else {
      BABYLON_LOGF_ERROR("BabylonFileLoader", "%s", msg.c_str())
    }
  }
  catch (...) {
  }

  if (!addToScene) {
    container->removeAllFromScene();
  }
  finally("loadAssets", log, parsedData);

  return container;
}

void BabylonFileLoader::finally(const std::string& producer, const std::ostringstream& log,
                                const json& parsedData) const
{
  const auto _log = log.str();
  if (!_log.empty() && SceneLoader::LoggingLevel() != SceneLoader::NO_LOGGING) {
    const auto msg
      = json_util::has_key(parsedData, "producer") ?
          logOperation(producer, json_util::get_string(parsedData, "producer", "Unknown")) :
          logOperation(producer);
    const auto logStr = SceneLoader::LoggingLevel() != SceneLoader::MINIMAL_LOGGING ? _log : "";
    BABYLON_LOGF_INFO("BabylonFileLoader", "%s%s", msg.c_str(), logStr.c_str())
  }
}

} // end of namespace BABYLON
