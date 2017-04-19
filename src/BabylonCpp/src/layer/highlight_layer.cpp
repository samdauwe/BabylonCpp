#include <babylon/layer/highlight_layer.h>

#include <babylon/bones/skeleton.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/layer/glow_blur_post_process.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/_instances_batch.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/pass_post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Color4 HighlightLayer::neutralColor             = Color4(0.f, 0.f, 0.f, 0.f);
int HighlightLayer::glowingMeshStencilReference = 0x02;
int HighlightLayer::normalMeshStencilReference  = 0x01;

template <typename... Ts>
HighlightLayer* HighlightLayer::New(Ts&&... args)
{
  auto highlightLayer
    = std_util::make_unique<HighlightLayer>(std::forward<Ts>(args)...);
  highlightLayer->_scene->highlightLayers.emplace_back(highlightLayer);
  return highlightLayer.get();
}

HighlightLayer::HighlightLayer(const std::string& name, Scene* scene)
    : HighlightLayer(name, scene, IHighlightLayerOptions())
{
}

HighlightLayer::HighlightLayer(const std::string& iName, Scene* scene,
                               const IHighlightLayerOptions& options)
    : name{iName}
    , innerGlow{true}
    , outerGlow{true}
    , isEnabled{true}
    , _scene{scene}
    , _engine{scene->getEngine()}
    , _maxSize{_engine->getCaps().maxTextureSize}
    , _shouldRender{false}
{
  // Warn on stencil.
  if (!_engine->isStencilEnable()) {
    BABYLON_LOG_WARN(
      "HighlightLayer",
      "Rendering the Highlight Layer requires the stencil to be active on the "
      "canvas.");
  }

  // Adapt options
  _options.mainTextureRatio     = options.mainTextureRatio;
  _options.blurTextureSizeRatio = options.blurTextureSizeRatio;
  _options.blurHorizontalSize   = options.blurHorizontalSize;
  _options.blurVerticalSize     = options.blurVerticalSize;
  _options.alphaBlendingMode    = options.alphaBlendingMode;

  // VBO
  Float32Array vertices{1, 1, -1, 1, -1, -1, 1, -1};

  _vertexBuffers[VertexBuffer::PositionKindChars]
    = std_util::make_unique<VertexBuffer>(
      _engine, vertices, VertexBuffer::PositionKind, false, false, 2);

  // Indices
  Uint32Array indices{0, 1, 2, 0, 2, 3};

  _indexBuffer = _engine->createIndexBuffer(indices);

  // Effect
  _glowMapMergeEffect
    = _engine->createEffect("glowMapMerge", {VertexBuffer::PositionKindChars},
                            {"offset"}, {"textureSampler"}, "");

  // Render target
  setMainTextureSize();

  // Create Textures and post processes
  createTextureAndPostProcesses();
}

HighlightLayer::~HighlightLayer()
{
}

void HighlightLayer::setBlurHorizontalSize(float value)
{
  _horizontalBlurPostprocess->blurWidth = value;
}

void HighlightLayer::setBlurVerticalSize(float value)
{
  _verticalBlurPostprocess->blurWidth = value;
}

float HighlightLayer::blurHorizontalSize() const
{
  return _horizontalBlurPostprocess->blurWidth;
}

float HighlightLayer::blurVerticalSize() const
{
  return _verticalBlurPostprocess->blurWidth;
}

Camera* HighlightLayer::camera() const
{
  return _options.camera;
}

RenderTargetTexture* HighlightLayer::mainTexture() const
{
  return _mainTexture.get();
}

void HighlightLayer::createTextureAndPostProcesses()
{
  int blurTextureWidth
    = static_cast<int>(static_cast<float>(_mainTextureDesiredSize.width)
                       * _options.blurTextureSizeRatio);
  int blurTextureHeight
    = static_cast<int>(static_cast<float>(_mainTextureDesiredSize.height)
                       * _options.blurTextureSizeRatio);
  blurTextureWidth  = Tools::GetExponentOfTwo(blurTextureWidth, _maxSize);
  blurTextureHeight = Tools::GetExponentOfTwo(blurTextureHeight, _maxSize);

#if 0
  _mainTexture
    = std_util::make_unique<RenderTargetTexture>(new RenderTargetTexture(
      "HighlightLayerMainRTT",
      {_mainTextureDesiredSize.width, _mainTextureDesiredSize.height}, _scene,
      false, true, Engine::TEXTURETYPE_UNSIGNED_INT));
#endif
  _mainTexture->activeCamera              = _options.camera;
  _mainTexture->wrapU                     = Texture::CLAMP_ADDRESSMODE;
  _mainTexture->wrapV                     = Texture::CLAMP_ADDRESSMODE;
  _mainTexture->anisotropicFilteringLevel = 1;
  _mainTexture->updateSamplingMode(Texture::BILINEAR_SAMPLINGMODE);
  _mainTexture->renderParticles = false;
  _mainTexture->renderList.clear();

#if 0
  _blurTexture
    = std_util::make_unique<RenderTargetTexture>(new RenderTargetTexture(
      "HighlightLayerBlurRTT", {blurTextureWidth, blurTextureHeight}, _scene,
      false, true, Engine::TEXTURETYPE_UNSIGNED_INT));
#endif
  _blurTexture->wrapU                     = Texture::CLAMP_ADDRESSMODE;
  _blurTexture->wrapV                     = Texture::CLAMP_ADDRESSMODE;
  _blurTexture->anisotropicFilteringLevel = 16;
  _blurTexture->updateSamplingMode(Texture::TRILINEAR_SAMPLINGMODE);
  _blurTexture->renderParticles = false;

  _downSamplePostprocess = std_util::make_unique<PassPostProcess>(
    "HighlightLayerPPP", _options.blurTextureSizeRatio, nullptr,
    Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine());
  _downSamplePostprocess->onApplyObservable.add([&](Effect* effect) {
    effect->setTexture("textureSampler", _mainTexture.get());
  });

  if (_options.alphaBlendingMode == Engine::ALPHA_COMBINE) {
    _horizontalBlurPostprocess = std_util::make_unique<GlowBlurPostProcess>(
      "HighlightLayerHBP", Vector2(1.f, 0.f), _options.blurHorizontalSize, 1,
      nullptr, Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _horizontalBlurPostprocess->onApplyObservable.add([&](Effect* effect) {
      effect->setFloat2("screenSize", blurTextureWidth, blurTextureHeight);
    });

    _verticalBlurPostprocess = std_util::make_unique<GlowBlurPostProcess>(
      "HighlightLayerVBP", Vector2(0.f, 1.f), _options.blurVerticalSize, 1,
      nullptr, Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _verticalBlurPostprocess->onApplyObservable.add([&](Effect* effect) {
      effect->setFloat2("screenSize", blurTextureWidth, blurTextureHeight);
    });
  }
  else {
    _horizontalBlurPostprocess = std_util::make_unique<GlowBlurPostProcess>(
      "HighlightLayerHBP", Vector2(1.f, 0.f), _options.blurHorizontalSize, 1,
      nullptr, Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _horizontalBlurPostprocess->onApplyObservable.add([&](Effect* effect) {
      effect->setFloat2("screenSize", blurTextureWidth, blurTextureHeight);
    });

    _verticalBlurPostprocess = std_util::make_unique<GlowBlurPostProcess>(
      "HighlightLayerVBP", Vector2(0.f, 1.f), _options.blurVerticalSize, 1,
      nullptr, Texture::BILINEAR_SAMPLINGMODE, _scene->getEngine());
    _verticalBlurPostprocess->onApplyObservable.add([&](Effect* effect) {
      effect->setFloat2("screenSize", blurTextureWidth, blurTextureHeight);
    });
  }

  _mainTexture->onAfterUnbindObservable.add([this]() {
    onBeforeBlurObservable.notifyObservers(this);

    _scene->postProcessManager->directRender(
      {_downSamplePostprocess.get(), _horizontalBlurPostprocess.get(),
       _verticalBlurPostprocess.get()},
      _blurTexture->getInternalTexture());

    onAfterBlurObservable.notifyObservers(this);
  });

  _mainTexture->customRenderFunction
    = [this](const std::vector<SubMesh*>& opaqueSubMeshes,
             const std::vector<SubMesh*>& transparentSubMeshes,
             const std::vector<SubMesh*>& alphaTestSubMeshes) {
        onBeforeRenderMainTextureObservable.notifyObservers(this);

        for (auto& opaqueSubMesh : opaqueSubMeshes) {
          renderSubMesh(opaqueSubMesh);
        }

        for (auto& alphaTestSubMesh : alphaTestSubMeshes) {
          renderSubMesh(alphaTestSubMesh);
        }

        for (auto& transparentSubMesh : transparentSubMeshes) {
          renderSubMesh(transparentSubMesh);
        }
      };

  _mainTexture->onClearObservable.add([this](Engine* engine) {
    engine->clear(HighlightLayer::neutralColor, true, true, true);
  });
}

void HighlightLayer::renderSubMesh(SubMesh* subMesh)
{
  auto mesh   = subMesh->getRenderingMesh();
  auto scene  = _scene;
  auto engine = scene->getEngine();

  // Culling
  engine->setState(subMesh->getMaterial()->backFaceCulling);

  // Managing instances
  auto batch = mesh->_getInstancesRenderList(subMesh->_id);
  if (batch->mustReturn) {
    return;
  }

  // Excluded Mesh
  if (std_util::contains(_excludedMeshes, mesh->uniqueId)) {
    return;
  };

  bool hardwareInstancedRendering
    = (engine->getCaps().instancedArrays != false)
      && std_util::contains(batch->visibleInstances, subMesh->_id)
      && (!batch->visibleInstances[subMesh->_id].empty());

  bool hashighlightLayerMesh   = std_util::contains(_meshes, mesh->uniqueId);
  auto material                = subMesh->getMaterial();
  BaseTexture* emissiveTexture = nullptr;
  if (hashighlightLayerMesh && _meshes[mesh->uniqueId].glowEmissiveOnly
      && material) {
    auto standardMaterial = dynamic_cast<StandardMaterial*>(material);
    if (standardMaterial) {
      emissiveTexture = standardMaterial->emissiveTexture;
    }
  }

  if (isReady(subMesh, hardwareInstancedRendering, emissiveTexture)) {
    engine->enableEffect(_glowMapGenerationEffect);
    mesh->_bind(subMesh, _glowMapGenerationEffect, Material::TriangleFillMode);

    _glowMapGenerationEffect->setMatrix("viewProjection",
                                        scene->getTransformMatrix());
    if (hashighlightLayerMesh) {
      auto& highlightLayerMesh = _meshes[mesh->uniqueId];
      _glowMapGenerationEffect->setFloat4("color", highlightLayerMesh.color.r,
                                          highlightLayerMesh.color.g,
                                          highlightLayerMesh.color.b, 1.f);
    }
    else {
      _glowMapGenerationEffect->setFloat4(
        "color", HighlightLayer::neutralColor.r, HighlightLayer::neutralColor.g,
        HighlightLayer::neutralColor.b, HighlightLayer::neutralColor.a);
    }

    // Alpha test
    if (material && material->needAlphaTesting()) {
      auto alphaTexture = material->getAlphaTestTexture();
      _glowMapGenerationEffect->setTexture("diffuseSampler", alphaTexture);
      _glowMapGenerationEffect->setMatrix("diffuseMatrix",
                                          *alphaTexture->getTextureMatrix());
    }

    // Glow emissive only
    if (emissiveTexture) {
      _glowMapGenerationEffect->setTexture("emissiveSampler", emissiveTexture);
      _glowMapGenerationEffect->setMatrix("emissiveMatrix",
                                          *emissiveTexture->getTextureMatrix());
    }

    // Bones
    if (mesh->useBones() && mesh->computeBonesUsingShaders) {
      _glowMapGenerationEffect->setMatrices(
        "mBones", mesh->skeleton()->getTransformMatrices(mesh));
    }

    // Draw
    mesh->_processRendering(
      subMesh, _glowMapGenerationEffect, Material::TriangleFillMode, batch,
      hardwareInstancedRendering,
      [&](bool /*isInstance*/, Matrix world, Material* /*effectiveMaterial*/) {
        _glowMapGenerationEffect->setMatrix("world", world);
      });
  }
  else {
    // Need to reset refresh rate of the shadowMap
    _mainTexture->resetRefreshCounter();
  }
}

bool HighlightLayer::isReady(SubMesh* subMesh, bool useInstances,
                             BaseTexture* emissiveTexture)
{
  if (!subMesh->getMaterial()->isReady(subMesh->getMesh(), useInstances)) {
    return false;
  }

  std::vector<std::string> defines;
  std::vector<std::string> attribs = {VertexBuffer::PositionKindChars};

  auto mesh     = subMesh->getMesh();
  auto material = subMesh->getMaterial();
  auto uv1      = false;
  auto uv2      = false;

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
    attribs.emplace_back(VertexBuffer::UVKindChars);
    defines.emplace_back("#define UV1");
  }
  if (uv2) {
    attribs.emplace_back(VertexBuffer::UV2KindChars);
    defines.emplace_back("#define UV2");
  }

  // Bones
  if (mesh->useBones() && mesh->computeBonesUsingShaders) {
    attribs.emplace_back(VertexBuffer::MatricesIndicesKindChars);
    attribs.emplace_back(VertexBuffer::MatricesWeightsKindChars);
    if (mesh->numBoneInfluencers > 4) {
      attribs.emplace_back(VertexBuffer::MatricesIndicesExtraKindChars);
      attribs.emplace_back(VertexBuffer::MatricesWeightsExtraKindChars);
    }
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
  auto join = String::join(defines, '\n');
  if (_cachedDefines != join) {
    _cachedDefines           = join;
    _glowMapGenerationEffect = _scene->getEngine()->createEffect(
      "glowMapGeneration", attribs,
      {"world", "mBones", "viewProjection", "diffuseMatrix", "color",
       "emissiveMatrix"},
      {"diffuseSampler", "emissiveSampler"}, join);
  }

  return _glowMapGenerationEffect->isReady();
}

void HighlightLayer::render()
{
  auto currentEffect = _glowMapMergeEffect;

  // Check
  if (!currentEffect->isReady() || !_blurTexture->isReady()) {
    return;
  }

  auto engine = _scene->getEngine();

  onBeforeComposeObservable.notifyObservers(this);

  // Render
  engine->enableEffect(currentEffect);
  engine->setState(false);

  // Cache
  auto previousStencilBuffer   = engine->getStencilBuffer();
  auto previousStencilFunction = engine->getStencilFunction();
  auto previousStencilMask     = engine->getStencilMask();
  auto previousAlphaMode       = engine->getAlphaMode();

  // Texture
  currentEffect->setTexture("textureSampler", _blurTexture.get());

  // VBOs
  std::unordered_map<std::string, VertexBuffer*> _vertexBuffersTmp;
  for (auto& item : _vertexBuffers) {
    _vertexBuffersTmp[item.first] = item.second.get();
  }
  engine->bindBuffers(_vertexBuffersTmp, _indexBuffer.get(), currentEffect);

  // Draw order
  engine->setAlphaMode(_options.alphaBlendingMode);
  engine->setStencilMask(0x00);
  engine->setStencilBuffer(true);
  engine->setStencilFunctionReference(_instanceGlowingMeshStencilReference);

  if (outerGlow) {
    currentEffect->setFloat("offset", 0.f);
    engine->setStencilFunction(Engine::NOTEQUAL);
    engine->draw(true, 0, 6);
  }
  if (innerGlow) {
    currentEffect->setFloat("offset", 1);
    engine->setStencilFunction(Engine::EQUAL);
    engine->draw(true, 0, 6);
  }

  // Restore Cache
  engine->setStencilFunction(previousStencilFunction);
  engine->setStencilMask(previousStencilMask);
  engine->setAlphaMode(previousAlphaMode);
  engine->setStencilBuffer(previousStencilBuffer);

  onAfterComposeObservable.notifyObservers(this);

  // Handle size changes.
  auto size = _mainTexture->getSize();
  setMainTextureSize();
  if (size.width != _mainTextureDesiredSize.width
      || size.height != _mainTextureDesiredSize.height) {
    // Recreate RTT and post processes on size change.
    onSizeChangedObservable.notifyObservers(this);
    disposeTextureAndPostProcesses();
    createTextureAndPostProcesses();
  }
}

void HighlightLayer::addExcludedMesh(Mesh* mesh)
{
  if (!std_util::contains(_excludedMeshes, mesh->uniqueId)) {
    IHighlightLayerExcludedMesh meshExcluded;
    meshExcluded.mesh         = mesh;
    meshExcluded.beforeRender = mesh->onBeforeRenderObservable.add(
      [](Mesh* mesh) { mesh->getEngine()->setStencilBuffer(false); });
    meshExcluded.afterRender = mesh->onAfterRenderObservable.add(
      [](Mesh* mesh) { mesh->getEngine()->setStencilBuffer(true); });
    _excludedMeshes[mesh->uniqueId] = meshExcluded;
  }
}

void HighlightLayer::removeExcludedMesh(Mesh* mesh)
{
  if (std_util::contains(_excludedMeshes, mesh->uniqueId)) {
    auto& meshExcluded = _excludedMeshes[mesh->uniqueId];
    mesh->onBeforeRenderObservable.remove(meshExcluded.beforeRender);
    mesh->onAfterRenderObservable.remove(meshExcluded.afterRender);
    _excludedMeshes.erase(mesh->uniqueId);
  }
}

void HighlightLayer::addMesh(Mesh* mesh, const Color3& color,
                             bool glowEmissiveOnly)
{
  if (std_util::contains(_meshes, mesh->uniqueId)) {
    _meshes[mesh->uniqueId].color = color;
  }
  else {
    IHighlightLayerMesh newMesh;
    newMesh.mesh  = mesh;
    newMesh.color = color;
    // Lambda required for capture due to Observable this context
    newMesh.observerHighlight
      = mesh->onBeforeRenderObservable.add([&](Mesh* mesh) {
          if (std_util::contains(_excludedMeshes, mesh->uniqueId)) {
            defaultStencilReference(mesh);
          }
          else {
            mesh->getScene()->getEngine()->setStencilFunctionReference(
              _instanceGlowingMeshStencilReference);
          }
        });
    newMesh.observerDefault = mesh->onAfterRenderObservable.add(
      [&](Mesh* mesh) { defaultStencilReference(mesh); });
    newMesh.glowEmissiveOnly = glowEmissiveOnly;
    _meshes[mesh->uniqueId]  = newMesh;
  }

  _shouldRender = true;
}

void HighlightLayer::removeMesh(Mesh* mesh)
{
  if (std_util::contains(_meshes, mesh->uniqueId)) {
    auto& meshHighlight = _meshes[mesh->uniqueId];
    mesh->onBeforeRenderObservable.remove(meshHighlight.observerHighlight);
    mesh->onAfterRenderObservable.remove(meshHighlight.observerDefault);
    _meshes.erase(mesh->uniqueId);
  }

  _shouldRender = _meshes.empty() ? false : true;
}

bool HighlightLayer::shouldRender() const
{
  return isEnabled && _shouldRender;
}

void HighlightLayer::setMainTextureSize()
{
  if (_options.mainTextureFixedSize) {
    _mainTextureDesiredSize.width  = _options.mainTextureFixedSize;
    _mainTextureDesiredSize.height = _options.mainTextureFixedSize;
  }
  else {
    _mainTextureDesiredSize.width = static_cast<int>(
      _engine->getRenderingCanvas()->width * _options.mainTextureRatio);
    _mainTextureDesiredSize.height = static_cast<int>(
      _engine->getRenderingCanvas()->height * _options.mainTextureRatio);

    _mainTextureDesiredSize.width
      = Tools::GetExponentOfTwo(_mainTextureDesiredSize.width, _maxSize);
    _mainTextureDesiredSize.height
      = Tools::GetExponentOfTwo(_mainTextureDesiredSize.height, _maxSize);
  }
}

void HighlightLayer::defaultStencilReference(Mesh* mesh)
{
  mesh->getScene()->getEngine()->setStencilFunctionReference(
    HighlightLayer::normalMeshStencilReference);
}

void HighlightLayer::disposeTextureAndPostProcesses()
{
  _blurTexture->dispose();
  _mainTexture->dispose();

  _downSamplePostprocess->dispose();
  _horizontalBlurPostprocess->dispose();
  _verticalBlurPostprocess->dispose();
}

void HighlightLayer::dispose()
{
  if (std_util::contains(_vertexBuffers, VertexBuffer::PositionKindChars)) {
    auto& vertexBuffer = _vertexBuffers[VertexBuffer::PositionKindChars];
    vertexBuffer->dispose();
    _vertexBuffers.erase(VertexBuffer::PositionKindChars);
  }

  if (_indexBuffer) {
    _scene->getEngine()->_releaseBuffer(_indexBuffer.get());
    _indexBuffer.reset(nullptr);
  }

  // Clean textures and post processes
  disposeTextureAndPostProcesses();

  // Clean mesh references
  for (auto item : _meshes) {
    auto& meshHighlight = item.second;
    if (meshHighlight.mesh) {
      meshHighlight.mesh->onBeforeRenderObservable.remove(
        meshHighlight.observerHighlight);
      meshHighlight.mesh->onAfterRenderObservable.remove(
        meshHighlight.observerDefault);
    }
  }
  _meshes.clear();
  for (auto item : _excludedMeshes) {
    auto& meshHighlight = item.second;
    meshHighlight.mesh->onBeforeRenderObservable.remove(
      meshHighlight.beforeRender);
    meshHighlight.mesh->onAfterRenderObservable.remove(
      meshHighlight.afterRender);
  }
  _excludedMeshes.clear();

  // Remove from scene
  _scene->highlightLayers.erase(
    std::remove_if(
      _scene->highlightLayers.begin(), _scene->highlightLayers.end(),
      [this](const std::unique_ptr<HighlightLayer>& highlightLayer) {
        return highlightLayer.get() == this;
      }),
    _scene->highlightLayers.end());

  // Callback
  onDisposeObservable.notifyObservers(this);

  onDisposeObservable.clear();
  onBeforeRenderMainTextureObservable.clear();
  onBeforeBlurObservable.clear();
  onBeforeComposeObservable.clear();
  onAfterComposeObservable.clear();
  onSizeChangedObservable.clear();
}

} // end of namespace BABYLON
