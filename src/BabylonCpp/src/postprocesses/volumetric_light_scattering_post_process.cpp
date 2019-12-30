#include <babylon/postprocesses/volumetric_light_scattering_post_process.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {

namespace {
Color4 savedSceneClearColor;
Color4 sceneClearColor(0.f, 0.f, 0.f, 1.f);
} // namespace

VolumetricLightScatteringPostProcess::VolumetricLightScatteringPostProcess(
  const std::string& iName, float ratio, const CameraPtr& camera, const MeshPtr& iMesh,
  unsigned int iSamples, const std::optional<unsigned int>& samplingMode, Engine* engine,
  bool reusable, Scene* scene)
    : PostProcess(iName, "volumetricLightScattering",
                  {"decay", "exposure", "weight", "meshPositionOnScreen", "density"},
                  {"lightScatteringSampler"}, ratio, camera,
                  samplingMode.value_or(TextureConstants::BILINEAR_SAMPLINGMODE), engine, reusable,
                  "#define NUM_SAMPLES " + std::to_string(iSamples))
    , attachedNode{nullptr}
    , customMeshPosition{Vector3::Zero()}
    , useCustomMeshPosition{false}
    , invert{true}
    , exposure{0.3f}
    , decay{0.96815f}
    , weight{0.58767f}
    , density{0.926f}
    , _screenCoordinates{Vector2::Zero()}
{
  // parameter "scene" can be null.
  auto scene_ = (camera == nullptr) ? scene : camera->getScene();

  auto engine_ = scene_->getEngine();
  _viewPort = Viewport(0, 0, 1, 1).toGlobal(engine_->getRenderWidth(), engine_->getRenderHeight());

  // Configure mesh
  mesh = (iMesh != nullptr) ? iMesh :
                              VolumetricLightScatteringPostProcess::CreateDefaultMesh(
                                "VolumetricLightScatteringMesh", scene);

  // Configure
  _createPass(scene, ratio);

  onActivate = [this](Camera* iCamera, EventState&) {
    if (!isSupported()) {
      dispose(iCamera);
    }

    onActivate = nullptr;
  };

  onApplyObservable.add([this, scene](Effect* effect, EventState&) {
    _updateMeshScreenCoordinates(scene);

    effect->setTexture("lightScatteringSampler", _volumetricLightScatteringRTT);
    effect->setFloat("exposure", exposure);
    effect->setFloat("decay", decay);
    effect->setFloat("weight", weight);
    effect->setFloat("density", density);
    effect->setVector2("meshPositionOnScreen", _screenCoordinates);
  });
}

VolumetricLightScatteringPostProcess::~VolumetricLightScatteringPostProcess() = default;

std::string VolumetricLightScatteringPostProcess::getClassName() const
{
  return "VolumetricLightScatteringPostProcess";
}

bool VolumetricLightScatteringPostProcess::_isReady(SubMesh* subMesh, bool useInstances)
{
  auto _mesh = subMesh->getMesh();

  // Render mesh as default
  if (_mesh == mesh && _mesh->material()) {
    return _mesh->material()->isReady(mesh.get());
  }

  std::vector<std::string> defines;
  std::vector<std::string> attribs{VertexBuffer::PositionKind};
  auto material = subMesh->getMaterial();

  // Alpha test
  if (material) {
    if (material->needAlphaTesting()) {
      defines.emplace_back("#define ALPHATEST");
    }

    if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
      attribs.emplace_back(VertexBuffer::UVKind);
      defines.emplace_back("#define UV1");
    }
    if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
      attribs.emplace_back(VertexBuffer::UV2Kind);
      defines.emplace_back("#define UV2");
    }
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders()) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKind);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKind);
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back(
      "#define BonesPerMesh "
      + std::to_string(mesh->skeleton() ? (mesh->skeleton()->bones.size() + 1) : 0));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    MaterialHelper::PushAttributesForInstances(attribs);
  }

  // Get correct effect
  std::string join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    IEffectCreationOptions options;
    options.attributes      = std::move(attribs);
    options.uniformsNames   = {"world", "mBones", "viewProjection", "diffuseMatrix"};
    options.samplers        = {"diffuseSampler"};
    options.defines         = std::move(join);
    options.indexParameters = {{"maxSimultaneousMorphTargets", mesh->numBoneInfluencers()}};

    _volumetricLightScatteringPass = mesh->getScene()->getEngine()->createEffect(
      "volumetricLightScatteringPass", options, mesh->getScene()->getEngine());
  }

  return _volumetricLightScatteringPass->isReady();
}

void VolumetricLightScatteringPostProcess::setCustomMeshPosition(const Vector3& position)
{
  customMeshPosition = position;
}

Vector3& VolumetricLightScatteringPostProcess::getCustomMeshPosition()
{
  return customMeshPosition;
}

void VolumetricLightScatteringPostProcess::dispose(Camera* camera)
{
  camera->getScene()->customRenderTargets.erase(
    std::remove(camera->getScene()->customRenderTargets.begin(),
                camera->getScene()->customRenderTargets.end(), _volumetricLightScatteringRTT),
    camera->getScene()->customRenderTargets.end());

  _volumetricLightScatteringRTT->dispose();
  PostProcess::dispose(camera);
}

RenderTargetTexturePtr& VolumetricLightScatteringPostProcess::getPass()
{
  return _volumetricLightScatteringRTT;
}

bool VolumetricLightScatteringPostProcess::_meshExcluded(const AbstractMeshPtr& mesh_)
{
  return !excludedMeshes.empty()

         && (stl_util::index_of(excludedMeshes, mesh_) != -1);
}

void VolumetricLightScatteringPostProcess::_createPass(Scene* scene, float ratio)
{
  auto engine = scene->getEngine();

  _volumetricLightScatteringRTT = RenderTargetTexture::New(
    "volumetricLightScatteringMap",
    ISize(static_cast<int>(static_cast<float>(engine->getRenderWidth()) * ratio),
          static_cast<int>(static_cast<float>(engine->getRenderHeight()) * ratio)),
    scene, false, true, Constants::TEXTURETYPE_UNSIGNED_INT);
  _volumetricLightScatteringRTT->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _volumetricLightScatteringRTT->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  _volumetricLightScatteringRTT->renderList().clear();
  _volumetricLightScatteringRTT->renderParticles      = false;
  _volumetricLightScatteringRTT->ignoreCameraViewport = true;

  auto camera = getCamera();
  if (camera) {
    camera->customRenderTargets.emplace_back(_volumetricLightScatteringRTT);
  }
  else {
    scene->customRenderTargets.emplace_back(_volumetricLightScatteringRTT);
  }

  // Custom render function for submeshes
  auto renderSubMesh = [this](SubMesh* subMesh) {
    auto _mesh = subMesh->getRenderingMesh();
    if (_meshExcluded(_mesh)) {
      return;
    }

    mesh->_internalAbstractMeshDataInfo._isActiveIntermediate = false;

    auto material = subMesh->getMaterial();

    if (!material) {
      return;
    }

    auto scene_  = _mesh->getScene();
    auto engine_ = scene_->getEngine();

    // Culling
    engine_->setState(material->backFaceCulling());

    // Managing instances
    auto batch = _mesh->_getInstancesRenderList(subMesh->_id);

    if (batch->mustReturn) {
      return;
    }

    bool hardwareInstancedRendering
      = (engine_->getCaps().instancedArrays)
        && (batch->visibleInstances.find(subMesh->_id) != batch->visibleInstances.end());

    if (_isReady(subMesh, hardwareInstancedRendering)) {
      auto effect = _volumetricLightScatteringPass;
      if (_mesh == mesh) {
        effect = material->getEffect();
      }

      engine_->enableEffect(effect);
      _mesh->_bind(subMesh, effect, Material::TriangleFillMode);

      if (_mesh == mesh) {
        material->bind(_mesh->getWorldMatrix(), _mesh.get());
      }
      else {
        auto iMterial = subMesh->getMaterial();

        _volumetricLightScatteringPass->setMatrix("viewProjection", scene_->getTransformMatrix());

        // Alpha test
        if (iMterial && iMterial->needAlphaTesting()) {
          auto alphaTexture = iMterial->getAlphaTestTexture();

          _volumetricLightScatteringPass->setTexture("diffuseSampler", alphaTexture);

          if (alphaTexture) {
            _volumetricLightScatteringPass->setMatrix("diffuseMatrix",
                                                      *alphaTexture->getTextureMatrix());
          }
        }

        // Bones
        if (mesh->useBones() && mesh->computeBonesUsingShaders() && mesh->skeleton()) {
          _volumetricLightScatteringPass->setMatrices(
            "mBones", mesh->skeleton()->getTransformMatrices(mesh.get()));
        }
      }

      // Draw
      mesh->_processRendering(
        subMesh, _volumetricLightScatteringPass, Material::TriangleFillMode, batch,
        hardwareInstancedRendering,
        [effect](bool /*isInstance*/, Matrix world, Material* /*effectiveMaterial*/) {
          effect->setMatrix("world", world);
        });
    }
  };

  // Render target texture callbacks
  // Color4 savedSceneClearColor;
  // Color4 sceneClearColor(0.f, 0.f, 0.f, 1.f);

  _volumetricLightScatteringRTT->onBeforeRenderObservable.add([scene](int*, EventState&) {
    savedSceneClearColor = scene->clearColor;
    scene->clearColor    = sceneClearColor;
  });

  _volumetricLightScatteringRTT->onAfterRenderObservable.add(
    [scene](int*, EventState&) { scene->clearColor = savedSceneClearColor; });

  _volumetricLightScatteringRTT->customRenderFunction
    = [scene, engine, renderSubMesh](const std::vector<SubMesh*>& opaqueSubMeshes,
                                     const std::vector<SubMesh*>& alphaTestSubMeshes,
                                     const std::vector<SubMesh*>& transparentSubMeshes,
                                     const std::vector<SubMesh*>& depthOnlySubMeshes,
                                     const std::function<void()>& /*beforeTransparents*/) {
        auto pEngine = scene->getEngine();

        if (!depthOnlySubMeshes.empty()) {
          engine->setColorWrite(false);
          for (auto& depthOnlySubMesh : depthOnlySubMeshes) {
            renderSubMesh(depthOnlySubMesh);
          }
          engine->setColorWrite(true);
        }

        for (const auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }

        if (!transparentSubMeshes.empty()) {
          // Sort sub meshes
          for (auto& submesh : transparentSubMeshes) {
            auto& boundingInfo = *submesh->getBoundingInfo();

            if (scene->activeCamera()) {
              submesh->_alphaIndex = submesh->getMesh()->alphaIndex;
              submesh->_distanceToCamera
                = boundingInfo.boundingSphere.centerWorld.subtract(scene->activeCamera()->position)
                    .length();
            }
          }

          auto sortedArray = stl_util::slice(transparentSubMeshes, 0,
                                             static_cast<int>(transparentSubMeshes.size()));
          BABYLON::stl_util::sort_js_style(sortedArray, [](const SubMesh* a, const SubMesh* b) {
            // Alpha index first
            if (a->_alphaIndex > b->_alphaIndex) {
              return 1;
            }
            if (a->_alphaIndex < b->_alphaIndex) {
              return -1;
            }

            // Then distance to camera
            if (a->_distanceToCamera < b->_distanceToCamera) {
              return 1;
            }
            if (a->_distanceToCamera > b->_distanceToCamera) {
              return -1;
            }

            return 0;
          });

          // Render sub meshes
          pEngine->setAlphaMode(Constants::ALPHA_COMBINE);
          for (const auto& subMesh : sortedArray) {
            renderSubMesh(subMesh);
          }
          pEngine->setAlphaMode(Constants::ALPHA_DISABLE);
        }
      };
}

void VolumetricLightScatteringPostProcess::_updateMeshScreenCoordinates(Scene* scene)
{
  auto transform = scene->getTransformMatrix();
  Vector3 meshPosition;

  if (useCustomMeshPosition) {
    meshPosition = customMeshPosition;
  }
  else if (attachedNode) {
    meshPosition = *attachedNode;
  }
  else {
    meshPosition = mesh->parent() ? mesh->getAbsolutePosition() : mesh->position();
  }

  auto identityMatrix = Matrix::Identity();
  auto pos            = Vector3::Project(meshPosition, identityMatrix, transform, _viewPort);

  _screenCoordinates.x = pos.x / static_cast<float>(_viewPort.width);
  _screenCoordinates.y = pos.y / static_cast<float>(_viewPort.height);

  if (invert) {
    _screenCoordinates.y = 1.f - _screenCoordinates.y;
  }
}

MeshPtr VolumetricLightScatteringPostProcess::CreateDefaultMesh(const std::string& iName,
                                                                Scene* scene)
{
  auto mesh           = Mesh::CreatePlane(iName, 1.f, scene);
  mesh->billboardMode = AbstractMesh::BILLBOARDMODE_ALL;

  auto material           = StandardMaterial::New(iName + "Material", scene);
  material->emissiveColor = Color3(1.f, 1.f, 1.f);

  mesh->material = material;

  return mesh;
}

} // end of namespace BABYLON
