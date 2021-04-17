#include <babylon/rendering/depth_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/draw_wrapper.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/maths/color4.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/rendering/depth_renderer.h>
#include <babylon/rendering/depth_renderer_scene_component.h>

namespace BABYLON {

size_t DepthRenderer::_Counter = 0ull;

DepthRenderer::DepthRenderer(Scene* scene, unsigned int type, const CameraPtr& camera,
                             bool storeNonLinearDepth)
    : isPacked{type == Constants::TEXTURETYPE_UNSIGNED_BYTE}
    , enabled{true}
    , useOnlyInActiveCamera{false}
    , _depthMap{nullptr}
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

  _nameForDrawWrapper = StringTools::printf(
    "%s%zu", Constants::SUBMESH_DRAWWRAPPER_DEPTHRENDERER_PREFIX, DepthRenderer::_Counter++);
  _camera     = camera;
  auto engine = scene->getEngine();

  // Render target
  const auto format = (isPacked || (engine && !engine->_features.supportExtendedTextureFormats)) ?
                        Constants::TEXTUREFORMAT_RGBA :
                        Constants::TEXTUREFORMAT_R;
  _depthMap         = RenderTargetTexture::New(
    "depthMap", RenderTargetSize{engine->getRenderWidth(), engine->getRenderHeight()}, _scene,
    false, true, type, false, TextureConstants::TRILINEAR_SAMPLINGMODE, true, false, false, format);
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
  _depthMap->onClearObservable.add(
    [this](Engine* _engine, EventState&) { _engine->clear(_clearColor, true, true); });

  _depthMap->onBeforeBindObservable.add(
    [engine](RenderTargetTexture* /*texture*/, EventState& /*es*/) -> void {
      engine->_debugPushGroup("depth renderer", 1);
    });

  _depthMap->onAfterUnbindObservable.add(
    [engine](RenderTargetTexture* /*texture*/, EventState& /*es*/) -> void {
      engine->_debugPopGroup(1);
    });

  _depthMap->customRenderFunction
    = [this, engine](const std::vector<SubMesh*>& opaqueSubMeshes,
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

  const auto subMeshEffect = subMesh->_getDrawWrapper(_nameForDrawWrapper, true);

  auto& effect        = subMeshEffect->effect;
  auto& cachedDefines = subMeshEffect->defines;

  std::vector<std::string> attribs{VertexBuffer::PositionKind};

  auto mesh = subMesh->getMesh();

  // Alpha test
  if (material && material->needAlphaTesting() && material->getAlphaTestTexture()) {
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
    defines.emplace_back(
      "#define BonesPerMesh "
      + std::to_string(mesh->skeleton() ? mesh->skeleton()->bones.size() + 1 : 0));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Morph targets
  auto morphTargetManager  = std::static_pointer_cast<Mesh>(mesh)->morphTargetManager();
  auto numMorphInfluencers = 0ull;
  if (morphTargetManager) {
    if (morphTargetManager->numInfluencers > 0) {
      numMorphInfluencers = morphTargetManager->numInfluencers();

      defines.emplace_back("#define MORPHTARGETS");
      defines.emplace_back("#define NUM_MORPH_INFLUENCERS " + std::to_string(numMorphInfluencers));

      if (morphTargetManager->isUsingTextureForTargets()) {
        defines.emplace_back("#define MORPHTARGETS_TEXTURE");
      }

      MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(
        attribs, mesh.get(), static_cast<unsigned>(numMorphInfluencers));
    }
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    MaterialHelper::PushAttributesForInstances(attribs);
    if (subMesh->getRenderingMesh()->hasThinInstances()) {
      defines.emplace_back("#define THIN_INSTANCES");
    }
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
  auto join = StringTools::join(defines, '\n');
  if (cachedDefines && std::holds_alternative<std::string>(*cachedDefines)
      && std::get<std::string>(*cachedDefines) != join) {
    cachedDefines = join;

    IEffectCreationOptions options;
    options.attributes    = std::move(attribs);
    options.uniformsNames = {"world",
                             "mBones",
                             "viewProjection",
                             "diffuseMatrix",
                             "depthValues",
                             "morphTargetInfluences",
                             "morphTargetTextureInfo",
                             "morphTargetTextureIndices"};
    options.samplers      = {"diffuseSampler", "morphTargets"};
    options.defines       = std::move(join);
    options.indexParameters
      = {{"maxSimultaneousMorphTargets", static_cast<unsigned>(numMorphInfluencers)}};

    effect = _scene->getEngine()->createEffect("depth", options, _scene->getEngine());
  }

  subMeshEffect->setEffect(effect, std::get<std::string>(*cachedDefines));

  return effect->isReady();
}

void DepthRenderer::renderSubMesh(SubMesh* subMesh)
{
  auto renderingMesh = subMesh->getRenderingMesh();
  auto effectiveMesh = subMesh->getEffectiveMesh();
  auto scene         = _scene;
  auto engine        = scene->getEngine();
  auto material      = subMesh->getMaterial();

  effectiveMesh->_internalAbstractMeshDataInfo._isActiveIntermediate = false;

  if (!material || effectiveMesh->infiniteDistance() || material->disableDepthWrite
      || subMesh->verticesCount == 0 || subMesh->_renderId == scene->getRenderId()) {
    return;
  }

  // Culling and reverse (right handed system)
  engine->setState(material->backFaceCulling(), 0, false, scene->useRightHandedSystem());

  // Managing instances
  auto batch = renderingMesh->_getInstancesRenderList(subMesh->_id,
                                                      subMesh->getReplacementMesh() != nullptr);

  if (batch->mustReturn) {
    return;
  }

  bool hardwareInstancedRendering
    = (engine->getCaps().instancedArrays
       && (batch->visibleInstances.find(subMesh->_id) != batch->visibleInstances.end()
           && !batch->visibleInstances[subMesh->_id].empty()))
      || renderingMesh->hasThinInstances();

  auto camera = (!_camera) ? _camera : scene->activeCamera();
  if (isReady(subMesh, hardwareInstancedRendering) && camera) {
    subMesh->_renderId = scene->getRenderId();

    const auto drawWrapper = subMesh->_getDrawWrapper(_nameForDrawWrapper);
    const auto effect      = DrawWrapper::GetEffect(drawWrapper);

    engine->enableEffect(drawWrapper);

    renderingMesh->_bind(subMesh, effect, material->fillMode());

    effect->setMatrix("viewProjection", _scene->getTransformMatrix());
    effect->setMatrix("world", effectiveMesh->getWorldMatrix());

    effect->setFloat2("depthValues", camera->minZ, camera->minZ + camera->maxZ);

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      if (alphaTexture) {
        effect->setTexture("diffuseSampler", alphaTexture);
        effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
      }
    }

    // Bones
    if (renderingMesh->useBones() && renderingMesh->computeBonesUsingShaders()
        && renderingMesh->skeleton()) {
      const auto& skeleton = renderingMesh->skeleton();

      if (skeleton->isUsingTextureForMatrices()) {
        const auto boneTexture = skeleton->getTransformMatrixTexture(renderingMesh.get());
        if (!boneTexture) {
          return;
        }

        effect->setTexture("boneSampler", boneTexture);
        effect->setFloat("boneTextureWidth", 4.f * static_cast<float>(skeleton->bones.size() + 1));
      }
      else {
        effect->setMatrices("mBones", skeleton->getTransformMatrices(renderingMesh.get()));
      }
    }

    // Morph targets
    MaterialHelper::BindMorphTargetParameters(renderingMesh.get(), effect.get());
    if (renderingMesh->morphTargetManager()
        && renderingMesh->morphTargetManager()->isUsingTextureForTargets()) {
      renderingMesh->morphTargetManager()->_bind(effect.get());
    }

    // Draw
    renderingMesh->_processRendering(
      effectiveMesh.get(), subMesh, effect, static_cast<int>(material->fillMode()), batch,
      hardwareInstancedRendering,
      [effect](bool /*isInstance*/, Matrix world, Material* /*effectiveMaterial*/) {
        effect->setMatrix("world", world);
      });
  }
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
