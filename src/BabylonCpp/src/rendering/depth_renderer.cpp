#include <babylon/rendering/depth_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/color4.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/depth_renderer_scene_component.h>

namespace BABYLON {

DepthRenderer::DepthRenderer(Scene* scene, unsigned int type,
                             const CameraPtr& camera, bool storeNonLinearDepth)
    : isPacked{type == Constants::TEXTURETYPE_UNSIGNED_BYTE}
    , useOnlyInActiveCamera{false}
    , _depthMap{nullptr}
    , _effect{nullptr}
{
  _scene               = scene;
  _storeNonLinearDepth = storeNonLinearDepth;
  if (isPacked) {
    _clearColor = Color4(1.f, 1.f, 1.f, 1.f);
  }
  else {
    _clearColor = Color4(1.f, 0.f, 0.f, 1.f);
  }

  // Register the G Buffer component to the scene.
  auto component = std::static_pointer_cast<DepthRendererSceneComponent>(
    scene->_getComponent(SceneComponentConstants::NAME_DEPTHRENDERER));
  if (!component) {
    component = DepthRendererSceneComponent::New(scene);
    scene->_addComponent(component);
  }

  _camera     = camera;
  auto engine = scene->getEngine();

  // Render target
  const auto format = (isPacked || engine->webGLVersion() == 1.f) ?
                        Constants::TEXTUREFORMAT_RGBA :
                        Constants::TEXTUREFORMAT_R;
  _depthMap = RenderTargetTexture::New(
    "depthMap", ISize{engine->getRenderWidth(), engine->getRenderHeight()},
    _scene, false, true, type, false, TextureConstants::TRILINEAR_SAMPLINGMODE,
    true, false, false, format);
  _depthMap->wrapU           = TextureConstants::CLAMP_ADDRESSMODE;
  _depthMap->wrapV           = TextureConstants::CLAMP_ADDRESSMODE;
  _depthMap->refreshRate     = 1;
  _depthMap->renderParticles = false;
  _depthMap->renderList().clear();

  // Camera to get depth map from to support multiple concurrent cameras
  _depthMap->activeCamera           = _camera;
  _depthMap->ignoreCameraViewport   = true;
  _depthMap->useCameraPostProcesses = false;

  // set default depth value to 1.0 (far away)
  _depthMap->onClearObservable.add([this](Engine* _engine, EventState&) {
    _engine->clear(_clearColor, true, true);
  });

  // Custom render function
  auto renderSubMesh = [this](SubMesh* subMesh) {
    auto mesh     = subMesh->getRenderingMesh();
    auto scene    = _scene;
    auto engine   = scene->getEngine();
    auto material = subMesh->getMaterial();

    mesh->_internalAbstractMeshDataInfo._isActiveIntermediate = false;

    if (!material) {
      return;
    }

    // Culling and reverse (right handed system)
    engine->setState(material->backFaceCulling(), 0, false,
                     scene->useRightHandedSystem());

    // Managing instances
    auto batch = mesh->_getInstancesRenderList(subMesh->_id);

    if (batch->mustReturn) {
      return;
    }

    bool hardwareInstancedRendering
      = engine->getCaps().instancedArrays
        && (batch->visibleInstances.find(subMesh->_id)
            != batch->visibleInstances.end());

    auto camera = (!_camera) ? _camera : scene->activeCamera;
    if (isReady(subMesh, hardwareInstancedRendering) && camera) {
      engine->enableEffect(_effect);
      mesh->_bind(subMesh, _effect, Material::TriangleFillMode);

      _effect->setMatrix("viewProjection", _scene->getTransformMatrix());

      _effect->setFloat2("depthValues", camera->minZ,
                         camera->minZ + camera->maxZ);

      // Alpha test
      if (material && material->needAlphaTesting()) {
        auto alphaTexture = material->getAlphaTestTexture();
        if (alphaTexture) {
          _effect->setTexture("diffuseSampler", alphaTexture);
          _effect->setMatrix("diffuseMatrix",
                             *alphaTexture->getTextureMatrix());
        }
      }

      // Bones
      if (mesh->useBones() && mesh->computeBonesUsingShaders()
          && mesh->skeleton()) {
        _effect->setMatrices(
          "mBones", mesh->skeleton()->getTransformMatrices(mesh.get()));
      }

      // Morph targets
      MaterialHelper::BindMorphTargetParameters(mesh.get(), _effect);

      // Draw
      mesh->_processRendering(subMesh, _effect, Material::TriangleFillMode,
                              batch, hardwareInstancedRendering,
                              [this](bool /*isInstance*/, Matrix world,
                                     Material* /*effectiveMaterial*/) {
                                _effect->setMatrix("world", world);
                              });
    }
  };

  _depthMap->customRenderFunction
    = [engine,
       renderSubMesh](const std::vector<SubMesh*>& opaqueSubMeshes,
                      const std::vector<SubMesh*>& alphaTestSubMeshes,
                      const std::vector<SubMesh*>& /*transparentSubMeshes*/,
                      const std::vector<SubMesh*>& depthOnlySubMeshes,
                      const std::function<void()>& /*beforeTransparents*/) {
        if (!depthOnlySubMeshes.empty()) {
          engine->setColorWrite(false);
          for (auto& depthOnlySubMesh : depthOnlySubMeshes) {
            renderSubMesh(depthOnlySubMesh);
          }
          engine->setColorWrite(true);
        }

        for (auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        for (auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }
      };
}

DepthRenderer::~DepthRenderer() = default;

bool DepthRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  auto material = subMesh->getMaterial();
  if (material && material->disableDepthWrite) {
    return false;
  }

  std::vector<std::string> defines;

  std::vector<std::string> attribs{VertexBuffer::PositionKind};

  auto mesh = subMesh->getMesh();

  // Alpha test
  if (material && material->needAlphaTesting()
      && material->getAlphaTestTexture()) {
    defines.emplace_back("#define ALPHATEST");
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
    if (mesh->numBoneInfluencers() > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKind);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKind);
    }
    defines.emplace_back("#define NUM_BONE_INFLUENCERS "
                         + std::to_string(mesh->numBoneInfluencers()));
    defines.emplace_back("#define BonesPerMesh "
                         + std::to_string(mesh->skeleton() ?
                                            mesh->skeleton()->bones.size() + 1 :
                                            0));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Morph targets
  auto morphTargetManager
    = std::static_pointer_cast<Mesh>(mesh)->morphTargetManager();
  auto numMorphInfluencers = 0ull;
  if (morphTargetManager) {
    if (morphTargetManager->numInfluencers > 0) {
      numMorphInfluencers = morphTargetManager->numInfluencers();

      defines.emplace_back("#define MORPHTARGETS");
      defines.emplace_back("#define NUM_MORPH_INFLUENCERS "
                           + std::to_string(numMorphInfluencers));

      MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(
        attribs, mesh.get(), static_cast<unsigned>(numMorphInfluencers));
    }
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    MaterialHelper::PushAttributesForInstances(attribs);
  }

  // None linear depth
  if (_storeNonLinearDepth) {
    defines.emplace_back("#define NONLINEARDEPTH");
  }

  // Float Mode
  if (isPacked) {
    defines.emplace_back("#define PACKED");
  }

  // Get correct effect
  auto join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    EffectCreationOptions options;
    options.attributes = std::move(attribs);
    options.uniformsNames
      = {"world",         "mBones",      "viewProjection",
         "diffuseMatrix", "depthValues", "morphTargetInfluences"};
    options.samplers        = {"diffuseSampler"};
    options.defines         = std::move(join);
    options.indexParameters = {{"maxSimultaneousMorphTargets",
                                static_cast<unsigned>(numMorphInfluencers)}};

    _effect = _scene->getEngine()->createEffect("depth", options,
                                                _scene->getEngine());
  }

  return _effect->isReady();
}

RenderTargetTexturePtr& DepthRenderer::getDepthMap()
{
  return _depthMap;
}

void DepthRenderer::dispose()
{
  _depthMap->dispose();
}

} // end of namespace BABYLON
