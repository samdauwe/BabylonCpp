#include <babylon/rendering/bounding_box_renderer.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/culling/bounding_box.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/ishader_material_options.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

BoundingBoxRenderer::BoundingBoxRenderer(Scene* iScene)
    : frontColor{Color3(1.f, 1.f, 1.f)}
    , backColor{Color3(0.1f, 0.1f, 0.1f)}
    , showBackLines{true}
    , enabled{true}
    , _colorShader{nullptr}
    , _indexBuffer{nullptr}
    , _fillIndexBuffer{nullptr}
{
  scene = iScene;
  renderList.reserve(32);
}

BoundingBoxRenderer::~BoundingBoxRenderer() = default;

void BoundingBoxRenderer::addToScene(const BoundingBoxRendererPtr& newBoundingBoxRenderer)
{
  ISceneComponent::name = BoundingBoxRenderer::name;
  scene->_addComponent(newBoundingBoxRenderer);
}

void BoundingBoxRenderer::_register()
{
  scene->_beforeEvaluateActiveMeshStage.registerStep(
    SceneComponentConstants::STEP_BEFOREEVALUATEACTIVEMESH_BOUNDINGBOXRENDERER, this,
    [this]() -> void { reset(); });

  scene->_preActiveMeshStage.registerStep(
    SceneComponentConstants::STEP_PREACTIVEMESH_BOUNDINGBOXRENDERER, this,
    [this](AbstractMesh* mesh) -> void { _preActiveMesh(mesh); });

  scene->_evaluateSubMeshStage.registerStep(
    SceneComponentConstants::STEP_EVALUATESUBMESH_BOUNDINGBOXRENDERER, this,
    [this](AbstractMesh* mesh, SubMesh* subMesh) -> void { _evaluateSubMesh(mesh, subMesh); });

  scene->_afterRenderingGroupDrawStage.registerStep(
    SceneComponentConstants::STEP_AFTERRENDERINGGROUPDRAW_BOUNDINGBOXRENDERER, this,
    [this](int renderingGroupId) -> void { render(renderingGroupId); });
}

void BoundingBoxRenderer::_evaluateSubMesh(AbstractMesh* mesh, SubMesh* subMesh)
{
  if (mesh->showSubMeshesBoundingBox) {
    auto& boundingInfo            = *subMesh->getBoundingInfo();
    boundingInfo.boundingBox._tag = mesh->renderingGroupId;
    renderList.emplace_back(boundingInfo.boundingBox);
  }
}

void BoundingBoxRenderer::_preActiveMesh(AbstractMesh* mesh)
{
  if (mesh->showBoundingBox || scene->forceShowBoundingBoxes) {
    auto& boundingInfo            = *mesh->getBoundingInfo();
    boundingInfo.boundingBox._tag = mesh->renderingGroupId;
    renderList.emplace_back(boundingInfo.boundingBox);
  }
}

void BoundingBoxRenderer::_prepareResources()
{
  if (_colorShader) {
    return;
  }

  IShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes = {VertexBuffer::PositionKind};
  shaderMaterialOptions.uniforms   = {"world", "viewProjection", "color"};

  _colorShader = ShaderMaterial::New("colorShader", scene, "color", shaderMaterialOptions);

  auto engine = scene->getEngine();
  BoxOptions options;
  options.size = 1.f;
  auto boxdata = VertexData::CreateBox(options);
  _vertexBuffers[VertexBuffer::PositionKind]
    = std::make_shared<VertexBuffer>(engine, boxdata->positions, VertexBuffer::PositionKind, false);
  _createIndexBuffer();
  _fillIndexData = boxdata->indices;
  onResourcesReadyObservable.notifyObservers(this);
}

void BoundingBoxRenderer::_createIndexBuffer()
{
  auto engine = scene->getEngine();
  const Uint32Array indices{0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 7, 1, 6, 2, 5, 3, 4};
  _indexBuffer = engine->createIndexBuffer(indices);
}

void BoundingBoxRenderer::rebuild()
{
  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& vb = _vertexBuffers[VertexBuffer::PositionKind];
    if (vb) {
      vb->_rebuild();
    }
  }
  _createIndexBuffer();
}

void BoundingBoxRenderer::reset()
{
  renderList.clear();
}

void BoundingBoxRenderer::render(int renderingGroupId)
{
  if (renderList.empty() || !enabled) {
    return;
  }

  _prepareResources();

  if (!_colorShader->isReady()) {
    return;
  }

  auto engine = scene->getEngine();
  engine->setDepthWrite(false);
  _colorShader->_preBind(EffectPtr{nullptr});

  for (auto& boundingBox : renderList) {
    if (boundingBox._tag != renderingGroupId) {
      continue;
    }

    onBeforeBoxRenderingObservable.notifyObservers(&boundingBox);

    const auto& min = boundingBox.minimum;
    const auto& max = boundingBox.maximum;
    auto diff       = max.subtract(min);
    auto median     = min.add(diff.scale(0.5f));

    auto worldMatrix = Matrix::Scaling(diff.x, diff.y, diff.z)
                         .multiply(Matrix::Translation(median.x, median.y, median.z))
                         .multiply(boundingBox.getWorldMatrix());

    // VBOs
    engine->bindBuffers(_vertexBuffers, _indexBuffer, _colorShader->getEffect());

    if (showBackLines) {
      // Back
      engine->setDepthFunctionToGreaterOrEqual();
      scene->resetCachedMaterial();
      _colorShader->setColor4("color", backColor.toColor4());
      _colorShader->bind(worldMatrix, nullptr);

      // Draw order
      engine->drawElementsType(Material::LineListDrawMode, 0, 24);
    }

    // Front
    engine->setDepthFunctionToLess();
    scene->resetCachedMaterial();
    _colorShader->setColor4("color", frontColor.toColor4());
    _colorShader->bind(worldMatrix);

    // Draw order
    engine->drawElementsType(Material::LineListDrawMode, 0, 24);

    onAfterBoxRenderingObservable.notifyObservers(&boundingBox);
  }
  _colorShader->unbind();
  engine->setDepthFunctionToLessOrEqual();
  engine->setDepthWrite(true);
}

void BoundingBoxRenderer::renderOcclusionBoundingBox(AbstractMesh* mesh)
{
  _prepareResources();

  if (!_colorShader->isReady() || !mesh->_boundingInfo) {
    return;
  }

  auto engine = scene->getEngine();
  if (!_fillIndexBuffer) {
    _fillIndexBuffer = engine->createIndexBuffer(_fillIndexData);
  }

  engine->setDepthWrite(false);
  engine->setColorWrite(false);
  _colorShader->_preBind(EffectPtr{nullptr});

  auto& boundingBox = mesh->_boundingInfo->boundingBox;
  auto min          = boundingBox.minimum;
  auto max          = boundingBox.maximum;
  auto diff         = max.subtract(min);
  auto median       = min.add(diff.scale(0.5f));

  auto worldMatrix = Matrix::Scaling(diff.x, diff.y, diff.z)
                       .multiply(Matrix::Translation(median.x, median.y, median.z))
                       .multiply(boundingBox.getWorldMatrix());

  engine->bindBuffers(_vertexBuffers, _fillIndexBuffer, _colorShader->getEffect());

  engine->setDepthFunctionToLess();
  scene->resetCachedMaterial();
  _colorShader->bind(worldMatrix);

  engine->drawElementsType(Material::TriangleFillMode, 0, 36);

  _colorShader->unbind();
  engine->setDepthFunctionToLessOrEqual();
  engine->setDepthWrite(true);
  engine->setColorWrite(true);
}

void BoundingBoxRenderer::dispose()
{
  if (!_colorShader) {
    return;
  }

  onBeforeBoxRenderingObservable.clear();
  onAfterBoxRenderingObservable.clear();
  onResourcesReadyObservable.clear();

  renderList.clear();

  _colorShader->dispose();

  if (stl_util::contains(_vertexBuffers, VertexBuffer::PositionKind)) {
    auto& buffer = _vertexBuffers[VertexBuffer::PositionKind];
    if (buffer) {
      buffer->dispose();
      buffer = nullptr;
      _vertexBuffers.erase(VertexBuffer::PositionKind);
    }
  }

  scene->getEngine()->_releaseBuffer(_indexBuffer);

  if (_fillIndexBuffer) {
    scene->getEngine()->_releaseBuffer(_fillIndexBuffer);
    _fillIndexBuffer = nullptr;
  }
}

} // end of namespace BABYLON
