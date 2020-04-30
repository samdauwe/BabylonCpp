#include <babylon/layers/glow_layer.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ieffect_creation_options.h>
#include <babylon/materials/material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/postprocesses/blur_post_process.h>
#include <babylon/postprocesses/post_process_manager.h>

namespace BABYLON {

GlowLayer::GlowLayer(const std::string& iName, Scene* scene)
    : GlowLayer(iName, scene,
                IGlowLayerOptions{
                  GlowLayer::DefaultTextureRatio, // mainTextureRatio;
                  std::nullopt,                   // mainTextureFixedSize
                  32,                             // blurKernelSize
                  nullptr,                        // camera
                  1,                              // mainTextureSamples
                  -1                              // renderingGroupId
                })
{
}

GlowLayer::GlowLayer(const std::string& iName, Scene* scene, const IGlowLayerOptions& options)
    : EffectLayer{iName, scene}
    , customEmissiveColorSelector{nullptr}
    , customEmissiveTextureSelector{nullptr}
    , blurKernelSize{this, &GlowLayer::get_blurKernelSize, &GlowLayer::set_blurKernelSize}
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
  effectLayerOptions.alphaBlendingMode    = Constants::ALPHA_ADD;
  effectLayerOptions.camera               = _options.camera;
  effectLayerOptions.mainTextureFixedSize = _options.mainTextureFixedSize;
  effectLayerOptions.mainTextureRatio     = _options.mainTextureRatio;
  effectLayerOptions.renderingGroupId     = _options.renderingGroupId;
  _init(effectLayerOptions);
}

GlowLayer::~GlowLayer() = default;

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

std::string GlowLayer::getEffectName() const
{
  return GlowLayer::EffectName;
}

EffectPtr GlowLayer::_createMergeEffect()
{
  // Effect
  IEffectCreationOptions effectCreationOptions;
  effectCreationOptions.attributes          = {VertexBuffer::PositionKind};
  effectCreationOptions.uniformBuffersNames = {"offset"};
  effectCreationOptions.samplers            = {"textureSampler", "textureSampler2"};
  effectCreationOptions.defines             = "#define EMISSIVE \n";

  return _engine->createEffect("glowMapMerge", effectCreationOptions, _scene->getEngine());
}

void GlowLayer::_createTextureAndPostProcesses()
{
  auto blurTextureWidth  = _mainTextureDesiredSize.width;
  auto blurTextureHeight = _mainTextureDesiredSize.height;
  blurTextureWidth       = _engine->needPOTTextures() ?
                       Engine::GetExponentOfTwo(blurTextureWidth, _maxSize) :
                       blurTextureWidth;
  blurTextureHeight = _engine->needPOTTextures() ?
                        Engine::GetExponentOfTwo(blurTextureHeight, _maxSize) :
                        blurTextureHeight;

  auto textureType = 0u;
  if (_engine->getCaps().textureHalfFloatRender) {
    textureType = Constants::TEXTURETYPE_HALF_FLOAT;
  }
  else {
    textureType = Constants::TEXTURETYPE_UNSIGNED_INT;
  }

  _blurTexture1        = RenderTargetTexture::New("GlowLayerBlurRTT",
                                           RenderTargetSize{blurTextureWidth, blurTextureHeight},
                                           _scene, false, true, textureType);
  _blurTexture1->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture1->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture1->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _blurTexture1->renderParticles      = false;
  _blurTexture1->ignoreCameraViewport = true;

  auto blurTextureWidth2  = static_cast<int>(std::floor(blurTextureWidth / 2));
  auto blurTextureHeight2 = static_cast<int>(std::floor(blurTextureHeight / 2));

  _blurTexture2        = RenderTargetTexture::New("GlowLayerBlurRTT2",
                                           RenderTargetSize{blurTextureWidth2, blurTextureHeight2},
                                           _scene, false, true, textureType);
  _blurTexture2->wrapU = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture2->wrapV = TextureConstants::CLAMP_ADDRESSMODE;
  _blurTexture2->updateSamplingMode(TextureConstants::BILINEAR_SAMPLINGMODE);
  _blurTexture2->renderParticles      = false;
  _blurTexture2->ignoreCameraViewport = true;

  _textures = {_blurTexture1, _blurTexture2};

  _horizontalBlurPostprocess1 = BlurPostProcess::New(
    "GlowLayerHBP1", Vector2(1.f, 0.f), _options.blurKernelSize / 2.f,
    PostProcessOptions{blurTextureWidth, blurTextureHeight}, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false, textureType);
  _horizontalBlurPostprocess1->width  = blurTextureWidth;
  _horizontalBlurPostprocess1->height = blurTextureHeight;
  _horizontalBlurPostprocess1->onApplyObservable.add([this](Effect* effect, EventState /*es*/) {
    effect->setTexture("textureSampler", _mainTexture);
  });

  _verticalBlurPostprocess1 = BlurPostProcess::New(
    "GlowLayerVBP1", Vector2(0.f, 1.f), _options.blurKernelSize / 2.f,
    PostProcessOptions{blurTextureWidth, blurTextureHeight}, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false, textureType);

  _horizontalBlurPostprocess2 = BlurPostProcess::New(
    "GlowLayerHBP2", Vector2(1.f, 0.f), _options.blurKernelSize / 2.f,
    PostProcessOptions{blurTextureWidth2, blurTextureHeight2}, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false, textureType);
  _horizontalBlurPostprocess2->width  = blurTextureWidth2;
  _horizontalBlurPostprocess2->height = blurTextureHeight2;
  _horizontalBlurPostprocess2->onApplyObservable.add([this](Effect* effect, EventState /*es*/) {
    effect->setTexture("textureSampler", _blurTexture1);
  });

  _verticalBlurPostprocess2 = BlurPostProcess::New(
    "GlowLayerVBP2", Vector2(0.f, 1.f), _options.blurKernelSize / 2.f,
    PostProcessOptions{blurTextureWidth2, blurTextureHeight2}, nullptr,
    TextureConstants::BILINEAR_SAMPLINGMODE, _scene->getEngine(), false, textureType);

  _postProcesses  = {_horizontalBlurPostprocess1, _verticalBlurPostprocess1,
                    _horizontalBlurPostprocess2, _verticalBlurPostprocess2};
  _postProcesses1 = {_horizontalBlurPostprocess1, _verticalBlurPostprocess1};
  _postProcesses2 = {_horizontalBlurPostprocess2, _verticalBlurPostprocess2};

  _mainTexture->samples = _options.mainTextureSamples.has_value() ?
                            (*_options.mainTextureSamples) >= 0 ?
                            static_cast<unsigned int>(*_options.mainTextureSamples) :
                            0 :
                            0;
  _mainTexture->onAfterUnbindObservable.add(
    [this](RenderTargetTexture* /*renderTargetTexture*/, EventState& /*es*/) {
      auto internalTexture = _blurTexture1->getInternalTexture();
      if (internalTexture) {
        _scene->postProcessManager->directRender(_postProcesses1, internalTexture, true);

        internalTexture = _blurTexture2->getInternalTexture();
        if (internalTexture) {
          _scene->postProcessManager->directRender(_postProcesses2, internalTexture, true);
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

  BaseTexturePtr emissiveTexture = nullptr;
  if (material->type() == Type::STANDARDMATERIAL) {
    emissiveTexture = std::static_pointer_cast<StandardMaterial>(material)->emissiveTexture();
  }

  return EffectLayer::_isReady(subMesh, useInstances, emissiveTexture);
}

bool GlowLayer::needStencil() const
{
  return false;
}

bool GlowLayer::_canRenderMesh(const AbstractMeshPtr& /*mesh*/,
                               const MaterialPtr& /*material*/) const
{
  return true;
}

void GlowLayer::_internalRender(const EffectPtr& effect)
{
  // Texture
  effect->setTexture("textureSampler", _blurTexture1);
  effect->setTexture("textureSampler2", _blurTexture2);
  effect->setFloat("offset", _intensity);

  // Cache
  auto engine                = _engine;
  auto previousStencilBuffer = engine->getStencilBuffer();

  // Draw order
  engine->setStencilBuffer(false);

  engine->drawElementsType(Material::TriangleFillMode, 0, 6);

  // Draw order
  engine->setStencilBuffer(previousStencilBuffer);
}

void GlowLayer::_setEmissiveTextureAndColor(const MeshPtr& mesh, SubMesh* subMesh,
                                            const MaterialPtr& iMaterial)
{
  StandardMaterialPtr material = nullptr;
  if (iMaterial->type() == Type::STANDARDMATERIAL) {
    material = std::static_pointer_cast<StandardMaterial>(iMaterial);
  }

  auto textureLevel = 1.f;

  if (customEmissiveTextureSelector) {
    _emissiveTextureAndColor.texture = customEmissiveTextureSelector(mesh, subMesh, material);
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
    customEmissiveColorSelector(mesh, subMesh, material.get(), _emissiveTextureAndColor.color);
  }
  else {
    if (material) {
      _emissiveTextureAndColor.color.set(material->emissiveColor.r * textureLevel, //
                                         material->emissiveColor.g * textureLevel, //
                                         material->emissiveColor.b * textureLevel, //
                                         material->alpha());
    }
    else {
      _emissiveTextureAndColor.color.set(neutralColor.r, //
                                         neutralColor.g, //
                                         neutralColor.b, //
                                         neutralColor.a);
    }
  }
}

bool GlowLayer::_shouldRenderMesh(AbstractMesh* mesh) const
{
  return hasMesh(mesh);
}

void GlowLayer::_addCustomEffectDefines(std::vector<std::string>& defines)
{
  defines.emplace_back("#define GLOW");
}

void GlowLayer::addExcludedMesh(Mesh* mesh)
{
  if (!stl_util::contains(_excludedMeshes, mesh->uniqueId)) {
    _excludedMeshes.emplace_back(mesh->uniqueId);
  }
}

void GlowLayer::removeExcludedMesh(Mesh* mesh)
{
  _excludedMeshes.erase(std::remove(_excludedMeshes.begin(), _excludedMeshes.end(), mesh->uniqueId),
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
  _includedOnlyMeshes.erase(
    std::remove(_includedOnlyMeshes.begin(), _includedOnlyMeshes.end(), mesh->uniqueId),
    _includedOnlyMeshes.end());
}

bool GlowLayer::hasMesh(AbstractMesh* mesh) const
{
  if (!EffectLayer::hasMesh(mesh)) {
    return false;
  }

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

bool GlowLayer::_useMeshMaterial(const AbstractMeshPtr& mesh) const
{
  if (_meshesUsingTheirOwnMaterials.empty()) {
    return false;
  }
  return stl_util::contains(_meshesUsingTheirOwnMaterials, mesh->uniqueId);
}

void GlowLayer::referenceMeshToUseItsOwnMaterial(const AbstractMeshPtr& mesh)
{
  _meshesUsingTheirOwnMaterials.emplace_back(mesh->uniqueId);
}

void GlowLayer::unReferenceMeshFromUsingItsOwnMaterial(const AbstractMeshPtr& mesh)
{
  auto index = stl_util::index_of(_meshesUsingTheirOwnMaterials, mesh->uniqueId);
  while (index >= 0) {
    stl_util::slice_in_place(_meshesUsingTheirOwnMaterials, index, index + 1);
    index = stl_util::index_of(_meshesUsingTheirOwnMaterials, mesh->uniqueId);
  }
}

void GlowLayer::_disposeMesh(Mesh* mesh)
{
  removeIncludedOnlyMesh(mesh);
  removeExcludedMesh(mesh);
}

std::string GlowLayer::getClassName() const
{
  return "GlowLayer";
}

json GlowLayer::serialize() const
{
  return nullptr;
}

GlowLayer* GlowLayer::Parse(const json& /*parsedGlowLayer*/, Scene* /*scene*/,
                            const std::string& /*rootUrl*/)
{
  return nullptr;
}

} // end of namespace BABYLON
