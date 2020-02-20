#include <babylon/layers/effect_layer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/layers/effect_layer_scene_component.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/raw_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/string_tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

EffectLayer::EffectLayer(const std::string& iName, Scene* scene)
    : name{iName}
    , isEnabled{true}
    , camera{this, &EffectLayer::get_camera}
    , renderingGroupId{this, &EffectLayer::get_renderingGroupId, &EffectLayer::set_renderingGroupId}
    , _mainTexture{nullptr}
    , _maxSize{0}
    , _mainTextureDesiredSize{ISize{0, 0}}
    , _shouldRender{true}
    , _indexBuffer{nullptr}
    , _effectLayerMapGenerationEffect{nullptr}
    , _mergeEffect{nullptr}
{
  _scene         = scene ? scene : Engine::LastCreatedScene();
  auto component = std::static_pointer_cast<EffectLayerSceneComponent>(
    _scene->_getComponent(SceneComponentConstants::NAME_EFFECTLAYER));
  if (!component) {
    component = EffectLayerSceneComponent::New(_scene);
    _scene->_addComponent(component);
  }

  _engine  = _scene->getEngine();
  _maxSize = _engine->getCaps().maxTextureSize;

  // Generate Buffers
  _generateIndexBuffer();
  _generateVertexBuffer();
}

EffectLayer::~EffectLayer() = default;

void EffectLayer::addToScene(const EffectLayerPtr& newEffectLayer)
{
  _scene->effectLayers.emplace_back(newEffectLayer);
}

CameraPtr& EffectLayer::get_camera()
{
  return _effectLayerOptions.camera;
}

int EffectLayer::get_renderingGroupId() const
{
  return _effectLayerOptions.renderingGroupId;
}

void EffectLayer::set_renderingGroupId(int iRenderingGroupId)
{
  _effectLayerOptions.renderingGroupId = iRenderingGroupId;
}

void EffectLayer::_init(const IEffectLayerOptions& options)
{
  _effectLayerOptions = options;

  _setMainTextureSize();
  _createMainTexture();
  _createTextureAndPostProcesses();
  _mergeEffect = _createMergeEffect();
}

void EffectLayer::_generateIndexBuffer()
{
  // Indices
  IndicesArray indices = {
    0, //
    1, //
    2, //
    0, //
    2, //
    3  //
  };

  _indexBuffer = _engine->createIndexBuffer(indices);
}

void EffectLayer::_generateVertexBuffer()
{
  // VBO
  Float32Array vertices = {
    1,  1,  //
    -1, 1,  //
    -1, -1, //
    1,  -1  //
  };

  auto vertexBuffer = std::make_shared<VertexBuffer>(_engine, vertices, VertexBuffer::PositionKind,
                                                     false, false, 2);
  _vertexBuffers[VertexBuffer::PositionKind] = std::move(vertexBuffer);
}

void EffectLayer::_setMainTextureSize()
{
  if (_effectLayerOptions.mainTextureFixedSize.has_value()) {
    _mainTextureDesiredSize.width  = *_effectLayerOptions.mainTextureFixedSize;
    _mainTextureDesiredSize.height = *_effectLayerOptions.mainTextureFixedSize;
  }
  else {
    _mainTextureDesiredSize.width
      = static_cast<int>(_engine->getRenderWidth() * _effectLayerOptions.mainTextureRatio);
    _mainTextureDesiredSize.height
      = static_cast<int>(_engine->getRenderHeight() * _effectLayerOptions.mainTextureRatio);

    _mainTextureDesiredSize.width
      = _engine->needPOTTextures() ?
          Engine::GetExponentOfTwo(_mainTextureDesiredSize.width, _maxSize) :
          _mainTextureDesiredSize.width;
    _mainTextureDesiredSize.height
      = _engine->needPOTTextures() ?
          Engine::GetExponentOfTwo(_mainTextureDesiredSize.height, _maxSize) :
          _mainTextureDesiredSize.height;
  }

  _mainTextureDesiredSize.width  = static_cast<int>(std::floor(_mainTextureDesiredSize.width));
  _mainTextureDesiredSize.height = static_cast<int>(std::floor(_mainTextureDesiredSize.height));
}

void EffectLayer::_createMainTexture()
{
  _mainTexture = RenderTargetTexture::New(
    "HighlightLayerMainRTT", ISize{_mainTextureDesiredSize.width, _mainTextureDesiredSize.height},
    _scene, false, true, Constants::TEXTURETYPE_UNSIGNED_INT);
  _mainTexture->activeCamera              = _effectLayerOptions.camera;
  _mainTexture->wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  _mainTexture->wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;
  _mainTexture->anisotropicFilteringLevel = 1;
  _mainTexture->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _mainTexture->renderParticles      = false;
  _mainTexture->renderList           = {};
  _mainTexture->ignoreCameraViewport = true;

  // Custom render function
  _mainTexture->customRenderFunction = [this](const std::vector<SubMesh*>& opaqueSubMeshes,
                                              const std::vector<SubMesh*>& alphaTestSubMeshes,
                                              const std::vector<SubMesh*>& transparentSubMeshes,
                                              const std::vector<SubMesh*>& depthOnlySubMeshes,
                                              const std::function<void()>& /*beforeTransparents*/) {
    onBeforeRenderMainTextureObservable.notifyObservers(this);

    auto engine = _scene->getEngine();

    if (!depthOnlySubMeshes.empty()) {
      engine->setColorWrite(false);
      for (const auto& depthOnlySubMesh : depthOnlySubMeshes) {
        _renderSubMesh(depthOnlySubMesh);
      }
      engine->setColorWrite(true);
    }

    for (const auto& opaqueSubMesh : opaqueSubMeshes) {
      _renderSubMesh(opaqueSubMesh);
    }

    for (const auto& alphaTestSubMesh : alphaTestSubMeshes) {
      _renderSubMesh(alphaTestSubMesh);
    }

    const auto previousAlphaMode = engine->getAlphaMode();

    for (const auto& transparentSubMesh : transparentSubMeshes) {
      _renderSubMesh(transparentSubMesh, true);
    }

    engine->setAlphaMode(previousAlphaMode);
  };

  _mainTexture->onClearObservable.add(
    [this](Engine* engine, EventState& /*es*/) { engine->clear(neutralColor, true, true, true); });
}

void EffectLayer::_addCustomEffectDefines(std::vector<std::string>& /*defines*/)
{
  // Nothing to add by default.
}

bool EffectLayer::_isReady(SubMesh* subMesh, bool useInstances,
                           const BaseTexturePtr& emissiveTexture)
{
  auto material = subMesh->getMaterial();

  if (!material) {
    return false;
  }

  if (!material->isReadyForSubMesh(subMesh->getMesh().get(), subMesh, useInstances)) {
    return false;
  }

  std::vector<std::string> defines = {};

  std::vector<std::string> attribs = {VertexBuffer::PositionKind};

  auto mesh = subMesh->getMesh();
  auto uv1  = false;
  auto uv2  = false;

  // Diffuse
  if (material) {
    const auto needAlphaTest = material->needAlphaTesting();

    const auto& diffuseTexture  = material->getAlphaTestTexture();
    const auto standardMaterial = std::static_pointer_cast<StandardMaterial>(material);
    const auto pbrMaterial      = std::static_pointer_cast<PBRMaterial>(material);
    const auto needAlphaBlendFromDiffuse
      = diffuseTexture && diffuseTexture->hasAlpha
        && ((standardMaterial && standardMaterial->useAlphaFromDiffuseTexture)
            || (pbrMaterial && pbrMaterial->useAlphaFromAlbedoTexture()));

    if (diffuseTexture && (needAlphaTest || needAlphaBlendFromDiffuse)) {
      defines.emplace_back("#define DIFFUSE");
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)
          && diffuseTexture->coordinatesIndex == 1) {
        defines.emplace_back("#define DIFFUSEUV2");
        uv2 = true;
      }
      else if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        defines.emplace_back("#define DIFFUSEUV1");
        uv1 = true;
      }

      if (needAlphaTest) {
        defines.emplace_back("#define ALPHATEST");
        defines.emplace_back("#define ALPHATESTVALUE 0.4");
      }
    }

    auto opacityTexture = standardMaterial ? standardMaterial->opacityTexture() :
                                             pbrMaterial ? pbrMaterial->opacityTexture() : nullptr;
    if (opacityTexture) {
      defines.emplace_back("#define OPACITY");
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)
          && opacityTexture->coordinatesIndex == 1) {
        defines.emplace_back("#define OPACITYUV2");
        uv2 = true;
      }
      else if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        defines.emplace_back("#define OPACITYUV1");
        uv1 = true;
      }
    }
  }

  // Emissive
  if (emissiveTexture) {
    defines.emplace_back("#define EMISSIVE");
    if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)
        && emissiveTexture->coordinatesIndex == 1) {
      defines.emplace_back("#define EMISSIVEUV2");
      uv2 = true;
    }
    else if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
      defines.emplace_back("#define EMISSIVEUV1");
      uv1 = true;
    }
  }

  // Vertex
  if (mesh->isVerticesDataPresent(VertexBuffer::ColorKind) && mesh->hasVertexAlpha) {
    attribs.emplace_back(VertexBuffer::ColorKind);
    defines.emplace_back("#define VERTEXALPHA");
  }

  if (uv1) {
    attribs.emplace_back(VertexBuffer::UVKind);
    defines.emplace_back("#define UV1");
  }
  if (uv2) {
    attribs.emplace_back(VertexBuffer::UV2Kind);
    defines.emplace_back("#define UV2");
  }

  // Bones
  auto fallbacks = std::make_unique<EffectFallbacks>();
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
      + std::to_string(mesh->skeleton() ? (mesh->skeleton()->bones.size() + 1) : 0));

    auto& skeleton = mesh->skeleton();
    if (skeleton && skeleton->isUsingTextureForMatrices()) {
      defines.emplace_back("#define BONETEXTURE");
    }
    else {
      defines.emplace_back("#define BonesPerMesh "
                           + std::to_string(skeleton ? (skeleton->bones.size() + 1) : 0ull));
    }

    if (mesh->numBoneInfluencers > 0) {
      fallbacks->addCPUSkinningFallback(0, mesh.get());
    }
  }
  else {
    defines.emplace_back("#define NUM_BONE_INFLUENCERS 0");
  }

  // Morph targets
  unsigned int morphInfluencers = 0;
  if (auto _mesh = std::static_pointer_cast<Mesh>(mesh)) {
    auto manager = _mesh->morphTargetManager();
    if (manager) {
      if (manager->numInfluencers > 0) {
        defines.emplace_back("#define MORPHTARGETS");
        morphInfluencers = static_cast<unsigned>(manager->numInfluencers);
        defines.emplace_back("#define NUM_MORPH_INFLUENCERS " + std::to_string(morphInfluencers));
        MaterialDefines iDefines;
        iDefines.intDef["NUM_MORPH_INFLUENCERS"] = morphInfluencers;
        MaterialHelper::PrepareAttributesForMorphTargetsInfluencers(attribs, mesh.get(),
                                                                    morphInfluencers);
      }
    }
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    MaterialHelper::PushAttributesForInstances(attribs);
  }

  _addCustomEffectDefines(defines);

  // Get correct effect
  auto join = StringTools::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    IEffectCreationOptions effectCreationOptions;
    effectCreationOptions.attributes      = std::move(attribs);
    effectCreationOptions.defines         = std::move(join);
    effectCreationOptions.fallbacks       = std::move(fallbacks);
    effectCreationOptions.indexParameters = {{"maxSimultaneousMorphTargets", morphInfluencers}};
    effectCreationOptions.uniformsNames   = {"world",
                                           "mBones",
                                           "viewProjection",
                                           "glowColor",
                                           "morphTargetInfluences",
                                           "boneTextureWidth",
                                           "diffuseMatrix",
                                           "emissiveMatrix",
                                           "opacityMatrix",
                                           "opacityIntensity"};
    effectCreationOptions.samplers
      = {"diffuseSampler", "emissiveSampler", "opacitySampler", "boneSampler"};

    _effectLayerMapGenerationEffect = _scene->getEngine()->createEffect(
      "glowMapGeneration", effectCreationOptions, _scene->getEngine());
  }

  return _effectLayerMapGenerationEffect->isReady();
}

void EffectLayer::render()
{
  auto& currentEffect = _mergeEffect;

  // Check
  if (!currentEffect->isReady()) {
    return;
  }

  for (const auto& _postProcess : _postProcesses) {
    if (!_postProcess->isReady()) {
      return;
    }
  }

  auto engine = _scene->getEngine();

  onBeforeComposeObservable.notifyObservers(this);

  // Render
  engine->enableEffect(currentEffect);
  engine->setState(false);

  // VBOs
  engine->bindBuffers(_vertexBuffers, _indexBuffer, currentEffect);

  // Cache
  auto previousAlphaMode = engine->getAlphaMode();

  // Go Blend.
  engine->setAlphaMode(_effectLayerOptions.alphaBlendingMode);

  // Blends the map on the main canvas.
  _internalRender(currentEffect);

  // Restore Alpha
  engine->setAlphaMode(previousAlphaMode);

  onAfterComposeObservable.notifyObservers(this);

  // Handle size changes.
  const auto size = _mainTexture->getSize();
  _setMainTextureSize();
  if (size.width != _mainTextureDesiredSize.width
      || size.height != _mainTextureDesiredSize.height) {
    // Recreate RTT and post processes on size change.
    onSizeChangedObservable.notifyObservers(this);
    _disposeTextureAndPostProcesses();
    _createMainTexture();
    _createTextureAndPostProcesses();
  }
}

bool EffectLayer::hasMesh(AbstractMesh* mesh) const
{
  return renderingGroupId() == -1 || mesh->renderingGroupId == renderingGroupId;
}

bool EffectLayer::shouldRender() const
{
  return isEnabled && _shouldRender;
}

bool EffectLayer::_shouldRenderMesh(AbstractMesh* /*mesh*/) const
{
  return true;
}

bool EffectLayer::_canRenderMesh(const AbstractMeshPtr& mesh, const MaterialPtr& material) const
{
  return material->needAlphaBlendingForMesh(*mesh);
}

bool EffectLayer::_shouldRenderEmissiveTextureForMesh() const
{
  return true;
}

void EffectLayer::_renderSubMesh(SubMesh* subMesh, bool enableAlphaMode)
{
  if (!shouldRender()) {
    return;
  }

  auto material = subMesh->getMaterial();
  auto mesh     = subMesh->getRenderingMesh();
  auto scene    = _scene;
  auto engine   = scene->getEngine();

  if (!material) {
    return;
  }

  // Do not block in blend mode.
  if (!_canRenderMesh(mesh, material)) {
    return;
  }

  // Culling
  engine->setState(material->backFaceCulling());

  // Managing instances
  auto batch = mesh->_getInstancesRenderList(subMesh->_id);
  if (!batch || batch->mustReturn) {
    return;
  }

  // Early Exit per mesh
  if (!_shouldRenderMesh(mesh.get())) {
    return;
  }

  auto hardwareInstancedRendering = (engine->getCaps().instancedArrays)
                                    && (stl_util::contains(batch->visibleInstances, subMesh->_id))
                                    && (!batch->visibleInstances[subMesh->_id].empty());

  _setEmissiveTextureAndColor(mesh, subMesh, material);

  onBeforeRenderMeshToEffect.notifyObservers(mesh.get());

  if (_useMeshMaterial(mesh)) {
    mesh->render(subMesh, hardwareInstancedRendering);
  }
  else if (_isReady(subMesh, hardwareInstancedRendering, _emissiveTextureAndColor.texture)) {
    engine->enableEffect(_effectLayerMapGenerationEffect);
    mesh->_bind(subMesh, _effectLayerMapGenerationEffect, Material::TriangleFillMode);

    _effectLayerMapGenerationEffect->setMatrix("viewProjection", scene->getTransformMatrix());

    _effectLayerMapGenerationEffect->setFloat4(
      "glowColor", _emissiveTextureAndColor.color.r, _emissiveTextureAndColor.color.g,
      _emissiveTextureAndColor.color.b, _emissiveTextureAndColor.color.a);

    const auto needAlphaTest = material->needAlphaTesting();

    const auto& diffuseTexture  = material->getAlphaTestTexture();
    const auto standardMaterial = std::static_pointer_cast<StandardMaterial>(material);
    const auto pbrMaterial      = std::static_pointer_cast<PBRMaterial>(material);
    const auto needAlphaBlendFromDiffuse
      = diffuseTexture && diffuseTexture->hasAlpha
        && ((standardMaterial && standardMaterial->useAlphaFromDiffuseTexture)
            || (pbrMaterial && pbrMaterial->useAlphaFromAlbedoTexture()));

    if (diffuseTexture && (needAlphaTest || needAlphaBlendFromDiffuse)) {
      _effectLayerMapGenerationEffect->setTexture("diffuseSampler", diffuseTexture);
      const auto& textureMatrix = diffuseTexture->getTextureMatrix();

      if (textureMatrix) {
        _effectLayerMapGenerationEffect->setMatrix("diffuseMatrix", *textureMatrix);
      }
    }

    auto opacityTexture = standardMaterial ? standardMaterial->opacityTexture() :
                                             pbrMaterial ? pbrMaterial->opacityTexture() : nullptr;
    if (opacityTexture) {
      _effectLayerMapGenerationEffect->setTexture("opacitySampler", opacityTexture);
      _effectLayerMapGenerationEffect->setFloat("opacityIntensity", opacityTexture->level);
      const auto& textureMatrix = opacityTexture->getTextureMatrix();
      if (textureMatrix) {
        _effectLayerMapGenerationEffect->setMatrix("opacityMatrix", *textureMatrix);
      }
    }

    // Glow emissive only
    if (_emissiveTextureAndColor.texture) {
      _effectLayerMapGenerationEffect->setTexture("emissiveSampler",
                                                  _emissiveTextureAndColor.texture);
      _effectLayerMapGenerationEffect->setMatrix(
        "emissiveMatrix", *_emissiveTextureAndColor.texture->getTextureMatrix());
    }

    // Bones
    if (mesh->useBones() && mesh->computeBonesUsingShaders() && mesh->skeleton()) {
      auto& skeleton = mesh->skeleton();

      if (skeleton->isUsingTextureForMatrices()) {
        auto boneTexture = skeleton->getTransformMatrixTexture(mesh.get());
        if (!boneTexture) {
          return;
        }

        _effectLayerMapGenerationEffect->setTexture("boneSampler", boneTexture);
        _effectLayerMapGenerationEffect->setFloat(
          "boneTextureWidth", 4.f * static_cast<float>(skeleton->bones.size() + 1));
      }
      else {
        _effectLayerMapGenerationEffect->setMatrices("mBones",
                                                     skeleton->getTransformMatrices((mesh.get())));
      }
    }

    // Morph targets
    MaterialHelper::BindMorphTargetParameters(mesh.get(), _effectLayerMapGenerationEffect);

    // Alpha mode
    if (enableAlphaMode) {
      engine->setAlphaMode(material->alphaMode());
    }

    // Draw
    mesh->_processRendering(
      subMesh, _effectLayerMapGenerationEffect, static_cast<int>(material->fillMode()), batch,
      hardwareInstancedRendering,
      [&](bool /*isInstance*/, const Matrix& world, Material* /*effectiveMaterial*/) {
        _effectLayerMapGenerationEffect->setMatrix("world", world);
      });
  }
  else {
    // Need to reset refresh rate of the main map
    _mainTexture->resetRefreshCounter();
  }

  onAfterRenderMeshToEffect.notifyObservers(mesh.get());
}

bool EffectLayer::_useMeshMaterial(const AbstractMeshPtr& /*mesh*/) const
{
  return false;
}

void EffectLayer::_rebuild()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& vb = _vertexBuffers[VertexBuffer::PositionKind];

    if (vb) {
      vb->_rebuild();
    }
  }

  _generateIndexBuffer();
}

void EffectLayer::_disposeTextureAndPostProcesses()
{
  _mainTexture->dispose();

  for (auto& _postProcess : _postProcesses) {
    if (_postProcess) {
      _postProcess->dispose();
    }
  }
  _postProcesses.clear();

  for (auto& _texture : _textures) {
    if (_texture) {
      _texture->dispose();
    }
  }
  _textures.clear();
}

void EffectLayer::dispose()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& vertexBuffer = _vertexBuffers[VertexBuffer::PositionKind];
    if (vertexBuffer) {
      vertexBuffer->dispose();
      _vertexBuffers[VertexBuffer::PositionKind] = nullptr;
    }
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer);
    _indexBuffer = nullptr;
  }

  // Clean textures and post processes
  _disposeTextureAndPostProcesses();

  // Remove from scene
  stl_util::remove_vector_elements_equal_sharedptr(_scene->effectLayers, this);

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBeforeRenderMainTextureObservable.clear();
  onBeforeComposeObservable.clear();
  onBeforeRenderMeshToEffect.clear();
  onAfterRenderMeshToEffect.clear();
  onAfterComposeObservable.clear();
  onSizeChangedObservable.clear();
}

std::string EffectLayer::getClassName() const
{
  return "EffectLayer";
}

EffectLayer* EffectLayer::Parse(const json& /*parsedEffectLayer*/, Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
