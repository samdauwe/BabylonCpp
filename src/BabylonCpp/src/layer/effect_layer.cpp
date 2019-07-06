#include <babylon/layer/effect_layer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/bones/skeleton.h>
#include <babylon/core/string.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/engines/scene_component_constants.h>
#include <babylon/layer/effect_layer_scene_component.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/material_defines.h>
#include <babylon/materials/material_helper.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/_instances_batch.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/misc/tools.h>
#include <babylon/morph/morph_target_manager.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

EffectLayer::EffectLayer(const std::string& iName, Scene* scene)
    : name{iName}
    , isEnabled{true}
    , camera{this, &EffectLayer::get_camera}
    , renderingGroupId{this, &EffectLayer::get_renderingGroupId}
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

  _engine  = scene->getEngine();
  _maxSize = _engine->getCaps().maxTextureSize;

  // Generate Buffers
  _generateIndexBuffer();
  _generateVertexBuffer();
}

EffectLayer::~EffectLayer()
{
}

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

  auto vertexBuffer = std::make_shared<VertexBuffer>(
    _engine, vertices, VertexBuffer::PositionKind, false, false, 2);
  _vertexBuffers[VertexBuffer::PositionKind] = std::move(vertexBuffer);
}

void EffectLayer::_setMainTextureSize()
{
  if (_effectLayerOptions.mainTextureFixedSize.has_value()) {
    _mainTextureDesiredSize.width  = *_effectLayerOptions.mainTextureFixedSize;
    _mainTextureDesiredSize.height = *_effectLayerOptions.mainTextureFixedSize;
  }
  else {
    _mainTextureDesiredSize.width = static_cast<int>(
      _engine->getRenderWidth() * _effectLayerOptions.mainTextureRatio);
    _mainTextureDesiredSize.height = static_cast<int>(
      _engine->getRenderHeight() * _effectLayerOptions.mainTextureRatio);

    _mainTextureDesiredSize.width
      = _engine->needPOTTextures() ?
          Tools::GetExponentOfTwo(_mainTextureDesiredSize.width, _maxSize) :
          _mainTextureDesiredSize.width;
    _mainTextureDesiredSize.height
      = _engine->needPOTTextures() ?
          Tools::GetExponentOfTwo(_mainTextureDesiredSize.height, _maxSize) :
          _mainTextureDesiredSize.height;
  }

  _mainTextureDesiredSize.width
    = static_cast<int>(std::floor(_mainTextureDesiredSize.width));
  _mainTextureDesiredSize.height
    = static_cast<int>(std::floor(_mainTextureDesiredSize.height));
}

void EffectLayer::_createMainTexture()
{
  _mainTexture = RenderTargetTexture::New(
    "HighlightLayerMainRTT",
    ISize{_mainTextureDesiredSize.width, _mainTextureDesiredSize.height},
    _scene, false, true, EngineConstants::TEXTURETYPE_UNSIGNED_INT);
  _mainTexture->activeCamera              = _effectLayerOptions.camera;
  _mainTexture->wrapU                     = TextureConstants::CLAMP_ADDRESSMODE;
  _mainTexture->wrapV                     = TextureConstants::CLAMP_ADDRESSMODE;
  _mainTexture->anisotropicFilteringLevel = 1;
  _mainTexture->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _mainTexture->renderParticles      = false;
  _mainTexture->renderList           = {};
  _mainTexture->ignoreCameraViewport = true;

  // Custom render function
  _mainTexture->customRenderFunction
    = [this](const std::vector<SubMesh*>& opaqueSubMeshes,
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

        for (const auto& transparentSubMesh : transparentSubMeshes) {
          _renderSubMesh(transparentSubMesh);
        }
      };

  _mainTexture->onClearObservable.add(
    [this](Engine* engine, EventState& /*es*/) {
      engine->clear(neutralColor, true, true, true);
    });
}

bool EffectLayer::_isReady(SubMesh* subMesh, bool useInstances,
                           const BaseTexturePtr& emissiveTexture)
{
  auto material = subMesh->getMaterial();

  if (!material) {
    return false;
  }

  if (!material->isReady(subMesh->getMesh().get(), useInstances)) {
    return false;
  }

  std::vector<std::string> defines = {};

  std::vector<std::string> attribs = {VertexBuffer::PositionKind};

  auto mesh = subMesh->getMesh();
  auto uv1  = false;
  auto uv2  = false;

  // Alpha test
  if (material && material->needAlphaTesting()) {
    auto alphaTexture = material->getAlphaTestTexture();
    if (alphaTexture) {
      defines.emplace_back("#define ALPHATEST");
      if (mesh->isVerticesDataPresent(VertexBuffer::UV2Kind)
          && alphaTexture->coordinatesIndex == 1) {
        defines.emplace_back("#define DIFFUSEUV2");
        uv2 = true;
      }
      else if (mesh->isVerticesDataPresent(VertexBuffer::UVKind)) {
        defines.emplace_back("#define DIFFUSEUV1");
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

  if (uv1) {
    attribs.emplace_back(VertexBuffer::UVKind);
    defines.emplace_back("#define UV1");
  }
  if (uv2) {
    attribs.emplace_back(VertexBuffer::UV2Kind);
    defines.emplace_back("#define UV2");
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
      + std::to_string(mesh->skeleton() ? (mesh->skeleton()->bones.size() + 1) :
                                          0));
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
        defines.emplace_back("#define NUM_MORPH_INFLUENCERS "
                             + std::to_string(morphInfluencers));
        MaterialDefines iDefines;
        iDefines.intDef["NUM_MORPH_INFLUENCERS"] = morphInfluencers;
        MaterialHelper::PrepareAttributesForMorphTargets(attribs, mesh.get(),
                                                         iDefines);
      }
    }
  }

  // Instances
  if (useInstances) {
    defines.emplace_back("#define INSTANCES");
    attribs.emplace_back(VertexBuffer::World0Kind);
    attribs.emplace_back(VertexBuffer::World1Kind);
    attribs.emplace_back(VertexBuffer::World2Kind);
    attribs.emplace_back(VertexBuffer::World3Kind);
  }

  // Get correct effect
  auto join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines = join;

    EffectCreationOptions effectCreationOptions;
    effectCreationOptions.attributes = std::move(attribs);
    effectCreationOptions.defines    = std::move(join);
    effectCreationOptions.indexParameters
      = {{"maxSimultaneousMorphTargets", morphInfluencers}};
    effectCreationOptions.uniformsNames
      = {"world", "mBones",         "viewProjection",       "diffuseMatrix",
         "color", "emissiveMatrix", "morphTargetInfluences"};
    effectCreationOptions.samplers = {"diffuseSampler", "emissiveSampler"};

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
  engine->bindBuffers(_vertexBuffers, _indexBuffer.get(), currentEffect);

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
  if (renderingGroupId() == -1 || mesh->renderingGroupId == renderingGroupId) {
    return true;
  }
  return false;
}

bool EffectLayer::shouldRender() const
{
  return isEnabled && _shouldRender;
}

bool EffectLayer::_shouldRenderMesh(const MeshPtr& /*mesh*/) const
{
  return true;
}

bool EffectLayer::_shouldRenderEmissiveTextureForMesh(Mesh* /*mesh*/) const
{
  return true;
}

void EffectLayer::_renderSubMesh(SubMesh* subMesh)
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
  if (material->needAlphaBlendingForMesh(*mesh)) {
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
  if (!_shouldRenderMesh(mesh)) {
    return;
  }

  auto hardwareInstancedRendering
    = (engine->getCaps().instancedArrays)
      && (stl_util::contains(batch->visibleInstances, subMesh->_id))
      && (!batch->visibleInstances[subMesh->_id].empty());

  _setEmissiveTextureAndColor(mesh, subMesh, material);

  if (_isReady(subMesh, hardwareInstancedRendering,
               _emissiveTextureAndColor.texture)) {
    engine->enableEffect(_effectLayerMapGenerationEffect);
    mesh->_bind(subMesh, _effectLayerMapGenerationEffect,
                Material::TriangleFillMode);

    _effectLayerMapGenerationEffect->setMatrix("viewProjection",
                                               scene->getTransformMatrix());

    _effectLayerMapGenerationEffect->setFloat4(
      "color",                          //
      _emissiveTextureAndColor.color.r, //
      _emissiveTextureAndColor.color.g, //
      _emissiveTextureAndColor.color.b, //
      _emissiveTextureAndColor.color.a  //
    );

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      if (alphaTexture) {
        _effectLayerMapGenerationEffect->setTexture("diffuseSampler",
                                                    alphaTexture);
        auto textureMatrix = alphaTexture->getTextureMatrix();

        if (textureMatrix) {
          _effectLayerMapGenerationEffect->setMatrix("diffuseMatrix",
                                                     *textureMatrix);
        }
      }
    }

    // Glow emissive only
    if (_emissiveTextureAndColor.texture) {
      _effectLayerMapGenerationEffect->setTexture(
        "emissiveSampler", _emissiveTextureAndColor.texture);
      _effectLayerMapGenerationEffect->setMatrix(
        "emissiveMatrix",
        *_emissiveTextureAndColor.texture->getTextureMatrix());
    }

    // Bones
    if (mesh->useBones() && mesh->computeBonesUsingShaders()
        && mesh->skeleton()) {
      _effectLayerMapGenerationEffect->setMatrices(
        "mBones", mesh->skeleton()->getTransformMatrices(mesh.get()));
    }

    // Morph targets
    MaterialHelper::BindMorphTargetParameters(mesh.get(),
                                              _effectLayerMapGenerationEffect);

    // Draw
    mesh->_processRendering(
      subMesh, _effectLayerMapGenerationEffect, Material::TriangleFillMode,
      batch, hardwareInstancedRendering,
      [&](bool /*isInstance*/, const Matrix& world,
          Material* /*effectiveMaterial*/) {
        _effectLayerMapGenerationEffect->setMatrix("world", world);
      });
  }
  else {
    // Need to reset refresh rate of the main map
    _mainTexture->resetRefreshCounter();
  }
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
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer = nullptr;
  }

  // Clean textures and post processes
  _disposeTextureAndPostProcesses();

  // Remove from scene
  _scene->effectLayers.erase(
    std::remove_if(_scene->effectLayers.begin(), _scene->effectLayers.end(),
                   [this](const EffectLayerPtr& effectLayer) {
                     return effectLayer.get() == this;
                   }),
    _scene->effectLayers.end());

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBeforeRenderMainTextureObservable.clear();
  onBeforeComposeObservable.clear();
  onAfterComposeObservable.clear();
  onSizeChangedObservable.clear();
}

std::string EffectLayer::getClassName() const
{
  return "EffectLayer";
}

EffectLayer* EffectLayer::Parse(const json& /*parsedEffectLayer*/,
                                Scene* /*scene*/,
                                const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
