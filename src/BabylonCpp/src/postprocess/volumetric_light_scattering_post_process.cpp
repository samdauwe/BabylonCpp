#include <babylon/postprocess/volumetric_light_scattering_post_process.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {

VolumetricLightScatteringPostProcess::VolumetricLightScatteringPostProcess(
  const std::string& _name, float ratio, Camera* camera, Mesh* mesh_,
  unsigned int samples, unsigned int samplingMode, Engine* engine,
  bool reusable, Scene* scene)
    : PostProcess(
        _name, "volumetricLightScattering",
        {"decay", "exposure", "weight", "meshPositionOnScreen", "density"},
        {"lightScatteringSampler"}, ratio, camera, samplingMode, engine,
        reusable, "#define NUM_SAMPLES " + std::to_string(samples))
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
  Scene* scene_ = (camera == nullptr) ? scene : camera->getScene();

  Engine* engine_ = scene_->getEngine();
  _viewPort
    = Viewport(0, 0, 1, 1)
        .toGlobal(engine_->getRenderWidth(), engine_->getRenderHeight());

  // Configure mesh
  mesh = (mesh_ != nullptr) ?
           mesh_ :
           VolumetricLightScatteringPostProcess::CreateDefaultMesh(
             "VolumetricLightScatteringMesh", scene);

  // Configure
  _createPass(scene, ratio);

  setOnActivate([&](Camera* camera_) {
    if (!isSupported()) {
      dispose(camera_);
    }

    setOnActivate(nullptr);
  });

  onApplyObservable.add([&](Effect* effect) {
    _updateMeshScreenCoordinates(scene);

    effect->setTexture("lightScatteringSampler", _volumetricLightScatteringRTT);
    effect->setFloat("exposure", exposure);
    effect->setFloat("decay", decay);
    effect->setFloat("weight", weight);
    effect->setFloat("density", density);
    effect->setVector2("meshPositionOnScreen", _screenCoordinates);
  });
}

VolumetricLightScatteringPostProcess::~VolumetricLightScatteringPostProcess()
{
}

bool VolumetricLightScatteringPostProcess::isReady(SubMesh* subMesh,
                                                   bool useInstances)
{
  AbstractMesh* _mesh = subMesh->getMesh();

  // Render mesh as default
  if (_mesh == mesh) {
    return _mesh->material->isReady(mesh);
  }

  std::vector<std::string> defines;
  std::vector<std::string> attribs{VertexBuffer::PositionKindChars};
  Material* material = subMesh->getMaterial();

  // Alpha test
  if (material) {
    if (material->needAlphaTesting()) {
      defines.emplace_back("#define ALPHATEST");
    }

    if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
      attribs.emplace_back(VertexBuffer::UVKindChars);
      defines.emplace_back("#define UV1");
    }
    if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)) {
      attribs.emplace_back(VertexBuffer::UV2KindChars);
      defines.emplace_back("#define UV2");
    }
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers));
    defines.emplace_back("#define BonesPerMesh "
                         + std::to_string(mesh->skeleton()->bones.size() + 1));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    attribs.emplace_back(VertexBuffer::World0KindChars);
    attribs.emplace_back(VertexBuffer::World1KindChars);
    attribs.emplace_back(VertexBuffer::World2KindChars);
    attribs.emplace_back(VertexBuffer::World3KindChars);
  }

  // Get correct effect
  std::string join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;
    std::unordered_map<std::string, std::string> baseName{
      {"vertexElement", "depth"},
      {"fragmentElement", "volumetricLightScatteringPass"}};
    _volumetricLightScatteringPass
      = mesh->getScene()->getEngine()->createEffect(
        baseName, attribs,
        {"world", "mBones", "viewProjection", "diffuseMatrix"},
        {"diffuseSampler"}, join);
  }

  return _volumetricLightScatteringPass->isReady();
}

void VolumetricLightScatteringPostProcess::setCustomMeshPosition(
  const Vector3& position)
{
  customMeshPosition = position;
}

Vector3& VolumetricLightScatteringPostProcess::getCustomMeshPosition()
{
  return customMeshPosition;
}

void VolumetricLightScatteringPostProcess::dispose(Camera* camera)
{
  int rttIndex = std_util::index_of(camera->getScene()->customRenderTargets,
                                    _volumetricLightScatteringRTT);
  if (rttIndex != -1) {
    std_util::splice(camera->getScene()->customRenderTargets, rttIndex, 1);
  }

  _volumetricLightScatteringRTT->dispose();
  PostProcess::dispose(camera);
}

RenderTargetTexture* VolumetricLightScatteringPostProcess::getPass()
{
  return _volumetricLightScatteringRTT;
}

bool VolumetricLightScatteringPostProcess::_meshExcluded(AbstractMesh* mesh_)
{
  if (excludedMeshes.size() > 0
      && (std_util::index_of(excludedMeshes, mesh_) != -1)) {
    return true;
  }

  return false;
}

void VolumetricLightScatteringPostProcess::_createPass(Scene* scene,
                                                       float ratio)
{
  Engine* engine = scene->getEngine();

  _volumetricLightScatteringRTT = new RenderTargetTexture(
    "volumetricLightScatteringMap",
    ISize(
      static_cast<int>(static_cast<float>(engine->getRenderWidth()) * ratio),
      static_cast<int>(static_cast<float>(engine->getRenderHeight()) * ratio)),
    scene, false, true, Engine::TEXTURETYPE_UNSIGNED_INT);
  _volumetricLightScatteringRTT->wrapU = Texture::CLAMP_ADDRESSMODE;
  _volumetricLightScatteringRTT->wrapV = Texture::CLAMP_ADDRESSMODE;
  _volumetricLightScatteringRTT->renderList.clear();
  _volumetricLightScatteringRTT->renderParticles = false;
  scene->customRenderTargets.emplace_back(_volumetricLightScatteringRTT);

  // Custom render function for submeshes
  auto renderSubMesh = [&](SubMesh* subMesh) {
    Mesh* _mesh = subMesh->getRenderingMesh();
    if (_meshExcluded(_mesh)) {
      return;
    }

    Scene* scene_   = _mesh->getScene();
    Engine* engine_ = scene_->getEngine();

    // Culling
    engine_->setState(subMesh->getMaterial()->backFaceCulling);

    // Managing instances
    _InstancesBatch* batch = _mesh->_getInstancesRenderList(subMesh->_id);

    if (batch->mustReturn) {
      return;
    }

    bool hardwareInstancedRendering
      = (engine_->getCaps().instancedArrays != nullptr)
        && (batch->visibleInstances.find(subMesh->_id)
            != batch->visibleInstances.end());

    if (isReady(subMesh, hardwareInstancedRendering)) {
      Effect* effect = _volumetricLightScatteringPass;
      if (_mesh == mesh) {
        effect = subMesh->getMaterial()->getEffect();
      }

      engine_->enableEffect(effect);
      _mesh->_bind(subMesh, effect, Material::TriangleFillMode);

      if (_mesh == mesh) {
        subMesh->getMaterial()->bind(_mesh->getWorldMatrix(), _mesh);
      }
      else {
        Material* material = subMesh->getMaterial();

        _volumetricLightScatteringPass->setMatrix("viewProjection",
                                                  scene_->getTransformMatrix());

        // Alpha test
        if (material && material->needAlphaTesting()) {
          BaseTexture* alphaTexture = material->getAlphaTestTexture();

          _volumetricLightScatteringPass->setTexture("diffuseSampler",
                                                     alphaTexture);

          if (alphaTexture) {
            _volumetricLightScatteringPass->setMatrix(
              "diffuseMatrix", *alphaTexture->getTextureMatrix());
          }
        }

        // Bones
        if (mesh->useBones() && mesh->computeBonesUsingShaders) {
          _volumetricLightScatteringPass->setMatrices(
            "mBones", mesh->skeleton()->getTransformMatrices(mesh));
        }
      }

      // Draw
      mesh->_processRendering(subMesh, _volumetricLightScatteringPass,
                              Material::TriangleFillMode, batch,
                              hardwareInstancedRendering,
                              [&](bool /*isInstance*/, Matrix world,
                                  Material* /*effectiveMaterial*/) {
                                effect->setMatrix("world", world);
                              });
    }
  };

  // Render target texture callbacks
  Color3 savedSceneClearColor;
  Color3 sceneClearColor(0.f, 0.f, 0.f);

  _volumetricLightScatteringRTT->onBeforeRenderObservable.add([&]() {
    savedSceneClearColor = scene->clearColor;
    scene->clearColor    = sceneClearColor;
  });

  _volumetricLightScatteringRTT->onAfterRenderObservable.add(
    [&]() { scene->clearColor = savedSceneClearColor; });

  _volumetricLightScatteringRTT->customRenderFunction
    = [&](const std::vector<SubMesh*>& opaqueSubMeshes,
          const std::vector<SubMesh*>& transparentSubMeshes,
          const std::vector<SubMesh*>& alphaTestSubMeshes) {
        Engine* pEngine = scene->getEngine();

        for (const auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        pEngine->setAlphaTesting(true);
        for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }
        pEngine->setAlphaTesting(false);

        if (!transparentSubMeshes.empty()) {
          // Sort sub meshes
          for (auto& submesh : transparentSubMeshes) {
            submesh->_alphaIndex = submesh->getMesh()->alphaIndex;
            submesh->_distanceToCamera
              = submesh->getBoundingInfo()
                  ->boundingSphere
                  .centerWorld.subtract(scene->activeCamera->position)
                  .length();
          }

          std::vector<SubMesh*> sortedArray
            = std_util::slice(transparentSubMeshes, 0,
                              static_cast<int>(transparentSubMeshes.size()));
          std::sort(sortedArray.begin(), sortedArray.end(),
                    [](const SubMesh* a, const SubMesh* b) {
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
          pEngine->setAlphaMode(Engine::ALPHA_COMBINE);
          for (const auto& subMesh : sortedArray) {
            renderSubMesh(subMesh);
          }
          pEngine->setAlphaMode(Engine::ALPHA_DISABLE);
        }
      };
}

void VolumetricLightScatteringPostProcess::_updateMeshScreenCoordinates(
  Scene* scene)
{
  Matrix transform = scene->getTransformMatrix();
  Vector3 meshPosition;

  if (useCustomMeshPosition) {
    meshPosition = customMeshPosition;
  }
  else if (attachedNode) {
    meshPosition = *attachedNode;
  }
  else {
    meshPosition
      = mesh->parent() ? *mesh->getAbsolutePosition() : mesh->position;
  }

  Matrix identityMatrix = Matrix::Identity();
  Vector3 pos
    = Vector3::Project(meshPosition, identityMatrix, transform, _viewPort);

  _screenCoordinates.x = pos.x / static_cast<float>(_viewPort.width);
  _screenCoordinates.y = pos.y / static_cast<float>(_viewPort.height);

  if (invert) {
    _screenCoordinates.y = 1.f - _screenCoordinates.y;
  }
}

Mesh* VolumetricLightScatteringPostProcess::CreateDefaultMesh(
  const std::string& /*name*/, Scene* /*scene*/)
{
  /*Mesh* mesh          = Mesh::CreatePlane(name, 1.f, scene);
  mesh->billboardMode = AbstractMesh::BILLBOARDMODE_ALL;

  Material* material = new StandardMaterial(name + "Material", scene);
  material->setEmissiveColor(Color3(1.f, 1.f, 1.f));

  mesh->material = material;

  return mesh;*/
  return nullptr;
}

} // end of namespace BABYLON
