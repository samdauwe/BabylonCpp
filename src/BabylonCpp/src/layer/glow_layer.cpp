#include <babylon/layer/glow_layer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/postprocess/blur_post_process.h>
#include <babylon/postprocess/post_process_manager.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

GlowLayer::GlowLayer(const string_t& name, Scene* scene)
    : GlowLayer(name, scene,
                IGlowLayerOptions{
                  GlowLayer::DefaultTextureRatio, // mainTextureRatio;
                  nullptr,                        // mainTextureFixedSize
                  32,                             // blurKernelSize
                  nullptr,                        // camera
                  1                               // mainTextureSamples
                })
{
}

GlowLayer::GlowLayer(const string_t& iName, Scene* scene,
                     const IGlowLayerOptions& options)
    : EffectLayer{iName, scene}
    , customEmissiveColorSelector{nullptr}
    , customEmissiveTextureSelector{nullptr}
    , blurKernelSize{this, &GlowLayer::get_blurKernelSize,
                     &GlowLayer::set_blurKernelSize}
    , intensity{this, &GlowLayer::get_intensity, &GlowLayer::set_intensity}
    , _intensity{1.f}
    , _horizontalBlurPostprocess1{nullptr}
    , _verticalBlurPostprocess1{nullptr}
    , _horizontalBlurPostprocess2{nullptr}
    , _verticalBlurPostprocess2{nullptr}
    , _blurTexture1{nullptr}
    , _blurTexture2{nullptr}
{
  neutralColor = Color4(0.f, 0.f, 0.f, 1.f);

  // Adapt options
  _options.mainTextureRatio     = options.mainTextureRatio;
  _options.blurKernelSize       = options.blurKernelSize;
  _options.mainTextureFixedSize = options.mainTextureFixedSize;
  _options.camera               = options.camera;
  _options.mainTextureSamples   = options.mainTextureSamples;

  // Initialize the layer
  IEffectLayerOptions effectLayerOptions;
  effectLayerOptions.alphaBlendingMode    = EngineConstants::ALPHA_ADD;
  effectLayerOptions.camera               = _options.camera;
  effectLayerOptions.mainTextureFixedSize = _options.mainTextureFixedSize;
  effectLayerOptions.mainTextureRatio     = _options.mainTextureRatio;
  _init(effectLayerOptions);
}

GlowLayer::~GlowLayer()
{
}

void GlowLayer::set_blurKernelSize(float value)
{
  _horizontalBlurPostprocess1->kernel = value;
  _verticalBlurPostprocess1->kernel   = value;
  _horizontalBlurPostprocess2->kernel = value;
  _verticalBlurPostprocess2->kernel   = value;
}

float GlowLayer::get_blurKernelSize() const
{
  return _horizontalBlurPostprocess1->kernel();
}

void GlowLayer::set_intensity(float value)
{
  _intensity = value;
}

float GlowLayer::get_intensity() const
{
  return _intensity;
}

string_t GlowLayer::getEffectName() const
{
  return GlowLayer::EffectName;
}

Effect* GlowLayer::_createMergeEffect()
{
  // Effect
  EffectCreationOptions effectCreationOptions;
  effectCreationOptions.attributes          = {VertexBuffer::PositionKindChars};
  effectCreationOptions.uniformBuffersNames = {"offset"};
  effectCreationOptions.samplers = {"textureSampler", "textureSampler2"};
  effectCreationOptions.defines  = "#define EMISSIVE \n";

  return _engine->createEffect("glowMapMerge", effectCreationOptions,
                               _scene->getEngine());
}

void GlowLayer::_createTextureAndPostProcesses()
{
  auto blurTextureWidth  = _mainTextureDesiredSize.width;
  auto blurTextureHeight = _mainTextureDesiredSize.height;
  blurTextureWidth       = _engine->needPOTTextures() ?
                       Tools::GetExponentOfTwo(blurTextureWidth, _maxSize) :
                       blurTextureWidth;
  blurTextureHeight = _engine->needPOTTextures() ?
                        Tools::GetExponentOfTwo(blurTextureHeight, _maxSize) :
                        blurTextureHeight;

  _blurTexture1 = ::std::make_unique<RenderTargetTexture>(
    "GlowLayerBlurRTT", ISize{blurTextureWidth, blurTextureHeight}, _scene,
    false, true, EngineConstants::TEXTURETYPE_HALF_FLOAT);
  _blurTexture1->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture1->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture1->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _blurTexture1->renderParticles      = false;
  _blurTexture1->ignoreCameraViewport = true;

  int blurTextureWidth2 = static_cast<int>(::std::floor(blurTextureWidth / 2));
  int blurTextureHeight2
    = static_cast<int>(::std::floor(blurTextureHeight / 2));

  _blurTexture2 = ::std::make_unique<RenderTargetTexture>(
    "GlowLayerBlurRTT2", ISize{blurTextureWidth2, blurTextureHeight2}, _scene,
    false, true, EngineConstants::TEXTURETYPE_HALF_FLOAT);
  _blurTexture2->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture2->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture2->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _blurTexture2->renderParticles      = false;
  _blurTexture2->ignoreCameraViewport = true;

  _textures = {_blurTexture1.get(), _blurTexture2.get()};

  _horizontalBlurPostprocess1 = ::std::make_unique<BlurPostProcess>(
    "GlowLayerHBP1", Vector2(1.f, 0.f), _options.blurKernelSize / 2,
    ToVariant<float, PostProcessOptions>(
      PostProcessOptions{blurTextureWidth, blurTextureHeight}),
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false, EngineConstants::TEXTURETYPE_HALF_FLOAT);
  _horizontalBlurPostprocess1->width  = blurTextureWidth;
  _horizontalBlurPostprocess1->height = blurTextureHeight;
  _horizontalBlurPostprocess1->onApplyObservable.add(
    [this](Effect* effect, EventState /*es*/) {
      effect->setTexture("textureSampler", _mainTexture.get());
    });

  _verticalBlurPostprocess1 = ::std::make_unique<BlurPostProcess>(
    "GlowLayerVBP1", Vector2(0.f, 1.f), _options.blurKernelSize / 2,
    ToVariant<float, PostProcessOptions>(
      PostProcessOptions{blurTextureWidth, blurTextureHeight}),
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false, EngineConstants::TEXTURETYPE_HALF_FLOAT);

  _horizontalBlurPostprocess2 = ::std::make_unique<BlurPostProcess>(
    "GlowLayerHBP2", Vector2(1.f, 0.f), _options.blurKernelSize / 2,
    ToVariant<float, PostProcessOptions>(
      PostProcessOptions{blurTextureWidth2, blurTextureHeight2}),
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false, EngineConstants::TEXTURETYPE_HALF_FLOAT);
  _horizontalBlurPostprocess2->width  = blurTextureWidth2;
  _horizontalBlurPostprocess2->height = blurTextureHeight2;
  _horizontalBlurPostprocess2->onApplyObservable.add(
    [this](Effect* effect, EventState /*es*/) {
      effect->setTexture("textureSampler", _blurTexture1.get());
    });

  _verticalBlurPostprocess2 = ::std::make_unique<BlurPostProcess>(
    "GlowLayerVBP2", Vector2(0.f, 1.f), _options.blurKernelSize / 2,
    ToVariant<float, PostProcessOptions>(
      PostProcessOptions{blurTextureWidth2, blurTextureHeight2}),
    nullptr, TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(),
    false, EngineConstants::TEXTURETYPE_HALF_FLOAT);

  _postProcesses
    = {_horizontalBlurPostprocess1.get(), _verticalBlurPostprocess1.get(),
       _horizontalBlurPostprocess2.get(), _verticalBlurPostprocess2.get()};
  _postProcesses1
    = {_horizontalBlurPostprocess1.get(), _verticalBlurPostprocess1.get()};
  _postProcesses2
    = {_horizontalBlurPostprocess2.get(), _verticalBlurPostprocess2.get()};

  _mainTexture->samples = _options.mainTextureSamples;
  _mainTexture->onAfterUnbindObservable.add(
    [this](RenderTargetTexture* /*renderTargetTexture*/, EventState& /*es*/) {
      auto internalTexture = _blurTexture1->getInternalTexture();
      if (internalTexture) {
        _scene->postProcessManager->directRender(_postProcesses1,
                                                 internalTexture, true);

        internalTexture = _blurTexture2->getInternalTexture();
        if (internalTexture) {
          _scene->postProcessManager->directRender(_postProcesses2,
                                                   internalTexture, true);
        }
      }
    });

  // Prevent autoClear.
  for (auto& pp : _postProcesses) {
    pp->autoClear = false;
  }
}

bool GlowLayer::isReady(SubMesh* subMesh, bool useInstances)
{
  auto material = subMesh->getMaterial();
  auto mesh     = subMesh->getRenderingMesh();

  if (!material || !mesh) {
    return false;
  }

  BaseTexture* emissiveTexture = nullptr;
  if (material->type() == IReflect::Type::STANDARDMATERIAL) {
    emissiveTexture
      = static_cast<StandardMaterial*>(material)->emissiveTexture();
  }

  return EffectLayer::_isReady(subMesh, useInstances, emissiveTexture);
}

bool GlowLayer::needStencil() const
{
  return false;
}

void GlowLayer::_internalRender(Effect* effect)
{
  // Texture
  effect->setTexture("textureSampler", _blurTexture1.get());
  effect->setTexture("textureSampler2", _blurTexture2.get());
  effect->setFloat("offset", _intensity);

  // Cache
  auto engine                = _engine;
  auto previousStencilBuffer = engine->getStencilBuffer();

  // Draw order
  engine->setStencilBuffer(false);

  engine->drawElementsType(Material::TriangleFillMode(), 0, 6);

  // Draw order
  engine->setStencilBuffer(previousStencilBuffer);
}

void GlowLayer::_setEmissiveTextureAndColor(Mesh* mesh, SubMesh* subMesh,
                                            Material* iMaterial)
{
  StandardMaterial* material = nullptr;
  if (iMaterial->type() == IReflect::Type::STANDARDMATERIAL) {
    material = static_cast<StandardMaterial*>(iMaterial);
  }

  auto textureLevel = 1.f;

  if (customEmissiveTextureSelector) {
    _emissiveTextureAndColor.texture
      = customEmissiveTextureSelector(mesh, subMesh, material);
  }
  else {
    if (material) {
      _emissiveTextureAndColor.texture = material->emissiveTexture();
      if (_emissiveTextureAndColor.texture) {
        textureLevel = _emissiveTextureAndColor.texture->level;
      }
    }
    else {
      _emissiveTextureAndColor.texture = nullptr;
    }
  }

  if (customEmissiveColorSelector) {
    customEmissiveColorSelector(mesh, subMesh, material,
                                _emissiveTextureAndColor.color);
  }
  else {
    if (material) {
      _emissiveTextureAndColor.color.set(
        material->emissiveColor.r * textureLevel, //
        material->emissiveColor.g * textureLevel, //
        material->emissiveColor.b * textureLevel, //
        1.f);
    }
    else {
      _emissiveTextureAndColor.color.set(neutralColor.r, //
                                         neutralColor.g, //
                                         neutralColor.b, //
                                         neutralColor.a);
    }
  }
}

bool GlowLayer::_shouldRenderMesh(Mesh* mesh) const
{
  return hasMesh(mesh);
}

void GlowLayer::addExcludedMesh(Mesh* mesh)
{
  if (!stl_util::contains(_excludedMeshes, mesh->uniqueId)) {
    _excludedMeshes.emplace_back(mesh->uniqueId);
  }
}

void GlowLayer::removeExcludedMesh(Mesh* mesh)
{
  _excludedMeshes.erase(::std::remove(_excludedMeshes.begin(),
                                      _excludedMeshes.end(), mesh->uniqueId),
                        _excludedMeshes.end());
}

void GlowLayer::addIncludedOnlyMesh(Mesh* mesh)
{
  if (!stl_util::contains(_includedOnlyMeshes, mesh->uniqueId)) {
    _includedOnlyMeshes.emplace_back(mesh->uniqueId);
  }
}

void GlowLayer::removeIncludedOnlyMesh(Mesh* mesh)
{
  _includedOnlyMeshes.erase(::std::remove(_includedOnlyMeshes.begin(),
                                          _includedOnlyMeshes.end(),
                                          mesh->uniqueId),
                            _includedOnlyMeshes.end());
}

bool GlowLayer::hasMesh(AbstractMesh* mesh) const
{
  // Included Mesh
  if (!_includedOnlyMeshes.empty()) {
    return stl_util::contains(_includedOnlyMeshes, mesh->uniqueId);
  }

  // Excluded Mesh
  if (!_excludedMeshes.empty()) {
    return stl_util::contains(_excludedMeshes, mesh->uniqueId);
  }

  return true;
}

void GlowLayer::_disposeMesh(Mesh* mesh)
{
  removeIncludedOnlyMesh(mesh);
  removeExcludedMesh(mesh);
}

} // end of namespace BABYLON
