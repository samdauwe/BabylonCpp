#include <babylon/rendering/outline_renderer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/draw_wrapper.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_fallbacks.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/states/alpha_state.h>

namespace BABYLON {

size_t OutlineRenderer::_Counter = 0ull;

OutlineRenderer::OutlineRenderer(Scene* iScene) : zOffset{1.f}
{
  ISceneComponent::name = OutlineRenderer::name;
  scene                 = iScene;
  _engine               = scene->getEngine();
  _nameForDrawWrapper   = StringTools::printf(
    "%s%zu", Constants::SUBMESH_DRAWWRAPPER_OUTLINERENDERER_PREFIX, OutlineRenderer::_Counter++);
}

OutlineRenderer::~OutlineRenderer() = default;

void OutlineRenderer::addToScene(const OutlineRendererPtr& outlineRenderer)
{
  scene->_addComponent(outlineRenderer);
}

void OutlineRenderer::_register()
{
  scene->_beforeRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_BEFORERENDERINGMESH_OUTLINE, this,
    [this](Mesh* mesh, SubMesh* subMesh, const _InstancesBatchPtr& batch, Effect* /*effect*/) {
      _beforeRenderingMesh(mesh, subMesh, batch);
    });
  scene->_afterRenderingMeshStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDERINGMESH_OUTLINE, this,
    [this](Mesh* mesh, SubMesh* subMesh, const _InstancesBatchPtr& batch, Effect* /*effect*/) {
      _afterRenderingMesh(mesh, subMesh, batch);
    });
}

void OutlineRenderer::rebuild()
{
  // Nothing to do here.
}

void OutlineRenderer::dispose()
{
  // Nothing to do here.
}

void OutlineRenderer::render(SubMesh* subMesh, const _InstancesBatchPtr& batch, bool useOverlay)
{
  auto engine = scene->getEngine();

  bool hardwareInstancedRendering
    = (engine->getCaps().instancedArrays
       && (batch->visibleInstances.find(subMesh->_id) != batch->visibleInstances.end())
       && (!batch->visibleInstances[subMesh->_id].empty()))
      || subMesh->getRenderingMesh()->hasThinInstances();

  if (!isReady(subMesh, hardwareInstancedRendering)) {
    return;
  }

  auto ownerMesh = subMesh->getMesh();
  auto replacementMesh
    = ownerMesh->_internalAbstractMeshDataInfo._actAsRegularMesh ? ownerMesh : nullptr;
  auto renderingMesh = subMesh->getRenderingMesh();
  auto effectiveMesh = replacementMesh ? replacementMesh : renderingMesh;
  auto material      = subMesh->getMaterial();

  if (!material || !scene->activeCamera()) {
    return;
  }

  const auto drawWrapper = subMesh->_getDrawWrapper(_nameForDrawWrapper);
  const auto effect      = DrawWrapper::GetEffect(drawWrapper);

  engine->enableEffect(drawWrapper);

  // Logarithmic depth
  if (material->useLogarithmicDepth()) {
    effect->setFloat("logarithmicDepthConstant",
                     2.f / (std::log(scene->activeCamera()->maxZ + 1.f) / Math::LN2));
  }

  effect->setFloat("offset", useOverlay ? 0.f : renderingMesh->outlineWidth);
  effect->setColor4("color", useOverlay ? renderingMesh->overlayColor : renderingMesh->outlineColor,
                    useOverlay ? renderingMesh->overlayAlpha : material->alpha());
  effect->setMatrix("viewProjection", scene->getTransformMatrix());
  effect->setMatrix("world", effectiveMesh->getWorldMatrix());

  // Bones
  if (renderingMesh->useBones() && renderingMesh->computeBonesUsingShaders()
      && renderingMesh->skeleton()) {
    effect->setMatrices("mBones",
                        renderingMesh->skeleton()->getTransformMatrices(renderingMesh.get()));
  }

  if (renderingMesh->morphTargetManager()
      && renderingMesh->morphTargetManager()->isUsingTextureForTargets()) {
    renderingMesh->morphTargetManager()->_bind(effect);
  }

  // Morph targets
  MaterialHelper::BindMorphTargetParameters(renderingMesh.get(), effect.get());

  renderingMesh->_bind(subMesh, effect, material->fillMode());

  // Alpha test
  if (material && material->needAlphaTesting()) {
    auto alphaTexture = material->getAlphaTestTexture();
    if (alphaTexture) {
      effect->setTexture("diffuseSampler", alphaTexture);
      effect->setMatrix("diffuseMatrix", *alphaTexture->getTextureMatrix());
    }
  }

  engine->setZOffset(-zOffset);

  renderingMesh->_processRendering(
    effectiveMesh.get(), subMesh, effect, static_cast<int>(material->fillMode()), batch,
    hardwareInstancedRendering,
    [effect](bool, const Matrix& world, Material*) -> void { effect->setMatrix("world", world); });

  engine->setZOffset(0.f);
}

bool OutlineRenderer::isReady(SubMesh* subMesh, bool useInstances)
{
  std::vector<std::string> defines;
  std::vector<std::string> attribs{VertexBuffer::PositionKind, VertexBuffer::NormalKind};

  const auto subMeshEffect = subMesh->_getDrawWrapper(_nameForDrawWrapper, true);

  auto& effect        = subMeshEffect->effect;
  auto& cachedDefines = subMeshEffect->defines;

  const auto mesh     = subMesh->getMesh();
  const auto material = subMesh->getMaterial();

  if (material) {
    // Alpha test
    if (material->needAlphaTesting()) {
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

    // Logarithmic depth
    if (material->useLogarithmicDepth()) {
      defines.emplace_back("#define LOGARITHMICDEPTH");
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
      + std::to_string((mesh->skeleton() ? mesh->skeleton()->bones.size() + 1 : 0)));
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Morph targets
  auto morphTargetManager  = std::static_pointer_cast<Mesh>(mesh)->morphTargetManager();
  auto numMorphInfluencers = 0ull;
  if (morphTargetManager) {
    if (morphTargetManager->numInfluencers() > 0) {
      numMorphInfluencers = morphTargetManager->numInfluencers();

      defines.emplace_back("#define MORPHTARGETS");
      defines.emplace_back("#define NUM_MORPH_INFLUENCERS " + std::to_string(numMorphInfluencers));

      if (morphTargetManager->isUsingTextureForTargets()) {
        defines.emplace_back("#define MORPHTARGETS_TEXTURE");
      }

      MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(
        attribs, mesh.get(), static_cast<unsigned int>(numMorphInfluencers));
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
                             "offset",
                             "color",
                             "logarithmicDepthConstant",
                             "morphTargetInfluences",
                             "morphTargetTextureInfo",
                             "morphTargetTextureIndices"};
    options.samplers      = {"diffuseSampler", "morphTargets"};
    options.defines       = std::move(join);
    options.indexParameters
      = {{"maxSimultaneousMorphTargets", static_cast<unsigned>(numMorphInfluencers)}};

    effect = scene->getEngine()->createEffect("outline", options, scene->getEngine());
  }

  subMeshEffect->setEffect(effect, std::get<std::string>(*cachedDefines));

  return effect->isReady();
}

void OutlineRenderer::_beforeRenderingMesh(Mesh* mesh, SubMesh* subMesh,
                                           const _InstancesBatchPtr& batch)
{
  // Outline - step 1
  _savedDepthWrite = _engine->getDepthWrite();
  if (mesh->renderOutline) {
    auto material = subMesh->getMaterial();
    if (material && material->needAlphaBlendingForMesh(*mesh)) {
      _engine->cacheStencilState();
      // Draw only to stencil buffer for the original mesh
      // The resulting stencil buffer will be used so the outline is not visible inside the mesh
      // when the mesh is transparent
      _engine->setDepthWrite(false);
      _engine->setColorWrite(false);
      _engine->setStencilBuffer(true);
      _engine->setStencilOperationPass(Constants::REPLACE);
      _engine->setStencilFunction(Constants::ALWAYS);
      _engine->setStencilMask(OutlineRenderer::_StencilReference);
      _engine->setStencilFunctionReference(OutlineRenderer::_StencilReference);
      render(subMesh, batch, /* This sets offset to 0 */ true);

      _engine->setColorWrite(true);
      _engine->setStencilFunction(Constants::NOTEQUAL);
    }

    // Draw the outline using the above stencil if needed to avoid drawing within the mesh
    _engine->setDepthWrite(false);
    render(subMesh, batch);
    _engine->setDepthWrite(_savedDepthWrite);

    if (material && material->needAlphaBlendingForMesh(*mesh)) {
      _engine->restoreStencilState();
    }
  }
}

void OutlineRenderer::_afterRenderingMesh(Mesh* mesh, SubMesh* subMesh,
                                          const _InstancesBatchPtr& batch)
{
  // Overlay
  if (mesh->renderOverlay) {
    auto currentMode     = _engine->getAlphaMode();
    auto alphaBlendState = _engine->alphaState()->alphaBlend();
    _engine->setAlphaMode(Constants::ALPHA_COMBINE);
    render(subMesh, batch, true);
    _engine->setAlphaMode(currentMode);
    _engine->setDepthWrite(_savedDepthWrite);
    _engine->alphaState()->alphaBlend = alphaBlendState;
  }

  // Outline - step 2
  if (mesh->renderOutline && _savedDepthWrite) {
    _engine->setDepthWrite(true);
    _engine->setColorWrite(false);
    render(subMesh, batch);
    _engine->setColorWrite(true);
  }
}

} // end of namespace BABYLON
